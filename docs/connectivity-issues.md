# Problemy z łącznością

## Wykrywanie dostępu do sieci

Moduł GSM potrafi samodzielnie określić stan połączenia z siecią oraz siłę sygnału. Przed każdą próbą transmisji stacja sprawdza status rejestracji w sieci i poziom sygnału — dzięki temu wie, czy wysyłka danych jest w ogóle możliwa.

Komendy AT używane do sprawdzenia stanu sieci:

| Komenda | Opis |
|---|---|
| `AT+CREG?` | Status rejestracji w sieci GSM (1 = zarejestrowany, 5 = roaming, 0 = brak) |
| `AT+CGATT?` | Czy transmisja pakietowa (GPRS/LTE) jest aktywna (1 = tak, 0 = nie) |
| `AT+CSQ` | Siła sygnału RSSI (wartość 99 = brak sygnału) |

## Buforowanie danych

Jeśli w momencie wybudzenia stacja nie ma dostępu do sieci, zebrane pomiary są zapisywane na karcie SD i czekają na wysłanie. Przy kolejnym wybudzeniu stacja najpierw próbuje nadrobić zaległe dane, a następnie wysyła bieżące pomiary.

Dzięki temu żaden pomiar nie jest tracony z powodu chwilowego braku zasięgu.

### Estymata pojemności (karta 8 GB)

Założenie: 10 pól pomiarowych na rekord (timestamp + 9 czujników) w formacie CSV = ~200 bajtów/rekord.

Kalkulacja pojemności: 8 GB = 8 589 934 592 B ÷ 200 B = ~42 949 672 rekordów.

Kalkulacja dni do zapełnienia (rekordy/dzień = rekordy/min × 60 × 24):
- co 1 min:  60 × 24 = 1 440 rekordów/dzień → 42 949 672 ÷ 1 440 = ~29 826 dni
- co 5 min:  12 × 24 = 288 rekordów/dzień → 42 949 672 ÷ 288 = ~149 130 dni
- co 15 min: 4 × 24 = 96 rekordów/dzień → 42 949 672 ÷ 96 = ~447 392 dni

Pojemność karty nigdy nie będzie ograniczeniem — ważniejsza jest trwałość samej karty (typowo 10 000–100 000 cykli zapisu na sektor). Warto stosować rotację plików (np. jeden plik na dobę) dla łatwiejszej diagnostyki, nie dla oszczędności miejsca.
