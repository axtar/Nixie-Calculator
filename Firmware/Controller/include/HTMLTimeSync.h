// HTMLTimeSync.h

// provides the browser-triggered device time sync page

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

const char htmlTimeSync[] = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
  <title>Nixie Calculator Time Sync</title>
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
      max-width: 400px;
      margin: 0 auto;
      padding: 20px 5px;
    }

    .row {
      display: flex;
      justify-content: space-between;
      gap: 10px;
      padding: 6px 4px;
      border-bottom: 1px solid #ccc;
      text-align: left;
    }

    .row span:first-child {
      color: #143642;
      font-weight: bold;
    }

    #saveBar {
      text-align: center;
      padding: 20px 0 10px 0;
    }

    #saveBar button,
    #saveBar a {
      display: inline-block;
      font-size: 1rem;
      padding: 8px 30px;
      margin: 0 5px;
      text-decoration: none;
      vertical-align: middle;
    }

    #backButton {
      background-color: white;
      border: 1px solid #143642;
      color: #143642;
      border-radius: 4px;
    }

    #backButton:hover {
      background-color: #143642;
      color: white;
    }

    #syncButton {
      background-color: #143642;
      border: 1px solid #143642;
      color: white;
      border-radius: 4px;
      cursor: pointer;
    }

    #syncButton:hover {
      background-color: white;
      color: #143642;
    }

    #status {
      display: block;
      margin-top: 10px;
      font-weight: bold;
      text-align: center;
    }
  </style>
</head>

<body>
  <div class="headline">
    <h1>Time Sync</h1>
  </div>
  <div class="content">
    <div class="row"><span>Browser Time (UTC)</span><span id="browserTime">-</span></div>
    <div id="saveBar">
      <a href="/" id="backButton">Back</a>
      <button type="button" id="syncButton">Sync with browser</button>
    </div>
    <span id="status"></span>
  </div>

  <script>
    function formatUTC(epochSeconds) {
      return new Date(epochSeconds * 1000).toISOString().replace('T', ' ').replace('.000Z', ' UTC');
    }

    function showStatus(text, isError) {
      const status = document.getElementById('status');
      status.textContent = text;
      status.style.color = isError ? 'crimson' : 'green';
    }

    function tick() {
      document.getElementById('browserTime').textContent = formatUTC(Math.floor(Date.now() / 1000));
    }

    window.addEventListener('load', () => {
      tick();
      setInterval(tick, 1000);
    });

    document.getElementById('syncButton').addEventListener('click', () => {
      const epoch = Math.floor(Date.now() / 1000);
      fetch('/api/time', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ epoch })
      })
        .then(response => {
          if (!response.ok) {
            throw new Error('Request failed');
          }
          return response.json();
        })
        .then(() => showStatus('Time synced.', false))
        .catch(() => showStatus('Failed to sync time.', true));
    });
  </script>
</body>

</html>
)rawliteral";
