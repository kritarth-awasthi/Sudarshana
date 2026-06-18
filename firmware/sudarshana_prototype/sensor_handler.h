/*
 * sensor_handler.h — Biometric Sensor Handler
 * Manages MAX30105 heart rate + analog GSR skin conductance sensor.
 *
 * ADRENALINE DETECTION APPROACH:
 * The system uses two physiological signals simultaneously:
 *
 * 1. Heart Rate (MAX30105 optical sensor)
 *    — Elevated HR (>110 BPM) indicates sympathetic nervous system activation
 *    — Red LED reflects off blood vessels; photodetector measures pulse
 *
 * 2. Galvanic Skin Response / GSR (analog sensor on GPIO34)
 *    — Stress triggers eccrine sweat glands, increasing skin conductance
 *    — Higher ADC reading = higher conductance = more stress response
 *    — Used in lie detectors and clinical stress assessment
 *
 * Both must exceed threshold simultaneously to trigger panic — prevents
 * false positives from exercise (HR up, GSR normal) or humidity (GSR up, HR normal).
 *
 * ROADMAP: Replace dual-threshold with TFLite Micro classification model
 *          trained on labelled stress/non-stress physiological datasets.
 */

#pragma once
#include "MAX30105.h"
#include "heartRate.h"
#include "config.h"

class SensorHandler {
public:
  SensorHandler(MAX30105* sensor) : _sensor(sensor) {
    _beatsPerMinute = 0;
    _beatAvg        = 0;
    _lastBeat       = 0;
    _sensorAwake    = false;
  }

  void wake() {
    _sensor->wakeUp();
    _sensorAwake = true;
    Serial.println(F("[SENSORS] Awake"));
  }

  void sleep() {
    _sensor->shutDown();
    _sensorAwake = false;
    Serial.println(F("[SENSORS] Sleeping"));
  }

  // Returns smoothed heart rate in BPM.
  float getHeartRate() {
    if (!_sensorAwake) return 0.0f;

    long irValue = _sensor->getIR();

    if (checkForBeat(irValue)) {
      long delta = millis() - _lastBeat;
      _lastBeat  = millis();
      _beatsPerMinute = 60.0f / (delta / 1000.0f);

      // Sanity check — valid HR range.
      if (_beatsPerMinute > 20 && _beatsPerMinute < 255) {
        // Running average over 4 samples.
        _rates[_rateSpot++] = (byte)_beatsPerMinute;
        _rateSpot %= RATE_SIZE;
        _beatAvg = 0;
        for (byte x = 0; x < RATE_SIZE; x++) _beatAvg += _rates[x];
        _beatAvg /= RATE_SIZE;
      }
    }
    return (float)_beatAvg;
  }

  // Returns raw ADC reading from GSR sensor (0–4095).
  // Higher value = higher skin conductance = higher stress.
  float getGSRReading() {
    int raw = analogRead(PIN_GSR_SENSOR);
    // Average 5 readings to reduce noise
    int sum = raw;
    for (int i = 0; i < 4; i++) {
      delay(10);
      sum += analogRead(PIN_GSR_SENSOR);
    }
    return (float)(sum / 5);
  }

private:
  MAX30105* _sensor;
  bool      _sensorAwake;

  // Heart rate calculation.
  static const byte RATE_SIZE = 4;
  byte          _rates[RATE_SIZE];
  byte          _rateSpot   = 0;
  float         _beatsPerMinute;
  int           _beatAvg;
  long          _lastBeat;
};
