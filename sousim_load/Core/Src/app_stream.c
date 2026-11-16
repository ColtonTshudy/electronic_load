/**
 * @file  app_stream.c
 * @brief Real-time sample stream: every measurement is written as a
 *        timestamped record into a RAM ring buffer; the host drains it with
 *        Modbus function 0x41 (see app_modbus.c).
 *
 * Record: [type u8][len u8][body: len bytes], little endian, ts = us since boot.
 *   0x01 V      ts u32, raw u16 (b15 = high range), value u16 (10 mV)
 *   0x02 I      ts u32, raw u16 (b15 = high range), value u16 (mA)
 *   0x03 VIN    ts u32, raw u16 (b15 = high range), value u16 (mV)
 *   0x04 SLOW   ts u32, aux u16, pa0 avg/min/max u16, pa6 avg/min/max u16,
 *               ntc1 u8, ntc2 u8, t_mcu u16, uah u32, energy u32, seconds u32
 *   0x05 STATE  see emit_state()
 *   0x06 FAST   ts u32 (first sample), period_us u32, mask u8, n u8,
 *               n x (one u16 per mask bit: PA6, PA0, CC DAC, CV DAC);
 *               0xFFFF = ADC sample skipped (ADC busy with a slow reading)
 *   0x07 EVENT  ts u32, code u8, data u16
 *
 * Stream control registers (FC 3/6/16):
 *   0x180 enable          0x181 fast divider (0 off, N -> 10 kHz / N)
 *   0x182 fast mask       0x183 state period, ms (0 = off)
 *   0x184/0x185 runtime baud (hi/lo, handled in app_modbus.c)
 *   0x186 ring overflows  0x187 fast overflows  0x188 ring fill  0x189 ring size
 *   0x18A ADS errors      0x18B I samples discarded (range change)
 *   0x18C/0x18D now, us (hi/lo)   0x18E ADS conversions/s   0x18F protocol 1
 *   0x190 ADS first-poll delay, us (adaptive, read only)
 */
#include "app.h"
#include <string.h>

#define RING_SZ    2560u
#define FAST_BLKS  4u
#define FAST_VALS  120u          /* u16 values per fast block (record <= 255 B) */

volatile uint32_t g_tick;
volatile uint8_t  g_prot_evt;

static uint8_t  s_ring[RING_SZ];
static uint16_t s_head, s_tail;                  /* main-loop context only */
static uint16_t s_ovf_cnt;
static uint8_t  s_ovf_flag, s_en;
static uint16_t s_state_ms = 20;
static uint32_t s_state_last;

typedef struct { uint32_t ts; uint8_t n; uint16_t d[FAST_VALS]; } fblk_t;
static fblk_t s_fb[FAST_BLKS];
static volatile uint8_t  s_fwr, s_frd, s_ffill, s_fflush;
static volatile uint16_t s_fdiv, s_fcnt, s_fdiv_req;
static volatile uint8_t  s_fmask, s_fnch, s_fper, s_fadc;
static volatile uint16_t s_fovf_cnt;
static volatile uint8_t  s_fovf_flag;

/* ------------------------------------------------------------------------- */
uint32_t now_us(void)
{
    uint32_t t, c;
    do { t = g_tick; c = TIM7->CNT; } while (t != g_tick);
    if ((TIM7->SR & TIM_SR_UIF) && c < 50u) t++;     /* wrap not yet counted */
    return t * 100u + c;
}

static uint16_t ring_used(void) { return (uint16_t)((s_head + RING_SZ - s_tail) % RING_SZ); }

static void ring_write(const void *src, uint32_t n)
{
    const uint8_t *p = (const uint8_t *)src;
    while (n--) {
        s_ring[s_head] = *p++;
        if (++s_head == RING_SZ) s_head = 0;
    }
}

bool stream_put(uint8_t type, const void *hdr, uint8_t hlen, const void *data, uint8_t dlen)
{
    if (!s_en) return false;
    uint32_t len = (uint32_t)hlen + dlen;
    if (len > 255u) return false;
    if (RING_SZ - 1u - ring_used() < len + 2u) {
        s_ovf_cnt++;
        s_ovf_flag = 1;
        return false;
    }
    uint8_t h[2] = { type, (uint8_t)len };
    ring_write(h, 2);
    if (hlen) ring_write(hdr, hlen);
    if (dlen) ring_write(data, dlen);
    return true;
}

void stream_sample(uint8_t type, uint32_t ts, uint16_t raw, uint16_t val)
{
    struct __attribute__((packed)) { uint32_t ts; uint16_t raw, val; } r = { ts, raw, val };
    stream_put(type, &r, sizeof r, NULL, 0);
}

void stream_event(uint8_t code, uint16_t data)
{
    struct __attribute__((packed)) { uint32_t ts; uint8_t code; uint16_t data; } r = { now_us(), code, data };
    stream_put(REC_EVENT, &r, sizeof r, NULL, 0);
}

