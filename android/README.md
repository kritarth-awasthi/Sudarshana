# Sudarshana Android App

## Prototype Status
The prototype Android application was built in Kotlin and functioned as an
SMS receiver — emergency contacts received a Google Maps link via SMS with
live GPS coordinates upon panic detection.

Direct ESP32 ↔ App communication was not implemented in the prototype.
All device output was routed through the SIM800L GSM module.

## Tech Stack
- Kotlin.
- Android SDK.
- SMS receiver (for prototype).

## Full Vision Roadmap
The complete application was designed to include:

- **Contact Management** — select emergency contacts from phonebook.
- **Danger Zone Mapping** — mark specific geographic areas as danger zones
  via Google Maps API; device automatically enters ALERT mode on entry.
- **BLE Sync** — push selected contacts and danger zone polygons to
  ESP32 SPIFFS storage via Bluetooth Low Energy.
- **Live Tracking** — real-time location session when PANIC mode is active.
- **Crime Area Integration** — pull crime-prone area data from public.
  government datasets to auto-trigger ALERT mode
- **SOS Dashboard** — confirm SOS received, track responder status.

## Why BLE for App ↔ Device Communication
BLE was chosen over WiFi for the full vision because:
1. Low power drain during continuous operation.
2. No network infrastructure dependency
3. Direct phone-to-device pairing without router
4. Standard Android BLE APIs — no custom drivers

---
*Kritarth Awasthi  | Jan 2024 – May 2025*
