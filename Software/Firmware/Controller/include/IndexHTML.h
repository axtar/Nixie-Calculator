// IndexHTML.h

// provides the root page information

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <Config.h>

#if CALC_TYPE == CALC_TYPE_RPN

const char indexHTML[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
  <title>RPN Nixie Calculator Server</title>
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

    .regx {
      color: white;
      background-color: indigo;
    }
  </style>
  <title>RPN Nixie Calculator Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
</head>

<body>
  <div class="headline">
    <h1>RPN Nixie Calculator</h1>
  </div>
  <div class="content">
    <p class="regx">X: <span id="regx"></span></p>
    <hr>
    <p class="regy">Y: <span id="regy"></span></p>
    <p class="regz">Z: <span id="regz"></span></p>
    <p class="regt">T: <span id="regt"></span></p>
    <p class="regl">L: <span id="regl"></span></p>
    <hr>
    <p class="reg0">0: <span id="reg0"></span></p>
    <p class="reg1">1: <span id="reg1"></span></p>
    <p class="reg2">2: <span id="reg2"></span></p>
    <p class="reg3">3: <span id="reg3"></span></p>
    <p class="reg4">4: <span id="reg4"></span></p>
    <p class="reg5">5: <span id="reg5"></span></p>
    <p class="reg6">6: <span id="reg6"></span></p>
    <p class="reg7">7: <span id="reg7"></span></p>
    <p class="reg8">8: <span id="reg8"></span></p>
    <p class="reg9">9: <span id="reg9"></span></p>
  </div>
  <script>
    var gateway = `ws://${window.location.hostname}/ws`;
    var websocket;
    window.addEventListener('load', onLoad);
    function initWebSocket() {
      websocket = new WebSocket(gateway);
      websocket.onopen = onOpen;
      websocket.onclose = onClose;
      websocket.onmessage = onMessage; 
    }
    function onOpen(event) {
    }
    function onClose(event) {
      document.getElementById('regx').innerHTML = "Disconnected"
      setTimeout(initWebSocket, 2000);
    }
    function onMessage(event) {
      let message = "";
      let identifier = "";

      message = event.data;
      identifier = message.substring(0, 2);

      switch (identifier) {
        case "X:":
          document.getElementById('regx').innerHTML = message.substring(2);
          break;
        case "Y:":
          document.getElementById('regy').innerHTML = message.substring(2);
          break;
        case "Z:":
          document.getElementById('regz').innerHTML = message.substring(2);
          break;
        case "T:":
          document.getElementById('regt').innerHTML = message.substring(2);
          break;
        case "L:":
          document.getElementById('regl').innerHTML = message.substring(2);
          break;
        case "0:":
          document.getElementById('reg0').innerHTML = message.substring(2);
          break;
        case "1:":
          document.getElementById('reg1').innerHTML = message.substring(2);
          break;
        case "2:":
          document.getElementById('reg2').innerHTML = message.substring(2);
          break;
        case "3:":
          document.getElementById('reg3').innerHTML = message.substring(2);
          break;
        case "4:":
          document.getElementById('reg4').innerHTML = message.substring(2);
          break;
        case "5:":
          document.getElementById('reg5').innerHTML = message.substring(2);
          break;
        case "6:":
          document.getElementById('reg6').innerHTML = message.substring(2);
          break;
        case "7:":
          document.getElementById('reg7').innerHTML = message.substring(2);
          break;
        case "8:":
          document.getElementById('reg8').innerHTML = message.substring(2);
          break;
        case "9:":
          document.getElementById('reg9').innerHTML = message.substring(2);
          break;
      }
    }
    function onLoad(event) {
      initWebSocket();
    }
  </script>
</body>

</html>
)rawliteral";

#else

const char indexHTML[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
  <title>RPN Nixie Calculator Server</title>
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

    .regx {
      color: white;
      background-color: indigo;
    }
  </style>
  <title>Nixie Calculator Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
</head>

<body>
  <div class="headline">
    <h1>Nixie Calculator</h1>
  </div>
  <div class="content">
    <p class="regx">X: <span id="regx"></span></p>
    <hr>
    <p class="regy">Y: <span id="regy"></span></p>
    <p class="regt">T: <span id="regt"></span></p>
    <hr>
    <p class="regm">M: <span id="regm"></span></p>
  </div>
  <script>
    var gateway = `ws://${window.location.hostname}/ws`;
    var websocket;
    window.addEventListener('load', onLoad);
    function initWebSocket() {
      websocket = new WebSocket(gateway);
      websocket.onopen = onOpen;
      websocket.onclose = onClose;
      websocket.onmessage = onMessage; 
    }
    function onOpen(event) {
    }
    function onClose(event) {
      document.getElementById('regx').innerHTML = "Disconnected"
      setTimeout(initWebSocket, 2000);
    }
    function onMessage(event) {
      let message = "";
      let identifier = "";

      message = event.data;
      identifier = message.substring(0, 2);

      switch (identifier) {
        case "X:":
          document.getElementById('regx').innerHTML = message.substring(2);
          break;
        case "Y:":
          document.getElementById('regy').innerHTML = message.substring(2);
          break;
        case "T:":
          document.getElementById('regt').innerHTML = message.substring(2);
          break;
        case "M:":
          document.getElementById('regm').innerHTML = message.substring(2);
          break;
      }
    }
    function onLoad(event) {
      initWebSocket();
    }
  </script>
</body>

</html>
)rawliteral";

#endif