// HTMLPassword.h

// provides the access point SSID/password and firmware update password change pages

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Config.h>

const char htmlPassword[] = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
  <title>Nixie Calculator SSID &amp; Passwords</title>
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

    h2 {
      font-size: 1rem;
      color: #143642;
      border-bottom: 1px solid #ccc;
      padding-bottom: 4px;
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
      max-width: 400px;
      margin: 0 auto;
      padding: 20px 5px;
    }

    .section {
      margin-bottom: 30px;
    }

    .row {
      display: flex;
      flex-direction: column;
      margin-bottom: 15px;
    }

    .row label {
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

    .row input[type=text] {
      padding: 6px;
      font-size: 1rem;
      width: 100%;
      box-sizing: border-box;
    }

    .saveBar {
      text-align: center;
      padding: 10px 0;
    }

    .saveBar button,
    .saveBar a {
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

    .status {
      display: block;
      margin-top: 10px;
      font-weight: bold;
      color: green;
      text-align: center;
    }
  </style>
</head>

<body>
  <div class="headline">
    <h1>SSID &amp; Passwords</h1>
  </div>
  <div class="content">

    <div class="section">
      <h2>Access Point SSID</h2>
      <p>Requires the current AP password. Changing it disconnects any connected
        WiFi clients, this page included.</p>
      <form id="ssidForm">
        <div class="row">
          <label for="ssidCurrentPassword">Current AP password</label>
          <div class="password-wrap">
            <input type="password" id="ssidCurrentPassword" autocomplete="current-password" required>
            <button type="button" class="toggleVisibility" data-target="ssidCurrentPassword">Show</button>
          </div>
        </div>
        <div class="row">
          <label for="newSsid">New SSID</label>
          <input type="text" id="newSsid" maxlength="32" required>
        </div>
        <div class="saveBar">
          <button type="submit">Change SSID</button>
        </div>
        <span class="status" id="ssidStatus"></span>
      </form>
    </div>

    <div class="section">
      <h2>Access Point Password</h2>
      <form id="passwordForm">
        <div class="row">
          <label for="currentPassword">Current password</label>
          <div class="password-wrap">
            <input type="password" id="currentPassword" name="currentPassword" autocomplete="current-password" required>
            <button type="button" class="toggleVisibility" data-target="currentPassword">Show</button>
          </div>
        </div>
        <div class="row">
          <label for="newPassword">New password</label>
          <div class="password-wrap">
            <input type="password" id="newPassword" name="newPassword" autocomplete="new-password" minlength="8" maxlength="63" required>
            <button type="button" class="toggleVisibility" data-target="newPassword">Show</button>
          </div>
        </div>
        <div class="row">
          <label for="confirmPassword">Confirm new password</label>
          <div class="password-wrap">
            <input type="password" id="confirmPassword" name="confirmPassword" autocomplete="new-password" minlength="8" maxlength="63" required>
            <button type="button" class="toggleVisibility" data-target="confirmPassword">Show</button>
          </div>
        </div>
        <div class="saveBar">
          <button type="submit">Change password</button>
        </div>
        <span class="status" id="passwordStatus"></span>
      </form>
    </div>

)rawliteral"
#if OTA_SUPPORT
R"rawliteral(
    <div class="section">
      <h2>Firmware Update Password</h2>
      <p>Separate from the AP password above. Used to authorize firmware uploads on
        the <a href="/firmware">Firmware Update</a> page, which also requires
        physically arming the device first ([F] + [&plusmn;] on the keyboard).</p>
      <form id="otaPasswordForm">
        <div class="row">
          <label for="currentOtaPassword">Current password</label>
          <div class="password-wrap">
            <input type="password" id="currentOtaPassword" name="currentPassword" autocomplete="current-password" required>
            <button type="button" class="toggleVisibility" data-target="currentOtaPassword">Show</button>
          </div>
        </div>
        <div class="row">
          <label for="newOtaPassword">New password</label>
          <div class="password-wrap">
            <input type="password" id="newOtaPassword" name="newPassword" autocomplete="new-password" minlength="8" maxlength="63" required>
            <button type="button" class="toggleVisibility" data-target="newOtaPassword">Show</button>
          </div>
        </div>
        <div class="row">
          <label for="confirmOtaPassword">Confirm new password</label>
          <div class="password-wrap">
            <input type="password" id="confirmOtaPassword" name="confirmPassword" autocomplete="new-password" minlength="8" maxlength="63" required>
            <button type="button" class="toggleVisibility" data-target="confirmOtaPassword">Show</button>
          </div>
        </div>
        <div class="saveBar">
          <button type="submit">Change password</button>
        </div>
        <span class="status" id="otaPasswordStatus"></span>
      </form>
    </div>
)rawliteral"
#endif
R"rawliteral(
    <div class="saveBar">
      <a href="/" id="backButton">Back</a>
    </div>
  </div>

  <script>
    function showStatus(elementId, text, kind) {
      const status = document.getElementById(elementId);
      status.textContent = text;
      status.style.color = kind === 'error' ? 'crimson' : (kind === 'warning' ? 'darkorange' : 'green');
    }

    document.querySelectorAll('.toggleVisibility').forEach((btn) => {
      btn.addEventListener('click', () => {
        const input = document.getElementById(btn.dataset.target);
        const isHidden = input.type === 'password';
        input.type = isHidden ? 'text' : 'password';
        btn.textContent = isHidden ? 'Hide' : 'Show';
      });
    });

    document.getElementById('ssidForm').addEventListener('submit', (event) => {
      event.preventDefault();
      const currentPassword = document.getElementById('ssidCurrentPassword').value;
      const newSsid = document.getElementById('newSsid').value;

      if (newSsid.length < 1 || newSsid.length > 32) {
        showStatus('ssidStatus', 'New SSID must be between 1 and 32 characters.', 'error');
        return;
      }

      fetch('/api/ssid', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ currentPassword, newSsid })
      })
        .then(async (response) => {
          const data = await response.json().catch(() => ({}));
          if (!response.ok) {
            throw new Error(data.message || 'Request failed');
          }
          document.getElementById('ssidForm').reset();
          showStatus('ssidStatus', 'SSID changed successfully.', 'success');
        })
        .catch((err) => {
          if (err instanceof TypeError) {
            // same cause as the AP password's disconnect warning: a successful change
            // reconfigures the access point immediately, dropping the current connection
            // before the response can be received
            showStatus('ssidStatus', 'SSID changed, but your device was disconnected because the WiFi ' +
              'network name just changed. Reconnect using the new network name.', 'warning');
          } else {
            showStatus('ssidStatus', err.message || 'Failed to change SSID.', 'error');
          }
        });
    });

    // shared submit handler for both password forms; onDisconnectWarning is only
    // relevant for the AP password, where a successful change drops the current
    // WiFi connection before the response can be received
    function setupPasswordForm(formId, statusId, endpoint, warnOnDisconnect) {
      document.getElementById(formId).addEventListener('submit', (event) => {
        event.preventDefault();
        const form = event.target;
        const currentPassword = form.currentPassword.value;
        const newPassword = form.newPassword.value;
        const confirmPassword = form.confirmPassword.value;

        if (newPassword.length < 8 || newPassword.length > 63) {
          showStatus(statusId, 'New password must be between 8 and 63 characters.', 'error');
          return;
        }
        if (newPassword !== confirmPassword) {
          showStatus(statusId, 'New passwords do not match.', 'error');
          return;
        }

        fetch(endpoint, {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ currentPassword, newPassword })
        })
          .then(async (response) => {
            const data = await response.json().catch(() => ({}));
            if (!response.ok) {
              throw new Error(data.message || 'Request failed');
            }
            form.reset();
            showStatus(statusId, 'Password changed successfully.', 'success');
          })
          .catch((err) => {
            if (warnOnDisconnect && (err instanceof TypeError)) {
              // the request itself (not the server) failed to complete; this happens because a
              // successful password change reconfigures the access point immediately, which drops
              // the current WiFi connection before the response can be received
              showStatus(statusId, 'Password changed, but your device was disconnected because the WiFi ' +
                'network password just changed. Reconnect to the WiFi network using the new password.', 'warning');
            } else {
              showStatus(statusId, err.message || 'Failed to change password.', 'error');
            }
          });
      });
    }

    setupPasswordForm('passwordForm', 'passwordStatus', '/api/password', true);
)rawliteral"
#if OTA_SUPPORT
R"rawliteral(
    setupPasswordForm('otaPasswordForm', 'otaPasswordStatus', '/api/otapassword', false);
)rawliteral"
#endif
R"rawliteral(
  </script>
</body>

</html>
)rawliteral";
