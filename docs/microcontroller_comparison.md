# Analiza porównawcza mikrokontrolerów i platform deweloperskich

## 1. Wprowadzenie i Kryteria Wyboru
W ramach projektu przenośnej stacji meteorologicznej działającej w warunkach outdoorowych (góry, las, brak stałego zasilania i zasięgu), kluczowymi kryteriami wyboru jednostki sterującej są:
* Niski pobór prądu w stanie czuwania (Deep Sleep) umożliwiający pracę bateryjną przez tygodnie.
* Odpowiednia moc obliczeniowa i pamięć do obsługi wielu czujników środowiskowych i karty SD.
* Stosunek ceny do możliwości (budżet poniżej 1000 PLN).
* Niezawodność w warunkach terenowych.

---

## 2. Porównanie alternatywnych rozwiązań

### A. Raspberry Pi Zero 2 W (Pełny komputer jednopłytkowy)
* **Zalety:** Posiada pełny system Linux, wbudowane Wi-Fi/Bluetooth, łatwość oprogramowania w Pythonie[cite: 7].
* **Dlaczego odrzucony:** Pobiera dramatycznie dużo prądu (ciągłe zużycie rzędu 200–500 mA i więcej). W warunkach leśnych, przy zasilaniu z ogniw 18650, wyładowałby akumulator w ciągu kilkunastu godzin[cite: 4, 7]. Generuje również zbyt dużo ciepła wewnętrznego, co zaburzałoby pomiary temperatury z czujnika BME280.

### B. Klasyczne Arduino (np. Arduino Nano / Uno na ATmega328P)
* **Zalety:** Bardzo proste w obsłudze, tanie.
* **Dlaczego odrzucony:** Posiada zaledwie 2 KB pamięci RAM, co uniemożliwia stabilną obsługę zaawansowanych bibliotek czujników pyłu (PMS5003), zegara RTC, karty SD oraz serwera HTTP jednocześnie. Brak wbudowanej łączności bezprzewodowej.

### C. Arduino + Moduły Radiowe (np. RFM / LoRa)
* **Zalety:** Dobra łączność dalekiego zasięgu w otwartym terenie.
* **Dlaczego odrzucony:** Wymaga noszenia ze sobą osobnego urządzenia odbiorczego (bazy) w plecaku. W gęstym, zalesionym i pagórkowatym terenie zasięg fal radiowych gwałtownie spada, a projekt zakłada jedną, zwartą, całkowicie autonomiczną stację typu Data Logger.

### D. Nordic nRF (np. nRF52840)
* **Zalety:** Legendarna, ultrawydajna energooszczędność w stanie uśpienia.
* **Dlaczego odrzucony:** Układy te nie posiadają wbudowanego Wi-Fi (tylko BLE/Zigbee), co uniemożliwia postawienie lokalnego serwera www i wygodnego pobierania danych przez użytkownika w terenie. Mają też mniejszą pulę gotowych bibliotek dla złożonego zestawu czujników meteorologicznych w środowisku Arduino IDE.

---

## 3. Wybór ostateczny: ESP32 (DevKit / WROOM-32)
* **Dlaczego wygrywa:** ESP32 stanowi optymalny kompromis ("sweet spot"). Oferuje tryb głębokiego snu (*Deep Sleep*) pobierający mikroampery[cite: 3], potężny dwurdzeniowy procesor z 520 KB pamięci RAM, natywne Wi-Fi/Bluetooth do tworzenia lokalnego punktu dostępowego (AP) z interfejsem www[cite: 3] oraz bardzo bogaty ekosystem gotowych bibliotek dla czujników BME280, BH1750, PMS5003 i modułu GPS[cite: 3].