/**
 * @file  app_measure.c
 * @brief Voltage, current, VIN and temperature measurement.
 *
 * Voltage, current and VIN come from the ADS1115 (860 SPS single-shot). The
 * sampler is pipelined: as soon as conversion N completes, conversion N+1 is
 * started and N's result is read and processed while N+1 converts, so the
 * ADC runs close to its native rate. V and I alternate; VIN is inserted every
 * 100 ms. Range switching for a channel is applied right after that channel's
 * result is processed, i.e. a whole conversion before it is used again.
 *
 * Each result is streamed (app_stream.c) with a timestamp at the middle of
 * its conversion window.
 */
#include "app.h"

/* NTC lookup: index = temperature in degC, value = ADC/4 (10 bit) */
static const uint16_t s_ntc_table[100] = {
    768, 758, 747, 737, 726, 716, 705, 694, 683, 672, 660, 649, 638, 626, 615, 603,
    591, 580, 568, 556, 545, 533, 522, 510, 499, 488, 476, 465, 454, 443, 433, 422,
    412, 401, 391, 381, 371, 361, 352, 343, 333, 324, 316, 307, 299, 290, 282, 274,
    267, 259, 252, 245, 238, 231, 224, 218, 212, 205, 200, 194, 188, 183, 177, 172,
    167, 162, 158, 153, 149, 144, 140, 136, 132, 128, 124, 121, 117, 114, 111, 107,
    104, 101,  98,  96,  93,  90,  88,  85,  83,  80,  78,  76,  74,  72,  70,  68,
     66,  64,  62,  61,
};

/* Current-measurement gain table boundaries (added to IMAX/1000) */
static const uint16_t s_imeas_lo_lim[14] = {
    50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 2000, 3000, 4000,
};
static const uint16_t s_imeas_hi_lim[23] = {
    6000, 7000, 8000, 9000, 10000, 11000, 12000, 13000, 14000, 15000, 16000,
    17000, 18000, 19000, 20000, 25000, 30000, 35000, 40000, 45000, 50000, 55000, 60000,
};
static const uint16_t s_imeas_hi_off[23] = {
    0x6E, 0x70, 0x72, 0x74, 0x76, 0x78, 0x7A, 0x7C, 0x7E, 0x80, 0x82, 0x84, 0x86,
    0x88, 0x8A, 0x94, 0x9E, 0xA8, 0xB2, 0xBC, 0xC6, 0xD0, 0xDA,
};

/* Voltage range state: want_hi selects range, pend_* request reconfiguration */
static uint8_t s_v_want_hi = 0, s_v_pend_hi = 1, s_v_pend_lo = 1;
static uint8_t s_vin_hi = 0, s_vin_pend_hi = 0, s_vin_pend_lo = 1;
/* Current range state */
static uint8_t  s_i_init, s_i_latch;
static uint16_t s_i_base, s_i_lo_max;

/* ADS1115 sampler state */
enum { JOB_V = 0, JOB_I = 1, JOB_VIN = 2 };
static struct {
    uint8_t  busy, job, range, polls, vin_req, next_i;
    uint32_t t0;
    uint16_t poll_at;                 /* us after start of the first ready poll */
    uint16_t err, discard, sps, cnt;
    uint32_t sps_t0;
} s_ads = { .poll_at = 1100 };

static uint16_t imeas_lo_gain(uint32_t i)
{
    for (uint32_t n = 0; n < 14; n++)
        if (i <= s_i_base + s_imeas_lo_lim[n]) return CFG(CFG_IMEAS_LO_TBL + 2u * n);
    return CFG(0x6C);
}

static uint16_t imeas_hi_gain(uint32_t i)
{
    for (uint32_t n = 0; n < 23; n++)
        if (i <= (uint32_t)s_i_base + s_imeas_hi_lim[n]) return CFG(s_imeas_hi_off[n]);
    return CFG(CFG_IMEAS_HI_LAST);
}

static void irange_set(bool high)
{
    if (high) pin_set(PIN_IRANGE_PORT, PIN_IRANGE);
    else      pin_reset(PIN_IRANGE_PORT, PIN_IRANGE);
}

static uint8_t forced(uint8_t force, uint8_t automatic)
{
    return (g_lc.raw_mode && force) ? (uint8_t)(force == 2u) : automatic;
}

uint8_t meas_vin_range(void) { return forced(g_lc.force_vinr, s_vin_hi); }

void meas_stats(uint16_t *err, uint16_t *discard, uint16_t *sps, uint16_t *poll_at)
{
    *err = s_ads.err;
    *discard = s_ads.discard;
    *sps = s_ads.sps;
    *poll_at = s_ads.poll_at;
}

