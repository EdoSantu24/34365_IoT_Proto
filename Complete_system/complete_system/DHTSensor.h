#ifndef DHTSENSOR_H
#define DHTSENSOR_H

#include <DHT.h>

// Sensor configuration
#define DHTPIN 9
#define DHTTYPE DHT11

// Structure to hold temperature and humidity data
struct DHTData {
  float temperature;
  float humidity;
  bool valid;
};

// Initializes the DHT sensor
void DHTSensor_Init();

// Reads the sensor values and returns a DHTData struct
DHTData DHTSensor_Read();

void DHTSensor_setTempThresholdLevel(int level);
void DHTSensor_setHumidityThresholdLevel(int level);

bool DHTSensor_isTempAboveThreshold();
bool DHTSensor_isHumidityAboveThreshold();

#endif
