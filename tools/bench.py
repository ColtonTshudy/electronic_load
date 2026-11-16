#!/usr/bin/env python3
"""Minimal SCPI helpers for the bench: OWON SPE6103 supply and OWON XDM2041 DMM."""
import time
import serial


class Scpi:
    def __init__(self, port, baud=115200, timeout=1.0):
        self.s = serial.Serial(port, baud, timeout=timeout)

    def write(self, cmd):
        self.s.write((cmd + "\n").encode())
        time.sleep(0.05)

    def query(self, cmd):
        self.s.reset_input_buffer()
        self.s.write((cmd + "\n").encode())
        return self.s.readline().decode(errors="replace").strip()

    def close(self):
        self.s.close()


class Supply(Scpi):
    """OWON SPE6103 (60 V / 10 A)."""
    def state(self):
        return dict(out=self.query("OUTP?"), vset=self.query("VOLT?"), iset=self.query("CURR?"),
                    v=self.query("MEAS:VOLT?"), i=self.query("MEAS:CURR?"))

    def set(self, volts=None, amps=None):
        if volts is not None:
            self.write("VOLT %.3f" % volts)
        if amps is not None:
            self.write("CURR %.3f" % amps)

    def output(self, on):
        self.write("OUTP %s" % ("ON" if on else "OFF"))


class Dmm(Scpi):
    """OWON XDM2041."""
    def function(self):
        return self.query("FUNC?")

    def read(self):
        return float(self.query("MEAS?"))
