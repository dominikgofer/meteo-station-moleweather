#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <esp_timer.h>
#include <esp_random.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

// Biblioteki sensorów i kart
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "RTClib.h"
#include <PMS.h>
#include <TinyGPSPlus.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

// ============================================================
// Configuration
// ============================================================

const char *AP_SSID = "ognistyzuczek";
const char *AP_PASSWORD = "123456789";

constexpr bool USE_MOCK_DATA = false;
constexpr bool DEBUG_ENABLED = true;
constexpr bool DEBUG_USB = false;

constexpr uint32_t SAMPLE_INTERVAL_MS = 5000;
constexpr size_t HISTORY_SIZE = 120; // Około 10 minut historii

constexpr size_t LOG_LINES = 32;
constexpr size_t LOG_LINE_LENGTH = 160;

// Piny dla karty SD (SPI)[cite: 1]
#define SD_CS   18
#define SD_MOSI 22
#define SD_MISO 23
#define SD_SCK  21

// Piny dla magistrali I2C (wspólna dla BME280 oraz DS3231)
#define I2C_SDA 19
#define I2C_SCL 20

// Piny dla czujnika PMS7003 (Serial1) - RX = 5, TX = 4
#define PMS_RX_PIN 5
#define PMS_TX_PIN 4

// Piny dla modulu GPS (Serial0) - RX = 17, TX = 16
#define GPS_RX_PIN 17
#define GPS_TX_PIN 16

// Piny dla czujników analogowych O3 i NO2
#define O3_PIN  1
#define NO2_PIN 2

// Pin wbudowanej diody LED sygnalizującej błąd zapisu na SD
#define LED_PIN LED_BUILTIN 

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;
RTC_DS3231 rtc;
PMS pms(Serial1);
PMS::DATA pmsData;
TinyGPSPlus gps;

bool bmeReady = false;
bool rtcReady = false;
bool sdReady = false;

const char* fileName = "/dane_pelne_system.csv";
unsigned long lastMessage = 0;

// ============================================================
// Data model
// ============================================================

enum Metric : uint8_t
{
	AIR_TEMP,	// Celsius
	HUMIDITY,	// Percent
	PRESSURE,	// hPa
	PROBE_TEMP, // Celsius
	PM1,		// Micrograms / cubic metre
	PM25,		// Micrograms / cubic metre
	PM10,		// Micrograms / cubic metre
	O3,			// ppb
	NO2,		// ppm
	METRIC_COUNT
};

struct Reading
{
	uint32_t id = 0;
	uint64_t ms = 0;  
	uint32_t utc = 0; 

	float v[METRIC_COUNT];

	double lat = NAN;
	double lon = NAN;
	float alt = NAN; 
	bool fix = false;
	uint8_t sats = 0;

	Reading()
	{
		for (size_t i = 0; i < METRIC_COUNT; i++)
		{
			v[i] = NAN;
		}
	}
};

void readBME280(Reading &r);
void readProbe(Reading &r);
void readParticles(Reading &r);
void readOzone(Reading &r);
void readNO2(Reading &r);
void readGPS(Reading &r);
void onNewReading(const Reading &r);
void appendReadingJSON(String &out, const Reading &r);

WebServer server(80);

Reading history[HISTORY_SIZE];
size_t historyHead = 0;
size_t historyCount = 0;

uint32_t sequenceNumber = 0;
uint32_t lastSampleAt = 0;

char logLines[LOG_LINES][LOG_LINE_LENGTH];
size_t logHead = 0;
size_t logCount = 0;

char bootId[9];
bool apReady = false;

// ============================================================
// Helpers and debug output
// ============================================================

uint64_t uptimeMs()
{
	return static_cast<uint64_t>(esp_timer_get_time()) / 1000ULL;
}

String uint64Text(uint64_t value)
{
	char buffer[24];
	snprintf(buffer, sizeof(buffer), "%llu", static_cast<unsigned long long>(value));
	return String(buffer);
}

String jsonNumber(double value, uint8_t decimals)
{
	return isfinite(value)
			   ? String(value, (unsigned int)decimals)
			   : String("null");
}

