#include <Arduino.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define SD_CS   18
#define SD_MOSI 19
#define SD_MISO 20
#define SD_SCK  21

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("\n--- SD card diagnostic ---");

  // Configure SPI pins explicitly
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  Serial.println("Initializing SD card...");

  // Start at a low SPI frequency for troubleshooting
  if (!SD.begin(SD_CS, SPI, 1000000)) {
    Serial.println("ERROR: Card Mount Failed");

    if (SD.cardType() == CARD_NONE) {
      Serial.println("No SD card detected.");
    }

    return;
  }

  Serial.println("SD card mounted successfully!");

  uint8_t type = SD.cardType();

  Serial.print("Card type: ");
  if (type == CARD_MMC) Serial.println("MMC");
  else if (type == CARD_SD) Serial.println("SDSC");
  else if (type == CARD_SDHC) Serial.println("SDHC");
  else Serial.println("Unknown");

  Serial.print("Card size: ");
  Serial.print(SD.cardSize() / (1024 * 1024));
  Serial.println(" MB");

  File file = SD.open("/test.txt", FILE_WRITE);

  if (file) {
    file.println("SD card test successful");
    file.close();
    Serial.println("File write successful!");
  } else {
    Serial.println("File write failed!");
  }
}

void loop() {
}