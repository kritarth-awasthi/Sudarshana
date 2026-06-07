# Sudarshana — System Architecture

## Hardware Components

| Component | Model | Role |
|---|---|---|
| Microcontroller | ESP32 (Dual-Core 240MHz) | Central processing, state machine |
| Heart Rate Sensor | MAX30105 (I2C) | Optical pulse detection via photoplethysmography |
| GSR Sensor | Analog (GPIO34 ADC) | Galvanic skin response — sweat conductance |
| GSM Module | SIM800L | SMS SOS dispatch |
| GPS Module | NEO-6M (UART) | Coordinate acquisition |
| Panic Button | SPDT Momentary | Manual ALERT/PANIC toggle |

## Pin Wiring

```
ESP32              SIM800L
GPIO17 (TX) ───► RX
GPIO16 (RX) ◄─── TX
GND         ───── GND
VIN (5V)    ───── VCC (via capacitor — SIM800L draws 2A peak)

ESP32              NEO-6M GPS
GPIO19 (TX) ───► RX
GPIO18 (RX) ◄─── TX
GND         ───── GND
3.3V        ───── VCC

ESP32              MAX30105
GPIO21 (SDA) ──── SDA
GPIO22 (SCL) ──── SCL
GND          ──── GND
3.3V         ──── VCC

ESP32              GSR Sensor
GPIO34 (ADC) ──── SIG
GND          ──── GND
3.3V         ──── VCC
```

## State Machine

```
┌─────────────────────────────────────────────────────────┐
│                      STATE: IDLE                         │
│  • Sensors powered OFF (MAX30105 shutdown mode)          │
│  • GPS parser running (passive)                          │
│  • ESP32 in light sleep between button polls             │
│  • LED: OFF                                              │
└───────────────┬─────────────────────────────────────────┘
                │ Button press (1st press)
                │ ROADMAP: Geofence entry from Android app
                ▼
┌─────────────────────────────────────────────────────────┐
│                     STATE: ALERT                         │
│  • Sensors powered ON                                    │
│  • HR + GSR sampled every 1 second                       │
│  • Adrenaline spike detection active                     │
│  • LED: YELLOW solid                                     │
└───────────────┬─────────────────────────────────────────┘
                │ isAdrenalineSpike() returns true
                │ OR button press (2nd press)
                ▼
┌─────────────────────────────────────────────────────────┐
│                     STATE: PANIC                         │
│  • SOS SMS dispatched via SIM800L                        │
│  • GPS coordinates embedded in message                   │
│  • Google Maps link generated                            │
│  • LED: RED flashing rapidly                             │
│  • Button press (3rd press) → returns to IDLE            │
└─────────────────────────────────────────────────────────┘
```

## Adrenaline Detection Logic

The prototype uses a dual-threshold approach:

- **Heart Rate** > 110 BPM (sympathetic nervous system activation)
- **GSR** > 600 ADC units (eccrine gland activation = stress sweat)

Both must be elevated simultaneously. This prevents false positives:
- Exercise alone: HR up, GSR elevated but from heat not stress
- Humidity: GSR up, HR normal → no trigger

### Roadmap: ML-Based Detection
Replace threshold logic with a TFLite Micro classification model:
- Training data: labelled physiological stress datasets (WESAD, DEAP)
- Features: HR variability (HRV), GSR slope, GSR amplitude
- Output: stress probability score → trigger at >0.85 confidence

---
*Kritarth Awasthi | BIT Mesra, Jaipur*
