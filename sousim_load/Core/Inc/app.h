/**
 * @file    app.h
 * @brief   Sousim 500 W electronic load - clean-sheet firmware, shared definitions.
 *
 * Re-implementation of the behaviour recovered from the original firmware
 * (see ../ghidra). Display, keypad and encoder support were dropped; the unit is
 * controlled through Modbus RTU (USART2 / RS-485) with the same register map
 * and command set as the original.
 *
 * Units (identical to the original firmware):
 *   voltage    10 mV      (20000 = 200.00 V)
 *   current     1 mA      (60000 =  60.000 A)
 *   power      10 mW      (100 mW when CFG_POWER_RES != 0)
 *   resistance V*100/I    (setpoint for CR mode, same scaling)
 *   temps      NTC: whole degC (0..99), MCU: 0.1 degC
 */
#ifndef APP_H
#define APP_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f1xx.h"

/* ------------------------------------------------------------------------- */
/* Board I/O (STM32F100C8T6B, LQFP48)                                         */
/* ------------------------------------------------------------------------- */
/* Load control */
#define PIN_LOAD_INH_PORT   GPIOA   /* PA15 high = load inhibited             */
#define PIN_LOAD_INH        (1u << 15)
#define PIN_AUX_OUT_PORT    GPIOB   /* PB11 auxiliary/alarm output            */
#define PIN_AUX_OUT         (1u << 11)
#define PIN_IRANGE_PORT     GPIOB   /* PB2  high = high current range (>5.5 A) */
#define PIN_IRANGE          (1u << 2)
#define PIN_VRANGE_PORT     GPIOA   /* PA1  analog = low V range, out-low = high */
#define PIN_VRANGE          (1u << 1)
/* Fan / buzzer */
#define PIN_FAN_PORT        GPIOB   /* PB4  fan, 50 Hz software PWM           */
#define PIN_FAN             (1u << 4)
#define PIN_BUZ_PORT        GPIOB   /* PB10 passive buzzer, 1 kHz             */
#define PIN_BUZ             (1u << 10)
/* TIM2_CH2 (full remap) on PB3: ~960 kHz 32 % PWM (bias / charge pump)      */
#define PIN_PWM_PORT        GPIOB
#define PIN_PWM             (1u << 3)
/* ADS1115 on bit-banged I2C */
#define PIN_ADS_PORT        GPIOC
#define PIN_ADS_SDA         (1u << 14)
#define PIN_ADS_SCL         (1u << 15)
/* DAC8562 on bit-banged SPI (initialised exactly like the original, then idle) */
#define PIN_X_DAC_PORT      GPIOB
#define PIN_X_DAC_SCLK      (1u << 12)
#define PIN_X_DAC_DIN       (1u << 13)
#define PIN_X_DAC_SYNC      (1u << 14)
/* RS-485 driver enable for USART2 */
#define PIN_RS485_DE_PORT   GPIOC
#define PIN_RS485_DE        (1u << 13)
/* Internal ADC channels */
#define ADC_CH_ISENSE       6u      /* PA6  OPA2188 output: current-sense signal
                                       (not used by the original firmware) */
#define ADC_CH_PA0          0u      /* PA0  unknown, rises with load current */
#define ADC_CH_AUX          7u      /* PA7  */
#define ADC_CH_NTC1         8u      /* PB0  */
#define ADC_CH_NTC2         9u      /* PB1  */
#define ADC_CH_TEMP         16u     /* internal temperature sensor */
/* ADS1115 channel selectors (index into config word table) */
#define ADS_CH_CURRENT      0u      /* AIN0-AIN3, +-2.048 V */
#define ADS_CH_VOLTAGE      1u      /* AIN1-AIN3, +-2.048 V */
#define ADS_CH_VIN_HI       2u      /* AIN2-AIN3, +-2.048 V */
#define ADS_CH_VIN_LO       3u      /* AIN2-AIN3, +-1.024 V */

/* ------------------------------------------------------------------------- */
/* Persistent configuration (byte-compatible with the original 0x0800F000)   */
/* ------------------------------------------------------------------------- */
#define CFG_FLASH_ADDR      0x0800F000u
#define CFG_BYTES           0x2BCu
#define CFG_WORDS           (CFG_BYTES / 2u)
#define CFG_MAGIC_VALUE     0x07E4u

