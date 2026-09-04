// HTMLFirmware.h

// provides the firmware update (OTA) page

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

const char htmlFirmware[] = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
  <title>Nixie Calculator Firmware Update</title>
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

    .warning {
      text-align: left;
      background: #fff3cd;
      border: 1px solid #ffcc00;
      border-radius: 4px;
      padding: 10px;
      margin-bottom: 15px;
      font-size: 0.9rem;
    }

    .row {
      margin-bottom: 15px;
      text-align: left;
    }

    .row label {
      display: block;
      margin-bottom: 4px;
    }

    .password-wrap {
      position: relative;
      display: flex;
    }

    .password-wrap input {
      flex: 1 1 auto;
      padding: 6px 60px 6px 6px;
      font-size: 1rem;
      width: 100%;
      box-sizing: border-box;
    }

    .toggleVisibility {
      position: absolute;
      right: 4px;
      top: 50%;
      transform: translateY(-50%);
      background: none;
      border: none;
      color: #143642;
      font-size: 0.85rem;
      cursor: pointer;
      padding: 4px 6px;
    }

    .toggleVisibility:hover {
      text-decoration: underline;
    }

    input[type=file] {
      width: 100%;
      box-sizing: border-box;
    }

    #saveBar {
      text-align: center;
      padding: 10px 0;
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

    #uploadButton {
      background-color: #143642;
      border: 1px solid #143642;
      color: white;
      border-radius: 4px;
      cursor: pointer;
    }

    #uploadButton:hover {
      background-color: white;
      color: #143642;
    }

    #progressWrap {
      margin-top: 15px;
    }

    #progressBar {
      width: 100%;
      height: 16px;
      border: 1px solid #143642;
      border-radius: 4px;
      overflow: hidden;
    }

    #progressFill {
      height: 100%;
      width: 0%;
      background-color: #143642;
    }

    #progressText {
      display: block;
      margin-top: 4px;
      font-size: 0.9rem;
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
    <h1>Firmware Update</h1>
  </div>
  <div class="content">
    <div class="warning">
      The device must be physically armed first: press [F] + [&plusmn;] on its keyboard.
      Arming stays active for a few minutes, and the net activity LED blinks while
      armed. Do not power off or close this page while the update is in progress -
      an interrupted update may require reflashing the device over USB.
    </div>
    <div class="row">
      <label for="currentPassword">Firmware Update Password</label>
      <div class="password-wrap">
        <input type="password" id="currentPassword" autocomplete="current-password" required>
        <button type="button" class="toggleVisibility" data-target="currentPassword">Show</button>
      </div>
    </div>
    <div class="row">
      <label for="firmwareFile">Firmware file (.bin)</label>
      <input type="file" id="firmwareFile" accept=".bin" required>
    </div>
    <div id="saveBar">
      <a href="/" id="backButton">Back</a>
      <button type="button" id="uploadButton">Upload</button>
    </div>
    <div id="progressWrap" hidden>
      <div id="progressBar">
        <div id="progressFill"></div>
      </div>
      <span id="progressText">0%</span>
    </div>
    <span id="status"></span>
  </div>

  <script>
    document.querySelectorAll('.toggleVisibility').forEach((btn) => {
      btn.addEventListener('click', () => {
        const input = document.getElementById(btn.dataset.target);
        const isHidden = input.type === 'password';
        input.type = isHidden ? 'text' : 'password';
        btn.textContent = isHidden ? 'Hide' : 'Show';
      });
    });

    function showStatus(text, isError) {
      const status = document.getElementById('status');
      status.textContent = text;
      status.style.color = isError ? 'crimson' : 'green';
    }

    // must match MAX_FIRMWARE_SIZE in WebServer.hpp (the ota_0/ota_1 partition size)
    const MAX_FIRMWARE_SIZE = 3 * 1024 * 1024;

    document.getElementById('uploadButton').addEventListener('click', () => {
      const password = document.getElementById('currentPassword').value;
      const fileInput = document.getElementById('firmwareFile');
      if (!fileInput.files.length) {
        showStatus('Select a firmware file first.', true);
        return;
      }
      if (fileInput.files[0].size > MAX_FIRMWARE_SIZE) {
        showStatus('File too large (max ' + Math.floor(MAX_FIRMWARE_SIZE / 1024 / 1024) + ' MB). Nothing was uploaded.', true);
        return;
      }
      if (!confirm('Uploading new firmware will restart the device once complete. Continue?')) {
        return;
      }

      const uploadButton = document.getElementById('uploadButton');
      uploadButton.disabled = true;
      const progressWrap = document.getElementById('progressWrap');
      const progressFill = document.getElementById('progressFill');
      const progressText = document.getElementById('progressText');
      progressWrap.hidden = false;
      progressFill.style.width = '0%';
      progressText.textContent = '0%';
      showStatus('Uploading...', false);

      const formData = new FormData();
      formData.append('firmware', fileInput.files[0]);

      const xhr = new XMLHttpRequest();
      xhr.open('POST', '/api/firmware?password=' + encodeURIComponent(password));
      xhr.upload.addEventListener('progress', (event) => {
        if (event.lengthComputable) {
          const pct = Math.round((event.loaded / event.total) * 100);
          progressFill.style.width = pct + '%';
          progressText.textContent = pct + '%';
        }
      });
      xhr.onload = () => {
        if (xhr.status === 200) {
          showStatus('Update successful. Device is rebooting...', false);
        } else {
          let message = 'Update failed.';
          try {
            message = JSON.parse(xhr.responseText).message || message;
          } catch (e) { }
          showStatus(message, true);
          uploadButton.disabled = false;
        }
      };
      xhr.onerror = () => {
        showStatus('Update failed (connection error).', true);
        uploadButton.disabled = false;
      };
      xhr.send(formData);
    });
  </script>
</body>

</html>
)rawliteral";
