#include "DHTSensor.h"
#include <Arduino.h>

// Create DHT object
DHT dht(DHTPIN, DHTTYPE);

// Internal storage for last reading
static DHTData lastReading;

// Threshold values
static float tempThreshold = 25.0;     // default temperature threshold
static float humidThreshold = 60.0;    // default humidity threshold

// Initialize the DHT sensor
void DHTSensor_Init() {
  dht.begin();
  Serial.println("DHT11 Sensor initialized.");
}

// Read temperature and humidity values
DHTData DHTSensor_Read() {
  DHTData data;

  data.humidity = dht.readHumidity();
  data.temperature = dht.readTemperature(); // Celsius

  if (isnan(data.humidity) || isnan(data.temperature)) {
    Serial.println("Mistake during reading sensor!");
    data.valid = false;
  } else {
    data.valid = true;
    lastReading = data;  // store last valid values
  }

  return data;
}

// -------------------------------------------------
// Set temperature threshold by level
// -------------------------------------------------
void DHTSensor_setTempThresholdLevel(int level) {
  switch (level) {
    case 1: tempThreshold = 20.0; break;
    case 2: tempThreshold = 25.0; break;
    case 3: tempThreshold = 30.0; break;
    default: tempThreshold = 25.0; break;
  }
}

// -------------------------------------------------
// NEW: Set humidity threshold by level
// -------------------------------------------------
void DHTSensor_setHumidityThresholdLevel(int level) {
  switch (level) {
    case 1: humidThreshold = 40.0; break;
    case 2: humidThreshold = 60.0; break;
    case 3: humidThreshold = 80.0; break;
    default: humidThreshold = 60.0; break;
  }
}

// -------------------------------------------------
// Check if temperature is above threshold
// -------------------------------------------------
bool DHTSensor_isTempAboveThreshold() {
  return (lastReading.valid && lastReading.temperature > tempThreshold);
}

// -------------------------------------------------
// Check if humidity is above threshold
// -------------------------------------------------
bool DHTSensor_isHumidityAboveThreshold() {
  return (lastReading.valid && lastReading.humidity > humidThreshold);
}
