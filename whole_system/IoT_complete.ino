#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#include "DHTSensor.h"
#include "LightSensor.h"
#include "SoilMoistureSensor.h"

#include "LoRaManager.h"
#include <Adafruit_Sensor.h>
#include <EEPROM.h>


// -------------------------------
// Sensor pin configuration
// -------------------------------
#define DHT_PIN 9
#define LIGHT_PIN A0
#define SOIL_PIN A1
#define BATT_PIN A2     // <-- battery measurement pin

const uint8_t PIN_LORA_RX = 10;
const uint8_t PIN_LORA_TX = 11;
const uint8_t PIN_LORA_RST = 12;
const uint8_t PIN_LED = 13;


// --- TTN Keys (Change these!) ---
const char *APP_EUI = "0000000000000000"; 
const char *APP_KEY = "9B3C437C79252CA4FF947A59F6F13C12"; 

// -------------------------------
// Create sensor objects
// -------------------------------
LightSensor lightSensor(LIGHT_PIN, 10);
SoilMoistureSensor soilSensor(SOIL_PIN, 10);
LoRaManager lora(PIN_LORA_RX, PIN_LORA_TX, PIN_LORA_RST);

volatile uint8_t wdt_count = 0;    // counts wakeups

// --- State ---
int plantType = 0;          
bool isSetupMode = true;

// --- Timers ---
unsigned long previousMillis = 0;
const long SETUP_INTERVAL = 20000;  // 20 Seconds (Fast for setup)
const long ACTIVE_INTERVAL = 60000; // 60 Seconds (Normal operation)
const int EEPROM_ADDR = 0;

// -------------------------------------
// Battery percentage helper
// -------------------------------------
int batteryPercent(float voltage) {
    if (voltage >= 4.20) return 100;
    if (voltage >= 4.00) return 90;
    if (voltage >= 3.85) return 75;
    if (voltage >= 3.70) return 50;
    if (voltage >= 3.55) return 25;
    if (voltage >= 3.40) return 10;
    return 0;
}

void setup() {
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(9600);

  // Initialize DHT11 sensor
  DHTSensor_Init();
  lora.begin();

   uint8_t saved = EEPROM.read(EEPROM_ADDR);
  if (saved >= 1 && saved <= 4) {
    plantType = saved;
    // Threshold presets
      lightSensor.setThresholdLevel(plantType);
      soilSensor.setThresholdLevel(plantType);
      DHTSensor_setTempThresholdLevel(plantType);
      DHTSensor_setHumidityThresholdLevel(plantType);
    isSetupMode = false;
    Serial.print(F("Loaded Plant Type from Memory: "));
    Serial.println(plantType);
  } else {
    Serial.println(F("No Plant Type set. Entering Setup Mode."));
    isSetupMode = true;
  }

  digitalWrite(PIN_LED, HIGH);
  lora.joinOTAA(APP_EUI, APP_KEY);
  digitalWrite(PIN_LED, LOW);

  // -------------------------------
  // ENABLE WATCHDOG AND SLEEP MODE
  // -------------------------------
  wdt_enable(WDTO_8S);        // wake every 8 seconds
  WDTCSR |= (1 << WDIE);      // enable WDT interrupt
  sei();                      // enable global interrupts

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  delay(2000);
}

