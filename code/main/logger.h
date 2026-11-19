#pragma once
#include <Arduino.h>

extern char bootId[9];

uint64_t uptimeMs();
String uint64Text(uint64_t value);
String jsonNumber(double value, uint8_t decimals);

void debugPrint(const String &message);
String getDebugLogsText();