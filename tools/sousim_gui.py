#!/usr/bin/env python3
"""
sousim_gui.py - real-time monitor and control panel for the Sousim load
(clean-sheet firmware), over Modbus RTU / RS-485.

Data path: the firmware writes every measurement as a timestamped record into
a ring buffer at the sensor's native rate (ADS1115 V/I ~400 S/s each, VIN,
PA0/PA6/DAC fast channels up to 10 kHz, temperatures, controller state).
This program drains it continuously with Modbus function 0x41, interleaving
ordinary register reads/writes, and plots everything with pyqtgraph.

Requires: Python 3.9+, PySide6, pyqtgraph, numpy, pyserial

  python sousim_gui.py                     interactive
  python sousim_gui.py -p COM10            auto-connect
  python sousim_gui.py -p COM10 --screenshot out.png --run 8   (test)
"""
import argparse
import math
import os
import queue
import struct
import sys
import threading
import time
from collections import defaultdict

import numpy as np
import serial
import serial.tools.list_ports
from PySide6 import QtCore, QtGui, QtWidgets
import pyqtgraph as pg

BAUDS = [9600, 19200, 38400, 57600, 115200, 256000, 460800, 921600, 1000000, 1500000]
BAUD_TABLE = [600, 1200, 2400, 4800, 7200, 9600, 14400, 19200, 38400, 43000, 56000, 57600,
              76800, 115200, 128000, 256000, 460800, 921600, 1000000, 1500000]
CFG_BAUD_WORD = 0x26 // 2           # config page word holding the saved baud index

# =============================================================================
# Modbus RTU link
# =============================================================================
_CRC = []
for _i in range(256):
    _c = _i
    for _ in range(8):
        _c = (_c >> 1) ^ 0xA001 if _c & 1 else _c >> 1
    _CRC.append(_c)


def crc16(data) -> int:
    crc = 0xFFFF
    tab = _CRC
    for b in data:
        crc = (crc >> 8) ^ tab[(crc ^ b) & 0xFF]
    return crc


class LinkError(Exception):
    pass


class Link:
    """Request/response Modbus RTU master. Not thread safe: used only by Acq."""

    def __init__(self, port, baud, addr, timeout=0.25):
        self.port, self.addr = port, addr
        self.ser = serial.Serial(port, baud, bytesize=8, parity="N", stopbits=1,
                                 timeout=timeout, write_timeout=1.0)
        self.rx_bytes = 0
        self.tx_bytes = 0

    @property
    def baud(self):
        return self.ser.baudrate

    def set_baud(self, baud):
        self.ser.baudrate = baud
        time.sleep(0.02)
        self.ser.reset_input_buffer()

    def close(self):
        try:
            self.ser.close()
        except Exception:
            pass

    def _send(self, pdu: bytes):
        fr = bytes([self.addr]) + pdu
        fr += struct.pack("<H", crc16(fr))
        self.ser.reset_input_buffer()
        self.ser.write(fr)
        self.tx_bytes += len(fr)

    def _recv(self, n):
        b = self.ser.read(n)
        self.rx_bytes += len(b)
        if len(b) < n:
            raise LinkError("timeout (%d of %d bytes)" % (len(b), n))
        return b

    def _head(self, fn):
        h = self._recv(2)
        if h[1] == (fn | 0x80):
            rest = self._recv(3)
            raise LinkError("Modbus exception %d (fn 0x%02X)" % (rest[0], fn))
        if h[0] != self.addr or h[1] != fn:
            raise LinkError("bad reply header %s" % h.hex())
        return h

    @staticmethod
    def _check(frame):
        if crc16(frame[:-2]) != struct.unpack("<H", frame[-2:])[0]:
            raise LinkError("CRC error")

    def read(self, addr, count, fn=3):
        self._send(struct.pack(">BHH", fn, addr, count))
        h = self._head(fn)
        body = self._recv(1 + 2 * count + 2)
        self._check(h + body)
        return list(struct.unpack(">%dH" % count, body[1:1 + 2 * count]))

    def write(self, addr, value):
        self._send(struct.pack(">BHH", 6, addr, value & 0xFFFF))
        h = self._head(6)
        self._check(h + self._recv(6))

    def write_many(self, addr, values):
        vals = [v & 0xFFFF for v in values]
        pdu = struct.pack(">BHHB", 16, addr, len(vals), 2 * len(vals)) + struct.pack(">%dH" % len(vals), *vals)
        self._send(pdu)
        h = self._head(16)
        self._check(h + self._recv(6))

    def stream(self, maxlen):
        self._send(struct.pack(">BH", 0x41, maxlen))
        h = self._head(0x41)
        h2 = self._recv(4)
        n = (h2[2] << 8) | h2[3]
        body = self._recv(n + 2)
        self._check(h + h2 + body)
        return h2[0], h2[1], body[:-2]


# =============================================================================
# Channels
# =============================================================================
G_V, G_A, G_W, G_R = "Voltage [V]", "Current [A]", "Power [W]", "Resistance [Ohm]"
G_T, G_RAW, G_MCU, G_DAC = "Temperature [degC]", "ADS1115 raw [counts]", "MCU ADC [counts]", "DAC [codes]"
G_E, G_S, G_CAL = "Charge / energy", "State / flags", "Calibration factors"

# key: (label, unit, group, max points)
CHANNELS = {
    "V": ("Voltage", "V", G_V, 2_000_000),
    "I": ("Current", "A", G_A, 2_000_000),
    "P_vi": ("Power  V x I (per sample)", "W", G_W, 2_000_000),
    "R_vi": ("Resistance  V / I (per sample)", "Ohm", G_R, 2_000_000),
    "VIN": ("VIN", "V", G_V, 200_000),
    "set_V": ("Set V", "V", G_V, 300_000),
    "set_I": ("Set I", "A", G_A, 300_000),
    "ramp": ("I ramp (slew-limited set I)", "A", G_A, 300_000),
    "P": ("Power (firmware)", "W", G_W, 300_000),
    "set_P": ("Set P", "W", G_W, 300_000),
    "R": ("Resistance (firmware)", "Ohm", G_R, 300_000),
    "set_R": ("Set R", "Ohm", G_R, 300_000),
    "V_raw": ("V raw", "cnt", G_RAW, 2_000_000),
    "I_raw": ("I raw", "cnt", G_RAW, 2_000_000),
    "VIN_raw": ("VIN raw", "cnt", G_RAW, 200_000),
    "PA6_fast": ("PA6 fast (OPA2188 out, I sense)", "cnt", G_MCU, 4_000_000),
    "PA6_A": ("PA6 fast as current (user mapping)", "A", G_A, 4_000_000),
    "PA6_avg_A": ("PA6 burst avg as current", "A", G_A, 200_000),
    "PA0_fast": ("PA0 fast", "cnt", G_MCU, 4_000_000),
    "PA6_avg": ("PA6 burst avg", "cnt", G_MCU, 200_000),
    "PA6_min": ("PA6 burst min", "cnt", G_MCU, 200_000),
    "PA6_max": ("PA6 burst max", "cnt", G_MCU, 200_000),
    "PA0_avg": ("PA0 burst avg", "cnt", G_MCU, 200_000),
    "PA0_min": ("PA0 burst min", "cnt", G_MCU, 200_000),
    "PA0_max": ("PA0 burst max", "cnt", G_MCU, 200_000),
    "aux": ("Aux PA7", "fw", G_MCU, 200_000),
    "CC_DAC_fast": ("CC DAC fast", "code", G_DAC, 4_000_000),
    "CV_DAC_fast": ("CV DAC fast", "code", G_DAC, 4_000_000),
    "dac_cc": ("CC DAC", "code", G_DAC, 300_000),
    "dac_cv": ("CV DAC", "code", G_DAC, 300_000),
    "NTC1": ("NTC1", "degC", G_T, 200_000),
    "NTC2": ("NTC2", "degC", G_T, 200_000),
    "T_MCU": ("MCU temperature", "degC", G_T, 200_000),
    "charge": ("Charge", "Ah", G_E, 200_000),
    "energy": ("Energy", "Wh", G_E, 200_000),
    "runtime": ("Run time", "s", G_E, 200_000),
    "on": ("Load on", "", G_S, 300_000),
    "stopping": ("Ramping to 0 after set I = 0", "", G_S, 300_000),
    "mode": ("Mode", "", G_S, 300_000),
    "cmd": ("Command reg", "", G_S, 300_000),
    "value": ("Value reg", "", G_S, 300_000),
    "fan": ("Fan speed", "/200", G_S, 300_000),
    "list_step": ("List step", "", G_S, 300_000),
    "list_time": ("List step time", "0.1ms", G_S, 300_000),
    "V_range": ("V range high (sample)", "", G_S, 2_000_000),
    "I_range": ("I range high (sample)", "", G_S, 2_000_000),
    "VIN_range": ("VIN range high (sample)", "", G_S, 200_000),
    "f_irange": ("I range pin", "", G_S, 300_000),
    "f_vrange": ("V range pin", "", G_S, 300_000),
    "f_alarm": ("Alarm", "", G_S, 300_000),
    "f_settled": ("DAC settled", "", G_S, 300_000),
    "f_raw": ("Raw (cal) mode", "", G_S, 300_000),
    "f_aux": ("AUX out pin", "", G_S, 300_000),
    "f_inh": ("Load inhibit pin", "", G_S, 300_000),
    "idac_cal": ("I-DAC cal factor (legacy)", "", G_CAL, 300_000),
    "vdac_scale": ("V-DAC scale (legacy)", "", G_CAL, 300_000),
    "imeas_gain": ("I meas gain (legacy)", "", G_CAL, 300_000),
    "vin_gain_reg": ("VIN gain reg", "", G_CAL, 300_000),
    "v_gain_reg": ("V gain reg", "", G_CAL, 300_000),
}
GROUP_ORDER = [G_V, G_A, G_W, G_R, G_RAW, G_MCU, G_DAC, G_T, G_E, G_S, G_CAL]
EVENTS = {1: "PROTECTION TRIP", 2: "protection cleared", 3: "config saved", 4: "calibration saved",
          5: "baud rate changed (x100)", 6: "ADS1115 bus error", 7: "stream started"}