/* Copy whole records (oldest first) into dst; consumed data is removed. */
uint32_t stream_read(uint8_t *dst, uint32_t max, uint8_t *flags)
{
    uint32_t n = 0;
    while (s_tail != s_head) {
        uint32_t rec = (uint32_t)s_ring[(s_tail + 1u) % RING_SZ] + 2u;
        if (n + rec > max) break;
        for (uint32_t k = 0; k < rec; k++) {
            dst[n++] = s_ring[s_tail];
            if (++s_tail == RING_SZ) s_tail = 0;
        }
    }
    *flags = (uint8_t)((s_ovf_flag ? 1u : 0u) | (s_fovf_flag ? 2u : 0u) | (s_en ? 4u : 0u));
    s_ovf_flag = 0;
    s_fovf_flag = 0;
    return n;
}

bool stream_enabled(void) { return s_en != 0u; }

/* ------------------------------------------------------------------------- */
/* Fast sampler (10 kHz ISR): PA6 / PA0 (injected ADC) and both DAC codes     */
/* ------------------------------------------------------------------------- */
static void fast_reconfig(uint16_t div, uint8_t mask)
{
    s_fdiv = 0;                                  /* ISR idle while we change */
    s_fdiv_req = div;
    mask &= 0x0Fu;
    uint8_t nch = 0;
    for (uint8_t b = 0; b < 4u; b++) if (mask & (1u << b)) nch++;
    s_fmask = mask;
    s_fnch = nch;
    s_fper = nch ? (uint8_t)(FAST_VALS / nch) : 0u;
    s_fadc = (uint8_t)((mask & 1u) + ((mask >> 1) & 1u));
    adc_fast_config(mask & 1u, mask & 2u);
    s_fwr = s_frd = 0;
    s_ffill = 0;
    s_fcnt = 0;
    s_fdiv = (nch && div) ? div : 0u;
}

void stream_fast_isr(void)
{
    if (!s_fdiv || !s_en) return;
    if (++s_fcnt < s_fdiv) return;
    s_fcnt = 0;

    fblk_t *b = &s_fb[s_fwr];
    uint32_t k = (uint32_t)s_ffill * s_fnch;
    if (s_ffill == 0u) b->ts = g_tick * 100u;

    uint16_t adc[2] = { 0xFFFF, 0xFFFF };
    if (s_fadc && !g_adc_lock) {
        ADC1->CR2 |= ADC_CR2_JSWSTART;
        uint32_t guard = 0;
        while (!(ADC1->SR & ADC_SR_JEOC) && ++guard < 2000u) { }
        adc[0] = (uint16_t)ADC1->JDR1;
        adc[1] = (uint16_t)ADC1->JDR2;
        ADC1->SR = 0;                            /* JEOC also sets EOC: clear */
    }
    uint8_t ai = 0;
    if (s_fmask & 1u) b->d[k++] = adc[ai++];
    if (s_fmask & 2u) b->d[k++] = adc[ai++];
    if (s_fmask & 4u) b->d[k++] = (uint16_t)DAC->DOR2;
    if (s_fmask & 8u) b->d[k++] = (uint16_t)DAC->DOR1;

    s_ffill++;
    if (s_ffill >= s_fper || s_fflush) {
        b->n = s_ffill;
        s_ffill = 0;
        s_fflush = 0;
        uint8_t nx = (uint8_t)((s_fwr + 1u) % FAST_BLKS);
        if (nx != s_frd) s_fwr = nx;
        else { s_fovf_cnt++; s_fovf_flag = 1; }  /* host too slow: drop block */
    }
}

void stream_fast_flush(void) { if (s_ffill) s_fflush = 1; }

