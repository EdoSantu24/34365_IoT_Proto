/*
 * Plant Monitor Main
 * Board: Arduino Pro Mini 3.3V
 * Logic: Setup Mode -> Receive Type -> Active Mode
 */

#include "LoRaManager.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <EEPROM.h>

// --- Definitions ---
const uint8_t PIN_LORA_RX = 10;
const uint8_t PIN_LORA_TX = 11;
const uint8_t PIN_LORA_RST = 12;
const uint8_t PIN_LED = 13;
const uint8_t PIN_DHT = 7;

// --- TTN Keys (Change these!) ---
const char *APP_EUI = "0000000000000000"; 
const char *APP_KEY = "9B3C437C79252CA4FF947A59F6F13C12"; 

// --- Global Objects ---
LoRaManager lora(PIN_LORA_RX, PIN_LORA_TX, PIN_LORA_RST);
DHT dht(PIN_DHT, DHT11);

// --- State ---
int plantType = 0;          
bool isSetupMode = true;

// --- Timers ---
unsigned long previousMillis = 0;
const long SETUP_INTERVAL = 20000;  // 20 Seconds (Fast for setup)
const long ACTIVE_INTERVAL = 60000; // 60 Seconds (Normal operation)
const int EEPROM_ADDR = 0;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(57600); // PC Serial
  
  // 1. Init Hardware
  dht.begin();
  lora.begin();

  // 2. Check Memory for existing plant type
  uint8_t saved = EEPROM.read(EEPROM_ADDR);
  if (saved >= 1 && saved <= 4) {
    plantType = saved;
    isSetupMode = false;
    Serial.print(F("Loaded Plant Type from Memory: "));
    Serial.println(plantType);
  } else {
    Serial.println(F("No Plant Type set. Entering Setup Mode."));
    isSetupMode = true;
  }

  // 3. Join
  digitalWrite(PIN_LED, HIGH);
  lora.joinOTAA(APP_EUI, APP_KEY);
  digitalWrite(PIN_LED, LOW);
}

void loop() {
  unsigned long currentMillis = millis();
  long interval = isSetupMode ? SETUP_INTERVAL : ACTIVE_INTERVAL;

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    int receivedDownlink = -1;

    digitalWrite(PIN_LED, HIGH);

    if (isSetupMode) {
      // --- SETUP MODE ---
      // Send small packet, wait for downlink
      receivedDownlink = lora.sendSetupMessage();
    } 
    else {
      // --- ACTIVE MODE ---
      // 1. Read Sensors
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      
      // Error check
      if (isnan(h) || isnan(t)) {
        Serial.println(F("DHT Read Failed"));
        digitalWrite(PIN_LED, LOW);
        return;
      }

      // 2. Fake Data (as requested)
      uint8_t soil = 50; 
      uint8_t light = 75; 
      uint8_t batt = 95;

      // 3. Send
      receivedDownlink = lora.sendSensorData((uint8_t)h, (int8_t)t, soil, light, batt);
    }

    digitalWrite(PIN_LED, LOW);

    // --- LOGIC: Did we get a configuration update? ---
    if (receivedDownlink >= 1 && receivedDownlink <= 4) {
      if (receivedDownlink != plantType) {
        Serial.print(F("!!! NEW PLANT TYPE RECEIVED: "));
        Serial.println(receivedDownlink);
        
        plantType = receivedDownlink;
        EEPROM.write(EEPROM_ADDR, plantType);
        
        // Switch to active mode immediately
        isSetupMode = false;
        
        // Reset timer so we don't wait long for the first active reading
        previousMillis = millis() - ACTIVE_INTERVAL; 
      }
    }
  }
}