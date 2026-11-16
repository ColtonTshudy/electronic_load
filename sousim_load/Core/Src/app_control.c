/**
 * @file  app_control.c
 * @brief Load control: 10 kHz tick, DAC setpoints (MOSFET drive), operating
 *        modes, list/preset modes, protection, fan and buzzer outputs.
 */
#include "app.h"

load_state_t      g_st;
volatile uint16_t g_cmd;
volatile uint16_t g_last_set[4];
volatile uint16_t g_vin_gain_reg, g_v_gain_reg, g_idac_cal, g_vdac_scale, g_vdac_base;
volatile uint32_t g_imeas_gain;
volatile uint16_t g_iscale;
volatile uint16_t g_pdiv = 1;
volatile uint32_t g_cal_step;
volatile uint8_t  g_dac_settled, g_fan_speed, g_alarm;
volatile uint8_t  g_flag_10ms, g_flag_100ms, g_flag_500ms, g_isr_alive;

#define DAC_MAX 4095u

/* ------------------------------------------------------------------------- */
/* Current DAC calibration slot lookup (shared with calibration)             */
/* ------------------------------------------------------------------------- */
static const uint16_t s_idac_lim[32] = {
    10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 200, 300, 400, 500, 600, 700, 800,
    900, 1000, 2000, 3000, 4000, 5500, 6000, 7000, 8000, 9000, 10000, 20000,
    30000, 40000, 50000,
};

uint16_t idac_slot_for(uint32_t ma)
{
    for (uint32_t n = 0; n < 32u; n++) {
        if (ma <= s_idac_lim[n]) {
            if (n < 28u) return (uint16_t)(CFG_IDAC_TBL + 2u * n);
            return (uint16_t)(0x176u + 0x14u * (n - 28u));   /* 20k..50k */
        }
    }
    return (ma <= 60000u) ? 0x1C6u : 0u;
}

/* ------------------------------------------------------------------------- */
/* Current DAC (CC loop)                                                     */
/* ------------------------------------------------------------------------- */
static volatile struct {
    uint32_t ramp;        /* slew-limited current setpoint (mA)   */
    uint32_t acc;         /* same, in RAMP_RES units per mA        */
} s_dac;

/* Set while the load is on, the host has set I = 0, and the current is still
 * ramping down to 0 (the range and DAC factor of the last setpoint are kept
 * until it gets there). Switching the output off is always immediate. */
static volatile uint8_t s_stopping;

/* ---- current ramp --------------------------------------------------------
 * Rates are in mA/s. The accumulator counts 1e-4 mA, so a rate in mA/s is
 * exactly its step per 0.1 ms control tick: no rounding, no minimum step.
 * The last step is clamped to the target. 0 = no limit (jump in one tick). */
#define RAMP_RES        10000u
#define RAMP_MAX_MAS    1000000000u     /* 1,000,000 A/s */
#define RAMP_TARGET_MAX 65535u          /* mA, keeps target * RAMP_RES in 32 bits */
#define LEGACY_MAS      5000u           /* legacy unit: 1 mA per 0.2 ms = 5 A/s */

static volatile uint32_t s_ramp_up, s_ramp_dn;      /* mA/s */
static uint16_t s_ramp_hi_w[2];                     /* high words latched until the low word */
volatile uint8_t g_cfg_save_req;

static void ramp_set(uint32_t ma)
{
    if (ma > RAMP_TARGET_MAX) ma = RAMP_TARGET_MAX;
    s_dac.ramp = ma;
    s_dac.acc  = ma * RAMP_RES;
}

static void ramp_toward(uint32_t target)
{
    if (target > RAMP_TARGET_MAX) target = RAMP_TARGET_MAX;
    uint32_t t = target * RAMP_RES, a = s_dac.acc;

    if (a < t) {
        uint32_t r = s_ramp_up;
        a = (r == 0u || r >= t - a) ? t : a + r;
    } else if (a > t) {
        uint32_t r = s_ramp_dn;
        a = (r == 0u || r >= a - t) ? t : a - r;
    } else {
        return;
    }
    s_dac.acc  = a;
    s_dac.ramp = a / RAMP_RES;
}

