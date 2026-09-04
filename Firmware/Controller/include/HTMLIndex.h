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
      padding: 20px 5px;
      display: flex;
      flex-direction: column;
      align-items: center;
    }

    .content a {
      display: block;
      text-decoration: none;
      color: #143642;
      border: 1px solid #143642;
      border-radius: 4px;
      margin: 10px 0;
      padding: 10px 30px;
      font-size: 1.1rem;
      width: 200px;
      text-align: center;
    }

    .content a:hover {
      background-color: #143642;
      color: white;
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
