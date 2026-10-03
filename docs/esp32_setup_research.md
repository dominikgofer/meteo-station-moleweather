# Portable Weather Station - Hardware Architecture (ESP32 Alternative)

## 1. Project Goal

The hardware must:

- measure basic weather data
- measure air quality
- work without permanent power supply
- be portable
- fit inside a 3D-printed enclosure
- expose data through an API endpoint for a web application
- stay below 1000 PLN (the cheaper the better)
- use parts that can be bought quickly in Poland

---

# 2. Chosen Hardware Baseline

| Subsystem | Chosen Hardware |
|---|---|
| Main controller | ESP32 DevKit (ESP32-WROOM-32 or ESP32-S3) |
| Temperature / humidity / pressure | BME280 |
| Light level | BH1750 |
| Air quality | PMS5003 or PMS7003 |
| External temperature | DS18B20 waterproof probe |
| Real-time clock | DS3231 |
| Storage | MicroSD card SPI module |
| Power source | 18650 Li-Ion battery + UPS/Charger shield |
| Connectivity | Wi-Fi / phone hotspot |
| Enclosure | 3D-printed case based on a real weather station design |

---

# 3. Main Controller

## ESP32 DevKit

### Purpose

The ESP32 is the main microcontroller of the weather station.

It will:

- read data from sensors
- store data locally on the MicroSD card
- **host a lightweight embedded web server to expose a REST API endpoint**
- connect to Wi-Fi
- enter deep sleep between measurements to save massive amounts of power

### Links

