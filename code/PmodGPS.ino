
#include <TinyGPSPlus.h>

#define GPS_RX_PIN 4
#define GPS_TX_PIN 5

TinyGPSPlus gps;

unsigned long lastDisplay = 0;
const unsigned long displayInterval = 5000; // 5 seconds

void setup() {
  Serial.begin(115200);

  Serial1.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

  Serial.println("\n==================================");
  Serial.println("       ESP32-C6 GPS MONITOR");
  Serial.println("==================================");
  Serial.println("Waiting for GPS signal...");
}

void loop() {

  // Continuously read GPS data
  while (Serial1.available() > 0) {
    gps.encode(Serial1.read());
  }

  // Display data every 5 seconds
  if (millis() - lastDisplay >= displayInterval) {
    lastDisplay = millis();

    Serial.println("\n----------------------------------");
    Serial.println("          GPS MEASUREMENT");
    Serial.println("----------------------------------");

    if (gps.location.isValid() &&
        gps.location.age() < 10000) {

      Serial.println("GPS STATUS: FIX ACQUIRED");

      Serial.print("Latitude:    ");
      Serial.println(gps.location.lat(), 6);

      Serial.print("Longitude:   ");
      Serial.println(gps.location.lng(), 6);

      Serial.print("Satellites:  ");
      if (gps.satellites.isValid())
        Serial.println(gps.satellites.value());
      else
        Serial.println("N/A");

      Serial.print("Altitude:    ");
      if (gps.altitude.isValid()) {
        Serial.print(gps.altitude.meters(), 2);
        Serial.println(" m");
      } else {
        Serial.println("N/A");
      }

      Serial.print("Speed:       ");
      if (gps.speed.isValid()) {
        Serial.print(gps.speed.kmph(), 2);
        Serial.println(" km/h");
      } else {
        Serial.println("N/A");
      }      

    } else {
      Serial.println("GPS STATUS: NO FIX");
      Serial.println("Waiting for satellite connection...");

      Serial.print("Satellites visible/used: ");
      if (gps.satellites.isValid())
        Serial.println(gps.satellites.value());
      else
        Serial.println("N/A");

      Serial.print("Characters received: ");
      Serial.println(gps.charsProcessed());
    }

    Serial.println("----------------------------------");
  }
}
