// Controller.hpp

// main logic

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <atomic>
#include <freertos/semphr.h>
#include <freertos/queue.h>
#include <time.h>
#include <Config.h>
#include <FirmwareInfo.h>
#include <DeviceModes.h>
#include <Wire.h>
#include <SerialPrint.h>
#include <Errors.hpp>
#include <KeyboardHandler.hpp>
#include <Helper.hpp>
#include <Settings.hpp>
#include <SettingsCache.hpp>
#include <DisplayHandler.hpp>
#include <Calculator.hpp>
#include <Clock.hpp>
#include <Lighting.hpp>
#include <PIR.hpp>
#include <GPS.hpp>
#include <Temperature.hpp>
#include <MenuHandler.hpp>
#if WEB_SUPPORT
#include <WebServer.hpp>
#include <WebHelper.hpp>
#endif

// pin definitions
constexpr uint8_t PIN_HVENABLE = 4;
constexpr uint8_t PIN_HVLED = 5;
constexpr uint8_t PIN_SHIFT = 17;
constexpr uint8_t PIN_STORE = 16;
constexpr uint8_t PIN_DATA = 18;
constexpr uint8_t PIN_BLANK = 19;
constexpr uint8_t PIN_LEDCTL = 14;
constexpr uint8_t PIN_GPSTX = 33;
constexpr uint8_t PIN_GPSRX = 32;
constexpr uint8_t PIN_PIR = 35;
constexpr uint8_t PIN_KINT = 27;
constexpr uint8_t PIN_TEMPERATURE = 25;
constexpr uint8_t PIN_BUTTON1 = 34;
constexpr uint8_t PIN_NETACT = 12;
constexpr uint8_t PIN_RTC = 36;

// controller temperature monitoring
constexpr float SHUTDOWN_TEMP = 50.0;
constexpr float RECOVER_TEMP = 45.0;
constexpr unsigned long TEMP_CHECK_INTERVAL = 10000; // in ms

// minimum allowed time between two high-voltage switch-on events
constexpr unsigned long MIN_HVON_INTERVAL = 1000; // in ms

// how long BUTTON1 must be held down at power-up to trigger a factory reset
constexpr unsigned long FACTORY_RESET_HOLD_TIME = 2500; // in ms

#if OTA_SUPPORT
// how long a firmware update stays armed after [F] + [±] is pressed
constexpr unsigned long OTA_ARM_WINDOW = 5UL * 60UL * 1000UL; // in ms
#endif

// minimum time between web display/LED snapshot broadcasts
constexpr unsigned long DISPLAY_BROADCAST_INTERVAL = 100; // in ms

// struct needed for digit rotation for cathode poisoning prevention
typedef struct
{
  uint8_t rotationStep;
  int rotationInterval;
  unsigned long lastRotationTimestamp;
} ROTATIONDATA;

#if WEB_SUPPORT
// a key event received from the web keypad, queued for processing on the main task
typedef struct
{
  uint8_t keyCode;
  bool functionKeyPressed;
  bool shiftKeyPressed;
} KEYPADEVENT;

// reserved keyCode values sent by the web keypad for commands
constexpr uint8_t KEYPAD_CMD_MODE_SWITCH = 254; // the "Switch Mode" button
constexpr uint8_t KEYPAD_CMD_MENU_MODE = 253;   // the "Menu" button

// a register panel subscription change, queued for processing on the main task
enum class register_sub_event : uint8_t
{
  subscribe,
  unsubscribe,
  reset
};
#endif

class Controller
{
public:
  Controller()
      : _displayHandler(DISPLAY_TYPE, PIN_DATA, PIN_STORE, PIN_SHIFT, PIN_BLANK, PIN_LEDCTL),
        _clock(&_displayHandler, &_settings),
        _lighting(&_displayHandler),
        _pir(PIN_PIR),
        _temperature(PIN_TEMPERATURE),
#if WEB_SUPPORT
        _web(PIN_NETACT, &_settings),
#endif
        _menuHandler(&_settings, _displayHandler.getDecimalSeparatorPosition())

  {
    _highVoltageOn = true;
    _hvOnTimestamp = 0;
    _hvOnAccumulatedSeconds = 0;
    _autoOff = false;
    _deviceMode = device_mode::calculator;
    _prevDeviceMode = device_mode::calculator;
    _rotationData = (ROTATIONDATA *)calloc(_displayHandler.getDigitCount() + MAX_SPECIAL_CHARS_DIGITS, sizeof(ROTATIONDATA));
    _rotationStopped = false;
    _scrollResult = false;
    _temperatureShutdown = false;
    _lastTempCheckTimestamp = 0;
    _dimmingActive = false;
    _settingsUpdateMutex = xSemaphoreCreateMutex();
    _pendingSettingsIsReset = false;
#if WEB_SUPPORT
    _keypadEventQueue = xQueueCreate(8, sizeof(KEYPADEVENT));
    _registerSubscriptionQueue = xQueueCreate(8, sizeof(register_sub_event));
    _registerSubscriberCount = 0;
    _deviceModeBroadcastInitialized = false;
    _displayDirty = false;
    _lastDisplaySnapshot = 0;
    _timeSyncQueue = xQueueCreate(2, sizeof(time_t));
#endif
  }

  virtual ~Controller()
  {
    free(_rotationData);
    vSemaphoreDelete(_settingsUpdateMutex);
#if WEB_SUPPORT
    vQueueDelete(_keypadEventQueue);
    vQueueDelete(_registerSubscriptionQueue);
    vQueueDelete(_timeSyncQueue);
#endif
  }

