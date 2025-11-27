// LightSensor.h
#ifndef LIGHTSENSOR_H
#define LIGHTSENSOR_H

#include <Arduino.h>

class LightSensor {
  public:
    LightSensor(int pin, int samples = 10);  // constructor

    int readAverage();                       // returns average light reading 0–1023
    void setThresholdLevel(int level);       // set threshold by level (1,2,3,...)
    bool isAboveThreshold();                 // check if light > threshold

  private:
    int _pin;        // analog pin
    int _samples;    // number of readings to average
    int _threshold;  // current threshold
};

#endif
