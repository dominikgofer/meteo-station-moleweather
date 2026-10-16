# Portable Weather Station - Hardware Architecture

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
| Main controller | Raspberry Pi Zero 2 W |
| Temperature / humidity / pressure | BME280 |
| Light level | BH1750 |
| Air quality | PMS5003 or PMS7003 |
| External temperature | DS18B20 waterproof probe |
| Real-time clock | DS3231 |
| Power source | USB power bank |
| Connectivity | Wi-Fi / phone hotspot |
| Enclosure | 3D-printed case based on a real weather station design |

---

# 3. Main Controller

## Raspberry Pi Zero 2 W

### Purpose

The Raspberry Pi Zero 2 W is the main computer of the weather station.

It will:

- read data from sensors
- store data locally
- expose an API endpoint
- connect to Wi-Fi
- run the station software

### Links

- Description: [Raspberry Pi Zero 2 W official page](https://www.raspberrypi.com/products/raspberry-pi-zero-2-w/)
- Botland: [Search Raspberry Pi Zero 2 W](https://botland.com.pl/moduly-i-zestawy-raspberry-pi-zero/20347-raspberry-pi-zero-2-w-512mb-ram-wifi-bt-42-5056561800004.html)

### Pros

| Advantage | Explanation |
|---|---|
| Full Linux system | Can run a proper API and local storage |
| Small size | Good for a portable station |
| Low cost | Fits easily in the budget |
| Wi-Fi built in | No extra network module needed |
| GPIO support | Can connect I2C, UART, and 1-Wire sensors |
| Easy development | Python-friendly |

### Cons

| Disadvantage | Mitigation |
|---|---|
| Consumes more power than a microcontroller | Use a large power bank and manage sensor duty cycles |
| Generates heat | Separate electronics chamber from sensor chamber |
| Requires microSD card | - |

### Why not other controllers?

| Alternative | Why not |
|---|---|
| Raspberry Pi 4 | More power-hungry and overkill for this project |
| ESP32 | Better battery life, but weaker as a local API/database device |
| Arduino | Not suitable for hosting an API endpoint |
| Raspberry Pi Pico W | Too limited for full API and local data storage |

### Approximate cost

| Item | Cost PLN |
|---|---:|
| Raspberry Pi Zero 2 W | 70-80 |

---

# 4. Storage

## microSD Card

### Purpose

Stores the operating system, configuration, and collected data.

### Recommended choice

- 32 GB or 64 GB
- Class 10 / U1
- A1 or A2 preferred
- Known brand: SanDisk, Samsung, Kingston

### Links

- Description: [Secure Digital overview](https://en.wikipedia.org/wiki/Secure_Digital)
- Botland: [Search microSD card](https://botland.com.pl/szukaj?s=micro%20sd)

### Pros

| Advantage | Explanation |
|---|---|
| Cheap | Low cost |
| Easy to replace | Can reflash if needed |
| Enough space | 32–64 GB is sufficient |

### Cons

| Disadvantage | Mitigation |
|---|---|
| Can corrupt on sudden power loss | Use a good card and avoid constant writes |
| Not industrial-grade | Acceptable for this project (?) |

### Why not other storage?

| Alternative | Why not |
|---|---|
| USB drive as main storage | More complex boot setup |
| No-name microSD card | Higher failure risk |
| Very large card | Unnecessary cost |

### Approximate cost

| Item | Cost PLN |
|---|---:|
| microSD 32–64 GB | 30–60 |

---

# 5. Power System

## USB Power Bank

### Purpose

Provides portable power to the Raspberry Pi and sensors.

### Recommended choice

- 20,000 mAh preferred
- 5 V output
- At least 2 A output
- Known brand
- Optional: power bank with charge/display feature

### Pros

| Advantage | Explanation |
|---|---|
| Simple | No custom battery electronics |
| Safe | Built-in protection |
| Portable | Easy to carry and replace |
| Cheap | Fits budget |
| Good for demos | Works without mains power |

### Cons

| Disadvantage | Mitigation |
|---|---|
| Limited runtime | Use 20,000 mAh and manage power-hungry sensors |
| Sudden power-off can corrupt SD card | Use good card and test runtime |
| Some power banks shut down at low load | Test with the Pi before final demo |

### Why not other power options?

| Alternative | Why not |
|---|---|
| Custom LiPo battery | More wiring, charging, and safety complexity |
| Lead-acid battery | Too heavy |
| Solar power | Adds complexity and depends on weather |
| Mains power only | Not portable |

### Approximate cost

| Item | Cost PLN |
|---|---:|
| 10,000 mAh power bank | 60–100 |
| 20,000 mAh power bank | 90–160 |

---

# 6. Power Cable

## Micro-USB Cable / Supply

### Purpose

Powers the Raspberry Pi Zero 2 W.

A good cable is important because a bad cable can cause voltage drops and reboots.

### Pros

| Advantage | Explanation |
|---|---|
| Cheap | Low cost |
| Easy to replace | Standard cable |
| Stable power if good quality | Reduces reboot risk |

### Cons

| Disadvantage | Mitigation |
|---|---|
| Micro-USB is mechanically weaker than USB-C | Add strain relief in the case |
| Bad cables cause unstable operation | Use a tested, good-quality cable |

### Approximate cost

| Item | Cost PLN |
|---|---:|
| Cable / supply | 30–60 |

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
| Cheap | Good value |
| Low power | Suitable for battery operation |
| Well supported | Many examples and libraries |

### Cons

| Disadvantage | Mitigation |
|---|---|
| Sensitive to heat from Pi | Place in separate sensor chamber |
| Needs airflow | Use louvered weather-station-style case |
| Fake modules exist | Buy from reputable store |

### Why not other sensors?

| Alternative | Why not |
|---|---|
| DHT11 | Low accuracy, no pressure |
| DHT22 | No pressure, less convenient |
| BMP280 | No humidity |
| SHT31 + BMP280 | More parts and cost for similar result |

### Placement in case

- Inside shaded, ventilated sensor chamber
- Away from Raspberry Pi heat
- Not in direct sunlight
- Not sealed inside plastic

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
| Measures lux | More useful than raw light intensity |
| Digital I2C output | Easy integration |
| Cheap | Low cost |
| Low power | Suitable for portable station |

### Cons

| Disadvantage | Mitigation |
|---|---|
| Needs exposure to ambient light | Mount near top window |
| Can be shadowed by case | Place on top or under clear cover |
| 3D-printed transparent covers can be cloudy | Use clear acrylic insert if possible |

### Why not other light sensors?

| Alternative | Why not |
|---|---|
| Photoresistor | Requires ADC and calibration |
| TSL2591 | Better but more expensive |
| VEML7700 | Good, but BH1750 is usually cheaper |

### Approximate cost

| Item | Cost PLN |
|---|---:|
| BH1750 module | 15–30 |

---

# 9. Real-Time Clock

## DS3231

### Purpose

Keeps accurate time even if the station loses internet or power.

Important because measurement data must have reliable timestamps.

### Links

- Description: [Analog Devices DS3231](https://www.analog.com/en/products/ds3231.html)
- Botland: [Search DS3231](https://botland.com.pl/szukaj?s=ds3231)

### Interface

- I2C

### Pros

| Advantage | Explanation |
|---|---|
| Accurate | Better than DS1307 |
| Battery-backed | Keeps time during power loss |
| Cheap | Low cost |
| Improves data reliability | Timestamps remain correct |

### Cons

| Disadvantage | Mitigation |
|---|---|
| Needs coin cell battery | Buy module with battery installed |
| Extra I2C device | Small complexity increase |

### Why not other time options?

| Alternative | Why not |
|---|---|
| No RTC | Time resets without internet |
| DS1307 | Less accurate |
| GPS time | Too complex for this project |

### Approximate cost

| Item | Cost PLN |
|---|---:|
| DS3231 module | 15–30 |

---

# 10. Air Quality Sensor

## PMS5003 or PMS7003

### Purpose

Measures particulate matter in the air.

Typical measurements:

- PM1.0
- PM2.5
- PM10

This is a valuable addition because air quality is strongly related to environmental monitoring.

### Links

- Botland: [Search PMS5003](https://botland.com.pl/szukaj?s=pms5003)
- Botland: [Search PMS7003](https://botland.com.pl/szukaj?s=pms7003)

### Interface

- UART / serial

### Pros

| Advantage | Explanation |
|---|---|
| Measures PM1.0 / PM2.5 / PM10 | Useful environmental data |
| Built-in fan | Actively samples air |
| Digital serial output | Easy to read |
| Adds significant project value | Makes station more complete |

### Cons

| Disadvantage | Mitigation |
|---|---|
| Higher power consumption | Use duty cycling if battery life is too short |
| Needs airflow | Design intake and exhaust paths in case |
| Larger size | Plan enclosure dimensions early |
| Can draw dust over time | Keep intake protected and serviceable |

### Why not other air quality sensors?

| Alternative | Why not |
|---|---|
| MQ gas sensors | Poor calibration, less meaningful data |
| BME688 | Gas/VOC data is less direct than PM2.5/PM10 |
| SCD30 / SCD4x CO₂ sensors | Good but expensive |
| No air quality sensor | Would reduce project value |

### Mechanical requirement

The PMS sensor needs air to flow through it.

The case should provide:

- shaded intake air
- exhaust path outside the case
- protection from rain
- no exhaust air flowing directly into the BME280

### Approximate cost

| Item | Cost PLN |
|---|---:|
| PMS5003 / PMS7003 | 130–200 |

---

# 11. External Temperature Sensor

## DS18B20 Waterproof Probe

### Purpose

Provides an external temperature reference outside the main enclosure.

This is useful because the Raspberry Pi and PMS sensor can slightly heat the inside of the case.

### Links

- Description: [Analog Devices DS18B20](https://www.analog.com/en/products/ds18b20.html)
- Botland: [Search DS18B20 waterproof probe](https://botland.com.pl/szukaj?s=ds18b20)

### Interface

- 1-Wire

### Pros

| Advantage | Explanation |
|---|---|
| Waterproof probe | Suitable for outside the case |
| Digital sensor | Simple to read |
| Cheap | Low cost |
| Helps validate case design | Compare internal and external temperature |

### Cons

| Disadvantage | Mitigation |
|---|---|
| Needs cable routing | Use cable gland |
| Requires pull-up resistor | Add 4.7 kΩ resistor |
| Probe can heat up in sun | Mount in shaded place |

### Why not other external temperature options?

| Alternative | Why not |
|---|---|
| Second BME280 outside | More expensive, humidity/pressure not needed externally |
| Thermistor | Requires ADC and calibration |
| LM35 | Analog, requires ADC |
| No external probe | Lose useful reference measurement |

### Approximate cost

| Item | Cost PLN |
|---|---:|
| DS18B20 waterproof probe | 20–40 |

---

# 12. 3D-Printed Case

## Design Concept

The case should follow the design principles of real meteorological stations. "Stevenson cage".

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
   - Raspberry Pi Zero 2 W
   - wiring
   - optional power bank

2. Sensor chamber  
   - BME280
   - BH1750 window
   - PMS5003 airflow path

## Sensor Placement

| Sensor | Placement | Reason |
|---|---|---|
| BME280 | Shaded, ventilated central sensor chamber | Avoid Pi heat and direct sunlight |
| BH1750 | Top of case under clear window | Needs ambient light exposure |
| PMS5003 | Inside case with intake/exhaust path | Needs airflow but protection from rain |
| DS18B20 | Outside case on cable | Measures true external temperature |
| Raspberry Pi | Separate electronics chamber | Prevents heat affecting sensor readings |

## Case Design Rules

| Rule | Reason |
|---|---|
| Use white or light-gray filament | Reflects sunlight |
| Use ASA or PETG | Better outdoor behavior than PLA |
| Use louvers | Allows airflow while blocking sun and rain |
| Separate Pi from BME280 | Pi heat would distort temperature readings |
| Route DS18B20 through cable gland | Waterproof and mechanically stable |
| Provide clear window for BH1750 | Needed for light measurement |
| Make case serviceable | Need access to SD card, USB, and sensors |

## Why not other case designs?

| Alternative | Why not |
|---|---|
| Single sealed box | Sensors need airflow; Pi heat would corrupt readings |
| Simple box with holes | Rain and sunlight can enter |
| Metal box | Blocks Wi-Fi and harder to customize |
| No enclosure | Not suitable for portable/outdoor use |
| Off-the-shelf box only | Less custom and less useful for project demonstration |

---

# 13. Battery Life Estimate

Battery life depends strongly on the PMS air quality sensor.

## Approximate Power Consumption

| Component | Estimated Power |
|---|---:|
| Raspberry Pi Zero 2 W | 0.8–2.5 W |
| BME280 | very small |
| BH1750 | very small |
| DS3231 | negligible |
| DS18B20 | very small |
| PMS5003 active | roughly 0.3–0.7 W |

## Estimated Runtime

| Power Bank | PMS Off / Low Load | PMS Duty-Cycled | PMS Always On |
|---|---:|---:|---:|
| 10,000 mAh | ~20–30 h | ~15–25 h | ~10–15 h |
| 20,000 mAh | ~40–60 h | ~30–50 h | ~20–30 h |

These are estimates. Actual runtime should be tested.

## Battery Recommendations

- Use a 20,000 mAh power bank if possible.
- Test the station before the final demo.
- If runtime is too short, reduce PMS sensor active time.
- Use a USB power meter during testing.

---

# 14. Final Bill of Materials

| # | Item | Required | Approx. Cost PLN |
|---:|---|---:|---:|
| 1 | Raspberry Pi Zero 2 W | Yes | 100–140 |
| 2 | microSD card 32–64 GB | Yes | 30–60 |
| 3 | Micro-USB cable / supply | Yes | 30–60 |
| 4 | Power bank 20,000 mAh | Yes | 90–160 |
| 5 | BME280 | Yes | 30–60 |
| 6 | BH1750 | Yes | 15–30 |
| 7 | DS3231 RTC | Yes | 15–30 |
| 8 | PMS5003 / PMS7003 | Yes | 130–200 |
| 9 | DS18B20 waterproof probe | Yes | 20–40 |

---

# 15. Hardware Decisions Summary

| Decision | Choice | Why not alternative |
|---|---|---|
| Main controller | Raspberry Pi Zero 2 W | ESP32 is lower power but worse for local API endpoint |
| Environmental sensor | BME280 | DHT22 has no pressure, BMP280 has no humidity |
| Light sensor | BH1750 | Photoresistor would need ADC and calibration |
| RTC | DS3231 | No RTC would cause timestamp issues offline |
| Air quality | PMS5003/PMS7003 | MQ sensors are less meaningful and harder to calibrate |
| External temperature | DS18B20 probe | Thermistor/LM35 would need ADC and calibration |
| Power | USB power bank | Custom LiPo adds complexity and safety concerns |
| Internet | Wi-Fi / hotspot | LTE/SIM adds cost, power draw, and complexity |
| Analog acquisition | Not planned | No analog sensors are needed |
| Battery monitoring chip | Not planned | USB power meter can be used for testing |
| Case | 3D-printed louvered case | Simple sealed box would distort sensor readings |
| Case material | ASA or PETG | PLA is worse outdoors |
| Case color | White / light gray | Black absorbs heat |

---

# 16. Final Hardware Baseline

The weather station hardware will be:

> Raspberry Pi Zero 2 W  
> + BME280 temperature/humidity/pressure sensor  
> + BH1750 light sensor  
> + DS3231 real-time clock  
> + PMS5003/PMS7003 air quality sensor  
> + DS18B20 external temperature probe  
> + USB power bank  
> + 3D-printed dual-chamber 3D-printed case based on real weather station design

The case will follow the same principles as professional weather stations:

- white exterior
- shaded sensors
- ventilated sensor chamber
- rain protection
- separation between heat-producing electronics and measurement sensors
- external temperature probe mounted outside the main body