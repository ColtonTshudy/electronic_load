/**
 * @file  app_hw.c
 * @brief Low-level hardware: GPIO, internal ADC/DAC, timers, UARTs, IWDG,
 *        ADS1115 (bit-banged I2C) and DAC8562 (bit-banged SPI).
 *
 * Peripherals without a HAL driver in this project are driven at register
 * level; the configuration mirrors the original firmware.
 */
#include "app.h"
#include "stm32f1xx_hal.h"

#define PCLK_HZ  24000000u

/* 0..15 as the original firmware; 16..19 added (USART max = PCLK/16 = 1.5 M) */
static const uint32_t s_baud_table[BAUD_TABLE_N] = {
    600, 1200, 2400, 4800, 7200, 9600, 14400, 19200,
    38400, 43000, 56000, 57600, 76800, 115200, 128000, 256000,
    460800, 921600, 1000000, 1500000,
};

uint32_t baud_from_index(uint32_t idx)
{
    return s_baud_table[(idx < BAUD_TABLE_N) ? idx : MB_BAUD_DEFAULT_IDX];
}

/* ADS1115 config words: OS=1, MODE=single, DR=860 SPS, comparator off */
static const uint16_t s_ads_cfg[4] = { 0x95E3, 0xA5E3, 0xB5E3, 0xB7E3 };

/* ------------------------------------------------------------------------- */
/* Timing                                                                     */
/* ------------------------------------------------------------------------- */
void delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000u);
    while ((DWT->CYCCNT - start) < ticks) { }
}

/* ------------------------------------------------------------------------- */
/* GPIO                                                                       */
/* ------------------------------------------------------------------------- */
static void gpio_cfg(GPIO_TypeDef *port, uint32_t pins, uint32_t mode,
                     uint32_t pull, uint32_t speed)
{
    GPIO_InitTypeDef gi = { .Pin = pins, .Mode = mode, .Pull = pull, .Speed = speed };
    HAL_GPIO_Init(port, &gi);
}

volatile uint8_t g_vrange_hi;