void debugPrint(const String &message)
{
	if (!DEBUG_ENABLED) return;

	char *line = logLines[logHead];
	snprintf(line, LOG_LINE_LENGTH, "[%llu ms] %s", static_cast<unsigned long long>(uptimeMs()), message.c_str());

	logHead = (logHead + 1) % LOG_LINES;
	if (logCount < LOG_LINES) logCount++;
}

// ============================================================
// Sensor integration hooks
// ============================================================

void beginSensors()
{
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, LOW);

	// Konfiguracja ADC dla czujników analogowych O3 i NO2
	analogReadResolution(12);
	analogSetPinAttenuation(O3_PIN, ADC_11db);
	analogSetPinAttenuation(NO2_PIN, ADC_11db);

	// 1. Inicjalizacja magistrali I2C
	Wire.begin(I2C_SDA, I2C_SCL);

	// 2. Inicjalizacja BME280
	bmeReady = bme.begin(0x76, &Wire);
	debugPrint(bmeReady ? "BME280 ready" : "BME280 unavailable");

	// 3. Inicjalizacja RTC DS3231
	rtcReady = rtc.begin(&Wire);
	if (rtcReady)
	{
		if (rtc.lostPower())
		{
			debugPrint("RTC lost power, setting compile time");
			rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
		}
		debugPrint("RTC DS3231 ready");
	}
	else
	{
		debugPrint("RTC DS3231 unavailable");
	}

	// 4. Inicjalizacja PMS7003 na Serial1 (RX=5, TX=4)
	Serial1.begin(9600, SERIAL_8N1, PMS_RX_PIN, PMS_TX_PIN);
	debugPrint("PMS7003 initialized on Serial1 (RX=5, TX=4)");

	// 5. Inicjalizacja GPS na Serial0 (RX=17, TX=16)
	Serial0.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
	debugPrint("GPS initialized on Serial0 (RX=17, TX=16)");

	// 6. Inicjalizacja karty SD[cite: 1]
	SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
	sdReady = SD.begin(SD_CS, SPI, 1000000);
	if (!sdReady)
	{
		debugPrint("ERROR: Card Mount Failed");
		digitalWrite(LED_PIN, HIGH);
	}
	else
	{
		debugPrint("SD card mounted successfully");
		if (!SD.exists(fileName))
		{
			File file = SD.open(fileName, FILE_WRITE);
			if (file)
			{
				file.println("Data_Czas;Temperatura_C;Cisnienie_hPa;WysokoscBME_m;Wilgotnosc_%;PM1_0_ug3;PM2_5_ug3;PM10_0_ug3;O3_ppb;NO2_ppm;Szer_Geogr;Dlug_Geogr;Satelity;WysokoscGPS_m;Predkosc_kmh");
				file.close();
				debugPrint("Created new CSV file with headers");
			}
			else
			{
				debugPrint("ERROR: Failed to create CSV file");
				digitalWrite(LED_PIN, HIGH);
			}
		}
	}
}

void pollSensors()
{
	// Pobieranie danych z PMS7003 w tle
	if (pms.read(pmsData))
	{
		lastMessage = millis();
	}

	// Pobieranie danych z GPS w tle
	while (Serial0.available() > 0)
	{
		gps.encode(Serial0.read());
	}
}

void readBME280(Reading &r)
{
	if (bmeReady)
	{
		r.v[AIR_TEMP] = bme.readTemperature();
		r.v[HUMIDITY] = bme.readHumidity();
		r.v[PRESSURE] = bme.readPressure() / 100.0F;
	}
}

void readProbe(Reading &r)
{
	(void)r;
}

void readParticles(Reading &r)
{
	r.v[PM1] = static_cast<float>(pmsData.PM_AE_UG_1_0);
	r.v[PM25] = static_cast<float>(pmsData.PM_AE_UG_2_5);
	r.v[PM10] = static_cast<float>(pmsData.PM_AE_UG_10_0);
}

void readOzone(Reading &r)
{
	const int samples = 20;
	long sum = 0;
	for (int i = 0; i < samples; i++) {
		sum += analogRead(O3_PIN);
	}
	float adc = sum / (float)samples;
	float voltage = adc * 3.3f / 4095.0f;
	// Przeliczenie napięcia na ppb (możesz dostosować współczynnik do dokumentacji czujnika O3)
	r.v[O3] = voltage * 100.0f;
}

