## WiFi vs BLE pros and cons
|Kryterium	|Bluetooth LE (BLE)|	Wi-Fi|
|---|---|---|
|Zużycie energii|	Zwykle niższe przy sporadycznej komunikacji|	Zwykle wyższe podczas aktywnego połączenia|
|Zasięg|	Kilka–kilkadziesiąt metrów, zależnie od warunków|	Podobnie lub dalej, zależnie od anten i otoczenia|
|Pobieranie historii z SD|	Wystarczające, ale duże historie mogą przesyłać się dłużej	|Zwykle znacznie szybsze|
|Odczyt aktualnych pomiarów|	Bardzo dobry|	Bardzo dobry|
|Zmiana ustawień stacji|	Tak	|Tak|
|Wymagany router	|Nie	|Nie, jeśli ESP32 tworzy własny punkt dostępowy|
|Aplikacja mobilna	|Potrzebna obsługa BLE	|Można użyć aplikacji albo strony WWW|
|Łatwość prototypowania|	Umiarkowana — trzeba zaprojektować komunikację BLE|	Prosta, jeśli wystarczy lokalny panel WWW|
|Praca bez internetu	|Tak|	Tak, w trybie punktu dostępowego|
|Działanie w tle na telefonie|	Ograniczone regułami Androida/iOS	|Zależy od sposobu połączenia i systemu telefonu|

##

wifi wydaje się lepszą opcją, jeśli chcemy przesyłać historię danych z długiego okresu czasu (tydzien+) szybkość przesyłania będzie lepsza, można zaimplementowac przycisk uruchamiający wifi na żądanie, więc nie tracimy prądu.

Aplikację tworzymy jako serwis webowy. Jest też lepiej dostępna, nie trzeba kombinować z obsługą web bluetooth, które też nie jest dostępne z każdej przeglądarki - trzebaby było zaprojektować protokół przesyłania danych.

główną zaletą BLE jest energooszczędność, ale jeśli włączalibyśmy wifi tylko na żądanie i wyłączali po chwili to nie powinno być dużej różnicy.