void meas_voltage(void) { }      /* voltage is sampled continuously now */

/* ------------------------------------------------------------------------- */
/* Voltage                                                                    */
/* ------------------------------------------------------------------------- */
static void v_apply_range(void)
{
    if (!forced(g_lc.force_vr, s_v_want_hi)) {
        if (s_v_pend_lo) {
            vrange_set_high(false);
            s_v_pend_hi = 1; s_v_pend_lo = 0;
            g_vdac_scale = g_vdac_base = CFG(CFG_VDAC_SCALE_LO);
            g_v_gain_reg = CFG(CFG_V_GAIN_LO);
        }
    } else {
        if (s_v_pend_hi) {
            vrange_set_high(true);
            s_v_pend_hi = 0; s_v_pend_lo = 1;
            g_vdac_scale = g_vdac_base = CFG(CFG_VDAC_SCALE_HI);
            g_v_gain_reg = CFG(CFG_V_GAIN_HI);
        }
    }
}

static void v_process(uint32_t raw, uint8_t hi, uint32_t ts)
{
    uint32_t v;

    g_lc.raw_v = (uint16_t)raw;
    if (!hi) {
        v = lc_valid(LC_V_LO) ? lc_apply_u(LC_V_LO, (int32_t)raw)
                              : (CFG(CFG_V_GAIN_LO) * raw) / 10000u;
    } else if (lc_valid(LC_V_HI)) {
        v = lc_apply_u(LC_V_HI, (int32_t)raw);
    } else {
        v = (CFG(CFG_V_GAIN_HI) * raw) / 10000u;
        /* small offset corrections at the bottom of the high range */
        if (v < 6101u) v = (v > 3u) ? v - 3u : 0u;
        if (v < 5101u) v = (v > 3u) ? v - 3u : 0u;
    }

    /* range selection with hysteresis (38.51 V / 36.00 V) */
    if (v <= CFG(CFG_V_ZERO)) { v = 0; s_v_want_hi = 0; }
    else if (v >= 3851u)      { s_v_want_hi = 1; }
    else if (v < 3600u)       { s_v_want_hi = 0; }

    /* lead-resistance compensation depending on the model's current rating */
    switch (CFG(CFG_IMAX)) {
    case 3000:  v += g_st.i / 100u;  break;
    case 10000: v += g_st.i / 1000u; break;
    case 20000: v += g_st.i / 2000u; break;
    case 30000:
    case 40000: v += g_st.i / 3000u; break;
    case 50000:
    case 60000: v += g_st.i / 4000u; break;
    default: break;
    }

    if (g_cmd == CMD_SET_V_ZERO) {
        if (v != 0u) { CFG(CFG_V_ZERO) = (uint16_t)v; g_cmd = CMD_SAVE; }
    } else if (g_cmd == CMD_CLR_V_ZERO) {
        CFG(CFG_V_ZERO) = 0; g_cmd = CMD_SAVE;
    }

    if (CFG(CFG_V_FROM_VIN) == 1u) v = g_st.vin / 10u;
    g_st.v = v;

    v_apply_range();                  /* the I conversion runs meanwhile */
    stream_sample(REC_V, ts, (uint16_t)(raw | (hi ? 0x8000u : 0u)),
                  (uint16_t)(v > 0xFFFFu ? 0xFFFFu : v));
    app_on_voltage();
}

/* ------------------------------------------------------------------------- */
/* Current                                                                    */
/* ------------------------------------------------------------------------- */
static void i_range_update(uint32_t i)
{
    if (!s_i_init) {
        uint16_t imax = CFG(CFG_IMAX);
        s_i_init = 1;
        s_i_base = imax / 1000u;
        if (imax >= 1000u)  s_i_lo_max = 6000;
        if (imax >= 40000u) s_i_lo_max = 8000;
    }

    if (g_cmd == CMD_CAL_IMEAS) return;   /* gain driven by the calibration */

    if (g_st.set_i <= 5500u) {
        if (i <= s_i_lo_max && !s_i_latch) {
            irange_set(false);
            g_imeas_gain = imeas_lo_gain(i);
            return;
        }
        if (i < (uint32_t)s_i_base + 5500u) {     /* hysteresis band */
            if (i < 5500u - s_i_base) s_i_latch = 0;
            return;
        }
        irange_set(true);
        s_i_latch = 1;
    } else {
        irange_set(true);
    }
    g_imeas_gain = imeas_hi_gain(i);
}

