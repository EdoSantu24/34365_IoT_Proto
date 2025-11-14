#include <Arduino.h>
#include "LightSensor.h"
#include "SoilMoistureSensor.h"

// Initialize sensors
LightSensor lightSensor(A0, 10);
SoilMoistureSensor soilSensor(A1, 10);

void setup() {
  Serial.begin(9600);
  Serial.println("Sensor Test gestartet...");

  // Set initial thresholds
  lightSensor.setThresholdLevel(2);  // 500
  soilSensor.setThresholdLevel(2);   // 600
}

void loop() {
  int lightValue = lightSensor.readAverage();
  int soilValue = soilSensor.readAverage();

  Serial.print("Lichtwert: ");
  Serial.print(lightValue);
  Serial.print(" | Feuchtigkeit: ");
  Serial.print(soilValue);

  // Check light
  if (lightSensor.isAboveThreshold()) {
    Serial.print(" -> Hell genug");
  } else {
    Serial.print(" -> Zu dunkel");
  }

  // Check soil moisture
  if (soilSensor.isAboveThreshold()) {
    Serial.println(" | Boden zu trocken!");
  } else {
    Serial.println(" | Boden feucht genug!");
  }

  delay(2000);

  // Example: change thresholds dynamically if you like
  // lightSensor.setThresholdLevel(3);
  // soilSensor.setThresholdLevel(1);
}
