#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#include "DHTSensor.h"
#include "LightSensor.h"
#include "SoilMoistureSensor.h"

// -------------------------------
// Sensor pin configuration
// -------------------------------
#define DHT_PIN 9
#define LIGHT_PIN A0
#define SOIL_PIN A1

// -------------------------------
// Create sensor objects
// -------------------------------
LightSensor lightSensor(LIGHT_PIN, 10);
SoilMoistureSensor soilSensor(SOIL_PIN, 10);

volatile uint8_t wdt_count = 0;    // counts wakeups

void setup() {
  Serial.begin(9600);

  // Initialize DHT11 sensor
  DHTSensor_Init();

  // Threshold presets
  lightSensor.setThresholdLevel(2);
  soilSensor.setThresholdLevel(2);
  DHTSensor_setTempThresholdLevel(2);
  DHTSensor_setHumidityThresholdLevel(2);

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
  // -------------------------------------
  // 💤 PUT CPU TO SLEEP UNTIL IRQ OCCURS
  // -------------------------------------
  sleep_cpu();

  // -------------------------------------
  // We wake up here
  // -------------------------------------
  if (++wdt_count < 4) {  
    // wake every 8 seconds, measure every 8 sec (change if needed)
    return;
  }
  wdt_count = 0;

  // -------------------------------
  // Read DHT11 Sensor
  // -------------------------------
  DHTData dhtData = DHTSensor_Read();

  Serial.print("Temperature: ");
  Serial.print(dhtData.temperature);
  Serial.print(" °C");

  if (DHTSensor_isTempAboveThreshold()) {
    Serial.println(" -> Too high!");
  } else {
    Serial.println(" -> Good");
  }

  Serial.print("Humidity: ");
  Serial.print(dhtData.humidity);
  Serial.print(" %");

  if (DHTSensor_isHumidityAboveThreshold()) {
    Serial.println(" -> Too high!");
  } else {
    Serial.println(" -> Good");
  }

  // -------------------------------
  // Read Light Sensor
  // -------------------------------
  int lightValue = lightSensor.readAverage();
  Serial.print("Brightness: ");
  Serial.print(lightValue);

  if (lightSensor.isAboveThreshold()) {
    Serial.println(" -> Bright enough");
  } else {
    Serial.println(" -> Too dark");
  }

  // -------------------------------
  // Read Soil Moisture Sensor
  // -------------------------------
  int soilValue = soilSensor.readAverage();
  Serial.print("Soil moisture: ");
  Serial.print(soilValue);

  if (soilSensor.isAboveThreshold()) {
    Serial.println(" -> Soil moist enough!");
  } else {
    Serial.println(" -> Soil too dry!");
  }

  Serial.println("----------------------------------");
  delay(2000);
}

// -------------------------------------
// WATCHDOG INTERRUPT
// -------------------------------------
ISR(WDT_vect) {
  WDTCSR |= (1 << WDIE); // re-enable interrupt
}
