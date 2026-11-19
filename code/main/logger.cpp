#include "logger.h"
#include "config.h"
#include <esp_timer.h>
#include <math.h>
#include <stdio.h>

char bootId[9] = {0};

static char logLines[LOG_LINES][LOG_LINE_LENGTH];
static size_t logHead = 0;
static size_t logCount = 0;

uint64_t uptimeMs() {
  return static_cast<uint64_t>(esp_timer_get_time()) / 1000ULL;
}

String uint64Text(uint64_t value) {
  char buffer[24];
  snprintf(buffer, sizeof(buffer), "%llu", static_cast<unsigned long long>(value));
  return String(buffer);
}

String jsonNumber(double value, uint8_t decimals) {
  return isfinite(value) ? String(value, static_cast<unsigned int>(decimals)) : String("null");
}

void debugPrint(const String &message) {
  if (!DEBUG_ENABLED) return;

  char *line = logLines[logHead];
  snprintf(line, LOG_LINE_LENGTH, "[%llu ms] %s",
           static_cast<unsigned long long>(uptimeMs()), message.c_str());

  logHead = (logHead + 1) % LOG_LINES;
  if (logCount < LOG_LINES) logCount++;
}

String getDebugLogsText() {
  String out;
  if (!out.reserve(logCount * LOG_LINE_LENGTH + 1)) {
    return String();
  }

  const size_t oldest = (logHead + LOG_LINES - logCount) % LOG_LINES;
  for (size_t i = 0; i < logCount; i++) {
    out += logLines[(oldest + i) % LOG_LINES];
    out += '\n';
  }
  return out;
}