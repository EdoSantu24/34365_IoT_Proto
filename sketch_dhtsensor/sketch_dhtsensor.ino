#include <Arduino.h>
#include "DHTSensor.h"

void setup() {
  Serial.begin(9600);
  Serial.println("DHT11 Sensor Test");
  DHTSensor_Init();
}

void loop() {
  delay(2000);

  DHTData data = DHTSensor_Read();

  if (data.valid) {
    Serial.print("Temperatur: ");
    Serial.print(data.temperature);
    Serial.print(" °C | Luftfeuchtigkeit: ");
    Serial.print(data.humidity);
    Serial.println(" %");
  }
}
