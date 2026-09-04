// HTMLCalculator.h

// provides the virtual calculator page that mirrors the physical calculator keyboard

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Config.h>

#if RPN_MODE

const char htmlCalculator[] = R"rawliteral(
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
      max-width: 640px;
      margin: 0 auto;
      padding: 15px 5px;
      overflow-x: auto;
      --key: clamp(28px, calc((100vw - 20px) / 9.3), 56px);
    }

    .display {
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 2px;
      background-color: #1a1a1a;
      color: #ff9d3c;
      font-family: 'Courier New', monospace;
      padding: 4px 8px;
      border-radius: 6px;
      margin: 0 auto 10px;
      width: calc(var(--key) * 9.13);
      max-width: 100%;
      box-sizing: border-box;
      overflow-x: auto;
    }

    .display-sign,
    .display-exp {
      display: inline-block;
      flex: 1 1 0;
      min-width: 0;
      text-align: center;
      font-size: max(0.9rem, calc(var(--key) * 0.5));
      font-weight: bold;
      text-shadow: 0 0 6px rgba(255, 157, 60, 0.6);
      white-space: pre;
    }

    .display-digits {
      display: flex;
      gap: 2px;
      flex: 14 1 0;
      min-width: 0;
    }

    .digit-cell {
      position: relative;
      display: flex;
      align-items: center;
      justify-content: center;
      flex: 1 1 0;
      min-width: 0;
      height: max(1.9rem, calc(var(--key) * 0.95));
    }

    .digit-glow {
      position: absolute;
      top: 6px;
      bottom: 6px;
      left: calc(20% - 1px);
      right: calc(20% - 1px);
      border: 1px solid transparent;
      border-top-width: 2px;
      border-bottom-width: 2px;
      border-radius: 4px;
      z-index: 0;
    }

    .digit-num {
      position: relative;
      z-index: 1;
      font-size: max(0.9rem, calc(var(--key) * 0.5));
      font-weight: bold;
      text-shadow: 0 0 6px rgba(255, 157, 60, 0.6);
    }

    .digit-dot {
      position: absolute;
      right: -2px;
      bottom: 10px;
      width: 2.5px;
      height: 2.5px;
      border-radius: 50%;
      background-color: #3a2c1a;
      z-index: 1;
    }

    .digit-dot.on {
      background-color: #ff9d3c;
      box-shadow: 0 0 4px rgba(255, 157, 60, 0.8);
    }

    .seven-segment {
      position: relative;
      display: flex;
      align-items: center;
      justify-content: center;
      flex: 1 1 0;
      min-width: 0;
      width: 100%;
      height: max(1.5rem, calc(var(--key) * 0.78));
      box-sizing: border-box;
    }

    .seg-digit {
      position: relative;
      width: 55%;
      height: 88%;
    }

    .seg {
      position: absolute;
      background-color: #3a2c1a;
      transition: background-color 0.1s, box-shadow 0.1s;
    }

    .seg.on {
      background-color: #ff2020;
      box-shadow: 0 0 4px rgba(255, 32, 32, 0.85);
    }

    .seg-a,
    .seg-g,
    .seg-d {
      left: 12%;
      right: 12%;
      height: 12%;
      clip-path: polygon(8% 50%, 20% 0%, 80% 0%, 92% 50%, 80% 100%, 20% 100%);
    }

    .seg-a {
      top: 0;
    }

    .seg-g {
      top: 44%;
    }

    .seg-d {
      bottom: 0;
    }

    .seg-f,
    .seg-b,
    .seg-e,
    .seg-c {
      width: 16%;
      height: 44%;
      clip-path: polygon(50% 8%, 100% 20%, 100% 80%, 50% 92%, 0% 80%, 0% 20%);
    }

    .seg-f,
    .seg-b {
      top: 6%;
    }

    .seg-e,
    .seg-c {
      bottom: 6%;
    }

    .seg-f,
    .seg-e {
      left: 0;
    }

    .seg-b,
    .seg-c {
      right: 0;
    }

    .seven-segment .digit-dot {
      position: absolute;
      right: 0;
      bottom: 2%;
      width: 3px;
      height: 3px;
      border-radius: 1px;
      background-color: #3a2c1a;
      z-index: 1;
    }

    .seven-segment .digit-dot.on {
      background-color: #ff2020;
      box-shadow: 0 0 4px rgba(255, 32, 32, 0.8);
    }

    .footer-row {
      display: flex;
      flex-wrap: wrap;
      gap: 8px;
      justify-content: center;
      align-items: center;
      margin-bottom: 12px;
    }

    .mode-button {
      display: inline-block;
      margin: 0;
      padding: 4px 10px;
      font-size: 0.75rem;
      background-color: white;
      border: 1px solid #143642;
      color: #143642;
      border-radius: 4px;
      cursor: pointer;
    }

    .mode-button:hover {
      background-color: #143642;
      color: white;
    }

    .keypad {
      display: flex;
      gap: calc(var(--key) * 0.29);
      justify-content: center;
      flex-wrap: nowrap;
      width: max-content;
      margin: 0 auto;
      transition: opacity 0.2s;
    }

    .keypad.disabled {
      opacity: 0.35;
      pointer-events: none;
    }

    .registers-panel {
      display: none;
      text-align: left;
      margin-top: 15px;
      transition: opacity 0.2s;
    }

    .registers-panel.visible {
      display: block;
    }

    .registers-panel.disabled {
      opacity: 0.35;
    }

    .registers-panel p {
      margin: 4px 0;
      padding: 2px 4px;
    }

    .regx {
      color: white;
      background-color: saddlebrown;
    }

    .block {
      display: grid;
      gap: calc(var(--key) * 0.11);
      grid-auto-rows: var(--key);
      align-content: start;
    }

    .block-left {
      grid-template-columns: repeat(3, var(--key));
    }

    .block-mid {
      grid-template-columns: repeat(4, var(--key));
    }

    .block-right {
      grid-template-columns: var(--key);
    }

    .key {
      width: var(--key);
      height: var(--key);
      border: none;
      border-radius: calc(var(--key) * 0.11);
      font-size: max(10px, calc(var(--key) * 0.32));
      display: table-cell;
      vertical-align: middle;
      text-align: center;
      cursor: pointer;
      user-select: none;
      -webkit-touch-callout: none;
      touch-action: manipulation;
      padding: 0;
    }

    .block-mid .key {
      font-size: max(12px, calc(var(--key) * 0.4));
    }

    .key-fn {
      background-color: #202020;
      color: white;
      transition: color 0.15s;
    }

    .key-fn.tint-shift {
      color: #ffab42;
    }

    .key-fn.tint-func {
      color: #4db4ec;
    }

    .key-num {
      background-color: #e8e2d5;
      color: #222;
    }

    .key-enter {
      background-color: white;
      color: #143642;
      border: 1px solid #143642;
      font-weight: bold;
    }

    .key-mod {
      background-color: #111;
    }

    #shiftKey {
      color: #ffab42;
      font-size: max(15px, calc(var(--key) * 0.44));
      font-weight: bold;
    }

    .key-arrow {
      font-size: max(15px, calc(var(--key) * 0.44));
    }

    #funcKey {
      color: #4db4ec;
      font-size: max(15px, calc(var(--key) * 0.44));
      font-weight: bold;
    }

    .key-mod.active {
      outline: 3px solid white;
    }

    #backButton {
      display: inline-block;
      margin: 0;
      padding: 4px 14px;
      font-size: 0.75rem;
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
    <h1>RPN Nixie Calculator</h1>
  </div>
  <div class="content">
    <div class="footer-row">
      <button id="switchModeButton" class="mode-button" onclick="switchMode()">Switch Mode</button>
      <button id="menuModeButton" class="mode-button" onclick="enterMenuMode()">Menu</button>
      <button id="toggleRegistersButton" class="mode-button" onclick="toggleRegisters()">Show Registers</button>
      <a href="/" id="backButton">Back</a>
    </div>
    <hr>
    <div id="display" class="display">
      <span id="displaySign" class="display-sign">&nbsp;</span>
      <div id="displayDigits" class="display-digits"></div>
      <span id="displayExp" class="display-exp"></span>
    </div>
    <div id="keypad" class="keypad disabled">
      <div class="block block-left">
        <button class="key key-fn" onclick="sendKey(1)">EXP</button>
        <button class="key key-fn" data-code="6" onclick="sendKey(6)">x<sup>y</sup></button>
        <button class="key key-fn" data-code="11" onclick="sendKey(11)">sin</button>
        <button class="key key-fn" data-code="2" onclick="sendKey(2)"><span class="key-arrow">&#11013;</span></button>
        <button class="key key-fn" data-code="7" onclick="sendKey(7)"><sup>y</sup>&radic;<sub>x</sub></button>
        <button class="key key-fn" data-code="12" onclick="sendKey(12)">cos</button>
        <button class="key key-fn" onclick="sendKey(3)">CLS</button>
        <button class="key key-fn" data-code="8" onclick="sendKey(8)">1/x</button>
        <button class="key key-fn" data-code="13" onclick="sendKey(13)">tan</button>
        <button class="key key-mod" id="shiftKey" onclick="setMod('shift')">&#11014;</button>
        <button class="key key-fn" data-code="9" onclick="sendKey(9)">ln</button>
        <button class="key key-fn" data-code="14" onclick="sendKey(14)">log</button>
        <button class="key key-mod" id="funcKey" onclick="setMod('func')">F</button>
        <button class="key key-fn" data-code="10" onclick="sendKey(10)">log<sub>y</sub></button>
        <button class="key key-fn" data-code="15" onclick="sendKey(15)">d&harr;r</button>
      </div>
      <div class="block block-mid">
        <button class="key key-fn" onclick="sendKey(16)">&plusmn;</button>
        <button class="key key-fn" onclick="sendKey(21)">&radic;</button>
        <button class="key key-fn" onclick="sendKey(26)">%</button>
        <button class="key key-fn" onclick="sendKey(31)">&divide;</button>
        <button class="key key-num" onclick="sendKey(17)">7</button>
        <button class="key key-num" onclick="sendKey(22)">8</button>
        <button class="key key-num" onclick="sendKey(27)">9</button>
        <button class="key key-fn" onclick="sendKey(32)">&times;</button>
        <button class="key key-num" onclick="sendKey(18)">4</button>
        <button class="key key-num" onclick="sendKey(23)">5</button>
        <button class="key key-num" onclick="sendKey(28)">6</button>
        <button class="key key-fn" onclick="sendKey(33)">&minus;</button>
        <button class="key key-num" onclick="sendKey(19)">1</button>
        <button class="key key-num" onclick="sendKey(24)">2</button>
        <button class="key key-num" onclick="sendKey(29)">3</button>
        <button class="key key-fn" onclick="sendKey(34)">+</button>
        <button class="key key-num" onclick="sendKey(20)">0</button>
        <button class="key key-num" onclick="sendKey(25)">00</button>
        <button class="key key-num" onclick="sendKey(30)">&bull;</button>
        <button class="key key-enter" onclick="sendKey(35)">ENT</button>
      </div>
      <div class="block block-right">
        <button class="key key-fn" data-code="36" onclick="sendKey(36)">X&harr;Y</button>
        <button class="key key-fn" data-code="37" onclick="sendKey(37)">R&darr;</button>
        <button class="key key-fn" onclick="sendKey(38)">CLR</button>
        <button class="key key-fn" onclick="sendKey(39)">STO</button>
        <button class="key key-fn" onclick="sendKey(40)">RCL</button>
      </div>
    </div>
    <div id="registersPanel" class="registers-panel">
      <p class="regx">X: <span id="regX"></span></p>
      <hr>
      <p class="regy">Y: <span id="regY"></span></p>
      <p class="regz">Z: <span id="regZ"></span></p>
      <p class="regt">T: <span id="regT"></span></p>
      <p class="regl">L: <span id="regL"></span></p>
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
  </div>
  <script>
    var gateway = `ws://${window.location.hostname}/ws`;
    var websocket;
    var pendingMod = null;
    var keyLabels = {
      2: { base: '<span class="key-arrow">&#11013;</span>', shift: 'clx' },
      6: { base: 'x<sup>y</sup>', func: 'x<sup>2</sup>' },
      7: { base: '<sup>y</sup>&radic;<sub>x</sub>', func: 'x<sup>3</sup>' },
      8: { base: '1/x', shift: 'n!', func: 'e<sup>x</sup>' },
      9: { base: 'ln', shift: 'e', func: 'mod' },
      10: { base: 'log<sub>y</sub>', shift: 'p<sub>y,x</sub>' },
      11: { base: 'sin', shift: 'sin<sup>-1</sup>', func: 'sinh' },
      12: { base: 'cos', shift: 'cos<sup>-1</sup>', func: 'cosh' },
      13: { base: 'tan', shift: 'tan<sup>-1</sup>', func: 'tanh' },
      14: { base: 'log', shift: '&pi;', func: 'rnd' },
      15: { base: 'd&harr;r', shift: 'c<sub>y,x</sub>', func: '&Delta;%' },
      36: { base: 'X&harr;Y', shift: 'lstx' },
      37: { base: 'R&darr;', shift: 'R&uarr;' }
    };
    var regElementMap = { "X:": "regX", "Y:": "regY", "Z:": "regZ", "T:": "regT", "L:": "regL" };
    for (var regIndex = 0; regIndex <= 9; regIndex++) {
      regElementMap[regIndex + ":"] = "reg" + regIndex;
    }
    var connected = false;
    var deviceMode = "";
    var lastSign = " ";
    var lastDigits = "";
    var lastDpBits = "";
    var lastExpSign = "";
    var lastLedColors = [];
    var isLedDisplay = false;
    var segMap = {
      '0': 'abcdef', '1': 'bc', '2': 'abged', '3': 'abgcd', '4': 'fgbc',
      '5': 'afgcd', '6': 'afgecd', '7': 'abc', '8': 'abcdefg', '9': 'abcdfg', ' ': ''
    };
    var switchModeLabels = { "Menu": "Exit menu", "Clock": "Calculator", "Calculator": "Clock", "Antipoisoning": "Stop ACP" };
    window.addEventListener('load', onLoad);
    function updateSwitchModeButton() {
      document.getElementById('switchModeButton').textContent = switchModeLabels[deviceMode] || "Switch Mode";
    }
    function createDigitCell() {
      var cell = document.createElement('div');
      cell.className = 'digit-cell';
      var glow = document.createElement('span');
      glow.className = 'digit-glow';
      var num = document.createElement('span');
      num.className = 'digit-num';
      var dot = document.createElement('span');
      dot.className = 'digit-dot';
      cell.appendChild(glow);
      cell.appendChild(num);
      cell.appendChild(dot);
      return cell;
    }
    function createSevenSegCell() {
      var cell = document.createElement('div');
      cell.className = 'seven-segment';
      var segDigit = document.createElement('span');
      segDigit.className = 'seg-digit';
      'abcdefg'.split('').forEach(function (s) {
        var seg = document.createElement('i');
        seg.className = 'seg seg-' + s;
        segDigit.appendChild(seg);
      });
      var dot = document.createElement('span');
      dot.className = 'digit-dot';
      cell.appendChild(segDigit);
      cell.appendChild(dot);
      return cell;
    }
    function setSegments(cell, lit) {
      var segEls = cell.querySelectorAll('.seg');
      for (var s = 0; s < segEls.length; s++) {
        var letter = segEls[s].className.match(/seg-([a-g])/)[1];
        segEls[s].classList.toggle('on', lit.indexOf(letter) !== -1);
      }
    }
    function updateDisplayCells(digits, dpBits, ledColors) {
      var container = document.getElementById('displayDigits');
      while (container.children.length > digits.length) {
        container.removeChild(container.lastChild);
      }
      for (var i = 0; i < digits.length; i++) {
        var cell = container.children[i];
        if (!cell) {
          cell = isLedDisplay ? createSevenSegCell() : createDigitCell();
          container.appendChild(cell);
        }
        if (isLedDisplay) {
          setSegments(cell, segMap[digits[i]] || '');
          cell.querySelector('.digit-dot').classList.toggle('on', dpBits[i] === '1');
        } else {
          cell.children[1].textContent = digits[i];
          cell.children[2].classList.toggle('on', dpBits[i] === '1');
          var color = ledColors[i];
          var colorLit = color && (color !== '000000');
          var lineColor = colorLit ? ('#' + color) : 'transparent';
          cell.children[0].style.borderColor = lineColor;
        }
      }
    }
    function updateSignField(id, value) {
      var el = document.getElementById(id);
      if (isLedDisplay) {
        var cell = el.querySelector('.seven-segment');
        if (!cell) {
          el.textContent = '';
          cell = createSevenSegCell();
          el.appendChild(cell);
        }
        setSegments(cell, value === '-' ? 'g' : '');
      } else {
        el.textContent = value;
      }
    }
    function updateDisplay() {
      if (!connected) {
        document.getElementById('displaySign').textContent = "Disconnected";
        document.getElementById('displayDigits').innerHTML = "";
        document.getElementById('displayExp').textContent = "";
      } else {
        updateSignField('displaySign', lastSign);
        updateDisplayCells(lastDigits, lastDpBits, lastLedColors);
        updateSignField('displayExp', lastExpSign);
      }
      document.getElementById('keypad').classList.toggle('disabled', !connected);
      document.getElementById('registersPanel').classList.toggle('disabled', !connected);
    }
    function initWebSocket() {
      connected = false;
      deviceMode = "";
      updateSwitchModeButton();
      updateDisplay();
      websocket = new WebSocket(gateway);
      websocket.onopen = onOpen;
      websocket.onclose = onClose;
      websocket.onmessage = onMessage;
    }
    function onOpen(event) {
      connected = true;
      updateDisplay();
      if (document.getElementById('registersPanel').classList.contains('visible')) {
        websocket.send('R1');
      }
    }
    function onClose(event) {
      connected = false;
      deviceMode = "";
      updateSwitchModeButton();
      updateDisplay();
      setTimeout(initWebSocket, 2000);
    }
    function onMessage(event) {
      var message = event.data;
      if (message.substring(0, 5) === "MODE:") {
        deviceMode = message.substring(5);
        updateSwitchModeButton();
        return;
      }
      if (message.substring(0, 5) === "TYPE:") {
        var wasLedDisplay = isLedDisplay;
        isLedDisplay = message.substring(5) === "7-segment LED";
        if (isLedDisplay !== wasLedDisplay) {
          document.getElementById('displayDigits').innerHTML = "";
          updateDisplay();
        }
        return;
      }
      if (message.substring(0, 2) === "S:") {
        var parts = message.substring(2).split('|');
        lastSign = parts[0] || " ";
        lastDigits = parts[1] || "";
        lastDpBits = parts[2] || "";
        lastExpSign = parts[3] || "";
        lastLedColors = (parts[4] || '').split(',');
        updateDisplay();
        return;
      }
      var id = message.substring(0, 2);
      if (regElementMap[id]) {
        var el = document.getElementById(regElementMap[id]);
        if (el) {
          el.textContent = message.substring(2);
        }
      }
    }
    function onLoad(event) {
      initWebSocket();
    }
    function sendCommand(code) {
      if (websocket && (websocket.readyState === WebSocket.OPEN)) {
        websocket.send('P' + code + ',0,0');
      }
    }
    function enterMenuMode() {
      sendCommand(253);
    }
    function switchMode() {
      sendCommand(254);
    }
    function toggleRegisters() {
      var panel = document.getElementById('registersPanel');
      var visible = panel.classList.toggle('visible');
      document.getElementById('toggleRegistersButton').textContent = visible ? 'Hide Registers' : 'Show Registers';
      if (websocket && (websocket.readyState === WebSocket.OPEN)) {
        websocket.send(visible ? 'R1' : 'R0');
      }
    }
    function updateLabels() {
      for (var code in keyLabels) {
        var entry = keyLabels[code];
        var el = document.querySelector('.key[data-code="' + code + '"]');
        if (!el) continue;
        var text = entry.base;
        if ((pendingMod === 'shift') && entry.shift) text = entry.shift;
        if ((pendingMod === 'func') && entry.func) text = entry.func;
        el.innerHTML = text;
        el.classList.toggle('tint-shift', (pendingMod === 'shift') && !!entry.shift);
        el.classList.toggle('tint-func', (pendingMod === 'func') && !!entry.func);
      }
    }
    function setMod(mod) {
      pendingMod = (pendingMod === mod) ? null : mod;
      document.getElementById('shiftKey').classList.toggle('active', pendingMod === 'shift');
      document.getElementById('funcKey').classList.toggle('active', pendingMod === 'func');
      updateLabels();
    }
    function sendKey(code) {
      if (websocket && (websocket.readyState === WebSocket.OPEN)) {
        var func = (pendingMod === 'func') ? 1 : 0;
        var shift = (pendingMod === 'shift') ? 1 : 0;
        websocket.send('P' + code + ',' + func + ',' + shift);
      }
      pendingMod = null;
      document.getElementById('shiftKey').classList.remove('active');
      document.getElementById('funcKey').classList.remove('active');
      updateLabels();
    }
  </script>
