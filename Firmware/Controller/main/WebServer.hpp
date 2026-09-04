// WebServer.hpp

// provides calculator information via websocket

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <functional>
#include <time.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SerialPrint.h>
#include <HTMLIndex.h>
#include <HTMLConfig.h>
#include <HTMLStatus.h>
#include <HTMLCalculator.h>
#include <HTMLPassword.h>
#include <HTMLTimeSync.h>
#include <Settings.hpp>
#if OTA_SUPPORT
#include <Update.h>
#include <HTMLFirmware.h>
#endif
#include <WebHelper.hpp>
#include <Config.h>

constexpr auto SERVER_PORT = 80;

// upper bound for the /api/settings request body
constexpr size_t MAX_SETTINGS_BODY_SIZE = 4096;

// accumulates the /api/settings POST body across chunks;
// tracked via AsyncWebServerRequest::_tempObject
struct SettingsRequestBody
{
  String data;
  bool rejected;
};

// upper bound for the /api/password request body
constexpr size_t MAX_PASSWORD_BODY_SIZE = 256;

// after this many consecutive wrong current-password attempts, /api/password is
// locked out for PASSWORD_LOCKOUT_TIME to slow down brute-force guessing
constexpr uint8_t MAX_PASSWORD_ATTEMPTS = 5;
constexpr unsigned long PASSWORD_LOCKOUT_TIME = 60000; // in ms

#if OTA_SUPPORT
// net activity LED toggle interval while a firmware update is armed
constexpr unsigned long NETACT_BLINK_INTERVAL = 300; // in ms

// upper bound for a /api/firmware upload; must match the ota_0/ota_1 partition size
constexpr size_t MAX_FIRMWARE_SIZE = 3 * 1024 * 1024;
#endif

// upper bound for the /api/time request body
constexpr size_t MAX_TIME_BODY_SIZE = 64;

// accumulates the /api/time POST body across chunks;
// tracked via AsyncWebServerRequest::_tempObject
struct TimeRequestBody
{
  String data;
  bool rejected;
};

#if OTA_SUPPORT
// tracks the state of an in-progress /api/firmware upload
struct FirmwareUploadState
{
  bool rejected;
  const char *message;
};
#endif

// accumulates the /api/password POST body across chunks;
// tracked via AsyncWebServerRequest::_tempObject
struct PasswordRequestBody
{
  String data;
  bool rejected;
};

// upper bound for the /api/ssid request body
constexpr size_t MAX_SSID_BODY_SIZE = 128;

// accumulates the /api/ssid POST body across chunks;
// tracked via AsyncWebServerRequest::_tempObject
struct SsidRequestBody
{
  String data;
  bool rejected;
};

class WebServer
{
protected:
  using connectionCallback = std::function<void(uint32_t id)>;
  using disconnectionCallback = std::function<void(uint32_t id)>;
  using settingsUpdateCallback = std::function<void(const String &json, bool isReset)>;
  using statusRequestCallback = std::function<String(void)>;
  using keypadEventCallback = std::function<void(uint8_t keyCode, bool functionKeyPressed, bool shiftKeyPressed)>;
  using registerSubscriptionCallback = std::function<void(bool subscribe)>;
  using timeSyncCallback = std::function<void(time_t utc)>;

public:
  WebServer(uint8_t netActPin, Settings *settings) : _netActPin(netActPin),
                                                     _server(SERVER_PORT),
                                                     _ws(nullptr),
                                                     _settings(settings),
                                                     _notifyConnection(nullptr),
                                                     _notifyDisconnection(nullptr),
                                                     _notifySettingsUpdate(nullptr),
                                                     _notifyStatusRequest(nullptr),
                                                     _notifyKeypadEvent(nullptr),
                                                     _notifyRegisterSubscription(nullptr),
                                                     _notifyTimeSync(nullptr)
  {
    _initialized = false;
    _serverInitialized = false;
  }

  virtual ~WebServer()
  {
    end();
    if (_ws)
    {
      delete (_ws);
    }
  }

  // start the AP and, the first time round, the HTTP/WebSocket server
  void begin()
  {
    if (!_initialized)
    {
      // turn net activity LED on
      digitalWrite(_netActPin, HIGH);

      // start access point
      WiFi.softAP(_settings->getApSsid().c_str(), _settings->getApPassword().c_str());
      _ip = WiFi.softAPIP();

      // configure and start server
      if (!_serverInitialized)
      {
        initWebServer();
        _server.begin();
        _serverInitialized = true;
      }

      _initialized = true;
    }
  }

