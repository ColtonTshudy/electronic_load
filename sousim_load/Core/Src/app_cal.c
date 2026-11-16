/**
 * @file  app_cal.c
 * @brief Factory calibration commands, driven through the Modbus command
 *        register, plus the USART1 link to the calibration instruments.
 *
 * USART1 (19200 8N1) talks SCPI to a bench supply ("VOLT", "CURR",
 * "MEASure:CURRent?") and receives its readings as "dd.dddd\n" (amps).
 *
 *   0x6E  current-measurement gain tables (CFG 0x050..0x128), needs PSU
 *   0x6F  current-DAC calibration table   (CFG 0x12C..0x1C6), needs meter
 *   0x73  voltage-DAC scale, low range   (CFG 0x040) at 30.00 V
 *   0x74  voltage-DAC scale, high range  (CFG 0x042) at 100.00 V
 *   0x76  voltage-measurement gain       (CFG 0x030/0x032), apply 30 V / 100 V
 *   0x77  VIN-measurement gain           (CFG 0x034/0x036), apply 30 V / 60 V
 *   0x79  store present I-DAC factor into the slot for the present set I
 */
#include "app.h"

volatile uint32_t g_meter;

/* ------------------------------------------------------------------------- */
/* Reference meter on USART1                                                  */
/* ------------------------------------------------------------------------- */
static volatile uint8_t s_m_buf[8];
static volatile uint8_t s_m_idx;

void cal_rx_byte_isr(uint8_t b)
{
    if (s_m_idx < sizeof s_m_buf) s_m_buf[s_m_idx] = b;
    if (s_m_idx < 0xFFu) s_m_idx++;
}

static uint32_t digit(uint8_t c) { return (c >= '0' && c <= '9') ? (uint32_t)(c - '0') : 0u; }

void cal_poll_meter(void)
{
    if (s_m_idx < 8u) return;
    s_m_idx = 0;
    if (s_m_buf[2] == '.' && s_m_buf[7] == '\n') {
        g_meter = digit(s_m_buf[0]) * 10000u + digit(s_m_buf[1]) * 1000u +
                  digit(s_m_buf[3]) * 100u + digit(s_m_buf[4]) * 10u + digit(s_m_buf[5]);
    }
}

/* ------------------------------------------------------------------------- */
/* 0x73 / 0x74 / 0x76 / 0x77 and live gain registers (every 10 ms)            */
/* ------------------------------------------------------------------------- */
static struct { uint16_t target, cnt, div; uint8_t started; } s_cv;

static void step_toward(volatile uint16_t *gain, uint32_t meas, uint32_t target)
{
    if (meas > target) (*gain)--;
    else if (meas < target) (*gain)++;
}

void cal_voltage_tick(void)
{
    uint32_t v = g_st.v;
    uint16_t cmd = g_cmd;

    if (cmd != CMD_CAL_VDAC_LO && cmd != CMD_CAL_VDAC_HI) s_cv.started = 0;

    if (cmd == CMD_CAL_VDAC_LO || cmd == CMD_CAL_VDAC_HI) {
        bool hi = (cmd == CMD_CAL_VDAC_HI);
        if (!s_cv.started) {
            s_cv.target = hi ? 10000u : 3000u;
            s_cv.started = 1;
            CFG(CFG_MODE) = MODE_CV;
            g_st.on = 1;
            g_st.set_v = s_cv.target;
            if (hi) {
                g_vdac_scale = CFG(CFG_VDAC_SCALE_HI);
                if (g_vdac_scale >= 60000u) g_vdac_scale = (uint16_t)(CFG(CFG_VDAC_SCALE_LO) * 2u);
            } else {
                g_vdac_scale = CFG(CFG_VDAC_SCALE_LO);
            }
        }
        if (v != s_cv.target) {
            if (v < s_cv.target) g_vdac_scale--; else g_vdac_scale++;
        }
        if (s_cv.cnt++ > 9u && (v == s_cv.target || v == s_cv.target + 1u)) {
            s_cv.started = 0; s_cv.cnt = 0;
            CFG(CFG_MODE) = MODE_CC;
            g_st.on = 0;
            CFG(hi ? CFG_VDAC_SCALE_HI : CFG_VDAC_SCALE_LO) = g_vdac_scale;
            g_cmd = CMD_SAVE;
        }
    }

    if (cmd == CMD_CAL_VMEAS) {
        if (s_cv.div++ > 9u) {
            s_cv.div = 0;
            bool lo = v < 3851u;
            uint16_t target = lo ? 3000u : 10000u;
            volatile uint16_t *gain = &CFG(lo ? CFG_V_GAIN_LO : CFG_V_GAIN_HI);
            step_toward(gain, v, target);
            g_v_gain_reg = *gain;
            if (s_cv.cnt++ > 9u && v + 1u >= target && v <= target + 1u) g_cmd = CMD_SAVE;
        }
    } else if (g_v_gain_reg != 0u) {
        /* holding register 10 adjusts the active voltage gain live */
        if (g_v_gain_reg < 3851u) CFG(CFG_V_GAIN_LO) = g_v_gain_reg;
        else                      CFG(CFG_V_GAIN_HI) = g_v_gain_reg;
    }

    /* holding register 9 adjusts the active VIN gain live */
    if (g_vin_gain_reg != 0u) {
        if (g_vin_gain_reg < CFG(CFG_VIN_GAIN_LO) + 1000u) CFG(CFG_VIN_GAIN_LO) = g_vin_gain_reg;
        else                                               CFG(CFG_VIN_GAIN_HI) = g_vin_gain_reg;
    }

    if (cmd == CMD_CAL_VINMEAS && s_cv.div++ > 9u) {
        s_cv.div = 0;
        uint32_t vin = g_st.vin;
        bool lo = vin < 50001u;
        uint32_t target = lo ? 30000u : 60000u;
        volatile uint16_t *gain = &CFG(lo ? CFG_VIN_GAIN_LO : CFG_VIN_GAIN_HI);
        step_toward(gain, vin, target);
        g_vin_gain_reg = *gain;
        if (s_cv.cnt++ > 9u && vin > target && vin <= target + 5u) g_cmd = CMD_SAVE;
    }
}