static uint32_t cfg32(uint16_t off)
{
    return (uint32_t)g_cfg[off / 2u] | ((uint32_t)g_cfg[off / 2u + 1u] << 16);
}

/* store a rate (mA/s) in RAM, the config copy and the legacy u16 field */
static void ramp_store(bool up, uint32_t mas)
{
    uint16_t off = up ? CFG_RAMP_UP : CFG_RAMP_DOWN;
    if (mas > RAMP_MAX_MAS) mas = RAMP_MAX_MAS;
    g_cfg[off / 2u]      = (uint16_t)mas;
    g_cfg[off / 2u + 1u] = (uint16_t)(mas >> 16);

    uint32_t step = (mas + LEGACY_MAS / 2u) / LEGACY_MAS;
    if (mas == 0u || step > 2750u) step = 2750u;
    if (step == 0u) step = 1u;
    CFG(up ? CFG_SLEW_UP : CFG_SLEW_DOWN) = (uint16_t)step;

    if (up) s_ramp_up = mas; else s_ramp_dn = mas;
}

/* boot: use the stored rates; a blank config (first boot of this version)
 * takes them from the legacy step fields */
static void ramp_load(void)
{
    for (uint32_t k = 0; k < 2u; k++) {
        bool up = (k == 0u);
        uint32_t mas = cfg32(up ? CFG_RAMP_UP : CFG_RAMP_DOWN);
        if (mas == 0xFFFFFFFFu) {
            uint16_t step = CFG(up ? CFG_SLEW_UP : CFG_SLEW_DOWN);
            mas = (step >= 1u && step <= 2750u) ? step * LEGACY_MAS : (up ? 100000u : 500000u);
        }
        ramp_store(up, mas);
    }
}

void ctl_ramp_from_legacy(bool up)
{
    ramp_store(up, (uint32_t)CFG(up ? CFG_SLEW_UP : CFG_SLEW_DOWN) * LEGACY_MAS);
}

bool ramp_reg_read(uint16_t a, uint16_t *v)
{
    switch (a) {
    case 0x1C0: *v = (uint16_t)(s_ramp_up >> 16); return true;
    case 0x1C1: *v = (uint16_t)s_ramp_up;         return true;
    case 0x1C2: *v = (uint16_t)(s_ramp_dn >> 16); return true;
    case 0x1C3: *v = (uint16_t)s_ramp_dn;         return true;
    default:    return false;
    }
}

bool ramp_reg_writable(uint16_t a)
{
    return a >= RAMP_REG_FIRST && a <= RAMP_REG_LAST;
}

void ramp_reg_write(uint16_t a, uint16_t v)
{
    switch (a) {
    case 0x1C0: s_ramp_hi_w[0] = v; return;
    case 0x1C2: s_ramp_hi_w[1] = v; return;
    case 0x1C1: ramp_store(true,  ((uint32_t)s_ramp_hi_w[0] << 16) | v); s_ramp_hi_w[0] = 0; break;
    case 0x1C3: ramp_store(false, ((uint32_t)s_ramp_hi_w[1] << 16) | v); s_ramp_hi_w[1] = 0; break;
    default:    return;
    }
    g_cfg_save_req = 1;
}

/* Host (Modbus) writes of on/off and set I. Off is immediate. Setting I to 0
 * while on ramps the current down at the ramp-down rate like any other lower
 * setpoint; a new nonzero setpoint ramps from wherever the current is. */
