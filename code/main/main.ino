#include <Arduino.h>
#include <esp_random.h> 
#include "config.h"
#include "types.h"
#include "logger.h"
#include "sensors.h"
#include "sd_logger.h"
#include "web_server.h"

static Reading history[HISTORY_SIZE]; 
static size_t historyHead = 0; 
static size_t historyCount = 0; 
static uint32_t sequenceNumber = 0; 
static uint32_t lastSampleAt = 0; 

static void initControlPins() {
  // MOSFET: 0 (LOW) włącza zasilanie układu
  pinMode(MOSFET_PIN, OUTPUT);
  digitalWrite(MOSFET_PIN, LOW);

  // Podświetlenie przycisku: 1 (HIGH)
  pinMode(BTN_LIGHT_PIN, OUTPUT);
  digitalWrite(BTN_LIGHT_PIN, HIGH);

  // Przycisk na pinach 2 i 3: pin 3 jako LOW, pin 2 jako INPUT_PULLUP
  pinMode(BTN_GND_PIN, OUTPUT);
  digitalWrite(BTN_GND_PIN, LOW);
  pinMode(BTN_INPUT_PIN, INPUT_PULLUP);
}

static void checkApButton() {
  static bool lastStablePressed = false;
  static uint32_t lastDebounceAt = 0;

  const bool rawPressed = (digitalRead(BTN_INPUT_PIN) == LOW);
  const uint32_t now = millis();

  if (rawPressed != lastStablePressed && (now - lastDebounceAt) >= 40) {
    lastDebounceAt = now;
    lastStablePressed = rawPressed;

    if (rawPressed) {
      if (isAccessPointReady()) {
        debugPrint("Button pressed -> stopping Wi-Fi AP");
        stopAccessPoint();
      } else {
        debugPrint("Button pressed -> starting Wi-Fi AP");
        startAccessPoint();
      }
    }
  }
}

static void takeSample() { 
  Reading r; 
  r.id = ++sequenceNumber; 
  r.ms = uptimeMs(); 
  r.utc = readClockUTC(r.ms); 

  readBME280(r); 
  readProbe(r); 
  readParticles(r); 
  readOzone(r); 
  readNO2(r); 
  readGPS(r); 

  history[historyHead] = r; 
  historyHead = (historyHead + 1) % HISTORY_SIZE; 
  if (historyCount < HISTORY_SIZE) historyCount++; 

  saveReadingToSD(r); 

  if (DEBUG_ENABLED) { 
    debugPrint(
      "Sample #" + String(r.id) +
      " | air=" + String(r.v[AIR_TEMP], 1) +
      " C | PM2.5=" + String(r.v[PM25], 1) +
      " | O3=" + String(r.v[O3], 1) +
      " | NO2=" + String(r.v[NO2], 3)
    );
  }
}

void setup() { 
#if ARDUINO_USB_CDC_ON_BOOT && ARDUINO_USB_MODE 
  if (DEBUG_ENABLED && DEBUG_USB) { 
    Serial.begin(115200); 
    Serial.setTxTimeoutMs(0); 
  }
#endif

  initControlPins();
  delay(20); // krótka chwila na ustabilizowanie zasilania po załączeniu MOSFET-a

  snprintf(bootId, sizeof(bootId), "%08lx", static_cast<unsigned long>(esp_random())); 
  debugPrint("Weather station starting"); 

  beginSensors(); 
  beginSD();

  initWebServerData(history, &historyCount, &historyHead, &sequenceNumber);

  takeSample(); 
  lastSampleAt = millis(); 
}

void loop() { 
  pollSensors(); 
  checkApButton();

  const uint32_t now = millis(); 
  if (static_cast<uint32_t>(now - lastSampleAt) >= SAMPLE_INTERVAL_MS) { 
    lastSampleAt = now; 
    takeSample(); 
  }

  loopWebServer();
  pollSensors(); 
  delay(1); 
}