#define CFG_MAGIC           0x000   /* u32 0x000007E4                         */
#define CFG_FW_MARKER       0x004   /* unused by original; set by this firmware */
#define CFG_FW_MARKER_VALUE 0xB0D5u
#define MB_BAUD_DEFAULT_IDX 13u     /* 115200 */
#define MB_BAUD_MAX_IDX     19u     /* extended table: 460800, 921600, 1M, 1.5M */
#define CFG_VMAX            0x00A   /* max voltage                            */
#define CFG_IMAX            0x00C   /* max current                            */
#define CFG_PMAX            0x00E   /* max power                              */
#define CFG_SAVED_V         0x010   /* voltage setpoint restored at boot      */
#define CFG_SAVED_I         0x012   /* current setpoint restored at boot      */
#define CFG_SAVED_P         0x014
#define CFG_SOFTSTART       0x018   /* original soft-start steps; unused (soft start removed) */
#define CFG_SLEW_UP         0x01A   /* legacy ramp step up   (mA / 0.2 ms), kept in step with CFG_RAMP_UP */
#define CFG_SLEW_DOWN       0x01C   /* legacy ramp step down (mA / 0.2 ms)    */
#define CFG_MODE            0x020   /* operating mode, see MODE_*             */
#define CFG_PRESET_TYPE     0x022   /* 0=V 1=I 2=P 3=R for presets/list       */
#define CFG_MB_ADDR         0x024
#define CFG_MB_BAUD         0x026   /* index into baud table                  */
#define CFG_I_GAIN          0x028   /* current ADC gain                       */
#define CFG_VREF            0x02A   /* reference in mV (DAC/ADC scaling)      */
#define CFG_V_ZERO          0x02C   /* voltage zero threshold                 */
#define CFG_I_ZERO          0x02E   /* current zero threshold                 */
#define CFG_V_GAIN_LO       0x030
#define CFG_V_GAIN_HI       0x032
#define CFG_VIN_GAIN_LO     0x034
#define CFG_VIN_GAIN_HI     0x036
#define CFG_RAMP_UP         0x038   /* u32 (low word first) current ramp up,   mA/s, 0 = no limit.
                                     * 0x038-0x03F are never used by the original firmware. */
#define CFG_RAMP_DOWN       0x03C   /* u32 current ramp down, mA/s, 0 = no limit */
#define CFG_VDAC_SCALE_LO   0x040
#define CFG_VDAC_SCALE_HI   0x042
#define CFG_V_FROM_VIN      0x04E   /* 1: report VIN/10 as voltage            */
#define CFG_IMEAS_LO_TBL    0x050   /* 15 entries, low current range gains    */
#define CFG_IMEAS_HI_TBL    0x06E   /* high range gains (sparse up to 0x128)  */
#define CFG_IMEAS_HI_LAST   0x128
#define CFG_IDAC_DEFAULT    0x12A   /* current DAC calibration, setpoint 0    */
#define CFG_IDAC_TBL        0x12C   /* current DAC calibration table          */
#define CFG_POWER_RES       0x1EA   /* !=0: power in 100 mW units             */
#define CFG_RANGE_FALLBACK  0x1EC
#define CFG_CUTOFF_V        0x1EE   /* !=0: switch off when V <= this         */
#define CFG_ITRIM_LIMIT     0x1F0   /* closed-loop current trim upper limit   */
#define CFG_PRESET_VAL      0x1F2   /* 10 preset / list values                */
#define CFG_PRESET_AUX      0x256   /* 10 preset on-states / list durations   */

extern uint16_t g_cfg[CFG_WORDS];
extern const uint16_t g_cfg_defaults[CFG_WORDS];
#define CFG(off)            (g_cfg[(off) / 2u])

void cfg_load(void);
void cfg_save(void);

/* ------------------------------------------------------------------------- */
/* Modes and Modbus commands                                                 */
/* ------------------------------------------------------------------------- */
enum {
    MODE_CV = 0, MODE_CC = 1, MODE_CP = 2, MODE_CR = 3, MODE_CC_FAST = 4,
    MODE_CR2 = 5, MODE_LIST2 = 9, MODE_LIST10 = 10,
    MODE_PRESET1 = 0x0B, MODE_PRESET10 = 0x14,
};