  // stop the access point but keep the web server running
  void end()
  {
    if (_initialized)
    {
      // stop access point
      WiFi.softAPdisconnect(true);

      // turn net activity LED off
      digitalWrite(_netActPin, LOW);
      _initialized = false;
    }
  }

  // set the callback function for client connection event
  void attachConnectionCb(connectionCallback callBack)
  {
    _notifyConnection = callBack;
  }

  // remove callback
  void detachConnectionCb()
  {
    _notifyConnection = nullptr;
  }

  // set the callback function for client connection event
  void attachDisconnectionCb(connectionCallback callBack)
  {
    _notifyDisconnection = callBack;
  }

  // remove callback
  void detachDisconnectionCb()
  {
    _notifyDisconnection = nullptr;
  }

  // set the callback function for settings update events
  void attachSettingsUpdateCb(settingsUpdateCallback callBack)
  {
    _notifySettingsUpdate = callBack;
  }

  // remove callback
  void detachSettingsUpdateCb()
  {
    _notifySettingsUpdate = nullptr;
  }

  // set the callback function providing the status page JSON
  void attachStatusRequestCb(statusRequestCallback callBack)
  {
    _notifyStatusRequest = callBack;
  }

  // remove callback
  void detachStatusRequestCb()
  {
    _notifyStatusRequest = nullptr;
  }

  // set the callback function for key events coming from the web keypad
  void attachKeypadEventCb(keypadEventCallback callBack)
  {
    _notifyKeypadEvent = callBack;
  }

  // remove callback
  void detachKeypadEventCb()
  {
    _notifyKeypadEvent = nullptr;
  }

  // set the callback function for the web page's register panel show/hide toggle
  void attachRegisterSubscriptionCb(registerSubscriptionCallback callBack)
  {
    _notifyRegisterSubscription = callBack;
  }

  // remove callback
  void detachRegisterSubscriptionCb()
  {
    _notifyRegisterSubscription = nullptr;
  }

  // set the callback function for a browser-triggered time sync
  void attachTimeSyncCb(timeSyncCallback callBack)
  {
    _notifyTimeSync = callBack;
  }

  // remove callback
  void detachTimeSyncCb()
  {
    _notifyTimeSync = nullptr;
  }

  // clean up clients if needed, blink the net activity LED while a firmware update is
  // armed, and reboot if a firmware update just completed
  void process()
  {
    if (_ws)
    {
      _ws->cleanupClients();
    }
#if OTA_SUPPORT
    if (_firmwareRebootPending && (millis() > _firmwareRebootAtMillis))
    {
      ESP.restart();
    }
    if (isFirmwareUpdateArmed())
    {
      if (millis() - _lastNetActToggle > NETACT_BLINK_INTERVAL)
      {
        _netActLedState = !_netActLedState;
        digitalWrite(_netActPin, _netActLedState ? HIGH : LOW);
        _lastNetActToggle = millis();
      }
    }
    else if (_firmwareArmedUntil != 0)
    {
      // the arm window just expired; restore the LED to reflect the AP state
      _firmwareArmedUntil = 0;
      digitalWrite(_netActPin, _initialized ? HIGH : LOW);
    }
#endif
  }

#if OTA_SUPPORT
  // arm firmware updates for durationMs, or disarm immediately if already armed
  void toggleFirmwareUpdateArm(unsigned long durationMs)
  {
    if (isFirmwareUpdateArmed())
    {
      _firmwareArmedUntil = 0;
      // restore the LED immediately instead of waiting for process() to notice
      digitalWrite(_netActPin, _initialized ? HIGH : LOW);
    }
    else
    {
      _firmwareArmedUntil = millis() + durationMs;
    }
  }

  // return whether a firmware update is currently armed
  bool isFirmwareUpdateArmed() const
  {
    return ((_firmwareArmedUntil != 0) && (millis() < _firmwareArmedUntil));
  }
#endif

  // called on client connection
  void onClientConnect(uint32_t id)
  {
    _notifyConnection(id);
  }

  // called on client disconnection
  void onClientDisconnect(uint32_t id)
  {
    _notifyDisconnection(id);
  }