void ctl_host_set(uint16_t on, uint16_t set_i)
{
    if (on != 1u) {                     /* output off: jump to 0, never ramped */
        g_st.on = on;
        s_stopping = 0;
        g_st.set_i = set_i;
        return;
    }
    if (set_i != 0u) {
        g_st.set_i = set_i;
        g_st.on = on;
        s_stopping = 0;                 /* after set_i, so the ramp is never reset in between */
        return;
    }
    bool ramp_down = (g_st.on == 1u) && s_ramp_dn != 0u && s_dac.acc != 0u && !g_lc.raw_mode &&
                     g_cmd != CMD_CAL_IMEAS && g_cmd != CMD_CAL_IDAC;
    if (ramp_down) s_stopping = 1;      /* before set_i = 0, for the same reason */
    g_st.set_i = 0;
    g_st.on = on;
}

/* immediate off (battery cut-off) */
void ctl_force_off(void)
{
    g_st.on = 0;
    s_stopping = 0;
}

static bool irange_is_high(void) { return pin_read(PIN_IRANGE_PORT, PIN_IRANGE); }
static void irange(bool hi)
{
    if (hi) pin_set(PIN_IRANGE_PORT, PIN_IRANGE); else pin_reset(PIN_IRANGE_PORT, PIN_IRANGE);
}

static uint32_t idac_value(bool low_scale)
{
    uint64_t v = (uint64_t)g_idac_cal * g_iscale * s_dac.ramp * (low_scale ? 10u : 1u);
    uint32_t den = (uint32_t)CFG(CFG_VREF) * 100u;
    if (den == 0u) return 0;
    return (uint32_t)(((uint64_t)DAC_MAX * (v / 10000u)) / den);
}

static void set_current_dac(uint32_t iset)
{
    uint32_t out = 0;
    bool cal = (g_cmd == CMD_CAL_IMEAS || g_cmd == CMD_CAL_IDAC);
    if (cal || g_st.on != 1u) s_stopping = 0;   /* any off path: immediate */

    /* While ramping down to 0 after set I = 0, the range and DAC factor of
     * the last setpoint stay in force. */
    bool stopping = s_stopping;
    uint32_t si = stopping ? g_last_set[1] : g_st.set_i;

    if (cal) {
        if (g_cmd == CMD_CAL_IDAC) ramp_set(iset);
        irange(g_cal_step > 5500u);
    } else if (!stopping) {
        if (!g_st.on && g_st.i == 0u && g_st.set_i <= 5500u && irange_is_high())
            irange(false);

        uint32_t s = g_st.set_i;
        if (s != g_last_set[1]) {
            if (s == 0u) {
                g_idac_cal = CFG(CFG_IDAC_DEFAULT);
            } else {
                uint16_t slot = idac_slot_for(s);
                irange(s > 5500u);
                if (slot) g_idac_cal = CFG(slot);
                g_last_set[1] = (uint16_t)s;
            }
        }
    }

    /* advance the slew-limited setpoint */
    bool drive = true;
    if (g_cmd != CMD_CAL_IDAC) {
        if (stopping) drive = (s_dac.acc != 0u) && g_st.v > CFG(CFG_V_ZERO);
        else          drive = (g_st.on == 1u && g_st.set_i != 0u && g_st.v > CFG(CFG_V_ZERO));
        if (drive) ramp_toward(stopping ? 0u : iset);
        else       { ramp_set(0); s_stopping = 0; }
    }

    /* DAC code from it, per current range */
    bool hi = irange_is_high();
    uint32_t lch = hi ? LC_IDAC_HI : LC_IDAC_LO;
    if (!drive || (si > 5500u && !hi)) {
        out = 0;                                   /* off, or range not switched yet */
    } else if (lc_valid(lch)) {
        out = lc_apply_u(lch, (int32_t)s_dac.ramp);
    } else {
        out = idac_value(!hi);
    }

    g_dac_settled = (!stopping && iset != 0u && s_dac.ramp == iset);
    if (out > DAC_MAX) out = DAC_MAX;
    if (CFG(CFG_MODE) == MODE_CV && g_st.set_v == 0u) out = DAC_MAX;
    dac_set_cc((uint16_t)out);
}

