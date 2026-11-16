/**
 * @file  app_main.c
 * @brief Application init and main loop: measurement, closed-loop trims,
 *        energy counters, fan curve and Modbus command handling.
 */
#include "app.h"

static uint16_t s_vtrim_cnt, s_itrim_cnt;
static uint32_t s_itrim_last_set;
static uint8_t  s_ui_tick;
static uint16_t s_addr_shadow;
static uint16_t s_armed_cmd;

/* ------------------------------------------------------------------------- */
/* Closed-loop trim of the voltage-DAC scale (CV accuracy)                    */
/* ------------------------------------------------------------------------- */
static void vtrim(void)
{
    uint32_t s = g_st.set_v;
    if (s != g_last_set[0]) {
        g_last_set[0] = (uint16_t)s;
        g_vdac_scale = g_vdac_base;
        s_vtrim_cnt = 0;
    }
    if (!g_st.on || CFG(CFG_MODE) == MODE_CC || g_lc.raw_mode) return;
    if (lc_valid(g_vrange_hi ? LC_VDAC_HI : LC_VDAC_LO)) return;   /* linear cal: no trim */

    uint32_t vmax = CFG(CFG_VMAX), v = g_st.v;
    if (v > vmax / 1000u && g_st.i > CFG(CFG_IMAX) / 1000u && v != s) {
        if (s_vtrim_cnt < g_vdac_base / 10u) {
            if (v > s + s / 1000u) { g_vdac_scale++; s_vtrim_cnt++; }
            if (v < s - s / 1000u) { g_vdac_scale--; s_vtrim_cnt++; }
        } else if (v > s + vmax / 500u || v < s - vmax / 500u) {
            s_vtrim_cnt = 0;
            g_vdac_scale = g_vdac_base;
        }
    }
}

/* ------------------------------------------------------------------------- */
/* Closed-loop trim of the current-DAC factor (CC accuracy)                   */
/* ------------------------------------------------------------------------- */
static void itrim(void)
{
    uint32_t s = g_st.set_i, i = g_st.i;

    if (s != s_itrim_last_set) { s_itrim_last_set = s; s_itrim_cnt = 0; }
    if (g_cmd == CMD_CAL_IMEAS || g_cmd == CMD_CAL_IDAC || g_lc.raw_mode) return;
    if (lc_valid(pin_read(PIN_IRANGE_PORT, PIN_IRANGE) ? LC_IDAC_HI : LC_IDAC_LO)) return;
    if (!g_st.on || g_st.v < 31u || i == s) return;
    if (s > CFG(CFG_ITRIM_LIMIT) || !g_dac_settled) return;

    uint32_t imax = CFG(CFG_IMAX), lim;
    if (imax < 5501u)                 lim = (s < 31u) ? CFG(0x12C) * 6u : g_idac_cal / 6u;
    else if (s < imax / 1000u + 120u) lim = CFG(0x12C) * 6u;
    else                              lim = g_idac_cal / 10u;

    if (s_itrim_cnt < lim) {
        if (i > s + s / 1000u) { g_idac_cal--; s_itrim_cnt++; }
        if (i < s - s / 1000u) { g_idac_cal++; s_itrim_cnt++; }
    } else if (i < s - imax / 500u || i > s + imax / 500u) {
        uint16_t slot = idac_slot_for(s);
        s_itrim_cnt = 0;
        if (slot) g_idac_cal = CFG(slot);
    }
}

/* ------------------------------------------------------------------------- */
/* Fan curve (evaluated every 5 s, or every loop while a command is active)   */
/* ------------------------------------------------------------------------- */
static void fan_update(void)
{
    uint32_t imax = CFG(CFG_IMAX), pmax = CFG(CFG_PMAX);
    uint32_t t1 = g_st.t_ntc1, t2 = g_st.t_ntc2, i = g_st.i, p = g_st.p;
    uint32_t tmax = (t1 > t2) ? t1 : t2;
    uint8_t f = g_fan_speed;

    if (g_cmd == CMD_NONE && t1 < 35u && t2 < 35u && i < imax - imax / 2u && p < pmax / 1000u) {
        f = 0;
    } else {
        if (i < imax - imax / 2u) f = 0;
        else if (f == 0u)         f = (uint8_t)((imax / 1000u) * 2u);

        /* temperature steps: 36..45 degC -> 20..200 */
        for (uint32_t t = 35; t <= 44u; t++) {
            if (tmax > t && (t >= 40u || i < 50000u)) f = (uint8_t)((t - 34u) * 20u);
        }
        /* no NTC fitted (both read 0): use power instead */
        if (t1 == 0u && t2 == 0u) {
            static const uint8_t div[10] = { 12, 11, 10, 9, 8, 7, 6, 5, 4, 3 };
            for (uint32_t k = 0; k < 10u; k++) {
                if (p > pmax / div[k] && (k >= 5u || i < 50000u)) f = (uint8_t)((k + 1u) * 20u);
            }
        }
        if (g_cmd == CMD_FAN_FULL) f = 200;
        else if (g_cmd == CMD_CAL_IMEAS || g_cmd == CMD_CAL_IDAC) f = 0;
    }
    g_fan_speed = f;
}

