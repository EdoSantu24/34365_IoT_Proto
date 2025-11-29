#ifndef SOILMOISTURESENSOR_H
#define SOILMOISTURESENSOR_H

#include <Arduino.h>

class SoilMoistureSensor {
  public:
    SoilMoistureSensor(int pin, int samples = 10); // Constructor

    int readAverage();                             // Returns average moisture value
    void setThresholdLevel(int level);             // Sets threshold by level (1,2,3)
    bool isAboveThreshold();                       // Returns true if above threshold

  private:
    int _pin;
    int _samples;
    int _threshold;
};

#endif
