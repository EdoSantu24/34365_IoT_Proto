#include <rn2xx3.h>

#include <SPI.h>
#include "epd1in54_V2.h"
#include "imagedata.h"
#include "epdpaint.h"
#include <stdio.h>
#include <math.h>   // for log()
#include "DHTSensor.h"
#include "LightSensor.h"
#include "SoilMoistureSensor.h"

Epd epd;
unsigned char image[1024];
Paint paint(image, 0, 0);
int i ;
#define COLORED     0
#define UNCOLORED   1

char soil_state[10];
float temperatureC ;
float light ;
float humidity ;

LightSensor lightSensor(4, 10);
// SoilMoistureSensor soilSensor(A1, 10);


void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("e-Paper init and clear");
  DHTSensor_Init();
  if (epd.LDirInit() != 0) {
    Serial.println("e-Paper init failed");
    return;
  }
  epd.Clear();

  // --- Draw Plant Sensors (static) ---
  paint.SetWidth(200);
  paint.SetHeight(20);

  paint.Clear(COLORED);
  paint.DrawStringAt(30, 4, "Plant Sensors", &Font16, UNCOLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());
  
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(30, 4, "Good", &Font16, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 80, 120, paint.GetWidth(), paint.GetHeight());

  

  paint.Clear(COLORED);
  paint.DrawStringAt(30, 4, "Temp: ", &Font16, UNCOLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 30, paint.GetWidth(), paint.GetHeight());
  
  paint.Clear(COLORED);
  paint.DrawStringAt(30, 4, "Hum: ", &Font16, UNCOLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 90, paint.GetWidth(), paint.GetHeight());
 
  paint.Clear(COLORED);
  paint.DrawStringAt(30, 4, "Light", &Font16, UNCOLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 60, paint.GetWidth(), paint.GetHeight());
  
  paint.SetWidth(60);
  paint.SetHeight(20);
  paint.Clear(COLORED);
  paint.DrawStringAt(30, 4, "SM:", &Font16, UNCOLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 120, paint.GetWidth(), paint.GetHeight());
  epd.DisplayFrame();  
}

void loop() {
  DHTData dht = DHTSensor_Read();
  int lightValue = lightSensor.readAverage();
  // int soilValue = soilSensor.readAverage();
  
  temperatureC = dht.temperature;
  humidity = dht.humidity;
  


  // Debug serial
  Serial.print(" Temperature: ");
  Serial.print(temperatureC);

 
  

  // --- Temperature string ---
  char tempBuffer[30];
  dtostrf(temperatureC, 5, 1, tempBuffer);
  strcat(tempBuffer, " C");

  paint.SetWidth(200);
  paint.SetHeight(24);
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(30, 4, tempBuffer, &Font16, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 80, 30, paint.GetWidth(), paint.GetHeight());
  // Debug serial
  Serial.print(" Light: ");
  Serial.print(lightValue, 1);
  // Serial.print(" C   Panel Voltage: ");
  

  
  char lightBuffer[30];
  dtostrf(lightValue, 5, 1, lightBuffer);
  //strcat(lightBuffer, " C");

  paint.SetWidth(200);
  paint.SetHeight(24);
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(30, 4, lightBuffer, &Font16, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 90, 60, paint.GetWidth(), paint.GetHeight());

    // Debug serial
  Serial.print(" Humidity: ");
  Serial.print(humidity, 1);

  paint.Clear(COLORED);
  paint.DrawStringAt(30, 4, "Hum: ", &Font16, UNCOLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 90, paint.GetWidth(), paint.GetHeight());

  Serial.print( humidity);

  

 
  //  --- Humidity ---
  char humBuffer[30];
  dtostrf(humidity, 5, 1, humBuffer);  // 2 decimal places
   //rcat(humBuffer, " V");
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(30, 4, humBuffer, &Font16, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 80, 90, paint.GetWidth(), paint.GetHeight());

  //     Debug serial
  // Serial.print(" Soil Moisture: ");
  // Serial.print(soilValue, 1);
  
  // char soilBuffer[30];
  //  dtostrf(soilValue, 5, 1, soilBuffer);  // 2 decimal places
  // //  strcat(soilValue);
  //  paint.Clear(UNCOLORED);
  //  paint.DrawStringAt(30, 4, soilBuffer, &Font16, COLORED);
  //  epd.SetFrameMemory(paint.GetImage(), 100, 90, paint.GetWidth(), paint.GetHeight());
   
  // if(humidity > 10.0) {
  //  paint.Clear(UNCOLORED);
  //  paint.DrawStringAt(30, 4, "Dry", &Font16, COLORED);
  //  epd.SetFrameMemory(paint.GetImage(), 70, 120, paint.GetWidth(), paint.GetHeight());
  //  }

  //   if(humidity  <9.99 ){
  //     paint.Clear(UNCOLORED);
  // paint.DrawStringAt(30, 4, "bueno", &Font16, COLORED);
  // epd.SetFrameMemory(paint.GetImage(), 80, 120, paint.GetWidth(), paint.GetHeight());

  //  }
  // else {
  // Serial.println(" -> Soil too dry!");
  // paint.Clear(UNCOLORED);
  // paint.DrawStringAt(30, 4, "b", &Font16, COLORED);
  // epd.SetFrameMemory(paint.GetImage(), 80, 120, paint.GetWidth(), paint.GetHeight());

  // }
   
 

  epd.DisplayFrame();

  delay(10000);
}