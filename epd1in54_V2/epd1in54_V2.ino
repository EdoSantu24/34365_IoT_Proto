#include <rn2xx3.h>

#include <SPI.h>
#include "epd1in54_V2.h"
#include "imagedata.h"
#include "epdpaint.h"
#include <stdio.h>
#include <math.h>   // for log()

Epd epd;
unsigned char image[1024];
Paint paint(image, 0, 0);

#define COLORED     0
#define UNCOLORED   1

const int tempPin = A1;   // Analog pin for thermistor
const int voltPin = A3;   // Analog pin for solar/capacitor voltage divider

// Thermistor parameters (TT6-10KC8-9-25)
const float Vcc   = 3.3;        // Supply voltage (for Pro Mini 3.3V)
const float Rref  = 10000.0;    // Fixed resistor 10kΩ
const float R0    = 10000.0;    // Thermistor resistance at 25°C
const float T0    = 298.15;     // 25°C in Kelvin
const float Beta  = 3435.0;     // Beta constant (datasheet)

// Voltage divider resistors
// Example: R1 = 100k, R2 = 100k -> Divider ratio = 2.0
const float dividerRatio = 2.0;

float readTemperature() {
  float raw = analogRead(tempPin);

  // Convert ADC to voltage
  float V = raw * (Vcc / 1023.0);

  // Convert voltage to thermistor resistance
  float Rntc = Rref * V / (Vcc - V);

  // Beta model to convert resistance to temperature
  float tempK = 1.0 / ( (1.0 / T0) + (1.0 / Beta) * log(Rntc / R0) );
  float tempC = tempK - 273.15;

  return tempC;
}

float readVoltage() {
  float raw = analogRead(voltPin);

  // Voltage at analog pin (after divider)
  float Vmeas = raw * (Vcc / 1023.0);

  // Compensate for divider ratio to get actual solar/capacitor voltage
  float Vin = Vmeas * dividerRatio;

  return Vin; // in volts
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("e-Paper init and clear");

  if (epd.LDirInit() != 0) {
    Serial.println("e-Paper init failed");
    return;
  }
  epd.Clear();

  // --- Draw Student IDs (static) ---
  paint.SetWidth(200);
  paint.SetHeight(24);

  paint.Clear(COLORED);
  paint.DrawStringAt(30, 4, "s240075", &Font16, UNCOLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 10, paint.GetWidth(), paint.GetHeight());

  paint.Clear(COLORED);
  paint.DrawStringAt(30, 4, "s240054", &Font16, UNCOLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 35, paint.GetWidth(), paint.GetHeight());

  paint.Clear(COLORED);
  paint.DrawStringAt(30, 4, "s243376", &Font16, UNCOLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 60, paint.GetWidth(), paint.GetHeight());

  epd.DisplayFrame();  // Show IDs once
}

void loop() {
  float temperatureC = readTemperature();
  float panelVoltage = readVoltage();

  // Debug serial
  Serial.print("Temperature: ");
  Serial.print(temperatureC, 1);
  Serial.print(" C   Panel Voltage: ");
  Serial.print(panelVoltage, 2);
  Serial.println(" V");

  // --- Temperature string ---
  char tempBuffer[30];
  dtostrf(temperatureC, 5, 1, tempBuffer);
  strcat(tempBuffer, " C");

  paint.SetWidth(200);
  paint.SetHeight(24);
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(30, 4, tempBuffer, &Font16, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 100, paint.GetWidth(), paint.GetHeight());

  // --- Voltage string ---
  char voltBuffer[30];
  dtostrf(panelVoltage, 5, 2, voltBuffer);  // 2 decimal places
  strcat(voltBuffer, " V");

  paint.Clear(UNCOLORED);
  paint.DrawStringAt(30, 4, voltBuffer, &Font16, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 130, paint.GetWidth(), paint.GetHeight());

  epd.DisplayFrame();

  delay(5000);
}
