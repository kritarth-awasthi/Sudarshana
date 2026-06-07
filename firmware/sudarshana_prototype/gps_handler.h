/*
 * gps_handler.h — NEO-6M GPS Module Handler
 * Wraps TinyGPS++ for coordinate retrieval.
 */

#pragma once
#include <TinyGPS++.h>
#include <HardwareSerial.h>

class GPSHandler {
public:
  GPSHandler(HardwareSerial* serial, TinyGPSPlus* gps)
    : _serial(serial), _gps(gps) {}

  // Feed GPS parser — call this every loop() iteration
  void update() {
    while (_serial->available()) {
      _gps->encode(_serial->read());
    }
  }

  // Returns latitude — 0.0 if no fix yet
  float getLatitude() {
    return _gps->location.isValid() ? _gps->location.lat() : 0.0f;
  }

  // Returns longitude — 0.0 if no fix yet
  float getLongitude() {
    return _gps->location.isValid() ? _gps->location.lng() : 0.0f;
  }

  bool hasFix() {
    return _gps->location.isValid();
  }

  // Returns age of GPS fix in milliseconds
  unsigned long fixAge() {
    return _gps->location.age();
  }

private:
  HardwareSerial* _serial;
  TinyGPSPlus*    _gps;
};
