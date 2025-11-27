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
      _threshold = 10;  // quite dry
      break;
    case 2:
      _threshold = 30;  // medium
      break;
    case 3:
      _threshold = 50;  // very moist
      break;
    default:
      _threshold = 30;  // fallback
      break;
  }
}

bool SoilMoistureSensor::isAboveThreshold() {
  int moistureValue = readAverage();
  return moistureValue > _threshold;
}
