#pragma once
#include <Arduino.h>

inline constexpr const char *AP_SSID = "ognistyzuczek"; 
inline constexpr const char *AP_PASSWORD = "123456789"; 

inline constexpr bool USE_MOCK_DATA = false; 
inline constexpr bool DEBUG_ENABLED = true; 
inline constexpr bool DEBUG_USB = false; 

inline constexpr uint32_t SAMPLE_INTERVAL_MS = 5000; 
inline constexpr size_t HISTORY_SIZE = 120; 

inline constexpr size_t LOG_LINES = 32; 
inline constexpr size_t LOG_LINE_LENGTH = 160; 

// Piny SD (SPI)
inline constexpr int SD_CS = 18; 
inline constexpr int SD_MOSI = 22; 
inline constexpr int SD_MISO = 23; 
inline constexpr int SD_SCK = 21; 

// Piny I2C
inline constexpr int I2C_SDA = 19; 
inline constexpr int I2C_SCL = 20; 

// Piny PMS7003 (Serial1) i GPS (Serial0)
inline constexpr int PMS_RX_PIN = 5; 
inline constexpr int PMS_TX_PIN = 4; 
inline constexpr int GPS_RX_PIN = 17; 
inline constexpr int GPS_TX_PIN = 16; 

// Piny analogowe i LED
inline constexpr int O3_PIN = 1; 
inline constexpr int NO2_PIN = 2; 
inline constexpr int LED_PIN = LED_BUILTIN; 

// Sterowanie MOSFET, podświetleniem i przyciskiem Wi-Fi
inline constexpr int MOSFET_PIN = 7;        // 0 (LOW) = włączone
inline constexpr int BTN_LIGHT_PIN = 6;     // 1 (HIGH) = podświetlenie przycisku
inline constexpr int BTN_INPUT_PIN = 2;     // Wejście przycisku (INPUT_PULLUP)
inline constexpr int BTN_GND_PIN = 3;       // Pin odniesienia (LOW) dla przycisku

inline constexpr float SEALEVELPRESSURE_HPA = 1013.25f; 
inline constexpr const char *CSV_FILE_NAME = "/dane.csv"; 