/* ------------------------------------------------------------------------- */
static void emit_state(uint32_t ts)
{
    uint32_t ramp; uint16_t stopping;
    ctl_dac_state(&ramp, &stopping);
    struct __attribute__((packed)) {
        uint32_t ts;
        uint8_t  on, mode;
        uint16_t cmd, value, set_v, set_i, set_p, set_r, p, r;
        uint16_t dac_cc, dac_cv, ramp, stopping, idac_cal, vdac_scale, imeas_gain;
        uint8_t  flags, fan, list_step, pdiv;
        uint16_t list_time, vin_gain_reg, v_gain_reg;
    } s;
    s.ts = ts;
    s.on = (uint8_t)g_st.on;
    s.mode = (uint8_t)CFG(CFG_MODE);
    s.cmd = g_cmd;
    s.value = (uint16_t)g_st.value;
    s.set_v = (uint16_t)g_st.set_v;
    s.set_i = (uint16_t)g_st.set_i;
    s.set_p = (uint16_t)g_st.set_p;
    s.set_r = (uint16_t)g_st.set_r;
    s.p = (uint16_t)((g_st.p > 0xFFFFu) ? 0xFFFFu : g_st.p);
    s.r = (uint16_t)((g_st.r > 0xFFFFu) ? 0xFFFFu : g_st.r);
    s.dac_cc = (uint16_t)DAC->DOR2;
    s.dac_cv = (uint16_t)DAC->DOR1;
    s.ramp = (uint16_t)((ramp > 0xFFFFu) ? 0xFFFFu : ramp);
    s.stopping = stopping;          /* was the soft-start ceiling */
    s.idac_cal = g_idac_cal;
    s.vdac_scale = g_vdac_scale;
    s.imeas_gain = (uint16_t)g_imeas_gain;
    s.flags = (uint8_t)((pin_read(PIN_IRANGE_PORT, PIN_IRANGE) ? 0x01u : 0u)
                      | (g_vrange_hi ? 0x02u : 0u)
                      | (meas_vin_range() ? 0x04u : 0u)
                      | (g_alarm ? 0x08u : 0u)
                      | (g_dac_settled ? 0x10u : 0u)
                      | (g_lc.raw_mode ? 0x20u : 0u)
                      | (pin_read(PIN_AUX_OUT_PORT, PIN_AUX_OUT) ? 0x40u : 0u)
                      | (pin_read(PIN_LOAD_INH_PORT, PIN_LOAD_INH) ? 0x80u : 0u));
    s.fan = g_fan_speed;
    s.list_step = (uint8_t)g_st.list_step;
    s.pdiv = (uint8_t)g_pdiv;
    s.list_time = (uint16_t)g_st.list_time;
    s.vin_gain_reg = g_vin_gain_reg;
    s.v_gain_reg = g_v_gain_reg;
    stream_put(REC_STATE, &s, sizeof s, NULL, 0);
}

void stream_service(void)
{
    if (!s_en) return;

    while (s_frd != s_fwr) {
        fblk_t *b = &s_fb[s_frd];
        struct __attribute__((packed)) { uint32_t ts; uint32_t period; uint8_t mask, n; } h =
            { b->ts, (uint32_t)s_fdiv * 100u, s_fmask, b->n };
        stream_put(REC_FAST, &h, sizeof h, b->d, (uint8_t)(2u * b->n * s_fnch));
        s_frd = (uint8_t)((s_frd + 1u) % FAST_BLKS);
    }

    uint8_t pe = g_prot_evt;
    if (pe) { g_prot_evt = 0; stream_event(pe, 0); }

    uint32_t t = now_us();
    if (s_state_ms && (uint32_t)(t - s_state_last) >= (uint32_t)s_state_ms * 1000u) {
        s_state_last = t;
        emit_state(t);
    }
}

void stream_init(void)
{
    s_head = s_tail = 0;
    fast_reconfig(0, 0);
}

/* ------------------------------------------------------------------------- */
bool stream_reg_read(uint16_t a, uint16_t *v)
{
    uint16_t err, disc, sps, poll_at;
    uint32_t t;
    switch (a) {
    case 0x180: *v = s_en; return true;
    case 0x181: *v = s_fdiv_req; return true;
    case 0x182: *v = s_fmask; return true;
    case 0x183: *v = s_state_ms; return true;
    case 0x186: *v = s_ovf_cnt; return true;
    case 0x187: *v = s_fovf_cnt; return true;
    case 0x188: *v = ring_used(); return true;
    case 0x189: *v = RING_SZ; return true;
    case 0x18A: meas_stats(&err, &disc, &sps, &poll_at); *v = err; return true;
    case 0x18B: meas_stats(&err, &disc, &sps, &poll_at); *v = disc; return true;
    case 0x18C: t = now_us(); *v = (uint16_t)(t >> 16); return true;
    case 0x18D: t = now_us(); *v = (uint16_t)t; return true;
    case 0x18E: meas_stats(&err, &disc, &sps, &poll_at); *v = sps; return true;
    case 0x18F: *v = 1; return true;
    case 0x190: meas_stats(&err, &disc, &sps, &poll_at); *v = poll_at; return true;
    default: return false;
    }
}

bool stream_reg_writable(uint16_t a)
{
    return a >= 0x180u && a <= 0x183u;
}

void stream_reg_write(uint16_t a, uint16_t v)
{
    switch (a) {
    case 0x180:
        if (v && !s_en) {
            s_head = s_tail = 0;
            s_ovf_flag = 0;
            s_en = 1;
            fast_reconfig(s_fdiv_req, s_fmask);
            stream_event(EVT_STREAM_START, 1);
        } else if (!v) {
            s_en = 0;
        }
        break;
    case 0x181: fast_reconfig(v > 10000u ? 10000u : v, s_fmask); break;
    case 0x182: fast_reconfig(s_fdiv_req, (uint8_t)v); break;
    case 0x183: s_state_ms = v; break;
    default: break;
    }
}
