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
    html {
      font-family: Arial, Helvetica, sans-serif;
    }

    h1 {
      font-size: 1.2rem;
      color: white;
    }

    .headline {
      overflow: hidden;
      background-color: #143642;
      text-align: center;
    }

    body {
      margin: 0;
    }

    .content {
      max-width: 700px;
      margin: 0 auto;
      padding: 10px 5px 40px 5px;
    }

    fieldset {
      margin-bottom: 15px;
      border: 1px solid #ccc;
      border-radius: 4px;
    }

    legend {
      font-weight: bold;
      color: #143642;
      padding: 0 5px;
    }

    .row {
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 10px;
      padding: 6px 4px;
    }

    .row label {
      flex: 1 1 auto;
    }

    .row select,
    .row input[type=number],
    .row input[type=text] {
      flex: 0 0 auto;
      min-width: 60px;
    }

    .row input[type=number],
    .row input[type=text] {
      width: 90px;
    }

    #saveBar {
      position: sticky;
      bottom: 0;
      background: white;
      padding: 10px 0;
      border-top: 1px solid #ccc;
      text-align: center;
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

    #resetButton {
      background-color: white;
      border: 1px solid crimson;
      color: crimson;
      border-radius: 4px;
    }

    #resetButton:hover {
      background-color: crimson;
      color: white;
    }

    #status {
      margin-left: 15px;
      font-weight: bold;
      color: green;
    }

  </style>
</head>

<body>
  <div class="headline">
    <h1>Nixie Calculator Configuration</h1>
  </div>
  <div class="content">
    <form id="configForm" novalidate>

      <fieldset>
        <legend>General</legend>
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
      </fieldset>

      <fieldset>
        <legend>Clock</legend>
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
      </fieldset>

      <fieldset>
        <legend>Calculator</legend>
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
        <div class="row" data-setting="calcinputdirec">
          <label for="calcinputdirec">Input direction</label>
          <select id="calcinputdirec" name="calcinputdirec">
            <option value="0">Left to right</option>
            <option value="1">Right to left</option>
            <option value="2">Right to left (zero padding)</option>
          </select>
        </div>
      </fieldset>

      <fieldset>
        <legend>PIR sensor</legend>
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
      </fieldset>

      <fieldset>
        <legend>GPS</legend>
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
      </fieldset>

      <fieldset>
        <legend>Temperature</legend>
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
      </fieldset>

      <fieldset>
        <legend>Cathode poisoning prevention</legend>
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
      </fieldset>

      <fieldset>
        <legend>Display brightness</legend>
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
      </fieldset>

      <fieldset>
        <legend>LED lighting</legend>
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
      </fieldset>

      <fieldset>
        <legend>Lighting colors</legend>
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
      </fieldset>

      <fieldset>
        <legend>Timezone settings</legend>
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
      </fieldset>

      <fieldset>
        <legend>Timezone settings (dual time)</legend>
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
      </fieldset>

      <div id="saveBar">
        <a href="/" id="backButton">Back</a>
        <button type="submit">Save</button>
        <button type="button" id="resetButton">Reset</button>
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