void vrange_set_high(bool high)
{
    g_vrange_hi = high ? 1u : 0u;
    if (high) {
        pin_reset(PIN_VRANGE_PORT, PIN_VRANGE);
        gpio_cfg(PIN_VRANGE_PORT, PIN_VRANGE, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH);
        pin_reset(PIN_VRANGE_PORT, PIN_VRANGE);
    } else {
        gpio_cfg(PIN_VRANGE_PORT, PIN_VRANGE, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
    }
}

static void gpio_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_AFIO_CLK_ENABLE();

    /* PA15/PB3/PB4 are JTAG pins by default: drop JTAG, keep SWD, and route
     * TIM2 (full remap: CH2 -> PB3). Single write on purpose: SWJ_CFG is
     * write-only and HAL's __HAL_AFIO_REMAP_xxx() macros write it as 0b111,
     * which disables SWD on this part. Never use those macros after this. */
    AFIO->MAPR = (AFIO->MAPR & ~(AFIO_MAPR_SWJ_CFG_Msk | AFIO_MAPR_TIM2_REMAP_Msk))
               | AFIO_MAPR_SWJ_CFG_JTAGDISABLE | AFIO_MAPR_TIM2_REMAP_FULLREMAP;

    /* Safe output levels before the pins become outputs */
    pin_set(PIN_LOAD_INH_PORT, PIN_LOAD_INH);                    /* load off */
    pin_reset(GPIOB, PIN_AUX_OUT | PIN_FAN | PIN_BUZ | PIN_IRANGE);
    pin_reset(PIN_RS485_DE_PORT, PIN_RS485_DE);                  /* receive  */
    pin_set(PIN_ADS_PORT, PIN_ADS_SDA | PIN_ADS_SCL);
    pin_set(PIN_X_DAC_PORT, PIN_X_DAC_SYNC | PIN_X_DAC_SCLK);

    gpio_cfg(PIN_LOAD_INH_PORT, PIN_LOAD_INH, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH);
    gpio_cfg(GPIOB, PIN_AUX_OUT | PIN_FAN | PIN_BUZ | PIN_IRANGE,
             GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH);
    gpio_cfg(PIN_RS485_DE_PORT, PIN_RS485_DE, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH);
    gpio_cfg(PIN_ADS_PORT, PIN_ADS_SDA | PIN_ADS_SCL, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH);
    gpio_cfg(PIN_X_DAC_PORT, PIN_X_DAC_SCLK | PIN_X_DAC_DIN | PIN_X_DAC_SYNC,
             GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM);

    /* Analog inputs: PA0, PA6, PA7, PB0, PB1; DAC outputs PA4, PA5 */
    gpio_cfg(GPIOA, GPIO_PIN_0 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,
             GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
    gpio_cfg(GPIOB, GPIO_PIN_0 | GPIO_PIN_1, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
    vrange_set_high(false);

    /* UARTs */
    gpio_cfg(GPIOA, GPIO_PIN_9 | GPIO_PIN_2, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH);
    gpio_cfg(GPIOA, GPIO_PIN_10 | GPIO_PIN_3, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH);

    /* TIM2_CH2 on PB3 (full remap, set above together with SWJ_CFG) */
    gpio_cfg(PIN_PWM_PORT, PIN_PWM, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH);
}

/* ------------------------------------------------------------------------- */
/* Internal ADC                                                               */
/* ------------------------------------------------------------------------- */
static void adc_init(void)
{
    __HAL_RCC_ADC1_CLK_ENABLE();
    MODIFY_REG(RCC->CFGR, RCC_CFGR_ADCPRE, RCC_CFGR_ADCPRE_DIV4);   /* 6 MHz */

    /* SCAN so a 2-channel injected group converts both channels; the regular
     * group is always a single channel (L = 0), so behaviour is unchanged. */
    ADC1->CR1 = ADC_CR1_SCAN;
    ADC1->CR2 = ADC_CR2_EXTSEL | ADC_CR2_EXTTRIG                     /* SWSTART  */
              | ADC_CR2_JEXTSEL | ADC_CR2_JEXTTRIG                   /* JSWSTART */
              | ADC_CR2_TSVREFE;
    ADC1->SQR1 = 0;
    /* 7.5 cycles on PA0/PA6 (fast sampler), 55.5 on ch7..9, 239.5 on temp */
    ADC1->SMPR2 = (1u << (3u * 0u)) | (1u << (3u * 6u))
                | (5u << (3u * 7u)) | (5u << (3u * 8u)) | (5u << (3u * 9u));
    ADC1->SMPR1 = (7u << (3u * (16u - 10u)));
    ADC1->JSQR = (6u << 15);                                         /* PA6 only */

    ADC1->CR2 |= ADC_CR2_ADON;
    delay_us(10);
    ADC1->CR2 |= ADC_CR2_RSTCAL;
    while (ADC1->CR2 & ADC_CR2_RSTCAL) { }
    ADC1->CR2 |= ADC_CR2_CAL;
    while (ADC1->CR2 & ADC_CR2_CAL) { }
}

/* Regular conversions run from the main loop only. While they run, the
 * 10 kHz injected sampler stays off the ADC (g_adc_lock), because on this
 * part EOC is also set by injected conversions. */
volatile uint8_t g_adc_lock;

static void adc_lock(void)   { g_adc_lock = 1; ADC1->SR = 0; }
static void adc_unlock(void) { ADC1->SR = 0; g_adc_lock = 0; }

static uint16_t adc_convert(uint32_t ch)
{
    ADC1->SQR3 = ch;
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while ((ADC1->SR & ADC_SR_EOC) == 0u) { }
    return (uint16_t)ADC1->DR;
}

/* Injected group for the fast sampler: PA6 and/or PA0 (in that order). */
void adc_fast_config(uint8_t pa6, uint8_t pa0)
{
    if (pa6 && pa0) ADC1->JSQR = (1u << 20) | (6u << 10) | (0u << 15);  /* JL=1: 6, 0 */
    else if (pa0)   ADC1->JSQR = (0u << 15);                            /* JL=0: 0    */
    else            ADC1->JSQR = (6u << 15);                            /* JL=0: 6    */
}

/* 10 samples, min and max discarded; returns sum of the remaining 8 */
uint16_t adc_read_avg10(uint32_t ch, uint32_t *sum8)
{
    uint32_t sum = 0, mn = 0xFFFF, mx = 0;
    adc_lock();
    for (int n = 0; n < 10; n++) {
        uint32_t s = adc_convert(ch);
        sum += s;
        if (s < mn) mn = s;
        if (s > mx) mx = s;
    }
    adc_unlock();
    sum = sum - mn - mx;
    if (sum8) *sum8 = sum;
    return (uint16_t)(sum / 8u);
}

/* Diagnostic burst: n back-to-back conversions (7.5-cycle sample time on
 * PA0/PA6 -> ~3.3 us each); returns average, minimum and maximum. */
void adc_burst(uint32_t ch, uint32_t n, uint16_t *avg, uint16_t *mn, uint16_t *mx)
{
    uint32_t sum = 0, lo = 0xFFFF, hi = 0;
    adc_lock();
    for (uint32_t k = 0; k < n; k++) {
        uint32_t s = adc_convert(ch);
        sum += s;
        if (s < lo) lo = s;
        if (s > hi) hi = s;
    }
    adc_unlock();
    *avg = (uint16_t)(sum / n);
    *mn = (uint16_t)lo;
    *mx = (uint16_t)hi;
}

/* 130 samples, min and max discarded, sum/16 (= 8x oversampled value) */
uint32_t adc_read_avg130(uint32_t ch)
{
    uint32_t sum = 0, mn = 0xFFFF, mx = 0;
    adc_lock();
    for (int n = 0; n < 130; n++) {
        uint32_t s = adc_convert(ch);
        sum += s;
        if (s < mn) mn = s;
        if (s > mx) mx = s;
    }
    adc_unlock();
    return ((sum - mx - mn) & 0xFFFFFu) >> 4;
}

/* ------------------------------------------------------------------------- */
/* Internal DAC (PA4 = CV threshold, PA5 = CC setpoint), buffers disabled     */
/* ------------------------------------------------------------------------- */
static void dac_init(void)
{
    __HAL_RCC_DAC_CLK_ENABLE();
    DAC->CR = DAC_CR_EN1 | DAC_CR_BOFF1 | DAC_CR_EN2 | DAC_CR_BOFF2;
    DAC->DHR12R1 = 0;
    DAC->DHR12R2 = 0;
}

void dac_set_cv(uint16_t v) { DAC->DHR12R1 = v & 0xFFFu; }
void dac_set_cc(uint16_t v) { DAC->DHR12R2 = v & 0xFFFu; }

/* ------------------------------------------------------------------------- */
/* DAC8562 - initialised exactly like the original firmware                   */
/* ------------------------------------------------------------------------- */
static void xdac_shift(uint32_t val, uint32_t bits)
{
    for (uint32_t n = 0; n < bits; n++) {
        pin_set(PIN_X_DAC_PORT, PIN_X_DAC_SCLK);
        if (val & (1u << (bits - 1u - n))) pin_set(PIN_X_DAC_PORT, PIN_X_DAC_DIN);
        else                                pin_reset(PIN_X_DAC_PORT, PIN_X_DAC_DIN);
        delay_us(1);
        pin_reset(PIN_X_DAC_PORT, PIN_X_DAC_SCLK);   /* data latched on falling edge */
        delay_us(1);
    }
}

static void xdac_write(uint8_t cmd, uint16_t data)
{
    pin_reset(PIN_X_DAC_PORT, PIN_X_DAC_SYNC);
    xdac_shift(cmd, 8);
    xdac_shift(data, 16);
    pin_set(PIN_X_DAC_PORT, PIN_X_DAC_SYNC);
    delay_us(2);
}

static void xdac_init(void)
{
    xdac_write(0x28, 1);   /* software reset                 */
    xdac_write(0x20, 3);   /* power up DAC A and B           */
    xdac_write(0x38, 1);   /* internal 2.5 V reference on    */
    xdac_write(0x02, 0);   /* gain register                  */
    xdac_write(0x30, 3);   /* LDAC pins inactive for A and B */
}

/* ------------------------------------------------------------------------- */
/* ADS1115 on bit-banged I2C (PC14 SDA, PC15 SCL, push-pull like original)    */
/* ------------------------------------------------------------------------- */
#define SDA_H() pin_set(PIN_ADS_PORT, PIN_ADS_SDA)
#define SDA_L() pin_reset(PIN_ADS_PORT, PIN_ADS_SDA)
#define SCL_H() pin_set(PIN_ADS_PORT, PIN_ADS_SCL)
#define SCL_L() pin_reset(PIN_ADS_PORT, PIN_ADS_SCL)
#define I2C_T   2u      /* start/stop/ack hold time, us                     */
#define I2C_TW  1u      /* half-bit time while the MCU drives SDA (writes), us */
#define I2C_TR  2u      /* half-bit time while the ADS drives SDA (reads), us  */

/* PC14 is in CRH[27:24]. Driven push-pull while the MCU owns the line,
 * released to input + internal pull-up whenever the ADS1115 may drive it. */
static inline void sda_drive(void)
{
    GPIOC->CRH = (GPIOC->CRH & ~(0xFu << 24)) | (0x3u << 24);   /* out PP 50 MHz */
}
static inline void sda_release(void)
{
    SDA_H();                                                    /* ODR=1: pull-up */
    GPIOC->CRH = (GPIOC->CRH & ~(0xFu << 24)) | (0x8u << 24);   /* input pull     */
}

static void i2c_start(void)
{
    sda_drive();
    SCL_H(); SDA_H(); delay_us(I2C_T);
    SDA_L(); delay_us(I2C_T);
    SCL_L(); delay_us(I2C_T);
}

static void i2c_stop(void)
{
    sda_drive();
    SDA_L(); delay_us(I2C_T);
    SCL_H(); delay_us(I2C_T);
    SDA_H(); delay_us(I2C_T);
}

static bool i2c_write_byte(uint8_t b)
{
    for (int n = 0; n < 8; n++) {
        if (b & 0x80u) SDA_H(); else SDA_L();
        b <<= 1;
        delay_us(I2C_TW);
        SCL_H(); delay_us(I2C_TW);
        SCL_L();
    }
    sda_release(); delay_us(I2C_TR);
    SCL_H(); delay_us(I2C_TR);
    bool ack = !pin_read(PIN_ADS_PORT, PIN_ADS_SDA);
    SCL_L(); delay_us(I2C_TW);
    sda_drive();
    return ack;
}

static uint8_t i2c_read_byte(bool ack)
{
    uint8_t b = 0;
    sda_release();
    for (int n = 0; n < 8; n++) {
        delay_us(I2C_TR);
        SCL_H(); delay_us(I2C_TR);
        b = (uint8_t)((b << 1) | (pin_read(PIN_ADS_PORT, PIN_ADS_SDA) ? 1u : 0u));
        SCL_L();
    }
    sda_drive();
    if (ack) SDA_L(); else SDA_H();
    delay_us(I2C_TW);
    SCL_H(); delay_us(I2C_T);
    SCL_L(); SDA_H(); delay_us(I2C_T);
    return b;
}

static bool ads_write(const uint8_t *buf, uint32_t len)
{
    i2c_start();
    for (uint32_t n = 0; n < len; n++) {
        if (!i2c_write_byte(buf[n])) { i2c_stop(); return false; }
    }
    i2c_stop();
    return true;
}

static bool ads_read_reg(uint8_t reg, uint16_t *val)
{
    uint8_t ptr[2] = { 0x90, reg };
    if (!ads_write(ptr, 2)) return false;
    i2c_start();
    if (!i2c_write_byte(0x91)) { i2c_stop(); return false; }
    uint8_t hi = i2c_read_byte(true);
    uint8_t lo = i2c_read_byte(false);
    i2c_stop();
    *val = (uint16_t)((hi << 8) | lo);
    return true;
}

/* Single-shot conversion. Negative results read as 0. */
bool ads_read(uint32_t ch, uint32_t *raw)
{
    uint16_t cfg = s_ads_cfg[ch & 3u];
    uint8_t wr[4] = { 0x90, 0x01, (uint8_t)(cfg >> 8), (uint8_t)cfg };
    uint16_t v;
    int tries;

    for (tries = 0; tries < 3; tries++) {
        if (ads_write(wr, 4)) break;
    }
    if (tries == 3) return false;

    delay_us(1000);
    for (tries = 0; tries < 10; tries++) {       /* wait for OS = 1 (idle) */
        if (ads_read_reg(0x01, &v) && (v & 0x8000u)) break;
        delay_us(200);
    }
    if (!ads_read_reg(0x00, &v)) return false;

    *raw = (v & 0x8000u) ? 0u : v;
    return true;
}

/* Pipelined use: ads_start() begins a single-shot conversion; ads_poll()
 * reports completion (OS bit); ads_fetch() reads the conversion register.
 * The conversion register keeps the previous result until the running
 * conversion finishes, so the caller starts conversion N+1 first and then
 * fetches result N while N+1 is converting. */
bool ads_start(uint32_t ch)
{
    uint16_t cfg = s_ads_cfg[ch & 3u];
    uint8_t wr[4] = { 0x90, 0x01, (uint8_t)(cfg >> 8), (uint8_t)cfg };
    for (int tries = 0; tries < 2; tries++)
        if (ads_write(wr, 4)) return true;
    return false;
}

int ads_poll(void)
{
    uint16_t v;
    if (!ads_read_reg(0x01, &v)) return -1;
    return (v & 0x8000u) ? 1 : 0;
}

bool ads_fetch(uint16_t *raw)
{
    return ads_read_reg(0x00, raw);
}

/* ------------------------------------------------------------------------- */
/* Timers                                                                     */
/* ------------------------------------------------------------------------- */
static void tim2_pwm_init(void)
{
    __HAL_RCC_TIM2_CLK_ENABLE();
    TIM2->PSC   = 0;
    TIM2->ARR   = 24;                                 /* 24 MHz / 25 = 960 kHz */
    TIM2->CCMR1 = (7u << TIM_CCMR1_OC2M_Pos) | TIM_CCMR1_OC2PE;   /* PWM2      */
    TIM2->CCR2  = 8;
    TIM2->CCER  = TIM_CCER_CC2E | TIM_CCER_CC2P;      /* active low            */
    TIM2->CR1   = TIM_CR1_ARPE;
    TIM2->EGR   = TIM_EGR_UG;
    TIM2->CR1  |= TIM_CR1_CEN;
}

static void tim7_init(void)
{
    __HAL_RCC_TIM7_CLK_ENABLE();
    TIM7->PSC  = 23;                                  /* 1 MHz                 */
    TIM7->ARR  = 99;                                  /* 10 kHz                */
    TIM7->EGR  = TIM_EGR_UG;
    TIM7->SR   = 0;
    TIM7->DIER = TIM_DIER_UIE;
    HAL_NVIC_SetPriority(TIM7_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(TIM7_IRQn);
}

void hw_start_tick(void)
{
    TIM7->CR1 |= TIM_CR1_CEN;
}

/* ------------------------------------------------------------------------- */
/* UARTs                                                                      */
/* ------------------------------------------------------------------------- */
static void usart_setup(USART_TypeDef *u, uint32_t baud)
{
    u->CR1 = 0;
    u->BRR = (PCLK_HZ + baud / 2u) / baud;
    u->CR2 = 0;
    u->CR3 = 0;
    u->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
}

void usart2_set_bps(uint32_t bps)
{
    while (rs485_busy()) { }
    usart_setup(USART2, bps);
}

void usart2_set_baud(uint32_t idx)
{
    usart2_set_bps(baud_from_index(idx));
}

static void uart_init(void)
{
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();
    usart_setup(USART1, 19200);
    usart2_set_baud(CFG(CFG_MB_BAUD));
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
}

/* RS-485 transmit by DMA (DMA1 channel 7 = USART2_TX). The buffer must stay
 * valid until rs485_busy() returns false. DE is released from the USART
 * transmission-complete interrupt once the last stop bit has left. */
static volatile uint8_t s_tx_busy;

bool rs485_busy(void) { return s_tx_busy != 0u; }

void rs485_send(const uint8_t *buf, uint32_t len)
{
    if (len == 0u) return;
    while (s_tx_busy) { }
    s_tx_busy = 1;
    DMA1_Channel7->CCR = 0;
    DMA1->IFCR = DMA_IFCR_CGIF7;
    DMA1_Channel7->CPAR = (uint32_t)&USART2->DR;
    DMA1_Channel7->CMAR = (uint32_t)buf;
    DMA1_Channel7->CNDTR = len;
    pin_set(PIN_RS485_DE_PORT, PIN_RS485_DE);
    USART2->SR = ~USART_SR_TC;
    USART2->CR3 |= USART_CR3_DMAT;
    DMA1_Channel7->CCR = DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_PL_1 | DMA_CCR_EN;
    USART2->CR1 |= USART_CR1_TCIE;
}

void rs485_tx_isr(void)
{
    if (DMA1_Channel7->CNDTR != 0u) {        /* not finished: spurious TC */
        USART2->SR = ~USART_SR_TC;
        return;
    }
    USART2->CR1 &= ~USART_CR1_TCIE;
    DMA1_Channel7->CCR = 0;
    USART2->CR3 &= ~USART_CR3_DMAT;
    pin_reset(PIN_RS485_DE_PORT, PIN_RS485_DE);
    s_tx_busy = 0;
}

void usart1_puts(const char *s)
{
    while (*s) {
        while ((USART1->SR & USART_SR_TXE) == 0u) { }
        USART1->DR = (uint8_t)*s++;
    }
}

/* ------------------------------------------------------------------------- */
/* Independent watchdog: /64, reload 625 -> ~1 s                               */
/* ------------------------------------------------------------------------- */
static void iwdg_init(void)
{
    DBGMCU->CR |= DBGMCU_CR_DBG_IWDG_STOP;
    IWDG->KR  = 0x5555;
    IWDG->PR  = 4;
    IWDG->RLR = 625;
    IWDG->KR  = 0xAAAA;
    IWDG->KR  = 0xCCCC;
}

void iwdg_kick(void)
{
    IWDG->KR = 0xAAAA;
}

/* ------------------------------------------------------------------------- */
void hw_init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    gpio_init();
    adc_init();
    dac_init();
    xdac_init();
    tim2_pwm_init();
    tim7_init();
    uart_init();
    iwdg_init();
}
