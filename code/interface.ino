#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <esp_timer.h>
#include <esp_random.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

// ============================================================
// Configuration
// ============================================================

const char *AP_SSID = "ognistyzuczek";
const char *AP_PASSWORD = "123456789";

constexpr bool USE_MOCK_DATA = true;
constexpr bool DEBUG_ENABLED = true;

// Optional debug output over native USB.
// Browser logging works independently of this option.
constexpr bool DEBUG_USB = false;

constexpr uint32_t SAMPLE_INTERVAL_MS = 5000;
constexpr size_t HISTORY_SIZE = 120; // About 10 minutes

constexpr size_t LOG_LINES = 32;
constexpr size_t LOG_LINE_LENGTH = 160;

// Mock GPS position: Warsaw.
constexpr double MOCK_LATITUDE = 52.2297;
constexpr double MOCK_LONGITUDE = 21.0122;

// Mock clock starts at 2026-01-01 00:00:00 UTC.
constexpr uint32_t MOCK_START_UTC = 1767225600UL;

// ============================================================
// Data model
//
// Keep this order synchronized with the JavaScript metrics array.
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
	uint64_t ms = 0;  // Monotonic uptime
	uint32_t utc = 0; // UTC Unix seconds; 0 = unavailable

	float v[METRIC_COUNT];

	double lat = NAN;
	double lon = NAN;
	float alt = NAN; // GPS altitude in metres
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

// Explicit declarations prevent Arduino auto-prototype issues
// with functions that use the Reading structure.
void readBME280(Reading &r);
void readProbe(Reading &r);
void readParticles(Reading &r);
void readOzone(Reading &r);
void readNO2(Reading &r);
void readGPS(Reading &r);
void onNewReading(const Reading &r);
void appendReadingJSON(String &out, const Reading &r);

// Add real sensor includes and global objects here later.
//
// Example:
// #include <Wire.h>
// #include <Adafruit_BME280.h>
// Adafruit_BME280 bme;
// bool bmeReady = false;

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

	snprintf(
		buffer, sizeof(buffer), "%llu",
		static_cast<unsigned long long>(value));

	return String(buffer);
}

String jsonNumber(double value, uint8_t decimals)
{
	// NaN and Infinity are not valid JSON numbers.
	return isfinite(value)
			   ? String(value, decimals)
			   : String("null");
}

// Call from setup(), loop(), or functions called by them.
// Stores a bounded browser log without using either sensor UART.
void debugPrint(const String &message)
{
	if (!DEBUG_ENABLED)
	{
		return;
	}

	char *line = logLines[logHead];

	snprintf(
		line, LOG_LINE_LENGTH, "[%llu ms] %s",
		static_cast<unsigned long long>(uptimeMs()),
		message.c_str());

	logHead = (logHead + 1) % LOG_LINES;

	if (logCount < LOG_LINES)
	{
		logCount++;
	}

	// Only use Serial when it represents native hardware USB CDC.
	// There is no UART fallback.
#if ARDUINO_USB_CDC_ON_BOOT && ARDUINO_USB_MODE
	if (DEBUG_USB && Serial)
	{
		const size_t length = strlen(line);

		if (Serial.availableForWrite() >= static_cast<int>(length + 2))
		{
			Serial.println(line);
		}
	}
#endif
}

// ============================================================
// Sensor integration hooks
// ============================================================

void beginSensors()
{
	// Initialize your sensors here.
	// Avoid waiting forever when a sensor is unavailable.
	//
	// BME280 example after adding its includes and globals:
	//
	// if (!USE_MOCK_DATA) {
	//   Wire.begin(SDA, SCL);
	//   bmeReady = bme.begin(0x76);
	//   debugPrint(bmeReady ? "BME280 ready" : "BME280 unavailable");
	// }
	//
	// Initialize your sensor UARTs here using your actual GPIOs.
	// Serial0 and Serial1 are the two normal hardware UARTs.
	// Set larger RX buffers before begin() if required.

	debugPrint(
		USE_MOCK_DATA
			? "Mock sensor mode enabled"
			: "Sensor mode enabled; implement the real sensor hooks");
}

void pollSensors()
{
	// Called frequently from loop().
	//
	// Parse incoming GPS and particulate-sensor UART bytes here.
	// Cache decoded readings together with their receive times.
	//
	// Example with TinyGPSPlus:
	//
	// while (Serial0.available()) {
	//   gps.encode(Serial0.read());
	// }
	//
	// Also manage asynchronous DS18B20 conversions here.
	// Avoid delay() and waiting loops.
}

