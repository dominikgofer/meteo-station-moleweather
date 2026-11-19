#pragma once
#include <pgmspace.h>

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