static void i_process(uint32_t raw, uint8_t hi, uint32_t ts)
{
    /* the range pin can also be switched by the control ISR; a conversion
     * that straddles a range change is meaningless */
    if ((pin_read(PIN_IRANGE_PORT, PIN_IRANGE) ? 1u : 0u) != hi) {
        s_ads.discard++;
        return;
    }

    if (g_lc.raw_mode && g_lc.force_ir)          /* legacy gain for the forced range */
        g_imeas_gain = hi ? imeas_hi_gain(g_st.i) : imeas_lo_gain(g_st.i);

    uint32_t i;
    uint32_t ch = hi ? LC_I_HI : LC_I_LO;
    if (lc_valid(ch)) i = lc_apply_u(ch, (int32_t)raw);
    else i = (uint32_t)(((uint64_t)g_imeas_gain * ((CFG(CFG_I_GAIN) * raw) / 1000u)) / 10000u);
    if (i <= CFG(CFG_I_ZERO)) i = 0;
    g_st.i = i;

    g_lc.raw_i = (uint16_t)raw;
    g_lc.ranges = (uint16_t)((g_lc.ranges & ~3u) | (g_vrange_hi ? 1u : 0u) | (hi ? 2u : 0u));
    g_lc.seq_vi++;
    stream_sample(REC_I, ts, (uint16_t)(raw | (hi ? 0x8000u : 0u)),
                  (uint16_t)(i > 0xFFFFu ? 0xFFFFu : i));

    if (!(g_lc.raw_mode && g_lc.force_ir)) {    /* else the range is held by the tool */
        if (g_cmd == CMD_SET_I_ZERO && i != 0u) {
            CFG(CFG_I_ZERO) = (uint16_t)i; g_cmd = CMD_SAVE;
        } else if (g_cmd == CMD_CLR_I_ZERO) {
            CFG(CFG_I_ZERO) = 0; g_cmd = CMD_SAVE;
        }
        i_range_update(i);
    }
    app_on_current();
}

/* ------------------------------------------------------------------------- */
/* VIN                                                                        */
/* ------------------------------------------------------------------------- */
static void vin_process(uint32_t raw, uint8_t hi, uint32_t ts)
{
    uint32_t vin;
    if (!hi) {
        if (s_vin_pend_lo) { s_vin_pend_hi = 1; s_vin_pend_lo = 0; g_vin_gain_reg = CFG(CFG_VIN_GAIN_LO); }
        vin = lc_valid(LC_VIN_LO) ? lc_apply_u(LC_VIN_LO, (int32_t)raw)
                                  : (CFG(CFG_VIN_GAIN_LO) * raw) / 10000u;
    } else {
        if (s_vin_pend_hi) { s_vin_pend_hi = 0; s_vin_pend_lo = 1; g_vin_gain_reg = CFG(CFG_VIN_GAIN_HI); }
        vin = lc_valid(LC_VIN_HI) ? lc_apply_u(LC_VIN_HI, (int32_t)raw)
                                  : (CFG(CFG_VIN_GAIN_HI) * raw) / 10000u;
    }
    g_lc.raw_vin = (uint16_t)raw;
    g_lc.ranges = (uint16_t)((g_lc.ranges & ~4u) | (hi ? 4u : 0u));
    g_lc.seq_vin++;

    if (vin >= 0xFFFFu)     { vin = 0xFFFF; s_vin_hi = 1; }
    else if (vin > 32000u)  { s_vin_hi = 1; }
    else if (vin < 29500u)  { s_vin_hi = 0; }
    g_st.vin = vin;
    stream_sample(REC_VIN, ts, (uint16_t)(raw | (hi ? 0x8000u : 0u)), (uint16_t)vin);
}

/* ------------------------------------------------------------------------- */
/* ADS1115 pipeline                                                           */
/* ------------------------------------------------------------------------- */
static void ads_begin(void)
{
    uint8_t job, range;
    uint32_t mux;

    if (s_ads.vin_req)      job = JOB_VIN;
    else if (s_ads.next_i)  job = JOB_I;
    else                    job = JOB_V;

    switch (job) {
    case JOB_V: mux = ADS_CH_VOLTAGE; range = g_vrange_hi; break;
    case JOB_I: mux = ADS_CH_CURRENT; range = pin_read(PIN_IRANGE_PORT, PIN_IRANGE) ? 1u : 0u; break;
    default:    range = meas_vin_range(); mux = range ? ADS_CH_VIN_HI : ADS_CH_VIN_LO; break;
    }

    if (!ads_start(mux)) {
        s_ads.err++;
        stream_event(EVT_ADS_ERR, s_ads.err);
        return;                       /* retried on the next service call */
    }
    s_ads.t0 = now_us();
    s_ads.job = job;
    s_ads.range = range;
    s_ads.polls = 0;
    s_ads.busy = 1;
    if (job == JOB_VIN) s_ads.vin_req = 0;
    else                s_ads.next_i ^= 1u;
}