enum {
    CMD_NONE          = 0,
    CMD_FAN_FULL      = 1,
    CMD_BEEP          = 2,
    CMD_TEMP_MCU      = 7,      /* input reg 3 shows MCU temperature          */
    CMD_TEMP_NTC2     = 8,      /* input reg 3 shows NTC2                      */
    CMD_RESET_ENERGY  = 10,
    CMD_RD_VSCALE     = 0x0B,   /* holding reg 8 read-back selectors           */
    CMD_RD_IGAIN      = 0x0C,
    CMD_RD_VDAC_LO    = 0x0D,
    CMD_RD_VDAC_HI    = 0x0E,
    CMD_RD_V_ZERO     = 0x0F,
    CMD_RD_I_ZERO     = 0x10,
    CMD_RD_VREF       = 0x11,
    CMD_RD_ITRIM      = 0x12,
    CMD_SET_V_FROM_VIN= 0x4D,
    CMD_SET_CUTOFF    = 0x58,
    CMD_CAL_IMEAS     = 0x6E,   /* factory current-measurement calibration     */
    CMD_CAL_IDAC      = 0x6F,   /* factory current-DAC calibration             */
    CMD_CAL_VDAC_LO   = 0x73,
    CMD_CAL_VDAC_HI   = 0x74,
    CMD_CAL_VMEAS     = 0x76,
    CMD_CAL_VINMEAS   = 0x77,
    CMD_SAVE          = 0x78,
    CMD_STORE_IDAC    = 0x79,
    CMD_SET_BAUD      = 0xE8,
    CMD_SET_POWER_RES = 500,
    CMD_SET_MB_ADDR   = 0x1E5,
    CMD_SET_SLEW_DOWN = 1000,
    CMD_SET_SLEW_UP   = 1001,
    CMD_SET_V_ZERO    = 1010,
    CMD_CLR_V_ZERO    = 1011,
    CMD_SET_I_ZERO    = 2010,
    CMD_CLR_I_ZERO    = 2011,
    CMD_SET_RANGE_FB  = 0xFFF8, /* -8 */
    CMD_SET_SOFTSTART = 0xFFF9, /* -7 */
    CMD_SET_ITRIM     = 0xFFFA, /* -6 */
    CMD_SET_VMAX      = 0xFFFB, /* -5 */
    CMD_SET_IMAX      = 0xFFFC, /* -4 */
    CMD_SET_PMAX      = 0xFFFD, /* -3 */
    CMD_SET_VREF      = 0xFFFE, /* -2 */
    CMD_AUX_OUT       = 0xFFFF, /* -1: drive PB11 high while load is off       */
};

/* ------------------------------------------------------------------------- */
/* Runtime state                                                             */
/* ------------------------------------------------------------------------- */
typedef struct {
    volatile uint32_t on;          /* load enabled                           */
    volatile uint32_t v;           /* measured voltage                       */
    volatile uint32_t i;           /* measured current                       */
    volatile uint32_t set_v;
    volatile uint32_t set_i;
    volatile uint32_t set_p;
    volatile uint32_t set_r;
    volatile uint32_t aux;         /* PA7 analog input                       */
    volatile uint32_t vin;         /* second voltage input (ADS AIN2), mV    */
    volatile uint32_t p;           /* measured power                         */
    volatile uint32_t r;           /* measured resistance                    */
    volatile uint32_t uah;         /* charge, uAh                            */
    volatile uint32_t energy;      /* energy accumulator (/1000 -> reg)      */
    volatile uint32_t seconds;     /* run-time counter                       */
    volatile uint32_t t_mcu;       /* 0.1 degC (negative: 9000 - value)      */
    volatile uint32_t t_ntc1;      /* degC                                   */
    volatile uint32_t t_ntc2;      /* degC                                   */
    volatile uint32_t list_step;   /* list mode step (0x0B..0x14)            */
    volatile uint32_t list_time;   /* current step duration (0.1 ms)         */
    volatile uint32_t value;       /* Modbus "value" register                */
} load_state_t;

extern load_state_t g_st;
extern volatile uint16_t g_cmd;          /* Modbus "command" register          */
extern volatile uint32_t g_meter;        /* reference meter reading (USART1)   */
extern volatile uint16_t g_last_set[4];  /* last V/I/P/R setpoints seen        */

