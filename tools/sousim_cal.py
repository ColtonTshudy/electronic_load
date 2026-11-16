#!/usr/bin/env python3
"""
sousim_cal.py - calibration / service tool for the Sousim 500 W electronic load
(clean-sheet firmware in ../sousim_load), over Modbus RTU on USART2.

Requires: Python 3.8+, pyserial  (pip install pyserial)

Examples
  python sousim_cal.py -p COM5 scan                 find baud rate / address
  python sousim_cal.py -p COM5 status               live readings + calibration state
  python sousim_cal.py -p COM5 cal                  guided calibration (all stages)
  python sousim_cal.py -p COM5 cal --stages v,i     only selected stages
  python sousim_cal.py -p COM5 show                 print stored coefficients
  python sousim_cal.py -p COM5 save | reload | erase
  python sousim_cal.py -p COM5 disable I IDAC        factory tables for current (keeps values)
  python sousim_cal.py -p COM5 enable I_LO           re-use stored coefficients
  python sousim_cal.py -p COM5 clear VIN             factory tables + zero stored values
  python sousim_cal.py -p COM5 set-baud 115200

Calibration stages (each applies immediately; `save` happens at the end)
  v     voltage measurement, low + high range   (DMM across the input)
  i     current measurement + current DAC       (DMM in series, stiff source)
  vdac  CV DAC                                  (current-limited supply)
  vin   VIN measurement (optional second input) (DMM across VIN)

Measurement channels are calibrated against your DMM. The CV DAC is
calibrated against the load's own (freshly calibrated) voltage measurement.
"""
import argparse
import struct
import sys
import threading
import time

try:
    import serial
except ImportError:
    sys.exit("pyserial is required:  pip install pyserial")

BAUDS = [600, 1200, 2400, 4800, 7200, 9600, 14400, 19200,
         38400, 43000, 56000, 57600, 76800, 115200, 128000, 256000]

# --- legacy register map ------------------------------------------------------
H_ADDR, H_MODE, H_ON, H_SETV, H_SETI, H_SETP, H_SETR, H_CMD, H_VALUE = range(9)
I_TEMP, I_AUX, I_VIN, I_V, I_I, I_P, I_R, I_MAH, I_WH, I_SEC = range(3, 13)
CMD_SET_BAUD = 0xE8
MODE_CC = 1

# --- calibration block (see Core/Src/app_lincal.c) ----------------------------
R_RAW_MODE, R_FORCE_V, R_FORCE_I, R_FORCE_VIN = 0x100, 0x101, 0x102, 0x103
R_RAW_CC, R_RAW_CV, R_ACTION, R_VALID, R_VERSION = 0x104, 0x105, 0x106, 0x107, 0x108
R_SAMPLES = 0x110            # raw V, raw I, raw VIN, ranges, seq VI, seq VIN, V, I, VIN
R_COEF = 0x120
ACT_SAVE, ACT_ERASE, ACT_RELOAD = 0x5A01, 0x5A02, 0x5A03

CH = ["V_LO", "V_HI", "I_LO", "I_HI", "VIN_LO", "VIN_HI",
      "IDAC_LO", "IDAC_HI", "VDAC_LO", "VDAC_HI"]
CHI = {n: k for k, n in enumerate(CH)}
FORCE_LO, FORCE_HI = 1, 2

# Nominal current-DAC mapping (fitted from the factory tables), used only to
# pick DAC codes that land near the requested test currents.
NOMINAL_IDAC = {"LO": (0.4413, 33.0), "HI": (0.06573, 33.0)}   # code = a*mA + b


# =============================================================================
# Modbus RTU client
# =============================================================================
class ModbusError(Exception):
    pass


def crc16(data: bytes) -> int:
    crc = 0xFFFF
    for b in data:
        crc ^= b
        for _ in range(8):
            crc = (crc >> 1) ^ 0xA001 if crc & 1 else crc >> 1
    return crc


