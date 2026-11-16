/**
 * @file  app_modbus.c
 * @brief Modbus RTU slave on USART2 / RS-485 - same register map as the
 *        original firmware, plus extension blocks and a streaming function.
 *
 * Holding registers (FC 3 / 6 / 16):
 *   0 address      1 mode         2 on/off       3 set V        4 set I
 *   5 set P        6 set R        7 command      8 value        9 VIN gain
 *  10 V gain      11 I gain      12 I-DAC cal
 * Input registers (FC 4):
 *   0 address      1 mode/list step  2 on/list time  3 temperature
 *   4 aux (PA7)    5 VIN          6 V            7 I            8 P
 *   9 R           10 mAh         11 energy      12 seconds
 * Extension registers (FC 3/4 read, FC 6/16 write where allowed):
 *   0x100-0x147  linear calibration (app_lincal.c)
 *   0x180-0x190  stream control / statistics (app_stream.c);
 *                0x184/0x185 = runtime baud rate hi/lo (write both with FC16;
 *                applied after the reply; reverts to the saved rate if no valid
 *                frame arrives within 3 s at the new rate)
 *   0x1C0-0x1C3  current ramp up / down (u32 mA/s, high word first, 0 = no
 *                limit); applied at once and saved
 *   0x400-0x55D  config page RAM copy, one word per register (read only)
 *
 * FC 0x41 "stream read" (user-defined function code):
 *   request  [addr][0x41][max hi][max lo][crc]
 *   response [addr][0x41][seq][flags][len hi][len lo][len bytes of records][crc]
 *   flags: b0 ring overflow since last read, b1 fast overflow, b2 stream enabled
 */
#include "app.h"
#include "stm32f1xx_hal.h"

#define MB_NREGS   13u
#define MB_RXSZ    64u
#define MB_TXSZ    1040u
#define MB_MAX_RD  125u
#define MB_MAX_WR  ((MB_RXSZ - 9u) / 2u)
#define REG_CFG    0x400u

static volatile uint8_t  s_rx[MB_RXSZ];
static volatile uint8_t  s_rx_len, s_rx_overflow, s_rx_active, s_rx_ready;
static volatile uint16_t s_rx_idle, s_rx_gap = 110;    /* in 100 us ticks */
static volatile uint8_t  s_addr = 1;
static uint8_t  s_tx[MB_TXSZ];
static uint8_t  s_seq;

/* runtime baud switching */
static uint32_t s_baud_cur, s_baud_pending;
static uint16_t s_baud_hi_w;
static uint8_t  s_baud_trial, s_baud_ok;
static uint32_t s_baud_t0;

static const uint16_t s_crc_tab[256] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040,
};

static uint16_t crc16(const uint8_t *p, uint32_t n)
{
    uint16_t crc = 0xFFFF;
    while (n--) crc = (uint16_t)((crc >> 8) ^ s_crc_tab[(crc ^ *p++) & 0xFFu]);
    return crc;
}

static void set_gap_for(uint32_t bps)
{
    /* 2 x 3.5 characters (11 bits), at least 0.5 ms */
    uint32_t ticks = (2u * 35u * 11u * 1000u + bps - 1u) / bps;   /* 100 us units */
    s_rx_gap = (uint16_t)(ticks < 5u ? 5u : ticks);
}

static void apply_baud(uint32_t bps)
{
    usart2_set_bps(bps);
    s_baud_cur = bps;
    set_gap_for(bps);
}

void mb_set_address(uint8_t addr) { s_addr = addr; }

void mb_init(void)
{
    s_addr = (uint8_t)CFG(CFG_MB_ADDR);
    s_baud_trial = 0;
    apply_baud(baud_from_index(CFG(CFG_MB_BAUD)));
}

void mb_rx_byte_isr(uint8_t b)
{
    if (rs485_busy()) return;                    /* our own echo, if any */
    s_rx_active = 1;
    s_rx_idle = 0;
    if (s_rx_ready || s_rx_overflow) { s_rx_overflow = 1; return; }
    if (s_rx_len < MB_RXSZ) s_rx[s_rx_len++] = b;
    else s_rx_overflow = 1;
}

void mb_tick_isr(void)
{
    if (!s_rx_active) return;
    if (++s_rx_idle < s_rx_gap) return;
    s_rx_active = 0;
    if (!s_rx_overflow && !s_rx_ready && s_rx_len >= 4u &&
        (s_rx[0] == s_addr || s_rx[0] == 0u)) {
        s_rx_ready = 1;                          /* hand frame to main loop */
    } else if (!s_rx_ready) {
        s_rx_len = 0;
    }
    s_rx_overflow = 0;
}

