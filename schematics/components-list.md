# Electronic Components List

### Mainboard & Connectivity
- **[ESP32 DevKit](https://botland.store/esp32-modules/24608-esp32-c6-devkitc-1-n8-wifibluetoothzigbeethread-development-board-with-esp32-c6-wroom-1-chip-5904422385644.html):** Main microcontroller managing sensor data, sleep cycles, and communication. **Interface:** Host (master) for SPI, I2C, UART, 1-Wire and GPIO. **Docs:** [Datasheet](components-documentation/ESP32-C6-WROOM-1_datasheet.pdf)
- **[MicroSD Card SPI Module](https://botland.store/memory-cards-accessories/5631-microsd-card-reader-module-waveshare-3947-5904422333867.html):** Stores local data logs and persistent configuration. **Interface:** SPI. **Docs:** [Schematic](components-documentation/MicroSD_Storage_Board_schematic.pdf)

### Sensors
- **[BME280](https://botland.store/pressure-sensors/11803-bme280-humidity-temperature-and-pressure-5904422366179.html):** Measures temperature, humidity, and barometric pressure via I2C. **Interface:** I2C. **Docs:** [Datasheet](components-documentation/BME280_datasheet.pdf)
- **[Plantower PMS5003](https://botland.store/air-quality-sensors/6797-dustair-quality-sensor-pm10-pm25-pm10-pms5003-5v-uart-5904422359041.html):** Measures PM1.0, PM2.5, and PM10 particulate matter via UART. **Interface:** UART. **Docs:** [Datasheet](components-documentation/PMS5003_datasheet.pdf)
- **[BH1750](https://botland.store/sensors-of-light-and-color/2024-light-intensity-sensor-bh1750-5904422373283.html):** Measures ambient light intensity in lux via I2C. **Interface:** I2C. **Docs:** [Datasheet](components-documentation/BH1750_datasheet.pdf)
- **[DS18B20](https://botland.store/digital-temperature-sensors/165-temperature-sensor-ds18b20-digital-1-wire-tht-5904422366513.html):** Measures external reference temperature via 1-Wire. **Interface:** 1-Wire. **Docs:** [Datasheet](components-documentation/DS18B20.pdf)

### Power Modules
- **[18650 Li-Ion Battery & Charger/Shield](https://botland.store/battery-holders/12118-cell-holder-for-li-ion-battery-type-18650-5904422318673.html):** Supplies portable power and manages battery charging. **Interface:** None (power only)
- **[N-Channel MOSFET (2N7000)](https://botland.store/n-mosfet/8318-transistor-n-mosfet-2n7000-10pcs-5904422308063.html):** Cuts power to high-draw sensors during deep sleep. **Interface:** GPIO (digital output drives the gate). **Docs:** [Datasheet](components-documentation/2N7000_2N7002_NDS7002A.PDF)