class Modbus:
    def __init__(self, port, baud, addr, timeout=0.6):
        self.addr = addr
        self.lock = threading.RLock()
        self.ser = serial.Serial(port, baud, bytesize=8, parity="N", stopbits=1,
                                 timeout=timeout, inter_byte_timeout=0.05)

    def set_baud(self, baud):
        with self.lock:
            self.ser.baudrate = baud

    def close(self):
        self.ser.close()

    def _xfer(self, pdu: bytes, expect: int) -> bytes:
        frame = bytes([self.addr]) + pdu
        frame += struct.pack("<H", crc16(frame))
        with self.lock:
            self.ser.reset_input_buffer()
            self.ser.write(frame)
            self.ser.flush()
            resp = self.ser.read(expect)
            if len(resp) < 5 and resp[1:2] and resp[1] & 0x80:
                resp += self.ser.read(5 - len(resp))
        if len(resp) < 5:
            raise ModbusError("no / short response (%d bytes)" % len(resp))
        if crc16(resp[:-2]) != struct.unpack("<H", resp[-2:])[0]:
            raise ModbusError("CRC error")
        if resp[1] & 0x80:
            raise ModbusError("exception code %d" % resp[2])
        return resp

    def read(self, reg, count=1, fn=3):
        r = self._xfer(struct.pack(">BHH", fn, reg, count), 5 + 2 * count)
        return list(struct.unpack(">%dH" % count, r[3:3 + 2 * count]))

    def write(self, reg, value):
        self._xfer(struct.pack(">BHH", 6, reg, value & 0xFFFF), 8)

    def write_many(self, reg, values):
        vals = [v & 0xFFFF for v in values]
        pdu = struct.pack(">BHHB", 16, reg, len(vals), 2 * len(vals))
        pdu += struct.pack(">%dH" % len(vals), *vals)
        self._xfer(pdu, 8)


# =============================================================================
# Device helper
# =============================================================================
class Load:
    def __init__(self, mb: Modbus):
        self.mb = mb
        self._ka_stop = threading.Event()
        self._ka = None
        self._saved_mode = None

    # ---- generic ----
    def hold(self):
        return self.mb.read(0, 13, fn=3)

    def inputs(self):
        return self.mb.read(0, 13, fn=4)

    def samples(self):
        r = self.mb.read(R_SAMPLES, 9)
        return dict(raw_v=r[0], raw_i=r[1], raw_vin=r[2], ranges=r[3],
                    seq_vi=r[4], seq_vin=r[5], v=r[6], i=r[7], vin=r[8])

    def wait_samples(self, n, key, seq_key, timeout=10.0):
        """Collect n fresh samples of `key` (new value each time `seq_key` changes)."""
        vals, last = [], self.samples()[seq_key]
        t0 = time.time()
        while len(vals) < n:
            if time.time() - t0 > timeout:
                raise ModbusError("timeout waiting for samples")
            s = self.samples()
            if s[seq_key] != last:
                last = s[seq_key]
                vals.append(s[key])
            else:
                time.sleep(0.01)
        return vals

    def avg(self, key, seq_key="seq_vi", n=16, settle=0.0):
        if settle:
            time.sleep(settle)
        v = self.wait_samples(n, key, seq_key)
        return sum(v) / len(v), max(v)

    # ---- coefficients ----
    def valid_mask(self):
        return self.mb.read(R_VALID)[0]

    def set_valid(self, ch, on=True):
        m = self.valid_mask()
        m = (m | (1 << ch)) if on else (m & ~(1 << ch))
        self.mb.write(R_VALID, m)

    def coef(self, ch):
        r = self.mb.read(R_COEF + 4 * ch, 4)
        g = struct.unpack(">i", struct.pack(">HH", r[0], r[1]))[0]
        o = struct.unpack(">i", struct.pack(">HH", r[2], r[3]))[0]
        return g, o

    def write_coef(self, ch, gain_f, offset_f):
        g = int(round(gain_f * 1e6))
        o = int(round(offset_f))
        if not (-2**31 <= g < 2**31):
            raise ValueError("gain out of range")
        regs = list(struct.unpack(">HHHH", struct.pack(">ii", g, o)))
        self.mb.write_many(R_COEF + 4 * ch, regs)
        self.set_valid(ch, True)

    def action(self, code):
        self.mb.write(R_ACTION, code)
        time.sleep(0.4)
        return self.mb.read(R_ACTION)[0] == 0

    # ---- raw mode ----
    def enter_raw(self):
        h = self.hold()
        self._saved_mode = h[H_MODE]
        self.mb.write(H_ON, 0)
        self.mb.write(H_MODE, MODE_CC)
        self.mb.write_many(H_SETV, [0, 0, 0, 0])
        self.mb.write_many(R_RAW_MODE, [1, 0, 0, 0, 0, 4095])   # raw, auto ranges, CC=0, CV=max
        self._ka_stop.clear()
        self._ka = threading.Thread(target=self._keepalive, daemon=True)
        self._ka.start()

    def exit_raw(self):
        self._ka_stop.set()
        for step in (lambda: self.mb.write(H_ON, 0),
                     lambda: self.mb.write_many(R_RAW_MODE, [0, 0, 0, 0, 0, 4095]),
                     lambda: self._saved_mode is not None and self.mb.write(H_MODE, self._saved_mode)):
            try:
                step()
            except Exception as e:  # keep going: get the load to a safe state
                print("  warning while leaving raw mode:", e)

    def _keepalive(self):
        while not self._ka_stop.wait(2.0):
            try:
                self.mb.read(R_RAW_MODE)
            except Exception:
                pass

    def on(self, state):
        self.mb.write(H_ON, 1 if state else 0)

    def force(self, v=None, i=None, vin=None):
        if v is not None:
            self.mb.write(R_FORCE_V, v)
        if i is not None:
            self.mb.write(R_FORCE_I, i)
        if vin is not None:
            self.mb.write(R_FORCE_VIN, vin)
        time.sleep(0.15)

    def cc(self, code):
        self.mb.write(R_RAW_CC, max(0, min(4095, int(code))))

    def cv(self, code):
        self.mb.write(R_RAW_CV, max(0, min(4095, int(code))))