  // called when the settings were updated via the web configuration API;
  // json holds the raw POST body (unused when isReset is true)
  void onSettingsUpdate(const String &json, bool isReset)
  {
    if (_notifySettingsUpdate)
    {
      _notifySettingsUpdate(json, isReset);
    }
  }

  // reset the access point password to the firmware default (Config.h AP_PWD) and,
  // if the access point is currently running, reconfigure it immediately
  void resetApPassword()
  {
    _settings->resetApPassword();
    if (_initialized)
    {
      WiFi.softAP(_settings->getApSsid().c_str(), _settings->getApPassword().c_str());
    }
  }

  // reset the access point SSID to the firmware default (Config.h AP_SSID) and,
  // if the access point is currently running, reconfigure it immediately
  void resetApSsid()
  {
    _settings->resetApSsid();
    if (_initialized)
    {
      WiFi.softAP(_settings->getApSsid().c_str(), _settings->getApPassword().c_str());
    }
  }

  // return IP address of AP
  IPAddress getIP()
  {
    return (_ip);
  }

  // return client count of WebSocket
  size_t getClientCount()
  {
    if (_ws)
    {
      return (_ws->count());
    }
    return (0);
  }

  // return number of stations connected to the access point
  size_t getStationCount()
  {
    return (WiFi.softAPgetStationNum());
  }

  // return if server is initialized
  bool isInitialized() const
  {
    return (_initialized);
  }

  // send message to all clients
  void updateClients(const String &regId, const String &value)
  {
    if (_ws)
    {
      if (_ws->count() > 0)
      {
        _ws->textAll(regId + value);
      }
    }
  }

  // set message to single client
  void updateClient(const String &regId, const String &value, uint32_t id)
  {
    if (_ws)
    {
      if (_ws->hasClient(id))
      {
        _ws->text(id, regId + value);
      }
    }
  }

private:
  uint8_t _netActPin;
  AsyncWebServer _server;
  AsyncWebSocket *_ws;
  Settings *_settings;
  IPAddress _ip;
  connectionCallback _notifyConnection;
  disconnectionCallback _notifyDisconnection;
  settingsUpdateCallback _notifySettingsUpdate;
  statusRequestCallback _notifyStatusRequest;
  keypadEventCallback _notifyKeypadEvent;
  registerSubscriptionCallback _notifyRegisterSubscription;
  timeSyncCallback _notifyTimeSync;
  bool _initialized;
  bool _serverInitialized;
  uint8_t _passwordFailCount = 0;
  unsigned long _passwordLockoutStart = 0;
#if OTA_SUPPORT
  bool _firmwareRebootPending = false;
  unsigned long _firmwareRebootAtMillis = 0;
  unsigned long _firmwareArmedUntil = 0;
  unsigned long _lastNetActToggle = 0;
  bool _netActLedState = false;
  uint8_t _otaPasswordFailCount = 0;
  unsigned long _otaPasswordLockoutStart = 0;
#endif

  // initialize the WebSocket and configure the web server
  void initWebServer()
  {
    // init WebSocket
    _ws = new AsyncWebSocket("/ws");
    _ws->onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
                 { onEvent(server, client, type, arg, data, len); });

    // configure server
    _server.addHandler(_ws);
    _server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(200, "text/html", htmlIndex); });
    _server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(200, "text/html", htmlConfig); });
    _server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(200, "text/html", htmlStatus); });
    _server.on("/calculator", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(200, "text/html", htmlCalculator); });
    _server.on("/password", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(200, "text/html", htmlPassword); });
    _server.on("/timesync", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(200, "text/html", htmlTimeSync); });
#if OTA_SUPPORT
    _server.on("/firmware", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(200, "text/html", htmlFirmware); });