/* ------------------------------------------------------------------------- */
/* Command register handling                                                 */
/* ------------------------------------------------------------------------- */
typedef struct { uint16_t cmd, off, min, max; } param_cmd_t;
static const param_cmd_t s_params[] = {
    { CMD_SET_RANGE_FB,   CFG_RANGE_FALLBACK, 0, 0xFFFF },
    { CMD_SET_V_FROM_VIN, CFG_V_FROM_VIN,     0, 0xFFFF },
    { CMD_SET_CUTOFF,     CFG_CUTOFF_V,       0, 0xFFFF },
    { CMD_SET_POWER_RES,  CFG_POWER_RES,      0, 0xFFFF },
    { CMD_SET_SLEW_UP,    CFG_SLEW_UP,        1, 2750 },
    { CMD_SET_SLEW_DOWN,  CFG_SLEW_DOWN,      1, 2750 },
    { CMD_SET_BAUD,       CFG_MB_BAUD,        0, MB_BAUD_MAX_IDX },
    { CMD_SET_MB_ADDR,    CFG_MB_ADDR,        1, 0xFFFF },
};

/* "Parameter" commands: selecting the command loads the present setting into
 * the value register; writing a different value stores it and saves. */
static void param_command(void)
{
    for (uint32_t k = 0; k < sizeof s_params / sizeof s_params[0]; k++) {
        const param_cmd_t *pc = &s_params[k];
        if (g_cmd != pc->cmd) continue;

        if (s_armed_cmd != g_cmd) {
            s_armed_cmd = g_cmd;
            g_st.value = CFG(pc->off);
            return;
        }
        uint32_t val = g_st.value;
        if (val == CFG(pc->off) || val < pc->min || val > pc->max) return;

        CFG(pc->off) = (uint16_t)val;
        s_armed_cmd = 0;
        if (pc->cmd == CMD_SET_BAUD) { usart2_set_baud(val); mb_init(); }
        if (pc->cmd == CMD_SET_MB_ADDR) { mb_set_address((uint8_t)val); s_addr_shadow = (uint16_t)val; }
        if (pc->cmd == CMD_SET_SLEW_UP)   ctl_ramp_from_legacy(true);    /* legacy unit, 5 A/s per step */
        if (pc->cmd == CMD_SET_SLEW_DOWN) ctl_ramp_from_legacy(false);
        g_cmd = CMD_SAVE;
        return;
    }
    s_armed_cmd = 0;
}

static void handle_commands(void)
{
    uint32_t val = g_st.value;

    if (!g_st.on) {
        if (g_cmd == CMD_AUX_OUT) pin_set(PIN_AUX_OUT_PORT, PIN_AUX_OUT);
        else                      pin_reset(PIN_AUX_OUT_PORT, PIN_AUX_OUT);
    }

    uint16_t cut = CFG(CFG_CUTOFF_V);
    if (cut != 0u && g_st.on == 1u && g_st.v <= cut) ctl_force_off();   /* battery cut-off */

    switch (g_cmd) {
    case CMD_RESET_ENERGY:
        g_st.uah = 0; g_st.energy = 0;
        g_cmd = CMD_NONE;
        break;
    case CMD_SET_ITRIM:
        CFG(CFG_ITRIM_LIMIT) = (uint16_t)((val < CFG(CFG_IMAX)) ? val : CFG(CFG_IMAX));
        g_cmd = CMD_SAVE;
        break;
    case CMD_SET_VMAX:
        CFG(CFG_VMAX) = (uint16_t)val; CFG(CFG_SAVED_V) = (uint16_t)val;
        g_cmd = CMD_SAVE;
        break;
    case CMD_SET_IMAX:
        CFG(CFG_IMAX) = (uint16_t)val; CFG(CFG_SAVED_I) = 0;
        g_cmd = CMD_SAVE;
        break;
    case CMD_SET_PMAX:
        CFG(CFG_PMAX) = (uint16_t)val; CFG(CFG_SAVED_P) = 0;
        g_cmd = CMD_SAVE;
        break;
    case CMD_SET_VREF:
        if (val >= 2000u) { CFG(CFG_VREF) = (uint16_t)val; g_cmd = CMD_SAVE; }
        break;
    default:
        param_command();
        break;
    }

    g_pdiv = CFG(CFG_POWER_RES) ? 10u : 1u;

    /* address written directly through holding register 0 */
    uint16_t addr = CFG(CFG_MB_ADDR);
    if (addr != 0u && addr != s_addr_shadow) {
        s_addr_shadow = addr;
        mb_set_address((uint8_t)addr);
        g_cmd = CMD_SAVE;
    }

    if (g_cmd == CMD_SAVE) {
        cfg_save();
        g_cmd = CMD_NONE;
        g_st.value = 0;
        g_cfg_save_req = 0;
    } else if (g_cfg_save_req) {        /* direct setting registers (0x1C0-0x1C4) */
        g_cfg_save_req = 0;
        cfg_save();
    }
}