# =============================================================================
# Helpers
# =============================================================================
def fit(points):
    """Least squares y = a*x + b over [(x, y), ...]."""
    n = len(points)
    if n < 2:
        raise ValueError("need at least two points")
    mx = sum(p[0] for p in points) / n
    my = sum(p[1] for p in points) / n
    sxx = sum((p[0] - mx) ** 2 for p in points)
    if sxx == 0:
        raise ValueError("points must differ")
    a = sum((p[0] - mx) * (p[1] - my) for p in points) / sxx
    return a, my - a * mx


def ask_float(prompt, allow_skip=True):
    while True:
        s = input(prompt).strip().lower()
        if allow_skip and s in ("s", "skip", ""):
            return None
        if s in ("q", "quit"):
            raise KeyboardInterrupt
        try:
            return float(s)
        except ValueError:
            print("  enter a number%s, or q to abort" % (", s to skip" if allow_skip else ""))


def confirm(prompt):
    return input(prompt + " [y/N] ").strip().lower() in ("y", "yes")


def report(name, a, b, pts, xunit, yunit):
    print("  %-8s y = %.6f*x %+.2f   (%d points)" % (name, a, b, len(pts)))
    for x, y in pts:
        print("           %9.1f %-5s -> %9.1f %-5s  residual %+.2f" % (x, xunit, y, yunit, y - (a * x + b)))