/* ------------------------------------------------------------------------- */
/* Voltage DAC (CV loop threshold)                                           */
/* ------------------------------------------------------------------------- */
static void set_voltage_dac(uint32_t vset)
{
    uint32_t out = 0;
    uint32_t den = (uint32_t)CFG(CFG_VREF) * 100u;
    uint32_t lch = g_vrange_hi ? LC_VDAC_HI : LC_VDAC_LO;
    if (lc_valid(lch))
        out = lc_apply_u(lch, (int32_t)vset);
    else if (g_vdac_scale != 0u && den != 0u)
        out = (uint32_t)(((uint64_t)DAC_MAX * ((100000u * vset) / g_vdac_scale)) / den);
    if (out > DAC_MAX) out = DAC_MAX;

    uint16_t mode = CFG(CFG_MODE);
    if ((mode == MODE_CC || mode == MODE_CP || mode == MODE_CR) && g_st.set_v == 0u)
        out = DAC_MAX;
    dac_set_cv((uint16_t)out);
}

/* ------------------------------------------------------------------------- */
/* List mode (modes 9 and 10): step through presets, duration in 0.1 ms      */
/* ------------------------------------------------------------------------- */
static uint16_t s_list_cnt;

static void apply_preset_value(uint32_t val)
{
    switch (CFG(CFG_PRESET_TYPE)) {
    case 0: g_st.set_v = val; break;
    case 1: g_st.set_i = val; break;
    case 2: g_st.set_p = val; break;
    case 3: g_st.set_r = val; break;
    default: break;
    }
}

static void list_tick(void)
{
    uint16_t mode = CFG(CFG_MODE);
    if (mode != MODE_LIST2 && mode != MODE_LIST10) return;

    if (g_st.on != 1u) {
        g_st.list_step = MODE_PRESET1;
        g_st.list_time = CFG(CFG_PRESET_AUX);
        return;
    }
    uint32_t steps = (mode == MODE_LIST2) ? 2u : 10u;
    uint32_t idx = g_st.list_step - MODE_PRESET1;
    if (idx >= steps) { idx = 0; g_st.list_step = MODE_PRESET1; s_list_cnt = 0; }

    uint16_t dur = CFG(CFG_PRESET_AUX + 2u * idx);
    apply_preset_value(CFG(CFG_PRESET_VAL + 2u * idx));
    g_st.list_time = dur;
    if (s_list_cnt++ >= dur) {
        s_list_cnt = 0;
        g_st.list_step = MODE_PRESET1 + ((idx + 1u) % steps);
    }
}

/* ------------------------------------------------------------------------- */
/* Protection (1 ms): OVP, OCP, OPP, OTP                                       */
/* ------------------------------------------------------------------------- */
static uint16_t s_prot_cnt, s_prot_out_cnt, s_prot_clr_cnt;
static uint8_t  s_prot_latched;

static void protection_1ms(void)
{
    if (g_cmd == CMD_CAL_IMEAS || g_cmd == CMD_CAL_IDAC) return;

    uint32_t imax = CFG(CFG_IMAX), pmax = CFG(CFG_PMAX);
    uint32_t ocp_margin = (imax < 5501u) ? 600u : 3000u;
    bool fault = g_st.v > CFG(CFG_VMAX) + 300u
              || g_st.i > imax + ocp_margin
              || g_st.p > pmax + pmax / 10u
              || g_st.t_ntc1 > 85u || g_st.t_ntc2 > 85u;

    if (fault) {
        if (s_prot_cnt++ >= 30u) {          /* 30 ms debounce, then trip */
            s_prot_cnt = 0;
            if (!s_prot_latched) g_prot_evt = EVT_PROT_TRIP;
            g_alarm = 1; s_prot_latched = 1;
            g_st.on = 0; s_stopping = 0;    /* trip: immediate, not ramped */
        }
        if (s_prot_out_cnt++ >= 100u) {
            s_prot_out_cnt = 0;
            pin_set(PIN_AUX_OUT_PORT, PIN_AUX_OUT);
        }
    } else {
        s_prot_cnt = 0; s_prot_out_cnt = 0;
        if (s_prot_latched && s_prot_clr_cnt++ >= 3000u) {   /* 3 s clear */
            s_prot_clr_cnt = 0;
            g_prot_evt = EVT_PROT_CLEAR;
            g_alarm = 0; s_prot_latched = 0;
            pin_reset(PIN_AUX_OUT_PORT, PIN_AUX_OUT);
        }
    }
}

