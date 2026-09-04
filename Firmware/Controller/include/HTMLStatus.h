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
    html {
      font-family: Arial, Helvetica, sans-serif;
      text-align: center;
    }

    h1 {
      font-size: 1.2rem;
      color: white;
    }

    .headline {
      overflow: hidden;
      background-color: #143642;
    }

    body {
      margin: 0;
    }

    .content {
      max-width: 600px;
      margin: 0 auto;
      text-align: left;
      padding-left: 5px;
      padding-right: 5px;
    }

    .row {
      display: flex;
      justify-content: space-between;
      gap: 10px;
      padding: 6px 4px;
      border-bottom: 1px solid #ccc;
    }

    .row span:first-child {
      color: #143642;
      font-weight: bold;
    }

    #backButton {
      display: inline-block;
      margin: 10px auto;
      padding: 8px 30px;
      font-size: 1rem;
      text-decoration: none;
      background-color: white;
      border: 1px solid #143642;
      color: #143642;
      border-radius: 4px;
    }

    #backButton:hover {
      background-color: #143642;
      color: white;
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
    <div class="row"><span>Flash Size</span><span id="flashSize">-</span></div>
    <div class="row"><span>Last Reset Reason</span><span id="resetReason">-</span></div>
    <div class="row"><span>Uptime</span><span id="uptime">-</span></div>
    <div class="row"><span>Free Heap</span><span id="freeHeap">-</span></div>
    <div class="row"><span>Minimum Free Heap</span><span id="minFreeHeap">-</span></div>
    <div class="row"><span>WiFi Clients</span><span id="wifiClients">-</span></div>
    <div class="row"><span>Access Point IP</span><span id="apIP">-</span></div>
    <div class="row"><span>Device Mode</span><span id="deviceMode">-</span></div>
    <div class="row"><span>Display Type</span><span id="displayType">-</span></div>
    <div class="row"><span>High Voltage</span><span id="highVoltageOn">-</span></div>
    <div class="row"><span>High Voltage On Time</span><span id="hvOnSeconds">-</span></div>
    <div class="row"><span>Board Temperature</span><span id="boardTemperature">-</span></div>
    <div class="row"><span>External Temperature</span><span id="externalTemperature">-</span></div>
    <div class="row"><span>PIR Presence Detected</span><span id="lastPresence">-</span></div>
    <div class="row"><span>Last GPS Time Sync</span><span id="lastGpsSync">-</span></div>
  </div>
  <div style="text-align: center;">
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
          document.getElementById('boardTemperature').textContent = formatTemperature(data.boardTemperature, data.temperatureUnit);
          document.getElementById('externalTemperature').textContent = formatTemperature(data.externalTemperature, data.temperatureUnit);
          document.getElementById('freeHeap').textContent = data.freeHeap + " bytes";
          document.getElementById('minFreeHeap').textContent = data.minFreeHeap + " bytes";
          document.getElementById('uptime').textContent = formatUptime(data.uptime);
          document.getElementById('lastPresence').textContent = data.lastPresence < 0 ? "Never" : formatElapsed(data.lastPresence);
          document.getElementById('deviceMode').textContent = data.deviceMode;
          document.getElementById('displayType').textContent = data.displayType;
          document.getElementById('highVoltageOn').textContent = data.highVoltageOn ? "On" : "Off";
          const hvPercent = data.uptime > 0 ? (data.hvOnSeconds / data.uptime * 100).toFixed(1) : "0.0";
          document.getElementById('hvOnSeconds').textContent = formatUptime(data.hvOnSeconds) + " (" + hvPercent + "%)";
          document.getElementById('wifiClients').textContent = data.wifiClients;
          document.getElementById('apIP').textContent = data.apIP;
          document.getElementById('resetReason').textContent = data.resetReason;
          document.getElementById('chip').textContent = data.chipModel + " rev " + data.chipRevision;
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