# =============================================================================
# Calibration stages
# =============================================================================
def stage_voltage(ld: Load):
    print("\n=== Voltage measurement (main input) ===")
    print("Load stays OFF. Apply a DC voltage to the load input and measure it with")
    print("your DMM at the load terminals. Use 2-4 different voltages spread across")
    print("what your source can do (e.g. 3 V, 15 V, 30 V). The low range only")
    print("covers up to ~38 V, so it is only sampled below 36 V.")
    ld.on(False)
    lo, hi = [], []
    while True:
        dmm = ask_float("\nDMM reading in volts (Enter = finished): ")
        if dmm is None:
            break
        ld.force(v=FORCE_HI)
        raw_hi, mx = ld.avg("raw_v", settle=0.3)
        hi.append((raw_hi, dmm * 100))
        print("  high range raw %.1f" % raw_hi)
        if dmm < 36.0:
            ld.force(v=FORCE_LO)
            raw_lo, mx = ld.avg("raw_v", settle=0.3)
            if mx >= 32700:
                print("  low range saturated - point skipped for low range")
            else:
                lo.append((raw_lo, dmm * 100))
                print("  low range raw %.1f" % raw_lo)
        ld.force(v=0)
    for name, pts in (("V_LO", lo), ("V_HI", hi)):
        if len(pts) < 2:
            print("  %s: not enough points, left unchanged" % name)
            continue
        a, b = fit(pts)
        report(name, a, b, pts, "raw", "10mV")
        ld.write_coef(CHI[name], a, b)


def stage_vin(ld: Load):
    print("\n=== VIN measurement (secondary voltage input) ===")
    print("Apply a known voltage to the VIN input (not the main terminals).")
    print("Low range covers up to ~32 V, high range up to ~65 V.")
    if not confirm("Do you want to calibrate VIN?"):
        return
    lo, hi = [], []
    while True:
        dmm = ask_float("\nDMM reading in volts at VIN (Enter = finished): ")
        if dmm is None:
            break
        ld.force(vin=FORCE_HI)
        raw_hi, _ = ld.avg("raw_vin", "seq_vin", n=6, settle=0.3)
        hi.append((raw_hi, dmm * 1000))
        print("  high range raw %.1f" % raw_hi)
        if dmm < 31.0:
            ld.force(vin=FORCE_LO)
            raw_lo, mx = ld.avg("raw_vin", "seq_vin", n=6, settle=0.3)
            if mx < 32700:
                lo.append((raw_lo, dmm * 1000))
                print("  low range raw %.1f" % raw_lo)
        ld.force(vin=0)
    for name, pts in (("VIN_LO", lo), ("VIN_HI", hi)):
        if len(pts) < 2:
            print("  %s: not enough points, left unchanged" % name)
            continue
        a, b = fit(pts)
        report(name, a, b, pts, "raw", "mV")
        ld.write_coef(CHI[name], a, b)


def _approach_current(ld: Load, rng: str, target_ma: float, max_ma: float):
    """Set a raw CC code that gives roughly target_ma, using the unit's own
    current reading for feedback. Returns the final code."""
    a, b = NOMINAL_IDAC[rng]
    code = a * target_ma + b
    ld.cc(code)
    ld.on(True)
    time.sleep(0.6)
    for _ in range(4):
        i_now, _ = ld.avg("i", n=6)
        if i_now > 1.15 * max_ma:
            break
        err = target_ma - i_now
        if abs(err) < 0.03 * target_ma:
            break
        code = max(0, min(4095, code + err * a))
        ld.cc(code)
        time.sleep(0.4)
    return int(round(code))


