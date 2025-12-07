#include "SoilMoistureSensor.h"

SoilMoistureSensor::SoilMoistureSensor(int pin, int samples) {
  _pin = pin;
  _samples = samples;
  _threshold = 600;  // Default threshold
}

int SoilMoistureSensor::readAverage() {
  long sum = 0;
  for (int i = 0; i < _samples; i++) {
    sum += analogRead(_pin);
    delay(2);
  }
  return sum / _samples;
}

void SoilMoistureSensor::setThresholdLevel(int level) {
  switch (level) {
    case 1:
      _threshold = 300;  // quite wet
      break;
    case 2:
      _threshold = 500;  // quite dry
      break;
    default:
      _threshold = 300;  // fallback
      break;
  }
}

bool SoilMoistureSensor::isAboveThreshold() {
  int moistureValue = readAverage();
  return moistureValue > _threshold;
}