/* Calibration working values (shared with Modbus registers 9..12) */
extern volatile uint16_t g_vin_gain_reg; /* active VIN gain (reg 9)           */
extern volatile uint16_t g_v_gain_reg;   /* active V gain (reg 10)            */
extern volatile uint16_t g_idac_cal;     /* active current-DAC factor (reg 12) */
extern volatile uint16_t g_vdac_scale;   /* active voltage-DAC scale          */
extern volatile uint16_t g_vdac_base;    /* range default for g_vdac_scale    */
extern volatile uint32_t g_imeas_gain;   /* active current-measurement gain   */
extern volatile uint16_t g_iscale;       /* current DAC scale from IMAX       */
extern volatile uint16_t g_pdiv;         /* 1 or 10                            */
extern volatile uint32_t g_cal_step;     /* factory calibration step (mA)     */
extern volatile uint8_t  g_dac_settled;  /* current ramp reached setpoint     */
extern volatile uint8_t  g_fan_speed;    /* 0..200                             */
extern volatile uint8_t  g_alarm;        /* protection tripped                 */

/* Tick flags set from the 10 kHz interrupt */
extern volatile uint8_t g_flag_10ms, g_flag_100ms, g_flag_500ms, g_isr_alive;

/* ------------------------------------------------------------------------- */
/* Hardware layer (app_hw.c)                                                 */
/* ------------------------------------------------------------------------- */
void hw_init(void);
void hw_start_tick(void);
void delay_us(uint32_t us);
void iwdg_kick(void);
void dac_set_cv(uint16_t v);       /* internal DAC ch1, PA4 */
void dac_set_cc(uint16_t v);       /* internal DAC ch2, PA5 */
uint16_t adc_read_avg10(uint32_t ch, uint32_t *sum8);
uint32_t adc_read_avg130(uint32_t ch);
void     adc_burst(uint32_t ch, uint32_t n, uint16_t *avg, uint16_t *mn, uint16_t *mx);
bool     ads_read(uint32_t ch, uint32_t *raw);
void usart2_set_baud(uint32_t idx);
void usart2_set_bps(uint32_t bps);
uint32_t baud_from_index(uint32_t idx);
#define BAUD_TABLE_N 20u                   /* 0..15 original, 16..19 = 460k8..1M5 */
void rs485_send(const uint8_t *buf, uint32_t len);   /* non-blocking (DMA)   */
bool rs485_busy(void);
void rs485_tx_isr(void);                   /* USART2 TC interrupt              */
void usart1_puts(const char *s);

/* ADS1115 split transactions for the pipelined sampler */
bool ads_start(uint32_t ch);               /* write config: starts conversion  */
int  ads_poll(void);                       /* 1 done, 0 busy, -1 bus error     */
bool ads_fetch(uint16_t *raw);             /* read conversion register         */

/* Internal ADC is shared between the main loop (regular conversions) and the
 * 10 kHz fast sampler (injected conversions); main holds this while converting */
extern volatile uint8_t g_adc_lock;
void adc_fast_config(uint8_t pa6, uint8_t pa0);

static inline void pin_set(GPIO_TypeDef *p, uint32_t m)   { p->BSRR = m; }
static inline void pin_reset(GPIO_TypeDef *p, uint32_t m) { p->BRR = m; }
static inline bool pin_read(GPIO_TypeDef *p, uint32_t m)  { return (p->IDR & m) != 0u; }
void vrange_set_high(bool high);

/* ------------------------------------------------------------------------- */
/* Modules                                                                    */
/* ------------------------------------------------------------------------- */
void ctl_init(void);
void ctl_tick_isr(void);           /* called from TIM7 at 10 kHz              */
void ctl_mode_update(void);        /* every 10 ms                             */

void meas_init(void);
void meas_service(void);           /* pipelined ADS1115 sampler, call often   */
void meas_slow(void);              /* 100 ms: aux, PA0/PA6, temps, VIN request */
void meas_voltage(void);           /* no-op: voltage is sampled continuously  */
void meas_stats(uint16_t *err, uint16_t *discard, uint16_t *sps, uint16_t *poll_at);
uint8_t meas_vin_range(void);
void app_on_voltage(void);         /* callbacks after each new V / I sample   */
void app_on_current(void);

void mb_init(void);
void mb_rx_byte_isr(uint8_t b);
void mb_tick_isr(void);            /* 10 kHz: inter-frame gap detection      */
void mb_poll(void);
void mb_set_address(uint8_t addr);