void readNO2(Reading &r)
{
	const int samples = 20;
	long sum = 0;
	for (int i = 0; i < samples; i++) {
		sum += analogRead(NO2_PIN);
	}
	float adc = sum / (float)samples;
	float voltage = adc * 3.3f / 4095.0f;
	// Przeliczenie napięcia na ppm (zgodnie ze specyfikacją czujnika NO2)
	r.v[NO2] = voltage * 0.5f;
}

void readGPS(Reading &r)
{
	if (gps.location.isValid() && gps.location.age() < 10000)
	{
		r.lat = gps.location.lat();
		r.lon = gps.location.lng();
		r.fix = true;
	}
	else
	{
		r.lat = NAN;
		r.lon = NAN;
		r.fix = false;
	}

	if (gps.altitude.isValid())
	{
		r.alt = gps.altitude.meters();
	}
	else
	{
		r.alt = NAN;
	}

	if (gps.satellites.isValid())
	{
		r.sats = gps.satellites.value();
	}
	else
	{
		r.sats = 0;
	}
}

uint32_t readClockUTC(uint64_t ms)
{
	(void)ms;
	if (rtcReady)
	{
		return rtc.now().unixtime();
	}
	return 0;
}

void onNewReading(const Reading &r)
{
	if (!sdReady)
	{
		digitalWrite(LED_PIN, HIGH);
		return;
	}

	char timeBuffer[30];
	if (rtcReady)
	{
		DateTime now = rtc.now();
		sprintf(timeBuffer, "%04d-%02d-%02d %02d:%02d:%02d", 
				now.year(), now.month(), now.day(), 
				now.hour(), now.minute(), now.second());
	}
	else
	{
		sprintf(timeBuffer, "Uptime_%llums", (unsigned long long)r.ms);
	}

	float temp = isfinite(r.v[AIR_TEMP]) ? r.v[AIR_TEMP] : 0.0f;
	float press = isfinite(r.v[PRESSURE]) ? r.v[PRESSURE] : 0.0f;
	float altBME = bmeReady ? bme.readAltitude(SEALEVELPRESSURE_HPA) : 0.0f;
	float hum = isfinite(r.v[HUMIDITY]) ? r.v[HUMIDITY] : 0.0f;
	int pm1 = isfinite(r.v[PM1]) ? static_cast<int>(r.v[PM1]) : 0;
	int pm25 = isfinite(r.v[PM25]) ? static_cast<int>(r.v[PM25]) : 0;
	int pm10 = isfinite(r.v[PM10]) ? static_cast<int>(r.v[PM10]) : 0;
	float o3 = isfinite(r.v[O3]) ? r.v[O3] : 0.0f;
	float no2 = isfinite(r.v[NO2]) ? r.v[NO2] : 0.0f;

	String latStr = r.fix ? String(r.lat, 6) : "Brak_Fixu";
	String lonStr = r.fix ? String(r.lon, 6) : "Brak_Fixu";
	String satStr = String(r.sats);
	String altGpsStr = isfinite(r.alt) ? String(r.alt, 2) : "Brak_Fixu";
	String speedStr = gps.speed.isValid() ? String(gps.speed.kmph(), 2) : "Brak_Fixu";

	String dataRow = String(timeBuffer) + ";" +
					 String(temp, 2) + ";" +
					 String(press, 2) + ";" +
					 String(altBME, 2) + ";" +
					 String(hum, 2) + ";" +
					 String(pm1) + ";" +
					 String(pm25) + ";" +
					 String(pm10) + ";" +
					 String(o3, 2) + ";" +
					 String(no2, 3) + ";" +
					 latStr + ";" +
					 lonStr + ";" +
					 satStr + ";" +
					 altGpsStr + ";" +
					 speedStr;

	File file = SD.open(fileName, FILE_APPEND);
	if (file)
	{
		file.println(dataRow);
		file.close();
		digitalWrite(LED_PIN, LOW); // Zapis powiódł się – dioda wygaszona
	}
	else
	{
		debugPrint("ERROR: Failed to append to CSV file on SD");
		digitalWrite(LED_PIN, HIGH); // Błąd zapisu – włącza się dioda ostrzegawcza
	}
}

