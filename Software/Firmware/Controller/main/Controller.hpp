// Controller.hpp

// main logic

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <time.h>
#include <Config.h>
#include <FirmwareInfo.h>
#include <GlobalEnums.h>
#include <Wire.h>
#include <DebugDefs.h>
#include <Errors.hpp>
#include <KeyboardHandler.hpp>
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
#if WEBSOCKET_SUPPORT
#include <CalcWebSocketServer.hpp>
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

// minimum allowed time between two high-voltage switch-on events
constexpr unsigned long MIN_HVON_INTERVAL = 1000; // in ms

// struct needed for digit rotation for cathode poisoning prevention
typedef struct
{
  uint8_t rotationStep;
  int rotationInterval;
  unsigned long lastRotationTimestamp;
} ROTATIONDATA;

class Controller
{
public:
  Controller()
      : _displayHandler(DISPLAY_TYPE, PIN_DATA, PIN_STORE, PIN_SHIFT, PIN_BLANK, PIN_LEDCTL),
        _clock(&_displayHandler),
        _lighting(&_displayHandler),
        _pir(PIN_PIR),
        _temperature(PIN_TEMPERATURE),
#if WEBSOCKET_SUPPORT
        _web(PIN_NETACT),
#endif
        _menuHandler(&_settings, _displayHandler.getDecimalSeparatorPosition())

  {
    _highVoltageOn = true;
    _autoOff = false;
    _deviceMode = device_mode::calculator;
    _prevDeviceMode = device_mode::calculator;
    _rotationData = (ROTATIONDATA *)calloc(_displayHandler.getDigitCount() + MAX_SPECIAL_CHARS_DIGITS, sizeof(ROTATIONDATA));
    _rotationStopped = false;
    _scrollResult = false;
  }

