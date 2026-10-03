## płytka
- raspberry pi zje największe ogniwo w kilkanaście godzin max
- ESP32 poradzi sobie z przesyłem http do serwera - są gotowe płytki z tym modułem (LilyGo TTGO t-Call lub LilyGo T-SIM7000G)
- ponieważ 2G umiera, to lepiej wziąć T-SIM7000G / T-SIM7080G-3S które ma 4g LTE
- T-SIM7000G nie ma na botlandzie
- czyste ESP32 z modułem GSM podobno stwarza problemy:
	- tani stabilizator napięcia powoduje, że podczas próby kontaktu z serwerem moduł GSM szarpnie sporo prądu, a przez to napięcie spadnie tak bardzo, że płytka się zrestartuje. T-SIM7000G ma przetwornicę
	- zwykła płytka ESP32 ma gorszy deep sleep i szybciej rozładuje akumulator (podobno stabilizator i układ konwertera USB-UART nawet w uśpieniu pobierają prąd)


#### opcje:

1. można próbować kupić T-SIM7080G gdzieś indziej (na amazonie jest z wysyłką z polski, 200zł) 
2. czyste ESP32 z modułem GSM, zrobić jakiś osobny układ zasilania modułu GSM żeby nie restartowało całego mikrokontrolera

opcja 1: https://www.amazon.pl/LILYGO-T-SIM7080G-S3-Standard-rozwojowa-PMU/dp/B0BW3NN54L?crid=3IVHZN4HZW64P&dib=eyJ2IjoiMSJ9.3dmzHWx-T_kVgvlv3YqJkFoGH2MudnKopQzAggTfbXDWdLd6mLmwDqPP4XFu_-vtWVQg3lBUp2jlpAPjagK-YQ.Qv7u6xUnWVbW4hqBgDF5BmJ6ubqM4wuXNSyKKthzqR4&dib_tag=se&keywords=t-sim7000g&qid=1791026674&sprefix=T-SIM%2Caps%2C112&sr=8-5




## komponenty

- Temperatura, wilgotność, ciśnienie - BME280, jest na botlandzie, 40zł

- zanieczyszczenie powietrza - Plantower PMS5003, jest na botlandzie, PM1.0, PM2.5, PM10, czujnik optyczny więc nie jest idealny, np. we mgle się może pogubić, ale w tej cenie tak będzie, 80zł

- GPS - jest wbudowany w T-SIM7080G

- natężenie światła - BH1750, jest na botlandzie, 27zł

- ogniwo - Samsung 3400mAh, jest na botlandzie, taka pojemność powinna wystarczyć na tygonie jeśli nie miesiące pracy ESP32 w deep sleep, 30zł

- jeśli jest czyste ESP32 a nie LilyGo, to potrzebny moduł ładowania TP4056, jest na botlandzie, ~5zł

- ewentualnie panel słoneczny, jakiś na botlandzie, LilyGo T-SIM7080G ma gniazdo na panel słoneczny, 20-40zł

- karta sim, kable itp. (koszyk na ogniwo jest na płycie), <100zł?



O ile zakup takiego LilyGo przejdzie, to chyba najprostsze rozwiązanie, bo ma moduł GSM i lepszy moduł zasilania. Jak będzie problem z zakupem, to chyba trzeba będzie składać z czystego ESP32 albo szukać jakichś innych płytek.

Jakoś bardzo się nie zagłębiałem, więc pewnie są jakieś lepsze opcje, ale takie chyba powinno zadziałać.