- Description: [Espressif ESP32](https://www.espressif.com/en/products/socs/esp32)
- Botland: [Search ESP32 DevKit](https://botland.com.pl/szukaj?s=esp32)

### API Endpoint Solution

Unlike the Raspberry Pi which runs a full OS and Python backend, the ESP32 will run C++ (Arduino framework) or MicroPython firmware. It will use a lightweight embedded HTTP server library (like `ESPAsyncWebServer`) to **expose a REST API endpoint directly from the microcontroller**. 

The web application will query the ESP32's IP address (e.g., `GET /api/latest` or `GET /api/history`) to fetch JSON data. This perfectly satisfies the requirement of having an API endpoint while keeping the hardware extremely lightweight and power-efficient.

### Pros

| Advantage | Explanation |
|---|---|
| Ultra-low power deep sleep | Can run for weeks on a single battery |
| Very low cost | Much cheaper than a Raspberry Pi |
| Built-in Wi-Fi | No extra network module needed |
| Can host API directly | Embedded web server handles REST endpoints |
| Rich GPIO / Interfaces | Supports I2C, UART, SPI, and 1-Wire |

### Cons

| Disadvantage | Mitigation |
|---|---|
| No full Linux OS | Use embedded C++/Arduino or MicroPython |
| Less RAM for heavy databases | Stream historical data directly from the SD card via the API |
| Harder to debug than Linux | Use serial logging and robust error handling |

### Why not other controllers?

| Alternative | Why not |
|---|---|
| Raspberry Pi Zero 2 W | Consumes too much power for long-term battery operation without heavy duty-cycling |
| Arduino / AVR | No built-in Wi-Fi, cannot easily host a modern API endpoint |
| ESP8266 | Fewer GPIO pins, no deep sleep capabilities as robust as ESP32, single-core |
| Raspberry Pi Pico W | Weaker ecosystem for hosting async web servers compared to ESP32 |

### Approximate cost

| Item | Cost PLN |
|---|---:|
| ESP32 DevKit | 25–45 |

---

# 4. Storage

## MicroSD Card SPI Module

### Purpose

Stores the collected measurement history locally. Because the ESP32 does not have a full database engine like SQLite, data is appended to a CSV or JSON file on the SD card. The API endpoint reads this file when historical data is requested.

### Recommended choice

- Standard MicroSD SPI breakout board
- Formatted as FAT32
- 8 GB to 32 GB capacity (ESP32 SD libraries sometimes struggle with >32GB SDHC/SDXC)

### Links

- Botland: [Search MicroSD SPI module](https://botland.com.pl/szukaj?s=modul%20micro%20sd%20spi)

### Pros

| Advantage | Explanation |
|---|---|
| Massive storage for logs | Cheap way to store months of data |
| SPI interface | Easy to wire to ESP32 |
| Portable data | SD card can be removed and read on a PC |

### Cons

| Disadvantage | Mitigation |
|---|---|
| SPI shares bus with other devices | Ensure proper CS (Chip Select) pin management |
| File system corruption on power loss | Write data in batches and close files properly before sleep |

### Why not other storage?

| Alternative | Why not |
|---|---|
| Internal ESP32 Flash (SPIFFS/LittleFS) | Limited write cycles, smaller capacity, harder to extract data physically |
| USB Flash Drive | ESP32 cannot natively act as a USB host for mass storage easily |
| Cloud-only storage | Fails if Wi-Fi is unavailable; local storage is required for reliability |

### Approximate cost

| Item | Cost PLN |
|---|---:|
| MicroSD SPI module + 16GB card | 30–50 |

---

# 5. Power System

## Pre-assembled 18650 Powerbank Module (Plug-and-Play)

### Purpose

Provides portable power without requiring any custom soldering or wiring of power components. 

Standard commercial power banks (like Xiaomi or Samsung) have a "smart" feature that turns them off if the device draws less than 50mA. This completely breaks the ESP32's deep sleep mode. To solve this without building a custom power circuit, we will use a pre-assembled "dumb" powerbank module. It provides standard USB output, has no auto-shutoff, and works instantly.

### Recommended Hardware Choice

1. **The Module:** A pre-assembled 18650 Powerbank PCB (often sold as "Moduł powerbank 18650" or "Uchwyt 18650 z przetwornicą"). It has built-in charging (Micro-USB/Type-C) and a standard USB-A output port.
2. **The Battery:** 1x or 2x 18650 Li-Ion cells (depending on the module chosen).
3. **The Connection:** A standard USB-A to Micro-USB/Type-C cable to plug the module directly into the ESP32.

### Links (Botland Search Queries)

- Powerbank Module: [Search Moduł powerbank 18650](https://botland.com.pl/szukaj?s=modu%C5%82%20powerbank%2018650)
- Battery: [Search 18650 battery](https://botland.com.pl/szukaj?s=akumulator%2018650)

### Pros

| Advantage | Explanation |
|---|---|
| **Zero soldering** | Fully assembled at the factory; just insert batteries and plug in |
| **No auto-shutoff** | Lacks the smart circuitry that kills ESP32 deep sleep projects |
| **Standard cables** | Uses standard USB cables to connect to the ESP32 |
| **Plug-and-play** | Takes less than 2 minutes to set up |
| **Long runtime** | Easily lasts weeks with ESP32 deep sleep |

### Cons

| Disadvantage | Mitigation |
|---|---|
| Takes up physical space | Plan for the module size in the 3D-printed electronics chamber |
| Exposed PCB | Mount it securely inside the 3D-printed case using standoffs to prevent short circuits |

### Why not other power options?

| Alternative | Why not |
|---|---|
| Standard Commercial Power Bank (Xiaomi/Samsung) | Auto-shutoff feature breaks ESP32 deep sleep |
| Custom TP4056 + Boost Converter | Requires manual soldering and wiring (too complex for this phase) |
| Dedicated ESP32 UPS Shields | Frequently out of stock in Polish retail stores |
| Mains power | Not portable |

### How to use it

1. Buy the **Moduł powerbank 18650** from Botland (choose a 1-cell or 2-cell version).
2. Buy a high-quality **18650 Li-Ion battery** (e.g., Samsung, Panasonic, or Sony).
3. Insert the battery into the module's spring contacts (ensure correct polarity).
4. Plug a standard USB cable from the module's USB-A port into the ESP32.
5. To recharge, plug a phone charger into the module's Micro-USB/Type-C input port.

### Approximate cost

| Item | Cost PLN |
|---|---:|
| Pre-assembled 18650 Powerbank Module | 20–40 |
| 18650 Battery (good brand, 3000mAh+) | 30–50 |
| **Total Power System** | **~50–90 PLN** |

# 6. Power Cable

## Micro-USB or USB-C Cable

### Purpose

Used to charge the 18650 battery via the UPS shield and to program the ESP32 via serial.

### Pros

| Advantage | Explanation |
|---|---|
| Standard | Everyone has these cables |
| Dual purpose | Charges battery and flashes firmware |

### Cons

| Disadvantage | Mitigation |
|---|---|
| Cable management in case | Use a panel-mount USB extension cable or route carefully |

### Approximate cost

| Item | Cost PLN |
|---|---:|
| Cable | 15–30 |

---

# 7. Temperature, Humidity, Pressure Sensor

## BME280

### Purpose

Main environmental sensor.

Measures:

- temperature
- relative humidity
- atmospheric pressure

### Links

- Description: [Bosch BME280](https://www.bosch-sensortec.com/products/environmental-sensors/humidity-sensors-bme280/)
- Botland: [Search BME280](https://botland.com.pl/szukaj?s=bme280)

### Interface

- I2C

### Pros

| Advantage | Explanation |
|---|---|
| Three measurements in one module | Temperature, humidity, pressure |
| Digital output | No analog calibration needed |
| Extremely low power | Perfect for ESP32 battery operation |

### Cons

| Disadvantage | Mitigation |
|---|---|
| Sensitive to heat | Place in separate sensor chamber |
| Needs airflow | Use louvered weather-station-style case |

### Why not other sensors?

| Alternative | Why not |
|---|---|
| DHT11 / DHT22 | Inaccurate, no pressure, blocking code |
| BMP280 | No humidity |

### Placement in case

- Inside shaded, ventilated sensor chamber
- Away from ESP32 and voltage regulator heat

### Approximate cost

| Item | Cost PLN |
|---|---:|
| BME280 module | 30–60 |

---

# 8. Light Sensor

## BH1750

### Purpose

Measures ambient light level in lux.

### Links

- Botland: [Search BH1750](https://botland.com.pl/szukaj?s=bh1750)

### Interface

- I2C

### Pros

| Advantage | Explanation |
|---|---|
| Measures lux directly | No complex math required |
| Low power | Can be powered down between readings |

### Cons

| Disadvantage | Mitigation |
|---|---|
| Needs sky exposure | Mount under a clear acrylic dome on the roof |

### Why not other light sensors?

| Alternative | Why not |
|---|---|
| Photoresistor | Requires ADC, calibration, and draws current continuously |

### Approximate cost

| Item | Cost PLN |
|---|---:|
| BH1750 module | 15–30 |

---

# 9. Real-Time Clock

## DS3231

### Purpose

Keeps accurate time while the ESP32 is in deep sleep. The ESP32 has an internal RTC, but it drifts and resets on power loss. The DS3231 ensures timestamps are perfectly accurate when the station wakes up offline.

### Links

- Botland: [Search DS3231](https://botland.com.pl/szukaj?s=ds3231)

### Interface

- I2C

### Pros

| Advantage | Explanation |
|---|---|
| Highly accurate | Essential for time-series weather data |
| Can wake ESP32 | DS3231 alarm pin can trigger ESP32 wake-up from deep sleep |

### Cons

| Disadvantage | Mitigation |
|---|---|
| Needs coin cell | Buy module with battery installed |

### Why not other time options?

| Alternative | Why not |
|---|---|
| ESP32 Internal RTC only | Drifts over time, loses time on total power loss |
| NTP over Wi-Fi only | Fails if Wi-Fi is unavailable; wastes power to connect just for time |

### Approximate cost

| Item | Cost PLN |
|---|---:|
| DS3231 module | 15–30 |

---

# 10. Air Quality Sensor

## PMS5003 or PMS7003

### Purpose

Measures particulate matter (PM1.0, PM2.5, PM10).

### Links

- Botland: [Search PMS5003](https://botland.com.pl/szukaj?s=pms5003)

### Interface

- UART / serial

### Pros

| Advantage | Explanation |
|---|---|
| High value data | PM2.5 is a critical environmental metric |
| Hardware sleep pin | ESP32 can use a GPIO to turn the PMS fan completely off to save power |

### Cons

| Disadvantage | Mitigation |
|---|---|
| High power when active | **Must** be duty-cycled (e.g., run for 30s, sleep for 5m) |
| Needs airflow | Design intake and exhaust paths in case |

### Why not other air quality sensors?

| Alternative | Why not |
|---|---|
| MQ gas sensors | Require continuous heating (massive power drain), poor accuracy |
| BME688 | VOC data is less standardized than PM2.5 |

### Mechanical requirement

- Shaded intake air
- Exhaust path outside the case
- ESP32 GPIO connected to the PMS "SET" pin to cut power to the fan during sleep |

### Approximate cost

| Item | Cost PLN |
|---|---:|
| PMS5003 / PMS7003 | 130–200 |

---

# 11. External Temperature Sensor

## DS18B20 Waterproof Probe

### Purpose

Provides an external temperature reference outside the main enclosure to validate the thermal isolation of the 3D-printed case.

### Links

- Botland: [Search DS18B20 waterproof probe](https://botland.com.pl/szukaj?s=ds18b20%20wodoodporna)

### Interface

- 1-Wire

### Pros

| Advantage | Explanation |
|---|---|
| Waterproof | Can hang completely outside the case |
| Parasitic power mode | Can operate with just 2 wires (VCC and Data) if needed |

### Cons

| Disadvantage | Mitigation |
|---|---|
| Requires pull-up resistor | Add 4.7 kΩ resistor on the data line |

### Why not other external temperature options?

| Alternative | Why not |
|---|---|
| Thermistor | Requires ADC calibration, wires act as antennas for noise |
| Second BME280 | Overkill and more expensive for just external temp |

### Approximate cost

| Item | Cost PLN |
|---|---:|
| DS18B20 waterproof probe | 20–40 |

---

# 12. 3D-Printed Case

## Design Concept

The case should follow the design principles of real meteorological stations ("Stevenson cage").

Real weather stations usually use:

- white enclosures
- shaded sensor chambers
- ventilated louvers
- protection from rain
- protection from direct sunlight
- separation between heat-producing electronics and sensitive sensors

Our 3D-printed case should follow this idea.

## Recommended Case Structure

The case should have two main chambers:

1. Electronics chamber  
   - ESP32 + UPS Shield
   - MicroSD module
   - wiring

2. Sensor chamber  
   - BME280
   - BH1750 window
   - PMS5003 airflow path

## Sensor Placement

| Sensor | Placement | Reason |
|---|---|---|
| BME280 | Shaded, ventilated central sensor chamber | Avoid electronics heat and direct sunlight |
| BH1750 | Top of case under clear dome | Needs ambient light exposure |
| PMS5003 | Inside case with intake/exhaust path | Needs airflow but protection from rain |
| DS18B20 | Outside case on cable | Measures true external temperature |
| ESP32 / UPS | Separate electronics chamber | Prevents voltage regulator heat affecting sensors |

## Case Design Rules

| Rule | Reason |
|---|---|
| Use white or light-gray filament | Reflects sunlight |
| Use ASA or PETG | Better outdoor behavior than PLA |
| Use louvers | Allows airflow while blocking sun and rain |
| Separate ESP32 from BME280 | Heat would distort temperature readings |
| Route DS18B20 through cable gland | Waterproof and mechanically stable |
| Provide clear window for BH1750 | Needed for light measurement |
| Make case serviceable | Need access to USB port for charging/flashing |

## Why not other case designs?

| Alternative | Why not |
|---|---|
| Single sealed box | Sensors need airflow; heat would corrupt readings |
| Simple box with holes | Rain and sunlight can enter |
| Metal box | Blocks Wi-Fi and harder to customize |
| No enclosure | Not suitable for portable/outdoor use |

---

# 13. Battery Life Estimate

This is where the ESP32 build vastly outperforms the Raspberry Pi build. By utilizing the ESP32's **Deep Sleep** mode and turning off the PMS5003 fan between readings, the power draw drops to microamps.

## Approximate Power Consumption

| State | Estimated Current Draw |
|---|---:|
| ESP32 Active (reading sensors, Wi-Fi TX) | ~150–250 mA |
| PMS5003 Active (fan spinning) | ~100 mA |
| ESP32 Deep Sleep + DS3231 running | ~0.015 mA (15 µA) |

## Estimated Runtime (Duty-Cycled)

Assume the station wakes up for 10 seconds every 5 minutes to read sensors, log to SD, and briefly broadcast API data.

- Average current draw: **~2 to 4 mA**
- Battery: Standard 3000 mAh 18650 Li-Ion

| Battery Capacity | Estimated Runtime |
|---|---:|
| 2500 mAh 18650 | ~30 to 50 days |
| 3500 mAh 18650 | ~45 to 70 days |

*Note: If the PMS5003 is left running continuously, runtime drops to roughly 24-48 hours. Duty-cycling is mandatory for long-term operation.*

## Battery Recommendations

- Use a high-quality 18650 cell (e.g., Samsung 35E or Panasonic NCR18650B).
- Ensure the UPS shield has a low-quiescent-current voltage booster.
- Use the DS3231 alarm interrupt to wake the ESP32 from deep sleep, rather than using the ESP32's internal timer (which draws more power).

---

# 14. Final Bill of Materials

| # | Item | Required | Approx. Cost PLN |
|---:|---|---:|---:|
| 1 | ESP32 DevKit | Yes | 25–45 |
| 2 | MicroSD SPI module + 16GB card | Yes | 30–50 |
| 3 | 18650 Li-Ion Battery (good brand) | Yes | 30–50 |
| 4 | ESP32 UPS / Charger Shield | Yes | 20–40 |
| 5 | Micro-USB / USB-C cable | Yes | 15–30 |
| 6 | BME280 | Yes | 30–60 |
| 7 | BH1750 | Yes | 15–30 |
| 8 | DS3231 RTC | Yes | 15–30 |
| 9 | PMS5003 / PMS7003 | Yes | 130–200 |
| 10 | DS18B20 waterproof probe | Yes | 20–40 |
| 11 | Wires, resistors, connectors | Yes | 20–40 |
| 12 | 3D-printing material / inserts | Yes | 40–80 |

**Total Estimated Cost: ~390 – 695 PLN** (Significantly cheaper than the Pi build)

---

# 15. Hardware Decisions Summary

| Decision | Choice | Why not alternative |
|---|---|---|
| Main controller | ESP32 DevKit | Pi Zero drains battery in hours; Arduino lacks Wi-Fi/API hosting |
| API Endpoint | Embedded Web Server (ESPAsyncWebServer) | No Linux OS available, but ESP32 can serve REST JSON directly |
| Storage | MicroSD SPI Module | Internal flash has limited write cycles and capacity |
| Environmental sensor | BME280 | DHT22 has no pressure, BMP280 has no humidity |
| Light sensor | BH1750 | Photoresistor would need ADC and continuous power |
| RTC | DS3231 | ESP32 internal RTC drifts and loses time on power loss |
| Air quality | PMS5003/PMS7003 | MQ sensors require continuous heating (destroys battery life) |
| External temperature | DS18B20 probe | Thermistor would need ADC and calibration |
| Power | 18650 Li-Ion + UPS Shield | USB Power banks auto-shutoff during ESP32 deep sleep |
| Internet | Wi-Fi / hotspot | LTE/SIM adds cost, power draw, and complexity |
| Case | 3D-printed louvered Stevenson cage | Simple sealed box would distort sensor readings |
| Case material | ASA or PETG | PLA is worse outdoors |

---

# 16. Final Hardware Baseline

The weather station hardware will be:

> ESP32 DevKit  
> + BME280 temperature/humidity/pressure sensor  
> + BH1750 light sensor  
> + DS3231 real-time clock  
> + PMS5003/PMS7003 air quality sensor  
> + DS18B20 external temperature probe  
> + MicroSD SPI storage module  
> + 18650 Li-Ion battery + UPS shield  
> + 3D-printed dual-chamber Stevenson cage case

The case will follow the same principles as professional weather stations:

- white exterior
- shaded sensors
- ventilated sensor chamber
- rain protection
- separation between heat-producing electronics and measurement sensors
- external temperature probe mounted outside the main body

The software will run on bare-metal/RTOS firmware, utilizing deep sleep for power management, and will host a lightweight REST API endpoint directly on the microcontroller to serve data to the web application.