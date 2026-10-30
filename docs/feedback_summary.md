# Feedback od drugiej drużyny

### Druga drużyna komentowała proponowane rozwiązania w następujących kwestiach

- Dyskusja Raspberry pi vs ESP32 (or adjescent)
    - czy wystarczy pinów na esp32
    - ile miałoby żyć na baterii
    - na podstawie dyskusji z drugą grupą doszliśmy do wniosku, że dobrym wyborem jest ESP32.

- Dyskusja dotycząca konfiguracji urządzenia
    - co ile czasu będzie możliwość komunikacji z urządzeniem, oraz jak zmieniać konfigurację.
    - druga grupa miała słuszne wątpliwości co to jakości rozwiązania polegającego na możliwości przesłania na urządzenie nowej konfiguracji jedynie w ustalonych interwałach czasu.
    - Na podstawie słusznej krytyki ze strony drugiej grupy zrezygnowaliśmy z tego rozwiązania (ostatecznie cały plan komunikacji z urządzeniem uległ zmianie, częściowo w związku z tym feedbackiem).

- Odporność druku 3d na warunki atmosferyczne
    - Druga grupa skomentowała rozwiązanie druku 3d podstawowym tworzywem PLA jako niewłaściwe ze względu na brak odporności tego tworzywa na światło słoneczne.
    - Ta kwestia jeszcze nie została do końca rozwiązana, jednak najprawdopodobniej zostanie wykorzystany gotowy box na elektronikę z certyfikatem IP65

- Druga grupa zwróciła uwagę, że urządzenie powinno komunikować się z użytkownikiem za pomocą diody, aby informować o podstawowych parametrach takich jak zasilanie, lub niski stan akumulatorów. Ze względu na maksymalną energooszczędność, dioda ta nie będzie na pewno zapalona cały czas, jednak możnaby np. zastosować rozwiązanie, w którym dioda zapala się, gdy akumulatory są prawie rozładowane.

- Druga drużyna nie była pewna, czy wystarczy pinów i protokołów aby obsłużyć wszystkie czujniki. Jednak z naszego researchu wynika, że wszystkie protokoły (uart, spi, I2C, one wire, ADC) będą obsłużone bez problemu.

- Druga drużyna zasugerowała, że możnaby pomyśleć nad zewnętrznym, samodzielnym modułem zasilania, zarządzającym procesem monitorowania i ładowania ogniw. Jednak w wyniku researchu podjęliśmy decyzję, żeby skorzystać z wbudowanego w płytkę deweloperską modułu, ponieważ jest on przewidziany do uzyskania maksymalnej energooszczędności.  Dyskutowaliśmy również o tym, czy LilyGo daje wystarczający prąd na wyjściu, i czy lepiej użyć przetwornicy step-up, czy standalone układu ładujęcego baterie i dającego dwa różne napięcia (3.3V i 5V) na wyjściu.

- Dostaliśmy feedback, że należałoby się dowiedzieć w jaki sposób urządzenie, i czy urządzenie jest świadome tego, że straciłoby dostęp do sieci. (Dotyczy starej wersji z GSM)

- Otrzymaliśmy wątpliwości odnośnie pojemności karty na dane. Po szacunkowych obliczeniach, wątpliwości rozwialiśmy

- Druga drużyna zwróciła uwagę na zakres temperatury operacyjnej (temperatura ogniw, czujników itd)