/* ------------------------------------------------------------------------- */
/* 0x79: store the active I-DAC factor for the present current setpoint      */
/* ------------------------------------------------------------------------- */
void cal_store_idac(void)
{
    uint32_t s = g_st.set_i;
    uint16_t off = (s == 0u || s > CFG(CFG_IMAX)) ? CFG_IDAC_DEFAULT : idac_slot_for(s);
    if (off) CFG(off) = g_idac_cal;
}

/* ------------------------------------------------------------------------- */
/* 0x6F: current-DAC table. The load sinks set_i = step; the I-DAC factor is */
/* trimmed until the reference meter agrees, then stored in the step's slot. */
/* ------------------------------------------------------------------------- */
typedef struct { uint32_t step; uint16_t slot; uint16_t win; } idac_step_t;
static const idac_step_t s_idac_steps[] = {
    {10, 0x12C, 1}, {20, 0x12E, 2}, {30, 0x130, 3}, {40, 0x132, 4}, {50, 0x134, 5},
    {60, 0x136, 5}, {70, 0x138, 5}, {80, 0x13A, 5}, {90, 0x13C, 5}, {100, 0x13E, 5},
    {200, 0x140, 10}, {300, 0x142, 10}, {400, 0x144, 10}, {500, 0x146, 10},
    {600, 0x148, 10}, {700, 0x14A, 10}, {800, 0x14C, 10}, {900, 0x14E, 10},
    {1000, 0x150, 10}, {2000, 0x152, 10}, {3000, 0x154, 10}, {4000, 0x156, 10},
    {5500, 0x158, 10}, {6000, 0x15A, 20}, {7000, 0x15C, 20}, {8000, 0x15E, 20},
    {9000, 0x160, 20}, {10000, 0x162, 20}, {20000, 0x176, 20}, {30000, 0x18A, 30},
    {40000, 0x19E, 40}, {50000, 0x1B2, 50}, {60000, 0x1C6, 60},
};
#define N_IDAC_STEPS (sizeof s_idac_steps / sizeof s_idac_steps[0])

static struct { uint8_t started, psu_limit; uint16_t c_adj, c_step, c_v; } s_idc;

