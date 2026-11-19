#include "web_server.h"
#include "config.h"
#include "logger.h"
#include "web_ui.h"

#include <WiFi.h>
#include <WebServer.h>
#include <stdlib.h>

static WebServer server(80);
static bool apReady = false;

static const Reading *history = nullptr;
static const size_t *historyCount = nullptr;
static const size_t *historyHead = nullptr;
static const uint32_t *sequenceNumber = nullptr;

// Deklaracje wyprzedzające (forward declarations)
static void appendReadingJSON(String &out, const Reading &r);
static void handleData();
static void handleLogs();

bool isAccessPointReady() {
  return apReady;
}

void initWebServerData(const Reading *historyBuf,
                       const size_t *countPtr,
                       const size_t *headPtr,
                       const uint32_t *seqPtr) {
  history = historyBuf;
  historyCount = countPtr;
  historyHead = headPtr;
  sequenceNumber = seqPtr;
}

bool startAccessPoint() {
  if (apReady) {
    return true;
  }

  WiFi.mode(WIFI_AP); 

  const IPAddress ip(192, 168, 4, 1); 
  const IPAddress mask(255, 255, 255, 0); 

  apReady = WiFi.softAPConfig(ip, ip, mask); 
  if (apReady) { 
    apReady = WiFi.softAP(AP_SSID, AP_PASSWORD, 6, false, 4); 
  }

  if (!apReady) { 
    debugPrint("Wi-Fi AP initialization failed"); 
    return false;
  }

  server.on("/", HTTP_GET, []() { 
    server.sendHeader("Cache-Control", "no-store"); 
    server.send_P(200, "text/html; charset=utf-8", INDEX_HTML); 
  });

  server.on("/api/data", HTTP_GET, handleData); 
  server.on("/api/logs", HTTP_GET, handleLogs); 
  server.on("/favicon.ico", HTTP_GET, []() { server.send(204, "text/plain", ""); }); 
  server.onNotFound([]() { server.send(404, "text/plain", "Page not found"); }); 

  server.begin(); 

  debugPrint("Wi-Fi network: " + String(AP_SSID)); 
  debugPrint("Open http://" + WiFi.softAPIP().toString()); 
  return true;
}

void loopWebServer() {
  if (apReady) { 
    server.handleClient(); 
  }
}

void stopAccessPoint() {
  if (!apReady) {
    return;
  }

  server.stop();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
  apReady = false;
  debugPrint("Wi-Fi AP stopped");
}
static void appendReadingJSON(String &out, const Reading &r) {
  out += "{\"id\":";
  out += String(r.id);
  out += ",\"ms\":";
  out += uint64Text(r.ms);
  out += ",\"utc\":";
  out += String(r.utc);
  out += ",\"v\":[";

  for (size_t i = 0; i < METRIC_COUNT; i++) {
    if (i) out += ',';
    out += jsonNumber(r.v[i], 3);
  }

  out += "],\"lat\":";
  out += jsonNumber(r.lat, 6);
  out += ",\"lon\":";
  out += jsonNumber(r.lon, 6);
  out += ",\"alt\":";
  out += jsonNumber(r.alt, 1);
  out += ",\"fix\":";
  out += (r.fix ? "true" : "false");
  out += ",\"sats\":";
  out += String(static_cast<unsigned int>(r.sats));
  out += '}';
}

static void handleData() {
  uint32_t after = 0;

  if (server.arg("boot") == String(bootId)) {
    after = static_cast<uint32_t>(strtoul(server.arg("after").c_str(), nullptr, 10));
  }

  const uint32_t currentSeq = (sequenceNumber != nullptr) ? *sequenceNumber : 0;
  const size_t count = (historyCount != nullptr) ? *historyCount : 0;
  const size_t head = (historyHead != nullptr) ? *historyHead : 0;

  if (after > currentSeq) after = 0;

  const size_t oldest = (head + HISTORY_SIZE - count) % HISTORY_SIZE;
  size_t rowsToSend = 0;

  for (size_t i = 0; i < count; i++) {
    if (history[(oldest + i) % HISTORY_SIZE].id > after) rowsToSend++;
  }

  String out;
  if (!out.reserve(1024 + rowsToSend * 280)) {
    server.send(503, "text/plain", "Not enough memory");
    return;
  }

  out += "{\"boot\":\"";
  out += bootId;
  out += "\",\"mock\":";
  out += (USE_MOCK_DATA ? "true" : "false");
  out += ",\"debug\":";
  out += (DEBUG_ENABLED ? "true" : "false");
  out += ",\"sampleMs\":";
  out += String(SAMPLE_INTERVAL_MS);
  out += ",\"capacity\":";
  out += String(static_cast<unsigned int>(HISTORY_SIZE));
  out += ",\"uptimeMs\":";
  out += uint64Text(uptimeMs());
  out += ",\"freeHeap\":";
  out += String(ESP.getFreeHeap());
  out += ",\"clients\":";
  out += String(WiFi.softAPgetStationNum());
  out += ",\"rows\":[";

  bool first = true;
  for (size_t i = 0; i < count; i++) {
    const Reading &r = history[(oldest + i) % HISTORY_SIZE];
    if (r.id <= after) continue;

    if (!first) out += ',';
    first = false;

    appendReadingJSON(out, r);
  }

  out += "]}";

  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json; charset=utf-8", out);
}

static void handleLogs() {
  if (!DEBUG_ENABLED) {
    server.send(404, "text/plain", "Debug disabled");
    return;
  }

  String out = getDebugLogsText();
  const size_t count = (historyCount != nullptr) ? *historyCount : 0;
  if (out.length() == 0 && count > 0) {
    server.send(503, "text/plain", "Not enough memory");
    return;
  }

  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "text/plain; charset=utf-8", out);
}