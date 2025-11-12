#include "DHTSensor.h"
#include <Arduino.h>

// Create DHT object
DHT dht(DHTPIN, DHTTYPE);

// Initialize the DHT sensor
void DHTSensor_Init() {
  dht.begin();
  Serial.println("DHT11 Sensor initialized.");
}

// Read temperature and humidity values
DHTData DHTSensor_Read() {
  DHTData data;

  // Read values from sensor
  data.humidity = dht.readHumidity();
  data.temperature = dht.readTemperature(); // Celsius

  // Check if the readings are valid
  if (isnan(data.humidity) || isnan(data.temperature)) {
    Serial.println("Mistake during reading sensor!");
    data.valid = false;
  } else {
    data.valid = true;
  }

  return data;
}