  // initialize controller
  int begin()
  {
    int result = ERR_SUCCESS;

    // set high voltage off
    pinMode(PIN_HVENABLE, OUTPUT);
    pinMode(PIN_HVLED, OUTPUT);
    hvOFF();

    // define additional pin modes
    pinMode(PIN_DATA, OUTPUT);
    pinMode(PIN_STORE, OUTPUT);
    pinMode(PIN_SHIFT, OUTPUT);
    pinMode(PIN_NETACT, OUTPUT);
    pinMode(PIN_BUTTON1, INPUT);

    // turn off the network activity LED
    digitalWrite(PIN_NETACT, LOW);

    // BUTTON1 held down (active low) for FACTORY_RESET_HOLD_TIME at power-up triggers a factory reset
    bool factoryReset = isFactoryResetButtonHeld();

    // initialize non-volatile storage
    if (_settings.begin())
    {
      if (factoryReset)
      {
        // erase all stored settings, including the AP password
        _settings.factoryReset();
      }

      // load settings into settings cache
      _settings.readSettings();

      // hide some setting according to display type
      adjustVisibleSettings();

      // initialize calculator
      _calculator.begin(_displayHandler.getDigitCount(), _displayHandler.getDecimalSeparatorCount(), _displayHandler.hasPlusSign());

      // get notified on long operations
      _calculator.attachLongOperationCb([this](long_operation lo)
                                        { onLongOperation(lo); });

      // initialize display
      _displayHandler.begin();
      _displayHandler.clearDisplay();

      // initialize LED lighting
      _lighting.begin();
      _lighting.off();

      // initialize menu handler
      _menuHandler.begin(_displayHandler.getDigitCount());

      // initialize GPS
      _gps.begin(PIN_GPSRX, PIN_GPSTX);

      // get notified on gps time sync
      _gps.attach([this](time_t utc)
                  { onGPSTimeSyncEvent(utc); });

#if WEB_SUPPORT
      // get notified on websocket connections
      _web.attachConnectionCb([this](uint32_t id)
                              { onClientConnection(id); });

      // get notified on websocket disconnections
      _web.attachDisconnectionCb([this](uint32_t id)
                                 { onClientDisconnection(id); });

      // get notified when settings are changed via the web configuration page
      _web.attachSettingsUpdateCb([this](const String &json, bool isReset)
                                  { onSettingsUpdate(json, isReset); });

      // provide status information to the status page
      _web.attachStatusRequestCb([this]()
                                 { return (getStatusJSON()); });

      // get notified on key events coming from the web keypad
      _web.attachKeypadEventCb([this](uint8_t keyCode, bool functionKeyPressed, bool shiftKeyPressed)
                               { onKeypadEvent(keyCode, functionKeyPressed, shiftKeyPressed); });

      // get notified when the web page's register panel is shown or hidden
      _web.attachRegisterSubscriptionCb([this](bool subscribe)
                                        { onRegisterSubscriptionEvent(subscribe); });

      // get notified when a browser triggers a manual time sync
      _web.attachTimeSyncCb([this](time_t utc)
                            { onWebTimeSyncEvent(utc); });

      // mark the display dirty on every hardware refresh
      _displayHandler.attachCommitCb([this]()
                                     { _displayDirty = true; });

      // needed for display the busy calc animations on the web page
      _displayHandler.attachBusyTickCb([this]()
                                       { broadcastDisplaySnapshot(); });
#endif

      // init I²C
      Wire.begin();

      // initialize clock
      _clock.begin();

      // initialize keyboard
      Serial1.begin(KEYBOARD_COMM_SPEED, SERIAL_8N1, PIN_KINT);
      _keyboard.begin(Serial1);

      // get notified on keyboard events
      _keyboard.attach([this](uint8_t keyCode, key_state keyState, bool functionKeyPressed, bool shiftKeyPressed, special_keyboard_event specialEvent)
                       { onKeyboardEvent(keyCode, keyState, functionKeyPressed, shiftKeyPressed, specialEvent); });

      // give keyboard MC time to start
      delay(500);

      // request version from keyboard
      _keyboard.requestVersion();

      // configure keyboard
      _keyboard.setAutoRepeatInterval(0);
      _keyboard.setHoldTime(2000);

      // turn on high voltage
      _hvOffTimestamp = MIN_HVON_INTERVAL;
      hvON();

      if (factoryReset)
      {
        // flash a confirmation pattern
        showFactoryResetConfirmation();
      }

      // show version
      if (SettingsCache::showVersion == show_version::on)
      {
        showVersion();
        delay(1000);
      }

      // set startup device mode
      switch (SettingsCache::startupMode)
      {
      case startup_mode::calculator:
        _deviceMode = device_mode::calculator;
        _prevDeviceMode = device_mode::calculator;
        break;

      case startup_mode::clock:
        _deviceMode = device_mode::clock;
        _prevDeviceMode = device_mode::clock;
        break;
      }

      // display initial values
      switch (_deviceMode)
      {
      case device_mode::calculator:
        refreshCalcDisplay();
        break;

      case device_mode::clock:
        _displayHandler.clear();
        break;

      default: // avoid warning
        break;
      }

#if WEB_SUPPORT
      // auto-start the WiFi access point if configured
      if (SettingsCache::apAutoStart == ap_auto_start::on)
      {
        _web.begin();
        displayIP(_web.getIP(), 1);
      }
#endif
    }
    else
    {
      // failed to initialize NVS
      result = ERR_INITSETTINGS;
    }
    return (result);
  }

  // main logic, called from the main loop
  void process()
  {
    struct tm tm, tm2;

    // periodically re-sync the system clock from the RTC
    _clock.syncTimeFromRTC();

    // we need the current system time for both timezones
    _clock.getCurrentTime(&tm, &tm2);

    // process keyboard input
    _keyboard.process();

    // check if it's time to switch to clock mode or turn the HV off
    checkAutoOff();

    // check if it's time to rotate digits for antipoisoning
    checkAntiPoisoning(&tm);

    // check if it's time to dim the display
    checkDimming(&tm);

    // check board temperature and shutdown HV if needed
    checkBoardTemperature();

    // process gps data
    _gps.process();

    // process PIR status
    _pir.process();

    // turn on or off the high voltage
    // HV can be off because of the autoff mode or because the PIR timed out
    if (checkHVStatus())
    {
      hvON();
    }
    else
    {
      hvOFF();
      _lighting.off();
    }

    // process lighting
    if (isHVON())
    {
      _lighting.process(&tm, _deviceMode);
    }

    // process temperature
    _temperature.process();

    // we need the temperature for some clock modes
    _clock.setTemperature(_temperature.getTemperature());

    // always check if timer is running
    _clock.processTimer();

#if WEB_SUPPORT
    if (_web.isInitialized())
    {
      _web.process();

      // process configuration changes made via the website
      processWebUpdates();

      // process key events sent by the web keypad
      processWebKeypadEvents();

      // process register panel subscription changes
      processRegisterSubscriptionEvents();

      // process any browser-triggered time sync requests
      processWebTimeSyncEvents();

      // let web clients know if the device mode changed
      broadcastDeviceModeIfChanged();

      // push a fresh display/LED snapshot to web clients, throttled
      broadcastDisplaySnapshotIfDue();
    }
#endif

    // process according to current device mode
    switch (_deviceMode)
    {
    case device_mode::clock:
      _clock.resetRefreshLighting();
      _clock.process(&tm, &tm2);
      if (_clock.getRefreshLighting())
      {
        _lighting.refresh();
      }
      // during process, the clock writes directly into display buffer
      // we just have to commit
      _displayHandler.show();
      break;

    case device_mode::calculator:
      // check if we have to scroll the result
      if (_scrollResult)
      {
        String scrollString;
        bool baseNegative;
        int decimalPos;
        bool exponentNegative;
        String exponent;
        if (_calculator.getScrollInfo(&baseNegative, scrollString, &decimalPos, &exponentNegative, exponent))
        {

          if (!scrollString.isEmpty())
          {
            _displayHandler.showScrollingResult(baseNegative, scrollString, decimalPos, exponentNegative, exponent);
            _lighting.refresh();
          }
        }
        else
        {
          // stop scrolling
          _scrollResult = false;
          _calculator.resetScrollInfo();
          refreshCalcDisplay();
          _lighting.refresh();
        }
      }
      break;

    case device_mode::menu:
      // menu is keyboard driven but we have to update the display for blinking input
      if (_menuHandler.updateDisplayNeeded())
      {
        if (_displayHandler.hasMenuSign())
        {
          _displayHandler.show(_menuHandler.getDisplay(), digit_content::none, true);
        }
        else
        {
          _displayHandler.show(_menuHandler.getDisplay());
        }
      }
      break;

    case device_mode::antipoisoning:
      // rotate the digits for cathode poisoning prevention
      rotate(&tm);
      break;
    }
  }

