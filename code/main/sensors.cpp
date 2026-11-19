#include "sensors.h"
#include "config.h"
#include "logger.h"

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "RTClib.h"
#include <PMS.h>
#include <TinyGPSPlus.h>

static Adafruit_BME280 bme;
static RTC_DS3231 rtc;
static PMS pms(Serial1);
static PMS::DATA pmsData;
static TinyGPSPlus gps;

static bool bmeReady = false;
static bool rtcReady = false;
static unsigned long lastPmsMessage = 0;

void beginSensors() {
  analogReadResolution(12);
  analogSetPinAttenuation(O3_PIN, ADC_11db);
  analogSetPinAttenuation(NO2_PIN, ADC_11db);

  Wire.begin(I2C_SDA, I2C_SCL);

  bmeReady = bme.begin(0x76, &Wire);
  debugPrint(bmeReady ? "BME280 ready" : "BME280 unavailable");

  rtcReady = rtc.begin(&Wire);
  if (rtcReady) {
    if (rtc.lostPower()) {
      debugPrint("RTC lost power, setting compile time");
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    debugPrint("RTC DS3231 ready");
  } else {
    debugPrint("RTC DS3231 unavailable");
  }

  Serial1.begin(9600, SERIAL_8N1, PMS_RX_PIN, PMS_TX_PIN);
  debugPrint("PMS7003 initialized on Serial1");

  Serial0.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  debugPrint("GPS initialized on Serial0");
}

void pollSensors() {
  if (pms.read(pmsData)) {
    lastPmsMessage = millis();
  }

  while (Serial0.available() > 0) {
    gps.encode(Serial0.read());
  }
}

void readBME280(Reading &r) {
  if (bmeReady) {
    r.v[AIR_TEMP] = bme.readTemperature();
    r.v[HUMIDITY] = bme.readHumidity();
    r.v[PRESSURE] = bme.readPressure() / 100.0F;
  }
}

void readProbe(Reading &r) {
  (void)r;
}

void readParticles(Reading &r) {
  r.v[PM1] = static_cast<float>(pmsData.PM_AE_UG_1_0);
  r.v[PM25] = static_cast<float>(pmsData.PM_AE_UG_2_5);
  r.v[PM10] = static_cast<float>(pmsData.PM_AE_UG_10_0);
}

void readOzone(Reading &r) {
  const int samples = 20;
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(O3_PIN);
  }
  const float adc = sum / static_cast<float>(samples);
  const float voltage = adc * 3.3f / 4095.0f;
  r.v[O3] = voltage * 100.0f;
}

void readNO2(Reading &r) {
  const int samples = 20;
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(NO2_PIN);
  }
  const float adc = sum / static_cast<float>(samples);
  const float voltage = adc * 3.3f / 4095.0f;
  r.v[NO2] = voltage * 0.5f;
}

void readGPS(Reading &r) { 
  if (gps.location.isValid() && gps.location.age() < 10000) { 
    r.lat = gps.location.lat(); 
    r.lon = gps.location.lng(); 
    r.fix = true; 
  } else { 
    r.lat = NAN; 
    r.lon = NAN; 
    r.fix = false; 
  }

  r.alt = gps.altitude.isValid() ? gps.altitude.meters() : NAN; 
  r.sats = gps.satellites.isValid() ? gps.satellites.value() : 0; 
}

uint32_t readClockUTC(uint64_t ms) {
  (void)ms;
  return rtcReady ? rtc.now().unixtime() : 0;
}

bool getFormattedTimestamp(char *buffer, size_t len, uint64_t fallbackMs) {
  if (rtcReady) {
    DateTime now = rtc.now();
    snprintf(buffer, len, "%04d-%02d-%02d %02d:%02d:%02d",
             now.year(), now.month(), now.day(),
             now.hour(), now.minute(), now.second());
    return true;
  }
  snprintf(buffer, len, "Uptime_%llums", static_cast<unsigned long long>(fallbackMs));
  return false;
}

float readBMEAltitude() {
  return bmeReady ? bme.readAltitude(SEALEVELPRESSURE_HPA) : 0.0f;
}