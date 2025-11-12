// LightSensor.cpp
#include "LightSensor.h"

LightSensor::LightSensor(int pin, int samples) {
  _pin = pin;
  _samples = samples;
}

int LightSensor::readAverage() {
  long sum = 0;
  for (int i = 0; i < _samples; i++) {
    sum += analogRead(_pin);
    delay(2);  // small pause between readings to let ADC settle
  }
  int reading = sum / _samples;  // average
  return reading;                 // 0..1023 (Uno/Nano) or 0..4095 on some boards
}
