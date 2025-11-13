/*
 * Smart Plant Monitor - Main Sketch
 *
 * This main file is now much cleaner. It handles:
 * - Initializing hardware (DHT, LoRa)
 * - Reading sensor data (real and fake)
 * - Calling the LoRaTransmitter library to send the data.
 */
#include <rn2xx3.h>
#include <SoftwareSerial.h>

// Include DHT library
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Include our new custom library
#include "LoRaTransmitter.h"

// --- Sensor Definitions ---
#define DHTPIN 7       // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11  // We are using the DHT11
DHT dht(DHTPIN, DHTTYPE);

// --- LoRa Definitions ---
SoftwareSerial mySerial(10, 11); // RX, TX
rn2xx3 myLora(mySerial); // This is our main LoRa object

// --- Transmitter Definition ---
// Create an instance of our transmitter library,
// passing it the LoRa object it needs to use.
LoRaTransmitter transmitter(myLora);

// the setup routine runs once when you press reset:
void setup()
{
  // We no longer need to manage pin 13 here; the library does it.
  
  // Open serial communications and wait for port to open:
  Serial.begin(57600); //serial port to computer
  mySerial.begin(9600); //serial port to radio
  Serial.println("Startup");

  // Initialize DHT sensor
  dht.begin();
  
  initialize_radio();

  //transmit a startup message
  myLora.tx("Smart Plant Monitor Start (Lib Version)");
}

void initialize_radio()
{
  //reset rn2483
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
  delay(500);
  digitalWrite(12, HIGH);

  delay(100); //wait for the RN2xx3's startup message
  mySerial.flush();

  //Autobaud the rn2483 module to 9600. The default would otherwise be 57600.
  myLora.autobaud();

  //check communication with radio
  String hweui = myLora.hweui();
  while (hweui.length() != 16)
  {
    Serial.println("Communication with RN2xx3 unsuccessful. Power cycle the board.");
    Serial.println(hweui);
    delay(10000);
    hweui = myLora.hweui();
  }

  //print out the HWEUI so that we can register it via ttnctl
  Serial.println("When using OTAA, register this DevEUI: ");
  Serial.println(myLora.hweui());
  Serial.println("RN2xx3 firmware version:");
  Serial.println(myLora.sysver());

  //configure your keys and join the network
  Serial.println("Trying to join TTN");
  bool join_result = false;


  /*
     * ABP: initABP(String addr, String AppSKey, String NwkSKey);
     * Paste the example code from the TTN console here:
     */
  // const char *devAddr = "02017201";
  // const char *nwkSKey = "AE17E567AECC8787F749A62F5541D522";
  // const char *appSKey = "8D7FFEF938589D95AAD928C2E2E7E48F";

  // join_result = myLora.initABP(devAddr, appSKey, nwkSKey);

  /*
     * OTAA: initOTAA(String AppEUI, String AppKey);
     * If you are using OTAA, paste the example code from the TTN console here:
     */
  const char *appEui = "0000000000000000"; // Use your TTN AppEUI
  const char *appKey = "9B3C437C79252CA4FF947A59F6F13C12"; // Use your TTN AppKey
  join_result = myLora.initOTAA(appEui, appKey);


  while (!join_result)
  {
    Serial.println("Unable to join. Are your keys correct, and do you have TTN coverage?");
    delay(60000); //delay a minute before retry
    join_result = myLora.init();
  }
  Serial.println("Successfully joined TTN");

}

// The sendSensorData function is GONE from this file.
// The led_on and led_off functions are GONE from this file.

// the loop routine runs over and over again forever:
void loop()
{
  // 1. Read REAL sensor data (This logic will be in sensor.h)
  float h_float = dht.readHumidity();
  float t_float = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h_float) || isnan(t_float)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    delay(5000); // Wait 5 seconds before retrying
    return; // Skip this loop iteration
  }
  
  // 2. Read FAKE sensor data (This logic will also be in sensor.h)
  // Here, we just create the fake data.
  uint8_t fakeSoil = 50; // Fake soil moisture at 50%
  uint8_t fakeLight = 75; // Fake light level at 75%
  uint8_t fakeBattery = 95; // Fake battery level at 95%

  // 3. Process/Prepare data for sending
  // Convert floats to the one-byte format.
  uint8_t humidity_int = (uint8_t)h_float;
  int8_t temperature_int = (int8_t)t_float;
  
  // 4. Call the send function from our LIBRARY object
  transmitter.sendSensorData(humidity_int, temperature_int, fakeSoil, fakeLight, fakeBattery);
  
  // 5. Wait for the next cycle
  delay(60000); 
}