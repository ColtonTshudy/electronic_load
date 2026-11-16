#!/usr/bin/env python3
"""List serial ports and their attributes (requires pyserial)."""
from serial.tools import list_ports

ports = sorted(list_ports.comports(), key=lambda p: p.device)
if not ports:
    print("no serial ports found")
for p in ports:
    print(p.device)
    for attr in ("name", "description", "hwid", "vid", "pid", "serial_number",
                 "manufacturer", "product", "interface", "location"):
        val = getattr(p, attr, None)
        if attr in ("vid", "pid") and val is not None:
            val = "0x%04X" % val
        if val not in (None, "", "n/a"):
            print("  %-14s %s" % (attr, val))