void readBME280(Reading &r)
{
	if (!USE_MOCK_DATA)
	{
		// Example after integrating the Adafruit BME280 library:
		//
		// if (bmeReady) {
		//   r.v[AIR_TEMP] = bme.readTemperature();
		//   r.v[HUMIDITY] = bme.readHumidity();
		//   r.v[PRESSURE] = bme.readPressure() / 100.0F;
		// }
		//
		// Unassigned values remain NAN and display as unavailable.
		return;
	}

	const float t = r.ms / 1000.0F;

	r.v[AIR_TEMP] = 21.0F + 2.0F * sinf(t / 35.0F);
	r.v[HUMIDITY] = 55.0F + 8.0F * sinf(t / 45.0F);
	r.v[PRESSURE] = 1013.0F + 2.0F * sinf(t / 70.0F);
}

void readProbe(Reading &r)
{
	if (!USE_MOCK_DATA)
	{
		// Copy a completed DS18B20 conversion here.
		// Convert disconnected/error readings to NAN.
		return;
	}

	r.v[PROBE_TEMP] =
		18.0F + sinf((r.ms / 1000.0F) / 50.0F);
}

void readParticles(Reading &r)
{
	if (!USE_MOCK_DATA)
	{
		// Copy a fresh, checksum-validated PMS frame here.
		// Use atmospheric/environmental mass-concentration fields.
		// Leave values NAN if the cached frame is too old.
		return;
	}

	const float wave = sinf((r.ms / 1000.0F) / 30.0F);

	r.v[PM1] = 6.0F + 2.0F * wave;
	r.v[PM25] = 10.0F + 4.0F * wave;
	r.v[PM10] = 16.0F + 5.0F * wave;
}

void readOzone(Reading &r)
{
	if (!USE_MOCK_DATA)
	{
		// Assign a valid O3 concentration in ppb.
		// Multiply by 1000 if your library returns ppm.
		return;
	}

	r.v[O3] =
		35.0F + 8.0F * sinf((r.ms / 1000.0F) / 40.0F);
}

void readNO2(Reading &r)
{
	if (!USE_MOCK_DATA)
	{
		// Assign your calibrated NO2 concentration in ppm.
		// An ADC voltage alone is not a concentration.
		return;
	}

	r.v[NO2] =
		0.150F + 0.025F * sinf((r.ms / 1000.0F) / 65.0F);
}

void readGPS(Reading &r)
{
	if (!USE_MOCK_DATA)
	{
		// Copy GPS values only when the fix is valid and recent.
		// Otherwise leave r.fix false.
		//
		// r.lat = ...;   // Decimal degrees
		// r.lon = ...;   // Decimal degrees
		// r.alt = ...;   // Metres
		// r.sats = ...;
		// r.fix = ...;
		return;
	}

	const double t = r.ms / 1000.0;

	r.lat = MOCK_LATITUDE + 0.0003 * sin(t / 60.0);
	r.lon = MOCK_LONGITUDE + 0.0003 * cos(t / 60.0);
	r.alt = 110.0F;
	r.sats = 8;
	r.fix = true;
}

uint32_t readClockUTC(uint64_t ms)
{
	if (!USE_MOCK_DATA)
	{
		// Return UTC Unix seconds from your DS3231.
		// Return 0 if its time is unavailable or invalid.
		return 0;
	}

	return MOCK_START_UTC + static_cast<uint32_t>(ms / 1000ULL);
}

