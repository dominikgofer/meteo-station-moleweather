# Gas Sensors (O3 & NO2) – Hardware Architecture

## 1. Executive Summary: The Best Course of Action

To achieve "Paczkomat-style" (professional Airly-style) air quality monitoring within our strict 1000 PLN budget and ESP32 battery constraints, we must make a calculated engineering trade-off between sensor accuracy and cost.

### The Recommended Combination:
1. **Ozone (O3): DFRobot SEN0321 (Electrochemical)**
2. **Nitrogen Dioxide (NO2): DFRobot SEN0574 (MEMS)**

### Why this is the optimal engineering choice:
Professional monitoring stations use **electrochemical sensors** for both gases. However, the high-end electrochemical NO2 sensor (DFRobot SEN0471) costs over **600 PLN**. Combining it with the O3 sensor would consume almost our entire project budget on just two gas sensors. 

Fortunately, the **electrochemical O3 sensor (SEN0321)** is significantly cheaper (~225 PLN) and provides true, quantitative parts-per-billion (ppb) data with extremely low power consumption. 

To stay within budget, we compromise on NO2 by using the **MEMS NO2 sensor (SEN0574)** for only **~33 PLN**. While the SEN0574 only provides *qualitative* trend data (detecting the presence and relative spikes of NO2 rather than exact laboratory-grade ppb), it operates at low voltage (3.3V), draws minimal current (<20mA), and allows us to monitor NO2 pollution events without bankrupting the project or killing the battery.

---

# 2. Detailed Sensor Breakdown

Below is the piece-by-piece evaluation of all considered O3 and NO2 sensors.

## 2.1. DFRobot SEN0321 – Electrochemical O3 (The Winner)

### Purpose
Measures ground-level Ozone (O3), a critical component of summer smog and photochemical pollution.

