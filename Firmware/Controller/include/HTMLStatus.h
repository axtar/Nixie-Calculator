// HTMLStatus.h

// provides the status information page

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

const char htmlStatus[] = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
  <meta charset="UTF-8">
  <title>Nixie Calculator Status</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    :root {
      --bg: #f4f1ec;
      --card-bg: #ffffff;
      --border: #e2dccf;
      --text: #2b2620;
      --text-muted: #7a7266;
      --accent: #ff8c00;
      --accent-dark: #d97400;
      --header-bg: #2f6f9f;
      --radius: 8px;
    }

    * {
      box-sizing: border-box;
    }

    html {
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
      background: var(--bg);
      color: var(--text);
      text-align: center;
    }

    body {
      margin: 0;
      min-height: 100vh;
      display: flex;
      flex-direction: column;
    }

    h1 {
      margin: 0;
      font-size: 1.25rem;
      font-weight: 600;
      color: #fff;
      letter-spacing: .02em;
    }

    .headline {
      background: var(--header-bg);
      padding: 10px 16px;
      border-bottom: 3px solid var(--accent);
    }

    .content {
      max-width: 600px;
      width: 100%;
      box-sizing: border-box;
      margin: 12px auto;
      text-align: left;
      padding: 0 14px;
      background: var(--card-bg);
      border: 1px solid var(--border);
      border-radius: var(--radius);
      box-shadow: 0 1px 2px rgba(0, 0, 0, .04);
      overflow: hidden;
    }

    .row {
      display: flex;
      justify-content: space-between;
      gap: 10px;
      padding: 4px 2px;
    }

    .row+.row {
      border-top: 1px solid var(--border);
    }

    .row span:first-child {
      color: var(--text-muted);
      font-weight: 600;
    }

    #saveBar {
      flex: 0 0 auto;
      margin-top: auto;
      position: sticky;
      bottom: 0;
      text-align: center;
      padding: 14px 0;
      background: rgba(244, 241, 236, .92);
      backdrop-filter: blur(6px);
      border-top: 1px solid var(--border);
    }

    #backButton {
      display: inline-block;
      padding: 9px 28px;
      font-size: .95rem;
      font-weight: 600;
      text-decoration: none;
      background-color: var(--accent);
      border: 1px solid var(--accent);
      color: #fff;
      border-radius: var(--radius);
      transition: background-color .15s ease, border-color .15s ease;
    }

    #backButton:hover {
      background-color: var(--accent-dark);
      border-color: var(--accent-dark);
    }
  </style>
</head>

