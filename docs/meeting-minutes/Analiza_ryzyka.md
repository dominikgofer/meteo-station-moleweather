# Analiza ryzyka – stacja meteo


## 1. Opis projektu i zakres analizy

Analiza obejmuje ryzyka techniczne, zasilania, bezpieczeństwa i organizacyjne stacji meteo opartej na ESP32, od zakupu komponentów do pierwszych pomiarów w terenie.

| Pole | Wartość |
| --- | --- |
| Nazwa projektu | Stacja meteo |
| Cel projektu | pomiar temperatury, wilgotności, ciśnienia, pyłów PM i gazów O3/NO2 z zapisem na kartę SD |
| Środowisko pracy | na zewnątrz, -10°C-30°C, osłona przed deszczem |
| Budżet | do 800 zł  |

### Elementy systemu objęte analizą

| Blok | Komponenty | Interfejs |
| --- | --- | --- |
| Jednostka sterująca | FireBeetle 2 ESP32-C6 | GPIO, I2C, UART, SPI |
| Pomiar klimatu | BME280 (temperatura, wilgotność, ciśnienie), DS18B20 (sonda wodoodporna) | I2C, 1-Wire |
| Jakość powietrza | PMS7003(pyły PM1.0 / PM2.5 / PM10), Czujnik O3 MQ131, Czujnik NO2 MEMS SEN0571| UART, I2C |
| Czas i lokalizacja | DS3231 (RTC), moduł GPS Pmod | I2C, UART |
| Zapis danych | moduł microSD + karta 16 GB | SPI |
| Zasilanie | 2 × 18650 (Samsung 35E, 3400 mAh), przetwornica step-up U3V40F5 (5 V), koszyk 2×AA z JST-PH | — |
| Montaż | płytka stykowa 400 otworów, przewody, rezystory (3 × 4,7 kΩ, 1 × 100 kΩ) | — |

**Poza zakresem:** punkt dostępowy poprzez Wi-Fi, obudowa wodoodporna

## 2. Matryca Ryzyk i Ocena Zagrożeń


| Lp. | Obszar analizy | Zagrożenie / Potencjalny Problem | Prawdopodobieństwo (1-5) | Wpływ (1-5) | Poziom Ryzyka (P X W) |
|:---:|:---|:---|:---:|:---:|:---:|
| **1** | **Zasilanie i Energetyka** | Spadek napięcia poniżej progu zadziałania pod obciążeniem czujnika ozonu (O3) i przetwornicy Step-Up. | 3 | 4 | **12 (Średni)** |
| **2** | **Kompatybilność I2C** | Kolizja adresów heksadecymalnych na wspólnej magistrali I2C (BME280, DS3231, SEN0321, SEN0574). | 2 | 4 | **8 (Niski)** |
| **3** | **Integracja Czujników** | Brak stabilności oprogramowania lub błędy kompilacji dla nowej architektury RISC-V w układzie ESP32-C6. | 3 | 3 | **9 (Średni)** |
| **4** | **Logistyka i Dostawy** | Opóźnienie dostawy czujnika pyłu PMS7003 / niedostępność wybranego modelu w sklepie Botland. | 3 | 4 | **12 (Średni)** |
| **5** | **Środowisko i Montaż** | Kondensacja wilgoci w klatce Stevensona i awaria połączeń na płytce stykowej w terenie. | 4 | 5 | **20 (Wysoki)** |
| **6** | **Przechowywanie Danych** | Błędy zapisu na karcie MicroSD (FAT32) w warunkach wahań napięcia i głębokiego uśpienia. | 2 | 3 | **6 (Niski)** |


---


## 3. Szczegółowy Opis Ryzyk, Założenia i Scenariusze Mitygacji


### Ryzyko 1: Wydajność układu zasilania i stabilizację napięcia
* **Założenia i specyfika sprzętowa:** Projekt wykorzystuje dwa ogniwa Samsung INR18650-35E (3400 mAh każde) połączone w koszyku równolegle (co daje napięcie nominalne 3.6V - 4.2V przy podwojonej pojemności ok. 6800 mAh). Do zasilania czujnika pyłu PMS7003 wymagającego stabilnych 5 V zastosowano wysokowydajną przetwornicę Step-Up Pololu U3V40F5.
* **Potencjalny skutek:** Chwilowy, gwałtowny wzrost poboru prądu (szczególnie podczas jednoczesnego uruchamiania wiatraczka czujnika pyłu) może spowodować nagły spadek napięcia na ogniwach (voltage sag), co doprowadzi do awaryjnego resetu mikrokontrolera ESP32-C6.
* **Strategia mitygacji:**
  1. Wprowadzenie oprogramowania sekwencyjnego (nieuruchamianie wszystkich sensorów naraz). Czujnik pyłu i czujniki gazu powinny być włączane pojedynczo w zaplanowanych oknach czasowych.
  2. Wykorzystanie dedykowanego tranzystora kluczującego (MOSFET) do całkowitego odcinania zasilania od prądożernych modułów w okresach uśpienia (Deep Sleep).