  // return the clock object
  Clock *getClock()
  {
    return (&_clock);
  }

  // return whether web support is compiled into this firmware
  bool isWebSupportEnabled() const
  {
    return (WEB_SUPPORT);
  }

private:
  DisplayHandler _displayHandler;
  Clock _clock;
  Lighting _lighting;
  PIR _pir;
  Temperature _temperature;
#if WEB_SUPPORT
  WebServer _web;
#endif
  MenuHandler _menuHandler;
  GPS _gps;
  KeyboardHandler _keyboard;
  Settings _settings;
  Calculator _calculator;
  bool _scrollResult;

  device_mode _deviceMode;
  device_mode _prevDeviceMode;
  bool _highVoltageOn;
  bool _autoOff;
  ROTATIONDATA *_rotationData;
  bool _rotationStopped;
  unsigned long _hvOffTimestamp;
  unsigned long _hvOnTimestamp;
  unsigned long _hvOnAccumulatedSeconds;
  bool _temperatureShutdown;
  unsigned long _lastTempCheckTimestamp;
  bool _dimmingActive;
  std::atomic<bool> _settingsUpdatePending{false};
  SemaphoreHandle_t _settingsUpdateMutex;
  String _pendingSettingsJSON;
  bool _pendingSettingsIsReset;
#if WEB_SUPPORT
  QueueHandle_t _keypadEventQueue;
  device_mode _lastBroadcastDeviceMode;
  bool _deviceModeBroadcastInitialized;
  QueueHandle_t _registerSubscriptionQueue;
  int _registerSubscriberCount;
  bool _displayDirty;
  unsigned long _lastDisplaySnapshot;
  QueueHandle_t _timeSyncQueue;
#endif

  // the display brightness that should currently be in effect
  int currentBrightness() const
  {
    return (_dimmingActive ? SettingsCache::dimBrightness : SettingsCache::brightness);
  }

  // turn the high voltage on
  void hvON()
  {
    if (!_highVoltageOn)
    {
      // avoid too fast off and on switching
      if (millis() - _hvOffTimestamp > MIN_HVON_INTERVAL)
      {
        _highVoltageOn = true;
        _hvOnTimestamp = millis();
        digitalWrite(PIN_HVLED, HIGH);
        _displayHandler.setDisplayBrightness(currentBrightness());
        if (_displayHandler.getDisplayType() != display_type::led)
        {
          digitalWrite(PIN_HVENABLE, HIGH);
        }
        else
        {
          _displayHandler.getLEDDriver()->on();
        }

        __serial_println("Turn HV on");
      }
    }
  }

  // turn the high voltage off
  void hvOFF()
  {
    if (_highVoltageOn)
    {
      _highVoltageOn = false;
      _hvOnAccumulatedSeconds += (millis() - _hvOnTimestamp) / 1000;
      digitalWrite(PIN_HVLED, LOW);
      if (_displayHandler.getDisplayType() != display_type::led)
      {
        digitalWrite(PIN_HVENABLE, LOW);
      }
      else
      {
        _displayHandler.getLEDDriver()->off();
      }
      _hvOffTimestamp = millis();
      __serial_println("Turn HV off");
    }
  }

  // return the status of the high voltage
  bool isHVON() const
  {
    return (_highVoltageOn);
  }

  // return the total accumulated time the high voltage has been on, in seconds
  unsigned long getHVOnSeconds() const
  {
    unsigned long total = _hvOnAccumulatedSeconds;
    if (_highVoltageOn)
    {
      total += (millis() - _hvOnTimestamp) / 1000;
    }
    return (total);
  }

  // set the device mode:
  // pressing the function key switches between calculator and clock mode
  void switchDeviceMode()
  {
    // reconfigure keyboard
    _keyboard.setAutoRepeatInterval(0);
    _keyboard.setFastAutoRepeatDelay(0);
    _keyboard.setFastAutoRepeatInterval(0);
    _keyboard.setHoldTime(2000);

    _displayHandler.clearDisplay();
    _displayHandler.clearLEDs();
    switch (_deviceMode)
    {
    case device_mode::calculator:
      _deviceMode = device_mode::clock;
      _lighting.refresh();
      break;

    case device_mode::clock:
      _deviceMode = device_mode::calculator;
      refreshCalcDisplay();
      _lighting.refresh();
      break;

      // leaving menu mode
    case device_mode::menu:
      _settings.storeSettings();
      _deviceMode = _prevDeviceMode;
      applyUpdatedSettings();
      break;

    case device_mode::antipoisoning:
      _rotationStopped = true;
      break;
    }
  }

  // switch to menu mode
  // pressing and holding the function key for more than 3 seconds
  void enterMenuMode()
  {
    if (_deviceMode != device_mode::menu)
    {
      // reconfigure keyboard
      _keyboard.setAutoRepeatInterval(250);
      _keyboard.setFastAutoRepeatInterval(25);
      _keyboard.setHoldTime(1000);
      _keyboard.setFastAutoRepeatDelay(15);

      _prevDeviceMode = _deviceMode;
      _deviceMode = device_mode::menu;
      _displayHandler.setAllLED(_menuHandler.getRed(), _menuHandler.getGreen(), _menuHandler.getBlue());
      _displayHandler.updateLEDs();
    }
  }

  // update configuration with changed settings and refresh display
  // called after settings were changed via the menu or the web configuration page
  void applyUpdatedSettings()
  {
    // apply configuration changes, the remaining settings are used directly
    _clock.setTimeZones();
    _gps.updateGPSSyncInterval();
    _calculator.setAngleMode(SettingsCache::angleMode);
    _calculator.setDecimals(SettingsCache::fixedDecimals);
    _calculator.setMaxExponentLength(SettingsCache::maxExpDigits);
    _displayHandler.setDisplayBrightness(currentBrightness());
    if (!_calculator.isInputPending())
    {
      // update, fixed decimals may have changed
      _calculator.updateNumber();
    }

    // refresh display
    if (_deviceMode == device_mode::calculator)
    {
      refreshCalcDisplay();
    }
    if (_deviceMode == device_mode::clock)
    {
      _displayHandler.clearDisplay();
    }
    _lighting.refresh();
  }

#if WEB_SUPPORT
  // settings update callback, called from the async web server task
  void onSettingsUpdate(const String &json, bool isReset)
  {
    xSemaphoreTake(_settingsUpdateMutex, portMAX_DELAY);
    _pendingSettingsJSON = json;
    _pendingSettingsIsReset = isReset;
    xSemaphoreGive(_settingsUpdateMutex);
    _settingsUpdatePending = true;
  }