/* ------------------------------------------------------------------------- */
static uint16_t hold_value_readback(void)
{
    switch (g_cmd) {
    case CMD_RD_VSCALE:
    case CMD_CAL_VDAC_LO:
    case CMD_CAL_VDAC_HI: return g_vdac_scale;
    case CMD_RD_IGAIN:    return (uint16_t)g_imeas_gain;
    case CMD_RD_VDAC_LO:  return CFG(CFG_VDAC_SCALE_LO);
    case CMD_RD_VDAC_HI:  return CFG(CFG_VDAC_SCALE_HI);
    case CMD_RD_V_ZERO:   return CFG(CFG_V_ZERO);
    case CMD_RD_I_ZERO:   return CFG(CFG_I_ZERO);
    case CMD_RD_VREF:     return CFG(CFG_VREF);
    case CMD_RD_ITRIM:    return CFG(CFG_ITRIM_LIMIT);
    default:              return (uint16_t)g_st.value;
    }
}

static void pack(uint16_t *hold, uint16_t *in)
{
    uint16_t mode = CFG(CFG_MODE);
    bool list = (mode == MODE_LIST2 || mode == MODE_LIST10);

    hold[0]  = CFG(CFG_MB_ADDR);
    hold[1]  = mode;
    hold[2]  = (uint16_t)g_st.on;
    hold[3]  = (uint16_t)g_st.set_v;
    hold[4]  = (uint16_t)((g_cmd == CMD_CAL_IMEAS) ? g_meter : g_st.set_i);
    hold[5]  = (uint16_t)g_st.set_p;
    hold[6]  = (uint16_t)g_st.set_r;
    hold[7]  = g_cmd;
    hold[8]  = hold_value_readback();
    hold[9]  = g_vin_gain_reg;
    hold[10] = g_v_gain_reg;
    hold[11] = (g_cmd == CMD_CAL_IMEAS) ? (uint16_t)g_imeas_gain : CFG(CFG_I_GAIN);
    hold[12] = g_idac_cal;

    in[0]  = CFG(CFG_MB_ADDR);
    in[1]  = list ? (uint16_t)g_st.list_step : mode;
    in[2]  = list ? (uint16_t)g_st.list_time : (uint16_t)g_st.on;
    in[3]  = (uint16_t)((g_cmd == CMD_TEMP_MCU) ? g_st.t_mcu :
                        (g_cmd == CMD_TEMP_NTC2) ? g_st.t_ntc2 : g_st.t_ntc1);
    in[4]  = (uint16_t)g_st.aux;
    in[5]  = (uint16_t)g_st.vin;
    in[6]  = (uint16_t)g_st.v;
    in[7]  = (uint16_t)((g_cmd == CMD_CAL_IDAC) ? g_meter : g_st.i);
    in[8]  = (uint16_t)g_st.p;
    in[9]  = (uint16_t)g_st.r;
    in[10] = (uint16_t)(g_st.uah / 1000u);
    in[11] = (uint16_t)(g_st.energy / 1000u);
    in[12] = (uint16_t)g_st.seconds;
}

static void unpack(const uint16_t *hold)
{
    CFG(CFG_MB_ADDR) = hold[0];
    CFG(CFG_MODE)    = hold[1];
    g_st.set_v = hold[3];
    ctl_host_set(hold[2], hold[4]);     /* on/off + set I: switching off ramps down */
    g_st.set_p = hold[5];
    g_st.set_r = hold[6];
    g_cmd      = hold[7];
    g_st.value = hold[8];
    g_vin_gain_reg = hold[9];
    g_v_gain_reg   = hold[10];
    if (hold[7] != CMD_CAL_IMEAS) CFG(CFG_I_GAIN) = hold[11];
    g_idac_cal = hold[12];
}

/* ------------------------------------------------------------------------- */
/* Extension registers                                                       */
/* ------------------------------------------------------------------------- */
static bool ext_read(uint16_t a, uint16_t *v)
{
    if (a >= 0x100u && a < LC_REG_END) return lc_reg_read(a, v);
    if (a == 0x184u) { *v = (uint16_t)(s_baud_cur >> 16); return true; }
    if (a == 0x185u) { *v = (uint16_t)s_baud_cur; return true; }
    if (a >= 0x180u && a <= 0x190u) return stream_reg_read(a, v);
    if (a >= RAMP_REG_FIRST && a <= RAMP_REG_LAST) return ramp_reg_read(a, v);
    if (a >= REG_CFG && a < REG_CFG + CFG_WORDS) { *v = g_cfg[a - REG_CFG]; return true; }
    return false;
}

static bool ext_writable(uint16_t a)
{
    return lc_reg_writable(a) || stream_reg_writable(a) || ramp_reg_writable(a) ||
           a == 0x184u || a == 0x185u;
}

static void ext_write(uint16_t a, uint16_t v)
{
    if (a == 0x184u) { s_baud_hi_w = v; return; }
    if (a == 0x185u) {
        uint32_t bps = ((uint32_t)s_baud_hi_w << 16) | v;
        if (bps >= 1200u && bps <= 1500000u) s_baud_pending = bps;
        return;
    }
    if (lc_reg_writable(a)) lc_reg_write(a, v);
    else if (ramp_reg_writable(a)) ramp_reg_write(a, v);
    else stream_reg_write(a, v);
}

/* ------------------------------------------------------------------------- */
static uint32_t exception(uint8_t *tx, uint8_t fn, uint8_t code)
{
    tx[1] = fn | 0x80u;
    tx[2] = code;
    return 3;
}