#endif

    // status information API
    _server.on("/api/status", HTTP_GET, [this](AsyncWebServerRequest *request)
               {
                 if (_notifyStatusRequest)
                 {
                   request->send(200, "application/json", _notifyStatusRequest());
                 }
                 else
                 {
                   request->send(503, "application/json", "{}");
                 } });

    // settings configuration API
    _server.on("/api/settings", HTTP_GET, [this](AsyncWebServerRequest *request)
               { request->send(200, "application/json", WebHelper::settingsToJSON(*_settings)); });
    _server.on(
        "/api/settings", HTTP_POST,
        [this](AsyncWebServerRequest *request)
        {
          SettingsRequestBody *body = reinterpret_cast<SettingsRequestBody *>(request->_tempObject);
          bool rejected = false;
          if (body)
          {
            rejected = body->rejected;
            if (!rejected)
            {
              onSettingsUpdate(body->data, false);
            }
            delete body;
            request->_tempObject = nullptr;
          }
          if (rejected)
          {
            request->send(413, "application/json", "{\"status\":\"error\"}");
          }
          else
          {
            request->send(200, "application/json", "{\"status\":\"ok\"}");
          }
        },
        nullptr,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
        {
          if (index == 0)
          {
            SettingsRequestBody *body = new SettingsRequestBody{String(), total > MAX_SETTINGS_BODY_SIZE};
            request->_tempObject = body;
            // free the accumulated body if the client disconnects mid-upload
            request->onDisconnect([request]()
                                  {
                                     if (request->_tempObject)
                                     {
                                       delete reinterpret_cast<SettingsRequestBody *>(request->_tempObject);
                                       request->_tempObject = nullptr;
                                     } });
          }
          SettingsRequestBody *body = reinterpret_cast<SettingsRequestBody *>(request->_tempObject);
          if (!body->rejected)
          {
            if ((body->data.length() + len) > MAX_SETTINGS_BODY_SIZE)
            {
              // actual data exceeds the cap regardless of the declared content length
              body->rejected = true;
              body->data.clear();
            }
            else
            {
              body->data.concat(reinterpret_cast<const char *>(data), len);
            }
          }
        });
    _server.on("/api/settings/reset", HTTP_POST, [this](AsyncWebServerRequest *request)
               {
                 onSettingsUpdate(String(), true);
                 request->send(200, "application/json", "{\"status\":\"ok\"}"); });

    // access point password change API
    _server.on(
        "/api/password", HTTP_POST,
        [this](AsyncWebServerRequest *request)
        {
          if ((_passwordFailCount >= MAX_PASSWORD_ATTEMPTS) && ((millis() - _passwordLockoutStart) < PASSWORD_LOCKOUT_TIME))
          {
            request->send(429, "application/json", "{\"status\":\"error\",\"message\":\"Too many attempts, try again later\"}");
            return;
          }
          PasswordRequestBody *body = reinterpret_cast<PasswordRequestBody *>(request->_tempObject);
          bool rejected = false;
          String json;
          if (body)
          {
            rejected = body->rejected;
            json = body->data;
            delete body;
            request->_tempObject = nullptr;
          }
          if (rejected)
          {
            request->send(413, "application/json", "{\"status\":\"error\",\"message\":\"Request too large\"}");
            return;
          }
          String currentPassword = WebHelper::jsonGetString(json, "currentPassword");
          String newPassword = WebHelper::jsonGetString(json, "newPassword");
          if (currentPassword != _settings->getApPassword())
          {
            _passwordFailCount++;
            if (_passwordFailCount >= MAX_PASSWORD_ATTEMPTS)
            {
              _passwordLockoutStart = millis();
            }
            request->send(403, "application/json", "{\"status\":\"error\",\"message\":\"Current password is incorrect\"}");
            return;
          }
          _passwordFailCount = 0;
          if (!_settings->setApPassword(newPassword))
          {
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"New password must be 8-63 characters\"}");
            return;
          }
          if (_initialized)
          {
            // reconfigure the running access point with the new password
            WiFi.softAP(_settings->getApSsid().c_str(), newPassword.c_str());
          }
          request->send(200, "application/json", "{\"status\":\"ok\"}");
        },
        nullptr,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
        {
          if (index == 0)
          {
            PasswordRequestBody *body = new PasswordRequestBody{String(), total > MAX_PASSWORD_BODY_SIZE};
            request->_tempObject = body;
            // free the accumulated body if the client disconnects mid-upload
            request->onDisconnect([request]()
                                  {
                                     if (request->_tempObject)
                                     {
                                       delete reinterpret_cast<PasswordRequestBody *>(request->_tempObject);
                                       request->_tempObject = nullptr;
                                     } });
          }
          PasswordRequestBody *body = reinterpret_cast<PasswordRequestBody *>(request->_tempObject);
          if (!body->rejected)
          {
            if ((body->data.length() + len) > MAX_PASSWORD_BODY_SIZE)
            {
              // actual data exceeds the cap regardless of the declared content length
              body->rejected = true;
              body->data.clear();
            }
            else
            {
              body->data.concat(reinterpret_cast<const char *>(data), len);
            }
          }
        });

    // access point SSID change API; authorized with the current AP password, shares
    // its brute-force lockout counters since it's the same secret being guessed
    _server.on(
        "/api/ssid", HTTP_POST,
        [this](AsyncWebServerRequest *request)
        {
          if ((_passwordFailCount >= MAX_PASSWORD_ATTEMPTS) && ((millis() - _passwordLockoutStart) < PASSWORD_LOCKOUT_TIME))
          {
            request->send(429, "application/json", "{\"status\":\"error\",\"message\":\"Too many attempts, try again later\"}");
            return;
          }
          SsidRequestBody *body = reinterpret_cast<SsidRequestBody *>(request->_tempObject);
          bool rejected = false;
          String json;
          if (body)
          {
            rejected = body->rejected;
            json = body->data;
            delete body;
            request->_tempObject = nullptr;
          }
          if (rejected)
          {
            request->send(413, "application/json", "{\"status\":\"error\",\"message\":\"Request too large\"}");
            return;
          }
          String currentPassword = WebHelper::jsonGetString(json, "currentPassword");
          String newSsid = WebHelper::jsonGetString(json, "newSsid");
          if (currentPassword != _settings->getApPassword())
          {
            _passwordFailCount++;
            if (_passwordFailCount >= MAX_PASSWORD_ATTEMPTS)
            {
              _passwordLockoutStart = millis();
            }
            request->send(403, "application/json", "{\"status\":\"error\",\"message\":\"Current password is incorrect\"}");
            return;
          }
          _passwordFailCount = 0;
          if (!_settings->setApSsid(newSsid))
          {
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"New SSID must be 1-32 characters\"}");
            return;
          }
          if (_initialized)
          {
            // reconfigure the running access point with the new SSID
            WiFi.softAP(newSsid.c_str(), _settings->getApPassword().c_str());
          }
          request->send(200, "application/json", "{\"status\":\"ok\"}");
        },
        nullptr,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
        {
          if (index == 0)
          {
            SsidRequestBody *body = new SsidRequestBody{String(), total > MAX_SSID_BODY_SIZE};
            request->_tempObject = body;
            // free the accumulated body if the client disconnects mid-upload
            request->onDisconnect([request]()
                                  {
                                     if (request->_tempObject)
                                     {
                                       delete reinterpret_cast<SsidRequestBody *>(request->_tempObject);
                                       request->_tempObject = nullptr;
                                     } });
          }
          SsidRequestBody *body = reinterpret_cast<SsidRequestBody *>(request->_tempObject);
          if (!body->rejected)
          {
            if ((body->data.length() + len) > MAX_SSID_BODY_SIZE)
            {
              // actual data exceeds the cap regardless of the declared content length
              body->rejected = true;
              body->data.clear();
            }
            else
            {
              body->data.concat(reinterpret_cast<const char *>(data), len);
            }
          }
        });

    // browser-triggered time sync API
    _server.on(
        "/api/time", HTTP_POST,
        [this](AsyncWebServerRequest *request)
        {
          TimeRequestBody *body = reinterpret_cast<TimeRequestBody *>(request->_tempObject);
          bool rejected = false;
          String json;
          if (body)
          {
            rejected = body->rejected;
            json = body->data;
            delete body;
            request->_tempObject = nullptr;
          }
          if (rejected)
          {
            request->send(413, "application/json", "{\"status\":\"error\",\"message\":\"Request too large\"}");
            return;
          }
          time_t epoch = WebHelper::jsonGetLong(json, "epoch");
          if (epoch <= 0)
          {
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid epoch\"}");
            return;
          }
          if (_notifyTimeSync)
          {
            _notifyTimeSync(epoch);
          }
          request->send(200, "application/json", "{\"status\":\"ok\"}");
        },
        nullptr,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
        {
          if (index == 0)
          {
            TimeRequestBody *body = new TimeRequestBody{String(), total > MAX_TIME_BODY_SIZE};
            request->_tempObject = body;
            // free the accumulated body if the client disconnects mid-upload
            request->onDisconnect([request]()
                                  {
                                     if (request->_tempObject)
                                     {
                                       delete reinterpret_cast<TimeRequestBody *>(request->_tempObject);
                                       request->_tempObject = nullptr;
                                     } });
          }
          TimeRequestBody *body = reinterpret_cast<TimeRequestBody *>(request->_tempObject);
          if (!body->rejected)
          {
            if ((body->data.length() + len) > MAX_TIME_BODY_SIZE)
            {
              // actual data exceeds the cap regardless of the declared content length
              body->rejected = true;
              body->data.clear();
            }
            else
            {
              body->data.concat(reinterpret_cast<const char *>(data), len);
            }
          }
        });

