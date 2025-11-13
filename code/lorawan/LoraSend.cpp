/*
 * LoRaTransmitter.cpp - Implementation for the LoRaTransmitter library.
 */

#include "LoRaTransmitter.h"

// Constructor implementation
// We use an initializer list (": _lora(loraModule)") to set our private reference.
LoRaTransmitter::LoRaTransmitter(rn2xx3 &loraModule) : _lora(loraModule)
{
  // The library takes responsibility for the LED
  pinMode(_ledPin, OUTPUT);
  digitalWrite(_ledPin, LOW); // Start with it off
}

// sendSensorData implementation
// This is the same logic from your old .ino file, but now it's part of the class.
void LoRaTransmitter::sendSensorData(uint8_t humidity, int8_t temperature, uint8_t soil, uint8_t light, uint8_t battery)
{
  led_on();

  // --- Payload Preparation ---
  // Payload is 5 bytes.
  byte payload[5]; 
  
  payload[0] = humidity; 
  payload[1] = temperature; 
  payload[2] = soil;
  payload[3] = light;
  payload[4] = battery;

  // We use Serial for logging, assuming it's initialized in the main .ino
  Serial.print(F("Sending packet (from Lib): Hum: "));
  Serial.print(payload[0]);
  Serial.print(F(" %, Temp: "));
  Serial.print(payload[1]);
  Serial.print(F(" C, Soil: "));
  Serial.print(payload[2]);
  Serial.print(F(" %, Light: "));
  Serial.print(payload[3]);
  Serial.print(F(" %, Battery: "));
  Serial.print(payload[4]);
  Serial.println(F(" %"));

  // Send the binary payload using our private reference _lora
  _lora.txBytes(payload, sizeof(payload)); 

  led_off();
}

// --- Private LED Functions ---

void LoRaTransmitter::led_on()
{
  digitalWrite(_ledPin, 1);
}

void LoRaTransmitter::led_off()
{
  digitalWrite(_ledPin, 0);
}