/* ------------------------------------------------------------------------- */
/* 10 kHz tick (TIM7)                                                         */
/* ------------------------------------------------------------------------- */
static struct {
    uint16_t c1ms, c10ms, c100ms, c500ms, c24ms, half;
    uint16_t beep_seq, beep_ph, fan_ph;
    uint8_t  buzz;
} s_tk;

void ctl_dac_state(uint32_t *ramp, uint16_t *stopping)
{
    *ramp = s_dac.ramp;
    *stopping = s_stopping;
}

void ctl_tick_isr(void)
{
    g_tick++;
    mb_tick_isr();
    list_tick();

    if (g_lc.raw_mode) s_stopping = 0;
    if (g_st.on == 1u) {
        pin_reset(PIN_LOAD_INH_PORT, PIN_LOAD_INH);
        pin_reset(PIN_AUX_OUT_PORT, PIN_AUX_OUT);
    } else {
        pin_set(PIN_LOAD_INH_PORT, PIN_LOAD_INH);
    }

    if (g_lc.raw_mode) {
        /* calibration tool drives the DACs and the current range directly */
        if (g_lc.force_ir) irange(g_lc.force_ir == 2u);
        dac_set_cv(g_lc.raw_cv);
        dac_set_cc(g_st.on == 1u ? g_lc.raw_cc : 0u);
    } else {
        if (g_st.set_v > CFG(CFG_VMAX)) g_st.set_v = CFG(CFG_VMAX);
        if (g_st.set_i > CFG(CFG_IMAX)) g_st.set_i = CFG(CFG_IMAX);
        if (g_st.set_p > CFG(CFG_PMAX)) g_st.set_p = CFG(CFG_PMAX);

        set_voltage_dac(g_st.set_v);

        uint32_t iset = g_st.set_i;
        uint32_t pmax = CFG(CFG_PMAX);
        if (g_pdiv && pmax + pmax / 4u <= ((iset * g_st.v) / 1000u) / g_pdiv) {
            iset = 0; g_st.set_i = 0;           /* hard power limit: immediate, not ramped */
            s_stopping = 0;
        }
        set_current_dac(iset);
    }

    stream_fast_isr();                  /* after the DACs: samples this tick's codes */

    /* timebase */
    if (++s_tk.c1ms >= 10u) {
        s_tk.c1ms = 0;
        lc_tick_1ms_isr();
        protection_1ms();
        if (++s_tk.c10ms >= 10u)  { s_tk.c10ms = 0;  g_flag_10ms = 1; }
        if (++s_tk.c100ms >= 100u){ s_tk.c100ms = 0; g_flag_100ms = 1; g_isr_alive = 1; }
        if (++s_tk.c500ms >= 500u){
            s_tk.c500ms = 0; g_flag_500ms = 1;
            if (++s_tk.half >= 2u) { s_tk.half = 0; g_st.seconds++; }
        }
        if (++s_tk.c24ms >= 24u) {             /* beep cadence */
            s_tk.c24ms = 0;
            if (g_cmd == CMD_BEEP || g_alarm) {
                s_tk.beep_seq++;
                if (s_tk.beep_seq > 5u) { s_tk.buzz = 0; if (s_tk.beep_seq > 9u) s_tk.beep_seq = 0; }
                else s_tk.buzz = 1;
            } else {
                s_tk.buzz = 0;
            }
        }
    }

    /* buzzer: 1 kHz, 20 % duty */
    if (s_tk.buzz) {
        if (++s_tk.beep_ph >= 10u) s_tk.beep_ph = 0;
        if (s_tk.beep_ph <= 1u) pin_set(PIN_BUZ_PORT, PIN_BUZ);
        else                    pin_reset(PIN_BUZ_PORT, PIN_BUZ);
    } else {
        pin_reset(PIN_BUZ_PORT, PIN_BUZ);
    }

    /* fan: 50 Hz software PWM, g_fan_speed of 200 */
    if (g_fan_speed) {
        if (++s_tk.fan_ph > 199u) s_tk.fan_ph = 0;
        if (s_tk.fan_ph <= g_fan_speed) pin_set(PIN_FAN_PORT, PIN_FAN);
        else                            pin_reset(PIN_FAN_PORT, PIN_FAN);
    } else {
        pin_reset(PIN_FAN_PORT, PIN_FAN);
    }
}

