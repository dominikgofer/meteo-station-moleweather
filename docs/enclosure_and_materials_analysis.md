# Analiza obudowy meteorologicznej, materiałów (ASA/PETG) oraz odporności termiczno-atmosferycznej

## 1. Wybór konstrukcji obudowy: Klatka Stevensona
Do budowy naszej przenośnej stacji pogodowej zastosujemy sprawdzoną koncepcję **klatki Stevensona** (Stevenson screen), szeroko stosowaną w profesjonalnej meteorologii.
* **Cel:** Zapewnienie swobodnego przepływu powietrza wokół czujników środowiskowych (BME280) przy jednoczesnej całkowitej ochronie przed bezpośrednim promieniowaniem słonecznym (które fałszowałoby odczyty temperatury) oraz opadami atmosferycznymi.
* **Architektura dwukomorowa:** Obudowę podzielimy projektowo na dwie sekcje:
  1. *Komora czujników:* Wentylowana, ożebrowana (louvered), odizolowana termicznie.
  2. *Komora elektroniki:* Hermetyczna obudowa montowana na klamry, zabezpieczona dławnicami kablowymi, mieszcząca ESP32, kartę SD i zasilanie.

## 2. Dobór polimeru do druku 3D w warunkach outdoorowych
Przeanalizowaliśmy popularne filamenty pod kątem zastosowania zewnętrznego w górach i lesie:
* **PLA (Odrzucone):** Choć najprostsze w druku, ma bardzo niską odporność na temperaturę zeszklenia (~60°C). Zostawione na letnim słońcu na polanie uległoby deformacji i stopieniu. Dodatkowo degraduje pod wpływem promieniowania UV.
* **PETG (Alternatywa):** Dobra odporność chemiczna i UV, łatwy w druku, wytrzymały mechanicznie. Jednak pod długotrwałym działaniem silnego słońcia również wykazuje tendencję do blaknięcia i powolnej degradacji strukturalnej.
* **ASA / ABS (Wybór główny - ASA):** Tworzywo optymalne do zastosowań zewnętrznych. **ASA (Acrylonitrile Styrene Acrylate)** posiada wbudowaną naturalną odporność na promieniowanie UV (w przeciwieństwie do ABS nie żółknie i nie kruszeje na słońcu), ma świetną odporność termiczną (do ponad 90°C) oraz dużą sztywność mechaniczną w trudnych warunkach leśnych.

## 3. Dodatkowa ochrona powierzchniowa (Powłoka UV)
Aby zmaksymalizować żywotność wydruku 3D z ASA i zabezpieczyć go przed mikropęknięciami od deszczu i mrozu:
* Zastosujemy natryskowe zabezpieczenie bezbarwnym lakierem odpornym na UV (np. *Dupli-Color Lakier Bezbarwny* dostępny w lokalnych marketach budowlanych w cenie ok. 27 zł).
* Lakier tworzy dodatkową barierę hydrofobową, zapobiegając wnikaniu wilgoci w warstwy wydruku.

## 4. Analiza wpływu niskich temperatur na elektronikę i obudowę
* W warunkach zimowych / górskich (testowo do -20°C) tworzywa sztuczne stają się bardziej kruche. Zastosowanie grubości ścianek min. 3 mm oraz odpowiedniej liczby obrysów (perimeters) w slicerze (min. 4 obrysy) zapewni obudowie odporność na przypadkowe uderzenia w plecaku podczas hikingu.
* Dodatkowo uwzględniono ograniczenie programowe: czujnik pyłu PMS5003 ze względu na mechanikę optyczną wiatraczka i ryzyko zamarzania kondensatu nie powinien pracować w temperaturach poniżej -20°C – w takich warunkach kod musi wyłączać procedurę pomiaru PM, chroniąc silniczek przed uszkodzeniem mechanicznym.