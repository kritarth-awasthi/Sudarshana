# Sudarshana — IoT Safety Wearable

> Wearable panic-detection device that monitors physiological stress indicators.
> and automatically dispatches GPS SOS via GSM upon adrenaline spike detection.

**Status:** Prototype complete · Full vision in roadmap  
**Developer:** Kritarth Awasthi  
**Duration:** January 2024 – May 2025

---

## What it does

The device continuously monitors two biometric signals — heart rate and galvanic
skin response (GSR/skin conductance) — to detect physiological stress signatures
associated with an adrenaline spike. When both signals exceed threshold simultaneously,
the device automatically transmits an SOS SMS containing a live Google Maps link
to emergency contacts via the SIM800L GSM module.

A physical button provides manual override — one press enters ALERT mode
(sensors active), second press triggers immediate SOS.

---

## How it works

User activates panic trigger -
GSM SIM module auto-transmits SOS SMS with live GPS coordinates -
Emergency contacts receive location in under 60 seconds -
Companion Android app shows real-time tracking -

---

## Hardware

| Component | Model | Function |
|---|---|---|
| Microcontroller | ESP32 | State machine, sensor fusion |
| Heart Rate | MAX30105 (I2C) | Optical pulse detection |
| Skin Conductance | Analog GSR | Stress sweat response |
| GSM | SIM800L | SMS SOS dispatch |
| GPS | NEO-6M (UART) | Location coordinates |
| Trigger | SPDT Button | Manual ALERT/PANIC toggle |

---

## State Machine

```
IDLE ──(button/trigger)──► ALERT ──(adrenaline spike / button)──► PANIC
Sensors OFF                 Sensors ON @ 1Hz                       SOS SMS sent
Low power                   Dual-threshold monitoring              GPS embedded
```

---

## Why dual-sensor adrenaline detection?

Using heart rate alone produces false positives during exercise.
Using GSR alone produces false positives in humid conditions.
Combined dual-threshold eliminates both false positive categories —
both signals must be elevated simultaneously to trigger SOS.

**Prototype:** Hardcoded threshold values (HR > 110 BPM + GSR > 600).
**Roadmap:** TFLite Micro ML model trained on WESAD stress dataset.

---

## SMS Format (Prototype)

```
SUDARSHANA ALERT: Emergency detected.
Location: https://maps.google.com/?q=26.912434,75.787271
```

---

## Setup

1. Install required libraries (see `firmware/sudarshana_prototype/libraries.txt`)
2. Open `firmware/sudarshana_prototype/sudarshana_prototype.ino` in Arduino IDE.
3. Edit `config.h` — set `EMERGENCY_NUMBER` and verify GPIO pins match your wiring.
4. Select board: `ESP32 Dev Module`
5. Flash at 115200 baud.
6. Monitor Serial at 115200 for debug output.

---

## Full Vision Roadmap

- [ ] Android app — contact selection, danger zone mapping, BLE sync to device
- [ ] Geofencing — auto ALERT on entry into marked danger zones
- [ ] Crime area auto-trigger — public dataset integration
- [ ] Multi-contact SOS dispatch
- [ ] ML-based stress detection (TFLite Micro)
- [ ] Nearest police station notification
- [ ] V2 hardware — smaller form factor, dedicated PCB

---

## Circuit Diagram

See `docs/system_architecture.md` for full wiring reference.

---

*Kritarth Awasthi | Jan 2024 – May 2025*