/* ------------------------------------------------------------------------- */
/* Mode control (10 ms)                                                      */
/* ------------------------------------------------------------------------- */
static uint8_t  s_preset_armed[10] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
static uint8_t  s_cpcr_direct = 1;     /* CP/CR computed as a current setpoint */
static uint16_t s_cpcr_cnt_a, s_cpcr_cnt_b;

static bool presets_all_armed(void)
{
    for (int n = 0; n < 10; n++) if (!s_preset_armed[n]) return false;
    return true;
}

static void presets_rearm_off(void)
{
    g_st.on = 0;
    for (int n = 0; n < 10; n++) s_preset_armed[n] = 1;
}

/* CP / CR regulation. When the voltage is too low the conversion is done
 * through the CV loop instead (setpoint = k / I). */
static void cp_cr(uint32_t mode, uint32_t k)
{
    uint32_t vth = CFG(CFG_V_ZERO) + 30u;
    uint32_t ith = CFG(CFG_IMAX) / 1000u + CFG(CFG_I_ZERO);
    uint32_t v = g_st.v, i = g_st.i;

    if (s_cpcr_direct && v > vth) {
        s_cpcr_cnt_a = 0; s_cpcr_cnt_b = 0;
        if (mode != MODE_CR2) g_st.set_v = 0;
        if (mode == MODE_CP) g_st.set_i = v ? k / v : 0u;
        else                 g_st.set_i = k ? (v * 100u) / k : 0u;
        return;
    }
    if (s_cpcr_cnt_a++ < 10u) return;
    s_cpcr_cnt_a = 0;
    if (i > ith) {
        s_cpcr_direct = 0;
        if (mode == MODE_CP) g_st.set_v = i ? k / i : 0u;
        else                 g_st.set_v = (k * i) / 100u;
        g_st.set_i = 0;
        return;
    }
    if (s_cpcr_cnt_b++ < 10u) return;
    s_cpcr_cnt_b = 0;
    if (v >= vth && i >= ith) return;
    s_cpcr_direct = 1;
}

