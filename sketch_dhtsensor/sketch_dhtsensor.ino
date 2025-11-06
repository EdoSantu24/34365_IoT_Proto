#include <DHT.h>

// ----------------------
// Konfiguration
// ----------------------
#define DHTPIN 9          // Pin, an dem der DHT11 angeschlossen ist
#define DHTTYPE DHT11     // Sensortyp: DHT11, DHT22 oder DHT21

// DHT-Objekt erstellen
DHT dht(DHTPIN, DHTTYPE);

// ----------------------
// Setup
// ----------------------
void setup() {
  Serial.begin(9600);
  Serial.println("DHT11 Sensor Test");
  dht.begin();  // Sensor starten
}

// ----------------------
// Loop
// ----------------------
void loop() {
  // Ein bisschen warten, DHT11 braucht Zeit
  delay(2000);

  // Messwerte auslesen
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(); // Celsius

  // Prüfen, ob die Werte gültig sind
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Fehler beim Auslesen des DHT11-Sensors!");
    return;
  }

  // Ausgabe
  Serial.print("Temperatur: ");
  Serial.print(temperature);
  Serial.print(" °C | Luftfeuchtigkeit: ");
  Serial.print(humidity);
  Serial.println(" %");
}