void takeSample()
{
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

	if (historyCount < HISTORY_SIZE)
	{
		historyCount++;
	}

	onNewReading(r);

	if (DEBUG_ENABLED)
	{
		debugPrint(
			"Sample #" + String(r.id) +
			" | air=" + String(r.v[AIR_TEMP], 1) +
			" C | PM2.5=" + String(r.v[PM25], 1) +
			" | O3=" + String(r.v[O3], 1) +
			" | NO2=" + String(r.v[NO2], 3));
	}
}

// ============================================================
// HTTP endpoints
// ============================================================

void appendReadingJSON(String &out, const Reading &r)
{
	out += "{\"id\":";
	out += String(r.id);
	out += ",\"ms\":";
	out += uint64Text(r.ms);
	out += ",\"utc\":";
	out += String(r.utc);
	out += ",\"v\":[";

	for (size_t i = 0; i < METRIC_COUNT; i++)
	{
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

void handleData()
{
	uint32_t after = 0;

	if (server.arg("boot") == String(bootId))
	{
		after = static_cast<uint32_t>(strtoul(server.arg("after").c_str(), nullptr, 10));
	}

	if (after > sequenceNumber) after = 0;

	const size_t oldest = (historyHead + HISTORY_SIZE - historyCount) % HISTORY_SIZE;
	size_t rowsToSend = 0;

	for (size_t i = 0; i < historyCount; i++)
	{
		if (history[(oldest + i) % HISTORY_SIZE].id > after) rowsToSend++;
	}

	String out;
	if (!out.reserve(1024 + rowsToSend * 280))
	{
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
	for (size_t i = 0; i < historyCount; i++)
	{
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

void handleLogs()
{
	if (!DEBUG_ENABLED)
	{
		server.send(404, "text/plain", "Debug disabled");
		return;
	}

	String out;
	if (!out.reserve(logCount * LOG_LINE_LENGTH + 1))
	{
		server.send(503, "text/plain", "Not enough memory");
		return;
	}

	const size_t oldest = (logHead + LOG_LINES - logCount) % LOG_LINES;
	for (size_t i = 0; i < logCount; i++)
	{
		out += logLines[(oldest + i) % LOG_LINES];
		out += '\n';
	}

	server.sendHeader("Cache-Control", "no-store");
	server.send(200, "text/plain; charset=utf-8", out);
}

// ============================================================
// Complete offline web interface
// ============================================================

const char INDEX_HTML[] PROGMEM = R"WEATHER(
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Portable Weather Station</title>
<style>
  * { box-sizing: border-box; }
  body {
    margin: 0;
    background: #eef2f5;
    color: #203040;
    font: 15px system-ui, sans-serif;
  }
  main {
    max-width: 1100px;
    margin: auto;
    padding: 18px;
  }
  h1 { font-size: 25px; margin: 0 0 10px; }
  h2 { font-size: 18px; margin: 0 0 14px; }
  p { line-height: 1.5; }
  #status { font-weight: 650; color: #17633b; }
  #status.warning { color: #925400; }
  #sample, .muted { color: #536477; font-size: 13px; }
  #cards {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(145px, 1fr));
    gap: 12px;
    margin: 18px 0;
  }
  article, section, details {
    background: white;
    border: 1px solid #dce3e8;
    border-radius: 10px;
    padding: 16px;
  }
  article span { display: block; min-height: 35px; color: #536477; }
  article strong { display: block; margin: 8px 0; font-size: 29px; }
  article small { color: #536477; }
  section, details { margin-bottom: 16px; }
  select {
    max-width: 100%;
    font: inherit;
    padding: 7px 10px;
    border: 1px solid #bac7d1;
    border-radius: 6px;
    background: white;
    color: #203040;
  }
  #chart { display: block; width: 100%; height: 250px; margin-top: 12px; }
  summary { cursor: pointer; font-weight: 650; }
  pre {
    background: #14212c;
    color: #d9eddf;
    padding: 12px;
    border-radius: 6px;
    max-height: 280px;
    overflow: auto;
    white-space: pre-wrap;
    font-size: 12px;
  }
</style>
</head>
<body>
<main>
  <h1>Portable Weather Station</h1>
  <p id="status" role="status">Connecting to the station...</p>
  <p id="sample">Waiting for the first sample.</p>

  <div id="cards"></div>

  <section>
    <h2>Recent measurements</h2>
    <label for="metric">Measurement:</label>
    <select id="metric"></select>
    <canvas id="chart" aria-label="Measurement history"></canvas>
    <p id="chartNote" class="muted"></p>
  </section>

  <section>
    <h2>Station location</h2>
    <p id="gps">Waiting for GPS fix...</p>
  </section>

  <details id="debug" hidden>
    <summary>Debug console</summary>
    <p id="debugStats" class="muted"></p>
    <pre id="console">Waiting for logs...</pre>
  </details>
</main>

<script>
'use strict';
const $ = id => document.getElementById(id);

const metrics = [
  ['Air temperature', '°C', 1],
  ['Humidity', '%', 1],
  ['Pressure', 'hPa', 1],
  ['Probe temperature', '°C', 1],
  ['PM1.0', 'µg/m³', 1],
  ['PM2.5', 'µg/m³', 1],
  ['PM10', 'µg/m³', 1],
  ['Ozone O3', 'ppb', 1],
  ['Nitrogen dioxide NO2', 'ppm', 3]
];

metrics.forEach((m, i) => {
  const card = document.createElement('article');
  card.innerHTML = '<span>' + m[0] + '</span><strong id="v' + i + '">--</strong><small>' + m[1] + '</small>';
  $('cards').appendChild(card);
  $('metric').add(new Option(m[0] + ' (' + m[1] + ')', i));
});

let readings = [], boot = '', lastId = 0, info = null;
let receivedAt = 0, online = false;

const latest = () => readings[readings.length - 1];
const value = (v, digits) => Number.isFinite(v) ? v.toFixed(digits) : '--';

function uptime(ms) {
  const s = Math.floor(ms / 1000);
  return Math.floor(s / 3600) + ':' + String(Math.floor(s / 60) % 60).padStart(2, '0') + ':' + String(s % 60).padStart(2, '0');
}

function paintStatus() {
  if (!info || !latest()) return;
  const age = Math.max(0, info.uptimeMs - latest().ms + performance.now() - receivedAt);
  const stale = !online || age > Math.max(10000, 3 * info.sampleMs);

  $('status').textContent = 'SENSOR DATA | ' + (stale ? 'STALE / reconnecting' : 'Connected') + ' | sample age ' + (age / 1000).toFixed(1) + ' s';
  $('status').className = stale ? 'warning' : '';
}

async function request(path, asText = false) {
  const controller = new AbortController();
  const timer = setTimeout(() => controller.abort(), 8000);
  try {
    const response = await fetch(path, { cache: 'no-store', signal: controller.signal });
    if (!response.ok) throw new Error('HTTP ' + response.status);
    return await (asText ? response.text() : response.json());
  } finally {
    clearTimeout(timer);
  }
}

async function refresh() {
  try {
    const d = await request('/api/data?boot=' + boot + '&after=' + lastId);
    if (boot !== d.boot) {
      readings = [];
      lastId = 0;
      boot = d.boot;
    }
    d.rows.forEach(r => {
      if (r.id > lastId) {
        readings.push(r);
        lastId = r.id;
      }
    });
    readings = readings.slice(-d.capacity);
    info = d;
    receivedAt = performance.now();
    online = true;

    paintStatus();
    const r = latest();
    if (r) {
      metrics.forEach((m, i) => {
        $('v' + i).textContent = value(r.v[i], m[2]);
      });
      $('sample').textContent = 'Sample #' + r.id + ' | uptime ' + uptime(r.ms) + ' | ' + (r.utc ? new Date(r.utc * 1000).toISOString().replace('T', ' ').replace('.000Z', ' UTC') : 'RTC unavailable');
      
      $('gps').textContent = r.fix
        ? 'Station GPS: ' + value(r.lat, 6) + ', ' + value(r.lon, 6) + ' | altitude ' + value(r.alt, 1) + ' m | satellites ' + r.sats
        : 'Station GPS: no current fix | satellites ' + r.sats;
    }

    $('debug').hidden = !d.debug;
    $('debugStats').textContent = 'Free heap: ' + d.freeHeap + ' bytes | Wi-Fi clients: ' + d.clients;
    drawChart();

    if (d.debug && $('debug').open) {
      try {
        $('console').textContent = await request('/api/logs', true);
      } catch (e) {
        $('console').textContent = 'Log request failed: ' + e.message;
      }
    }
  } catch (e) {
    online = false;
    $('status').textContent = 'Disconnected: ' + e.message + '; retrying...';
    paintStatus();
  } finally {
    setTimeout(refresh, 2000);
  }
}

function drawChart() {
  const canvas = $('chart');
  const w = Math.max(1, canvas.clientWidth);
  const h = Math.max(1, canvas.clientHeight);
  const ratio = Math.min(window.devicePixelRatio || 1, 2);

  canvas.width = Math.round(w * ratio);
  canvas.height = Math.round(h * ratio);
  const ctx = canvas.getContext('2d');
  ctx.setTransform(ratio, 0, 0, ratio, 0, 0);
  ctx.font = '12px sans-serif';

  const k = Number($('metric').value);
  const good = readings.filter(r => Number.isFinite(r.v[k]));

  if (!good.length) {
    ctx.fillText('No valid readings yet', 20, 35);
    return;
  }

  let lo = Math.min(...good.map(r => r.v[k]));
  let hi = Math.max(...good.map(r => r.v[k]));
  const pad = Math.max((hi - lo) * 0.12, Math.abs(hi) * 0.005, 0.001);
  lo -= pad;
  hi += pad;

  const left = 64, top = 15;
  const pw = Math.max(1, w - left - 15);
  const ph = h - 48;
  const firstMs = readings[0].ms;
  const lastMs = Math.max(firstMs + 1, latest().ms);

  const x = r => left + (r.ms - firstMs) / (lastMs - firstMs) * pw;
  const y = r => top + (hi - r.v[k]) / (hi - lo) * ph;

  for (let i = 0; i <= 4; i++) {
    const yy = top + i * ph / 4;
    ctx.strokeStyle = '#dce3e8';
    ctx.beginPath();
    ctx.moveTo(left, yy);
    ctx.lineTo(left + pw, yy);
    ctx.stroke();

    ctx.fillStyle = '#435161';
    ctx.textAlign = 'right';
    ctx.fillText((hi - i * (hi - lo) / 4).toFixed(metrics[k][2]), left - 8, yy + 4);
  }

  ctx.strokeStyle = '#126caa';
  ctx.fillStyle = '#126caa';
  ctx.lineWidth = 2;
  ctx.beginPath();

  let connected = false;
  readings.forEach(r => {
    if (!Number.isFinite(r.v[k])) {
      connected = false;
      return;
    }
    if (!connected) {
      ctx.moveTo(x(r), y(r));
    } else {
      ctx.lineTo(x(r), y(r));
    }
    connected = true;
  });
  ctx.stroke();

  good.forEach(r => {
    ctx.beginPath();
    ctx.arc(x(r), y(r), 2, 0, Math.PI * 2);
    ctx.fill();
  });
}

$('metric').addEventListener('change', drawChart);
window.addEventListener('resize', drawChart);
setInterval(paintStatus, 1000);
drawChart();
refresh();
</script>
</body>
</html>
)WEATHER";

// ============================================================
// Arduino entry points
// ============================================================

void setup()
{
#if ARDUINO_USB_CDC_ON_BOOT && ARDUINO_USB_MODE
	if (DEBUG_ENABLED && DEBUG_USB)
	{
		Serial.begin(115200);
		Serial.setTxTimeoutMs(0);
	}
#endif

	snprintf(bootId, sizeof(bootId), "%08lx", static_cast<unsigned long>(esp_random()));

	debugPrint("Weather station starting");

	beginSensors();

	takeSample();
	lastSampleAt = millis();

	WiFi.mode(WIFI_AP);

	const IPAddress ip(192, 168, 4, 1);
	const IPAddress mask(255, 255, 255, 0);

	apReady = WiFi.softAPConfig(ip, ip, mask);
	if (apReady)
	{
		apReady = WiFi.softAP(AP_SSID, AP_PASSWORD, 6, false, 4);
	}

	if (!apReady)
	{
		debugPrint("Wi-Fi AP initialization failed");
		return;
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
}

void loop()
{
	pollSensors();

	const uint32_t now = millis();

	if (static_cast<uint32_t>(now - lastSampleAt) >= SAMPLE_INTERVAL_MS)
	{
		lastSampleAt = now;
		takeSample();
	}

	if (apReady)
	{
		server.handleClient();
	}

	pollSensors();
	delay(1);
}