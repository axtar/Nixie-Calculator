// HTMLConfig.h

// provides the settings configuration page

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

const char htmlConfig[] = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
  <title>Nixie Calculator Configuration</title>
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
      --danger: #d1293d;
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
      text-align: center;
      border-bottom: 3px solid var(--accent);
      flex: 0 0 auto;
    }

    .content {
      max-width: 720px;
      width: 100%;
      box-sizing: border-box;
      margin: 0 auto;
      padding: 20px 14px 0;
      flex: 1 1 auto;
      display: flex;
      flex-direction: column;
    }

    #configForm {
      flex: 1 1 auto;
      display: flex;
      flex-direction: column;
    }

    details.section {
      background: var(--card-bg);
      border: 1px solid var(--border);
      border-radius: var(--radius);
      margin-bottom: 14px;
      box-shadow: 0 1px 2px rgba(0, 0, 0, .04);
      overflow: hidden;
    }

    details.section summary {
      list-style: none;
      cursor: pointer;
      padding: 12px 16px;
      font-weight: 600;
      color: var(--text);
      display: flex;
      align-items: center;
      justify-content: space-between;
      user-select: none;
    }

    details.section summary::-webkit-details-marker {
      display: none;
    }

    details.section summary::after {
      content: '';
      width: 9px;
      height: 9px;
      border-right: 2px solid var(--text-muted);
      border-bottom: 2px solid var(--text-muted);
      transform: rotate(-45deg);
      transition: transform .15s ease;
      margin-left: 8px;
      flex: none;
    }

    details.section[open] summary::after {
      transform: rotate(45deg);
    }

    .row {
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 12px;
      padding: 10px 16px;
    }

    .row+.row {
      border-top: 1px solid var(--border);
    }

    .row label {
      flex: 1 1 auto;
      font-size: .95rem;
    }

    .row select,
    .row input[type=number],
    .row input[type=text],
    .row input[type=time] {
      flex: 0 0 auto;
      min-width: 60px;
      padding: 6px 10px;
      border: 1px solid var(--border);
      border-radius: 6px;
      background-color: #f0f0f0;
      color: var(--text);
      font-size: .95rem;
      font-family: inherit;
    }

    .row input[type=number],
    .row input[type=text],
    .row input[type=time] {
      width: 100px;
    }

    .row select {
      appearance: none;
      -webkit-appearance: none;
      min-width: 130px;
      padding-right: 28px;
      background-image: url("data:image/svg+xml;charset=UTF-8,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 12 8'%3E%3Cpath fill='%237a7266' d='M1 1l5 5 5-5'/%3E%3C/svg%3E");
      background-repeat: no-repeat;
      background-position: right 8px center;
      background-size: 10px 7px;
    }

    .row select:focus,
    .row input:focus {
      outline: none;
      border-color: var(--accent);
      box-shadow: 0 0 0 3px rgba(255, 140, 0, .18);
    }

    .row input[type=color] {
      width: 44px;
      height: 30px;
      padding: 2px;
      border: 1px solid var(--border);
      border-radius: 6px;
      background: #fff;
      cursor: pointer;
    }

    #saveBar {
      flex: 0 0 auto;
      margin-top: auto;
      position: sticky;
      bottom: 0;
      background: rgba(244, 241, 236, .92);
      backdrop-filter: blur(6px);
      padding: 14px 0;
      border-top: 1px solid var(--border);
      text-align: center;
    }

    #saveBar button,
    #saveBar a {
      display: inline-flex;
      align-items: center;
      justify-content: center;
      box-sizing: border-box;
      height: 38px;
      font-size: .95rem;
      font-weight: 600;
      padding: 0 18px;
      margin: 0 5px;
      text-decoration: none;
      vertical-align: middle;
      border-radius: 6px;
      border: 1px solid transparent;
      cursor: pointer;
      transition: background-color .15s ease, color .15s ease, border-color .15s ease;
    }

    #backButton {
      background-color: var(--accent);
      border-color: var(--accent);
      color: #fff;
    }

    #backButton:hover {
      background-color: var(--accent-dark);
      border-color: var(--accent-dark);
    }

    #saveBar button[type=submit] {
      background-color: var(--accent);
      border-color: var(--accent);
      color: #fff;
    }

    #saveBar button[type=submit]:hover {
      background-color: var(--accent-dark);
      border-color: var(--accent-dark);
    }

    #resetButton {
      background-color: var(--accent);
      border-color: var(--accent);
      color: #fff;
    }

    #resetButton:hover {
      background-color: var(--accent-dark);
      border-color: var(--accent-dark);
    }

    #toggleAllButton {
      background-color: var(--accent);
      border-color: var(--accent);
      color: #fff;
    }

    #saveBar #toggleAllButton {
      width: 34px;
      padding: 0;
    }

    #toggleAllButton:hover {
      background-color: var(--accent-dark);
      border-color: var(--accent-dark);
    }

    #toggleAllButton::after {
      content: '';
      display: inline-block;
      width: 9px;
      height: 9px;
      border-right: 2px solid #fff;
      border-bottom: 2px solid #fff;
      transform: translate(-2px, 0) rotate(-45deg);
      transition: transform .15s ease;
    }

    #toggleAllButton.all-open::after {
      transform: translate(0, -2px) rotate(45deg);
    }

    #status {
      display: block;
      margin-top: 8px;
      font-weight: 600;
      font-size: .9rem;
      min-height: 1.2em;
    }

  </style>
