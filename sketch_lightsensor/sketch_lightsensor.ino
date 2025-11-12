#include <Arduino.h>
#include "LightSensor.h"
#include "SoilMoistureSensor.h"

// Initialize sensors
LightSensor lightSensor(A0, 10);         // pin A0, average 10 samples
SoilMoistureSensor soilSensor(A1, 10);   // pin A1, average 10 samples

void setup() {
  Serial.begin(9600);
  Serial.println("Sensor Test gestartet...");
}

void loop() {
  int lightValue = lightSensor.readAverage();
  int soilValue = soilSensor.readAverage();

  Serial.print("Lichtwert: ");
  Serial.print(lightValue);
  Serial.print(" | Feuchtigkeitswert: ");
  Serial.println(soilValue);

  delay(2000);
}
