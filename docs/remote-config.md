# Zdalna konfiguracja stacji przez serwer

## Problem

Stacja meteorologiczna działa w trybie oszczędzania energii — moduł GSM wybudza się tylko na czas transmisji danych, po czym wraca do uśpienia. Stacja nie nasłuchuje połączeń przychodzących i nie ma stałego adresu (brak potrzeby DNS, statycznego IP, otwartych portów).

## Rozwiązanie: konfiguracja pull z serwera

Przy każdym cyklu wybudzenia moduł GSM wykonuje dwa oddzielne zapytania:

1. Wysyła zebrane dane pomiarowe do serwera (POST).
2. Pobiera aktualną konfigurację z serwera (GET).
3. Odbiera nowe ustawienia i zapisuje je na karcie SD.
4. Przechodzi z powrotem w tryb uśpienia.

Konfiguracja przechowywana na karcie SD jest nieulotna — stacja zachowuje ostatnie ustawienia nawet po utracie zasilania lub resecie.

Dzięki temu wszelkie zmiany konfiguracji są dostarczane bez konieczności aktywnego dostępu do urządzenia.

## Parametr konfigurowalny przez serwer

Jedynym parametrem jest `sleep_interval_s` — czas między kolejnymi pomiarami wyrażony w sekundach.

## Schemat komunikacji

```
[Stacja wstaje] → POST (pomiary)
               → GET  (konfiguracja) ← { sleep_interval_s: 300 }
[Stacja zapisuje config na SD] → [Stacja zasypia]
```

## Zalety

- Stacja nie wymaga stałego połączenia ani nasłuchiwania.
- Brak potrzeby DNS, statycznego IP ani VPN po stronie urządzenia.
- Konfiguracja może być zmieniana w dowolnym momencie — zostanie pobrana przy następnym wybudzeniu.
- Prosta implementacja po stronie firmware (jedno zapytanie przy każdym cyklu).
