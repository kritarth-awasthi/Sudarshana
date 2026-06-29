/*
 * gsm_handler.h — SIM800L GSM Module Handler
 * Manages AT command communication with the SIM800L for SMS dispatch.
 */

#pragma once
#include <HardwareSerial.h>

class GSMHandler {
public:
  GSMHandler(HardwareSerial* serial) : _serial(serial) {}

  // Initialise SIM800L and verify network registration
  bool begin() {
    delay(3000);  // SIM800L power-on delay
    sendAT("AT");
    if (!waitForResponse("OK", 3000)) return false;

    sendAT("AT+CMGF=1");  // Set SMS to text mode
    if (!waitForResponse("OK", 2000)) return false;

    sendAT("AT+CREG?");   // Check network registration
    delay(1000);

    Serial.println(F("[GSM] Network registered!"));
    return true;
  }

  // Send SMS to specified number
  bool sendSMS(const String& number, const String& message) {
    sendAT("AT+CMGF=1");
    delay(500);

    _serial->print("AT+CMGS=\"");
    _serial->print(number);
    _serial->println("\"");
    delay(500);

    _serial->print(message);
    _serial->write(26);  // Ctrl+Z to send
    delay(500);

    return waitForResponse("+CMGS:", 10000);
  }

private:
  HardwareSerial* _serial;

  void sendAT(const String& command) {
    _serial->println(command);
    delay(100);
  }

  bool waitForResponse(const String& expected, unsigned long timeout) {
    unsigned long start = millis();
    String response = "";
    while (millis() - start < timeout) {
      while (_serial->available()) {
        char c = _serial->read();
        response += c;
        if (response.indexOf(expected) != -1) return true;
      }
    }
    return false;
  }
};