/* ------------------------------------------------------------------------- */
void app_init(void)
{
    bool dirty = false;

    cfg_load();
    if (CFG(CFG_MB_ADDR) == 0u)                              { CFG(CFG_MB_ADDR) = 1; dirty = true; }
    if (CFG(CFG_MB_BAUD) == 0u || CFG(CFG_MB_BAUD) > MB_BAUD_MAX_IDX) { CFG(CFG_MB_BAUD) = MB_BAUD_DEFAULT_IDX; dirty = true; }
    /* First boot of this firmware: switch Modbus to 115200 once. Later
     * changes (command 0xE8) are kept. */
    if (CFG(CFG_FW_MARKER) != CFG_FW_MARKER_VALUE) {
        CFG(CFG_FW_MARKER) = CFG_FW_MARKER_VALUE;
        CFG(CFG_MB_BAUD) = MB_BAUD_DEFAULT_IDX;
        dirty = true;
    }
    if (dirty) cfg_save();

    switch (CFG(CFG_MODE)) {
    case 0: case 1: case 2: case 3: case 4: case 5: case 9: case 10: break;
    default: CFG(CFG_MODE) = MODE_CC; break;
    }
    s_addr_shadow = CFG(CFG_MB_ADDR);
    g_pdiv = CFG(CFG_POWER_RES) ? 10u : 1u;

    lc_init();
    stream_init();
    hw_init();
    meas_init();
    ctl_init();
    mb_init();
    hw_start_tick();
}

/* ------------------------------------------------------------------------- */
/* Per-sample callbacks from the ADS1115 sampler                              */
/* ------------------------------------------------------------------------- */
static void update_pr(void)
{
    uint32_t v = g_st.v, i = g_st.i;
    g_st.p = (uint32_t)(((uint64_t)i * v) / ((uint32_t)g_pdiv * 1000u));
    g_st.r = i ? (v * 100u) / i : 0u;
}

void app_on_voltage(void)
{
    if (g_cmd != CMD_CAL_IMEAS && g_cmd != CMD_CAL_IDAC &&
        g_cmd != CMD_CAL_VDAC_LO && g_cmd != CMD_CAL_VDAC_HI) vtrim();
    update_pr();
}

void app_on_current(void)
{
    itrim();
    update_pr();
}

void app_loop(void)
{
    for (;;) {
        meas_service();                 /* V / I / VIN at the ADS1115's pace */

        if (g_flag_10ms) {
            g_flag_10ms = 0;
            ctl_mode_update();
            cal_voltage_tick();
            cal_idac_tick();
            cal_imeas_tick();
        }
        if (g_flag_100ms) {
            g_flag_100ms = 0;
            s_ui_tick++;
            meas_slow();
            stream_fast_flush();
        }
        if (g_flag_500ms) {
            g_flag_500ms = 0;
            g_st.uah    += (g_st.i * 500u) / 3600u;
            g_st.energy += (g_st.v * g_st.i) / 7200u;
        }
        if (s_ui_tick >= 50u || g_cmd != CMD_NONE) {
            s_ui_tick = 0;
            fan_update();
        }

        stream_service();
        mb_poll();
        cal_poll_meter();
        handle_commands();
        lc_poll();

        /* watchdog only while both the main loop and the 10 kHz tick run */
        if (g_isr_alive) { g_isr_alive = 0; iwdg_kick(); }
    }
}
