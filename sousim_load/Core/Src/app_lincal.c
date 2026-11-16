/**
 * @file  app_lincal.c
 * @brief Linear (gain + offset) calibration, independent of the legacy tables.
 *
 * Each channel maps x -> y = x * gain / 1e6 + offset:
 *   measurement channels: x = ADS1115 raw count, y = 10 mV / mA / mV
 *   DAC channels:         x = setpoint (mA or 10 mV), y = DAC code
 * A channel is used only when its bit is set in `valid`; otherwise the legacy
 * calibration from the 0x0800F000 page stays in effect.
 *
 * Stored in its own flash page (0x0800EC00) with a CRC32.
 *
 * Calibration register block (Modbus FC 3/4 read, FC 6/16 write), see
 * tools/sousim_cal.py:
 *   0x100 raw mode (1 = DACs driven from 0x104/0x105, ranges forceable)
 *   0x101 force V range   (0 auto, 1 low, 2 high)    - raw mode only
 *   0x102 force I range   (0 auto, 1 low, 2 high)    - raw mode only
 *   0x103 force VIN range (0 auto, 1 low, 2 high)    - raw mode only
 *   0x104 raw CC DAC code   0x105 raw CV DAC code
 *   0x106 action: 0x5A01 save, 0x5A02 erase, 0x5A03 reload (reads: last result)
 *   0x107 valid mask        0x108 firmware/cal version (read)
 *   0x110 raw V  0x111 raw I  0x112 raw VIN  0x113 ranges (b0 V, b1 I, b2 VIN high)
 *   0x114 V/I sample counter  0x115 VIN sample counter
 *   0x116 V  0x117 I  0x118 VIN (calibrated, same units as input registers)
 *   0x120 + 4*ch: gain (int32 hi, lo), offset (int32 hi, lo)
 */
#include "app.h"
#include "stm32f1xx_hal.h"
#include <stddef.h>
#include <string.h>

#define LC_FLASH_ADDR   0x0800EC00u
#define LC_MAGIC        0x31434C53u   /* "SLC1" */
#define LC_VERSION      1u
#define LC_RAW_TIMEOUT  10000u        /* ms without Modbus traffic */

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t valid;
    int32_t  gain[LC_N];
    int32_t  offset[LC_N];
    uint32_t crc;
} lincal_t;

static lincal_t s_lc __attribute__((aligned(4)));
lc_ctl_t g_lc;

static uint32_t crc32(const uint8_t *p, uint32_t n)
{
    uint32_t c = 0xFFFFFFFFu;
    while (n--) {
        c ^= *p++;
        for (int b = 0; b < 8; b++) c = (c & 1u) ? (c >> 1) ^ 0xEDB88320u : c >> 1;
    }
    return ~c;
}

static uint32_t lc_crc(const lincal_t *l)
{
    return crc32((const uint8_t *)l, offsetof(lincal_t, crc));
}

/* ------------------------------------------------------------------------- */
bool lc_valid(uint32_t ch)
{
    return ch < LC_N && (s_lc.valid & (1u << ch)) != 0u;
}

int32_t lc_apply(uint32_t ch, int32_t x)
{
    int64_t y = (int64_t)x * s_lc.gain[ch];
    y = (y >= 0) ? (y + 500000) / 1000000 : (y - 500000) / 1000000;
    return (int32_t)(y + s_lc.offset[ch]);
}

uint32_t lc_apply_u(uint32_t ch, int32_t x)
{
    int32_t y = lc_apply(ch, x);
    return (y < 0) ? 0u : (uint32_t)y;
}

/* ------------------------------------------------------------------------- */
static bool lc_load(void)
{
    lincal_t t;
    memcpy(&t, (const void *)LC_FLASH_ADDR, sizeof t);
    if (t.magic == LC_MAGIC && t.version == LC_VERSION && t.crc == lc_crc(&t)) {
        s_lc = t;
        return true;
    }
    memset(&s_lc, 0, sizeof s_lc);
    return false;
}

static bool lc_erase_page(void)
{
    FLASH_EraseInitTypeDef e = { .TypeErase = FLASH_TYPEERASE_PAGES,
                                 .PageAddress = LC_FLASH_ADDR, .NbPages = 1 };
    uint32_t err = 0;
    return HAL_FLASHEx_Erase(&e, &err) == HAL_OK;
}

static bool lc_save(void)
{
    s_lc.magic = LC_MAGIC;
    s_lc.version = LC_VERSION;
    s_lc.crc = lc_crc(&s_lc);

    bool ok;
    HAL_FLASH_Unlock();
    ok = lc_erase_page();
    const uint16_t *w = (const uint16_t *)&s_lc;
    for (uint32_t n = 0; ok && n < sizeof s_lc / 2u; n++)
        ok = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, LC_FLASH_ADDR + 2u * n, w[n]) == HAL_OK;
    HAL_FLASH_Lock();
    return ok && memcmp((const void *)LC_FLASH_ADDR, &s_lc, sizeof s_lc) == 0;
}

static bool lc_erase(void)
{
    bool ok;
    memset(&s_lc, 0, sizeof s_lc);
    HAL_FLASH_Unlock();
    ok = lc_erase_page();
    HAL_FLASH_Lock();
    return ok;
}