  // time sync callback, called from the async web server task
  void onWebTimeSyncEvent(time_t utc)
  {
    xQueueSend(_timeSyncQueue, &utc, 0);
  }

  // process any browser-triggered time sync requests
  void processWebTimeSyncEvents()
  {
    time_t utc;
    while (xQueueReceive(_timeSyncQueue, &utc, 0) == pdTRUE)
    {
      // RTC/system time are only ever touched from this task, never from the web server's
      _clock.setSystemAndRTCTime(utc);
    }
  }

  // process any pending configuration changes made via the website
  void processWebUpdates()
  {
    // Settings/NVS are only ever touched from this task, never from the web server's
    if (_settingsUpdatePending.exchange(false))
    {
      String json;
      bool isReset;
      xSemaphoreTake(_settingsUpdateMutex, portMAX_DELAY);
      json = _pendingSettingsJSON;
      isReset = _pendingSettingsIsReset;
      xSemaphoreGive(_settingsUpdateMutex);

      if (isReset)
      {
        _settings.resetDefaults();
      }
      else
      {
        WebHelper::settingsFromJSON(_settings, json);
      }
      _settings.storeSettings();
      applyUpdatedSettings();
    }
  }

  // build the status page JSON, called from the async web server task
  String getStatusJSON()
  {
    unsigned long lastPirDetectionMillis = _pir.getLastPIRDetectionMillis();
    unsigned long lastSyncMillis = _gps.getLastSyncMillis();

    StatusInfo info;
    info.controllerMajor = MAJOR_VERSION;
    info.controllerMinor = MINOR_VERSION;
    info.controllerRevision = REVISION;
    info.keyboardMajor = _keyboard.getMajorVersion();
    info.keyboardMinor = _keyboard.getMinorVersion();
    info.keyboardRevision = _keyboard.getRevision();
    float boardTempC = _clock.getBoardTemperature();
    info.boardTemperature = (SettingsCache::temperatureCF == temperature_cf::celsius) ? boardTempC : Helper::celsiusToFahrenheit(boardTempC);
    info.externalTemperature = _temperature.getTemperature();
    info.temperatureUnit = (SettingsCache::temperatureCF == temperature_cf::celsius) ? "C" : "F";
    info.uptimeSeconds = static_cast<unsigned int>(esp_timer_get_time() / 1000000);
    info.presenceSeconds = (lastPirDetectionMillis != 0) ? static_cast<int>((millis() - lastPirDetectionMillis) / 1000) : -1;
    info.deviceMode = Helper::deviceModeToString(_deviceMode);
    info.displayType = Helper::displayTypeToString(_displayHandler.getDisplayType());
    info.highVoltageOn = _highVoltageOn;
    info.hvOnSeconds = getHVOnSeconds();
    info.wifiClients = _web.getStationCount();
    info.apIP = _web.getIP().toString();
    info.currentTime = static_cast<unsigned long>(time(nullptr));
    info.resetReason = Helper::resetReasonToString(esp_reset_reason());
    info.chipModel = ESP.getChipModel();
    info.chipRevision = ESP.getChipRevision();
    info.flashSize = ESP.getFlashChipSize();
    info.lastGpsSync = (lastSyncMillis != 0) ? static_cast<long>((millis() - lastSyncMillis) / 1000) : -1;

    return (WebHelper::buildStatusJSON(info));
  }
#endif

  // return true if HV should be on, false if HV should be off
  bool checkHVStatus()
  {
    bool result = true;
    // check if HV should be on or off
    if (_deviceMode != device_mode::antipoisoning)
    {
      // if PIR is configured and has timed out, shutdown the HV
      if (!_pir.getPresenceDetected() && (SettingsCache::pirMode == pir_mode::on))
      {
        result = false;
      }
      // if auto off is configured and active, shutdown the HV
      if (_autoOff && (SettingsCache::autoOffMode == auto_off_mode::on))
      {
        result = false;
      }
    }
    // shutdown the HV if temperature shutdown flag is set
    if (_temperatureShutdown)
    {
      result = false;
    }
    return (result);
  }

  // returns true if PIN_BUTTON1 is held down continuously at power up
  bool isFactoryResetButtonHeld()
  {
    unsigned long start = millis();
    while ((millis() - start) < FACTORY_RESET_HOLD_TIME)
    {
      if (digitalRead(PIN_BUTTON1) != LOW)
      {
        return (false);
      }
      delay(10);
    }
    return (true);
  }

  // flash all digits on and off a few times to confirm a factory reset happened
  void showFactoryResetConfirmation()
  {
    String pattern;
    for (uint8_t i = 0; i < _displayHandler.getDigitCount(); i++)
    {
      pattern += DECIMAL_SEPARATOR;
    }
    for (uint8_t i = 0; i < 5; i++)
    {
      _displayHandler.show(pattern);
      delay(300);
      _displayHandler.clearDisplay();
      delay(300);
    }
  }

  // display the version of the keyboard firmware on the right
  // display the version of the controller firmware on the left
  void showVersion()
  {
    char buffer[20];
    sprintf(buffer, "%02u.%02u.%02u  %02u.%02u.%02u", MAJOR_VERSION, MINOR_VERSION, REVISION,
            _keyboard.getMajorVersion(), _keyboard.getMinorVersion(), _keyboard.getRevision());
    _displayHandler.show(buffer);
  }

  // show heap size
  void showFreeMemory()
  {
    char buffer[20];
    sprintf(buffer, "%06u  %06u", static_cast<unsigned int>(esp_get_free_heap_size()), static_cast<unsigned int>(esp_get_minimum_free_heap_size()));
    _displayHandler.show(buffer);
  }

  // show uptime
  void showUptime()
  {
    char buffer[20];
    int64_t up = esp_timer_get_time();
    up /= 1000000; // get seconds
    unsigned int tm = (up / 60) % 60;
    unsigned int th = (up / 3600) % 24;
    unsigned int td = (up / 86400);
    sprintf(buffer, "%05u %02u %02u  ", td, th, tm);
    _displayHandler.show(buffer);
  }

  // show board temperature using the RTC sensor
  void showBoardTemperature()
  {
    float boardTempC = _clock.getBoardTemperature();
    float boardTemp = (SettingsCache::temperatureCF == temperature_cf::celsius) ? boardTempC : Helper::celsiusToFahrenheit(boardTempC);
    char buffer[20];
    sprintf(buffer, "%.2f", boardTemp);
    _displayHandler.show(buffer);
  }