static void preset_mode(uint32_t n)
{
    uint16_t type = CFG(CFG_PRESET_TYPE);
    uint16_t vmax = CFG(CFG_VMAX);
    uint16_t off_val = (uint16_t)(CFG_PRESET_VAL + 2u * n);
    uint16_t off_aux = (uint16_t)(CFG_PRESET_AUX + 2u * n);

    if (s_preset_armed[n]) {            /* entering this preset: load it */
        s_preset_armed[n] = 0;
        if (type <= 3u) {
            apply_preset_value(CFG(off_val));
            g_st.on = CFG(off_aux);
        }
    }
    for (uint32_t m = 0; m < 10u; m++) if (m != n) s_preset_armed[m] = 1;

    /* store back whichever setpoint the host changed and switch type */
    if (g_st.set_v != vmax) {
        CFG(off_val) = (uint16_t)g_st.set_v; CFG(off_aux) = (uint16_t)g_st.on;
        if (g_st.set_v != g_last_set[0]) {
            g_last_set[0] = (uint16_t)g_st.set_v;
            g_st.set_i = 0; g_st.set_p = 0; g_st.set_r = 0;
            CFG(CFG_PRESET_TYPE) = 0;
            return;
        }
    }
    if (g_st.set_i) {
        CFG(off_val) = (uint16_t)g_st.set_i; CFG(off_aux) = (uint16_t)g_st.on;
        if (g_st.set_i != g_last_set[1]) {
            g_last_set[1] = (uint16_t)g_st.set_i;
            g_st.set_p = 0; g_st.set_v = vmax; g_st.set_r = 0;
            CFG(CFG_PRESET_TYPE) = 1;
            return;
        }
    }
    if (g_st.set_p) {
        CFG(off_val) = (uint16_t)g_st.set_p; CFG(off_aux) = (uint16_t)g_st.on;
        if (g_st.set_p != g_last_set[2]) {
            g_last_set[2] = (uint16_t)g_st.set_p;
            g_st.set_v = vmax; g_st.set_i = 0; g_st.set_r = 0;
            CFG(CFG_PRESET_TYPE) = 2;
            return;
        }
    }
    if (g_st.set_r) {
        CFG(off_val) = (uint16_t)g_st.set_r; CFG(off_aux) = (uint16_t)g_st.on;
        if (g_st.set_r != g_last_set[3]) {
            g_last_set[3] = (uint16_t)g_st.set_r;
            g_st.set_v = vmax; g_st.set_i = 0; g_st.set_p = 0;
            CFG(CFG_PRESET_TYPE) = 3;
        }
    }
}

void ctl_mode_update(void)
{
    uint16_t mode = CFG(CFG_MODE);

    switch (mode) {
    case MODE_CV:
        g_st.set_i = 0;
        goto simple;
    case MODE_CC:
        g_st.set_v = 0;
        /* fallthrough */
    case MODE_CC_FAST:
    simple:
        g_st.set_p = 0; g_st.set_r = 0;
        if (!presets_all_armed()) presets_rearm_off();
        return;

    case MODE_CP:
        g_st.set_r = 0;
        if (!presets_all_armed()) { presets_rearm_off(); return; }
        if (!g_st.on) return;
        cp_cr(MODE_CP, (uint32_t)g_pdiv * 1000u * g_st.set_p);
        return;

    case MODE_CR:
    case MODE_CR2:
        g_st.set_p = 0;
        if (!presets_all_armed()) { presets_rearm_off(); return; }
        if (!g_st.on) return;
        cp_cr(mode, g_st.set_r);
        return;

    default:
        if (mode >= MODE_PRESET1 && mode <= MODE_PRESET10) preset_mode(mode - MODE_PRESET1);
        return;
    }
}

void ctl_init(void)
{
    g_st.list_step = MODE_PRESET1;
    ramp_load();
    g_idac_cal = CFG(CFG_IDAC_DEFAULT);
    g_imeas_gain = CFG(CFG_IMEAS_LO_TBL);
    g_last_set[0] = (uint16_t)g_st.set_v;
    g_last_set[1] = (uint16_t)g_st.set_i;
    g_last_set[2] = (uint16_t)g_st.set_p;
    g_last_set[3] = (uint16_t)g_st.set_r;

    uint16_t imax = CFG(CFG_IMAX);
    if (imax >= 1000u)  g_iscale = 100;
    if (imax >= 20000u) g_iscale = 50;
    if (imax >= 30000u) g_iscale = 33;
    if (imax >= 50000u) g_iscale = 25;
}