void onNewReading(const Reading &r)
{
	// Call your existing SD measurement logger here.
	// This hook runs once per collected sample.
	(void)r;
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

	r.fix = r.fix &&
			isfinite(r.lat) && isfinite(r.lon) &&
			r.lat >= -90.0 && r.lat <= 90.0 &&
			r.lon >= -180.0 && r.lon <= 180.0;

	if (!r.fix)
	{
		r.lat = NAN;
		r.lon = NAN;
		r.alt = NAN;
	}

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
			" C | PM2.5=" + String(r.v[PM25], 1));
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
		if (i)
			out += ',';
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

	// The browser requests only samples it has not received yet.
	// A changed boot ID resets its history after an ESP restart.
	if (server.arg("boot") == String(bootId))
	{
		after = static_cast<uint32_t>(
			strtoul(server.arg("after").c_str(), nullptr, 10));
	}

	if (after > sequenceNumber)
	{
		after = 0;
	}

	const size_t oldest =
		(historyHead + HISTORY_SIZE - historyCount) % HISTORY_SIZE;

	size_t rowsToSend = 0;

	for (size_t i = 0; i < historyCount; i++)
	{
		if (history[(oldest + i) % HISTORY_SIZE].id > after)
		{
			rowsToSend++;
		}
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

		if (r.id <= after)
			continue;

		if (!first)
			out += ',';
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

	const size_t oldest =
		(logHead + LOG_LINES - logCount) % LOG_LINES;

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

  #sample, .muted {
    color: #536477;
    font-size: 13px;
  }

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

  article span {
    display: block;
    min-height: 35px;
    color: #536477;
  }

  article strong {
    display: block;
    margin: 8px 0;
    font-size: 29px;
  }

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

  #chart {
    display: block;
    width: 100%;
    height: 250px;
    margin-top: 12px;
  }

  summary { cursor: pointer; font-weight: 650; }

  pre {
    background: #14212c;
    color: #d9eddf;
    padding: 12px;
    border-radius: 6px;
    max-height: 280px;
    overflow: auto;
    white-space: pre-wrap;
    overflow-wrap: anywhere;
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
    <p class="muted">
      Unavailable readings appear as -- and create gaps in the chart.
    </p>
  </section>

  <section>
    <h2>Station location</h2>
    <p id="gps">Waiting for GPS.</p>
    <p class="muted">Coordinates come from the station GPS module.</p>
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

// Same order as the C++ Metric enum.
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

  card.innerHTML =
    '<span>' + m[0] + '</span><strong id="v' + i +
    '">--</strong><small>' + m[1] + '</small>';

  $('cards').appendChild(card);
  $('metric').add(new Option(m[0] + ' (' + m[1] + ')', i));
});

let readings = [], boot = '', lastId = 0, info = null;
let receivedAt = 0, online = false;

const latest = () => readings[readings.length - 1];
const value = (v, digits) =>
  Number.isFinite(v) ? v.toFixed(digits) : '--';

function uptime(ms) {
  const s = Math.floor(ms / 1000);

  return Math.floor(s / 3600) + ':' +
    String(Math.floor(s / 60) % 60).padStart(2, '0') + ':' +
    String(s % 60).padStart(2, '0');
}

function paintStatus() {
  if (!info || !latest()) return;

  const age = Math.max(
    0,
    info.uptimeMs - latest().ms + performance.now() - receivedAt
  );

  const stale =
    !online || age > Math.max(10000, 3 * info.sampleMs);

  $('status').textContent =
    (info.mock ? 'MOCK DATA' : 'SENSOR DATA') +
    ' | ' + (stale ? 'STALE / reconnecting' : 'Connected') +
    ' | sample age ' + (age / 1000).toFixed(1) + ' s';

  $('status').className = stale || info.mock ? 'warning' : '';
}

async function request(path, asText = false) {
  const controller = new AbortController();
  const timer = setTimeout(() => controller.abort(), 8000);

  try {
    const response = await fetch(path, {
      cache: 'no-store',
      signal: controller.signal
    });

    if (!response.ok) {
      throw new Error('HTTP ' + response.status);
    }

    return await (
      asText ? response.text() : response.json()
    );
  } finally {
    clearTimeout(timer);
  }
}