<body>
  <div class="headline">
    <h1>Nixie Calculator Status</h1>
  </div>
  <div class="content">
    <div class="row"><span>Controller Firmware</span><span id="controllerVersion">-</span></div>
    <div class="row"><span>Keyboard Firmware</span><span id="keyboardVersion">-</span></div>
    <div class="row"><span>MCU Chip</span><span id="chip">-</span></div>
    <div class="row"><span>MCU Speed</span><span id="cpuFreq">-</span></div>
    <div class="row"><span>Flash Size</span><span id="flashSize">-</span></div>
    <div class="row"><span>Board/MCU Temperature</span><span id="boardTemperature">-</span></div>
    <div class="row"><span>Free Heap</span><span id="freeHeap">-</span></div>
    <div class="row"><span>Heap Fragmentation</span><span id="heapFragmentation">-</span></div>
    <div class="row"><span>Last Reset Reason</span><span id="resetReason">-</span></div>
    <div class="row"><span>Uptime</span><span id="uptime">-</span></div>
    <div class="row"><span>Access Point IP</span><span id="apIP">-</span></div>
    <div class="row"><span>WiFi Clients</span><span id="wifiClients">-</span></div>
    <div class="row"><span>Device Mode</span><span id="deviceMode">-</span></div>
    <div class="row"><span>Calculator Mode</span><span id="calculatorMode">-</span></div>
    <div class="row"><span>Display Type</span><span id="displayType">-</span></div>
    <div class="row"><span>High Voltage</span><span id="highVoltageOn">-</span></div>
    <div class="row"><span>High Voltage On Time</span><span id="hvOnSeconds">-</span></div>
    <div class="row"><span>External Temperature</span><span id="externalTemperature">-</span></div>
    <div class="row"><span>PIR Presence Detected</span><span id="lastPresence">-</span></div>
    <div class="row"><span>Last GPS Time Sync</span><span id="lastGpsSync">-</span></div>
  </div>
  <div id="saveBar">
    <a href="/" id="backButton">Back</a>
  </div>
  <script>
    function formatUptime(seconds) {
      const days = Math.floor(seconds / 86400);
      const hours = Math.floor((seconds / 3600) % 24);
      const minutes = Math.floor((seconds / 60) % 60);
      return `${days}d ${hours}h ${minutes}m`;
    }
    function formatElapsed(seconds) {
      if (seconds < 60) {
        return `${seconds}s ago`;
      }
      if (seconds < 3600) {
        return `${Math.floor(seconds / 60)}m ago`;
      }
      if (seconds < 86400) {
        return `${Math.floor(seconds / 3600)}h ${Math.floor((seconds / 60) % 60)}m ago`;
      }
      return `${Math.floor(seconds / 86400)}d ${Math.floor((seconds / 3600) % 24)}h ago`;
    }
    function formatTemperature(value, unit) {
      if (value <= -255) {
        return "N/A";
      }
      return value.toFixed(2) + " °" + unit;
    }
    function refreshStatus() {
      fetch('/api/status')
        .then(response => response.json())
        .then(data => {
          document.getElementById('controllerVersion').textContent = data.controllerVersion;
          document.getElementById('keyboardVersion').textContent = data.keyboardVersion;
          document.getElementById('boardTemperature').textContent = formatTemperature(data.boardTemperature, data.temperatureUnit) +
            " / " + formatTemperature(data.mcuTemperature, data.temperatureUnit);
          document.getElementById('externalTemperature').textContent = formatTemperature(data.externalTemperature, data.temperatureUnit);
          document.getElementById('freeHeap').textContent = data.freeHeap + " (min " + data.minFreeHeap + ")";
          document.getElementById('heapFragmentation').textContent = data.heapFragmentation + "% (largest " + data.largestFreeBlock + ")";
          document.getElementById('uptime').textContent = formatUptime(data.uptime);
          document.getElementById('lastPresence').textContent = data.lastPresence < 0 ? "Never" : formatElapsed(data.lastPresence);
          document.getElementById('deviceMode').textContent = data.deviceMode;
          document.getElementById('calculatorMode').textContent = data.calculatorMode;
          document.getElementById('displayType').textContent = data.displayType;
          document.getElementById('highVoltageOn').textContent = data.highVoltageOn ? "On" : "Off";
          const hvPercent = data.uptime > 0 ? (data.hvOnSeconds / data.uptime * 100).toFixed(1) : "0.0";
          document.getElementById('hvOnSeconds').textContent = formatUptime(data.hvOnSeconds) + " (" + hvPercent + "%)";
          document.getElementById('wifiClients').textContent = data.wifiClients;
          document.getElementById('apIP').textContent = data.apIP;
          document.getElementById('resetReason').textContent = data.resetReason;
          document.getElementById('chip').textContent = data.chipModel + " rev " + data.chipRevision;
          document.getElementById('cpuFreq').textContent = data.cpuFreqMHz + " MHz";
          document.getElementById('flashSize').textContent = (data.flashSize / (1024 * 1024)).toFixed(0) + " MB";
          document.getElementById('lastGpsSync').textContent = data.lastGpsSync < 0 ? "Never" : formatElapsed(data.lastGpsSync);
        })
        .catch(() => { });
    }
    window.addEventListener('load', () => {
      refreshStatus();
      setInterval(refreshStatus, 2000);
    });
  </script>
</body>

</html>
)rawliteral";