def stage_current(ld: Load):
    print("\n=== Current measurement + current DAC ===")
    print("Wiring: source (+) -> DMM (A range) -> load (+);  load (-) -> source (-)")
    print("The LOAD must set the current, so the source has to deliver more than")
    print("the largest test current (on a bench supply: current limit ABOVE it).")
    print("Use a low source voltage (3-12 V) to keep dissipation small.")
    max_a = ask_float("Maximum current your source/DMM can handle, in A: ", allow_skip=False)
    max_ma = max_a * 1000.0
    plans = {
        "LO": [t for t in (200.0, 1500.0, min(5000.0, 0.9 * max_ma)) if t <= 0.9 * max_ma],
        "HI": sorted({min(t, 0.9 * max_ma) for t in (1000.0, 0.45 * max_ma, 0.9 * max_ma) if t >= 500.0}),
    }
    for rng in ("LO", "HI"):
        targets = plans[rng]
        if len(targets) < 2:
            print("\n%s range: source too weak for two points, skipped" % rng)
            continue
        print("\n--- %s current range (forced) ---" % ("Low" if rng == "LO" else "High"))
        ld.force(i=FORCE_LO if rng == "LO" else FORCE_HI)
        meas_pts, dac_pts = [], []
        for t in targets:
            code = _approach_current(ld, rng, t, max_ma)
            time.sleep(1.0)
            raw, mx = ld.avg("raw_i", n=16)
            est, _ = ld.avg("i", n=4)
            print("  code %4d  -> unit reads ~%.0f mA (raw %.1f)" % (code, est, raw))
            dmm = ask_float("  DMM current in A (s = skip point): ")
            ld.on(False)
            ld.cc(0)
            if dmm is None:
                continue
            if mx >= 32700:
                print("  ADC saturated - point skipped")
                continue
            meas_pts.append((raw, dmm * 1000.0))
            dac_pts.append((dmm * 1000.0, code))
            time.sleep(0.5)
        ld.force(i=0)
        if len(meas_pts) < 2:
            print("  not enough points for %s range" % rng)
            continue
        a, b = fit(meas_pts)
        report("I_" + rng, a, b, meas_pts, "raw", "mA")
        ld.write_coef(CHI["I_" + rng], a, b)
        a, b = fit(dac_pts)
        report("IDAC_" + rng, a, b, dac_pts, "mA", "code")
        if a <= 0:
            print("  IDAC slope not positive - was the source limiting? not applied")
            continue
        ld.write_coef(CHI["IDAC_" + rng], a, b)


def _seek_cv(ld: Load, target_10mv: float, v_src: float):
    """Binary-search the CV DAC code that regulates at target; returns (V, code)."""
    lo, hi = 0, 4095
    best = None
    ld.on(True)
    for _ in range(12):
        mid = (lo + hi) // 2
        ld.cv(mid)
        time.sleep(0.35)
        v, _ = ld.avg("v", n=4)
        regulating = v < 0.97 * v_src
        if best is None or abs(v - target_10mv) < abs(best[0] - target_10mv):
            if regulating:
                best = (v, mid)
        if v < target_10mv:
            lo = mid + 1
        else:
            hi = mid - 1
        if lo > hi:
            break
    if best is None:
        return None
    ld.cv(best[1])
    time.sleep(0.6)
    v, _ = ld.avg("v", n=12)
    return v, best[1]


def stage_vdac(ld: Load):
    print("\n=== CV DAC (constant-voltage threshold) ===")
    print("Connect a CURRENT-LIMITED supply directly to the load input.")
    print("Set it to its highest convenient voltage and a current limit of ~0.3-1 A.")
    print("The load will pull the voltage down to the CV setpoint; the supply goes CC.")
    print("(Uses the load's own voltage measurement - calibrate 'v' first.)")
    ld.cc(0)
    for rng in ("LO", "HI"):
        if rng == "LO":
            print("\nLow range: set the supply to 36 V or less (low range tops out ~38 V).")
            v_src = ask_float("Supply voltage now, in V: ", allow_skip=False)
            if v_src > 36.0:
                print("  too high for the low range - skipped")
                continue
        else:
            print("\nHigh range: you may raise the supply voltage now (any value).")
            v_src = ask_float("Supply voltage now, in V: ", allow_skip=False)
        top = 0.85 * v_src
        targets = [max(2.0, 0.15 * top), top]
        if targets[1] - targets[0] < 2.0:
            print("%s range: supply voltage too low, skipped" % rng)
            continue
        print("\n--- %s voltage range (forced) ---" % ("Low" if rng == "LO" else "High"))
        ld.force(v=FORCE_LO if rng == "LO" else FORCE_HI)
        pts = []
        for t in targets:
            r = _seek_cv(ld, t * 100.0, v_src * 100.0)
            ld.on(False)
            ld.cv(4095)
            if r is None:
                print("  could not regulate near %.1f V (supply limit too high?)" % t)
                continue
            print("  code %4d -> %.2f V" % (r[1], r[0] / 100.0))
            pts.append((r[0], r[1]))
            time.sleep(0.5)
        ld.force(v=0)
        if len(pts) < 2:
            continue
        a, b = fit(pts)
        report("VDAC_" + rng, a, b, pts, "10mV", "code")
        if a > 0:
            ld.write_coef(CHI["VDAC_" + rng], a, b)