/* ------------------------------------------------------------------------- */
/* Real-time sample stream (app_stream.c)                                    */
/* ------------------------------------------------------------------------- */
enum {
    REC_V = 0x01, REC_I = 0x02, REC_VIN = 0x03, REC_SLOW = 0x04,
    REC_STATE = 0x05, REC_FAST = 0x06, REC_EVENT = 0x07,
};
enum {
    EVT_PROT_TRIP = 1, EVT_PROT_CLEAR = 2, EVT_CFG_SAVE = 3, EVT_LC_SAVE = 4,
    EVT_BAUD = 5, EVT_ADS_ERR = 6, EVT_STREAM_START = 7,
};
extern volatile uint32_t g_tick;           /* 10 kHz tick counter           */
extern volatile uint8_t  g_prot_evt;       /* set in ISR, emitted by main   */
uint32_t now_us(void);
void stream_init(void);
bool stream_put(uint8_t type, const void *hdr, uint8_t hlen, const void *data, uint8_t dlen);
void stream_sample(uint8_t type, uint32_t ts, uint16_t raw, uint16_t val);
void stream_event(uint8_t code, uint16_t data);
void stream_service(void);                 /* main loop: fast blocks, state */
void stream_fast_isr(void);                /* 10 kHz                         */
void stream_fast_flush(void);
uint32_t stream_read(uint8_t *dst, uint32_t max, uint8_t *flags);
bool stream_enabled(void);
bool stream_reg_read(uint16_t a, uint16_t *v);
bool stream_reg_writable(uint16_t a);
void stream_reg_write(uint16_t a, uint16_t v);
void ctl_dac_state(uint32_t *ramp, uint16_t *stopping);

/* Current ramp settings, Modbus registers 0x1C0-0x1C3 (app_control.c):
 *   0x1C0/0x1C1 ramp up   (u32 mA/s, high word first)
 *   0x1C2/0x1C3 ramp down (u32 mA/s, high word first)
 * A write applies at once and saves the config page. */
#define RAMP_REG_FIRST  0x1C0u
#define RAMP_REG_LAST   0x1C3u
void ctl_host_set(uint16_t on, uint16_t set_i);   /* Modbus on/off (off immediate) + set I (set 0 ramps) */
void ctl_force_off(void);               /* immediate off */
void ctl_ramp_from_legacy(bool up);     /* after legacy commands 1000 / 1001 */
bool ramp_reg_read(uint16_t a, uint16_t *v);
bool ramp_reg_writable(uint16_t a);
void ramp_reg_write(uint16_t a, uint16_t v);
extern volatile uint8_t g_cfg_save_req;  /* main loop saves the config page */

void cal_rx_byte_isr(uint8_t b);
void cal_poll_meter(void);
void cal_voltage_tick(void);
void cal_idac_tick(void);
void cal_imeas_tick(void);
void cal_store_idac(void);
uint16_t idac_slot_for(uint32_t ma);   /* returns cfg offset, 0 if > 60 A    */

/* ------------------------------------------------------------------------- */
/* Linear calibration (app_lincal.c)                                         */
/* ------------------------------------------------------------------------- */
enum {
    LC_V_LO, LC_V_HI, LC_I_LO, LC_I_HI, LC_VIN_LO, LC_VIN_HI,
    LC_IDAC_LO, LC_IDAC_HI, LC_VDAC_LO, LC_VDAC_HI, LC_N
};
#define LC_REG_COEF  0x120u
#define LC_REG_END   (LC_REG_COEF + 4u * LC_N)

typedef struct {
    volatile uint8_t  raw_mode, force_vr, force_ir, force_vinr;
    volatile uint16_t raw_cc, raw_cv;
    volatile uint16_t action, action_result;
    volatile uint16_t raw_v, raw_i, raw_vin, ranges, seq_vi, seq_vin;
    volatile uint16_t diag[6];               /* PA0 avg/min/max, PA6 avg/min/max */
    volatile uint32_t comm_ms;
} lc_ctl_t;
extern lc_ctl_t g_lc;
extern volatile uint8_t g_vrange_hi;     /* PA1 state: high voltage range    */

void     lc_init(void);
void     lc_poll(void);
bool     lc_valid(uint32_t ch);
int32_t  lc_apply(uint32_t ch, int32_t x);
uint32_t lc_apply_u(uint32_t ch, int32_t x);
void     lc_tick_1ms_isr(void);
void     lc_comm_seen(void);
bool     lc_reg_read(uint16_t a, uint16_t *v);
bool     lc_reg_writable(uint16_t a);
void     lc_reg_write(uint16_t a, uint16_t v);

void app_clock_config(void);
void app_init(void);
void app_loop(void);

#endif /* APP_H */
