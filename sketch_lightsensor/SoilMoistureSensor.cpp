#include "SoilMoistureSensor.h"

SoilMoistureSensor::SoilMoistureSensor(int pin, int samples) {
  _pin = pin;
  _samples = samples;
}

int SoilMoistureSensor::readAverage() {
  long sum = 0;
  for (int i = 0; i < _samples; i++) {
    sum += analogRead(_pin);
    delay(2); // short delay for ADC stability
  }
  int reading = sum / _samples;
  return reading;  // 0..1023 
}