MODES = [(0, "CV"), (1, "CC"), (2, "CP"), (3, "CR"), (4, "CC fast (same as CC)"), (5, "CR2"),
         (9, "List x2"), (10, "List x10")] + [(11 + k, "Preset %d" % (k + 1)) for k in range(10)]


# =============================================================================
# Data store
# =============================================================================
class Series:
    __slots__ = ("t", "v", "n", "cap", "maxcap", "total", "last")

    def __init__(self, maxcap):
        self.cap = 4096
        self.maxcap = maxcap
        self.t = np.empty(self.cap)
        self.v = np.empty(self.cap)
        self.n = 0
        self.total = 0
        self.last = math.nan

    def append(self, t, v):
        k = len(t)
        if k == 0:
            return
        if self.n + k > self.cap:
            need = self.n + k
            if need <= self.maxcap:
                cap = min(self.maxcap, max(need, self.cap * 2))
                nt, nv = np.empty(cap), np.empty(cap)
                nt[:self.n] = self.t[:self.n]
                nv[:self.n] = self.v[:self.n]
                self.t, self.v, self.cap = nt, nv, cap
            else:                                   # keep newest half (new arrays)
                if k > self.maxcap // 2:
                    t, v, k = t[-(self.maxcap // 2):], v[-(self.maxcap // 2):], self.maxcap // 2
                keep = min(self.n, self.maxcap // 2)
                nt, nv = np.empty(self.maxcap), np.empty(self.maxcap)
                nt[:keep] = self.t[self.n - keep:self.n]
                nv[:keep] = self.v[self.n - keep:self.n]
                self.t, self.v, self.cap, self.n = nt, nv, self.maxcap, keep
        self.t[self.n:self.n + k] = t
        self.v[self.n:self.n + k] = v
        self.n += k
        self.total += k
        self.last = float(v[-1])


class Store:
    def __init__(self):
        self.lock = threading.Lock()
        self.s = {k: Series(c[3]) for k, c in CHANNELS.items()}
        self.tmax = 0.0

    def append_batch(self, batch):
        with self.lock:
            for k, (t, v) in batch.items():
                if len(t):
                    self.s[k].append(np.asarray(t, dtype=float), np.asarray(v, dtype=float))
                    tl = float(t[-1])
                    if tl > self.tmax:
                        self.tmax = tl

    def snap(self, k):
        with self.lock:
            s = self.s[k]
            return s.t, s.v, s.n

    def clear(self):
        with self.lock:
            self.s = {k: Series(c[3]) for k, c in CHANNELS.items()}


# =============================================================================
# Record decoder
# =============================================================================
ST_FMT = struct.Struct("<IBB15HBBBB3H")
SLOW_FMT = struct.Struct("<IH3H3HBBHIII")


PA6_DEFAULT = {0: None, 1: None}    # A = k * counts + b per I range; entered by the user
RAMP_MAX_AS = 1_000_000.0           # firmware clamps ramp rates to 1e9 mA/s


class Decoder:
    def __init__(self):
        self.last = None
        self.ext = 0
        self.last_v = math.nan
        self.pdiv = 1
        self.irange = 0                         # range of the latest I sample
        self.pa6 = dict(PA6_DEFAULT)            # replaced atomically from the GUI

    def pa6_amps(self, counts):
        kb = self.pa6.get(self.irange)
        if kb is None:
            return np.full_like(np.asarray(counts, dtype=float), np.nan)
        return np.asarray(counts, dtype=float) * kb[0] + kb[1]

    def t(self, ts):
        if self.last is None:
            self.last = ts
            return 0.0
        d = (ts - self.last) & 0xFFFFFFFF
        if d >= 0x80000000:
            d -= 0x100000000
        if d < -3_000_000:                      # device reset: continue the time axis
            d = 0
        self.ext += d
        self.last = ts
        return self.ext * 1e-6

    def decode(self, p):
        out = defaultdict(lambda: ([], []))
        chunks = defaultdict(list)                 # fast channels: numpy chunks
        events = []

        def add(k, t, v):
            o = out[k]
            o[0].append(t)
            o[1].append(v)

        i, n = 0, len(p)
        while i + 2 <= n:
            typ, ln = p[i], p[i + 1]
            b = i + 2
            i = b + ln
            if i > n:
                break
            if typ in (1, 2, 3) and ln >= 8:
                ts, raw, val = struct.unpack_from("<IHH", p, b)
                t = self.t(ts)
                hi = raw >> 15
                raw &= 0x7FFF
                if typ == 1:
                    v = val / 100.0
                    self.last_v = v
                    add("V", t, v); add("V_raw", t, raw); add("V_range", t, hi)
                elif typ == 2:
                    a = val / 1000.0
                    self.irange = hi
                    add("I", t, a); add("I_raw", t, raw); add("I_range", t, hi)
                    add("P_vi", t, self.last_v * a)
                    add("R_vi", t, self.last_v / a if a > 0.0005 else math.nan)
                else:
                    add("VIN", t, val / 1000.0); add("VIN_raw", t, raw); add("VIN_range", t, hi)
            elif typ == 4 and ln >= SLOW_FMT.size:
                (ts, aux, a0, a1, a2, s0, s1, s2, n1, n2, tm, uah, en, sec) = SLOW_FMT.unpack_from(p, b)
                t = self.t(ts)
                add("aux", t, aux)
                add("PA0_avg", t, a0); add("PA0_min", t, a1); add("PA0_max", t, a2)
                add("PA6_avg", t, s0); add("PA6_min", t, s1); add("PA6_max", t, s2)
                add("PA6_avg_A", t, float(self.pa6_amps(s0)))
                add("NTC1", t, n1); add("NTC2", t, n2)
                add("T_MCU", t, -(tm - 9000) / 10.0 if tm >= 9000 else tm / 10.0)
                add("charge", t, uah * 1e-6); add("energy", t, en * 1e-5); add("runtime", t, sec)
            elif typ == 5 and ln >= ST_FMT.size:
                f = ST_FMT.unpack_from(p, b)
                (ts, on, mode, cmd, value, sv, si, sp, sr, pp, rr, dcc, dcv, ramp, stopping,
                 idac, vds, img, flags, fan, lstep, pdiv, ltime, vin_g, v_g) = f
                t = self.t(ts)
                self.pdiv = pdiv or 1
                pw = 0.01 * self.pdiv
                for k, v in (("on", on), ("mode", mode), ("cmd", cmd), ("value", value),
                             ("set_V", sv / 100.0), ("set_I", si / 1000.0), ("set_P", sp * pw),
                             ("set_R", sr / 10.0), ("P", pp * pw), ("R", rr / 10.0),
                             ("dac_cc", dcc), ("dac_cv", dcv), ("ramp", ramp / 1000.0), ("stopping", stopping),
                             ("idac_cal", idac), ("vdac_scale", vds), ("imeas_gain", img),
                             ("f_irange", flags & 1), ("f_vrange", (flags >> 1) & 1),
                             ("f_alarm", (flags >> 3) & 1), ("f_settled", (flags >> 4) & 1),
                             ("f_raw", (flags >> 5) & 1), ("f_aux", (flags >> 6) & 1),
                             ("f_inh", (flags >> 7) & 1), ("fan", fan), ("list_step", lstep),
                             ("list_time", ltime), ("vin_gain_reg", vin_g), ("v_gain_reg", v_g)):
                    add(k, t, v)
            elif typ == 6 and ln >= 10:
                ts, period, mask, cnt = struct.unpack_from("<IIBB", p, b)
                keys = [k for bit, k in ((1, "PA6_fast"), (2, "PA0_fast"), (4, "CC_DAC_fast"), (8, "CV_DAC_fast"))
                        if mask & bit]
                nch = len(keys)
                if nch and cnt and ln >= 10 + 2 * cnt * nch:
                    d = np.frombuffer(p, dtype="<u2", count=cnt * nch, offset=b + 10).reshape(cnt, nch).astype(float)
                    t0 = self.t(ts)
                    tt = t0 + np.arange(cnt) * (period * 1e-6)
                    for j, k in enumerate(keys):
                        col = d[:, j].copy()
                        if k in ("PA6_fast", "PA0_fast"):
                            col[col == 0xFFFF] = np.nan
                        chunks[k].append((tt, col))
                        if k == "PA6_fast":
                            chunks["PA6_A"].append((tt, self.pa6_amps(col)))
            elif typ == 7 and ln >= 7:
                ts, code, data = struct.unpack_from("<IBH", p, b)
                events.append((self.t(ts), code, data))
        res = {k: (np.asarray(t, dtype=float), np.asarray(v, dtype=float)) for k, (t, v) in out.items()}
        for k, cl in chunks.items():
            res[k] = (np.concatenate([c[0] for c in cl]), np.concatenate([c[1] for c in cl]))
        return res, events


# =============================================================================
# Acquisition thread
# =============================================================================
class Acq(threading.Thread):
    def __init__(self, link: Link, store: Store, ui_q: queue.Queue):
        super().__init__(daemon=True)
        self.link, self.store, self.ui_q = link, store, ui_q
        self.cmd_q = queue.Queue()
        self.running = True
        self.streaming = True
        self.maxlen = 1024
        self.reg_period = 0.5
        self.dec = Decoder()
        self.rec_file = None
        self.rec_lock = threading.Lock()
        self.stats = dict(polls=0, bytes=0, errors=0, seq_gaps=0, ovf=0, fovf=0)
        self._last_seq = None

    def submit(self, fn, cb=None):
        self.cmd_q.put((fn, cb))

    def stop(self):
        self.running = False

    def start_recording(self, path):
        with self.rec_lock:
            self.rec_file = open(path, "w", buffering=1 << 20)
            self.rec_file.write("t_s,channel,value\n")

    def stop_recording(self):
        with self.rec_lock:
            if self.rec_file:
                self.rec_file.close()
            self.rec_file = None

    def _record(self, batch):
        with self.rec_lock:
            f = self.rec_file
            if not f:
                return
            for k, (t, v) in batch.items():
                f.write("".join("%.6f,%s,%.9g\n" % (a, k, b) for a, b in zip(t, v)))

    def _refresh_regs(self):
        L = self.link
        snap = {}
        snap["hold"] = L.read(0, 13, 3)
        snap["input"] = L.read(0, 13, 4)
        snap["x100"] = L.read(0x100, 9)
        snap["x110"] = L.read(0x110, 15)
        snap["x120"] = L.read(0x120, 40)
        snap["x180"] = L.read(0x180, 17)
        try:
            snap["x1C0"] = L.read(0x1C0, 4)           # ramp block (firmware with direct ramp registers)
        except LinkError:
            snap["x1C0"] = None
        snap["baud"] = L.baud
        self.ui_q.put(("regs", snap))

    def run(self):
        next_regs = 0.0
        fails = 0
        try:
            self.link.write(0x180, 1)                 # stream on
        except Exception as e:
            self.ui_q.put(("log", "stream enable failed: %s" % e))
        while self.running:
            try:
                while True:
                    try:
                        fn, cb = self.cmd_q.get_nowait()
                    except queue.Empty:
                        break
                    try:
                        r = fn(self.link)
                        self.ui_q.put(("cb", cb, r, None))
                    except Exception as e:
                        self.ui_q.put(("cb", cb, None, e))

                if time.monotonic() >= next_regs:
                    next_regs = time.monotonic() + self.reg_period
                    self._refresh_regs()

                if self.streaming:
                    seq, flags, payload = self.link.stream(self.maxlen)
                    st = self.stats
                    st["polls"] += 1
                    st["bytes"] += len(payload) + 8
                    if self._last_seq is not None and seq != ((self._last_seq + 1) & 0xFF):
                        st["seq_gaps"] += 1
                    self._last_seq = seq
                    if flags & 1:
                        st["ovf"] += 1
                    if flags & 2:
                        st["fovf"] += 1
                    if payload:
                        batch, events = self.dec.decode(payload)
                        self.store.append_batch(batch)
                        if self.rec_file:
                            self._record(batch)
                        for ev in events:
                            self.ui_q.put(("event", ev))
                    if not (flags & 4):
                        self.link.write(0x180, 1)
                    if len(payload) < 64:
                        time.sleep(0.001)
                else:
                    time.sleep(0.02)
                fails = 0
            except (LinkError, serial.SerialException, OSError) as e:
                self.stats["errors"] += 1
                fails += 1
                self.ui_q.put(("log", "link: %s" % e))
                if fails >= 8:
                    self.ui_q.put(("lost", str(e)))
                    break
                time.sleep(0.05)
            except Exception as e:                      # decoder / unexpected: keep running
                self.stats["errors"] += 1
                self.ui_q.put(("log", "acquisition error: %r" % e))
                time.sleep(0.05)
        try:
            self.link.write(0x180, 0)
        except Exception:
            pass


# =============================================================================
# GUI widgets
# =============================================================================
def dspin(lo, hi, dec, step, suffix=""):
    s = QtWidgets.QDoubleSpinBox()
    s.setRange(lo, hi)
    s.setDecimals(dec)
    s.setSingleStep(step)
    s.setKeyboardTracking(False)
    if suffix:
        s.setSuffix(" " + suffix)
    return s


class ControlPanel(QtWidgets.QWidget):
    COMMANDS = [
        (0, "0  None / clear", "direct"),
        (1, "1  Fan full speed", "direct"),
        (2, "2  Beep", "direct"),
        (7, "7  Temp reg = MCU", "direct"),
        (8, "8  Temp reg = NTC2", "direct"),
        (10, "10  Reset charge/energy", "direct"),
        (0x0B, "0x0B  Read active V-DAC scale", "read"),
        (0x0C, "0x0C  Read I meas gain", "read"),
        (0x0D, "0x0D  Read V-DAC scale low (cfg)", "read"),
        (0x0E, "0x0E  Read V-DAC scale high (cfg)", "read"),
        (0x0F, "0x0F  Read V zero", "read"),
        (0x10, "0x10  Read I zero", "read"),
        (0x11, "0x11  Read Vref", "read"),
        (0x12, "0x12  Read I trim limit", "read"),
        (0x4D, "0x4D  Param: V from VIN", "param"),
        (0x58, "0x58  Param: cut-off voltage", "param"),
        (0xE8, "0xE8  Param: saved baud index", "param"),
        (500, "500  Param: power resolution", "param"),
        (0x1E5, "0x1E5  Param: Modbus address", "param"),
        (1000, "1000  Param: slew down (legacy, 5 A/s units - use Current ramp)", "param"),
        (1001, "1001  Param: slew up (legacy, 5 A/s units - use Current ramp)", "param"),
        (0xFFF8, "-8  Param: range fallback", "param"),
        (0xFFFA, "-6  Set I trim limit = value", "value"),
        (0xFFFB, "-5  Set Vmax = value", "value"),
        (0xFFFC, "-4  Set Imax = value", "value"),
        (0xFFFD, "-3  Set Pmax = value", "value"),
        (0xFFFE, "-2  Set Vref = value (>= 2000)", "value"),
        (0xFFFF, "-1  AUX out high while off", "direct"),
        (1010, "1010  Set V zero from reading", "direct"),
        (1011, "1011  Clear V zero", "direct"),
        (2010, "2010  Set I zero from reading", "direct"),
        (2011, "2011  Clear I zero", "direct"),
        (0x78, "0x78  Save config to flash", "direct"),
        (0x79, "0x79  Store I-DAC factor for set I", "direct"),
        (0x6E, "0x6E  FACTORY cal: I measurement (SCPI)", "danger"),
        (0x6F, "0x6F  FACTORY cal: I DAC", "danger"),
        (0x73, "0x73  FACTORY cal: V DAC low", "danger"),
        (0x74, "0x74  FACTORY cal: V DAC high", "danger"),
        (0x76, "0x76  FACTORY cal: V measurement", "danger"),
        (0x77, "0x77  FACTORY cal: VIN measurement", "danger"),
    ]

    def __init__(self, win):
        super().__init__()
        self.win = win
        lay = QtWidgets.QVBoxLayout(self)

        row = QtWidgets.QHBoxLayout()
        self.b_on = QtWidgets.QPushButton("LOAD ON")
        self.b_off = QtWidgets.QPushButton("LOAD OFF  (Esc)")
        for b, c in ((self.b_on, "#2e7d32"), (self.b_off, "#c62828")):
            b.setMinimumHeight(40)
            b.setStyleSheet("QPushButton{background:%s;color:white;font-weight:bold;font-size:14px}" % c)
            row.addWidget(b)
        self.b_on.clicked.connect(lambda: win.write_hold(2, 1))
        self.b_off.clicked.connect(lambda: win.write_hold(2, 0))
        lay.addLayout(row)
        self.l_state = QtWidgets.QLabel("-")
        self.l_state.setStyleSheet("font-size:13px")
        lay.addWidget(self.l_state)

        g = QtWidgets.QGroupBox("Mode and setpoints")
        f = QtWidgets.QGridLayout(g)
        self.c_mode = QtWidgets.QComboBox()
        for code, name in MODES:
            self.c_mode.addItem("%s (%d)" % (name, code), code)
        b = QtWidgets.QPushButton("Set")
        b.clicked.connect(lambda: win.write_hold(1, self.c_mode.currentData()))
        f.addWidget(QtWidgets.QLabel("Mode"), 0, 0); f.addWidget(self.c_mode, 0, 1); f.addWidget(b, 0, 2)
        self.s_v = dspin(0, 655.35, 2, 0.1, "V")
        self.s_i = dspin(0, 65.535, 3, 0.01, "A")
        self.s_p = dspin(0, 6553.5, 2, 0.5, "W")
        self.s_r = dspin(0, 6553.5, 1, 0.1, "Ohm")
        for r, (lab, sp, reg, conv) in enumerate((
                ("Set V", self.s_v, 3, lambda x: round(x * 100)),
                ("Set I", self.s_i, 4, lambda x: round(x * 1000)),
                ("Set P", self.s_p, 5, lambda x: round(x / (0.01 * win.dec_pdiv()))),
                ("Set R", self.s_r, 6, lambda x: round(x * 10))), start=1):
            bt = QtWidgets.QPushButton("Set")
            bt.clicked.connect(lambda _=False, sp=sp, reg=reg, conv=conv: win.write_hold(reg, conv(sp.value())))
            f.addWidget(QtWidgets.QLabel(lab), r, 0); f.addWidget(sp, r, 1); f.addWidget(bt, r, 2)
        lay.addWidget(g)

        g = QtWidgets.QGroupBox("Current ramp (registers 0x1C0-0x1C3, saved on write)")
        f = QtWidgets.QGridLayout(g)
        self.s_rup = dspin(0, RAMP_MAX_AS, 3, 1.0, "A/s")
        self.s_rdn = dspin(0, RAMP_MAX_AS, 3, 1.0, "A/s")
        bt = QtWidgets.QPushButton("Apply")
        bt.clicked.connect(self.apply_ramp)
        f.addWidget(QtWidgets.QLabel("Ramp up"), 0, 0); f.addWidget(self.s_rup, 0, 1)
        f.addWidget(QtWidgets.QLabel("Ramp down"), 1, 0); f.addWidget(self.s_rdn, 1, 1)
        f.addWidget(bt, 0, 2, 2, 1)
        self.l_ramp = QtWidgets.QLabel("present: -")
        self.l_ramp.setWordWrap(True)
        f.addWidget(self.l_ramp, 2, 0, 1, 3)
        note = QtWidgets.QLabel("0 A/s = no limit. Load on ramps up from 0; setpoint changes (including "
                                "set I = 0) ramp. Load off and faults cut off immediately.")
        note.setWordWrap(True)
        note.setStyleSheet("color:gray")
        f.addWidget(note, 3, 0, 1, 3)
        self._ramp_loaded = False
        lay.addWidget(g)

        g = QtWidgets.QGroupBox("Command register (7) / value register (8)")
        f = QtWidgets.QGridLayout(g)
        self.c_cmd = QtWidgets.QComboBox()
        for code, name, kind in self.COMMANDS:
            self.c_cmd.addItem(name, (code, kind))
        self.s_val = QtWidgets.QSpinBox()
        self.s_val.setRange(0, 65535)
        self.b_send = QtWidgets.QPushButton("Send")
        self.b_wval = QtWidgets.QPushButton("Write value")
        self.l_cmd = QtWidgets.QLabel("")
        self.l_cmd.setWordWrap(True)
        f.addWidget(self.c_cmd, 0, 0, 1, 3)
        f.addWidget(QtWidgets.QLabel("Value"), 1, 0); f.addWidget(self.s_val, 1, 1); f.addWidget(self.b_send, 1, 2)
        f.addWidget(self.b_wval, 2, 2); f.addWidget(self.l_cmd, 2, 0, 1, 2)
        self.b_send.clicked.connect(self.send_cmd)
        self.b_wval.clicked.connect(lambda: win.write_hold(8, self.s_val.value()))
        self.c_cmd.currentIndexChanged.connect(self._kind_hint)
        self._kind_hint()
        lay.addWidget(g)

        row = QtWidgets.QHBoxLayout()
        for txt, fn in (("Save config", lambda: win.write_hold(7, 0x78)),
                        ("Reset energy", lambda: win.write_hold(7, 10)),
                        ("Clear cmd", lambda: win.write_hold(7, 0))):
            bt = QtWidgets.QPushButton(txt)
            bt.clicked.connect(fn)
            row.addWidget(bt)
        lay.addLayout(row)
        lay.addStretch(1)

    def _kind_hint(self):
        code, kind = self.c_cmd.currentData()
        hint = {"direct": "Send writes the command.",
                "read": "Send selects it; the value register shows the reading.",
                "param": "Send selects it and loads the present setting into Value; "
                         "edit Value, then 'Write value' to store and save.",
                "value": "Send writes Value and the command together (applies and saves).",
                "danger": "Factory calibration sequence - needs the original SCPI bench setup."}[kind]
        self.l_cmd.setText(hint)
        self.b_wval.setEnabled(kind == "param")

    def send_cmd(self):
        code, kind = self.c_cmd.currentData()
        w = self.win
        if kind == "danger":
            if QtWidgets.QMessageBox.question(self, "Factory calibration",
                                              "Start factory calibration command 0x%02X?\n"
                                              "It drives the load on its own and overwrites calibration."
                                              % code) != QtWidgets.QMessageBox.Yes:
                return
        if kind == "value":
            w.acq_submit(lambda L: L.write_many(7, [code, self.s_val.value()]))
        elif kind in ("param", "read"):
            def go(L):
                L.write(7, code)
                time.sleep(0.15)
                return L.read(8, 1)[0]
            w.acq_submit(go, lambda r: self.s_val.setValue(r))
        else:
            w.write_hold(7, code)

    def apply_ramp(self):
        up = int(round(self.s_rup.value() * 1000))
        dn = int(round(self.s_rdn.value() * 1000))
        regs = [up >> 16, up & 0xFFFF, dn >> 16, dn & 0xFFFF]
        self.win.acq_submit(lambda L: L.write_many(0x1C0, regs),
                            lambda _: self.win.log("ramp: up %.3f A/s, down %.3f A/s" % (up / 1000, dn / 1000)))

    def update_ramp(self, x):
        if x is None:
            self.l_ramp.setText("present: not available (firmware without registers 0x1C0-0x1C3)")
            return
        up, dn = (x[0] << 16) | x[1], (x[2] << 16) | x[3]
        fmt = lambda m: "no limit" if m == 0 else "%.3f A/s" % (m / 1000)
        self.l_ramp.setText("present: up %s, down %s" % (fmt(up), fmt(dn)))
        if not self._ramp_loaded:
            self._ramp_loaded = True
            self.s_rup.setValue(up / 1000)
            self.s_rdn.setValue(dn / 1000)

    def update_state(self, hold, inp):
        on, mode = hold[2], hold[1]
        name = dict(MODES).get(mode, str(mode))
        self.l_state.setText("<b>%s</b>  mode %s  |  set V %.2f V  I %.3f A  |  cmd %d  value %d" %
                             ("ON" if on else "off", name, hold[3] / 100, hold[4] / 1000, hold[7], hold[8]))


class RegisterPanel(QtWidgets.QWidget):
    def __init__(self, win):
        super().__init__()
        self.win = win
        rows = []
        hold = ["address", "mode", "on/off", "set V (10 mV)", "set I (mA)", "set P", "set R",
                "command", "value", "VIN gain reg", "V gain reg", "I gain (cfg 0x28)", "I-DAC cal"]
        inp = ["address", "mode / list step", "on / list time", "temperature", "aux", "VIN (mV)",
               "V (10 mV)", "I (mA)", "P", "R", "mAh", "energy", "seconds"]
        for a, nm in enumerate(hold):
            rows.append(("hold", a, nm, True))
        for a, nm in enumerate(inp):
            rows.append(("input", a, nm, False))
        x = {0x100: ("raw mode", True), 0x101: ("force V range", True), 0x102: ("force I range", True),
             0x103: ("force VIN range", True), 0x104: ("raw CC DAC", True), 0x105: ("raw CV DAC", True),
             0x106: ("lin-cal action", True), 0x107: ("lin-cal valid mask", True), 0x108: ("lin-cal version", False),
             0x110: ("raw V", False), 0x111: ("raw I", False), 0x112: ("raw VIN", False), 0x113: ("ranges", False),
             0x114: ("seq V/I", False), 0x115: ("seq VIN", False), 0x116: ("V", False), 0x117: ("I", False),
             0x118: ("VIN", False), 0x119: ("PA0 avg", False), 0x11A: ("PA0 min", False), 0x11B: ("PA0 max", False),
             0x11C: ("PA6 avg", False), 0x11D: ("PA6 min", False), 0x11E: ("PA6 max", False),
             0x180: ("stream enable", True), 0x181: ("fast divider", True), 0x182: ("fast mask", True),
             0x183: ("state period ms", True), 0x184: ("baud hi", False), 0x185: ("baud lo", False),
             0x186: ("ring overflows", False), 0x187: ("fast overflows", False), 0x188: ("ring fill", False),
             0x189: ("ring size", False), 0x18A: ("ADS errors", False), 0x18B: ("I discards", False),
             0x18C: ("now us hi", False), 0x18D: ("now us lo", False), 0x18E: ("ADS conv/s", False),
             0x18F: ("protocol", False), 0x190: ("ADS poll delay us", False),
             0x1C0: ("ramp up hi (mA/s)", True), 0x1C1: ("ramp up lo (mA/s)", True),
             0x1C2: ("ramp down hi (mA/s)", True), 0x1C3: ("ramp down lo (mA/s)", True)}
        chn = ["V_LO", "V_HI", "I_LO", "I_HI", "VIN_LO", "VIN_HI", "IDAC_LO", "IDAC_HI", "VDAC_LO", "VDAC_HI"]
        for k, nm in enumerate(chn):
            for j, part in enumerate(("gain hi", "gain lo", "offset hi", "offset lo")):
                x[0x120 + 4 * k + j] = ("%s %s" % (nm, part), True)
        for a in sorted(x):
            rows.append(("ext", a, x[a][0], x[a][1]))
        self.rows = rows

        lay = QtWidgets.QVBoxLayout(self)
        self.t = QtWidgets.QTableWidget(len(rows), 6)
        self.t.setHorizontalHeaderLabels(["Space", "Addr", "Name", "Value", "New", ""])
        self.t.verticalHeader().setVisible(False)
        self.t.setEditTriggers(QtWidgets.QAbstractItemView.AllEditTriggers)
        for r, (sp, a, nm, rw) in enumerate(rows):
            for c, txt in enumerate((sp, "0x%03X" % a if sp == "ext" else str(a), nm, "")):
                it = QtWidgets.QTableWidgetItem(txt)
                it.setFlags(QtCore.Qt.ItemIsEnabled)
                self.t.setItem(r, c, it)
            if rw:
                self.t.setItem(r, 4, QtWidgets.QTableWidgetItem(""))
                b = QtWidgets.QPushButton("Write")
                b.clicked.connect(lambda _=False, r=r: self.write_row(r))
                self.t.setCellWidget(r, 5, b)
            else:
                it = QtWidgets.QTableWidgetItem("")
                it.setFlags(QtCore.Qt.ItemIsEnabled)
                self.t.setItem(r, 4, it)
        self.t.resizeColumnsToContents()
        self.t.horizontalHeader().setStretchLastSection(True)
        lay.addWidget(self.t)

    def write_row(self, r):
        sp, a, nm, rw = self.rows[r]
        txt = self.t.item(r, 4).text().strip()
        if not txt:
            return
        try:
            v = int(txt, 0)
        except ValueError:
            self.win.log("bad value '%s'" % txt)
            return
        if v < 0:
            v &= 0xFFFF
        self.win.acq_submit(lambda L: L.write(a, v), lambda _: self.win.log("wrote %s %s = %d" % (sp, nm, v)))

    def update_values(self, snap):
        hold, inp = snap["hold"], snap["input"]
        ext = {}
        for base, key in ((0x100, "x100"), (0x110, "x110"), (0x120, "x120"), (0x180, "x180"), (0x1C0, "x1C0")):
            for k, v in enumerate(snap.get(key) or ()):
                ext[base + k] = v
        for r, (sp, a, nm, rw) in enumerate(self.rows):
            v = hold[a] if sp == "hold" else inp[a] if sp == "input" else ext.get(a)
            if v is not None:
                self.t.item(r, 3).setText("%d  (0x%04X)" % (v, v))


class CalPanel(QtWidgets.QWidget):
    CH = ["V_LO", "V_HI", "I_LO", "I_HI", "VIN_LO", "VIN_HI", "IDAC_LO", "IDAC_HI", "VDAC_LO", "VDAC_HI"]
    DESC = ["raw cnt -> 10 mV", "raw cnt -> 10 mV", "raw cnt -> mA", "raw cnt -> mA", "raw cnt -> mV",
            "raw cnt -> mV", "mA -> DAC code", "mA -> DAC code", "10 mV -> DAC code", "10 mV -> DAC code"]

    def __init__(self, win):
        super().__init__()
        self.win = win
        lay = QtWidgets.QVBoxLayout(self)
        info = QtWidgets.QLabel("Linear trim on top of the factory calibration: y = x * gain + offset. "
                                "Unticked channels use the factory tables. Apply/nudge changes RAM only; "
                                "'Save to flash' makes them permanent.")
        info.setWordWrap(True)
        lay.addWidget(info)
        self.t = QtWidgets.QTableWidget(10, 9)
        self.t.setHorizontalHeaderLabels(["Channel", "Use", "Gain", "Offset", "", "Gain -0.1%", "Gain +0.1%",
                                          "Off -1", "Off +1"])
        self.t.verticalHeader().setVisible(False)
        self.use, self.gain, self.off = [], [], []
        for r, nm in enumerate(self.CH):
            it = QtWidgets.QTableWidgetItem("%s  (%s)" % (nm, self.DESC[r]))
            it.setFlags(QtCore.Qt.ItemIsEnabled)
            self.t.setItem(r, 0, it)
            cb = QtWidgets.QCheckBox()
            cb.clicked.connect(lambda checked, r=r: self.set_use(r, checked))
            self.t.setCellWidget(r, 1, cb)
            g = dspin(-2000, 2000, 6, 0.0001)
            o = QtWidgets.QSpinBox()
            o.setRange(-2**31, 2**31 - 1)
            self.t.setCellWidget(r, 2, g)
            self.t.setCellWidget(r, 3, o)
            b = QtWidgets.QPushButton("Apply")
            b.clicked.connect(lambda _=False, r=r: self.apply(r))
            self.t.setCellWidget(r, 4, b)
            for c, (kind, d) in enumerate((("g", 0.999), ("g", 1.001), ("o", -1), ("o", 1)), start=5):
                bt = QtWidgets.QPushButton({5: "-0.1%", 6: "+0.1%", 7: "-1", 8: "+1"}[c])
                bt.clicked.connect(lambda _=False, r=r, kind=kind, d=d: self.nudge(r, kind, d))
                self.t.setCellWidget(r, c, bt)
            self.use.append(cb); self.gain.append(g); self.off.append(o)
        self.t.resizeColumnsToContents()
        lay.addWidget(self.t)
        row = QtWidgets.QHBoxLayout()
        for txt, fn in (("Read from unit", self.read), ("Save to flash", lambda: self.action(0x5A01, "save")),
                        ("Reload from flash", lambda: self.action(0x5A03, "reload")),
                        ("Erase (factory only)", self.erase)):
            b = QtWidgets.QPushButton(txt)
            b.clicked.connect(fn)
            row.addWidget(b)
        lay.addLayout(row)
        self.l_live = QtWidgets.QLabel("")
        lay.addWidget(self.l_live)

        # PA6 (OPA2188 current-sense output) -> amps, GUI-side only
        g = QtWidgets.QGroupBox("PA6 (OPA2188 current-sense output) to amps  -  GUI display only")
        f = QtWidgets.QGridLayout(g)
        note = QtWidgets.QLabel("A = k * counts + b, per current range, from the hardware (shunt, "
                                "OPA2188 gain, ADC reference). No mapping is set by default: k = 0 leaves "
                                "the 'PA6 ... as current' channels blank for that range.")
        note.setWordWrap(True)
        f.addWidget(note, 0, 0, 1, 4)
        self.pk, self.pb = {}, {}
        for r, nm in ((0, "Low range"), (1, "High range")):
            k = dspin(0, 1, 7, 0.00001, "A/cnt")
            b = dspin(-10, 10, 4, 0.001, "A")
            k.valueChanged.connect(self.pa6_edited)
            b.valueChanged.connect(self.pa6_edited)
            self.pk[r], self.pb[r] = k, b
            f.addWidget(QtWidgets.QLabel(nm), r + 1, 0); f.addWidget(k, r + 1, 1); f.addWidget(b, r + 1, 2)
        bd = QtWidgets.QPushButton("Clear mapping")
        bd.clicked.connect(lambda: self.pa6_set(dict(PA6_DEFAULT)))
        f.addWidget(bd, 3, 2)
        self.l_pa6 = QtWidgets.QLabel("")
        self.l_pa6.setWordWrap(True)
        f.addWidget(self.l_pa6, 4, 0, 1, 4)
        lay.addWidget(g)
        lay.addStretch(1)
        self._loaded = False
        self._pa6_busy = False
        self.pa6 = self.pa6_load()
        self.pa6_set(self.pa6)

    # ---- PA6 scaling ------------------------------------------------------
    def pa6_load(self):
        st = QtCore.QSettings("sousim", "gui")
        out = dict(PA6_DEFAULT)
        for r in (0, 1):
            v = st.value("pa6/%d" % r)
            if v:
                try:
                    k, b = (float(x) for x in str(v).split(","))
                    out[r] = (k, b)
                except ValueError:
                    pass
        return out

    def pa6_set(self, cal):
        self.pa6 = dict(cal)
        self._pa6_busy = True
        for r in (0, 1):
            kb = cal.get(r)
            self.pk[r].setValue(kb[0] if kb else 0.0)
            self.pb[r].setValue(kb[1] if kb else 0.0)
        self._pa6_busy = False
        self.pa6_apply()

    def pa6_edited(self):
        if self._pa6_busy:
            return
        for r in (0, 1):
            k = self.pk[r].value()
            self.pa6[r] = (k, self.pb[r].value()) if k > 0 else None
        self.pa6_apply()

    def pa6_apply(self):
        if self.win.acq:
            self.win.acq.dec.pa6 = dict(self.pa6)
        st = QtCore.QSettings("sousim", "gui")
        for r in (0, 1):
            kb = self.pa6.get(r)
            st.setValue("pa6/%d" % r, "%.9g,%.9g" % kb if kb else "")
        txt = []
        for r, nm in ((0, "low"), (1, "high")):
            kb = self.pa6.get(r)
            txt.append("%s: %s" % (nm, "%.3f mA/count %+.1f mA" % (kb[0] * 1e3, kb[1] * 1e3) if kb else "not set"))
        self.l_pa6.setText("Active: " + ";  ".join(txt) + "  (applies to new samples)")

    def load(self, x107, x120):
        mask = x107
        for r in range(10):
            g = struct.unpack(">i", struct.pack(">HH", x120[4 * r], x120[4 * r + 1]))[0]
            o = struct.unpack(">i", struct.pack(">HH", x120[4 * r + 2], x120[4 * r + 3]))[0]
            self.use[r].setChecked(bool(mask & (1 << r)))
            if not self.gain[r].hasFocus():
                self.gain[r].setValue(g / 1e6)
            if not self.off[r].hasFocus():
                self.off[r].setValue(o)

    def maybe_load(self, snap):
        if not self._loaded:
            self._loaded = True
            self.load(snap["x100"][7], snap["x120"])
        else:                                   # keep checkboxes in sync
            m = snap["x100"][7]
            for r in range(10):
                self.use[r].setChecked(bool(m & (1 << r)))

    def read(self):
        def go(L):
            return L.read(0x107, 1)[0], L.read(0x120, 40)
        self.win.acq_submit(go, lambda r: self.load(*r))

    def _write(self, r, g, o):
        gi = int(round(g * 1e6))
        regs = list(struct.unpack(">HHHH", struct.pack(">ii", gi, int(o))))
        self.win.acq_submit(lambda L: L.write_many(0x120 + 4 * r, regs),
                            lambda _: self.win.log("%s: gain %.6f offset %d (RAM)" % (self.CH[r], g, o)))

    def apply(self, r):
        self._write(r, self.gain[r].value(), self.off[r].value())

    def nudge(self, r, kind, d):
        g, o = self.gain[r].value(), self.off[r].value()
        if kind == "g":
            g *= d
            self.gain[r].setValue(g)
        else:
            o += d
            self.off[r].setValue(o)
        self._write(r, g, o)

    def set_use(self, r, on):
        def go(L):
            m = L.read(0x107, 1)[0]
            m = (m | (1 << r)) if on else (m & ~(1 << r))
            L.write(0x107, m)
            return m
        self.win.acq_submit(go, lambda m: self.win.log("valid mask 0x%03X (RAM)" % m))

    def action(self, code, name):
        def go(L):
            L.write(0x106, code)
            time.sleep(0.4)
            return L.read(0x106, 1)[0]
        self.win.acq_submit(go, lambda r: self.win.log("lin-cal %s: %s" % (name, "ok" if r == 0 else "FAILED")))

    def erase(self):
        if QtWidgets.QMessageBox.question(self, "Erase", "Erase all linear calibration (flash and RAM)?") \
                == QtWidgets.QMessageBox.Yes:
            self.action(0x5A02, "erase")
            QtCore.QTimer.singleShot(800, self.read)


class LinkPanel(QtWidgets.QWidget):
    def __init__(self, win):
        super().__init__()
        self.win = win
        lay = QtWidgets.QVBoxLayout(self)
        g = QtWidgets.QGroupBox("Fast channels (sampled in the 10 kHz control interrupt)")
        f = QtWidgets.QGridLayout(g)
        self.cb = []
        for k, nm in enumerate(("PA6 (OPA2188 out, I sense)", "PA0", "CC DAC code", "CV DAC code")):
            c = QtWidgets.QCheckBox(nm)
            c.setChecked(k == 0)
            self.cb.append(c)
            f.addWidget(c, k // 2, k % 2)
        self.c_rate = QtWidgets.QComboBox()
        for hz in (0, 100, 500, 1000, 2000, 5000, 10000):
            self.c_rate.addItem("off" if hz == 0 else "%d Hz" % hz, hz)
        self.c_rate.setCurrentIndex(3)
        f.addWidget(QtWidgets.QLabel("Rate"), 2, 0); f.addWidget(self.c_rate, 2, 1)
        b = QtWidgets.QPushButton("Apply")
        b.clicked.connect(self.apply_fast)
        f.addWidget(b, 3, 1)
        lay.addWidget(g)

        g = QtWidgets.QGroupBox("Stream")
        f = QtWidgets.QGridLayout(g)
        self.s_state = QtWidgets.QSpinBox()
        self.s_state.setRange(0, 1000)
        self.s_state.setValue(20)
        self.s_state.setSuffix(" ms")
        b = QtWidgets.QPushButton("Apply")
        b.clicked.connect(lambda: win.acq_submit(lambda L: L.write(0x183, self.s_state.value())))
        f.addWidget(QtWidgets.QLabel("State record period"), 0, 0); f.addWidget(self.s_state, 0, 1); f.addWidget(b, 0, 2)
        self.s_max = QtWidgets.QSpinBox()
        self.s_max.setRange(64, 1032)
        self.s_max.setValue(1024)
        self.s_max.valueChanged.connect(lambda v: setattr(win.acq, "maxlen", v) if win.acq else None)
        f.addWidget(QtWidgets.QLabel("Max bytes per poll"), 1, 0); f.addWidget(self.s_max, 1, 1)
        lay.addWidget(g)

        g = QtWidgets.QGroupBox("Baud rate")
        f = QtWidgets.QGridLayout(g)
        self.c_baud = QtWidgets.QComboBox()
        for b_ in BAUDS:
            self.c_baud.addItem(str(b_), b_)
        self.c_baud.setCurrentText("921600")
        b1 = QtWidgets.QPushButton("Switch (temporary)")
        b1.clicked.connect(lambda: win.switch_baud(self.c_baud.currentData()))
        b2 = QtWidgets.QPushButton("Save current as default")
        b2.clicked.connect(win.save_baud)
        f.addWidget(self.c_baud, 0, 0); f.addWidget(b1, 0, 1); f.addWidget(b2, 1, 1)
        lab = QtWidgets.QLabel("A switch reverts by itself after 3 s if the new rate does not work.")
        lab.setWordWrap(True)
        f.addWidget(lab, 2, 0, 1, 2)
        lay.addWidget(g)

        self.stats = QtWidgets.QLabel("")
        self.stats.setTextInteractionFlags(QtCore.Qt.TextSelectableByMouse)
        self.stats.setStyleSheet("font-family:Consolas,monospace")
        lay.addWidget(self.stats)
        lay.addStretch(1)

    def apply_fast(self):
        mask = sum(1 << k for k, c in enumerate(self.cb) if c.isChecked())
        hz = self.c_rate.currentData()
        div = 0 if hz == 0 else max(1, int(round(10000 / hz)))
        self.win.acq_submit(lambda L: L.write_many(0x181, [div, mask]),
                            lambda _: self.win.log("fast channels: mask 0x%X, divider %d" % (mask, div)))


class ChannelTree(QtWidgets.QTreeWidget):
    def __init__(self, win):
        super().__init__()
        self.win = win
        self.setColumnCount(4)
        self.setHeaderLabels(["Channel", "Value", "Unit", "Rate"])
        self.items = {}
        groups = {}
        for g in GROUP_ORDER:
            gi = QtWidgets.QTreeWidgetItem([g])
            f = gi.font(0)
            f.setBold(True)
            gi.setFont(0, f)
            self.addTopLevelItem(gi)
            gi.setFirstColumnSpanned(True)
            groups[g] = gi
        for k, (lab, unit, grp, _) in CHANNELS.items():
            it = QtWidgets.QTreeWidgetItem([lab, "", unit, ""])
            it.setFlags(it.flags() | QtCore.Qt.ItemIsUserCheckable)
            it.setCheckState(0, QtCore.Qt.Checked if k in ("V", "I") else QtCore.Qt.Unchecked)
            it.setData(0, QtCore.Qt.UserRole, k)
            groups[grp].addChild(it)
            self.items[k] = it
        self.expandAll()
        self.setColumnWidth(0, 230)
        self.setColumnWidth(1, 90)
        self.setColumnWidth(2, 45)
        self.itemChanged.connect(lambda it, col: win.selection_changed() if col == 0 else None)
        self._prev = {}
        self._prev_t = time.monotonic()

    def selected(self):
        return [k for k, it in self.items.items() if it.checkState(0) == QtCore.Qt.Checked]

    def refresh(self, store: Store):
        now = time.monotonic()
        dt = max(now - self._prev_t, 1e-3)
        self.blockSignals(True)
        for k, it in self.items.items():
            s = store.s[k]
            v = s.last
            if not math.isnan(v):
                it.setText(1, ("%.6g" % v))
            tot = s.total
            rate = (tot - self._prev.get(k, tot)) / dt
            self._prev[k] = tot
            it.setText(3, "%.0f Hz" % rate if rate else "")
        self.blockSignals(False)
        self._prev_t = now


# =============================================================================
# Main window
# =============================================================================
class Main(QtWidgets.QMainWindow):
    def __init__(self, args):
        super().__init__()
        self.setWindowTitle("Sousim load - live")
        self.resize(1600, 950)
        self.store = Store()
        self.ui_q = queue.Queue()
        self.acq = None
        self.link = None
        self.last_regs = None
        self.curves = {}
        self.plots = []
        self.paused = False
        self.window_s = 10.0
        self._pdiv = 1
        self.saved_baud = 115200

        # toolbar
        tb = self.addToolBar("main")
        tb.setMovable(False)
        self.c_port = QtWidgets.QComboBox()
        self.c_port.setMinimumWidth(260)
        self.refresh_ports()
        b = QtWidgets.QToolButton()
        b.setText("refresh")
        b.clicked.connect(self.refresh_ports)
        self.c_baud = QtWidgets.QComboBox()
        for b_ in BAUDS:
            self.c_baud.addItem(str(b_), b_)
        self.c_baud.setCurrentText(str(args.baud))
        self.s_addr = QtWidgets.QSpinBox()
        self.s_addr.setRange(1, 247)
        self.s_addr.setValue(args.addr)
        self.b_conn = QtWidgets.QPushButton("Connect")
        self.b_conn.clicked.connect(self.toggle_connect)
        self.cb_fast = QtWidgets.QCheckBox("Auto 1.5M")
        self.cb_fast.setChecked(not args.no_auto_baud)
        self.cb_fast.setToolTip("After connecting, switch the link to 1.5 Mbaud for this session\n"
                                "(falls back to 921600; restored to the saved rate on disconnect)")
        for w in (QtWidgets.QLabel(" Port "), self.c_port, b, QtWidgets.QLabel(" Baud "), self.c_baud,
                  QtWidgets.QLabel(" Addr "), self.s_addr, self.cb_fast, self.b_conn):
            tb.addWidget(w)
        tb.addSeparator()
        self.c_win = QtWidgets.QComboBox()
        for s in (0.5, 1, 2, 5, 10, 30, 60, 300, 1800):
            self.c_win.addItem("%g s" % s, float(s))
        self.c_win.setCurrentText("10 s")
        self.c_win.currentIndexChanged.connect(lambda: setattr(self, "window_s", self.c_win.currentData()))
        self.b_pause = QtWidgets.QPushButton("Pause")
        self.b_pause.setCheckable(True)
        self.b_pause.toggled.connect(self.set_paused)
        b_auto = QtWidgets.QPushButton("Autoscale Y")
        b_auto.clicked.connect(lambda: [p.enableAutoRange(axis="y") for p in self.plots])
        b_clear = QtWidgets.QPushButton("Clear")
        b_clear.clicked.connect(self.store.clear)
        self.b_rec = QtWidgets.QPushButton("Record CSV")
        self.b_rec.setCheckable(True)
        self.b_rec.toggled.connect(self.toggle_record)
        for w in (QtWidgets.QLabel(" Window "), self.c_win, self.b_pause, b_auto, b_clear, self.b_rec):
            tb.addWidget(w)

        # central: channel tree | plots
        pg.setConfigOptions(antialias=False, foreground="#ddd", background="#111")
        self.glw = pg.GraphicsLayoutWidget()
        self.tree = ChannelTree(self)
        split = QtWidgets.QSplitter()
        split.addWidget(self.tree)
        split.addWidget(self.glw)
        split.setSizes([380, 1220])
        self.setCentralWidget(split)

        # right dock: tabs
        self.ctrl = ControlPanel(self)
        self.regs = RegisterPanel(self)
        self.cal = CalPanel(self)
        self.linkp = LinkPanel(self)
        self.cfg = QtWidgets.QWidget()
        cl = QtWidgets.QVBoxLayout(self.cfg)
        bcfg = QtWidgets.QPushButton("Read config page (0x400-0x55D, read only)")
        bcfg.clicked.connect(self.read_cfg)
        self.t_cfg = QtWidgets.QTableWidget(0, 3)
        self.t_cfg.setHorizontalHeaderLabels(["Byte offset", "Word", "Value"])
        cl.addWidget(bcfg)
        cl.addWidget(self.t_cfg)
        tabs = QtWidgets.QTabWidget()
        for w, nm in ((self.ctrl, "Control"), (self.cal, "Calibration"), (self.regs, "Registers"),
                      (self.linkp, "Link"), (self.cfg, "Config page")):
            sa = QtWidgets.QScrollArea()
            sa.setWidget(w)
            sa.setWidgetResizable(True)
            tabs.addTab(sa, nm)
        dock = QtWidgets.QDockWidget("Panel")
        dock.setWidget(tabs)
        dock.setFeatures(QtWidgets.QDockWidget.DockWidgetMovable | QtWidgets.QDockWidget.DockWidgetFloatable)
        self.addDockWidget(QtCore.Qt.RightDockWidgetArea, dock)
        dock.setMinimumWidth(520)

        self.logw = QtWidgets.QPlainTextEdit()
        self.logw.setReadOnly(True)
        self.logw.setMaximumBlockCount(2000)
        ld = QtWidgets.QDockWidget("Log / events")
        ld.setWidget(self.logw)
        self.addDockWidget(QtCore.Qt.BottomDockWidgetArea, ld)

        self.status = QtWidgets.QLabel("disconnected")
        self.statusBar().addPermanentWidget(self.status, 1)

        QtGui.QShortcut(QtGui.QKeySequence("Esc"), self, activated=lambda: self.write_hold(2, 0))

        self.selection_changed()
        self.t_plot = QtCore.QTimer(self, interval=33, timeout=self.update_plots)
        self.t_plot.start()
        self.t_ui = QtCore.QTimer(self, interval=20, timeout=self.drain_ui)
        self.t_ui.start()
        self.t_tree = QtCore.QTimer(self, interval=250, timeout=self.slow_ui)
        self.t_tree.start()
        self._st_prev = None

        if args.port:
            i = self.c_port.findData(args.port)
            if i < 0:
                self.c_port.addItem(args.port, args.port)
                i = self.c_port.count() - 1
            self.c_port.setCurrentIndex(i)
            QtCore.QTimer.singleShot(100, self.toggle_connect)

    # ---------------------------------------------------------------- helpers
    def log(self, msg):
        self.logw.appendPlainText(time.strftime("%H:%M:%S ") + msg)

    def dec_pdiv(self):
        return self.acq.dec.pdiv if self.acq else 1

    def refresh_ports(self):
        self.c_port.clear()
        for p in sorted(serial.tools.list_ports.comports(), key=lambda p: p.device):
            self.c_port.addItem("%s  %s" % (p.device, p.description), p.device)

    def acq_submit(self, fn, cb=None):
        if not self.acq:
            self.log("not connected")
            return
        self.acq.submit(fn, cb)

    def write_hold(self, reg, val):
        self.acq_submit(lambda L: L.write(reg, int(val) & 0xFFFF))

    # ------------------------------------------------------------- connection
    def toggle_connect(self):
        if self.acq:
            self.disconnect_link()
            return
        port = self.c_port.currentData()
        if not port:
            return
        self.link = None
        try:
            self.link = Link(port, self.c_baud.currentData(), self.s_addr.value())
            self.link.read(0, 1)
            self.saved_baud = BAUD_TABLE[self.link.read(0x400 + CFG_BAUD_WORD, 1)[0] % len(BAUD_TABLE)]
        except Exception as e:
            self.log("connect failed: %s" % e)
            if self.link:
                self.link.close()
            self.link = None
            return
        self.store.clear()
        self.acq = Acq(self.link, self.store, self.ui_q)
        self.acq.dec.pa6 = dict(self.cal.pa6)
        self.acq.maxlen = self.linkp.s_max.value()
        self.acq.start()
        self.b_conn.setText("Disconnect")
        self.log("connected %s @ %d (saved rate %d)" % (port, self.link.baud, self.saved_baud))
        if self.cb_fast.isChecked() and self.link.baud < 1500000:
            self.switch_baud(1500000, fallback=921600)
        self.linkp.apply_fast()
        self.acq_submit(lambda L: L.write(0x183, self.linkp.s_state.value()))

    def disconnect_link(self):
        if self.acq:
            self.acq.stop()
            self.acq.join(timeout=2)
            self.acq.stop_recording()
        if self.link:
            try:                                    # leave the load at its saved rate
                if self.link.baud != self.saved_baud:
                    b = self.saved_baud
                    self.link.write_many(0x184, [b >> 16, b & 0xFFFF])
            except Exception:
                pass
            self.link.close()
        self.acq = self.link = None
        self.b_conn.setText("Connect")
        self.b_rec.setChecked(False)
        self.log("disconnected")

    def switch_baud(self, new, fallback=None):
        def attempt(L, rate):
            old = L.baud
            L.write_many(0x184, [rate >> 16, rate & 0xFFFF])
            time.sleep(0.05)
            L.set_baud(rate)
            for _ in range(4):
                try:
                    L.read(0x18F, 1)
                    return True
                except LinkError:
                    time.sleep(0.2)
            L.set_baud(old)                         # firmware reverts to its saved rate
            time.sleep(3.3)
            L.set_baud(self.saved_baud)
            return False

        def go(L):
            for rate in [new] + ([fallback] if fallback else []):
                if attempt(L, rate):
                    return (True, rate)
            return (False, L.baud)

        def done(r):
            ok, rate = r
            self.log(("link now at %d baud" % rate) if ok else ("baud switch failed - at %d" % rate))
        self.acq_submit(go, done)

    def save_baud(self):
        if not self.link:
            return
        b = self.link.baud
        if b not in BAUD_TABLE:
            self.log("%d is not in the firmware baud table" % b)
            return
        idx = BAUD_TABLE.index(b)

        def go(L):
            L.write(7, 0xE8)
            time.sleep(0.2)
            L.write(8, idx)
            time.sleep(0.3)
            return idx
        self.acq_submit(go, lambda i: self.log("default baud saved: %d (index %d)" % (b, i)))

    def toggle_record(self, on):
        if not self.acq:
            self.b_rec.setChecked(False)
            return
        if on:
            path, _ = QtWidgets.QFileDialog.getSaveFileName(self, "Record to", time.strftime("sousim_%Y%m%d_%H%M%S.csv"),
                                                            "CSV (*.csv)")
            if not path:
                self.b_rec.setChecked(False)
                return
            self.acq.start_recording(path)
            self.log("recording to %s" % path)
        else:
            self.acq.stop_recording()
            self.log("recording stopped")

    def read_cfg(self):
        def go(L):
            out = []
            for a in range(0x400, 0x400 + 350, 125):
                out += L.read(a, min(125, 0x400 + 350 - a))
            return out

        def show(words):
            names = {0x00: "magic", 0x04: "fw marker", 0x0A: "Vmax", 0x0C: "Imax", 0x0E: "Pmax", 0x10: "saved V",
                     0x12: "saved I", 0x18: "unused (was soft start)", 0x1A: "legacy slew up", 0x1C: "legacy slew down", 0x20: "mode",
                     0x22: "preset type", 0x24: "Modbus addr", 0x26: "baud index", 0x28: "I gain", 0x2A: "Vref",
                     0x2C: "V zero", 0x2E: "I zero", 0x30: "V gain lo", 0x32: "V gain hi", 0x34: "VIN gain lo",
                     0x36: "VIN gain hi", 0x40: "V-DAC scale lo", 0x42: "V-DAC scale hi", 0x4E: "V from VIN",
                     0x12A: "I-DAC default", 0x1EA: "power res", 0x1EC: "range fallback", 0x1EE: "cut-off V",
                     0x1F0: "I trim limit"}
            self.t_cfg.setRowCount(len(words))
            for k, w in enumerate(words):
                off = 2 * k
                nm = names.get(off, "")
                if 0x50 <= off <= 0x6C: nm = "I meas gain lo [%d]" % ((off - 0x50) // 2)
                elif 0x6E <= off <= 0x128: nm = "I meas gain hi @0x%X" % off
                elif 0x12C <= off <= 0x1C6: nm = "I-DAC table @0x%X" % off
                elif 0x1F2 <= off <= 0x204: nm = "preset value %d" % ((off - 0x1F2) // 2 + 1)
                elif 0x256 <= off <= 0x268: nm = "preset aux/duration %d" % ((off - 0x256) // 2 + 1)
                for c, txt in enumerate(("0x%03X" % off, nm, "%d (0x%04X)" % (w, w))):
                    self.t_cfg.setItem(k, c, QtWidgets.QTableWidgetItem(txt))
            self.t_cfg.resizeColumnsToContents()
        self.acq_submit(go, show)

    # ------------------------------------------------------------------ plots
    def set_paused(self, on):
        self.paused = on
        for p in self.plots:
            p.setMouseEnabled(x=on, y=True)

    def selection_changed(self):
        sel = self.tree.selected()
        self.glw.clear()
        self.plots, self.curves = [], {}
        first = None
        groups = [g for g in GROUP_ORDER if any(CHANNELS[k][2] == g for k in sel)]
        for row, g in enumerate(groups):
            p = self.glw.addPlot(row=row, col=0)
            p.showGrid(x=True, y=True, alpha=0.25)
            p.setLabel("left", g)
            p.addLegend(offset=(5, 5))
            p.setClipToView(True)
            p.setDownsampling(auto=True, mode="peak")
            p.setMouseEnabled(x=self.paused, y=True)
            if first is None:
                first = p
            else:
                p.setXLink(first)
            if row == len(groups) - 1:
                p.setLabel("bottom", "time", "s")
            self.plots.append(p)
            for k in sel:
                if CHANNELS[k][2] != g:
                    continue
                color = pg.intColor(list(CHANNELS).index(k), hues=17, values=1, maxValue=255, minValue=150)
                c = p.plot(name=CHANNELS[k][0], pen=pg.mkPen(color, width=1),
                           connect="finite", skipFiniteCheck=False)
                self.curves[k] = c

    def update_plots(self):
        if self.paused or not self.curves:
            return
        tnow = self.store.tmax
        t0 = tnow - self.window_s
        for k, c in self.curves.items():
            t, v, n = self.store.snap(k)
            if n == 0:
                c.setData([], [])
                continue
            i0 = int(np.searchsorted(t[:n], t0 - 0.05 * self.window_s))
            c.setData(t[i0:n], v[i0:n])
        for p in self.plots:
            p.setXRange(t0, tnow, padding=0)

    # --------------------------------------------------------------- ui queue
    def drain_ui(self):
        for _ in range(200):
            try:
                m = self.ui_q.get_nowait()
            except queue.Empty:
                break
            kind = m[0]
            if kind == "cb":
                _, cb, r, err = m
                if err is not None:
                    self.log("error: %s" % err)
                elif cb:
                    try:
                        cb(r)
                    except Exception as e:
                        self.log("callback error: %s" % e)
            elif kind == "regs":
                self.last_regs = m[1]
            elif kind == "event":
                t, code, data = m[1]
                self.log("[t=%.3f s] EVENT %s (%d)" % (t, EVENTS.get(code, "code %d" % code), data))
            elif kind == "log":
                self.log(m[1])
            elif kind == "lost":
                self.log("connection lost: %s" % m[1])
                self.disconnect_link()

    def slow_ui(self):
        self.tree.refresh(self.store)
        snap = self.last_regs
        if snap:
            self.regs.update_values(snap)
            self.ctrl.update_state(snap["hold"], snap["input"])
            self.ctrl.update_ramp(snap.get("x1C0"))
            self.cal.maybe_load(snap)
            x = snap["x180"]
            x1 = snap["x110"]
            self.cal.l_live.setText("live raw: V %d  I %d  VIN %d   |   ranges 0x%X" % (x1[0], x1[1], x1[2], x1[3]))
        if self.acq:
            st = dict(self.acq.stats)
            now = time.monotonic()
            if self._st_prev:
                dt = now - self._st_prev[0]
                bps = (st["bytes"] - self._st_prev[1]["bytes"]) / dt
                pps = (st["polls"] - self._st_prev[1]["polls"]) / dt
            else:
                bps = pps = 0
            self._st_prev = (now, st)
            txt = "%s @ %d  |  %.1f kB/s  %.0f polls/s  |  errors %d  seq gaps %d  overflow polls %d/%d" % (
                self.link.port, self.link.baud, bps / 1000, pps, st["errors"], st["seq_gaps"], st["ovf"], st["fovf"])
            if snap:
                x = snap["x180"]
                txt += "  |  ring %d/%d  ADS %d conv/s  ADS err %d  I discard %d" % (x[8], x[9], x[14], x[10], x[11])
                self.linkp.stats.setText(
                    "link            %s @ %d baud\n"
                    "throughput      %.1f kB/s, %.0f polls/s\n"
                    "errors          %d  (seq gaps %d)\n"
                    "ring            %d / %d bytes, overflows %d (fast %d)\n"
                    "ADS1115         %d conversions/s, errors %d, I discards %d, poll delay %d us\n"
                    "stream          enable %d, fast divider %d, mask 0x%X, state %d ms" % (
                        self.link.port, self.link.baud, bps / 1000, pps, st["errors"], st["seq_gaps"],
                        x[8], x[9], x[6], x[7], x[14], x[10], x[11], x[16], x[0], x[1], x[2], x[3]))
            self.status.setText(txt)
        else:
            self.status.setText("disconnected")

    def closeEvent(self, ev):
        self.disconnect_link()
        super().closeEvent(ev)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("-p", "--port")
    ap.add_argument("-b", "--baud", type=int, default=115200)
    ap.add_argument("-a", "--addr", type=int, default=1)
    ap.add_argument("--plot", default="V,I", help="channels plotted at start (comma list of keys)")
    ap.add_argument("--no-auto-baud", action="store_true", help="stay at --baud instead of switching to 1.5M")
    ap.add_argument("--screenshot", help="save a screenshot after --run seconds and exit (testing)")
    ap.add_argument("--run", type=float, default=0, help="seconds to run before exiting (testing)")
    args = ap.parse_args()

    app = QtWidgets.QApplication(sys.argv)
    app.setStyle("Fusion")
    pal = QtGui.QPalette()
    for role, col in ((QtGui.QPalette.Window, "#202124"), (QtGui.QPalette.WindowText, "#e8e8e8"),
                      (QtGui.QPalette.Base, "#16171a"), (QtGui.QPalette.AlternateBase, "#25262a"),
                      (QtGui.QPalette.Text, "#e8e8e8"), (QtGui.QPalette.Button, "#2d2e32"),
                      (QtGui.QPalette.ButtonText, "#e8e8e8"), (QtGui.QPalette.Highlight, "#3d6fb4")):
        pal.setColor(role, QtGui.QColor(col))
    app.setPalette(pal)
    w = Main(args)
    sel = [k.strip() for k in args.plot.split(",") if k.strip() in CHANNELS]
    if sel:
        w.tree.blockSignals(True)
        for k, it in w.tree.items.items():
            it.setCheckState(0, QtCore.Qt.Checked if k in sel else QtCore.Qt.Unchecked)
        w.tree.blockSignals(False)
        w.selection_changed()
    w.show()
    if args.run:
        def finish():
            if args.screenshot:
                w.grab().save(args.screenshot)
            w.close()
            app.quit()
        QtCore.QTimer.singleShot(int(args.run * 1000), finish)
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