STAGES = {"v": stage_voltage, "i": stage_current, "vdac": stage_vdac, "vin": stage_vin}


# =============================================================================
# Commands
# =============================================================================
def cmd_cal(ld: Load, args):
    stages = [s.strip() for s in args.stages.split(",") if s.strip()]
    for s in stages:
        if s not in STAGES:
            sys.exit("unknown stage '%s' (use %s)" % (s, ",".join(STAGES)))
    ver = ld.mb.read(R_VERSION)[0]
    print("Firmware calibration interface version 0x%04X" % ver)
    print("Safety: the load switches off and leaves raw mode by itself if this tool")
    print("stops talking to it for 10 s. Ctrl+C / 'q' aborts at any prompt.")
    ld.enter_raw()
    try:
        for s in stages:
            STAGES[s](ld)
    except KeyboardInterrupt:
        print("\naborted.")
    finally:
        ld.exit_raw()
    print("\nNew coefficients are active now (RAM).")
    cmd_show(ld, args)
    if confirm("Save calibration to flash?"):
        print("saved." if ld.action(ACT_SAVE) else "SAVE FAILED")
    else:
        print("Not saved - power-cycle or run 'reload' to discard.")


def cmd_show(ld: Load, args):
    mask = ld.valid_mask()
    print("\nCalibration in use (valid mask 0x%03X):" % mask)
    for k, name in enumerate(CH):
        g, o = ld.coef(k)
        if mask & (1 << k):
            print("  %-8s LINEAR   gain %12.6f  offset %8d" % (name, g / 1e6, o))
        elif g or o:
            print("  %-8s FACTORY  (inactive stored values: gain %.6f offset %d)" % (name, g / 1e6, o))
        else:
            print("  %-8s FACTORY" % name)


def cmd_status(ld: Load, args):
    try:
        while True:
            h, i = ld.hold(), ld.inputs()
            s = ld.samples()
            print("V %7.2f V  I %7.3f A  P %8.2f W  VIN %7.3f V  T %3d C  on=%d mode=%d  "
                  "raw V/I %5d/%5d  ranges V%s I%s" % (
                      i[I_V] / 100, i[I_I] / 1000, i[I_P] / 100, i[I_VIN] / 1000, i[I_TEMP],
                      h[H_ON], h[H_MODE], s["raw_v"], s["raw_i"],
                      "H" if s["ranges"] & 1 else "L", "H" if s["ranges"] & 2 else "L"))
            if not args.follow:
                break
            time.sleep(0.5)
    except KeyboardInterrupt:
        pass
    if not args.follow:
        cmd_show(ld, args)


def cmd_action(code, label):
    def run(ld: Load, args):
        if code == ACT_ERASE and not confirm("Erase linear calibration (fall back to legacy)?"):
            return
        print("%s: %s" % (label, "ok" if ld.action(code) else "FAILED"))
    return run


def _channels(names):
    out = []
    for n in names:
        n = n.upper()
        if n == "ALL":
            out += list(range(len(CH)))
        elif n in ("I", "V", "VIN", "IDAC", "VDAC"):
            out += [CHI[n + "_LO"], CHI[n + "_HI"]]
        elif n in CHI:
            out.append(CHI[n])
        else:
            sys.exit("unknown channel '%s' (use %s, a group I/V/VIN/IDAC/VDAC, or ALL)" % (n, ", ".join(CH)))
    return sorted(set(out))


