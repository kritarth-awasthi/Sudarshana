/*
 * ╔══════════════════════════════════════════════════════════════╗
 * ║                    S U D A R S H A N A                       ║
 * ║              IoT Safety Wearable — Prototype v1.0            ║
 * ║                                                              ║
 * ║  Developer : Kritarth Awasthi                                ║
 * ║  Hardware  : ESP32 + MAX30105 + GSR + SIM800L + NEO-6M       ║
 * ║  Status    : Prototype — SOS to hardcoded number             ║
 * ╚══════════════════════════════════════════════════════════════╝
 *
 *  STATE MACHINE:
 *  IDLE → (button/trigger) → ALERT → (adrenaline spike) → PANIC
 *  Sensors OFF               Sensors ON continuous         SOS SMS sent
 */

#include <Wire.h>
#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "config.h"
#include "gsm_handler.h"
#include "gps_handler.h"
#include "sensor_handler.h"

//  System State Machine 
enum SystemState { STATE_IDLE, STATE_ALERT, STATE_PANIC };
SystemState currentState = STATE_IDLE;

//  Hardware Serial Ports 
HardwareSerial gsmSerial(1);  // SIM800L  → UART1: TX=GPIO17, RX=GPIO16
HardwareSerial gpsSerial(2);  // NEO-6M   → UART2: TX=GPIO19, RX=GPIO18

//  Object Instances 
TinyGPSPlus   gps;
MAX30105      heartRateSensor;
GSMHandler    gsm(&gsmSerial);
GPSHandler    gpsHandler(&gpsSerial, &gps);
SensorHandler sensors(&heartRateSensor);

//  Timing Variables 
unsigned long alertStartTime  = 0;
unsigned long lastSensorRead  = 0;
bool          sosConfirmed    = false;

//  Button Interrupt 
volatile bool buttonPressed = false;
void IRAM_ATTR onButtonPress() { buttonPressed = true; }

void setup() {
  Serial.begin(115200);
  Serial.println(F("\n[SUDARSHANA] Booting..."));

  pinMode(PIN_PANIC_BUTTON, INPUT_PULLUP);
  pinMode(PIN_ALERT_LED,    OUTPUT);
  pinMode(PIN_PANIC_LED,    OUTPUT);
  pinMode(PIN_GSR_SENSOR,   INPUT);

  attachInterrupt(digitalPinToInterrupt(PIN_PANIC_BUTTON), onButtonPress, FALLING);

  // Initialise GSM
  gsmSerial.begin(GSM_BAUD, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN);
  if (!gsm.begin()) Serial.println(F("[GSM] Init failed"));
  else               Serial.println(F("[GSM] Ready"));

  // Initialise GPS
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  Serial.println(F("[GPS] Waiting for fix..."));

  // Initialise Heart Rate Sensor
  if (!heartRateSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println(F("[HR] MAX30105 not found"));
  } else {
    heartRateSensor.setup();
    heartRateSensor.setPulseAmplitudeRed(0x0A);
    heartRateSensor.setPulseAmplitudeGreen(0);
    Serial.println(F("[HR] MAX30105 Ready"));
  }

  enterIdle();
  Serial.println(F("[SUDARSHANA] Boot complete. State: IDLE"));
}

void loop() {
  gpsHandler.update();

  if (buttonPressed) {
    buttonPressed = false;
    handleButtonPress();
  }

  switch (currentState) {

    case STATE_IDLE:
      // Low power — sensors dormant
      // Woken by: button press
      // ROADMAP: geofence trigger from Android app via BLE
      break;

    case STATE_ALERT:
      if (millis() - lastSensorRead >= SENSOR_READ_INTERVAL_MS) {
        lastSensorRead = millis();
        float heartRate = sensors.getHeartRate();
        float gsrValue  = sensors.getGSRReading();
        Serial.printf("[SENSORS] HR: %.1f BPM | GSR: %.2f\n", heartRate, gsrValue);
        if (isAdrenalineSpike(heartRate, gsrValue)) {
          Serial.println(F("[ALERT] Adrenaline spike detected → PANIC"));
          enterPanic();
        }
      }
      break;

    case STATE_PANIC:
      if (!sosConfirmed) {
        dispatchSOS();
        sosConfirmed = true;
      }
      // Rapid LED flash to signal panic state
      digitalWrite(PIN_PANIC_LED, (millis() / 300) % 2);
      break;
  }
}

// STATE TRANSITIONS

void enterIdle() {
  currentState = STATE_IDLE;
  sensors.sleep();
  digitalWrite(PIN_ALERT_LED, LOW);
  digitalWrite(PIN_PANIC_LED, LOW);
  Serial.println(F("[STATE] → IDLE"));
}

void enterAlert() {
  currentState   = STATE_ALERT;
  alertStartTime = millis();
  sosConfirmed   = false;
  sensors.wake();
  digitalWrite(PIN_ALERT_LED, HIGH);
  Serial.println(F("[STATE] → ALERT"));
}

void enterPanic() {
  currentState = STATE_PANIC;
  sosConfirmed = false;
  digitalWrite(PIN_ALERT_LED, LOW);
  Serial.println(F("[STATE] → PANIC"));
}

// BUTTON HANDLER

void handleButtonPress() {
  Serial.println(F("[BUTTON] Triggered"));
  switch (currentState) {
    case STATE_IDLE:  enterAlert(); break;
    case STATE_ALERT: enterPanic(); break;
    case STATE_PANIC: enterIdle();  break;
  }
}

// ADRENALINE SPIKE DETECTION
// Combines HR elevation + GSR increase — both must exceed threshold.
// ROADMAP: Replace with TFLite Micro ML model trained on stress datasets.

bool isAdrenalineSpike(float heartRate, float gsrValue) {
  return (heartRate > HR_PANIC_THRESHOLD) && (gsrValue > GSR_PANIC_THRESHOLD);
}

// SOS DISPATCH

void dispatchSOS() {
  Serial.println(F("[SOS] Building distress message..."));

  float lat = gpsHandler.getLatitude();
  float lng = gpsHandler.getLongitude();

  String message = "SUDARSHANA ALERT: Emergency detected.\n";
  message += "Location: https://maps.google.com/?q=";
  message += String(lat, 6) + "," + String(lng, 6);

  Serial.println(F("[SOS] Sending SMS...."));

  // Prototype: hardcoded emergency number
  // ROADMAP: Fetch contact list from Android app sync via BLE/WiFi
  // ROADMAP: Dispatch to nearest police station via lookup table
  bool sent = gsm.sendSMS(EMERGENCY_NUMBER, message);
  if (!sent) {
    delay(5000);
    gsm.sendSMS(EMERGENCY_NUMBER, message);  // Single retry
  }
  Serial.println(sent ? F("[SOS] Sent") : F("[SOS] FAILED AFTER RETRY!!"));
}
