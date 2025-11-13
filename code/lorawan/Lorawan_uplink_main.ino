/*
 * Smart Plant Monitor - Main Sketch
 *
 * This main file now has two modes:
 * 1. Setup Mode (in setup()): Pings for a plant type.
 * 2. Operating Mode (in loop()): Sends sensor data.
 */
#include <rn2xx3.h>
#include <SoftwareSerial.h>

// Include DHT library
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Include our new custom library
#include "LoRaTransmitter.h"

// --- Global State ---
// This variable controls our state.
// 0 = Setup mode (needs plant type)
// 1-4 = Operating mode (plant type is set)
int plantType = 0;

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

// the setup routine runs once when you press reset: it stay in a loop until it gets plant type
void setup()
{
  
  // Open serial communications and wait for port to open:
  Serial.begin(57600); //serial port to computer
  mySerial.begin(9600); //serial port to radio
  Serial.println("Startup");

  // Initialize DHT sensor
  dht.begin();
  
  initialize_radio(); // connection to lorawan TTN

  //transmit a startup message
  myLora.tx("Smart Plant Monitor Start (Lib Version)");

  // --- NEW: Setup Mode Loop ---
  // This loop will run until we get a valid plant type
  // from a downlink message.
  Serial.println("Entering Setup Mode. Waiting for Plant Type downlink...");

  while (plantType == 0)
  {
    // 1. Get a fake battery value for our ping
    uint8_t fakeBattery = 99; // Using 99% to indicate "setup"
    
    // 2. Call the setup ping. This function blocks, sends,
    //    and listens for a downlink response.
    String downlink = transmitter.sendSetupPing(fakeBattery);

    // 3. Check if we received a downlink
    if (downlink.length() > 0)
    {
      Serial.print("Downlink received: ");
      Serial.println(downlink);

      // Convert the received HEX string (e.g., "01") to an integer
      // The "16" at the end means "base 16" (hexadecimal).
      long plantId = strtol(downlink.c_str(), NULL, 16);

      if (plantId >= 1 && plantId <= 4)
      {
        // Success! We got a valid plant type.
        plantType = (int)plantId;
        Serial.print("Plant Type set to: ");
        Serial.println(plantType);
        Serial.println("Setup complete. Starting Operating Mode.");
      }
      else
      {
        Serial.println("Invalid downlink. Retrying...");
      }
    }
    else
    {
      // No downlink received. Wait 30 seconds and try again.
      Serial.println("No downlink. Retrying in 30 seconds...");
      delay(30000); // 30 second wait
    }
  }
}

// function for lorawan intialization
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

// the loop routine runs over and over again forever:
// This function will only be called AFTER setup() is complete,
// meaning plantType is already set (1-4).
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
  // User requested 10 minutes (10 * 60 * 1000)
  Serial.println("Data sent. Sleeping for 10 minutes...");
  delay(600000); 
}