// LightSensor.h
#ifndef LIGHTSENSOR_H
#define LIGHTSENSOR_H

#include <Arduino.h>

class LightSensor {
  public:
    LightSensor(int pin, int samples = 10);  // constructor
    int readAverage();                       // returns average light reading 0–1023
  private:
    int _pin;        // which analog pin we read (e.g., A0)
    int _samples;    // how many readings to average each time
};

#endif
