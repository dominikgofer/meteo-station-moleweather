#include "sd_logger.h"
#include "config.h"
#include "logger.h"
#include "sensors.h"

#include "FS.h" 
#include "SD.h" 
#include "SPI.h" 
#include <math.h>
#include <stdio.h>

static bool sdReady = false;

void beginSD() {
  pinMode(LED_PIN, OUTPUT); 
  digitalWrite(LED_PIN, LOW); 

  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS); 
  sdReady = SD.begin(SD_CS, SPI, 1000000); 

  if (!sdReady) { 
    debugPrint("ERROR: Card Mount Failed"); 
    digitalWrite(LED_PIN, HIGH); 
    return;
  }

  debugPrint("SD card mounted successfully"); 

  if (!SD.exists(CSV_FILE_NAME)) { 
    File file = SD.open(CSV_FILE_NAME, FILE_WRITE); 
    if (file) { 
      file.println("Data_Czas;Temperatura_C;Cisnienie_hPa;WysokoscBME_m;Wilgotnosc_%;PM1_0_ug3;PM2_5_ug3;PM10_0_ug3;O3_ppb;NO2_ppm;Szer_Geogr;Dlug_Geogr;Satelity;WysokoscGPS_m"); 
      file.close(); 
      debugPrint("Created new CSV file with headers"); 
    } else { 
      debugPrint("ERROR: Failed to create CSV file"); 
      digitalWrite(LED_PIN, HIGH); 
    }
  }
}

void saveReadingToSD(const Reading &r) {
  if (!sdReady) { 
    digitalWrite(LED_PIN, HIGH); 
    return;
  }

  char timeBuffer[32];
  getFormattedTimestamp(timeBuffer, sizeof(timeBuffer), r.ms);

  const float temp = isfinite(r.v[AIR_TEMP]) ? r.v[AIR_TEMP] : 0.0f; 
  const float press = isfinite(r.v[PRESSURE]) ? r.v[PRESSURE] : 0.0f; 
  const float altBME = readBMEAltitude(); 
  const float hum = isfinite(r.v[HUMIDITY]) ? r.v[HUMIDITY] : 0.0f; 
  const int pm1 = isfinite(r.v[PM1]) ? static_cast<int>(r.v[PM1]) : 0; 
  const int pm25 = isfinite(r.v[PM25]) ? static_cast<int>(r.v[PM25]) : 0; 
  const int pm10 = isfinite(r.v[PM10]) ? static_cast<int>(r.v[PM10]) : 0; 
  const float o3 = isfinite(r.v[O3]) ? r.v[O3] : 0.0f; 
  const float no2 = isfinite(r.v[NO2]) ? r.v[NO2] : 0.0f; 

  char latBuf[20], lonBuf[20], altGpsBuf[20];

  if (r.fix) { 
    snprintf(latBuf, sizeof(latBuf), "%.6f", r.lat); 
    snprintf(lonBuf, sizeof(lonBuf), "%.6f", r.lon); 
  } else { 
    snprintf(latBuf, sizeof(latBuf), "Brak_Fixu"); 
    snprintf(lonBuf, sizeof(lonBuf), "Brak_Fixu"); 
  }

  if (isfinite(r.alt)) { 
    snprintf(altGpsBuf, sizeof(altGpsBuf), "%.2f", r.alt); 
  } else { 
    snprintf(altGpsBuf, sizeof(altGpsBuf), "Brak_Fixu"); 
  }

  char row[240];
  snprintf(row, sizeof(row),
           "%s;%.2f;%.2f;%.2f;%.2f;%d;%d;%d;%.2f;%.3f;%s;%s;%u;%s",
           timeBuffer, temp, press, altBME, hum,
           pm1, pm25, pm10, o3, no2,
           latBuf, lonBuf, static_cast<unsigned int>(r.sats),
           altGpsBuf); 

  File file = SD.open(CSV_FILE_NAME, FILE_APPEND); 
  if (file) { 
    file.println(row);
    file.close(); 
    digitalWrite(LED_PIN, LOW); 
  } else { 
    debugPrint("ERROR: Failed to append to CSV file on SD"); 
    digitalWrite(LED_PIN, HIGH); 
  }
}