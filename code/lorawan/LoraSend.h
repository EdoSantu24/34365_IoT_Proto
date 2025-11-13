/*
 * LoRaTransmitter.h - Library for sending sensor data via RN2483.
 *
 * This library encapsulates the logic for preparing and sending
 * a 5-byte payload. It takes a reference to an existing rn2xx3
 * object to perform the transmission.
 */

#ifndef LORA_TRANSMITTER_H
#define LORA_TRANSMITTER_H

#include <Arduino.h>
#include <rn2xx3.h> // We need this to know what "rn2xx3" type is

class LoRaTransmitter
{
public:
  // Constructor:
  // Takes a reference to the already-initialized LoRa object.
  LoRaTransmitter(rn2xx3 &loraModule);

  // Public function to send the 5-byte sensor payload
  void sendSensorData(uint8_t humidity, int8_t temperature, uint8_t soil, uint8_t light, uint8_t battery);

private:
  rn2xx3 &_lora; // Private reference to the main LoRa object
  const int _ledPin = 13; // The library will control the LED

  // Private helper functions for the LED
  void led_on();
  void led_off();
};

#endif