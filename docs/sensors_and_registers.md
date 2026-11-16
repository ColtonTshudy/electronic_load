# Sousim 500 W load: sensors and Modbus registers

Reference for the clean-sheet firmware in `sousim_load/`. It covers what the load can measure,
how each measurement is made, and what every Modbus register does.

The source files are the final authority:

| Topic | File |
|---|---|
| Measurement | `Core/Src/app_measure.c` |
| Modbus | `Core/Src/app_modbus.c` |
| Streaming | `Core/Src/app_stream.c` |
| Linear calibration | `Core/Src/app_lincal.c` |
| Config page layout | `Core/Inc/app.h` |
| Current ramp, control | `Core/Src/app_control.c` |

Items marked **unknown** have not been traced on the board.

---

## 1. Units used throughout

| Quantity | Raw unit | Example |
|---|---|---|
| Voltage (V, set V) | 10 mV | 20000 = 200.00 V |
| Current (I, set I) | 1 mA | 60000 = 60.000 A |
| VIN | 1 mV | 9100 = 9.100 V |
| Power (P, set P) | 10 mW × pdiv | 50000 = 500.00 W (pdiv = 1) |
| Resistance (R, set R) | 0.1 Ω | 100 = 10.0 Ω |
| Charge (accumulator) | 1 µAh | input reg 10 = mAh |
| Energy (accumulator) | 10 µWh | input reg 11 = 10 mWh |
| NTC temperature | 1 °C | 0 to 99 |
| MCU temperature | 0.1 °C | 275 = 27.5 °C; negative as 9000 + abs(T) |

- **pdiv:** 1 normally, or 10 when config `CFG_POWER_RES` ≠ 0. In that case power is in 100 mW units.
- **Resistance:** R = V × 100 / I in the raw units above.

This unit is rated 200 V / 60 A / 500 W (config Vmax, Imax and Pmax).

---

## 2. Sensors

### 2.1 ADS1115: main measurements

The ADS1115 is a 16-bit delta-sigma ADC.