void cal_idac_tick(void)
{
    static uint16_t prev;
    uint16_t was = prev;
    prev = g_cmd;

    if (g_cmd == CMD_STORE_IDAC) { cal_store_idac(); g_cmd = CMD_SAVE; return; }
    if (g_cmd != CMD_CAL_IDAC) return;
    if (was != CMD_CAL_IDAC) { s_idc.started = 0; s_idc.psu_limit = 0; }

    if (!s_idc.started) {
        s_idc.started = 1;
        g_cal_step = 10;
        g_st.on = 1;
        g_idac_cal = CFG(0x12C);
    }
    usart1_puts("MEASure:CURRent?\r\n");

    if (g_cal_step >= 30000u) {          /* beyond the 20 A bench supply */
        if (g_cal_step == 30000u && g_st.v > 296u && g_st.v < 304u) {
            if (s_idc.c_v++ > 299u) g_st.on = 1;
        } else {
            s_idc.c_v = 0;
        }
        if (g_meter >= 20099u && g_meter <= 20101u) { s_idc.psu_limit = 1; g_st.on = 0; meas_voltage(); }
        else if (!g_st.on) meas_voltage();
        if (s_idc.psu_limit) g_meter = g_st.i;
    }

    uint32_t s = g_st.set_i, m = g_meter;
    uint16_t period;
    if (s < 5500u) period = (m > s - 25u && m < s + 25u) ? 10u : 5u;
    else           period = (m > s - s / 500u && m < s + s / 500u) ? 50u : 5u;

    if (s_idc.c_adj++ >= period) {
        s_idc.c_adj = 0;
        if (g_st.on && m != 0u && s != m) {
            if (s < m) g_idac_cal--; else g_idac_cal++;
        }
    }

    if (s_idc.c_step++ >= 5u) {
        s_idc.c_step = 0;
        g_st.set_i = g_cal_step;
        for (uint32_t k = 0; k < N_IDAC_STEPS; k++) {
            const idac_step_t *e = &s_idac_steps[k];
            if (e->step != g_cal_step) continue;
            if (m + e->win >= e->step && m <= e->step) {
                CFG(e->slot) = g_idac_cal;
                if (k + 1u < N_IDAC_STEPS) {
                    g_cal_step = s_idac_steps[k + 1u].step;
                    if (e->step < 6000u) g_idac_cal = CFG(s_idac_steps[k + 1u].slot);
                } else {
                    g_cal_step = 70000u;
                }
            }
            break;
        }
    }

    uint32_t imax = CFG(CFG_IMAX);
    if (imax < g_cal_step) {            /* done: pick default factor for IMAX */
        uint16_t off = 0;
        if (imax >= 3000u)  off = 0x154;
        if (imax >= 10000u) off = 0x162;
        if (imax >= 20000u) off = 0x176;
        if (imax >= 30000u) off = 0x18A;
        if (imax >= 40000u) off = 0x19E;
        if (imax >= 50000u) off = 0x1B2;
        if (imax >= 60000u) off = 0x1C6;
        if (off) { CFG(CFG_IDAC_DEFAULT) = CFG(off); g_idac_cal = CFG(off); }
        s_idc.started = 0; s_idc.psu_limit = 0;
        g_cal_step = 0;
        g_st.set_i = 0;
        g_cmd = CMD_SAVE;
    }
}

/* ------------------------------------------------------------------------- */
/* 0x6E: current-measurement gain tables. The bench supply forces current    */
/* into the shorted load (CV, Vset = 0); the measurement gain is trimmed     */
/* until it matches the supply's reading.                                    */
/* ------------------------------------------------------------------------- */
static const uint16_t s_lo_seq[15] = {
    0x50, 0x52, 0x54, 0x56, 0x58, 0x5A, 0x5C, 0x5E, 0x60, 0x62, 0x64, 0x66, 0x68, 0x6A, 0x6C,
};
static const uint16_t s_hi_seq[27] = {
    0x6E, 0x70, 0x72, 0x74, 0x76, 0x78, 0x7A, 0x7C, 0x7E, 0x80, 0x82, 0x84, 0x86, 0x88, 0x8A,
    0x94, 0x9E, 0xA8, 0xB2, 0xBC, 0xC6, 0xD0, 0xDA, 0xEE, 0x102, 0x116, 0x128,
};

