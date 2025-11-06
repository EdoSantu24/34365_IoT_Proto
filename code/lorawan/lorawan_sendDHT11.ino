/*
 * Based on code by JP Meijers, 2016
 * Modified for DHT11 Sensor Integration
 *
 * This program sends temperature and humidity data from a DHT11
 * sensor via LoRaWAN (TTN).
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
 * Vcc     -- 3.3V (Note: RN2483A is 3.3V logic)
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
  myLora.tx("Smart Plant Monitor Start");

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

// the loop routine runs over and over again forever:
void loop()
{
  led_on();

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds old (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    led_off();
    delay(5000); // Wait 5 seconds before retrying
    return; // Exit loop and try again
  }

  // --- Payload Preparation ---
  // We will send 2 bytes:
  // Byte 0: Humidity (as a rounded integer, 0-100)
  // Byte 1: Temperature (as a rounded integer, can be negative)
  
  byte payload[2];
  
  // Use (uint8_t) for humidity, as it's always positive (0-100)
  payload[0] = (uint8_t)h; 
  // Use (int8_t) for temperature, as it can be negative (-128 to 127)
  payload[1] = (int8_t)t; 

  Serial.print(F("Sending packet: Hum: "));
  Serial.print(payload[0]);
  Serial.print(F(" %, Temp: "));
  Serial.print(payload[1]);
  Serial.println(F(" C"));

  // Send the binary payload.
  // Using txBytes is better than tx(String) for binary data.
  // This is a blocking function.
  myLora.txBytes(payload, sizeof(payload)); 

  led_off();
  
  // IMPORTANT: Wait at least 60 seconds before sending again
  // to respect TTN Fair Use Policy.
  // The DHT11 sensor also needs >2 seconds between readings.
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