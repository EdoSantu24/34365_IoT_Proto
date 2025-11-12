#ifndef SOILMOISTURESENSOR_H
#define SOILMOISTURESENSOR_H

#include <Arduino.h>

class SoilMoistureSensor {
  public:
    SoilMoistureSensor(int pin, int samples = 10); // Constructor: pin + number of averaged samples
    int readAverage();                             // Returns the average moisture reading (0–1023)
  private:
    int _pin;       // analog input pin
    int _samples;   // number of samples for averaging
};

#endif
