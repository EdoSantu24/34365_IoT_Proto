/*
 * Based on code by JP Meijers, 2016
 * Modified for modular sensor reading and sending.
 *
 * This program demonstrates:
 * - Reading sensor data in the main loop()
 * - Passing data as parameters to a dedicated send function
 * - Now sends a 5-byte payload
 *
 * Hardware:
 * - Arduino Pro-Mini
 * - RN2483A LoRa Module
 * - DHT11 Sensor
 *
 * Connections:
 * RN2xx3 -- Arduino
 * Uart TX -- 10 (SoftwareSerial RX)
 * Uart RX -- 11 (SoftwareSerial TX)
 * Reset   -- 12
 * Vcc     -- 3.3V
 * Gnd     -- Gnd
 *
 * DHT11 -- Arduino
 * VCC     -- 5V
 * GND     -- GND
 * DATA    -- 7
 *
 */
#include <rn2xx3.h>
#include <SoftwareSerial.h>

// Include DHT library
#include <Adafruit_Sensor.h>
#include <DHT.h>

// --- Sensor Definitions ---
#define DHTPIN 7       // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11  // We are using the DHT11
DHT dht(DHTPIN, DHTTYPE);

// --- LoRa Definitions ---
SoftwareSerial mySerial(10, 11); // RX, TX
rn2xx3 myLora(mySerial);

// the setup routine runs once when you press reset:
void setup()
{
  //output LED pin
  pinMode(13, OUTPUT);
  led_on();

  // Open serial communications and wait for port to open:
  Serial.begin(57600); //serial port to computer
  mySerial.begin(9600); //serial port to radio
  Serial.println("Startup");

  // Initialize DHT sensor
  dht.begin();
  
  initialize_radio();

  //transmit a startup message
  myLora.tx("Smart Plant Monitor Start (Refactored 5-byte)");

  led_off();
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

// New function to handle ONLY sending data.
// It receives the processed sensor data as parameters.
void sendSensorData(uint8_t humidity, int8_t temperature, uint8_t soil, uint8_t light, uint8_t battery)
{
  led_on();

  // --- Payload Preparation ---
  // This function just packages and sends the data it was given.
  // Payload is now 5 bytes.
  byte payload[5]; 
  
  payload[0] = humidity; 
  payload[1] = temperature; 
  payload[2] = soil;
  payload[3] = light;
  payload[4] = battery; // Added battery

  Serial.print(F("Sending packet: Hum: "));
  Serial.print(payload[0]);
  Serial.print(F(" %, Temp: "));
  Serial.print(payload[1]);
  Serial.print(F(" C, Soil: "));
  Serial.print(payload[2]);
  Serial.print(F(" %, Light: "));
  Serial.print(payload[3]);
  Serial.print(F(" %, Battery: ")); // Added battery log
  Serial.print(payload[4]);        // Added battery log
  Serial.println(F(" %"));

  // Send the binary payload.
  myLora.txBytes(payload, sizeof(payload)); 

  led_off();
}

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
  
  // 4. Call the send function with the prepared data
  sendSensorData(humidity_int, temperature_int, fakeSoil, fakeLight, fakeBattery);
  
  // 5. Wait for the next cycle
  delay(60000); 
}

void led_on()
{
  digitalWrite(13, 1);
}

void led_off()
{
  digitalWrite(13, 0);
}