def cmd_channels(ld: Load, args):
    """enable / disable / clear channels, then save."""
    chans = _channels(args.channels)
    for ch in chans:
        if args.cmd == "enable":
            g, o = ld.coef(ch)
            if g == 0 and o == 0:
                print("  %s has no stored coefficients - not enabled" % CH[ch])
                continue
            ld.set_valid(ch, True)
        elif args.cmd == "disable":
            ld.set_valid(ch, False)
        else:  # clear: disable and zero the stored coefficients
            ld.set_valid(ch, False)
            ld.mb.write_many(R_COEF + 4 * ch, [0, 0, 0, 0])
    print("save:", "ok" if ld.action(ACT_SAVE) else "FAILED")
    cmd_show(ld, args)


def cmd_set_baud(ld: Load, args):
    if args.new_baud not in BAUDS:
        sys.exit("baud must be one of %s" % BAUDS)
    idx = BAUDS.index(args.new_baud)
    ld.mb.write(H_CMD, CMD_SET_BAUD)        # select parameter (firmware arms it)
    time.sleep(0.2)
    ld.mb.write(H_VALUE, idx)               # new value -> stored and saved
    time.sleep(0.5)
    ld.mb.set_baud(args.new_baud)
    ld.hold()
    print("now talking at %d baud (saved)" % args.new_baud)


def cmd_scan(port, addr):
    for baud in [115200, 9600] + [b for b in reversed(BAUDS) if b not in (115200, 9600)]:
        try:
            mb = Modbus(port, baud, addr, timeout=0.25)
        except serial.SerialException as e:
            sys.exit(str(e))
        try:
            h = mb.read(0, 2)
            print("found unit: %d baud, address %d, mode %d" % (baud, h[0], h[1]))
            return
        except ModbusError:
            pass
        finally:
            mb.close()
        print("  no answer at %d" % baud)
    print("not found - check wiring (A/B swapped?) and address (-a)")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("-p", "--port", required=True, help="serial port, e.g. COM5 or /dev/ttyUSB0")
    ap.add_argument("-b", "--baud", type=int, default=115200)
    ap.add_argument("-a", "--addr", type=int, default=1, help="Modbus address (default 1)")
    sub = ap.add_subparsers(dest="cmd", required=True)
    c = sub.add_parser("cal", help="guided calibration")
    c.add_argument("--stages", default="v,i,vdac,vin", help="comma list of v,i,vdac,vin")
    st = sub.add_parser("status", help="show readings")
    st.add_argument("-f", "--follow", action="store_true", help="keep printing")
    sub.add_parser("show", help="show calibration coefficients")
    sub.add_parser("save", help="save RAM calibration to flash")
    sub.add_parser("reload", help="reload calibration from flash")
    sub.add_parser("erase", help="erase linear calibration (legacy tables used)")
    for name, hlp in (("enable", "use stored linear coefficients for channels"),
                      ("disable", "fall back to factory tables for channels (keeps coefficients)"),
                      ("clear", "fall back to factory tables and zero the stored coefficients")):
        sp = sub.add_parser(name, help=hlp + "; saves to flash")
        sp.add_argument("channels", nargs="+", help="e.g. I_LO IDAC_HI, a group I V VIN IDAC VDAC, or ALL")
    sb = sub.add_parser("set-baud", help="change Modbus baud rate")
    sb.add_argument("new_baud", type=int)
    sub.add_parser("scan", help="find the unit's baud rate")
    args = ap.parse_args()

    if args.cmd == "scan":
        cmd_scan(args.port, args.addr)
        return

    mb = Modbus(args.port, args.baud, args.addr)
    ld = Load(mb)
    handlers = {
        "cal": cmd_cal, "status": cmd_status, "show": cmd_show, "set-baud": cmd_set_baud,
        "save": cmd_action(ACT_SAVE, "save"), "reload": cmd_action(ACT_RELOAD, "reload"),
        "erase": cmd_action(ACT_ERASE, "erase"),
        "enable": cmd_channels, "disable": cmd_channels, "clear": cmd_channels,
    }
    if not hasattr(args, "follow"):
        args.follow = False
    try:
        handlers[args.cmd](ld, args)
    except ModbusError as e:
        sys.exit("Modbus error: %s  (try 'scan')" % e)
    finally:
        mb.close()


if __name__ == "__main__":
    main()
