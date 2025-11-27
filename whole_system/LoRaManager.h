/*
 * LoRaManager.h
 * Wrapper for the RN2483 library
 */

#ifndef LORA_MANAGER_H
#define LORA_MANAGER_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <rn2xx3.h>

class LoRaManager {
  public:
    // Constructor
    LoRaManager(uint8_t rxPin, uint8_t txPin, uint8_t resetPin);

    // Setup
    void begin();
    void joinOTAA(const char *appEui, const char *appKey);

    // Sending (Returns the Downlink Plant Type, or -1 if none)
    int sendSetupMessage(); 
    int sendSensorData(uint8_t hum, int8_t temp, uint8_t soil, uint8_t light, uint8_t batt);

  private:
    uint8_t _resetPin;
    SoftwareSerial _loraSerial;
    rn2xx3 _myLora;
    
    void resetModule();
    int handleTxResult(TX_RETURN_TYPE result); // Helper to parse response
};

#endif