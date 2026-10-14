# Raport Wyboru Czujników

## Ograniczenia Projektu Wpływające na Wszystkie Decyzje

| Ograniczenie | Wartość |
|---|---|
| Całkowity budżet projektu | Maksymalnie 1000 PLN |
| Kontroler | ESP32 (logika 3,3 V, zasilanie bateryjne, tryb deep sleep) |
| Harmonogram | 2 miesiące, brak długich czasów dostawy |
| Przenośność | Zasilanie bateryjne, brak zasilania sieciowego |
| Dostępność | Musi być dostępne w Polsce (preferowany Botland) |

---

## 1. Temperatura, Wilgotność, Ciśnienie

### Opisy Kandydatów

*   **BME280:** Kompaktowy, cyfrowy czujnik środowiskowy firmy Bosch, integrujący pomiar temperatury, wilgotności względnej i ciśnienia barometrycznego w jednym module o niskim poborze mocy. Komunikuje się przez I2C lub SPI i jest powszechnie uznawany za standard w amatorskich stacjach pogodowych.
    *   [Nota katalogowa BME280 (Bosch)](https://www.bosch-sensortec.com/products/environmental-sensors/humidity-sensors-bme280/)
    *   [Wyszukiwanie w Botland](https://botland.com.pl/szukaj?s=bme280)
*   **DHT22:** Klasyczny czujnik wilgotności pojemnościowej i temperatury na bazie termistora. Jest bardzo tani, ale korzysta z jednoprzewodowego protokołu cyfrowego wymagającego precyzyjnego taktowania (bit-banging) i nie mierzy ciśnienia.
    *   [Nota katalogowa DHT22](https://cdn-shop.adafruit.com/datasheets/DHT22.pdf)
    *   [Wyszukiwanie w Botland](https://botland.com.pl/szukaj?s=dht22)
*   **BMP280:** Wysokoprecyzyjny cyfrowy czujnik ciśnienia i temperatury firmy Bosch. Jest fizycznie mniejszy niż BME280 i nieco tańszy, ale nie posiada elementu pomiarowego wilgotności.
    *   [Nota katalogowa BMP280 (Bosch)](https://www.bosch-sensortec.com/products/environmental-sensors/pressure-sensors/bmp280/)
    *   [Wyszukiwanie w Botland](https://botland.com.pl/szukaj?s=bmp280)
*   **SHT31:** Wysokiej klasy, w pełni skalibrowany cyfrowy czujnik wilgotności i temperatury firmy Sensirion. Oferuje doskonałą dokładność, ale nie mierzy ciśnienia i jest znacznie droższy niż BME280.
    *   [Nota katalogowa SHT31 (Sensirion)](https://sensirion.com/products/catalog/SHT31-DIS-B/)
    *   [Wyszukiwanie w Botland](https://botland.com.pl/szukaj?s=sht31)

### Porównanie

| Kryterium | BME280 | DHT22 | BMP280 | SHT31 + BMP280 |
|---|---|---|---|---|
| Temperatura | ✅ | ✅ | ✅ | ✅ |
| Wilgotność | ✅ | ✅ | ❌ | ✅ |
| Ciśnienie | ✅ | ❌ | ✅ | ✅ |
| Cyfrowy I2C | ✅ | ❌ (wymaga precyzyjnego taktowania) | ✅ | ✅ |
| Niski pobór mocy | ✅ | ⚠️ | ✅ | ✅ |
| Pojedynczy moduł | ✅ | ✅ | ✅ | ❌ (dwa moduły) |
| Cena | ~40 PLN | ~25 PLN | ~25 PLN | ~70 PLN |

### Decyzja
**Zwycięzca: BME280**

BME280 to jedyny czujnik, który dostarcza wszystkie trzy podstawowe pomiary meteorologiczne (temperaturę, wilgotność, ciśnienie) w jednym, tanim module o niskim poborze mocy z interfejsem I2C. **DHT22** został odrzucony, ponieważ nie mierzy ciśnienia, a jego protokół wymagający precyzyjnego taktowania może blokować wykonywanie kodu na ESP32. **BMP280** został odrzucony, ponieważ nie mierzy wilgotności, co wymagałoby drugiego czujnika. Połączenie **SHT31** z **BMP280** zostało odrzucone, ponieważ podwaja złożoność okablowania i kosztuje niemal dwa razy więcej bez znaczącej korzyści w standardowej stacji pogodowej.

---

## 2. Światło Otoczenia

### Opisy Kandydatów

*   **BH1750:** Cyfrowy czujnik światła otoczenia firmy ROHM, który zwraca natężenie oświetlenia w luksach bezpośrednio przez I2C. Posiada wbudowany przetwornik ADC i szeroki zakres dynamiczny, co czyni go idealnym do monitorowania nasłonecznienia i pogody.
    *   [Nota katalogowa BH1750 (ROHM)](https://www.rohm.com/products/sensors/light-sensors)
    *   [Wyszukiwanie w Botland](https://botland.com.pl/szukaj?s=bh1750)
*   **Fotorezystor (LDR):** Pasywny element analogowy, którego rezystancja maleje wraz ze wzrostem natężenia światła. Wymaga zewnętrznego przetwornika ADC i skomplikowanej kalibracji, aby przeliczyć rezystancję na wartości luksów.
    *   [Fotorezystor – Wikipedia](https://en.wikipedia.org/wiki/Photoresistor)
    *   [Wyszukiwanie w Botland](https://botland.com.pl/szukaj?s=fotorezystor)
*   **TSL2591:** Bardzo wysokodynamiczny cyfrowy czujnik światła firmy AMS. Jest znacznie bardziej czuły niż BH1750 i może działać w warunkach bardzo słabego oświetlenia, ale kosztuje zdecydowanie więcej.
    *   [Nota katalogowa TSL2591 (AMS)](https://ams.com/en/tsl25911)
    *   [Wyszukiwanie w Botland](https://botland.com.pl/szukaj?s=tsl2591)

### Porównanie

| Kryterium | BH1750 | Fotorezystor (LDR) | TSL2591 |
|---|---|---|---|
| Cyfrowy odczyt luksów | ✅ | ❌ (surowa rezystancja) | ✅ |
| Brak konieczności ADC | ✅ | ❌ | ✅ |
| Niski pobór mocy | ✅ | ⚠️ | ✅ |
| Cena | ~20 PLN | ~5 PLN + ADC | ~60 PLN |
| Dokładność | Dobra | Słaba | Doskonała |

### Decyzja
**Zwycięzca: BH1750**

BH1750 dostarcza skalibrowane cyfrowe odczyty luksów przez I2C za najniższą cenę spośród wszystkich cyfrowych czujników światła. **Fotorezystor** został odrzucony, ponieważ wymaga przetwornika ADC, ręcznej kalibracji i daje wysoce nieliniową odpowiedź. **TSL2591** został odrzucony, ponieważ choć jest dokładniejszy w ekstremalnie słabym oświetleniu, kosztuje trzy razy więcej niż BH1750 bez praktycznej korzyści w zewnętrznej stacji pogodowej.

---

## 3. Zegar Czasu Rzeczywistego (RTC)

### Opisy Kandydatów

*   **DS3231:** Wysokoprecyzyjny, kompensowany temperaturowo zegar czasu rzeczywistego firmy Maxim/Analog Devices. Utrzymuje dokładny czas (±2 ppm) nawet bez zasilania głównego, korzystając z baterii pastylkowej.
    *   [Nota katalogowa DS3231 (Analog Devices)](https://www.analog.com/en/products/ds3231.html)
    *   [Wyszukiwanie w Botland](https://botland.com.pl/szukaj?s=ds3231)
*   **DS1307:** Starszy zegar czasu rzeczywistego firmy Maxim/Analog Devices. Jest tańszy niż DS3231, ale nie posiada kompensacji temperaturowej, co oznacza, że jego dokładność znacząco dryfuje wraz ze zmianami temperatury.
    *   [Nota katalogowa DS1307 (Analog Devices)](https://www.analog.com/en/products/ds1307.html)
    *   [Wyszukiwanie w Botland](https://botland.com.pl/szukaj?s=ds1307)
*   **Wewnętrzny RTC ESP32:** Wbudowany sprzęt timera w układzie ESP32 SoC. Choć może śledzić czas podczas głębokiego uśpienia, nie jest kompensowany temperaturowo i traci czas, jeśli napięcie baterii spadnie poniżej progu brownout.
    *   [Dokumentacja trybów uśpienia ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/sleep_modes.html)

### Porównanie

| Kryterium | DS3231 | DS1307 | Wewnętrzny RTC ESP32 |
|---|---|---|---|
| Dokładność | ±2 ppm | ±20 ppm | Znaczący dryf |
| Podtrzymanie bateryjne | ✅ | ✅ | ❌ (traci czas przy utracie zasilania) |
| Przetrwanie deep sleep | ✅ | ✅ | ⚠️ (częściowo) |
| Cena | ~20 PLN | ~15 PLN | Darmowy |

### Decyzja
**Zwycięzca: DS3231**

DS3231 jest obowiązkowy dla integralności danych w przenośnej stacji, która może tracić połączenie Wi-Fi przez dłuższy czas. **DS1307** został odrzucony, ponieważ jego dokładność dryfuje nawet o 20 ppm (minuty miesięcznie) ze względu na zmiany temperatury, podczas gdy DS3231 posiada wewnętrzny kompensowany temperaturowo oscylator kwarcowy (TCXO). **Wewnętrzny RTC ESP32** został odrzucony, ponieważ jest znany ze znaczącego dryfu i traci czas całkowicie, jeśli napięcie baterii spadnie zbyt nisko podczas głębokiego uśpienia.

---

## 4. Pyły Zawieszone (PM1.0 / PM2.5 / PM10)

### Opisy Kandydatów

*   **PMS7003:** Kompaktowy czujnik pyłów zawieszonych oparty na rozpraszaniu laserowym firmy Plantower. Mierzy stężenia PM1.0, PM2.5 i PM10 i zwraca dane przez UART. Jest znacznie cieńszy (12 mm) niż PMS5003.
    *   [Nota katalogowa PMS7003 (Plantower)](https://www.plantower.com/)
    *   [Wyszukiwanie w Botland](https://botland.com.pl/szukaj?s=pms7003)
*   **PMS5003:** Standardowy, grubszy (26 mm) czujnik pyłów zawieszonych oparty na rozpraszaniu laserowym firmy Plantower. Jest funkcjonalnie identyczny z PMS7003, ale fizycznie bardziej masywny.
    *   [Nota katalogowa PMS5003 (Plantower)](https://www.plantower.com/)
    *   [Wyszukiwanie w Botland](https://botland.com.pl/szukaj?s=pms5003)
*   **Sensirion SPS30:** Wysokiej klasy czujnik PM firmy Sensirion z mechanizmem samoczyszczącego wentylatora i doskonałą długoterminową stabilnością. Obsługuje zarówno I2C, jak i UART, ale jest znacznie droższy.
    *   [Nota katalogowa SPS30 (Sensirion)](https://sensirion.com/products/catalog/SPS30/)
    *   [Wyszukiwanie w Botland](https://botland.com.pl/szukaj?s=sps30)
*   **Nova SDS011:** Duży, cylindryczny czujnik pyłów zawieszonych oparty na rozpraszaniu laserowym, popularny w projektach obywatelskiej nauki. Jest bardzo dokładny, ale fizycznie masywny i bardziej energochłonny w porównaniu z serią PMS.
    *   [Nota katalogowa SDS011 (Nova Fitness)](http://www.novafitness.com/)
    *   [Wyszukiwanie w Botland](https://botland.com.pl/szukaj?s=sds011)
*   **Sharp GP2Y1010:** Starszy optyczny czujnik kurzu firmy Sharp. Wykorzystuje diodę IR do wykrywania cząstek kurzu, ale dostarcza jedynie sygnał analogowy wskazujący „obecność kurzu", a nie skalibrowane stężenia masowe.
    *   [Nota katalogowa GP2Y1010 (Sharp)](https://www.sharp-semiconductor.com/)
    *   [Wyszukiwanie w Botland](https://botland.com.pl/szukaj?s=gp2y1010)

### Porównanie

| Kryterium | PMS7003 | PMS5003 | Sensirion SPS30 | Nova SDS011 | Sharp GP2Y1010 |
|---|---|---|---|---|---|
| PM1.0 / 2.5 / 10 | ✅ | ✅ | ✅ | ⚠️ (tylko 2.5/10) | ❌ |
| Cyfrowy UART | ✅ | ✅ | ✅ (I2C/UART) | ✅ | ❌ (analogowy) |
| Rozmiar (grubość) | 12 mm | 26 mm | 4 mm | 40 mm | 15 mm |
| Cena | ~150 PLN | ~130 PLN | ~250 PLN | ~120 PLN | ~40 PLN |

### Decyzja
**Zwycięzca: PMS7003**

PMS7003 dostarcza wszystkie trzy frakcje PM przez prosty interfejs UART 3,3 V i jest fizycznie kompaktowy na tyle, by zmieścić się w obudowie drukowanej 3D. *(Uwaga: Wymaganie 5 V dotyczy wentylatora/lasera, zasilanych bezpośrednio z modułu przetwornicy baterii, podczas gdy logika UART 3,3 V bezpiecznie łączy się z ESP32).* **PMS5003** został odrzucony, ponieważ jest ponad dwukrotnie grubszy (26 mm vs 12 mm), co utrudnia jego czyste zintegrowanie. **SPS30** został odrzucony, ponieważ kosztuje ~100 PLN więcej za marginalny zysk dokładności. **SDS011** został odrzucony, ponieważ jest fizycznie masywny. **Sharp GP2Y1010** został odrzucony, ponieważ jest przestarzałym urządzeniem analogowym, które nie potrafi obliczyć rzeczywistych stężeń masowych PM.

---

## 5. Zewnętrzny Czujnik Temperatury Referencyjnej

### Opisy Kandydatów

*   **DS18B20:** Cyfrowy czujnik temperatury firmy Maxim/Analog Devices dostępny w postaci wodoodpornej sondy ze stali nierdzewnej. Korzysta z protokołu 1-Wire, który obsługuje długie przewody z pojedynczym rezystorem podciągającym.
    *   [Nota katalogowa DS18B20 (Analog Devices)](https://www.analog.com/en/products/ds18b20.html)
    *   [Wyszukiwanie w Botland](https://botland.com.pl/szukaj?s=ds18b20%20wodoodporna)
*   **Drugi BME280:** Użycie drugiej kopii głównego czujnika środowiskowego jako sondy zewnętrznej.
    *   [Nota katalogowa BME280 (Bosch)](https://www.bosch-sensortec.com/products/environmental-sensors/humidity-sensors-bme280/)
    *   [Wyszukiwanie w Botland](https://botland.com.pl/szukaj?s=bme280)
*   **LM35:** Precyzyjny analogowy czujnik temperatury firmy Texas Instruments. Zwraca liniowe napięcie proporcjonalne do temperatury, ale wymaga przetwornika ADC i jest podatny na szumy na długich przewodach.
    *   [Nota katalogowa LM35 (TI)](https://www.ti.com/product/LM35)
    *   [Wyszukiwanie w Botland](https://botland.com.pl/szukaj?s=lm35)
*   **Termistor NTC:** Pasywny analogowy rezystor zmienny, którego rezystancja zmienia się wraz z temperaturą. Wymaga przetwornika ADC i skomplikowanej kalibracji matematycznej Steinharta-Harta.
    *   [Termistor – Wikipedia](https://en.wikipedia.org/wiki/Thermistor)
    *   [Wyszukiwanie w Botland](https://botland.com.pl/szukaj?s=termistor%20ntc)

### Porównanie

| Kryterium | DS18B20 | Drugi BME280 | LM35 | Termistor NTC |
|---|---|---|---|---|
| Dostępna wodoodporna sonda | ✅ | ❌ | ❌ | ⚠️ (z wysiłkiem) |
| Wyjście cyfrowe | ✅ | ✅ | ❌ (analogowe) | ❌ (analogowe) |
| Obsługa długich przewodów | ✅ (1-Wire) | ⚠️ (ograniczenia I2C) | ⚠️ (szumy) | ⚠️ (szumy) |
| Cena | ~25 PLN | ~40 PLN | ~10 PLN + ADC | ~5 PLN + ADC |

### Decyzja
**Zwycięzca: DS18B20**

Wodoodporna sonda DS18B20 może być wyprowadzona na zewnątrz obudowy przez dławnicę kablową i zawieszona w swobodnym powietrzu. Jej protokół 1-Wire obsługuje długie przewody z pojedynczym rezystorem podciągającym, co czyni ją idealną dla sondy zewnętrznej. **Drugi BME280** został odrzucony, ponieważ I2C nie jest zaprojektowane dla długich przewodów i kosztuje niemal dwa razy więcej. **LM35** i **termistor NTC** zostały odrzucone, ponieważ są analogowe, wymagają kanałów ADC i zbierają zakłócenia elektromagnetyczne na długich przewodach.

---

## 6. Ozon (O3)

### Opisy Kandydatów

*   **DFRobot SEN0321:** Elektrochemiczny czujnik ozonu z serii Gravity. Mierzy stężenie O3 w ppb przez I2C, nie wymaga grzałki i jest fabrycznie skalibrowany. Zaprojektowany z myślą o kompatybilności z Arduino/ESP32.
    *   [SEN0321 Wiki (DFRobot)](https://wiki.dfrobot.com/sen0321/)
    *   [Link do produktu w Botland](https://botland.com.pl/gravity-czujniki-gazow-i-pylow/16575-gravity-czujnik-ozonu-i2c-elektrochemiczny-dfrobot-sen0321-6959420916641.html)
*   **MQ-131:** Czujnik gazu ozonowego oparty na tlenku metalu (MOX). Wymaga ciągłej grzałki o dużej mocy do działania i zwraca analogowy sygnał rezystancji. Jest powszechnie uważany za przestarzały do precyzyjnego monitorowania środowiska.
    *   [Czujniki gazów MQ – Wikipedia](https://en.wikipedia.org/wiki/MQ_gas_sensor)
    *   [Wyszukiwanie w Botland](https://botland.com.pl/szukaj?s=mq-131)
*   **DFRobot SEN0472:** Fabrycznie skalibrowana, wyższej klasy wersja elektrochemicznego czujnika O3 firmy DFRobot. Oferuje marginalnie lepszą kalibrację fabryczną, ale kosztuje znacznie więcej.
    *   [SEN0472 Strona DFRobot](https://www.dfrobot.com/product-2516.html)
    *   [Wyszukiwanie w Botland](https://botland.com.pl/szukaj?s=sen0472)

### Porównanie

| Kryterium | SEN0321 | SEN0472 (fabrycznie kalibrowany O3) | MQ-131 |
|---|---|---|---|
| Technologia | Elektrochemiczna | Elektrochemiczna | MOX |
| Ilościowy (ppb) | ✅ | ✅ | ❌ |
| Mierzy O3 | ✅ | ✅ | ✅ |
| Brak grzałki | ✅ | ✅ | ❌ (grzałka) |
| Cena | ~225 PLN | ~300 PLN | ~30 PLN |

### Decyzja
**Zwycięzca: SEN0321**

SEN0321 to jedyny elektrochemiczny czujnik O3 dostępny w Botland z wysyłką 24-godzinną. Dostarcza prawdziwe ilościowe dane w ppb przez I2C bez ciągłej grzałki, co czyni go idealnym dla zasilania bateryjnego z trybem głębokiego uśpienia. **MQ-131** został odrzucony, ponieważ jest starszym czujnikiem MOX wymagającym ciągłej grzałki o dużej mocy (rozładowującej baterię w ciągu godzin) i dostarczającym fatalną dokładność. **SEN0472** został odrzucony, ponieważ kosztuje znacznie więcej za marginalną korzyść kalibracji fabrycznej, którą możemy łatwo skompensować w oprogramowaniu, wykorzystując dane temperatury z BME280.

---

## 7. Dwutlenek Azotu (NO2)

### Opisy Kandydatów

*   **DFRobot SEN0574:** Czujnik NO2 MEMS z serii Fermion. Wykorzystuje technologię mikroelektromechaniczną do wykrywania trendów NO2. Jest tani, energooszczędny (<20 mA) i zwraca sygnał analogowy. Dostarcza jakościowe wykrywanie trendów, a nie skalibrowane wartości ppb.
    *   [SEN0574 Wiki (DFRobot)](https://wiki.dfrobot.com/sen0574/)
    *   [Link do produktu w Botland](https://botland.com.pl/czujniki-gazow/23739-fermion-czujnik-dwutlenku-azotu-no2-mems-01-10ppm-dfrobot-sen0574-6959420923878.html)
*   **DFRobot SEN0471:** Fabrycznie skalibrowany elektrochemiczny czujnik NO2 z serii Gravity. Dostarcza wysoce dokładne, ilościowe dane w ppb przez I2C/UART, ale jest ekstremalnie drogi.
    *   [SEN0471 Strona DFRobot](https://www.dfrobot.com/product-2515.html)
    *   [Wyszukiwanie w Botland](https://botland.com.pl/szukaj?s=sen0471)
*   **DFRobot SEN0441 (MiCS-2714):** Czujnik wielogazowy MEMS z serii Fermion wykrywający NO, NO2 i H2. Wymaga ścisłego zasilania 5 V i pobiera 0,45 W (90 mA) ciągłej mocy.
    *   [SEN0441 Wiki (DFRobot)](https://wiki.dfrobot.com/sen0441/)
    *   [Link do produktu w Botland](https://botland.com.pl/czujniki-gazow/20679-fermion-analogowy-czujnik-gazu-no-no2-h2-mems-mics-2714-dfrobot-sen0441-6959420920617.html)
*   **Sensirion SGP41:** Cyfrowy czujnik MOX mierzący indeksy VOC i NOx. Nie mierzy O3 i zwraca względny indeks (1–500) zamiast bezwzględnych stężeń gazu.
    *   [Nota katalogowa SGP41 (Sensirion)](https://sensirion.com/products/catalog/SGP41)
    *   [Wyszukiwanie w Botland](https://botland.com.pl/szukaj?s=sgp41)
*   **SGX MICS-6814:** Potrójny czujnik MOX mierzący CO, NO2 i NH3. Posiada trzy wewnętrzne grzałki pobierające łącznie 88 mA ciągłego prądu i jest zazwyczaj dostępny wyłącznie przez AliExpress z długim czasem wysyłki.
    *   [Nota katalogowa MICS-6814 (SGX)](https://www.sgxsensortech.com/products/products-for-industrial-applications/mems-sensors/mics-6814/)
    *   [Link do produktu na AliExpress](https://pl.aliexpress.com/item/1005008394812865.html)

### Porównanie

| Kryterium | SEN0574 | SEN0471 (elektrochemiczny NO2) | SEN0441 (MiCS-2714) | SGP41 | MICS-6814 |
|---|---|---|---|---|---|
| Technologia | MEMS | Elektrochemiczna | MEMS MOX | Cyfrowy MOX | Potrójny MOX |
| Ilościowy ppb | ❌ (jakościowy) | ✅ | ⚠️ (półilościowy) | ❌ (tylko indeks) | ⚠️ (półilościowy) |
| Pobór mocy | <20 mA | <5 mA | ~90 mA (0,45 W) | ~2 mA | ~88 mA |
| Napięcie | 3,3–5 V | 3,3–5,5 V | Tylko 5 V | 3,3 V | 5 V |
| Cena | ~33 PLN | ~600 PLN | ~36 PLN | ~90 PLN | ~60 PLN |

### Decyzja
**Zwycięzca: SEN0574**

To najważniejszy kompromis inżynierski w projekcie. Idealny czujnik NO2 (**SEN0471**) kosztuje ~600 PLN i pochłonąłby ponad połowę całkowitego budżetu projektu na jeden komponent. **SEN0574** dostarcza wykrywanie NO2 za 33 PLN, wykorzystując technologię MEMS z niskim poborem mocy. *Znane ograniczenie: SEN0574 dostarcza jakościowe wykrywanie trendów, a nie skalibrowane wartości ppb. Jest to akceptowane jako kompromis inżynierski podyktowany budżetem.* **SEN0441** został odrzucony, ponieważ pobiera 0,45 W ciągłej mocy i wymaga ścisłego zasilania 5 V, co jest fatalnym profilem energetycznym dla ESP32 zasilanego bateryjnie. **SGP41** został odrzucony, ponieważ zwraca jedynie względny „indeks NOx" zamiast stężenia NO2 i nie mierzy O3. **MICS-6814** został odrzucony, ponieważ jego trzy wewnętrzne grzałki pobierają 88 mA ciągłego prądu (zabijając żywotność baterii), a jest dostępny wyłącznie przez AliExpress z 3–6-tygodniowym czasem wysyłki.

---

## 8. Przetwornik Analogowo-Cyfrowy (ADC)

### Opisy Kandydatów

*   **ADS1115:** Wysokoprecyzyjny, 16-bitowy przetwornik analogowo-cyfrowy firmy Texas Instruments. Komunikuje się przez I2C i zapewnia doskonałą liniowość oraz niski szum do odczytu analogowych czujników gazów.
    *   [Nota katalogowa ADS1115 (TI)](https://www.ti.com/product/ADS1115)
    *   [Wyszukiwanie w Botland](https://botland.com.pl/szukaj?s=ads1115)
*   **MCP3008:** 10-bitowy przetwornik analogowo-cyfrowy firmy Microchip. Komunikuje się przez magistralę SPI i jest tańszy niż ADS1115, ale oferuje znacznie niższą rozdzielczość.
    *   [Nota katalogowa MCP3008 (Microchip)](https://www.microchip.com/en-us/product/mcp3008)
    *   [Wyszukiwanie w Botland](https://botland.com.pl/szukaj?s=mcp3008)
*   **Wewnętrzny ADC ESP32:** Wbudowany przetwornik analogowo-cyfrowy w układzie ESP32 SoC. Choć darmowy, jest powszechnie udokumentowany w społeczności inżynierskiej jako wysoce nieliniowy, szumny i niedokładny w niskich zakresach napięć.
    *   [Dokumentacja ADC ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc_oneshot.html)

### Porównanie

| Kryterium | ADS1115 | Wewnętrzny ADC ESP32 | MCP3008 |
|---|---|---|---|
| Rozdzielczość | 16-bit | 12-bit | 10-bit |
| Liniowość | Doskonała | Słaba | Dobra |
| Interfejs | I2C | Wbudowany | SPI |
| Kanały | 4 | Do 18 | 8 |
| Cena | ~30 PLN | Darmowy | ~20 PLN |

### Decyzja
**Zwycięzca: ADS1115**

Czujniki gazów zwracają małe napięcia analogowe, które wymagają precyzyjnego pomiaru. ADS1115 zapewnia 16-bitową rozdzielczość przez I2C, współdzieląc tę samą magistralę z innymi czujnikami cyfrowymi. **Wewnętrzny ADC ESP32** został odrzucony, ponieważ jest powszechnie udokumentowany jako wysoce nieliniowy i szumny, co uczyniłoby dane z czujnika NO2 SEN0574 bezużytecznymi. **MCP3008** został odrzucony, ponieważ korzysta z magistrali SPI (dodając niepotrzebną złożoność okablowania, gdy nasza magistrala I2C ma dużo wolnych adresów) i oferuje jedynie 10-bitową rozdzielczość w porównaniu z 16-bitową rozdzielczością ADS1115.

---

## Podsumowanie: Kompletna Lista Materiałów Czujników

| # | Czujnik | Pomiar | Interfejs | Cena PLN |
|---:|---|---|---|---:|
| 1 | BME280 | Temperatura, Wilgotność, Ciśnienie | I2C | 30–60 |
| 2 | BH1750 | Światło otoczenia (luksy) | I2C | 15–30 |
| 3 | DS3231 | Zegar czasu rzeczywistego | I2C | 15–30 |
| 4 | PMS7003 | PM1.0 / PM2.5 / PM10 | UART | 130–200 |
| 5 | DS18B20 | Temperatura zewnętrzna | 1-Wire | 20–40 |
| 6 | SEN0321 | Ozon (O3) | I2C | ~225 |
| 7 | SEN0574 | Dwutlenek azotu (NO2) | Analogowy → ADS1115 | ~33 |
| 8 | ADS1115 | ADC dla czujnika NO2 | I2C | 25–35 |
| | **Całkowity koszt czujników** | | | **~493–653 PLN** |

---

## Mapa Interfejsów Czujników

| Magistrala | Urządzenia |
|---|---|
| **I2C** | BME280, BH1750, DS3231, SEN0321, ADS1115 |
| **UART** | PMS7003 |
| **1-Wire** | DS18B20 |
| **Analogowy → ADS1115** | SEN0574 |
| **GPIO (sterowanie zasilaniem)** | Pin SET PMS7003, bramka MOSFET SEN0574 |

---