async function refresh() {
  try {
    const d = await request(
      '/api/data?boot=' + boot + '&after=' + lastId
    );

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

      $('sample').textContent =
        'Sample #' + r.id +
        ' | uptime ' + uptime(r.ms) +
        ' | ' + (
          r.utc
            ? new Date(r.utc * 1000).toISOString()
                .replace('T', ' ')
                .replace('.000Z', ' UTC')
            : 'RTC unavailable'
        );

      $('gps').textContent = r.fix
        ? 'Station GPS: ' +
          value(r.lat, 6) + ', ' + value(r.lon, 6) +
          ' | altitude ' + value(r.alt, 1) +
          ' m | satellites ' + r.sats
        : 'Station GPS: no current fix | satellites ' + r.sats;
    }

    $('debug').hidden = !d.debug;

    $('debugStats').textContent =
      'Free heap: ' + d.freeHeap +
      ' bytes | Wi-Fi clients: ' + d.clients;

    drawChart();

    // Fetch logs only while the console is open.
    if (d.debug && $('debug').open) {
      try {
        // Render debug output as text, never as HTML.
        $('console').textContent =
          await request('/api/logs', true);
      } catch (e) {
        $('console').textContent =
          'Log request failed: ' + e.message;
      }
    }
  } catch (e) {
    online = false;

    $('status').textContent =
      'Disconnected: ' + e.message + '; retrying...';

    paintStatus();
  } finally {
    // Schedule after completion to avoid overlapping requests.
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

  $('chartNote').textContent =
    readings.length + ' samples | up to ' +
    (info ? info.capacity * info.sampleMs / 60000 : 0).toFixed(1) +
    ' minutes | x axis: uptime since boot';

  if (!good.length) {
    ctx.fillText('No valid readings yet', 20, 35);
    return;
  }

  let lo = Math.min(...good.map(r => r.v[k]));
  let hi = Math.max(...good.map(r => r.v[k]));

  const pad = Math.max(
    (hi - lo) * 0.12,
    Math.abs(hi) * 0.005,
    0.001
  );

  lo -= pad;
  hi += pad;

  const left = 64, top = 15;
  const pw = Math.max(1, w - left - 15);
  const ph = h - 48;

  const firstMs = readings[0].ms;
  const lastMs = Math.max(firstMs + 1, latest().ms);

  const x = r =>
    left + (r.ms - firstMs) / (lastMs - firstMs) * pw;

  const y = r =>
    top + (hi - r.v[k]) / (hi - lo) * ph;

  // Grid and vertical-axis labels.
  for (let i = 0; i <= 4; i++) {
    const yy = top + i * ph / 4;

    ctx.strokeStyle = '#dce3e8';
    ctx.beginPath();
    ctx.moveTo(left, yy);
    ctx.lineTo(left + pw, yy);
    ctx.stroke();

    ctx.fillStyle = '#435161';
    ctx.textAlign = 'right';

    ctx.fillText(
      (hi - i * (hi - lo) / 4).toFixed(metrics[k][2]),
      left - 8,
      yy + 4
    );
  }

  ctx.strokeStyle = '#126caa';
  ctx.fillStyle = '#126caa';
  ctx.lineWidth = 2;
  ctx.beginPath();

  let connected = false;
  let previous = null;

  readings.forEach(r => {
    if (!Number.isFinite(r.v[k])) {
      connected = false;
      previous = r;
      return;
    }

    const gap = previous && (
      r.id !== previous.id + 1 ||
      r.ms - previous.ms > info.sampleMs * 1.8
    );

    if (!connected || gap) {
      ctx.moveTo(x(r), y(r));
    } else {
      ctx.lineTo(x(r), y(r));
    }

    connected = true;
    previous = r;
  });

  ctx.stroke();

  // Points also make a single initial sample visible.
  good.forEach(r => {
    ctx.beginPath();
    ctx.arc(x(r), y(r), 2, 0, Math.PI * 2);
    ctx.fill();
  });

  ctx.fillStyle = '#435161';
  ctx.textAlign = 'left';
  ctx.fillText(uptime(firstMs), left, h - 8);

  ctx.textAlign = 'right';
  ctx.fillText(uptime(latest().ms), w - 15, h - 8);
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
		// Do not wait for a USB host.
	}
#endif

	snprintf(
		bootId, sizeof(bootId), "%08lx",
		static_cast<unsigned long>(esp_random()));

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
		apReady = WiFi.softAP(
			AP_SSID, AP_PASSWORD, 6, false, 4);
	}

	if (!apReady)
	{
		debugPrint("Wi-Fi AP initialization failed");
		return;
	}

	server.on("/", HTTP_GET, []()
			  {
    server.sendHeader("Cache-Control", "no-store");

    server.send_P(
      200, "text/html; charset=utf-8", INDEX_HTML
    ); });

	server.on("/api/data", HTTP_GET, handleData);
	server.on("/api/logs", HTTP_GET, handleLogs);

	server.on("/favicon.ico", HTTP_GET, []()
			  { server.send(204, "text/plain", ""); });

	server.onNotFound([]()
					  { server.send(404, "text/plain", "Page not found"); });

	server.begin();

	debugPrint("Wi-Fi network: " + String(AP_SSID));
	debugPrint("Open http://" + WiFi.softAPIP().toString());
}

void loop()
{
	pollSensors();

	const uint32_t now = millis();

	// Unsigned subtraction handles millis() rollover.
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