- **Bus:** I2C address 0x48, bit-banged on PC14 (SDA) and PC15 (SCL).
- **Mode:** single-shot at 860 samples/s (the chip's maximum data rate).
- **Pipelining:** each conversion starts as soon as the previous one completes, and its result is read while the next one converts.
- **Order:** V and I alternate; VIN is inserted every 100 ms.
- **Throughput:** about 500 to 560 conversions/s in practice, so V and I are each about 245 samples/s.
- **Clamping:** negative results read as 0.

| Signal | ADS input | PGA | Range switch | Notes |
|---|---|---|---|---|
| **Load current** | AIN0 − AIN3 | ±2.048 V | PB2: low ≤ 5.5 A, high above | Same signal as the OPA2188 output on PA6 (PA6 ≈ 0.102 × ADS raw − 3). PB2 is set by the measured current with hysteresis, and is also forced high by a setpoint > 5.5 A. A sample is discarded if PB2 changes during its conversion. |
| **Load voltage** | AIN1 − AIN3 | ±2.048 V | PA1: analog/off = low (divider ≈ 21:1, up to ~38 V); output low = high (≈ 122:1) | Switches to high at ≥ 38.51 V and back to low below 36.00 V. A range change is applied while the current conversion runs, so it has a full conversion time to settle. |
| **VIN** | AIN2 − AIN3 | ±1.024 V (low) / ±2.048 V (high) | PGA only | Goes to the high PGA above 32.000 V and back below 29.500 V. **Unknown input:** it reads about 9.1 V on the bench with nothing connected to the load terminals, so it is probably the board's supply input. |

**Converting raw counts:** each channel uses the linear calibration (section 4.5) if its bit is set, otherwise the factory tables:

| Channel | Factory conversion |
|---|---|
| V | raw × CFG `V_GAIN_LO/HI` / 10000 (10 mV). The high range also subtracts 3 + 3 near the bottom. |
| I | gain_table(I) × (CFG `I_GAIN` × raw / 1000) / 10000 (mA). The gain table is a piecewise table per range (config 0x50 to 0x128). |
| VIN | raw × CFG `VIN_GAIN_LO/HI` / 10000 (mV) |

The firmware then applies three corrections:
- Readings at or below CFG `V_ZERO` / `I_ZERO` are forced to 0.
- On this 60 A model, V gets lead-resistance compensation of + I / 4000.
- If CFG `V_FROM_VIN` = 1, V is replaced by VIN / 10.

### 2.2 STM32 internal ADC

The internal ADC is 12-bit, clocked at 6 MHz, and referenced to VDDA.

| Signal | Pin / ch | Sample time | Rate | What it is |
|---|---|---|---|---|
| **I sense** | PA6 / 6 | 7.5 cycles | 10 kHz fast channel (optional) + 64-sample burst every 100 ms | Output of a TI **OPA2188** (zero-drift op-amp) current-sense stage. 0 at idle. The counts-to-amps mapping is not yet established; it will come from measuring the circuit. |
| **PA0** | PA0 / 0 | 7.5 cycles | as above | **Unknown.** Reads 0 at idle and ≈ 380 counts at 0.8 A, so it rises with load current. Possibly the other OPA2188 half (error amp) or the gate drive. |
| **Aux** | PA7 / 7 | 55.5 cycles | 10 Hz | **Unknown.** Reported as avg×8 × 0.765 (as the original firmware did); reads 2 to 3 at idle. |
| **NTC1** | PB0 / 8 | 55.5 cycles | 10 Hz | Heatsink NTC. A 100-entry table maps it to 0 to 99 °C. |
| **NTC2** | PB1 / 9 | 55.5 cycles | 10 Hz | Second NTC. Reads 0 on this unit, so it is likely not fitted or open. |
| **MCU temperature** | internal / 16 | 239.5 cycles | 10 Hz | T = ((14300 − 10·mV)·100/43 + 2500)/10 in 0.1 °C, where mV = avg × CFG `VREF` / 4095. |

**NTC conversion details:**
- The value used is a 10-sample average with min/max dropped, scaled to 10 bits (ADC / 4).
- The temperature is the first table index whose entry is ≤ that value.
- Hotter than 99 °C keeps the previous value; an open or very cold NTC reads 0.

**Fast channel vs burst caveat:** fast samples are taken in the 10 kHz control interrupt right after the DACs are written. They read about 12% lower than the free-running bursts on both PA6 and PA0 at the same operating point. This may be a ripple synchronised with the control tick; it is under investigation.

### 2.3 Controller outputs that are also streamed

| Signal | Where | Notes |
|---|---|---|
| CC DAC | PA5, DAC ch 2 (12-bit, unbuffered) | Current setpoint into the analog CC loop. Read back from DAC DOR2. |
| CV DAC | PA4, DAC ch 1 | Voltage threshold for the analog CV loop; 4095 = CV disengaged. |
| I range | PB2 output | High = high current range |
| V range | PA1 | Output low = high voltage range |
| Load inhibit | PA15 | High = load off (hardware enable) |
| AUX out | PB11 | Goes low when the load turns on and high (pulsed) on a protection fault; command 0xFFFF holds it high while off. |
| Fan | PB4 | 50 Hz software PWM, 0 to 200 |
| Buzzer | PB10 | 1 kHz at 20% duty, in bursts |
| Bias PWM | PB3, TIM2 CH2 | 960 kHz, 32%, always on (probably a charge pump or bias supply) |
| RS-485 DE | PC13 | Transmit enable |
| DAC8562 | PB12 to PB14 | Initialised exactly as the original firmware did, then never written again |

### 2.4 Derived values

| Value | How it is computed | Rate |
|---|---|---|
| P, R (firmware) | From the latest V and I after every V or I sample | about 490/s |
| Charge, energy | Integrated every 0.5 s | 2 Hz |
| Run time | Counts seconds since boot, including while the load is off | 1 Hz |

---

## 3. Operating modes (holding register 1)

| Code | Mode | Setpoint used |
|---|---|---|
| 0 | CV: sinks whatever current holds V at set V | set V (CC DAC = 0; CC DAC = full if set V = 0) |
| 1 | CC | set I (CV DAC = full when set V = 0) |
| 2 | CP: set I = P / V, falls back to CV at very low voltage | set P |
| 3 | CR: set I = V / R, same fallback | set R |
| 4 | CC "fast": now identical to CC (it only skipped the original soft start, which is removed) | set I |
| 5 | CR variant (does not zero set V) | set R |
| 9 | List, 2 steps: alternates preset 1 and 2 | presets, durations in 0.1 ms |
| 10 | List, 10 steps | presets 1 to 10 |
| 11 to 20 | Preset slots 1 to 10 | stored value of the type in CFG `PRESET_TYPE` (0 = V, 1 = I, 2 = P, 3 = R) |

At boot, a mode outside 0 to 5, 9 or 10 becomes 1.

---

## 4. Modbus RTU

### 4.1 Link

- **Interface:** USART2 over RS-485 (PA2/PA3, DE on PC13), 8N1.
- **Address:** CFG `MB_ADDR`, default 1. Address 0 is broadcast: writes are applied and no reply is sent.
- **Saved baud:** index CFG `MB_BAUD`:

  | Index | Baud | Index | Baud |
  |---|---|---|---|
  | 0 | 600 | 10 | 56000 |
  | 1 | 1200 | 11 | 57600 |
  | 2 | 2400 | 12 | 76800 |
  | 3 | 4800 | **13** | **115200** (default) |
  | 4 | 7200 | 14 | 128000 |
  | 5 | 9600 | 15 | 256000 |
  | 6 | 14400 | 16 | 460800 |
  | 7 | 19200 | 17 | 921600 |
  | 8 | 38400 | 18 | 1000000 |
  | 9 | 43000 | 19 | 1500000 (USART maximum) |

  Indices 16 to 19 are new; the original firmware only knows 0 to 15.
- **Runtime baud:** registers 0x184/0x185 change the rate without saving it.
- **Frame end:** 2 × 3.5 character times, minimum 0.5 ms.
- **Transmit:** replies are sent by DMA, and received bytes are ignored while transmitting.
- **Functions:** 3 (read holding), 4 (read input), 6 (write one), 16 (write many), and 0x41 (stream read, section 5).
- **Exceptions:** 1 = illegal function, 2 = illegal address, 3 = illegal count or byte count.
- **Count limits:**

  | Request | Maximum registers |
  |---|---|
  | Read, registers 0 to 12 | 13 |
  | Read, extension registers | 125 |
  | Write many, extension registers | 27 |

### 4.2 Holding registers 0 to 12 (FC 3 / 6 / 16)

These are the original firmware's map.

| Reg | Name | Units / meaning |
|---|---|---|
| 0 | Address | Changing it applies immediately and saves the config. |
| 1 | Mode | See section 3. RAM only; save with command 0x78. |
| 2 | On/off | 1 = load on |
| 3 | Set V | 10 mV |
| 4 | Set I | mA. During command 0x6E it reads the reference meter value instead. |
| 5 | Set P | 10 mW × pdiv |
| 6 | Set R | 0.1 Ω |
| 7 | Command | See section 4.4 |
| 8 | Value | Argument and readback for the command register (section 4.4) |
| 9 | VIN gain register | Writing it adjusts the active VIN gain live: below CFG 0x34 + 1000 it goes to the low range, otherwise the high range. |
| 10 | V gain register | Writing it adjusts the active V gain live: below 3851 it goes to the low range, otherwise the high range. |
| 11 | I gain | CFG `I_GAIN`. During command 0x6E it reads the calibration gain instead and writes are ignored. |
| 12 | I-DAC cal | Active current-DAC factor (factory path) |

Any write to holding registers re-applies all 13 values from a snapshot taken at the start of
the request. That snapshot is fresh, so writing one register has no side effects on the others.

### 4.3 Input registers 0 to 12 (FC 4)

| Reg | Name | Units |
|---|---|---|
| 0 | Address | |
| 1 | Mode, or the list step in modes 9/10 | |
| 2 | On/off, or the list step time in modes 9/10 | 0.1 ms |
| 3 | Temperature | NTC1 in °C. Command 7 switches it to MCU temperature (0.1 °C), command 8 to NTC2. |
| 4 | Aux (PA7) | firmware units |
| 5 | VIN | mV |
| 6 | V | 10 mV |
| 7 | I | mA. During command 0x6F it shows the reference meter instead. |
| 8 | P | 10 mW × pdiv |
| 9 | R | 0.1 Ω |
| 10 | Charge | mAh |
| 11 | Energy | 10 mWh |
| 12 | Run time | s (16-bit, wraps) |

### 4.4 Command register (holding 7) and value register (holding 8)

Commands come in four kinds:
- **Direct:** write the command.
- **Value:** write the value, then the command (FC16 writing registers 7 and 8 together works). It applies and saves.
- **Parameter:** write the command. The firmware loads the current setting into the value register; write a new value to register 8 and it is stored and saved.
- **Read:** write the command, then read the value register.

| Code | Kind | Action |
|---|---|---|
| 0 | | None / idle |
| 1 | Direct | Fan full speed while selected |
| 2 | Direct | Beep while selected |
| 7 / 8 | Direct | Input register 3 shows MCU temperature / NTC2 |
| 10 | Direct | Reset charge and energy |
| 0x0B | Read | Active V-DAC scale (factory CV path) |
| 0x0C | Read | Active I-measurement gain (factory path) |
| 0x0D / 0x0E | Read | CFG V-DAC scale low / high |
| 0x0F / 0x10 | Read | CFG V zero / I zero |
| 0x11 | Read | CFG Vref (mV) |
| 0x12 | Read | CFG I trim limit |
| 0x4D | Parameter | CFG `V_FROM_VIN` |
| 0x58 | Parameter | CFG cut-off voltage (10 mV; 0 = off). The load turns off when V ≤ cut-off. |
| 0xE8 | Parameter | Saved baud index (0 to 19) |
| 500 | Parameter | CFG power resolution (0: 10 mW, ≠ 0: 100 mW) |
| 0x1E5 | Parameter | Modbus address (≥ 1) |
| 1000 / 1001 | Parameter | Legacy current ramp down / up in the original unit, 5 A/s per count (1 to 2750). Writes through to the ramp registers; reads return the nearest count. Use registers 0x1C0 to 0x1C3 instead (section 4.7). |
| 0xFFF8 (−8) | Parameter | CFG range fallback |
| 0xFFF9 (−7) | — | Was the soft-start setting; soft start is removed and the command does nothing |
| 0xFFFA (−6) | Value | I trim limit = min(value, Imax) |
| 0xFFFB (−5) | Value | Vmax = value (also the saved V) |
| 0xFFFC (−4) | Value | Imax = value |
| 0xFFFD (−3) | Value | Pmax = value |
| 0xFFFE (−2) | Value | Vref = value (only if ≥ 2000) |
| 0xFFFF (−1) | Direct | Hold AUX out (PB11) high while the load is off |
| 1010 / 1011 | Direct | Set V zero from the present reading / clear it |
| 2010 / 2011 | Direct | Set I zero from the present reading / clear it |
| 0x78 | Direct | Save the config page to flash |
| 0x79 | Direct | Store the active I-DAC factor in the table slot for the present set I |
| 0x6E, 0x6F | Factory cal | Current-measurement / current-DAC table calibration. Needs the original SCPI bench supply on USART1. |
| 0x73, 0x74 | Factory cal | CV DAC scale, low (30 V) / high (100 V) |
| 0x76, 0x77 | Factory cal | V / VIN measurement gain (apply 30 V and 100 V / 30 V and 60 V) |

Register 8 readback depends on the command selected:

| Command | Register 8 shows |
|---|---|
| 0x0B, 0x73, 0x74 | Active V-DAC scale |
| 0x0C | Active I-measurement gain |
| 0x0D to 0x12 | The configuration value listed in the table above |
| Anything else | The value last written |

### 4.5 Linear calibration block, 0x100 to 0x147

This is the gain/offset trim layered on top of the factory calibration. A channel uses it only
while its bit in 0x107 is set.

| Reg | Access | Meaning |
|---|---|---|
| 0x100 | R/W | Raw mode. 1 = DACs come from 0x104/0x105 and ranges can be forced. It drops out, and the load turns off, after 10 s without Modbus traffic. |
| 0x101 | R/W | Force V range: 0 auto, 1 low, 2 high (raw mode only) |
| 0x102 | R/W | Force I range: 0 / 1 / 2 (raw mode only) |
| 0x103 | R/W | Force VIN range: 0 / 1 / 2 (raw mode only) |
| 0x104 | R/W | Raw CC DAC code, 0 to 4095 (applied only while the load is on) |
| 0x105 | R/W | Raw CV DAC code |
| 0x106 | R/W | Action: 0x5A01 save to flash, 0x5A02 erase, 0x5A03 reload. Reads back the last result (0 ok, 1 failed). |
| 0x107 | R/W | Valid mask, bit n = channel n (see the channel table below) |
| 0x108 | R | Version (0x0201) |
| 0x110 / 0x111 / 0x112 | R | Latest raw V / I / VIN counts |
| 0x113 | R | Ranges: b0 V high, b1 I high, b2 VIN high |
| 0x114 / 0x115 | R | Sample counters: one per I sample / one per VIN sample |
| 0x116 / 0x117 / 0x118 | R | V (10 mV) / I (mA) / VIN (mV) |
| 0x119 to 0x11B | R | PA0 burst average / min / max (64 samples, every 100 ms) |
| 0x11C to 0x11E | R | PA6 burst average / min / max |
| 0x120 + 4n | R/W | Channel n gain, high word. Gain is int32 = gain × 1e6. |
| 0x121 + 4n | R/W | Channel n gain, low word |
| 0x122 + 4n | R/W | Channel n offset, high word (int32, output units) |
| 0x123 + 4n | R/W | Channel n offset, low word |

Each linear channel computes y = x × gain / 1e6 + offset. Write all four of a channel's words together with FC16,
because each word takes effect as soon as it is written.

| n | Channel | x (input) | y (output) |
|---|---|---|---|
| 0 | V_LO | raw count | 10 mV |
| 1 | V_HI | raw count | 10 mV |
| 2 | I_LO | raw count | mA |
| 3 | I_HI | raw count | mA |
| 4 | VIN_LO | raw count | mV |
| 5 | VIN_HI | raw count | mV |
| 6 | IDAC_LO | mA | CC DAC code |
| 7 | IDAC_HI | mA | CC DAC code |
| 8 | VDAC_LO | 10 mV | CV DAC code |
| 9 | VDAC_HI | 10 mV | CV DAC code |

The record is stored at 0x0800EC00 with magic "SLC1" and a CRC32. The factory config page is not touched.
When an IDAC or VDAC channel is active, the factory closed-loop trim for that path is disabled.

### 4.6 Stream block, 0x180 to 0x190

| Reg | Access | Meaning |
|---|---|---|
| 0x180 | R/W | Stream enable. Enabling clears the ring buffer. |
| 0x181 | R/W | Fast-channel divider: 0 = off, N = 10 kHz / N (1 to 10000) |
| 0x182 | R/W | Fast-channel mask: b0 PA6, b1 PA0, b2 CC DAC code, b3 CV DAC code |
| 0x183 | R/W | State record period in ms (0 = off; default 20) |
| 0x184 | R/W | Runtime baud, high word |
| 0x185 | R/W | Runtime baud, low word. Write both with FC16. It is applied after the reply has been sent; if no valid frame arrives within 3 s at the new rate, the firmware reverts to the saved rate. |
| 0x186 | R | Ring overflow count (records dropped because the host polled too slowly) |
| 0x187 | R | Fast-block overflow count |
| 0x188 / 0x189 | R | Ring fill / ring size (2560 bytes) |
| 0x18A | R | ADS1115 bus errors |
| 0x18B | R | I samples discarded because the range changed mid-conversion |
| 0x18C / 0x18D | R | Device time in µs, high / low word |
| 0x18E | R | ADS1115 conversions per second |
| 0x18F | R | Stream protocol version (1) |
| 0x190 | R | ADS1115 first-poll delay in µs (adaptive) |

### 4.7 Current ramp block, 0x1C0 to 0x1C3

These are ordinary read/write registers. A write takes effect at once and saves the config page.

| Reg | Access | Meaning |
|---|---|---|
| 0x1C0 / 0x1C1 | R/W | Ramp up, in mA/s (A/s × 1000, so 0.001 A/s resolution). 32-bit, high word / low word. 0 = no limit. The maximum is 1,000,000 A/s; larger values are clamped. |
| 0x1C2 / 0x1C3 | R/W | Ramp down, same format |

Write a 32-bit rate with one FC16 covering both words. The firmware holds the high word until the low
word arrives, then applies the value. A lone FC6 write to the low word sets the high word to 0, so rates
up to 65.535 A/s can also be written with FC6. Example: 2.5 A/s up and 0.75 A/s down is FC16 to 0x1C0
with four values `0, 2500, 0, 750`.

How the ramp behaves:

- The current setpoint moves toward its target once every 0.1 ms control tick, by rate × 0.1 ms. The
  ramp is tracked to 0.0001 mA, so slow rates are exact, and the last step lands on the target. The CC
  DAC is driven from the whole-mA part.
- It applies only to the CC DAC. That covers CC and CC-fast (identical now), and CP / CR while they
  regulate through the CC DAC. The CV DAC is not ramped. So CV mode, and CP / CR whenever they hand
  regulation to the CV loop, step at the analog loop's speed.
- **Load on** ramps the current up from 0 at the ramp-up rate.
- **Load off** is immediate, with no ramp: the CC DAC goes to 0 and the output is disabled (PA15 high) on
  the next 0.1 ms tick. The same goes for every other off path: protection trips (OVP/OCP/OPP/OTP),
  battery cut-off, preset switching and raw-mode timeout. The hard power limit also cuts at once.
- **Set I = 0** while on is treated as a lower setpoint and ramps down at the ramp-down rate. The range
  and DAC factor of the previous setpoint stay in force until the ramp reaches 0. The STATE record's
  "ramping to 0" field is 1 while this happens. Writing a new nonzero setpoint meanwhile ramps to it from
  wherever the current is.
- A setpoint change that crosses 5.5 A switches the current range at once, not when the ramp gets there.
  Ramping down from above 5.5 A to a setpoint below it therefore clamps the current to the low range's
  top until the ramp comes down to it. Turning off from above 5.5 A ramps in the high range and is not
  affected.
- Soft start (the original firmware's DAC-ceiling delay) has been removed. Config offset 0x18 and
  command −7 no longer do anything.
- This sets how fast the firmware moves the setpoint. The analog CC loop adds its own response, which
  dominates at very high rates.
- Saving rewrites the flash page, which stalls the controller for a few tens of ms (true of every config
  save). Avoid changing the ramp registers in the middle of a transient you are measuring.

The rates are stored as 32-bit values at config offsets 0x38 and 0x3C (section 4.8). The legacy u16 fields
at 0x1A / 0x1C are kept in step for the original commands 1000 / 1001. On the first boot of this firmware
version, the rates are converted from those legacy fields (count × 5 A/s, so the factory 20 / 100 become
100 / 500 A/s).

### 4.8 Config page window, 0x400 to 0x55D (read only)

Register 0x400 + n returns RAM config word n, which is config byte offset 2n. The page lives in flash
at 0x0800F000 and keeps the original firmware's layout, so the factory calibration is reused as is.

| Byte offset | Register | Field |
|---|---|---|
| 0x00 | 0x400 | Magic 0x07E4 (low word) |
| 0x04 | 0x402 | Firmware marker 0xB0D5 (this firmware has run) |
| 0x0A / 0x0C / 0x0E | 0x405 / 0x406 / 0x407 | Vmax / Imax / Pmax |
| 0x10 / 0x12 / 0x14 | 0x408 / 0x409 / 0x40A | Setpoints restored at boot: V / I / P |
| 0x18 | 0x40C | Original soft-start steps; unused by this firmware |
| 0x1A / 0x1C | 0x40D / 0x40E | Legacy ramp up / down (5 A/s per count), kept in step with 0x38 / 0x3C |
| 0x20 | 0x410 | Mode |
| 0x22 | 0x411 | Preset type |
| 0x24 / 0x26 | 0x412 / 0x413 | Modbus address / baud index |
| 0x28 | 0x414 | I gain |
| 0x2A | 0x415 | Vref (mV) |
| 0x2C / 0x2E | 0x416 / 0x417 | V zero / I zero |
| 0x30 / 0x32 | 0x418 / 0x419 | V gain low / high |
| 0x34 / 0x36 | 0x41A / 0x41B | VIN gain low / high |
| 0x38 to 0x3E | 0x41C to 0x41F | Ramp up, then ramp down: 32-bit mA/s, low word first. Used only by this firmware; the original never reads these words. |
| 0x40 / 0x42 | 0x420 / 0x421 | V-DAC scale low / high |
| 0x4E | 0x427 | V from VIN |
| 0x50 to 0x6C | 0x428 to 0x436 | I-measurement gain table, low range (15 entries) |
| 0x6E to 0x128 | 0x437 to 0x494 | I-measurement gain table, high range (sparse) |
| 0x12A | 0x495 | I-DAC default factor |
| 0x12C to 0x1C6 | 0x496 to 0x4E3 | I-DAC calibration table |
| 0x1EA | 0x4F5 | Power resolution |
| 0x1EC | 0x4F6 | Range fallback |
| 0x1EE | 0x4F7 | Cut-off voltage |
| 0x1F0 | 0x4F8 | I trim limit |
| 0x1F2 to 0x204 | 0x4F9 to 0x502 | Preset values 1 to 10 |
| 0x256 to 0x268 | 0x52B to 0x534 | Preset on-states / list durations 1 to 10 (0.1 ms) |

---

## 5. Streaming (FC 0x41)

The firmware writes every reading into a 2560-byte ring buffer as a timestamped record. The host
drains it with a user-defined function code:

```
request   [addr][0x41][max hi][max lo][crc lo][crc hi]          max = payload bytes wanted (<= 1032)
response  [addr][0x41][seq][flags][len hi][len lo][len bytes][crc lo][crc hi]
          seq   increments every reply (a gap means a lost reply)
          flags b0 ring overflow since last read, b1 fast overflow, b2 stream enabled
```

The payload is whole records: `[type u8][len u8][body]`, little endian. Timestamps are microseconds
since boot (u32, wraps after 71.6 min).

| Type | Record | Body |
|---|---|---|
| 0x01 | V sample | ts u32, raw u16 (b15 = high range), V u16 (10 mV) |
| 0x02 | I sample | ts u32, raw u16 (b15 = high range), I u16 (mA) |
| 0x03 | VIN sample | ts u32, raw u16 (b15 = high PGA), VIN u16 (mV) |
| 0x04 | Slow (10 Hz) | ts u32, aux u16, PA0 avg/min/max u16, PA6 avg/min/max u16, NTC1 u8, NTC2 u8, MCU temp u16, charge µAh u32, energy u32, seconds u32 |
| 0x05 | State | ts u32, on u8, mode u8, cmd u16, value u16, set V/I/P/R u16, P u16, R u16, CC DAC u16, CV DAC u16, I ramp u16 (mA), ramping to 0 u16 (1 while the current ramps down after set I = 0; was the soft-start ceiling), I-DAC cal u16, V-DAC scale u16, I-meas gain u16, flags u8, fan u8, list step u8, pdiv u8, list time u16, VIN gain reg u16, V gain reg u16 |
| 0x06 | Fast block | ts u32 (first sample), period u32 (µs), mask u8, n u8, then n samples × one u16 per mask bit in the order PA6, PA0, CC DAC, CV DAC. 0xFFFF = ADC skipped because a slow conversion was running. |
| 0x07 | Event | ts u32, code u8, data u16 |

State record flags:

| Bit | Meaning |
|---|---|
| b0 | I range pin |
| b1 | V range |
| b2 | VIN range |
| b3 | Alarm |
| b4 | DAC settled |
| b5 | Raw mode |
| b6 | AUX out pin |
| b7 | Load inhibit pin |

Event codes:

| Code | Event |
|---|---|
| 1 | Protection trip |
| 2 | Protection cleared |
| 3 | Config saved |
| 4 | Linear calibration saved (data = ok) |
| 5 | Baud changed (data = baud / 100) |
| 6 | ADS1115 bus error |
| 7 | Stream started |

**Bandwidth:** everything at full rate, including PA6 and PA0 at 10 kHz, needs about 50 kB/s, which
means 1.5 Mbaud. At 115200 the link carries only about 11 kB/s; drop the fast channels or slow the
state records there.

---

## 6. Protection and safety timers

| Condition | Threshold | Action |
|---|---|---|
| Over-voltage | V > Vmax + 3.00 V | After 30 ms the load turns off and the alarm and buzzer start. AUX out is pulsed. |
| Over-current | I > Imax + 3 A (or + 0.6 A if Imax ≤ 5.5 A) | Same |
| Over-power | P > Pmax + 10 % | Same |
| Over-temperature | NTC1 or NTC2 > 85 °C | Same |
| Hard power limit | Set I × V ≥ 1.25 × Pmax | Set I forced to 0 (10 kHz) |
| Cut-off voltage | V ≤ CFG cut-off (if non-zero) | Load off |
| Alarm clear | Condition gone for 3 s | Alarm off |
| Raw mode | No Modbus traffic for 10 s | Raw mode off, load off |
| Runtime baud | No valid frame within 3 s of a switch | Back to the saved baud |
| Watchdog | Main loop or 10 kHz tick stalls about 1 s | MCU reset |

---

## 7. Tools

| Tool | Purpose |
|---|---|
| `tools/sousim_gui.py -p COM10` | Live plots of every channel above, register editing, current ramp (A/s), linear-calibration trims, fast-channel setup. Switches to 1.5 Mbaud for the session. |
| `tools/sousim_cal.py` | Guided linear calibration, plus `show` / `enable` / `disable` / `clear` / `save` |
| `tools/bench.py` | SCPI helpers for the OWON SPE6103 supply and XDM2041 DMM |
