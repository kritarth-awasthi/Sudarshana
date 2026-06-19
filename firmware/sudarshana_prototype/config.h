/*
 * config.h — Sudarshana Hardware Configuration
 * Edit these values to match your wiring before flashing.
 */

#pragma once

// ── Emergency Contact (Prototype)
// ROADMAP: Store multiple contacts fetched from Android app
#define EMERGENCY_NUMBER      "+91XXXXXXXXXX"  // Replace with actual number

// ── GPIO Pin Assignments 
#define PIN_PANIC_BUTTON       0    // Physical panic/alert toggle button
#define PIN_ALERT_LED         25    // Yellow LED — alert mode indicator
#define PIN_PANIC_LED         26    // Red LED — panic mode indicator
#define PIN_GSR_SENSOR        34    // GSR analog input (ADC1 only on ESP32)

// ── GSM (SIM800L) 
#define GSM_TX_PIN            17
#define GSM_RX_PIN            16
#define GSM_BAUD           9600

// ── GPS (NEO-6M) ──────────────────────────────────────────────────────────────
#define GPS_TX_PIN            19
#define GPS_RX_PIN            18
#define GPS_BAUD           9600

// ── Heart Rate (MAX30105) I2C ─────────────────────────────────────────────────
#define HR_SDA_PIN            21    // Default ESP32 I2C SDA
#define HR_SCL_PIN            22    // Default ESP32 I2C SCL

// ── Sensor Thresholds ─────────────────────────────────────────────────────────
// HR: Normal resting 60-80 BPM. Panic threshold set at acute stress level.
#define HR_PANIC_THRESHOLD   110.0f  // BPM — sustained elevation = stress
// GSR: Higher value = higher conductance = more stress sweat response
#define GSR_PANIC_THRESHOLD  600.0f  // Raw ADC value (0-4095 on ESP32 12-bit)

// ── Timing ────────────────────────────────────────────────────────────────────
#define SENSOR_READ_INTERVAL_MS  1000   // Read sensors every 1 second in ALERT
#define PANIC_SUSTAIN_MS         5000   // Spike must sustain 5s to trigger SOS

// ── Debug ─────────────────────────────────────────────────────────────────────
#define DEBUG_MODE  true   // Set false to disable Serial output in production
