// HTMLIndex.h

// provides the landing page linking to the registers and configuration pages

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Config.h>

const char htmlIndex[] = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
  <title>Nixie Calculator</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    :root {
      --bg: #f4f1ec;
      --border: #e2dccf;
      --text: #2b2620;
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
      max-width: 600px;
      margin: 0 auto;
      padding: 24px 5px;
      display: flex;
      flex-direction: column;
      align-items: center;
    }

    .content a {
      display: block;
      text-decoration: none;
      color: #fff;
      background-color: var(--accent);
      border: 1px solid var(--accent);
      border-radius: var(--radius);
      margin: 8px 0;
      padding: 10px 30px;
      font-size: 1.05rem;
      font-weight: 600;
      width: 200px;
      text-align: center;
      transition: background-color .15s ease, border-color .15s ease;
    }

    .content a:hover {
      background-color: var(--accent-dark);
      border-color: var(--accent-dark);
    }
  </style>
</head>

<body>
  <div class="headline">
    <h1>Nixie Calculator</h1>
  </div>
  <div class="content">
    <a href="/calculator">Calculator</a>
    <a href="/config">Configuration</a>
    <a href="/status">Status</a>
    <a href="/timesync">Time Sync</a>
    <a href="/password">SSID &amp; Passwords</a>
)rawliteral"
#if OTA_SUPPORT
R"rawliteral(
    <a href="/firmware">Firmware Update</a>
)rawliteral"
#endif
R"rawliteral(
  </div>
</body>

</html>
)rawliteral";