#if OTA_SUPPORT
    // firmware update API
    _server.on(
        "/api/firmware", HTTP_POST,
        [this](AsyncWebServerRequest *request)
        {
          FirmwareUploadState *state = reinterpret_cast<FirmwareUploadState *>(request->_tempObject);
          bool ok = state && !state->rejected && !Update.hasError();
          const char *message = (state && state->message) ? state->message : "Update failed";
          if (state)
          {
            delete state;
            request->_tempObject = nullptr;
          }
          if (!ok)
          {
            request->send(400, "application/json", String("{\"status\":\"error\",\"message\":\"") + message + "\"}");
            return;
          }
          request->send(200, "application/json", "{\"status\":\"ok\"}");
          // reboot shortly after, so the response has time to actually reach the client first
          _firmwareRebootPending = true;
          _firmwareRebootAtMillis = millis() + 1000;
        },
        [this](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
        {
          FirmwareUploadState *state;
          if (index == 0)
          {
            state = new FirmwareUploadState{false, nullptr};
            request->_tempObject = state;
            // free upload state if the client disconnects mid-upload
            request->onDisconnect([request]()
                                  {
                                     if (request->_tempObject)
                                     {
                                       delete reinterpret_cast<FirmwareUploadState *>(request->_tempObject);
                                       request->_tempObject = nullptr;
                                     } });
            if (request->contentLength() > MAX_FIRMWARE_SIZE)
            {
              state->rejected = true;
              state->message = "File too large";
              return;
            }
            if (!isFirmwareUpdateArmed())
            {
              state->rejected = true;
              state->message = "Not armed: press [F] + [\xc2\xb1] on the device first"; // ±
              return;
            }
            if ((_otaPasswordFailCount >= MAX_PASSWORD_ATTEMPTS) && ((millis() - _otaPasswordLockoutStart) < PASSWORD_LOCKOUT_TIME))
            {
              state->rejected = true;
              state->message = "Too many attempts, try again later";
              return;
            }
            const AsyncWebParameter *pwdParam = request->getParam("password");
            if (!pwdParam || (pwdParam->value() != _settings->getOtaPassword()))
            {
              _otaPasswordFailCount++;
              if (_otaPasswordFailCount >= MAX_PASSWORD_ATTEMPTS)
              {
                _otaPasswordLockoutStart = millis();
              }
              state->rejected = true;
              state->message = "Incorrect firmware update password";
              return;
            }
            _otaPasswordFailCount = 0;
            if (!Update.begin(UPDATE_SIZE_UNKNOWN))
            {
              state->rejected = true;
              state->message = "Failed to start update";
              return;
            }
          }
          else
          {
            state = reinterpret_cast<FirmwareUploadState *>(request->_tempObject);
          }
          if (!state || state->rejected)
          {
            return;
          }
          if (Update.write(data, len) != len)
          {
            state->rejected = true;
            state->message = "Write failed";
            Update.abort();
            return;
          }
          if (final && !Update.end(true))
          {
            state->rejected = true;
            state->message = "Failed to finalize update";
          }
        });

    // firmware update password change API;
    _server.on(
        "/api/otapassword", HTTP_POST,
        [this](AsyncWebServerRequest *request)
        {
          PasswordRequestBody *body = reinterpret_cast<PasswordRequestBody *>(request->_tempObject);
          bool rejected = false;
          String json;
          if (body)
          {
            rejected = body->rejected;
            json = body->data;
            delete body;
            request->_tempObject = nullptr;
          }
          if (rejected)
          {
            request->send(413, "application/json", "{\"status\":\"error\",\"message\":\"Request too large\"}");
            return;
          }
          if ((_otaPasswordFailCount >= MAX_PASSWORD_ATTEMPTS) && ((millis() - _otaPasswordLockoutStart) < PASSWORD_LOCKOUT_TIME))
          {
            request->send(429, "application/json", "{\"status\":\"error\",\"message\":\"Too many attempts, try again later\"}");
            return;
          }
          String currentPassword = WebHelper::jsonGetString(json, "currentPassword");
          String newPassword = WebHelper::jsonGetString(json, "newPassword");
          if (currentPassword != _settings->getOtaPassword())
          {
            _otaPasswordFailCount++;
            if (_otaPasswordFailCount >= MAX_PASSWORD_ATTEMPTS)
            {
              _otaPasswordLockoutStart = millis();
            }
            request->send(403, "application/json", "{\"status\":\"error\",\"message\":\"Current password is incorrect\"}");
            return;
          }
          _otaPasswordFailCount = 0;
          if (!_settings->setOtaPassword(newPassword))
          {
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"New password must be 8-63 characters\"}");
            return;
          }
          request->send(200, "application/json", "{\"status\":\"ok\"}");
        },
        nullptr,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
        {
          if (index == 0)
          {
            PasswordRequestBody *body = new PasswordRequestBody{String(), total > MAX_PASSWORD_BODY_SIZE};
            request->_tempObject = body;
            // free the accumulated body if the client disconnects mid-upload
            request->onDisconnect([request]()
                                  {
                                     if (request->_tempObject)
                                     {
                                       delete reinterpret_cast<PasswordRequestBody *>(request->_tempObject);
                                       request->_tempObject = nullptr;
                                     } });
          }
          PasswordRequestBody *body = reinterpret_cast<PasswordRequestBody *>(request->_tempObject);
          if (!body->rejected)
          {
            if ((body->data.length() + len) > MAX_PASSWORD_BODY_SIZE)
            {
              // actual data exceeds the cap regardless of the declared content length
              body->rejected = true;
              body->data.clear();
            }
            else
            {
              body->data.concat(reinterpret_cast<const char *>(data), len);
            }
          }
        });