</head>

<body>
  <div class="headline">
    <h1>Nixie Calculator Configuration</h1>
  </div>
  <div class="content">
    <form id="configForm" novalidate>

      <details class="section" id="section-general">
        <summary>General</summary>
        <div class="row" data-setting="startupmode">
          <label for="startupmode">Startup mode</label>
          <select id="startupmode" name="startupmode">
            <option value="0">Calculator</option>
            <option value="1">Clock</option>
          </select>
        </div>
        <div class="row" data-setting="showversion">
          <label for="showversion">Show version at startup</label>
          <select id="showversion" name="showversion">
            <option value="0">Off</option>
            <option value="1">On</option>
          </select>
        </div>
        <div class="row" data-setting="autooffmode">
          <label for="autooffmode">Auto-off mode</label>
          <select id="autooffmode" name="autooffmode">
            <option value="0">Off</option>
            <option value="1">On</option>
            <option value="2">Switch to clock mode</option>
          </select>
        </div>
        <div class="row" data-setting="autooffdelay">
          <label for="autooffdelay">Auto-off delay (minutes)</label>
          <input type="number" id="autooffdelay" name="autooffdelay" min="1" max="720">
        </div>
        <div class="row" data-setting="inputblinking">
          <label for="inputblinking">Input blinking</label>
          <select id="inputblinking" name="inputblinking">
            <option value="0">Off</option>
            <option value="1">On</option>
          </select>
        </div>
        <div class="row" data-setting="apautostart">
          <label for="apautostart">Auto-start WiFi access point</label>
          <select id="apautostart" name="apautostart">
            <option value="0">Off</option>
            <option value="1">On</option>
          </select>
        </div>
      </details>

      <details class="section" id="section-clock">
        <summary>Clock</summary>
        <div class="row" data-setting="clockmode">
          <label for="clockmode">Clock display mode</label>
          <select id="clockmode" name="clockmode">
            <option value="0">Time</option>
            <option value="1">Time (no seconds)</option>
            <option value="2">Time (moving)</option>
            <option value="3">Time or date</option>
            <option value="4">Time and date</option>
            <option value="5">Time and temperature</option>
            <option value="6">Time, date and temperature</option>
            <option value="7">Date and time (raw)</option>
            <option value="8">Dual time</option>
            <option value="9">Timer</option>
            <option value="10">Stopwatch</option>
          </select>
        </div>
        <div class="row" data-setting="hourmode">
          <label for="hourmode">Hour mode</label>
          <select id="hourmode" name="hourmode">
            <option value="0">12-hour</option>
            <option value="1">24-hour</option>
          </select>
        </div>
        <div class="row" data-setting="leadingzero">
          <label for="leadingzero">Hours leading zero</label>
          <select id="leadingzero" name="leadingzero">
            <option value="0">Off</option>
            <option value="1">On</option>
          </select>
        </div>
        <div class="row" data-setting="timeseparator">
          <label for="timeseparator">Time separator</label>
          <select id="timeseparator" name="timeseparator">
            <option value="0">Off</option>
            <option value="1">Blinking</option>
            <option value="2">On</option>
          </select>
        </div>
        <div class="row" data-setting="dateformat">
          <label for="dateformat">Date format</label>
          <select id="dateformat" name="dateformat">
            <option value="0">DD-MM-YY</option>
            <option value="1">YY-MM-DD</option>
            <option value="2">MM-DD-YY</option>
            <option value="3">YY-DD-MM</option>
          </select>
        </div>
        <div class="row" data-setting="notifytimer">
          <label for="notifytimer">Notify end of timer</label>
          <select id="notifytimer" name="notifytimer">
            <option value="0">Off</option>
            <option value="1">On</option>
          </select>
        </div>
        <div class="row" data-setting="timercolor">
          <label for="timercolor">Timer notification color</label>
          <input type="color" id="timercolor" name="timercolor">
        </div>
        <div class="row" data-setting="rtcdriftcorr">
          <label for="rtcdriftcorr">RTC drift correction (sec/month)</label>
          <input type="text" inputmode="numeric" id="rtcdriftcorr" name="rtcdriftcorr" min="-60" max="60">
        </div>
      </details>

      <details class="section" id="section-calculator">
        <summary>Calculator</summary>
        <div class="row" data-setting="fixeddecimals">
          <label for="fixeddecimals">Fixed decimals</label>
          <select id="fixeddecimals" name="fixeddecimals">
            <option value="0">Off (floating)</option>
            <option value="1">1</option>
            <option value="2">2</option>
            <option value="3">3</option>
            <option value="4">4</option>
            <option value="5">5</option>
            <option value="6">6</option>
            <option value="7">7</option>
            <option value="8">8</option>
          </select>
        </div>
        <div class="row" data-setting="anglemode">
          <label for="anglemode">Startup angle mode</label>
          <select id="anglemode" name="anglemode">
            <option value="0">Degrees</option>
            <option value="1">Radians</option>
          </select>
        </div>
        <div class="row" data-setting="showbusycalc">
          <label for="showbusycalc">Busy animation</label>
          <select id="showbusycalc" name="showbusycalc">
            <option value="0">Off</option>
            <option value="1">Moving decimal separator</option>
            <option value="2">Digit flickering</option>
          </select>
        </div>
        <div class="row" data-setting="maxexpdigits">
          <label for="maxexpdigits">Max exponent digits</label>
          <input type="number" id="maxexpdigits" name="maxexpdigits" min="2" max="4">
        </div>
        <div class="row" data-setting="scrolldelay">
          <label for="scrolldelay">Scroll delay (1/10 s)</label>
          <input type="number" id="scrolldelay" name="scrolldelay" min="1" max="20">
        </div>
        <div class="row" data-setting="calcprecision">
          <label for="calcprecision">Calculation precision</label>
          <input type="number" id="calcprecision" name="calcprecision" min="20" max="32">
        </div>
        <div class="row" data-setting="roundingmode">
          <label for="roundingmode">Rounding mode</label>
          <select id="roundingmode" name="roundingmode">
            <option value="0">5/4</option>
            <option value="1">Cut</option>
          </select>
        </div>
        <div class="row" data-setting="calcinputdirec">
          <label for="calcinputdirec">Input direction</label>
          <select id="calcinputdirec" name="calcinputdirec">
            <option value="0">Left to right</option>
            <option value="1">Right to left</option>
            <option value="2">Right to left (zero padding)</option>
          </select>
        </div>
      </details>

      <details class="section" id="section-pir">
        <summary>PIR sensor</summary>
        <div class="row" data-setting="pirmode">
          <label for="pirmode">PIR sensor</label>
          <select id="pirmode" name="pirmode">
            <option value="0">Off</option>
            <option value="1">On</option>
          </select>
        </div>
        <div class="row" data-setting="pirdelay">
          <label for="pirdelay">PIR delay (minutes)</label>
          <input type="number" id="pirdelay" name="pirdelay" min="1" max="720">
        </div>
      </details>

      <details class="section" id="section-gps">
        <summary>GPS</summary>
        <div class="row" data-setting="gpsmode">
          <label for="gpsmode">GPS time sync</label>
          <select id="gpsmode" name="gpsmode">
            <option value="0">Off</option>
            <option value="1">On</option>
          </select>
        </div>
        <div class="row" data-setting="gpsspeed">
          <label for="gpsspeed">GPS baud rate</label>
          <select id="gpsspeed" name="gpsspeed">
            <option value="0">2400</option>
            <option value="1">4800</option>
            <option value="2">9600</option>
            <option value="3">19200</option>
            <option value="4">38400</option>
            <option value="5">57600</option>
            <option value="6">115200</option>
          </select>
        </div>
        <div class="row" data-setting="gpssyncinterval">
          <label for="gpssyncinterval">GPS sync interval (minutes)</label>
          <input type="number" id="gpssyncinterval" name="gpssyncinterval" min="1" max="720">
        </div>
        <div class="row" data-setting="gpsnotifysync">
          <label for="gpsnotifysync">Notify GPS sync</label>
          <select id="gpsnotifysync" name="gpsnotifysync">
            <option value="0">Off</option>
            <option value="1">On</option>
          </select>
        </div>
        <div class="row" data-setting="gpssynccolor">
          <label for="gpssynccolor">GPS sync notification color</label>
          <input type="color" id="gpssynccolor" name="gpssynccolor">
        </div>
      </details>

      <details class="section" id="section-temperature">
        <summary>Temperature</summary>
        <div class="row" data-setting="temperaturemode">
          <label for="temperaturemode">Temperature sensor</label>
          <select id="temperaturemode" name="temperaturemode">
            <option value="0">Off</option>
            <option value="1">On</option>
          </select>
        </div>
        <div class="row" data-setting="temperaturecf">
          <label for="temperaturecf">Temperature unit</label>
          <select id="temperaturecf" name="temperaturecf">
            <option value="0">Celsius</option>
            <option value="1">Fahrenheit</option>
          </select>
        </div>
        <div class="row" data-setting="exttempcorr">
          <label for="exttempcorr">Sensor correction (0.1 &deg;C steps)</label>
          <input type="text" inputmode="numeric" id="exttempcorr" name="exttempcorr" min="-100" max="100">
        </div>
      </details>

      <details class="section" id="section-acp">
        <summary>Cathode poisoning prevention</summary>
        <div class="row" data-setting="acpstarttime">
          <label for="acpstarttime">Start time</label>
          <input type="time" id="acpstarttime" name="acpstarttime">
        </div>
        <div class="row" data-setting="acpduration">
          <label for="acpduration">Duration (minutes)</label>
          <input type="number" id="acpduration" name="acpduration" min="0" max="720">
        </div>
        <div class="row" data-setting="acpforceon">
          <label for="acpforceon">Force nixies on</label>
          <select id="acpforceon" name="acpforceon">
            <option value="0">Off</option>
            <option value="1">On</option>
          </select>
        </div>
      </details>

      <details class="section" id="section-brightness">
        <summary>Display brightness</summary>
        <div class="row" data-setting="brightness">
          <label for="brightness">Display brightness</label>
          <input type="number" id="brightness" name="brightness" min="1" max="15">
        </div>
        <div class="row" data-setting="dimbrightness">
          <label for="dimbrightness">Dim brightness</label>
          <input type="number" id="dimbrightness" name="dimbrightness" min="1" max="15">
        </div>
        <div class="row" data-setting="dimstarttime">
          <label for="dimstarttime">Dim start time</label>
          <input type="time" id="dimstarttime" name="dimstarttime">
        </div>
        <div class="row" data-setting="dimduration">
          <label for="dimduration">Dim duration (minutes)</label>
          <input type="number" id="dimduration" name="dimduration" min="0" max="720">
        </div>
      </details>

      <details class="section" id="section-led">
        <summary>LED lighting</summary>
        <div class="row" data-setting="ledmode">
          <label for="ledmode">LED lighting</label>
          <select id="ledmode" name="ledmode">
            <option value="0">By time</option>
            <option value="1">Always</option>
          </select>
        </div>
        <div class="row" data-setting="calcrgbmode">
          <label for="calcrgbmode">RGB mode (calculator)</label>
          <select id="calcrgbmode" name="calcrgbmode">
            <option value="0">Off</option>
            <option value="1">By content</option>
            <option value="2">By content (all digits)</option>
            <option value="3">Random (turned on only)</option>
            <option value="4">Full random (turned on only)</option>
            <option value="5">Fixed (all digits)</option>
            <option value="6">Random (all digits)</option>
            <option value="7">Full random (all digits)</option>
            <option value="8">Color wheel (turned on only)</option>
            <option value="9">Color wheel (all digits)</option>
            <option value="10">Rainbow (turned on only)</option>
            <option value="11">Rainbow (all digits)</option>
          </select>
        </div>
        <div class="row" data-setting="clockrgbmode">
          <label for="clockrgbmode">RGB mode (clock)</label>
          <select id="clockrgbmode" name="clockrgbmode">
            <option value="0">Off</option>
            <option value="1">By content</option>
            <option value="2">Random (turned on only)</option>
            <option value="3">Full random (turned on only)</option>
            <option value="4">Fixed (all digits)</option>
            <option value="5">Random (all digits)</option>
            <option value="6">Full random (all digits)</option>
            <option value="7">Color wheel (turned on only)</option>
            <option value="8">Color wheel (all digits)</option>
            <option value="9">Rainbow (turned on only)</option>
            <option value="10">Rainbow (all digits)</option>
          </select>
        </div>
        <div class="row" data-setting="breathingmode">
          <label for="breathingmode">Breathing effect</label>
          <select id="breathingmode" name="breathingmode">
            <option value="0">Off</option>
            <option value="1">On</option>
          </select>
        </div>
        <div class="row" data-setting="trigcolorchange">
          <label for="trigcolorchange">Color change (clock)</label>
          <select id="trigcolorchange" name="trigcolorchange">
            <option value="0">Off</option>
            <option value="1">Every second</option>
            <option value="2">Every minute</option>
            <option value="3">Every hour</option>
          </select>
        </div>
        <div class="row" data-setting="ledstarttime">
          <label for="ledstarttime">LED start time</label>
          <input type="time" id="ledstarttime" name="ledstarttime">
        </div>
        <div class="row" data-setting="ledduration">
          <label for="ledduration">LED duration (minutes)</label>
          <input type="number" id="ledduration" name="ledduration" min="0" max="720">
        </div>
        <div class="row" data-setting="ledstarttime2">
          <label for="ledstarttime2">LED start time 2</label>
          <input type="time" id="ledstarttime2" name="ledstarttime2">
        </div>
        <div class="row" data-setting="ledduration2">
          <label for="ledduration2">LED duration 2 (minutes)</label>
          <input type="number" id="ledduration2" name="ledduration2" min="0" max="720">
        </div>
      </details>

      <details class="section" id="section-colors">
        <summary>Lighting colors</summary>
        <div class="row" data-setting="negativecolor">
          <label for="negativecolor">Negative number color</label>
          <input type="color" id="negativecolor" name="negativecolor">
        </div>
        <div class="row" data-setting="positivecolor">
          <label for="positivecolor">Positive number color</label>
          <input type="color" id="positivecolor" name="positivecolor">
        </div>
        <div class="row" data-setting="errorcolor">
          <label for="errorcolor">Error color</label>
          <input type="color" id="errorcolor" name="errorcolor">
        </div>
        <div class="row" data-setting="negexpcolor">
          <label for="negexpcolor">Negative exponent color</label>
          <input type="color" id="negexpcolor" name="negexpcolor">
        </div>
        <div class="row" data-setting="posexpcolor">
          <label for="posexpcolor">Positive exponent color</label>
          <input type="color" id="posexpcolor" name="posexpcolor">
        </div>
        <div class="row" data-setting="fixedcalccolor">
          <label for="fixedcalccolor">Fixed color (calculator)</label>
          <input type="color" id="fixedcalccolor" name="fixedcalccolor">
        </div>
        <div class="row" data-setting="timecolor">
          <label for="timecolor">Time color</label>
          <input type="color" id="timecolor" name="timecolor">
        </div>
        <div class="row" data-setting="timecolor2">
          <label for="timecolor2">Time color (dual time)</label>
          <input type="color" id="timecolor2" name="timecolor2">
        </div>
        <div class="row" data-setting="datecolor">
          <label for="datecolor">Date color</label>
          <input type="color" id="datecolor" name="datecolor">
        </div>
        <div class="row" data-setting="tempcolor">
          <label for="tempcolor">Temperature color</label>
          <input type="color" id="tempcolor" name="tempcolor">
        </div>
        <div class="row" data-setting="fixedcolor">
          <label for="fixedcolor">Fixed color (clock)</label>
          <input type="color" id="fixedcolor" name="fixedcolor">
        </div>
      </details>

      <details class="section" id="section-timezone">
        <summary>Timezone settings</summary>
        <div class="row" data-setting="dstweek">
          <label for="dstweek">DST change - week</label>
          <select id="dstweek" name="dstweek">
            <option value="1">First</option>
            <option value="2">Second</option>
            <option value="3">Third</option>
            <option value="4">Fourth</option>
            <option value="5">Last</option>
          </select>
        </div>
        <div class="row" data-setting="dstdow">
          <label for="dstdow">DST change - day of week</label>
          <select id="dstdow" name="dstdow">
            <option value="0">Sunday</option>
            <option value="1">Monday</option>
            <option value="2">Tuesday</option>
            <option value="3">Wednesday</option>
            <option value="4">Thursday</option>
            <option value="5">Friday</option>
            <option value="6">Saturday</option>
          </select>
        </div>
        <div class="row" data-setting="dstmonth">
          <label for="dstmonth">DST change - month</label>
          <select id="dstmonth" name="dstmonth">
            <option value="0">January</option>
            <option value="1">February</option>
            <option value="2">March</option>
            <option value="3">April</option>
            <option value="4">May</option>
            <option value="5">June</option>
            <option value="6">July</option>
            <option value="7">August</option>
            <option value="8">September</option>
            <option value="9">October</option>
            <option value="10">November</option>
            <option value="11">December</option>
          </select>
        </div>
        <div class="row" data-setting="dsthour">
          <label for="dsthour">DST change - hour</label>
          <input type="number" id="dsthour" name="dsthour" min="0" max="23">
        </div>
        <div class="row" data-setting="dstoffset">
          <label for="dstoffset">DST offset to UTC (minutes)</label>
          <input type="text" inputmode="numeric" id="dstoffset" name="dstoffset" min="-720" max="840">
        </div>
        <div class="row" data-setting="stdweek">
          <label for="stdweek">STD change - week</label>
          <select id="stdweek" name="stdweek">
            <option value="1">First</option>
            <option value="2">Second</option>
            <option value="3">Third</option>
            <option value="4">Fourth</option>
            <option value="5">Last</option>
          </select>
        </div>
        <div class="row" data-setting="stddow">
          <label for="stddow">STD change - day of week</label>
          <select id="stddow" name="stddow">
            <option value="0">Sunday</option>
            <option value="1">Monday</option>
            <option value="2">Tuesday</option>
            <option value="3">Wednesday</option>
            <option value="4">Thursday</option>
            <option value="5">Friday</option>
            <option value="6">Saturday</option>
          </select>
        </div>
        <div class="row" data-setting="stdmonth">
          <label for="stdmonth">STD change - month</label>
          <select id="stdmonth" name="stdmonth">
            <option value="0">January</option>
            <option value="1">February</option>
            <option value="2">March</option>
            <option value="3">April</option>
            <option value="4">May</option>
            <option value="5">June</option>
            <option value="6">July</option>
            <option value="7">August</option>
            <option value="8">September</option>
            <option value="9">October</option>
            <option value="10">November</option>
            <option value="11">December</option>
          </select>
        </div>
        <div class="row" data-setting="stdhour">
          <label for="stdhour">STD change - hour</label>
          <input type="number" id="stdhour" name="stdhour" min="0" max="23">
        </div>
        <div class="row" data-setting="stdoffset">
          <label for="stdoffset">STD offset to UTC (minutes)</label>
          <input type="text" inputmode="numeric" id="stdoffset" name="stdoffset" min="-720" max="840">
        </div>
      </details>

      <details class="section" id="section-timezone2">
        <summary>Timezone settings (dual time)</summary>
        <div class="row" data-setting="dstweek2">
          <label for="dstweek2">DST change - week</label>
          <select id="dstweek2" name="dstweek2">
            <option value="1">First</option>
            <option value="2">Second</option>
            <option value="3">Third</option>
            <option value="4">Fourth</option>
            <option value="5">Last</option>
          </select>
        </div>
        <div class="row" data-setting="dstdow2">
          <label for="dstdow2">DST change - day of week</label>
          <select id="dstdow2" name="dstdow2">
            <option value="0">Sunday</option>
            <option value="1">Monday</option>
            <option value="2">Tuesday</option>
            <option value="3">Wednesday</option>
            <option value="4">Thursday</option>
            <option value="5">Friday</option>
            <option value="6">Saturday</option>
          </select>
        </div>
        <div class="row" data-setting="dstmonth2">
          <label for="dstmonth2">DST change - month</label>
          <select id="dstmonth2" name="dstmonth2">
            <option value="0">January</option>
            <option value="1">February</option>
            <option value="2">March</option>
            <option value="3">April</option>
            <option value="4">May</option>
            <option value="5">June</option>
            <option value="6">July</option>
            <option value="7">August</option>
            <option value="8">September</option>
            <option value="9">October</option>
            <option value="10">November</option>
            <option value="11">December</option>
          </select>
        </div>
        <div class="row" data-setting="dsthour2">
          <label for="dsthour2">DST change - hour</label>
          <input type="number" id="dsthour2" name="dsthour2" min="0" max="23">
        </div>
        <div class="row" data-setting="dstoffset2">
          <label for="dstoffset2">DST offset to UTC (minutes)</label>
          <input type="text" inputmode="numeric" id="dstoffset2" name="dstoffset2" min="-720" max="840">
        </div>
        <div class="row" data-setting="stdweek2">
          <label for="stdweek2">STD change - week</label>
          <select id="stdweek2" name="stdweek2">
            <option value="1">First</option>
            <option value="2">Second</option>
            <option value="3">Third</option>
            <option value="4">Fourth</option>
            <option value="5">Last</option>
          </select>
        </div>
        <div class="row" data-setting="stddow2">
          <label for="stddow2">STD change - day of week</label>
          <select id="stddow2" name="stddow2">
            <option value="0">Sunday</option>
            <option value="1">Monday</option>
            <option value="2">Tuesday</option>
            <option value="3">Wednesday</option>
            <option value="4">Thursday</option>
            <option value="5">Friday</option>
            <option value="6">Saturday</option>
          </select>
        </div>
        <div class="row" data-setting="stdmonth2">
          <label for="stdmonth2">STD change - month</label>
          <select id="stdmonth2" name="stdmonth2">
            <option value="0">January</option>
            <option value="1">February</option>
            <option value="2">March</option>
            <option value="3">April</option>
            <option value="4">May</option>
            <option value="5">June</option>
            <option value="6">July</option>
            <option value="7">August</option>
            <option value="8">September</option>
            <option value="9">October</option>
            <option value="10">November</option>
            <option value="11">December</option>
          </select>
        </div>
        <div class="row" data-setting="stdhour2">
          <label for="stdhour2">STD change - hour</label>
          <input type="number" id="stdhour2" name="stdhour2" min="0" max="23">
        </div>
        <div class="row" data-setting="stdoffset2">
          <label for="stdoffset2">STD offset to UTC (minutes)</label>
          <input type="text" inputmode="numeric" id="stdoffset2" name="stdoffset2" min="-720" max="840">
        </div>
      </details>

      <div id="saveBar">
        <a href="/" id="backButton">Back</a>
        <button type="submit">Save</button>
        <button type="button" id="resetButton">Reset</button>
        <button type="button" id="toggleAllButton" title="Expand all" aria-label="Expand all"></button>
        <span id="status"></span>
      </div>
    </form>
  </div>

  <script>
    function getKind(el) {
      if (el.tagName === 'SELECT') return 'number';
      return el.type;
    }

    function setFieldValue(el, value) {
      const kind = getKind(el);
      if (kind === 'color') {
        el.value = '#' + Number(value).toString(16).padStart(6, '0');
      } else if (kind === 'time') {
        const h = Math.floor(value / 60);
        const m = value % 60;
        el.value = String(h).padStart(2, '0') + ':' + String(m).padStart(2, '0');
      } else {
        el.value = value;
      }
    }

    function getFieldValue(el) {
      const kind = getKind(el);
      if (kind === 'color') {
        return parseInt(el.value.substring(1), 16);
      } else if (kind === 'time') {
        const parts = el.value.split(':');
        return (parseInt(parts[0], 10) * 60) + parseInt(parts[1], 10);
      } else {
        return parseInt(el.value, 10);
      }
    }

    function fieldLabel(el) {
      const label = document.querySelector(`label[for="${el.id}"]`);
      return label ? label.textContent : el.name;
    }

    // validates a field's parsed value against its min/max attributes (still present in
    // markup even for type="text" inputs); returns an error message, or null if valid
    function validateField(el) {
      const kind = getKind(el);
      if ((kind === 'color') || (kind === 'time')) {
        return null;
      }
      const value = getFieldValue(el);
      if (Number.isNaN(value)) {
        return `${fieldLabel(el)} must be a number.`;
      }
      if (el.hasAttribute('min') && (value < parseInt(el.getAttribute('min'), 10))) {
        return `${fieldLabel(el)} must be at least ${el.getAttribute('min')}.`;
      }
      if (el.hasAttribute('max') && (value > parseInt(el.getAttribute('max'), 10))) {
        return `${fieldLabel(el)} must be at most ${el.getAttribute('max')}.`;
      }
      return null;
    }

    function showStatus(text, isError) {
      const status = document.getElementById('status');
      status.textContent = text;
      status.style.color = isError ? 'crimson' : 'green';
      setTimeout(() => { status.textContent = ''; }, 3000);
    }

    // remember which sections are expanded across page loads (e.g. Back then returning),
    // since a plain page navigation would otherwise reset every <details> to collapsed
    const SECTIONS_STATE_KEY = 'configSectionsOpen';

    function restoreSectionState() {
      let saved = {};
      try {
        saved = JSON.parse(localStorage.getItem(SECTIONS_STATE_KEY)) || {};
      } catch (e) {
        saved = {};
      }
      document.querySelectorAll('details.section[id]').forEach(el => {
        if (el.id in saved) {
          el.open = saved[el.id];
        }
      });
    }

    function saveSectionState() {
      const state = {};
      document.querySelectorAll('details.section[id]').forEach(el => {
        state[el.id] = el.open;
      });
      try {
        localStorage.setItem(SECTIONS_STATE_KEY, JSON.stringify(state));
      } catch (e) {
        // ignore, e.g. storage disabled or full
      }
    }

    restoreSectionState();

    const toggleAllButton = document.getElementById('toggleAllButton');

    function updateToggleAllButton() {
      const sections = document.querySelectorAll('details.section[id]');
      const allOpen = Array.from(sections).every(el => el.open);
      toggleAllButton.classList.toggle('all-open', allOpen);
      toggleAllButton.title = allOpen ? 'Collapse all' : 'Expand all';
      toggleAllButton.setAttribute('aria-label', toggleAllButton.title);
    }

    toggleAllButton.addEventListener('click', () => {
      const sections = document.querySelectorAll('details.section[id]');
      const allOpen = Array.from(sections).every(el => el.open);
      sections.forEach(el => { el.open = !allOpen; });
      saveSectionState();
      updateToggleAllButton();
    });

    document.querySelectorAll('details.section[id]').forEach(el => {
      el.addEventListener('toggle', saveSectionState);
      el.addEventListener('toggle', updateToggleAllButton);
    });

    updateToggleAllButton();

    function loadSettings() {
      return fetch('/api/settings')
        .then(response => response.json())
        .then(data => {
          document.querySelectorAll('#configForm [name]').forEach(el => {
            const row = el.closest('.row');
            if (!(el.name in data)) {
              if (row) row.style.display = 'none';
              return;
            }
            setFieldValue(el, data[el.name]);
          });
        })
        .catch(() => showStatus('Failed to load settings.', true));
    }

    window.addEventListener('load', loadSettings);

    document.getElementById('configForm').addEventListener('submit', (event) => {
      event.preventDefault();
      const payload = {};
      let firstError = null;
      document.querySelectorAll('#configForm [name]').forEach(el => {
        const row = el.closest('.row');
        if (row && row.style.display === 'none') return;
        const error = validateField(el);
        if (error && !firstError) {
          firstError = error;
        }
        payload[el.name] = getFieldValue(el);
      });
      if (firstError) {
        showStatus(firstError, true);
        return;
      }
      fetch('/api/settings', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(payload)
      })
        .then(response => {
          if (!response.ok) {
            throw new Error('Request failed');
          }
          return response.json();
        })
        .then(() => showStatus('Settings saved.', false))
        .catch(() => showStatus('Failed to save settings.', true));
    });

    document.getElementById('resetButton').addEventListener('click', () => {
      if (!confirm('Reset all settings to their default values? This cannot be undone.')) {
        return;
      }
      fetch('/api/settings/reset', { method: 'POST' })
        .then(response => {
          if (!response.ok) {
            throw new Error('Request failed');
          }
          return response.json();
        })
        .then(() => loadSettings())
        .then(() => showStatus('Settings reset to defaults.', false))
        .catch(() => showStatus('Failed to reset settings.', true));
    });
  </script>
</body>

</html>
)rawliteral";