### Links
- Botland: [Gravity - Ozone Sensor I2C - Electrochemical](https://botland.com.pl/gravity-czujniki-gazow-i-pylow/16575-gravity-czujnik-ozonu-i2c-elektrochemiczny-dfrobot-sen0321-6959420916641.html)
- DFRobot Wiki: [Gravity I2C Ozone Sensor](https://wiki.dfrobot.com/sen0321/)

### Specifications
- **Technology:** Electrochemical (generates micro-current via chemical reaction)
- **Interface:** I2C (Digital)
- **Range:** 0–10 ppm (Resolution: 10 ppb)
- **Power Draw:** Extremely low (No internal heater required)
- **Voltage:** 3.3V – 5.5V

### Pros
| Advantage | Explanation |
|---|---|
| **Paczkomat-grade Tech** | Uses the same electrochemical principle as professional Airly stations. |
| **Digital I2C** | Plugs directly into the ESP32 I2C bus. No analog noise issues. |
| **Ultra-Low Power** | No heater means it does not drain the battery during deep sleep cycles. |
| **High Resolution** | 10 ppb resolution is excellent for environmental monitoring. |

### Cons
| Disadvantage | Mitigation |
|---|---|
| **Price** | ~225 PLN is a significant chunk of the budget, but justified for O3 data. |
| **Initial Burn-in** | Requires 24–48 hours of continuous power on first use to stabilize. |

### Verdict
**BUY.** This is the best possible O3 sensor for a battery-powered ESP32 project that fits the budget.

---

## 2.2. DFRobot SEN0574 – MEMS NO2 (The Pragmatic Choice)

### Purpose
Detects Nitrogen Dioxide (NO2), a primary pollutant from vehicle exhaust and winter smog.

### Links
- Botland: [Fermion - NO2 nitrogen dioxide sensor - MEMS](https://botland.com.pl/czujniki-gazow/23739-fermion-czujnik-dwutlenku-azotu-no2-mems-01-10ppm-dfrobot-sen0574-6959420923878.html)
- DFRobot Wiki: [Fermion MEMS NO2 Sensor](https://wiki.dfrobot.com/sen0574/)

### Specifications
- **Technology:** MEMS (Micro-Electro-Mechanical Systems) Metal Oxide
- **Interface:** Analog Voltage Output
- **Range:** 0.1 – 10 ppm
- **Power Draw:** < 20 mA
- **Voltage:** 3.3V – 5V

### Pros
| Advantage | Explanation |
|---|---|
| **Extremely Cheap** | ~33 PLN allows us to add NO2 monitoring without breaking the budget. |
| **Low Power (for MOX)** | <20mA is manageable and can be power-gated via a MOSFET. |
| **3.3V Compatible** | Works natively with the ESP32 logic levels. |
| **Compact** | Tiny 13x13mm footprint. |

### Cons
| Disadvantage | Mitigation |
|---|---|
| **Qualitative Only** | Botland explicitly states it provides *qualitative* measurements. It will show NO2 trends/spikes, not exact calibrated ppb. |
| **Analog Output** | Requires an external ADC (ADS1115) because the ESP32 internal ADC is too noisy for gas sensors. |

### Verdict
**BUY.** The perfect budget compromise. We sacrifice exact laboratory calibration to gain NO2 trend detection for only 33 PLN.

---

## 2.3. DFRobot SEN0471 – Electrochemical NO2 (The Budget Buster)

### Purpose
Professional, factory-calibrated quantitative NO2 measurement.

### Links
- DFRobot: [Factory Calibrated Electrochemical NO2 Sensor](https://www.dfrobot.com/product-2515.html)

### Specifications
- **Technology:** Electrochemical
- **Interface:** I2C / UART / Analog
- **Power Draw:** < 5 mA
- **Voltage:** 3.3V – 5.5V

### Why we are NOT choosing this:
While this is the exact "Paczkomat-grade" equivalent for NO2, it costs **~600 PLN** ($152 USD). Adding this to the SEN0321 O3 sensor would cost over 825 PLN just for gas sensors, making the rest of the weather station impossible to build within the 1000 PLN limit. 

---

## 2.4. DFRobot SEN0441 – MiCS-2714 MEMS (The Power Hog)

### Purpose
Multi-gas detection (NO, NO2, H2).

### Links
- Botland: [Fermion - Analog gas sensor NO, NO2, H2 - MEMS MiCS-2714](https://botland.com.pl/czujniki-gazow/20679-fermion-analogowy-czujnik-gazu-no-no2-h2-mems-mics-2714-dfrobot-sen0441-6959420920617.html)

### Why we are NOT choosing this:
- **0.45W Power Draw:** It draws ~90mA continuously. 
- **Strict 5V Requirement:** It requires 4.9V–5.1V, meaning it cannot be powered directly from the ESP32's 3.3V rail.
- While it has an "Enable" pin to turn it off, its high power requirements and voltage constraints make it much harder to integrate into a deep-sleep battery architecture than the SEN0574.

---

## 2.5. Sensirion SGP41 – Digital VOC/NOx (The Index Sensor)

### Purpose
Measures VOC and NOx indices for indoor air quality.

### Links
- Sensirion: [SGP41 Product Page](https://sensirion.com/products/catalog/SGP41)

### Why we are NOT choosing this:
- **No O3:** It does not measure Ozone.
- **No Absolute Values:** It outputs a "NOx Index" (1 to 500) rather than actual ppm/ppb concentrations.
- **Availability:** Not readily available at Botland; requires international shipping, risking our 2-month timeline.

---

## 2.6. MICS-6814 – Triple MOX (The Battery Killer)

### Purpose
Measures CO, NO2, and NH3 on a single chip.

### Links
- AliExpress: [CJMCU MICS-6814 Module](https://pl.aliexpress.com/item/1005008394812865.html)

### Why we are NOT choosing this:
- **88mA Continuous Heater:** It has three internal heaters that draw massive current. It will drain our 18650 battery in roughly 34 hours.
- **Shipping:** AliExpress delivery takes 3–6 weeks.
- **No Warranty:** High risk of dead-on-arrival modules.

---

# 3. System Integration & Architecture Impact

Adding the **SEN0321 (O3)** and **SEN0574 (NO2)** to the ESP32 baseline requires the following hardware and software adjustments:

## Hardware Additions
1. **ADS1115 (16-bit I2C ADC):** 
   - *Why:* The ESP32's internal analog-to-digital converter is notoriously noisy and non-linear. To get usable data from the analog SEN0574 NO2 sensor, we must reintroduce the ADS1115 to the I2C bus.
2. **N-Channel MOSFET (e.g., 2N7000 or BSS138):**
   - *Why:* The SEN0574 has no built-in sleep pin and draws <20mA continuously. To preserve battery life during ESP32 deep sleep, the ESP32 will use a GPIO pin to trigger the MOSFET, physically cutting power to the SEN0574 when not in use.

## Software Workflow (Duty Cycling)
1. ESP32 wakes from Deep Sleep.
2. ESP32 triggers MOSFET to power ON the SEN0574 (NO2).
3. Wait 30–60 seconds for the MEMS sensor to stabilize.
4. Read SEN0321 (O3) via I2C.
5. Read SEN0574 (NO2) via ADS1115 (I2C).
6. Read BME280 (Temp/Hum) to apply mathematical compensation to the gas readings.
7. Save data to SD Card / Broadcast via API.
8. Trigger MOSFET to power OFF the SEN0574.
9. ESP32 enters Deep Sleep.

*(Note: The SEN0321 O3 sensor draws so little power that it can remain powered on, or power-gated alongside the NO2 sensor depending on exact microamp measurements during testing).*

---

# 4. Updated Budget Impact

| Item | Purpose | Estimated Cost (PLN) |
|---|---|---:|
| DFRobot SEN0321 | Quantitative O3 (Electrochemical, I2C) | ~225 PLN |
| DFRobot SEN0574 | Qualitative NO2 Trends (MEMS, Analog) | ~33 PLN |
| ADS1115 Module | 16-bit ADC for NO2 analog reading | ~30 PLN |
| 2N7000 MOSFET + Resistors | Power-gating for SEN0574 deep sleep | ~5 PLN |
| **Total Gas Sensor Addition** | | **~293 PLN** |

**Previous ESP32 Baseline Cost:** ~390 – 695 PLN  
**New Total Estimated Cost:** ~683 – 988 PLN  

**Result:** The project successfully incorporates advanced, dual-gas environmental monitoring (O3 and NO2) while strictly remaining under the **1000 PLN maximum budget limit**.