void meas_service(void)
{
    if (!s_ads.busy) { ads_begin(); return; }

    uint32_t el = now_us() - s_ads.t0;
    if (el < s_ads.poll_at) return;

    int r = ads_poll();
    s_ads.polls++;
    if (r <= 0) {
        if (r < 0) s_ads.err++;
        else if (s_ads.polls == 1u && s_ads.poll_at < 1500u) s_ads.poll_at += 10u;
        if (el > 5000u) { s_ads.busy = 0; s_ads.err++; }     /* give up, restart */
        return;
    }
    if (s_ads.polls == 1u && s_ads.poll_at > 900u) s_ads.poll_at -= 2u;   /* creep earlier */

    uint8_t  job = s_ads.job, range = s_ads.range;
    uint32_t ts = s_ads.t0 + el / 2u;               /* middle of the conversion */
    s_ads.busy = 0;
    ads_begin();                                    /* next one converts while we work */

    uint16_t v;
    if (!ads_fetch(&v)) { s_ads.err++; return; }
    uint32_t raw = (v & 0x8000u) ? 0u : v;          /* negative -> 0 */

    s_ads.cnt++;
    uint32_t t = now_us();
    if ((uint32_t)(t - s_ads.sps_t0) >= 1000000u) {
        s_ads.sps = s_ads.cnt;
        s_ads.cnt = 0;
        s_ads.sps_t0 = t;
    }

    switch (job) {
    case JOB_V:   v_process(raw, range, ts); break;
    case JOB_I:   i_process(raw, range, ts); break;
    default:      vin_process(raw, range, ts); break;
    }
}

void meas_init(void)
{
    v_apply_range();
}

/* ------------------------------------------------------------------------- */
/* Slow channels (100 ms): aux, PA0/PA6 bursts, temperatures, VIN request      */
/* ------------------------------------------------------------------------- */
static void ntc_read(uint32_t ch, volatile uint32_t *out)
{
    uint32_t sum8;
    adc_read_avg10(ch, &sum8);
    uint32_t v = (sum8 & 0x1FFFFFu) >> 5;          /* 10-bit average */
    for (uint32_t t = 0; t < 100u; t++) {
        if (s_ntc_table[t] <= v) { *out = t; return; }
    }
    /* hotter than the table: keep previous value (as original) */
}

static uint32_t mcu_temp_read(void)
{
    uint32_t sum8;
    adc_read_avg10(ADC_CH_TEMP, &sum8);
    uint32_t mv = ((((sum8 & 0x7FFFFu) >> 3) * CFG(CFG_VREF)) / 4095u) & 0xFFFFu;
    int32_t t = (((int32_t)(14300 - 10 * (int32_t)mv) * 100) / 43 + 2500) / 10;
    if (t < 0) t = 9000 - t;
    return (uint32_t)t;
}

void meas_slow(void)
{
    uint32_t ts = now_us();
    s_ads.vin_req = 1;                              /* next ADS slot reads VIN */

    g_st.aux = (adc_read_avg130(ADC_CH_AUX) * 7650u) / 10000u;

    uint16_t d_avg, d_min, d_max;
    adc_burst(0, 64, &d_avg, &d_min, &d_max);
    g_lc.diag[0] = d_avg; g_lc.diag[1] = d_min; g_lc.diag[2] = d_max;
    adc_burst(6, 64, &d_avg, &d_min, &d_max);
    g_lc.diag[3] = d_avg; g_lc.diag[4] = d_min; g_lc.diag[5] = d_max;

    ntc_read(ADC_CH_NTC1, &g_st.t_ntc1);
    ntc_read(ADC_CH_NTC2, &g_st.t_ntc2);
    g_st.t_mcu = mcu_temp_read();

    struct __attribute__((packed)) {
        uint32_t ts;
        uint16_t aux, pa0[3], pa6[3];
        uint8_t  ntc1, ntc2;
        uint16_t t_mcu;
        uint32_t uah, energy, seconds;
    } s = {
        ts, (uint16_t)g_st.aux,
        { g_lc.diag[0], g_lc.diag[1], g_lc.diag[2] },
        { g_lc.diag[3], g_lc.diag[4], g_lc.diag[5] },
        (uint8_t)g_st.t_ntc1, (uint8_t)g_st.t_ntc2, (uint16_t)g_st.t_mcu,
        g_st.uah, g_st.energy, g_st.seconds,
    };
    stream_put(REC_SLOW, &s, sizeof s, NULL, 0);
}