  // show calc result
  void refreshCalcDisplay()
  {
    operation_return_code error = _calculator.getOperationReturnCode();
    if (error == operation_return_code::success)
    {
      CALC_NUMBER number = _calculator.getNumber();
      _displayHandler.showCalc(number.baseNegative, number.base,
                               number.exponentNegative, number.exponent);
    }
    else
    {
      _displayHandler.showCalcError((uint8_t)error);
    }
  }

#if WEB_SUPPORT
  // send the current device mode to web clients, called from the main loop
  void broadcastDeviceModeIfChanged()
  {
    if (!_deviceModeBroadcastInitialized || (_deviceMode != _lastBroadcastDeviceMode))
    {
      _deviceModeBroadcastInitialized = true;
      _lastBroadcastDeviceMode = _deviceMode;
      if (_web.isInitialized() && (_web.getClientCount() > 0))
      {
        _web.updateClients("MODE:", Helper::deviceModeToString(_deviceMode));
      }
    }
  }

  // build a snapshot of whatever is currently on the physical display
  String buildDisplaySnapshotText() const
  {
    String sign;
    if (_displayHandler.hasMenuSign() && _displayHandler.isMenuSignOn())
    {
      sign += 'M';
    }
    else
    {
      sign += (_displayHandler.isMinusSignOn() ? '-' : (_displayHandler.isPlusSignOn() ? '+' : ' '));
    }
    uint8_t digitCount = _displayHandler.getDigitCount();
    uint8_t dspOffset = _displayHandler.getDspOffset();
    String digits;
    String dpBits;
    for (uint8_t i = 0; i < digitCount; i++)
    {
      digits += (_displayHandler.isDigitOn(i) ? static_cast<char>('0' + _displayHandler.getDigit(i)) : ' ');
      dpBits += (_displayHandler.getDecimalSeparator(i + dspOffset) == display_state::on) ? '1' : '0';
    }
    String expSign;
    if (_displayHandler.isExpMinusSignOn())
    {
      expSign = "-";
    }
    else if (_displayHandler.isExpPlusSignOn())
    {
      expSign = "+";
    }
    return (sign + "|" + digits + "|" + dpBits + "|" + expSign);
  }

  // build a compact hex CSV of the current LED colors read directly from DisplayHandler
  String buildLedSnapshotText() const
  {
    String text;
    uint8_t ledCount = _displayHandler.getLedCount();
    uint8_t digitCount = _displayHandler.getDigitCount();
    uint8_t ledOffset = ((_displayHandler.getDisplayType() == display_type::in12a) ||
                         (_displayHandler.getDisplayType() == display_type::in12b))
                            ? 1
                            : 0;
    for (uint8_t d = 0; d < digitCount; d++)
    {
      if (d > 0)
      {
        text += ',';
      }
      uint8_t i = d + ledOffset;
      uint32_t color = (i < ledCount) ? _displayHandler.getLED(i) : 0;
      char buffer[7];
      sprintf(buffer, "%06x", static_cast<unsigned int>(color & 0xFFFFFF));
      text += buffer;
    }
    return (text);
  }

  // send a fresh display/LED snapshot to a single client (used on connect)
  void sendDisplaySnapshot(uint32_t id)
  {
    _web.updateClient("S:", buildDisplaySnapshotText() + "|" + buildLedSnapshotText(), id);
  }

  // unconditionally push a fresh display/LED snapshot to all web clients
  // keep it lightweight since is also called from separate task
  void broadcastDisplaySnapshot()
  {
    _displayDirty = false;
    _lastDisplaySnapshot = millis();
    if (_web.isInitialized() && (_web.getClientCount() > 0))
    {
      _web.updateClients("S:", buildDisplaySnapshotText() + "|" + buildLedSnapshotText());
    }
  }

  // push a fresh display/LED snapshot to all web clients, throttled to DISPLAY_BROADCAST_INTERVAL
  void broadcastDisplaySnapshotIfDue()
  {
    if (_displayDirty && ((millis() - _lastDisplaySnapshot) > DISPLAY_BROADCAST_INTERVAL))
    {
      broadcastDisplaySnapshot();
    }
  }

  // called on a key event coming from the web keypad
  void onKeypadEvent(uint8_t keyCode, bool functionKeyPressed, bool shiftKeyPressed)
  {
    KEYPADEVENT event{keyCode, functionKeyPressed, shiftKeyPressed};
    xQueueSend(_keypadEventQueue, &event, 0);
  }

  // process any key events queued by the web keypad
  void processWebKeypadEvents()
  {
    KEYPADEVENT event;
    while (xQueueReceive(_keypadEventQueue, &event, 0) == pdTRUE)
    {
      // web activity counts the same as a physical key press for auto-off/PIR timing,
      _keyboard.resetActivityTimestamp();
      _pir.onKeyPressed();
      switch (event.keyCode)
      {
      case KEYPAD_CMD_MODE_SWITCH:
        switchDeviceMode();
        break;

      case KEYPAD_CMD_MENU_MODE:
        enterMenuMode();
        break;

      default:
        processKeyboardEvent(event.keyCode, key_state::pressed, event.functionKeyPressed, event.shiftKeyPressed, special_keyboard_event::none, false);
        break;
      }
    }
  }

  // called when a client's register panel is shown or hidden
  void onRegisterSubscriptionEvent(bool subscribe)
  {
    register_sub_event event = subscribe ? register_sub_event::subscribe : register_sub_event::unsubscribe;
    xQueueSend(_registerSubscriptionQueue, &event, 0);
  }

  // process any register panel subscription changes
  void processRegisterSubscriptionEvents()
  {
    register_sub_event event;
    while (xQueueReceive(_registerSubscriptionQueue, &event, 0) == pdTRUE)
    {
      switch (event)
      {
      case register_sub_event::subscribe:
        _registerSubscriberCount++;
        if (_registerSubscriberCount == 1)
        {
          _calculator.attachRegisterUpadteCb([this](String regId, String value)
                                             { onRegisterUpdate(regId, value); });
        }
        // always send a fresh snapshot
        {
          REGISTERSTRINGMAP regStringMap;
          _calculator.getRegisterStrings(regStringMap);
          for (const auto &value : regStringMap)
          {
            _web.updateClients(value.first, value.second);
          }
        }
        break;

      case register_sub_event::unsubscribe:
        if (_registerSubscriberCount > 0)
        {
          _registerSubscriberCount--;
        }
        if (_registerSubscriberCount == 0)
        {
          _calculator.detachRegisterUpdateCb();
        }
        break;

      case register_sub_event::reset:
        _registerSubscriberCount = 0;
        _calculator.detachRegisterUpdateCb();
        break;
      }
    }
  }
#endif

  // called on a keyboard event coming from the physical keyboard
  void onKeyboardEvent(uint8_t keyCode, key_state keyState, bool functionKeyPressed, bool shiftKeyPressed, special_keyboard_event specialEvent)
  {
    processKeyboardEvent(keyCode, keyState, functionKeyPressed, shiftKeyPressed, specialEvent, true);
  }