void lc_init(void)
{
    memset(&g_lc, 0, sizeof g_lc);
    g_lc.raw_cv = 4095;
    lc_load();
}

/* ------------------------------------------------------------------------- */
/* Raw mode safety: drop out and switch the load off if the host goes away   */
/* ------------------------------------------------------------------------- */
void lc_tick_1ms_isr(void)
{
    if (!g_lc.raw_mode) { g_lc.comm_ms = 0; return; }
    if (++g_lc.comm_ms > LC_RAW_TIMEOUT) {
        g_lc.raw_mode = 0;
        g_st.on = 0;
    }
}

void lc_comm_seen(void) { g_lc.comm_ms = 0; }

/* ------------------------------------------------------------------------- */
/* Register access                                                           */
/* ------------------------------------------------------------------------- */
bool lc_reg_read(uint16_t a, uint16_t *v)
{
    if (a >= LC_REG_COEF && a < LC_REG_COEF + 4u * LC_N) {
        uint32_t ch = (a - LC_REG_COEF) / 4u, k = (a - LC_REG_COEF) % 4u;
        uint32_t x = (k < 2u) ? (uint32_t)s_lc.gain[ch] : (uint32_t)s_lc.offset[ch];
        *v = (uint16_t)((k & 1u) ? x : x >> 16);
        return true;
    }
    switch (a) {
    case 0x100: *v = g_lc.raw_mode; return true;
    case 0x101: *v = g_lc.force_vr; return true;
    case 0x102: *v = g_lc.force_ir; return true;
    case 0x103: *v = g_lc.force_vinr; return true;
    case 0x104: *v = g_lc.raw_cc; return true;
    case 0x105: *v = g_lc.raw_cv; return true;
    case 0x106: *v = g_lc.action_result; return true;
    case 0x107: *v = s_lc.valid; return true;
    case 0x108: *v = (uint16_t)(0x0200u | LC_VERSION); return true;
    case 0x110: *v = g_lc.raw_v; return true;
    case 0x111: *v = g_lc.raw_i; return true;
    case 0x112: *v = g_lc.raw_vin; return true;
    case 0x113: *v = g_lc.ranges; return true;
    case 0x114: *v = g_lc.seq_vi; return true;
    case 0x115: *v = g_lc.seq_vin; return true;
    case 0x116: *v = (uint16_t)g_st.v; return true;
    case 0x117: *v = (uint16_t)g_st.i; return true;
    case 0x118: *v = (uint16_t)g_st.vin; return true;
    case 0x119: case 0x11A: case 0x11B: case 0x11C: case 0x11D: case 0x11E:
        *v = g_lc.diag[a - 0x119]; return true;      /* PA0/PA6 avg,min,max */
    default:
        if (a >= 0x100u && a < LC_REG_END) { *v = 0; return true; }   /* reserved */
        return false;
    }
}

bool lc_reg_writable(uint16_t a)
{
    return (a >= 0x100u && a <= 0x107u) || (a >= LC_REG_COEF && a < LC_REG_COEF + 4u * LC_N);
}

void lc_reg_write(uint16_t a, uint16_t v)
{
    if (a >= LC_REG_COEF && a < LC_REG_COEF + 4u * LC_N) {
        uint32_t ch = (a - LC_REG_COEF) / 4u, k = (a - LC_REG_COEF) % 4u;
        int32_t *p = (k < 2u) ? &s_lc.gain[ch] : &s_lc.offset[ch];
        uint32_t x = (uint32_t)*p;
        x = (k & 1u) ? ((x & 0xFFFF0000u) | v) : ((x & 0x0000FFFFu) | ((uint32_t)v << 16));
        *p = (int32_t)x;
        return;
    }
    switch (a) {
    case 0x100:
        g_lc.raw_mode = (v == 1u);
        if (!g_lc.raw_mode) { g_lc.force_vr = 0; g_lc.force_ir = 0; g_lc.force_vinr = 0; }
        break;
    case 0x101: g_lc.force_vr   = (uint8_t)((v <= 2u) ? v : 0u); break;
    case 0x102: g_lc.force_ir   = (uint8_t)((v <= 2u) ? v : 0u); break;
    case 0x103: g_lc.force_vinr = (uint8_t)((v <= 2u) ? v : 0u); break;
    case 0x104: g_lc.raw_cc = (v > 4095u) ? 4095u : v; break;
    case 0x105: g_lc.raw_cv = (v > 4095u) ? 4095u : v; break;
    case 0x106: g_lc.action = v; break;             /* executed from main loop */
    case 0x107: s_lc.valid = (uint16_t)(v & ((1u << LC_N) - 1u)); break;
    default: break;
    }
}

/* Flash actions run from the main loop (not inside the Modbus handler) */
void lc_poll(void)
{
    uint16_t act = g_lc.action;
    if (!act) return;
    g_lc.action = 0;
    bool ok = false;
    switch (act) {
    case 0x5A01: ok = lc_save();  break;
    case 0x5A02: ok = lc_erase(); break;
    case 0x5A03: ok = lc_load();  break;
    default: break;
    }
    g_lc.action_result = ok ? 0u : 1u;
    if (act == 0x5A01) stream_event(EVT_LC_SAVE, ok);
}
