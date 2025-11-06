#include "ElecbeeSoilMoisture.h"

// Pin analógico donde está conectado el OUT del sensor
const uint8_t SOIL_PIN = A0;

// Valores de calibración (EJEMPLO, ajústalos tú):
// 1) AIR_VALUE: lectura con el sensor en el aire (seco).
// 2) WATER_VALUE: lectura con el sensor sumergido en agua.
// Haz analogRead con el código de abajo y apunta los valores reales.
const uint16_t AIR_VALUE   = 600; // <-- cambia con tu medida "en aire"
const uint16_t WATER_VALUE = 300; // <-- cambia con tu medida "en agua"

void setup()
{
  Serial.begin(9600);
  while (!Serial) {
    ; // espera en placas con USB nativo
  }

  soilSensorBegin(SOIL_PIN);

  Serial.println(F("Elecbee Capacitive Soil Moisture Sensor"));
  Serial.println(F("Iniciando mediciones..."));
}

void loop()
{
  // Lectura cruda
  uint16_t raw = soilSensorReadRaw();

  // Lectura en porcentaje usando calibración
  uint8_t percent = soilSensorReadPercent(AIR_VALUE, WATER_VALUE, 10);

  Serial.print(F("Crudo = "));
  Serial.print(raw);
  Serial.print(F("  | Humedad = "));
  Serial.print(percent);
  Serial.println(F(" %"));

  delay(1000);
}