  // processes a keyboard event coming from either the physical keyboard or the web keypad
  void processKeyboardEvent(uint8_t keyCode, key_state keyState, bool functionKeyPressed, bool shiftKeyPressed, special_keyboard_event specialEvent, bool isPhysical)
  {
    // __serial_print("Keycode: ");
    // __serial_print(keyCode);
    // __serial_print("  Keystate: ");
    // __serial_print((int)keyState);
    // __serial_print("  Special event: ");
    // __serial_print((int)specialEvent);
    // __serial_print("  Function key pressed: ");
    // __serial_println(functionKeyPressed);

    if (keyState == key_state::idle)
    {
      // when a key is pressed, the PIR timeout is reset even if no presence was detected
      _pir.onKeyPressed();
      // when a key is pressed, stop rotation
      if (_deviceMode == device_mode::antipoisoning)
      {
        _rotationStopped = true;
      }
    }
    if (keyState == key_state::pressed)
    {
      // if CLS / AC is pressed and in timer ended status
      // stop timer ended indication and ignore key
#if RPN_MODE
      if (keyCode == KEY_CLS)
#else
      if (keyCode == KEY_AC)
#endif
      {
        if (_clock.getTimerEnded())
        {
          _clock.resetTimerEnded();
          return;
        }
      }
    }

    // ignore first key pressed if waking up or in antipoisoning mode
    if (_autoOff || !_pir.getPresenceDetected() || (_deviceMode == device_mode::antipoisoning))
    {
      return;
    }
    switch (specialEvent)
    {
    // switch between clock and calculator mode or leave menu mode
    case special_keyboard_event::mode_switch:
      _lighting.refresh();
      switchDeviceMode();
      break;

    // enter menu mode
    case special_keyboard_event::menu_mode:
      enterMenuMode();
      break;

    case special_keyboard_event::none:
      // do nothing
      break;
    }

    if (functionKeyPressed && (keyState == key_state::pressed))
    {
      if (handleShortcuts(keyCode, isPhysical))
      {
        // shortcut handled everything, nothing else to do
        return;
      }
    }

    switch (_deviceMode)
    {
    case device_mode::calculator:
      // calculator is keyboard driven, send key event and update display
      if (keyState == key_state::pressed)
      {
        if (_calculator.onKeyboardEvent(keyCode, keyState, functionKeyPressed, shiftKeyPressed))
        {
          // first we may have to stop scrolling
          if (_scrollResult)
          {
            _scrollResult = false;
            _calculator.resetScrollInfo();
          }
          // show result
          refreshCalcDisplay();
          _lighting.refresh();
        }
      }
      break;

    case device_mode::clock:
      // the clock also needs some keyboard events for setting
      // the time and changing the clock mode
      _clock.onKeyboardEvent(keyCode, keyState, functionKeyPressed, shiftKeyPressed);
      if (_clock.getRefreshLighting())
      {
        _lighting.refresh();
        _clock.resetRefreshLighting();
      }
      break;

    case device_mode::menu:
      // menus are keyboard driven, send key event and update display
      _menuHandler.onKeyboardEvent(keyCode, keyState, functionKeyPressed);
      if (_displayHandler.hasMenuSign())
      {
        _displayHandler.show(_menuHandler.getDisplay(), digit_content::none, true);
      }
      else
      {
        _displayHandler.show(_menuHandler.getDisplay());
      }
      _displayHandler.setAllLED(_menuHandler.getRed(), _menuHandler.getGreen(), _menuHandler.getBlue());
      _displayHandler.updateLEDs();
      break;

    case device_mode::antipoisoning:
      // do nothing
      break;
    }
  }

  // handle some combinations of the function key with other keys
  bool handleShortcuts(uint8_t keyCode, bool isPhysical)
  {
    bool result = false;
    switch (keyCode)
    {
    case KEY_PLUS:
      switch (_deviceMode)
      {
      case device_mode::clock:
        // adjust the time by plus 1 second
        _clock.adjustTime(1);
        break;

      case device_mode::calculator:
        changeBrightness(1);
        break;

      default:
        break;
      }
      break;

    case KEY_MINUS:
      switch (_deviceMode)
      {
      case device_mode::clock:
        // adjust the time by minus 1 second
        _clock.adjustTime(-1);
        break;

      case device_mode::calculator:
        changeBrightness(-1);
        break;

      default:
        break;
      }
      break;

    case KEY_9:
      switch (_deviceMode)
      {
      case device_mode::calculator:
        switch (SettingsCache::calcInputDirec)
        {
        case calc_input_direc::ltr:
          SettingsCache::calcInputDirec = calc_input_direc::rtl;
          break;

        case calc_input_direc::rtl:
          SettingsCache::calcInputDirec = calc_input_direc::rtl_zero_padding;
          break;

        case calc_input_direc::rtl_zero_padding:
          SettingsCache::calcInputDirec = calc_input_direc::ltr;
          break;
        }
        refreshCalcDisplay();
        _lighting.refresh();
        break;

      default:
        break;
      }
      break;

    case KEY_00:
      switch (_deviceMode)
      {
      case device_mode::calculator:
        _calculator.trimXToDisplayedValue();
        break;

      case device_mode::clock:
        _displayHandler.saveLEDColors();
        _displayHandler.clearLEDs();
        showVersion();
        delay(3000);
        restoreDisplay();
        break;

      default:
        break;
      }
      break;

#if RPN_MODE
    case KEY_STO:
#else
    case KEY_MS:
#endif
      switch (_deviceMode)
      {
      case device_mode::clock:
      case device_mode::calculator:
        // store all current settings cache values in NVS
        _settings.updateFromCache();
        _settings.storeSettings();
        break;

      default:
        break;
      }
      break;

#if RPN_MODE
    case KEY_RCL:
#else
    case KEY_MMINUS:
#endif
      switch (_deviceMode)
      {
      case device_mode::clock:
        _displayHandler.saveLEDColors();
        _displayHandler.clearLEDs();
        showUptime();
        delay(3000);
        restoreDisplay();
        break;

      default:
        break;
      }
      break;

#if RPN_MODE
    case KEY_CLR:
#else
    case KEY_MC:
#endif
      switch (_deviceMode)
      {
      case device_mode::menu:
        // restore all settings to the default value
        _settings.resetDefaults();
        _settings.storeSettings();
        _menuHandler.resetValue();
        break;

      default:
        break;
      }
      break;

#if RPN_MODE
    case KEY_BACK:
#else
    case KEY_C:
#endif
      switch (_deviceMode)
      {
      case device_mode::menu:
#if WEB_SUPPORT
        if (isPhysical)
        {
          // reset the access point SSID and password, and the firmware update
          // password, to their firmware defaults, restricted to the physical keyboard
          _web.resetApSsid();
          _web.resetApPassword();
          _settings.resetOtaPassword();
        }
#endif
        break;

      default:
        // step through the light modes, override the lighting time constraints
        _lighting.switchLightingMode(_deviceMode);
        _lighting.refresh();
        break;
      }
      break;

#if RPN_MODE
    case KEY_CLS:
#else
    case KEY_AC:
#endif
      switch (_deviceMode)
      {
      case device_mode::menu:
        // leave menu mode and ignore changes
        _displayHandler.clearDisplay();
        _displayHandler.clearLEDs();
        _deviceMode = _prevDeviceMode;

        // reload current settings to discard changes
        _settings.readSettings();
        _menuHandler.revertValue();
        if (_deviceMode == device_mode::calculator)
        {
          refreshCalcDisplay();
        }
        _lighting.refresh();
        break;

      default:
        // restore the lighting time constraints
        _lighting.forcedOff();
        break;
      }
      break;

    case KEY_EXP:
      switch (_deviceMode)
      {
      // set force scientific notation mode
      case device_mode::calculator:
        _calculator.switchForceScientific();
        if (_scrollResult)
        {
          // display format has changed, stop scrolling
          _scrollResult = false;
          _calculator.resetScrollInfo();
        }
        refreshCalcDisplay();
        _lighting.refresh();
        break;

      default:
        break;
      }

      break;

    case KEY_CHS:
#if OTA_SUPPORT
      if (isPhysical)
      {
        // physically arm (or, if already armed, disarm) firmware updates via [F] + [±];
        _web.toggleFirmwareUpdateArm(OTA_ARM_WINDOW);
      }
#endif
      break;

    case KEY_DOT:
      switch (_deviceMode)
      {
      // start or stop result scrolling
      case device_mode::calculator:
        _scrollResult = !_scrollResult;
        _calculator.resetScrollInfo();
        if (!_scrollResult)
        {
          // scrolling stopped, update display
          refreshCalcDisplay();
          _lighting.refresh();
        }
        result = true; // everything done
        break;

      // show board temperature
      case device_mode::clock:
        _displayHandler.saveLEDColors();
        _displayHandler.clearLEDs();
        showBoardTemperature();
        delay(3000);
        restoreDisplay();
        break;

      default:
        break;
      }
      break;

#if RPN_MODE
    case KEY_ENTER:
#else
    case KEY_EQUALS:
#endif
      switch (_deviceMode)
      {
      case device_mode::calculator:
      case device_mode::clock:
#if WEB_SUPPORT
        if (!_web.isInitialized())
        {
          _web.begin();
          displayIP(_web.getIP());
        }
        else
        {
          _web.end();
        }
        result = true;
#endif
        break;

      default:
        break;
      }
      break;

    case KEY_0:
      switch (_deviceMode)
      {
      case device_mode::clock:
        _displayHandler.saveLEDColors();
        _displayHandler.clearLEDs();
        showFreeMemory();
        delay(3000);
        restoreDisplay();
        result = true;
        break;

      default:
        break;
      }
      break;
    }
    return (result);
  }

