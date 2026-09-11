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
      max-width: 400px;
      margin: 20px auto;
      padding: 0 14px;
    }

    .row {
      display: flex;
      justify-content: space-between;
      gap: 10px;
      padding: 10px 2px;
      text-align: left;
      background: var(--card-bg);
      border: 1px solid var(--border);
      border-radius: var(--radius);
      box-shadow: 0 1px 2px rgba(0, 0, 0, .04);
    }

    .row span:first-child {
      color: var(--text-muted);
      font-weight: 600;
    }

    #saveBar {
      text-align: center;
      padding: 20px 0 10px 0;
    }

    #saveBar button,
    #saveBar a {
      display: inline-block;
      font-size: .95rem;
      font-weight: 600;
      padding: 9px 28px;
      margin: 0 5px;
      text-decoration: none;
      vertical-align: middle;
      border-radius: var(--radius);
      border: 1px solid transparent;
      cursor: pointer;
      transition: background-color .15s ease, border-color .15s ease;
    }

    #backButton,
    #syncButton {
      background-color: var(--accent);
      border-color: var(--accent);
      color: #fff;
    }

    #backButton:hover,
    #syncButton:hover {
      background-color: var(--accent-dark);
      border-color: var(--accent-dark);
    }

    #status {
      display: block;
      margin-top: 10px;
      font-weight: 600;
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