static void baud_service(void)
{
    if (s_baud_pending && !rs485_busy()) {       /* after the reply has left */
        uint32_t bps = s_baud_pending;
        s_baud_pending = 0;
        apply_baud(bps);
        s_baud_trial = 1;
        s_baud_ok = 0;
        s_baud_t0 = now_us();
        stream_event(EVT_BAUD, (uint16_t)(bps / 100u));
    }
    if (s_baud_trial && !s_baud_ok && (uint32_t)(now_us() - s_baud_t0) > 3000000u) {
        s_baud_trial = 0;                        /* nobody talked to us: revert */
        apply_baud(baud_from_index(CFG(CFG_MB_BAUD)));
        stream_event(EVT_BAUD, (uint16_t)(s_baud_cur / 100u));
    }
}

void mb_poll(void)
{
    baud_service();
    if (!s_rx_ready || rs485_busy()) return;

    uint8_t rx[MB_RXSZ];
    uint32_t len = s_rx_len, n = 0;
    for (uint32_t k = 0; k < len; k++) rx[k] = s_rx[k];
    s_rx_len = 0;
    s_rx_ready = 0;

    uint16_t crc = crc16(rx, len - 2u);
    if (rx[len - 2u] != (uint8_t)crc || rx[len - 1u] != (uint8_t)(crc >> 8)) return;
    s_baud_ok = 1;                               /* link works at this rate */

    bool bcast = (rx[0] == 0u);
    uint8_t fn = rx[1];
    uint16_t a = (uint16_t)((rx[2] << 8) | rx[3]);
    uint16_t c = (uint16_t)((rx[4] << 8) | rx[5]);
    uint16_t hold[MB_NREGS], in[MB_NREGS];
    bool wrote = false;
    uint8_t *tx = s_tx;

    pack(hold, in);
    tx[0] = rx[0];
    lc_comm_seen();

    bool ext = (a >= 0x100u);

    switch (fn) {
    case 0x03:
    case 0x04: {
        const uint16_t *src = (fn == 0x03) ? hold : in;
        if (len != 8u) return;
        if (c == 0u || c > (ext ? MB_MAX_RD : MB_NREGS)) { n = exception(tx, fn, 3); break; }
        if (!ext && a + c > MB_NREGS)                    { n = exception(tx, fn, 2); break; }
        tx[1] = fn;
        tx[2] = (uint8_t)(c * 2u);
        n = 3;
        for (uint16_t k = 0; k < c; k++) {
            uint16_t v = 0;
            if (ext) {
                if (!ext_read((uint16_t)(a + k), &v)) { n = exception(tx, fn, 2); break; }
            } else {
                v = src[a + k];
            }
            tx[n++] = (uint8_t)(v >> 8);
            tx[n++] = (uint8_t)v;
        }
        break;
    }
    case 0x06:
        if (len != 8u) return;
        if (ext) {
            if (!ext_writable(a)) { n = exception(tx, fn, 2); break; }
            ext_write(a, c);
        } else {
            if (a >= MB_NREGS) { n = exception(tx, fn, 2); break; }
            hold[a] = c;
            wrote = true;
        }
        for (n = 1; n < 6u; n++) tx[n] = rx[n];
        break;
    case 0x10: {
        uint8_t bc = rx[6];
        if (len < 9u || len != 9u + bc) return;
        if (c == 0u || c > (ext ? MB_MAX_WR : MB_NREGS) || bc != c * 2u) { n = exception(tx, fn, 3); break; }
        if (ext) {
            bool ok = true;
            for (uint16_t k = 0; k < c; k++) ok = ok && ext_writable((uint16_t)(a + k));
            if (!ok) { n = exception(tx, fn, 2); break; }
            for (uint16_t k = 0; k < c; k++)
                ext_write((uint16_t)(a + k), (uint16_t)((rx[7u + 2u * k] << 8) | rx[8u + 2u * k]));
        } else {
            if (a + c > MB_NREGS) { n = exception(tx, fn, 2); break; }
            for (uint16_t k = 0; k < c; k++)
                hold[a + k] = (uint16_t)((rx[7u + 2u * k] << 8) | rx[8u + 2u * k]);
            wrote = true;
        }
        for (n = 1; n < 6u; n++) tx[n] = rx[n];
        break;
    }
    case 0x41: {                                  /* stream read */
        if (len != 6u || bcast) return;
        uint32_t max = a;                         /* bytes 2..3 = max payload */
        if (max > MB_TXSZ - 8u) max = MB_TXSZ - 8u;
        uint8_t flags;
        uint32_t pl = stream_read(&tx[6], max, &flags);
        tx[1] = 0x41;
        tx[2] = s_seq++;
        tx[3] = flags;
        tx[4] = (uint8_t)(pl >> 8);
        tx[5] = (uint8_t)pl;
        n = 6u + pl;
        break;
    }
    default:
        n = exception(tx, fn, 1);
        break;
    }

    if (wrote) unpack(hold);
    if (bcast || n == 0u) return;

    crc = crc16(tx, n);
    tx[n++] = (uint8_t)crc;
    tx[n++] = (uint8_t)(crc >> 8);
    rs485_send(tx, n);
}
