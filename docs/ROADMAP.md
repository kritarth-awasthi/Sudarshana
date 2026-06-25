# Sudarshana — Full Vision Roadmap

## Original Design Goal

Sudarshana was conceived as a complete personal safety ecosystem — not just a panic button, but a proactive threat-aware wearable that monitors physiological stress and environmental danger simultaneously.

## Full System Architecture (Planned)

```
┌─────────────────────────────────────────────────────────────┐
│                    SUDARSHANA ECOSYSTEM                     │
├─────────────────┬───────────────────────┬───────────────────┤
│   WEARABLE      │     ANDROID APP       │   CLOUD/ML        │
│                 │                       │                   │
│ MAX30102 (HR)   │ Emergency contacts    │ Crime zone DB     │
│ GSR Sensor      │ Danger zone marking   │ Adrenaline ML     │
│ NEO-6M GPS      │ Live location track   │ model training    │
│ SIM800L GSM     │ Alert mode toggle     │                   │
│ ESP32 MCU       │ BLE config sync       │                   │
│ Panic Button    │                       │                   │
└─────────────────┴───────────────────────┴───────────────────┘
```

## Phase 1 — Prototype (Complete)
- Biometric monitoring (HR + GSR)
- Threshold-based spike detection
- GPS coordinate acquisition
- SOS SMS to hardcoded number
- Physical panic button

## Phase 2 — Android App Integration (Planned)
- BLE pairing between wearable and app
- Dynamic emergency contact management
- User-defined danger zone geofencing
- Real-time location sharing during panic

## Phase 3 — ML Adrenaline Classifier (Planned)
- Collect labelled HR + GSR dataset across stress/non-stress states
- Train binary classifier (stressed / not-stressed)
- Quantise and deploy on ESP32 via TFLite Micro
- Replace threshold logic with model inference

## Phase 4 — Environmental Awareness (Planned)
- Integration with crime statistics API (NCRB data)
- Automatic alert mode activation in high-risk zones
- Nearest police station geolocation and SMS routing