  virtual ~Controller()
  {
    free(_rotationData);
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
    pinMode(PIN_BLANK, OUTPUT);
    pinMode(PIN_NETACT, OUTPUT);
    pinMode(PIN_BUTTON1, INPUT);

    // set blank line to high
    digitalWrite(PIN_BLANK, HIGH);

    // initialize non-volatile storage
    if (_settings.begin())
    {
      // load settings into settings cache
      _settings.readSettings();
      // hide some setting according to display type
      adjustVisibleSettings();

      // initialize calculator
      _calculator.begin(_displayHandler.getDigitCount(), _displayHandler.getDecimalSeparatorCount(), _displayHandler.hasPlusSign());

      // get notified on long operations
      _calculator.attachLongOperationCb(std::bind(&Controller::onLongOperation, this, std::placeholders::_1));

      // initialize display
      _displayHandler.begin();
      _displayHandler.clearDisplay();

      // initialize LED lighting
      _lighting.begin();
      _lighting.off();

      // initialize menu handler
      _menuHandler.begin(_displayHandler.getDigitCount());

      // set gps pins
      _gps.begin(PIN_GPSRX, PIN_GPSTX);

      // get notified on gps time sync
      _gps.attach(std::bind(&Controller::onGPSTimeSyncEvent, this, std::placeholders::_1));

#if WEBSOCKET_SUPPORT
      // get notified on websocket connections and disconnections
      _web.attachConnectionCb(std::bind(&Controller::onClientConnection, this, std::placeholders::_1));
      _web.attachDisconnectionCb(std::bind(&Controller::onClientDisconnection, this, std::placeholders::_1));
#endif

      // init I2C
      Wire.begin();

      // initialize clock
      _clock.begin();

      // init keyboard
      Serial1.begin(KEYBOARD_COMM_SPEED, SERIAL_8N1, PIN_KINT);
      _keyboard.begin(Serial1);

      // get notified on keyboard events
      _keyboard.attach(std::bind(&Controller::onKeyboardEvent, this, std::placeholders::_1,
                                 std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                                 std::placeholders::_5));

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

      // show version
      if (SettingsCache::showVersion == show_version::on)
      {
        showVersion();
        delay(1000);
      }

      // set startup mode
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
    TimeElements tm;

    // we need the current system time
    _clock.getCurrentTime(&tm);

    // process keyboard input
    _keyboard.process();

    // check if it's time to switch to clock mode or turn the HV off
    checkAutoOff();

    // check if it's time to rotate digits for antipoisoning
    checkAntiPoisoning(&tm);

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

    // process according to current device mode
    switch (_deviceMode)
    {
    case device_mode::clock:
      _clock.resetRefreshLighting();
      _clock.process(&tm);
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
#if WEBSOCKET_SUPPORT
      if (_web.isInitialized())
      {
        _web.process();
      }
#endif
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

private:
  DisplayHandler _displayHandler;
  Clock _clock;
  Lighting _lighting;
  PIR _pir;
  Temperature _temperature;
#if WEBSOCKET_SUPPORT
  CalcWebSocketServer _web;
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

  // turn the high voltage on
  void hvON()
  {
    if (!_highVoltageOn)
    {
      // avoid too fast off and on switching
      if (millis() - _hvOffTimestamp > MIN_HVON_INTERVAL)
      {
        _highVoltageOn = true;
        digitalWrite(PIN_HVLED, HIGH);
        if (_displayHandler.getDisplayType() != display_type::led)
        {
          digitalWrite(PIN_HVENABLE, HIGH);
        }
        else
        {
          _displayHandler.getLEDDriver()->setBrightness(SettingsCache::brightness);
          _displayHandler.getLEDDriver()->on();
        }
        D_println("Turn HV on");
      }
    }
  }

  // turn the high voltage off
  void hvOFF()
  {
    if (_highVoltageOn)
    {
      _highVoltageOn = false;
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
      D_println("Turn HV off");
    }
  }

  // return the status of the high voltage
  bool isHVON() const
  {
    return (_highVoltageOn);
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

      // update some indirect settings
      _gps.updateGPSSyncInterval();
      _calculator.setAngleMode(SettingsCache::angleMode);
      _calculator.setDecimals(SettingsCache::fixedDecimals);
      _calculator.setMaxExponentLength(SettingsCache::maxExpDigits);
      _displayHandler.setDisplayBrightness(SettingsCache::brightness);
      if (!_calculator.isInputPending())
      {
        // update, fixed decimals may have changed
        _calculator.updateNumber();
      }
      if (_deviceMode == device_mode::calculator)
      {
        refreshCalcDisplay();
      }
      _lighting.refresh();
      break;

    case device_mode::antipoisoning:
      // do nothing
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
    }
  }

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
    return (result);
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

  // called on a keyboard event
  void onKeyboardEvent(uint8_t keyCode, key_state keyState, bool functionKeyPressed, bool shiftKeyPressed, special_keyboard_event specialEvent)
  {
    // D_print("Keycode: ");
    // D_print(keyCode);
    // D_print("  Keystate: ");
    // D_print((int)keyState);
    // D_print("  Special event: ");
    // D_print((int)specialEvent);
    // D_print("  Function key pressed: ");
    // D_println(functionKeyPressed);

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
#if CALC_TYPE == CALC_TYPE_RPN
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
      if (handleShortcuts(keyCode))
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
          if (keyState == key_state::pressed)
          {
            _lighting.refresh();
          }
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

  // handle some special key combinations
  bool handleShortcuts(uint8_t keyCode)
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

    case KEY_00:
      if (_deviceMode == device_mode::calculator)
      {
        _displayHandler.saveLEDColors();
        _displayHandler.clearLEDs();
        showVersion();
        delay(3000);
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
      }
      break;

#if CALC_TYPE == CALC_TYPE_RPN
    case KEY_STO:
#else
    case KEY_MS:
#endif
      switch (_deviceMode)
      {
      case device_mode::clock:
        // store the current clock mode in NVS
        _settings.setSetting(setting_id::clockmode, SettingsCache::clockMode);
        _settings.storeSettings();
        break;

      case device_mode::calculator:
        // store the fixed decimals setting in NVS
        _settings.setSetting(setting_id::fixeddecimals, SettingsCache::fixedDecimals);
        _settings.storeSettings();
        break;

      case device_mode::antipoisoning:
      case device_mode::menu:
        // do nothing
        break;
      }
      break;

#if CALC_TYPE == CALC_TYPE_RPN
    case KEY_RCL:
#else
    case KEY_MR:
#endif
      if (_deviceMode == device_mode::calculator)
      {
        _displayHandler.saveLEDColors();
        _displayHandler.clearLEDs();
        showFreeMemory();
        delay(3000);
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
      }
      break;
#if CALC_TYPE == CALC_TYPE_RPN
    case KEY_CLR:
#else
    case KEY_MC:
#endif
      // restore all settings to the default value
      if (_deviceMode == device_mode::menu)
      {
        _settings.resetDefaults();
        _settings.storeSettings();
        _menuHandler.resetValue();
      }
      break;

#if CALC_TYPE == CALC_TYPE_RPN
    case KEY_BACK:
#else
    case KEY_C:
#endif
      // step through the light modes, override the lighting time constraints
      _lighting.switchLightingMode(_deviceMode);
      _lighting.refresh();
      break;

#if CALC_TYPE == CALC_TYPE_RPN
    case KEY_CLS:
#else
    case KEY_AC:
#endif
      if (_deviceMode == device_mode::menu)
      {
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
      }
      else
      {
        // restore the lighting time constraints
        _lighting.forcedOff();
      }
      break;

    case KEY_EXP:
      // set force scientific notation mode
      if (_deviceMode == device_mode::calculator)
      {
        _calculator.switchForceScientific();
        if (_scrollResult)
        {
          // display format has changed, stop scrolling
          _scrollResult = false;
          _calculator.resetScrollInfo();
        }
        refreshCalcDisplay();
        _lighting.refresh();
      }
      break;

    case KEY_DOT:
      // start or stop result scrolling
      if (_deviceMode == device_mode::calculator)
      {
        _scrollResult = !_scrollResult;
        _calculator.resetScrollInfo();
        if (!_scrollResult)
        {
          // scrolling stopped, update display
          refreshCalcDisplay();
          _lighting.refresh();
        }
        result = true; // everything done
      }
      break;

#if WEBSOCKET_SUPPORT
#if CALC_TYPE == CALC_TYPE_RPN
    case KEY_ENTER:
#else
    case KEY_EQUALS:
#endif
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
      break;
#endif
    }
    return (result);
  }

  // called to sync the rtc time with the gps time
  void onGPSTimeSyncEvent(time_t utc)
  {
    // set the real time clock
    _clock.setSystemAndRTCTime(utc);

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

#if WEBSOCKET_SUPPORT
  // display the local IP address
  void displayIP(IPAddress ip)
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
    delay(2000);
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
  }

  // client connection callback
  void onClientConnection(uint32_t id)
  {
    if (_web.getClientCount() == 1)
    {
      // first client, start notifications
      _calculator.attachRegisterUpadteCb(std::bind(&Controller::onRegisterUpdate, this, std::placeholders::_1, std::placeholders::_2));
    }
    // get all registers
    REGISTERSTRINGMAP regStringMap;
    _calculator.getRegisterStrings(regStringMap);
    // send all registers to client
    for (const auto &value : regStringMap)
    {
      _web.updateClient(value.first, value.second, id);
    }
  }

  // client disconnection callback
  void onClientDisconnection(uint32_t id)
  {
    if (_web.getClientCount() == 0)
    {
      // if no clients left, stop notifications
      _calculator.detachRegisterUpdateCb();
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
  void checkAntiPoisoning(TimeElements *tm)
  {
    // switch to antipoisoning mode only if in clock or calculator mode
    if (_deviceMode == device_mode::clock || _deviceMode == device_mode::calculator)
    {

      // check if anti poisioning is enabled
      if (SettingsCache::acpDuration > 0)
      {
        // check if in time range
        if (Helper::isInTimeRange(SettingsCache::acpStartTime.hour, SettingsCache::acpStartTime.minute,
                                  tm->Hour, tm->Minute, SettingsCache::acpDuration))
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

  // set a random rotation interval for each digit
  void setRotationInterval()
  {
    for (int i = 0; i < (_displayHandler.getDigitCount() + MAX_SPECIAL_CHARS_DIGITS); i++)
    {
      _rotationData[i].rotationInterval = random(100, 1000);
    }
  }

  // rotate the digits for cathode poisoning prevention
  void rotate(TimeElements *tm)
  {
    if (Helper::isInTimeRange(SettingsCache::acpStartTime.hour, SettingsCache::acpStartTime.minute,
                              tm->Hour, tm->Minute, SettingsCache::acpDuration))
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
      _settings.hideSetting(setting_id::datecolor, true);
      _settings.hideSetting(setting_id::tempcolor, true);
      _settings.hideSetting(setting_id::fixedcolor, true);
      _settings.hideSetting(setting_id::gpsnotifysync, true);
      _settings.hideSetting(setting_id::gpssynccolor, true);
      _settings.hideSetting(setting_id::notifytimer, true);
      _settings.hideSetting(setting_id::timercolor, true);
      break;

    default:
      // just hide brightness setting
      _settings.hideSetting(setting_id::brightness, true);
      break;
    }
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