### Ryzyko 2: Konflikty adresów na magistrali I2C
* **Założenia i specyfika sprzętowa:** Do wspólnej magistrali I2C podłączamy równolegle czujnik BME280, zegar RTC DS3231, czujnik ozonu SEN0321 oraz czujnik NO2 SEN0574.
* **Potencjalny skutek:** Jeśli dwa urządzenia posiadają domyślnie ten sam adres w przestrzeni I2C, mikrokontroler nie będzie w stanie odczytać z nich danych, a magistrala ulegnie zawieszeniu.
* **Strategia mitygacji:**
  1. Szczegółowa weryfikacja tabel adresowych w dokumentacjach technicznych każdego modułu przed fizycznym połączeniem.
  2. W razie kolizji adresów – wykorzystanie programowej szyny I2C (SoftWire) na alternatywnych pinach GPIO lub zastosowanie multipleksera I2C (np. TCA9548A).


### Ryzyko 3: Wyzwania oprogramowania i nowa architektura ESP32-C6 (RISC-V)
* **Założenia i specyfika sprzętowa:** Płytka FireBeetle 2 opiera się na nowoczesnym czipie ESP32-C6 z rdzeniem RISC-V oraz obsługą Wi-Fi 6 i 802.15.4, co różni ją od starszych układów opartych na architekturze Xtensa.
* **Potencjalny skutek:** Niektóre niszowe lub starsze biblioteki czujników w środowisku Arduino IDE mogą nie być w pełni przystosowane do nowej architektury kompilacji, powodując błędy typu `compilation error`.
* **Strategia mitygacji:**
  1. Przeprowadzenie wstępnych testów kompilacji bibliotek (Sensor, RTClib, TinyGPS++) na wczesnym etapie prac projektowych.
  2. W przypadku uporczywych problemów z bibliotekami niskopoziomowymi, przygotowanie kodu opierającego się bezpośrednio na stabilnym SDK (ESP-IDF).


### Ryzyko 4: Logistyka i opóźnienia w dostawie podzespołów
* **Założenia i specyfika sprzętowa:** Czujnik pyłu zawieszonego (np. PMS7003) w wybranym momencie ma status "oczekiwanie na dostawę". Dodatkowo moduł GPS (Pmod) ma być pozyskany bezpośrednio od dr. Moronia.
* **Potencjalny skutek:** Zakłócenie pracy zespołu nad montażem obudowy i testami zintegrowanymi ze względu na brak kluczowych komponentów.
* **Strategia mitygacji:**
  1. Uwzględnienie w harmonogramie alternatywnego modelu czujnika pyłu (np. dostępnego od ręki wariantu PMS5003 na 5 V, który ma identyczną logikę transmisji UART).
  2. Równoległe prowadzenie prac nad logiką zapisu danych na karcie MicroSD, obsługą RTC i czujnika BME280, podczas gdy sekcja pyłowa i GPS zostaną zintegrowane natychmiast po fizycznym dotarciu komponentów.


### Ryzyko 5: Środowisko zewnętrze, kondensacja i awaryjność prototypu
* **Założenia i specyfika sprzętowa:** Stacja ma pracować w warunkach outdoorowych (góry, las) w klatce Stevensona drukowanej w 3D z materiału ASA. W fazie prototypowej planowane jest użycie płytki stykowej (400 otworów) oraz przewodów męsko-męskich i żeńsko-męskich.
* **Potencjalny skutek:** Wilgoć i poranna rosa w lesie mogą powodować zwarcia na stykowych połączeniach otwartych. Luźne połączenia na płytce stykowej mogą się wypiąć pod wpływem drgań lub zmian temperatury.
* **Strategia mitygacji:**
  1. Zrezygnowanie z płytki stykowej w wersji docelowej na rzecz trwałego lutowania połączeń na uniwersalnej płytce drukowanej lub bezpośrednio w oparciu o listwy goldpin.
  2. Zabezpieczenie elektroniki specjalnym lakierem ochronnym (np. Plastik 71) oraz umieszczenie modułów wrażliwych wewnątrz szczelnej komory głównej klatki Stevensona, wyprowadzając jedynie sondę temperatury DS18B20 na zewnątrz poprzez uszczelnioną dławnicę kablową.


## 3. Podsumowanie

Projekt cechuje się bardzo dobrą relacją kosztów do możliwości (szacowany budżet podzespołów z czujnikami gazów O3 i NO2 zamyka się w granicach ok. 600–650 PLN, mieszcząc się bezpiecznie w limicie 1000 PLN). Kluczem do sukcesu technicznego będzie:
* **Zarządzanie energią** poprzez programowe wyłączanie czujników gazów i pyłu w okresach między pomiarami.
* **Wykorzystanie przycisku zewnętrznego** do wybudzania modułu Wi-Fi na żądanie, co gwarantuje długą żywotność ogniw 18650 w trybie autonomicznym.
 