typedef struct { uint32_t step; uint16_t slot; uint8_t fill; const char *next; } imeas_step_t;
static const imeas_step_t s_imeas_steps[] = {
    {50, 0x50, 0, "CURR 0.1\r\n"},   {100, 0x52, 0, "CURR 0.2\r\n"},
    {200, 0x54, 0, "CURR 0.3\r\n"},  {300, 0x56, 0, "CURR 0.4\r\n"},
    {400, 0x58, 0, "CURR 0.5\r\n"},  {500, 0x5A, 0, "CURR 0.6\r\n"},
    {600, 0x5C, 0, "CURR 0.7\r\n"},  {700, 0x5E, 0, "CURR 0.8\r\n"},
    {800, 0x60, 0, "CURR 0.9\r\n"},  {900, 0x62, 0, "CURR 1.0\r\n"},
    {1000, 0x64, 0, "CURR 2.0\r\n"}, {2000, 0x66, 0, "CURR 3.0\r\n"},
    {3000, 0x68, 1, "CURR 4.0\r\n"}, {4000, 0x6A, 0, "CURR 5.5\r\n"},
    {5500, 0x6C, 0, "CURR 6.0\r\n"}, {6000, 0x6E, 0, "CURR 7.0\r\n"},
    {7000, 0x70, 0, "CURR 8.0\r\n"}, {8000, 0x72, 0, "CURR 9.0\r\n"},
    {9000, 0x74, 0, "CURR 10.0\r\n"},{10000, 0x76, 1, "CURR 11.0\r\n"},
    {11000, 0x78, 0, "CURR 12.0\r\n"},{12000, 0x7A, 0, "CURR 13.0\r\n"},
    {13000, 0x7C, 0, "CURR 14.0\r\n"},{14000, 0x7E, 0, "CURR 15.0\r\n"},
    {15000, 0x80, 0, "CURR 16.0\r\n"},{16000, 0x82, 0, "CURR 17.0\r\n"},
    {17000, 0x84, 0, "CURR 18.0\r\n"},{18000, 0x86, 0, "CURR 19.0\r\n"},
    {19000, 0x88, 0, "CURR 20.0\r\n"},{20000, 0x8A, 1, "CURR 25.0\r\n"},
    {25000, 0x94, 0, "CURR 30.0\r\n"},{30000, 0x9E, 1, "CURR 35.0\r\n"},
    {35000, 0xA8, 0, "CURR 40.0\r\n"},{40000, 0xB2, 1, "CURR 45.0\r\n"},
    {45000, 0xBC, 0, "CURR 50.0\r\n"},{50000, 0xC6, 1, "CURR 55.0\r\n"},
    {55000, 0xD0, 0, "CURR 60.0\r\n"},{60000, 0xDA, 1, "CURR 65.0\r\n"},
};
#define N_IMEAS_STEPS (sizeof s_imeas_steps / sizeof s_imeas_steps[0])

static struct { uint16_t tol, c_adj, c_step; } s_imc;

static void imeas_store(uint16_t slot, bool fill)
{
    const uint16_t *seq = (slot <= 0x6Cu) ? s_lo_seq : s_hi_seq;
    uint32_t n = (slot <= 0x6Cu) ? 15u : 27u;
    bool on = false;
    for (uint32_t k = 0; k < n; k++) {
        if (seq[k] == slot) on = true;
        if (on) {
            CFG(seq[k]) = (uint16_t)g_imeas_gain;
            if (!fill) break;
        }
    }
}

void cal_imeas_tick(void)
{
    static uint16_t prev;
    uint16_t was = prev;
    prev = g_cmd;

    if (g_cmd != CMD_CAL_IMEAS) return;
    if (was != CMD_CAL_IMEAS) g_cal_step = 0;      /* (re)start the sequence */

    uint32_t imax = CFG(CFG_IMAX);
    if (g_cal_step == 0u) {
        usart1_puts("VOLT 5.0\r\n");
        usart1_puts("CURR 0.05\r\n");
        s_imc.tol = (uint16_t)(imax / 10000u);
        CFG(CFG_MODE) = MODE_CV;
        g_st.on = 1;
        g_st.set_v = 0;                 /* CV at 0 V: CC DAC full -> short */
        g_cal_step = 50;
        g_imeas_gain = CFG(0x50);
    }
    if (imax < g_cal_step) {
        g_cal_step = 0;
        g_st.on = 0;
        CFG(CFG_MODE) = MODE_CC;
        g_cmd = CMD_SAVE;
        return;
    }

    usart1_puts("MEASure:CURRent?\r\n");

    if (s_imc.c_adj++ >= 10u) {
        s_imc.c_adj = 0;
        if (g_st.on && g_meter != g_st.i) {
            if (g_st.i < g_meter) g_imeas_gain++; else g_imeas_gain--;
        }
    }

    if (s_imc.c_step++ < 50u) return;
    s_imc.c_step = 0;

    if (g_cal_step == 50u) meas_voltage();
    if (g_cal_step == 25000u && g_meter >= 20099u && g_meter <= 20101u) {
        g_cal_step = 0xFFFF;            /* supply limited at 20 A: stop */
        return;
    }

    for (uint32_t k = 0; k < N_IMEAS_STEPS; k++) {
        const imeas_step_t *e = &s_imeas_steps[k];
        if (e->step != g_cal_step) continue;
        uint32_t tol = (e->step <= 4000u) ? 1u : (e->step <= 20000u) ? s_imc.tol : e->step / 1000u;
        if (g_st.i + tol >= e->step && g_st.i <= e->step) {
            imeas_store(e->slot, e->fill);
            if (e->step == 5500u) g_imeas_gain = CFG(0x6E);
            g_cal_step = (k + 1u < N_IMEAS_STEPS) ? s_imeas_steps[k + 1u].step : 61000u;
            usart1_puts(e->next);
        }
        break;
    }
}