#endif
  }

  // WebSocket event handler
  void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
               void *arg, uint8_t *data, size_t len)
  {
    switch (type)
    {
    case WS_EVT_CONNECT:
      __serial_printf("WebSocket client #%u connected\n", static_cast<unsigned int>(client->id())); //, s.c_str());
      onClientConnect(client->id());
      break;

    case WS_EVT_DISCONNECT:
      __serial_printf("WebSocket client #%u disconnected\n", static_cast<unsigned int>(client->id()));
      onClientDisconnect(client->id());
      break;

    case WS_EVT_DATA:
    {
      AwsFrameInfo *info = reinterpret_cast<AwsFrameInfo *>(arg);
      if (info->final && (info->index == 0) && (info->len == len) && (info->opcode == WS_TEXT))
      {
        handleIncomingMessage(data, len);
      }
      break;
    }

    case WS_EVT_PING:
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
    }
  }

  // parse a message received from a client; handles web keypad key events, sent as
  // "P<keyCode>,<functionKeyPressed 0|1>,<shiftKeyPressed 0|1>", and the register
  // panel show/hide toggle, sent as "R1" (subscribe) or "R0" (unsubscribe)
  void handleIncomingMessage(uint8_t *data, size_t len)
  {
    if (len < 2)
    {
      return;
    }
    if (data[0] == 'P')
    {
      if (!_notifyKeypadEvent)
      {
        return;
      }
      String payload;
      payload.concat(reinterpret_cast<const char *>(data + 1), len - 1);
      int comma1 = payload.indexOf(',');
      int comma2 = payload.indexOf(',', comma1 + 1);
      if ((comma1 < 0) || (comma2 < 0))
      {
        return;
      }
      uint8_t keyCode = static_cast<uint8_t>(payload.substring(0, comma1).toInt());
      bool functionKeyPressed = payload.substring(comma1 + 1, comma2).toInt() != 0;
      bool shiftKeyPressed = payload.substring(comma2 + 1).toInt() != 0;
      _notifyKeypadEvent(keyCode, functionKeyPressed, shiftKeyPressed);
    }
    else if ((data[0] == 'R') && _notifyRegisterSubscription)
    {
      _notifyRegisterSubscription(data[1] == '1');
    }
  }
};