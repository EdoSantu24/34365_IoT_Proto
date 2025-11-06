// Reads a photoresistor on A0 and prints values 0–1023.
// Higher = brighter.

#include "LightSensor.h"
const int sensorPin = A1;
int sensorValue = 0;

LightSensor sensor(A0, 10);  // use analog pin A0, average 10 samples

void setup() {
  Serial.begin(9600);
}

void loop() {
  int reading = sensor.readAverage();  // get a stable light value
  Serial.print("Light_reading: ");
  Serial.println(reading);             // send it to Serial Monitor
  delay(2000);             

  sensorValue = analogRead(sensorPin);
  Serial.print("Feuchtigkeitswert: ");
  Serial.println(sensorValue);
  delay(1000);             
}