  // called to sync the rtc time with the gps time
  void onGPSTimeSyncEvent(time_t utc)
  {
    // set the real time clock
    _clock.setSystemAndRTCTime(utc, false);
    _gps.notifySynced();

    // notify GPS time sync
    if (SettingsCache::gpsNotifySync == gps_notify_sync::on)
    {
      _displayHandler.saveLEDColors();
      _displayHandler.setAllLED(SettingsCache::gpsSyncColor.red, SettingsCache::gpsSyncColor.green, SettingsCache::gpsSyncColor.blue);
      _displayHandler.updateLEDs();
      delay(200);
      if (_lighting.isOn())
      {
        _displayHandler.restoreLEDColors();
        _displayHandler.updateLEDs();
      }
      else
      {
        _displayHandler.clearLEDs();
      }
    }
  }

  // restore the display after showing a temporary information
  void restoreDisplay()
  {
    switch (_deviceMode)
    {
    case device_mode::calculator:
      refreshCalcDisplay();
      if (_lighting.isOn())
      {
        _displayHandler.restoreLEDColors();
        _displayHandler.updateLEDs();
      }
      else
      {
        _displayHandler.clearLEDs();
      }
      break;

    case device_mode::clock:
      _displayHandler.clearDisplay();
      if (_lighting.isOn())
      {
        _displayHandler.restoreLEDColors();
        _displayHandler.updateLEDs();
      }
      else
      {
        _displayHandler.clearLEDs();
      }
      break;

    default:
      break;
    }
  }

  // called for long calculations
  // this is used to display an animation
  void onLongOperation(long_operation lo)
  {
    switch (lo)
    {
    case long_operation::begin:
      _displayHandler.createBusyCalcTask();
      break;

    case long_operation::end:
      _displayHandler.stopBusyCalcTask();
      break;
    }
  }

#if WEB_SUPPORT
  // display the local IP address
  void displayIP(IPAddress ip, int seconds = 2)
  {
    _displayHandler.saveLEDColors();
    _displayHandler.clearLEDs();
    String s = ip.toString();
    int padding = (_displayHandler.getDigitCount() + 3) - s.length(); // 3 dots
    for (int i = 0; i < padding; i++)
    {
      s += " ";
    }
    _displayHandler.show(s);
    delay(seconds * 1000);
    restoreDisplay();
  }

  // client connection callback
  void onClientConnection(uint32_t id)
  {
    // send the display type first, the web UI needs it to pick a digit rendering style
    // before it processes the display snapshot below
    _web.updateClient("TYPE:", Helper::displayTypeToString(_displayHandler.getDisplayType()), id);
    // send the current display/LED snapshot
    sendDisplaySnapshot(id);
    // send the current device mode
    _web.updateClient("MODE:", Helper::deviceModeToString(_deviceMode), id);
  }

  // client disconnection callback
  void onClientDisconnection(uint32_t id)
  {
    if (_web.getClientCount() == 0)
    {
      // if no clients left, stop notifications
      register_sub_event event = register_sub_event::reset;
      xQueueSend(_registerSubscriptionQueue, &event, 0);
    }
  }

  // send registers to web clients
  void onRegisterUpdate(String regId, String value)
  {
    if (_web.isInitialized() && (_web.getClientCount() > 0))
    {
      _web.updateClients(regId, value);
    }
  }
#endif

  // check if it's time to turn off the high voltage or switch to clock mode
  void checkAutoOff()
  {
    if (SettingsCache::autoOffMode != auto_off_mode::off)
    {
      if (millis() - _keyboard.getLastKeyTimestamp() > (SettingsCache::autoOffDelay))
      {
        switch (SettingsCache::autoOffMode)
        {
        case auto_off_mode::on:
          _autoOff = true;
          break;

        case auto_off_mode::clock:
          if ((_deviceMode != device_mode::antipoisoning) && (_deviceMode != device_mode::menu))
          {
            if (_deviceMode != device_mode::clock)
            {
              _deviceMode = device_mode::clock;
              _displayHandler.clear();
              _lighting.refresh();
            }
          }
          break;

        case auto_off_mode::off:
          // do nothing
          break;
        }
      }
      else
      {
        _autoOff = false;
      }
    }
    else
    {
      _autoOff = false;
    }
  }

