# 34365_IoT_Proto
IoT Prototyping project for DTU course 34365
# FLoRa

FLoRa is an IoT plant monitoring device that measures key environmental parameters such as temperature, humidity, soil moisture, and light intensity. It sends this data via LoRa to a companion app, allowing users to monitor their plants remotely and see what care they need. The device also features an e-paper display to show real-time sensor readings locally.

---

## Features

- Measures:
  - Temperature (°C)
  - Humidity (%)
  - Soil Moisture
  - Light Intensity
  - Battery level (%)
- Sends data wirelessly via LoRa to a companion app.
- E-paper display shows live readings.
- Automatic threshold adjustment based on plant type.
- Low-power operation using sleep mode and watchdog timer.
- Supports OTAA LoRa network join.
- Setup and active modes for device configuration and normal operation.

---

## Hardware Requirements

- Arduino-compatible board
- DHT sensor (temperature & humidity)
- Soil Moisture Sensor
- Light Sensor
- e-Paper Display (1.54" V2)
- RN2xx3 LoRa module
- Battery with voltage monitoring
- LED indicator

### Pin Configuration

| Component | Pin |
|-----------|-----|
| DHT Sensor | D5 |
| Light Sensor | A0 |
| Soil Sensor | A1 |
| Battery Measurement | A2 |
| LoRa RX | D4 |
| LoRa TX | D3 |
| LoRa Reset | D12 |
| LED | D2 |
| e-Paper Display RST | D8 |
| e-Paper Display DC | D9 |
| e-Paper Display CS | D10 |
| e-Paper Display BUSY | D7 |
| e-Paper Display PWR | D6 |

---

## Libraries Used

- [Arduino.h](https://www.arduino.cc/reference/en/)
- [avr/sleep.h](https://www.nongnu.org/avr-libc/user-manual/group__avr__power.html)
- [avr/wdt.h](https://www.nongnu.org/avr-libc/user-manual/group__avr__watchdog.html)
- [avr/interrupt.h](https://www.nongnu.org/avr-libc/user-manual/group__avr__interrupts.html)
- [EEPROM.h](https://www.arduino.cc/reference/en/libraries/eeprom/)
- [Adafruit_Sensor.h](https://github.com/adafruit/Adafruit_Sensor)
- [rn2xx3](https://github.com/jgromes/RN2483-Arduino)
- [SPI.h](https://www.arduino.cc/en/reference/SPI)
- Custom headers: `DHTSensor.h`, `LightSensor.h`, `SoilMoistureSensor.h`, `LoRaManager.h`
- e-Paper libraries: `epd1in54_V2.h`, `epdpaint.h`, `epdif.h`, `fonts.h`

---

## Installation

1. Clone the repository or download the ZIP.
2. Open the Arduino IDE and install the required libraries via Library Manager if not already installed.
3. Connect your sensors and e-paper display according to the pin configuration above.
4. Replace the TTN keys in `FLoRa.ino` with your own:
   ```cpp
   const char *APP_EUI = "YOUR_APP_EUI"; 
   const char *APP_KEY = "YOUR_APP_KEY";
5. Upload the FLoRa.ino sketch to your Arduino board.

## Usage

### Modes

**Setup Mode**

- Entered automatically if no plant type is saved in EEPROM.
- Sends a setup packet via LoRa to the app to select the plant type.
- Configures sensor thresholds based on plant type.
- LED indicates setup process.

**Active Mode**

- Default mode after setup.
- Periodically measures all sensors and battery level.
- Displays sensor readings on the e-paper screen.
- Sends sensor data via LoRa to the app.
- Device sleeps between measurements to conserve battery.

### Running the Device

1. Power on the device.
2. Observe the e-paper display showing:
   - Temperature
   - Humidity
   - Soil moisture
   - Light level
   - Battery percentage
3. Monitor the serial output for detailed sensor logs and LoRa status.
4. The device automatically wakes up using the watchdog timer, takes readings, updates display, sends data, then goes back to sleep.

### Sensor Details

- **DHT Sensor** – Reads temperature and humidity; checks thresholds depending on plant type.
- **Light Sensor** – Measures light intensity, averaged over multiple samples, with configurable threshold.
- **Soil Moisture Sensor** – Reads soil moisture analog value, checks against threshold.
- **Battery Monitoring** – Converts analog voltage reading into percentage.  

| Voltage (V) | Battery (%) |
|------------|-------------|
| ≥ 4.20     | 100         |
| ≥ 4.00     | 90          |
| ≥ 3.85     | 75          |
| ≥ 3.70     | 50          |
| ≥ 3.55     | 25          |
| ≥ 3.40     | 10          |
| < 3.40     | 0           |

### LoRa Communication

- Device joins the network using OTAA.
- Sends:
  - **Setup Packet** – Initial plant type configuration.
  - **Sensor Packet** – Temperature, humidity, soil moisture, light, battery.
- Receives downlink packets for configuration changes (plant type updates).

### Code Structure

- **FLoRa.ino** – Main sketch for setup, loop, sensor reading, LoRa communication, e-paper update, and sleep management.
- **DHTSensor.h / .cpp** – Wrapper for DHT sensor with threshold handling.
- **LightSensor.h / .cpp** – Handles light sensor readings and threshold checks.
- **SoilMoistureSensor.h / .cpp** – Handles soil moisture readings and threshold checks.
- **LoRaManager.h / .cpp** – Wrapper around RN2483 LoRa module for sending/receiving data.
- **e-Paper libraries** – Manage e-paper display initialization, drawing, and frame updates.

## Authors

- Edoardo Santucci - s243100  
- Núria Domènech Pla - s243315  
- Gea Staropoli - s243967  
- Felix Hell - s243376  
- Dodzi Kwasi Nuhoho - s240054  
- Eduardo Frances Torres - s240075
