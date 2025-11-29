// LightSensor.cpp
#include "LightSensor.h"

LightSensor::LightSensor(int pin, int samples) {
  _pin = pin;
  _samples = samples;
  _threshold = 500;  // default threshold
}

int LightSensor::readAverage() {
  long sum = 0;
  for (int i = 0; i < _samples; i++) {
    sum += analogRead(_pin);
    delay(2);  // small pause between readings
  }
  return sum / _samples;
}

void LightSensor::setThresholdLevel(int level) {
  // You can define your custom mapping here
  Serial.println("Threshold Update");
  switch (level) {
    case 1:
      _threshold = 500;
      break;
    case 2:
      _threshold = 800;
      break;
    default:
      _threshold = 500; // fallback / default
      break;
  }
}

bool LightSensor::isAboveThreshold() {
  int lightValue = readAverage();
  return lightValue > _threshold;
}