</body>

</html>
)rawliteral";

#else

const char htmlCalculator[] = R"rawliteral(
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
      max-width: 640px;
      margin: 0 auto;
      padding: 15px 5px;
      overflow-x: auto;
      --key: clamp(28px, calc((100vw - 20px) / 9.3), 56px);
    }

    .display {
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 2px;
      background-color: #1a1a1a;
      color: #ff9d3c;
      font-family: 'Courier New', monospace;
      padding: 4px 8px;
      border-radius: 6px;
      margin: 0 auto 10px;
      width: calc(var(--key) * 9.13);
      max-width: 100%;
      box-sizing: border-box;
      overflow-x: auto;
    }

    .display-sign,
    .display-exp {
      display: inline-block;
      flex: 1 1 0;
      min-width: 0;
      text-align: center;
      font-size: max(0.9rem, calc(var(--key) * 0.5));
      font-weight: bold;
      text-shadow: 0 0 6px rgba(255, 157, 60, 0.6);
      white-space: pre;
    }

    .display-digits {
      display: flex;
      gap: 2px;
      flex: 14 1 0;
      min-width: 0;
    }

    .digit-cell {
      position: relative;
      display: flex;
      align-items: center;
      justify-content: center;
      flex: 1 1 0;
      min-width: 0;
      height: max(1.9rem, calc(var(--key) * 0.95));
    }

    .digit-glow {
      position: absolute;
      top: 6px;
      bottom: 6px;
      left: calc(20% - 1px);
      right: calc(20% - 1px);
      border: 1px solid transparent;
      border-top-width: 2px;
      border-bottom-width: 2px;
      border-radius: 4px;
      z-index: 0;
    }

    .digit-num {
      position: relative;
      z-index: 1;
      font-size: max(0.9rem, calc(var(--key) * 0.5));
      font-weight: bold;
      text-shadow: 0 0 6px rgba(255, 157, 60, 0.6);
    }

    .digit-dot {
      position: absolute;
      right: -2px;
      bottom: 10px;
      width: 2.5px;
      height: 2.5px;
      border-radius: 50%;
      background-color: #3a2c1a;
      z-index: 1;
    }

    .digit-dot.on {
      background-color: #ff9d3c;
      box-shadow: 0 0 4px rgba(255, 157, 60, 0.8);
    }

    .seven-segment {
      position: relative;
      display: flex;
      align-items: center;
      justify-content: center;
      flex: 1 1 0;
      min-width: 0;
      width: 100%;
      height: max(1.5rem, calc(var(--key) * 0.78));
      box-sizing: border-box;
    }

    .seg-digit {
      position: relative;
      width: 55%;
      height: 88%;
    }

    .seg {
      position: absolute;
      background-color: #3a2c1a;
      transition: background-color 0.1s, box-shadow 0.1s;
    }

    .seg.on {
      background-color: #ff2020;
      box-shadow: 0 0 4px rgba(255, 32, 32, 0.85);
    }

    .seg-a,
    .seg-g,
    .seg-d {
      left: 12%;
      right: 12%;
      height: 12%;
      clip-path: polygon(8% 50%, 20% 0%, 80% 0%, 92% 50%, 80% 100%, 20% 100%);
    }

    .seg-a {
      top: 0;
    }

    .seg-g {
      top: 44%;
    }

    .seg-d {
      bottom: 0;
    }

    .seg-f,
    .seg-b,
    .seg-e,
    .seg-c {
      width: 16%;
      height: 44%;
      clip-path: polygon(50% 8%, 100% 20%, 100% 80%, 50% 92%, 0% 80%, 0% 20%);
    }

    .seg-f,
    .seg-b {
      top: 6%;
    }

    .seg-e,
    .seg-c {
      bottom: 6%;
    }

    .seg-f,
    .seg-e {
      left: 0;
    }

    .seg-b,
    .seg-c {
      right: 0;
    }

    .seven-segment .digit-dot {
      position: absolute;
      right: 0;
      bottom: 2%;
      width: 3px;
      height: 3px;
      border-radius: 1px;
      background-color: #3a2c1a;
      z-index: 1;
    }

    .seven-segment .digit-dot.on {
      background-color: #ff2020;
      box-shadow: 0 0 4px rgba(255, 32, 32, 0.8);
    }

    .footer-row {
      display: flex;
      flex-wrap: wrap;
      gap: 8px;
      justify-content: center;
      align-items: center;
      margin-bottom: 12px;
    }

    .mode-button {
      display: inline-block;
      margin: 0;
      padding: 4px 10px;
      font-size: 0.75rem;
      background-color: white;
      border: 1px solid #143642;
      color: #143642;
      border-radius: 4px;
      cursor: pointer;
    }

    .mode-button:hover {
      background-color: #143642;
      color: white;
    }

    .keypad {
      display: flex;
      gap: calc(var(--key) * 0.29);
      justify-content: center;
      flex-wrap: nowrap;
      width: max-content;
      margin: 0 auto;
      transition: opacity 0.2s;
    }

    .keypad.disabled {
      opacity: 0.35;
      pointer-events: none;
    }

    .registers-panel {
      display: none;
      text-align: left;
      margin-top: 15px;
      transition: opacity 0.2s;
    }

    .registers-panel.visible {
      display: block;
    }

    .registers-panel.disabled {
      opacity: 0.35;
    }

    .registers-panel p {
      margin: 4px 0;
      padding: 2px 4px;
    }

    .regx {
      color: white;
      background-color: indigo;
    }

    .block {
      display: grid;
      gap: calc(var(--key) * 0.11);
      grid-auto-rows: var(--key);
      align-content: start;
    }

    .block-left {
      grid-template-columns: repeat(3, var(--key));
    }

    .block-mid {
      grid-template-columns: repeat(4, var(--key));
    }

    .block-right {
      grid-template-columns: var(--key);
    }

    .key {
      width: var(--key);
      height: var(--key);
      border: none;
      border-radius: calc(var(--key) * 0.11);
      font-size: max(10px, calc(var(--key) * 0.32));
      display: table-cell;
      vertical-align: middle;
      text-align: center;
      cursor: pointer;
      user-select: none;
      -webkit-touch-callout: none;
      touch-action: manipulation;
      padding: 0;
    }

    .block-mid .key {
      font-size: max(12px, calc(var(--key) * 0.4));
    }

    .key-fn {
      background-color: #202020;
      color: white;
      transition: color 0.15s;
    }

    .key-fn.tint-shift {
      color: #ffab42;
    }

    .key-fn.tint-func {
      color: #4db4ec;
    }

    .key-num {
      background-color: #e8e2d5;
      color: #222;
    }

    .key-enter {
      background-color: white;
      color: #143642;
      border: 1px solid #143642;
      font-weight: bold;
    }

    .key-mod {
      background-color: #111;
    }

    #shiftKey {
      color: #ffab42;
      font-size: max(15px, calc(var(--key) * 0.44));
      font-weight: bold;
    }

    .key-arrow {
      font-size: max(15px, calc(var(--key) * 0.44));
    }

    #funcKey {
      color: #4db4ec;
      font-size: max(15px, calc(var(--key) * 0.44));
      font-weight: bold;
    }

    .key-mod.active {
      outline: 3px solid white;
    }

    #backButton {
      display: inline-block;
      margin: 0;
      padding: 4px 14px;
      font-size: 0.75rem;
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
    <h1>Nixie Calculator</h1>
  </div>
  <div class="content">
    <div class="footer-row">
      <button id="switchModeButton" class="mode-button" onclick="switchMode()">Switch Mode</button>
      <button id="menuModeButton" class="mode-button" onclick="enterMenuMode()">Menu</button>
      <button id="toggleRegistersButton" class="mode-button" onclick="toggleRegisters()">Show Registers</button>
      <a href="/" id="backButton">Back</a>
    </div>
    <hr>
    <div id="display" class="display">
      <span id="displaySign" class="display-sign">&nbsp;</span>
      <div id="displayDigits" class="display-digits"></div>
      <span id="displayExp" class="display-exp"></span>
    </div>
    <div id="keypad" class="keypad disabled">
      <div class="block block-left">
        <button class="key key-fn" onclick="sendKey(1)">EXP</button>
        <button class="key key-fn" data-code="6" onclick="sendKey(6)">x<sup>y</sup></button>
        <button class="key key-fn" data-code="11" onclick="sendKey(11)">sin</button>
        <button class="key key-fn" onclick="sendKey(2)">C</button>
        <button class="key key-fn" data-code="7" onclick="sendKey(7)"><sup>y</sup>&radic;x</button>
        <button class="key key-fn" data-code="12" onclick="sendKey(12)">cos</button>
        <button class="key key-fn" onclick="sendKey(3)">AC</button>
        <button class="key key-fn" data-code="8" onclick="sendKey(8)">1/x</button>
        <button class="key key-fn" data-code="13" onclick="sendKey(13)">tan</button>
        <button class="key key-mod" id="shiftKey" onclick="setMod('shift')">&#11014;</button>
        <button class="key key-fn" data-code="9" onclick="sendKey(9)">ln</button>
        <button class="key key-fn" data-code="14" onclick="sendKey(14)">log</button>
        <button class="key key-mod" id="funcKey" onclick="setMod('func')">F</button>
        <button class="key key-fn" data-code="10" onclick="sendKey(10)">log<sub>y</sub></button>
        <button class="key key-fn" data-code="15" onclick="sendKey(15)">d&harr;r</button>
      </div>
      <div class="block block-mid">
        <button class="key key-fn" onclick="sendKey(16)">&plusmn;</button>
        <button class="key key-fn" onclick="sendKey(21)">&radic;</button>
        <button class="key key-fn" onclick="sendKey(26)">%</button>
        <button class="key key-fn" onclick="sendKey(31)">&divide;</button>
        <button class="key key-num" onclick="sendKey(17)">7</button>
        <button class="key key-num" onclick="sendKey(22)">8</button>
        <button class="key key-num" onclick="sendKey(27)">9</button>
        <button class="key key-fn" onclick="sendKey(32)">&times;</button>
        <button class="key key-num" onclick="sendKey(18)">4</button>
        <button class="key key-num" onclick="sendKey(23)">5</button>
        <button class="key key-num" onclick="sendKey(28)">6</button>
        <button class="key key-fn" onclick="sendKey(33)">&minus;</button>
        <button class="key key-num" onclick="sendKey(19)">1</button>
        <button class="key key-num" onclick="sendKey(24)">2</button>
        <button class="key key-num" onclick="sendKey(29)">3</button>
        <button class="key key-fn" onclick="sendKey(34)">+</button>
        <button class="key key-num" onclick="sendKey(20)">0</button>
        <button class="key key-num" onclick="sendKey(25)">00</button>
        <button class="key key-num" onclick="sendKey(30)">&bull;</button>
        <button class="key key-enter" onclick="sendKey(35)">=</button>
      </div>
      <div class="block block-right">
        <button class="key key-fn" onclick="sendKey(36)">MC</button>
        <button class="key key-fn" onclick="sendKey(37)">MR</button>
        <button class="key key-fn" onclick="sendKey(38)">MS</button>
        <button class="key key-fn" onclick="sendKey(39)">M+</button>
        <button class="key key-fn" onclick="sendKey(40)">M-</button>
      </div>
    </div>
    <div id="registersPanel" class="registers-panel">
      <p class="regx">X: <span id="regX"></span></p>
      <hr>
      <p class="regy">Y: <span id="regY"></span></p>
      <p class="regt">T: <span id="regT"></span></p>
      <hr>
      <p class="regm">M: <span id="regM"></span></p>
    </div>
  </div>
  <script>
    var gateway = `ws://${window.location.hostname}/ws`;
    var websocket;
    var pendingMod = null;
    var keyLabels = {
      6: { base: 'x<sup>y</sup>', func: 'x<sup>2</sup>' },
      7: { base: '<sup>y</sup>&radic;<sub>x</sub>', func: 'x<sup>3</sup>' },
      8: { base: '1/x', shift: 'n!', func: 'e<sup>x</sup>' },
      9: { base: 'ln', shift: 'e', func: 'mod' },
      10: { base: 'log<sub>y</sub>', shift: 'p<sub>y,x</sub>' },
      11: { base: 'sin', shift: 'sin<sup>-1</sup>', func: 'sinh' },
      12: { base: 'cos', shift: 'cos<sup>-1</sup>', func: 'cosh' },
      13: { base: 'tan', shift: 'tan<sup>-1</sup>', func: 'tanh' },
      14: { base: 'log', shift: '&pi;', func: 'rnd' },
      15: { base: 'd&harr;r', shift: 'c<sub>y,x</sub>', func: '&Delta;%' }
    };
    var regElementMap = { "X:": "regX", "Y:": "regY", "T:": "regT", "M:": "regM" };
    var connected = false;
    var deviceMode = "";
    var lastSign = " ";
    var lastDigits = "";
    var lastDpBits = "";
    var lastExpSign = "";
    var lastLedColors = [];
    var isLedDisplay = false;
    var segMap = {
      '0': 'abcdef', '1': 'bc', '2': 'abged', '3': 'abgcd', '4': 'fgbc',
      '5': 'afgcd', '6': 'afgecd', '7': 'abc', '8': 'abcdefg', '9': 'abcdfg', ' ': ''
    };
    var switchModeLabels = { "Menu": "Exit menu", "Clock": "Calculator", "Calculator": "Clock", "Antipoisoning": "Stop ACP" };
    window.addEventListener('load', onLoad);
    function updateSwitchModeButton() {
      document.getElementById('switchModeButton').textContent = switchModeLabels[deviceMode] || "Switch Mode";
    }
    function createDigitCell() {
      var cell = document.createElement('div');
      cell.className = 'digit-cell';
      var glow = document.createElement('span');
      glow.className = 'digit-glow';
      var num = document.createElement('span');
      num.className = 'digit-num';
      var dot = document.createElement('span');
      dot.className = 'digit-dot';
      cell.appendChild(glow);
      cell.appendChild(num);
      cell.appendChild(dot);
      return cell;
    }
    function createSevenSegCell() {
      var cell = document.createElement('div');
      cell.className = 'seven-segment';
      var segDigit = document.createElement('span');
      segDigit.className = 'seg-digit';
      'abcdefg'.split('').forEach(function (s) {
        var seg = document.createElement('i');
        seg.className = 'seg seg-' + s;
        segDigit.appendChild(seg);
      });
      var dot = document.createElement('span');
      dot.className = 'digit-dot';
      cell.appendChild(segDigit);
      cell.appendChild(dot);
      return cell;
    }
    function setSegments(cell, lit) {
      var segEls = cell.querySelectorAll('.seg');
      for (var s = 0; s < segEls.length; s++) {
        var letter = segEls[s].className.match(/seg-([a-g])/)[1];
        segEls[s].classList.toggle('on', lit.indexOf(letter) !== -1);
      }
    }
    function updateDisplayCells(digits, dpBits, ledColors) {
      var container = document.getElementById('displayDigits');
      while (container.children.length > digits.length) {
        container.removeChild(container.lastChild);
      }
      for (var i = 0; i < digits.length; i++) {
        var cell = container.children[i];
        if (!cell) {
          cell = isLedDisplay ? createSevenSegCell() : createDigitCell();
          container.appendChild(cell);
        }
        if (isLedDisplay) {
          setSegments(cell, segMap[digits[i]] || '');
          cell.querySelector('.digit-dot').classList.toggle('on', dpBits[i] === '1');
        } else {
          cell.children[1].textContent = digits[i];
          cell.children[2].classList.toggle('on', dpBits[i] === '1');
          var color = ledColors[i];
          var colorLit = color && (color !== '000000');
          var lineColor = colorLit ? ('#' + color) : 'transparent';
          cell.children[0].style.borderColor = lineColor;
        }
      }
    }
    function updateSignField(id, value) {
      var el = document.getElementById(id);
      if (isLedDisplay) {
        var cell = el.querySelector('.seven-segment');
        if (!cell) {
          el.textContent = '';
          cell = createSevenSegCell();
          el.appendChild(cell);
        }
        setSegments(cell, value === '-' ? 'g' : '');
      } else {
        el.textContent = value;
      }
    }
    function updateDisplay() {
      if (!connected) {
        document.getElementById('displaySign').textContent = "Disconnected";
        document.getElementById('displayDigits').innerHTML = "";
        document.getElementById('displayExp').textContent = "";
      } else {
        updateSignField('displaySign', lastSign);
        updateDisplayCells(lastDigits, lastDpBits, lastLedColors);
        updateSignField('displayExp', lastExpSign);
      }
      document.getElementById('keypad').classList.toggle('disabled', !connected);
      document.getElementById('registersPanel').classList.toggle('disabled', !connected);
    }
    function initWebSocket() {
      connected = false;
      deviceMode = "";
      updateSwitchModeButton();
      updateDisplay();
      websocket = new WebSocket(gateway);
      websocket.onopen = onOpen;
      websocket.onclose = onClose;
      websocket.onmessage = onMessage;
    }
    function onOpen(event) {
      connected = true;
      updateDisplay();
      if (document.getElementById('registersPanel').classList.contains('visible')) {
        websocket.send('R1');
      }
    }
    function onClose(event) {
      connected = false;
      deviceMode = "";
      updateSwitchModeButton();
      updateDisplay();
      setTimeout(initWebSocket, 2000);
    }
    function onMessage(event) {
      var message = event.data;
      if (message.substring(0, 5) === "MODE:") {
        deviceMode = message.substring(5);
        updateSwitchModeButton();
        return;
      }
      if (message.substring(0, 5) === "TYPE:") {
        var wasLedDisplay = isLedDisplay;
        isLedDisplay = message.substring(5) === "7-segment LED";
        if (isLedDisplay !== wasLedDisplay) {
          document.getElementById('displayDigits').innerHTML = "";
          updateDisplay();
        }
        return;
      }
      if (message.substring(0, 2) === "S:") {
        var parts = message.substring(2).split('|');
        lastSign = parts[0] || " ";
        lastDigits = parts[1] || "";
        lastDpBits = parts[2] || "";
        lastExpSign = parts[3] || "";
        lastLedColors = (parts[4] || '').split(',');
        updateDisplay();
        return;
      }
      var id = message.substring(0, 2);
      if (regElementMap[id]) {
        var el = document.getElementById(regElementMap[id]);
        if (el) {
          el.textContent = message.substring(2);
        }
      }
    }
    function onLoad(event) {
      initWebSocket();
    }
    function sendCommand(code) {
      if (websocket && (websocket.readyState === WebSocket.OPEN)) {
        websocket.send('P' + code + ',0,0');
      }
    }
    function enterMenuMode() {
      sendCommand(253);
    }
    function switchMode() {
      sendCommand(254);
    }
    function toggleRegisters() {
      var panel = document.getElementById('registersPanel');
      var visible = panel.classList.toggle('visible');
      document.getElementById('toggleRegistersButton').textContent = visible ? 'Hide Registers' : 'Show Registers';
      if (websocket && (websocket.readyState === WebSocket.OPEN)) {
        websocket.send(visible ? 'R1' : 'R0');
      }
    }
    function updateLabels() {
      for (var code in keyLabels) {
        var entry = keyLabels[code];
        var el = document.querySelector('.key[data-code="' + code + '"]');
        if (!el) continue;
        var text = entry.base;
        if ((pendingMod === 'shift') && entry.shift) text = entry.shift;
        if ((pendingMod === 'func') && entry.func) text = entry.func;
        el.innerHTML = text;
        el.classList.toggle('tint-shift', (pendingMod === 'shift') && !!entry.shift);
        el.classList.toggle('tint-func', (pendingMod === 'func') && !!entry.func);
      }
    }
    function setMod(mod) {
      pendingMod = (pendingMod === mod) ? null : mod;
      document.getElementById('shiftKey').classList.toggle('active', pendingMod === 'shift');
      document.getElementById('funcKey').classList.toggle('active', pendingMod === 'func');
      updateLabels();
    }
    function sendKey(code) {
      if (websocket && (websocket.readyState === WebSocket.OPEN)) {
        var func = (pendingMod === 'func') ? 1 : 0;
        var shift = (pendingMod === 'shift') ? 1 : 0;
        websocket.send('P' + code + ',' + func + ',' + shift);
      }
      pendingMod = null;
      document.getElementById('shiftKey').classList.remove('active');
      document.getElementById('funcKey').classList.remove('active');
      updateLabels();
    }
  </script>
</body>

</html>
)rawliteral";

#endif
