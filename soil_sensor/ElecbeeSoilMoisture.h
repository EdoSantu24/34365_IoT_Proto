#pragma once

#include <Arduino.h>

// Inicializa el módulo de lectura del sensor.
// Debes llamarla en setup(), indicando el pin analógico donde está conectado el sensor.
void soilSensorBegin(uint8_t analogPin);

// Lee el valor "crudo" del ADC (0-1023 en Arduino UNO, 0-4095 en ESP32, etc.).
uint16_t soilSensorReadRaw();

// Lee la humedad en % haciendo media de varias muestras y usando una calibración básica.
//  - airValue: lectura en aire completamente seco (sensor fuera de la tierra).
//  - waterValue: lectura con el sensor totalmente sumergido en agua.
//  - samples: número de lecturas para promediar (por defecto 10).
uint8_t soilSensorReadPercent(uint16_t airValue, uint16_t waterValue, uint8_t samples = 10);
