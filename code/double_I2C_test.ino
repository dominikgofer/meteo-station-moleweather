#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SDA_PIN 19
#define SCL_PIN 20

RTC_DS3231 rtc;
Adafruit_BME280 bme;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.println("Initializing sensors...");

  if (!rtc.begin()) {
    Serial.println("ERROR: DS3231 not found!");
  } else {
    Serial.println("DS3231 OK");

    if (rtc.lostPower()) {
      Serial.println("RTC lost power. Setting compile time.");
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
  }

  if (!bme.begin(0x76)) {
    Serial.println("ERROR: BME280 not found at 0x76!");
  } else {
    Serial.println("BME280 OK");
  }
}

void loop() {
  DateTime now = rtc.now();

  Serial.println("------------------------");

  Serial.printf("Date: %04d-%02d-%02d\n",
                now.year(), now.month(), now.day());

  Serial.printf("Time: %02d:%02d:%02d\n",
                now.hour(), now.minute(), now.second());

  Serial.print("Temperature: ");
  Serial.print(bme.readTemperature());
  Serial.println(" C");

  Serial.print("Humidity: ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.print("Pressure: ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  delay(2000);
}