void loop() {

  

  // -------------------------------------------------
  // Convert watchdog tick to millis-like timing
  // (Each WDT wake is ~8s)
  // -------------------------------------------------
  unsigned long currentMillis = millis();
  long interval = isSetupMode ? SETUP_INTERVAL : ACTIVE_INTERVAL;

  if (currentMillis - previousMillis < interval) {
    return;
  }
  previousMillis = currentMillis;

  // -------------------------------------------------
  // Measure battery
  // -------------------------------------------------
  int raw = analogRead(BATT_PIN);
  float pinVoltage = raw * (3.3 / 1023.0);
  float batteryVoltage = pinVoltage * 2.0;
  int battPercent = batteryPercent(batteryVoltage);

  Serial.print("Battery: ");
  Serial.print(battPercent);
  Serial.print("% (");
  Serial.print(batteryVoltage, 2);
  Serial.println(" V)");

  // -------------------------------------------------
  // Read sensors
  // -------------------------------------------------
  DHTData dhtData = DHTSensor_Read();

  float t = dhtData.temperature;
  float h = dhtData.humidity;

  if (isnan(t) || isnan(h)) {
    Serial.println(F("DHT FAILED READ"));
    return;
  }

  float lightValue = lightSensor.readAverage();
  float soilValue = soilSensor.readAverage();

  Serial.print("Temp: "); Serial.print(t); Serial.println(" C");
  if (DHTSensor_isTempAboveThreshold()) {
    Serial.println(" -> Good!");
  } else {
    Serial.println(" -> Too low!");
  }
  Serial.print("Humidity: "); Serial.print(h); Serial.println(" %");
  if (DHTSensor_isHumidityAboveThreshold()) {
    Serial.println(" -> Too high!");
  } else {
    Serial.println(" -> Good");
  }
  Serial.print("Light: "); Serial.println(lightValue);
  if (lightSensor.isAboveThreshold()) {
    Serial.println(" -> Bright enough");
  } else {
    Serial.println(" -> Too dark");
  }
  Serial.print("Soil: "); Serial.println(soilValue);
  if (soilSensor.isAboveThreshold()) {
    Serial.println(" -> Soil moist enough!");
  } else {
    Serial.println(" -> Soil too dry!");
  }


  // -------------------------------------------------
  // LORA SEND (Setup mode OR Active mode)
  // -------------------------------------------------
  digitalWrite(PIN_LED, HIGH);
  int receivedDownlink = -1;

  if (isSetupMode) {
    Serial.println(F("[SETUP MODE] Sending Setup Packet"));
    receivedDownlink = lora.sendSetupMessage();
    // Threshold presets
    lightSensor.setThresholdLevel(receivedDownlink);
    soilSensor.setThresholdLevel(receivedDownlink);
    DHTSensor_setTempThresholdLevel(receivedDownlink);
    DHTSensor_setHumidityThresholdLevel(receivedDownlink);
  }
  else {
    Serial.println(F("[ACTIVE MODE] Sending Sensor Packet"));
    receivedDownlink = lora.sendSensorData(
        (uint8_t)h,
        (int8_t)t,
        (uint8_t)((soilValue/1024)*100),
        (uint8_t)((lightValue/1024)*100),
        (uint8_t)battPercent
    );
  }

  digitalWrite(PIN_LED, LOW);

  // -------------------------------------------------
  // Handle downlink configuration
  // -------------------------------------------------
  if (receivedDownlink >= 1 && receivedDownlink <= 4) {
    if (receivedDownlink != plantType) {
      Serial.print(F("!!! NEW PLANT TYPE RECEIVED: "));
      Serial.println(receivedDownlink);
      // Threshold presets
      lightSensor.setThresholdLevel(receivedDownlink);
      soilSensor.setThresholdLevel(receivedDownlink);
      DHTSensor_setTempThresholdLevel(receivedDownlink);
      DHTSensor_setHumidityThresholdLevel(receivedDownlink);
      plantType = receivedDownlink;
      EEPROM.write(EEPROM_ADDR, plantType);

      // Immediately switch to active mode
      isSetupMode = false;

      // Force next reading right away
      previousMillis = millis() - ACTIVE_INTERVAL;
    }
  }

  Serial.println("----------------------------------");

  delay(3000);

  // 💤 Sleep until watchdog wakes us
  sleep_cpu();

  // Woke up here
  if (++wdt_count < 1) {
    // only run logic every ~8 sec
    return;
  }
  wdt_count = 0;
}

// -------------------------------------
// WATCHDOG INTERRUPT
// -------------------------------------
ISR(WDT_vect) {
  WDTCSR |= (1 << WDIE); // re-enable interrupt
}