  // check if we have to switch to antipoisoning mode
  void checkAntiPoisoning(const struct tm *tm)
  {
    // switch to antipoisoning mode only if in clock or calculator mode
    if (_deviceMode == device_mode::clock || _deviceMode == device_mode::calculator)
    {

      // check if anti-poisoning is enabled
      if (SettingsCache::acpDuration > 0)
      {
        // check if in time range
        if (Helper::isInTimeRange(SettingsCache::acpStartTime.hour, SettingsCache::acpStartTime.minute,
                                  tm->tm_hour, tm->tm_min, SettingsCache::acpDuration))
        {
          if (!_rotationStopped)
          {
            _prevDeviceMode = _deviceMode;
            _deviceMode = device_mode::antipoisoning;
            setRotationInterval();
            _displayHandler.clear();
          }
        }
        else
        {
          _rotationStopped = false;
        }
      }
    }
  }

  // check if we have to dim the display
  void checkDimming(const struct tm *tm)
  {
    if (SettingsCache::dimDuration > 0 &&
        Helper::isInTimeRange(SettingsCache::dimStartTime.hour, SettingsCache::dimStartTime.minute,
                              tm->tm_hour, tm->tm_min, SettingsCache::dimDuration))
    {
      if (!_dimmingActive)
      {
        _dimmingActive = true;
        _displayHandler.setDisplayBrightness(SettingsCache::dimBrightness);
      }
    }
    else
    {
      if (_dimmingActive)
      {
        _dimmingActive = false;
        _displayHandler.setDisplayBrightness(SettingsCache::brightness);
      }
    }
  }

  // check board temperature every 10 seconds
  void checkBoardTemperature()
  {
    if (millis() - _lastTempCheckTimestamp > TEMP_CHECK_INTERVAL)
    {
      float boardTemp = _clock.getBoardTemperature(); // temperature sensor is in the RTC chip
      __serial_print("Board temperature: ");
      __serial_println(boardTemp);
      if (_temperatureShutdown)
      {
        if (boardTemp <= RECOVER_TEMP)
        {
          _temperatureShutdown = false;
        }
      }
      else if (boardTemp >= SHUTDOWN_TEMP)
      {
        _temperatureShutdown = true;
      }
      _lastTempCheckTimestamp = millis();
    }
  }

  // set a random rotation interval for each digit
  void setRotationInterval()
  {
    for (int i = 0; i < (_displayHandler.getDigitCount() + MAX_SPECIAL_CHARS_DIGITS); i++)
    {
      _rotationData[i].rotationInterval = random(100, 1000);
    }
  }

  // rotate the digits for cathode poisoning prevention
  void rotate(const struct tm *tm)
  {
    if (Helper::isInTimeRange(SettingsCache::acpStartTime.hour, SettingsCache::acpStartTime.minute,
                              tm->tm_hour, tm->tm_min, SettingsCache::acpDuration))
    {
      if (!_rotationStopped)
      {
        // rotate digits
        bool change = false;
        for (int i = 0; i < (_displayHandler.getDigitCount() + MAX_SPECIAL_CHARS_DIGITS); i++)
        {
          if (millis() - _rotationData[i].lastRotationTimestamp > _rotationData[i].rotationInterval)
          {
            _rotationData[i].rotationStep++;
            if (_rotationData[i].rotationStep > 9)
            {
              _rotationData[i].rotationStep = 0;
            }
            _rotationData[i].lastRotationTimestamp = millis();

            if (i < _displayHandler.getDigitCount())
            {
              _displayHandler.setDigit(i, _rotationData[i].rotationStep);
            }
            else
            {
              _displayHandler.setSpecialChar(i - _displayHandler.getDigitCount(), _rotationData[i].rotationStep);
            }
            change = true;
          }
          if (change)
          {
            _displayHandler.show();
          }
        }
      }
    }
    else
    {
      _rotationStopped = false;
      // no longer in time range, exit antipoisoning mode
      restoreDeviceMode();
    }
    if (_rotationStopped)
    {
      // rotation manually stopped, exit antipoisoning mode
      restoreDeviceMode();
    }
  }

  // switch back to the previous device mode
  void restoreDeviceMode()
  {
    _deviceMode = _prevDeviceMode;
    _prevDeviceMode = _deviceMode;
    switch (_deviceMode)
    {
    case device_mode::clock:
      _displayHandler.clear();
      break;

    case device_mode::calculator:
      refreshCalcDisplay();
      break;

    default: // avoid warning
      break;
    }
  }

  // Hide settings that do not make sense for a particular display type
  void adjustVisibleSettings()
  {
    switch (_displayHandler.getDisplayType())
    {
    case display_type::led:
      // we have to hide all lighting settings because
      // the 7-seg LED display has no back/underlighting
      _settings.hideSetting(setting_id::ledmode, true);
      _settings.hideSetting(setting_id::calcrgbmode, true);
      _settings.hideSetting(setting_id::clockrgbmode, true);
      _settings.hideSetting(setting_id::trigcolorchange, true);
      _settings.hideSetting(setting_id::ledstarttime, true);
      _settings.hideSetting(setting_id::ledduration, true);
      _settings.hideSetting(setting_id::ledstarttime2, true);
      _settings.hideSetting(setting_id::ledduration2, true);
      _settings.hideSetting(setting_id::negativecolor, true);
      _settings.hideSetting(setting_id::positivecolor, true);
      _settings.hideSetting(setting_id::errorcolor, true);
      _settings.hideSetting(setting_id::negexpcolor, true);
      _settings.hideSetting(setting_id::posexpcolor, true);
      _settings.hideSetting(setting_id::fixedcalccolor, true);
      _settings.hideSetting(setting_id::timecolor, true);
      _settings.hideSetting(setting_id::timecolor2, true);
      _settings.hideSetting(setting_id::datecolor, true);
      _settings.hideSetting(setting_id::tempcolor, true);
      _settings.hideSetting(setting_id::fixedcolor, true);
      _settings.hideSetting(setting_id::gpsnotifysync, true);
      _settings.hideSetting(setting_id::gpssynccolor, true);
      _settings.hideSetting(setting_id::notifytimer, true);
      _settings.hideSetting(setting_id::timercolor, true);
      break;

    default:
      break;
    }

#if !WEB_SUPPORT
    // AP autostart is meaningless without web support
    _settings.hideSetting(setting_id::apautostart, true);
#endif
  }

  // change display brightness
  void changeBrightness(int value)
  {
    int current = SettingsCache::brightness;
    int max = _settings.getSettingMax(setting_id::brightness);
    int min = _settings.getSettingMin(setting_id::brightness);
    current += value;
    if (current <= max && current >= min)
    {
      SettingsCache::brightness = current;
      _displayHandler.setDisplayBrightness(current);
    }
  }
};
