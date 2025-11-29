/*
 * LoRaManager.cpp
 * Implementation
 */

#include "LoRaManager.h"

LoRaManager::LoRaManager(uint8_t rxPin, uint8_t txPin, uint8_t resetPin)
  : _resetPin(resetPin), _loraSerial(rxPin, txPin), _myLora(_loraSerial) {
}

void LoRaManager::begin() {
  _loraSerial.begin(9600);
  resetModule();
}

void LoRaManager::resetModule() {
  pinMode(_resetPin, OUTPUT);
  digitalWrite(_resetPin, LOW);
  delay(500);
  digitalWrite(_resetPin, HIGH);
  delay(100);
  
  _loraSerial.flush();
  _myLora.autobaud();
  
  String hweui = _myLora.hweui();
  while (hweui.length() != 16) {
    Serial.println(F("Radio comms failed, retrying..."));
    delay(10000);
    resetModule();
    hweui = _myLora.hweui();
  }
  
  Serial.print(F("Radio Init OK. DevEUI: "));
  Serial.println(_myLora.hweui());
}

void LoRaManager::joinOTAA(const char *appEui, const char *appKey) {
  Serial.println(F("Attempting Join..."));
  bool result = _myLora.initOTAA(appEui, appKey);
  
  while (!result) {
    Serial.println(F("Join failed. Retrying in 60s..."));
    delay(60000);
    result = _myLora.init();
  }
  Serial.println(F("Joined Network Successfully."));
}

// SETUP MODE: Send a small packet to save airtime
int LoRaManager::sendSetupMessage() {
  Serial.println(F("TX: Setup Keep-Alive (Waiting for Type...)"));
  // Sending 1 byte '0' as keep-alive
  byte payload[] = {0x00}; 
  
  // txBytes returns the status immediately after RX windows close
  TX_RETURN_TYPE result = _myLora.txBytes(payload, 1);
  
  return handleTxResult(result);
}

// ACTIVE MODE: Send full sensor data
int LoRaManager::sendSensorData(uint8_t hum, int8_t temp, uint8_t soil, uint8_t light, uint8_t batt) {
  byte payload[5];
  payload[0] = hum;
  payload[1] = (uint8_t)temp; // Cast to uint8 for transmission
  payload[2] = soil;
  payload[3] = light;
  payload[4] = batt;

  Serial.println(F("TX: Sensor Data"));
  TX_RETURN_TYPE result = _myLora.txBytes(payload, 5);

  return handleTxResult(result);
}

// Helper: Checks if the TX resulted in a Downlink
int LoRaManager::handleTxResult(TX_RETURN_TYPE result) {
  
  switch(result) {
    case TX_SUCCESS:
      Serial.println(F("TX Success (No Downlink)"));
      return -1; // No data
      
    case TX_WITH_RX:
      {
        // --- THIS FIXES YOUR ERROR ---
        // 1. Get the Hex String (e.g., "04")
        String hexPayload = _myLora.getRx(); 
        
        Serial.print(F("RX Downlink Hex: "));
        Serial.println(hexPayload);

        // 2. Convert Hex String to Integer
        // "04" becomes integer 4
        int receivedVal = (int)strtol(hexPayload.c_str(), 0, 16);
        
        return receivedVal;
      }
      
    default:
      Serial.println(F("TX Failed or Error"));
      return -1;
  }
}