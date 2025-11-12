#include <Arduino.h>
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
LightSensor lightSensor(LIGHT_PIN, 10);          // pin A0, average 10 samples
SoilMoistureSensor soilSensor(SOIL_PIN, 10);     // pin A1, average 10 samples

// DHT sensor uses pin + type defined in DHTSensor.h, 
// but you could easily change it to dynamic later
// Example: DHTSensor_Init(DHT_PIN, DHT11);  ← if extended later

void setup() {
  Serial.begin(9600);

  // Initialize DHT11 sensor
  DHTSensor_Init();
}

void loop() {
  // Delay between readings
  delay(2000);

  // -------------------------------
  // Read DHT11 Sensor (Temperature + Humidity)
  // -------------------------------
  DHTData dhtData = DHTSensor_Read();
  if (dhtData.valid) {
    Serial.print("Temperature: ");
    Serial.print(dhtData.temperature);
    Serial.print(" °C | Humidity: ");
    Serial.print(dhtData.humidity);
    Serial.println(" %");
  } else {
    Serial.println("Problem while reading sensor DHT11!");
  }

  // -------------------------------
  // Read Light Sensor
  // -------------------------------
  int lightValue = lightSensor.readAverage();
  Serial.print("Brightness: ");
  Serial.println(lightValue);

  // -------------------------------
  // Read Soil Moisture Sensor
  // -------------------------------
  int soilValue = soilSensor.readAverage();
  Serial.print("Soil moisture: ");
  Serial.println(soilValue);

  Serial.println("----------------------------------");
  delay(3000); // wait before next round
}
