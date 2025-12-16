// PIR.hpp

// provides GPS functionality

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <functional>
#include <Timezone.h>
#include <ubGPSTime.hpp>
#include <SettingsCache.hpp>

constexpr unsigned long GPS_SYNC_INTERVAL_SHORT = 15 * 1000; // the initial interval is 15 seconds
constexpr unsigned long GPS_MSG_INTERVAL = 60;               // one msg every 60 seconds

class GPS
{

protected:
  using notifyCallBack = std::function<void(time_t utc)>;

public:
  GPS()
  {
    _notify = nullptr;
    _gpsSyncTimestamp = 0;
    _resetTimestamp = 0;
    _gpsSyncIntervalActive = GPS_SYNC_INTERVAL_SHORT;
    _initialized = false;
    _gpsMessageInterval = GPS_MSG_INTERVAL;
  }

  virtual ~GPS()
  {
  }

  // set serial pins
  void begin(uint8_t pinRX, uint8_t pinTX)
  {
    _pinRX = pinRX;
    _pinTX = pinTX;
  }

  // initialize the GPS module
  bool initialize()
  {
    bool retVal = false;
    if (_uGPS.isInitialized())
    {
      // we don't need NMEA messages
      _uGPS.disableDefaultNMEA();
      // subscriptions
      _uGPS.subscribeGPSStatus(0); // we don't need status messages
      _uGPS.subscribeTimeUTC(_gpsMessageInterval);
      // set callback to receive GPS messages
      _uGPS.attach(std::bind(&GPS::onGPSMessage, this, std::placeholders::_1));

      // print some GPS module information
      D_println();
      D_println("GPS module successfully initialized");
      D_print("Software version: ");
      D_println(_uGPS.getModuleVersion().swVersion);
      D_print("Hardware version: ");
      D_println(_uGPS.getModuleVersion().hwVersion);
      for (uint8_t i = 0; i < MAX_EXTENSIONS; i++)
      {
        D_printf("Extension %u: ", i + 1);
        D_println(_uGPS.getModuleVersion().extensions[i]);
      }
      _initialized = true;
      retVal = true;
    }
    else
    {
      D_println("GPS not initialized");
    }
    return (retVal);
  }

  // stop GPS
  void end()
  {
    Serial2.end();
    _initialized = false;
    _uGPS.detach();
    _uGPS.end();
    _resetTimestamp = 0;
    _gpsSyncIntervalActive = GPS_SYNC_INTERVAL_SHORT;
    D_println("GPS shutdown");
  }

  // return if GPS is initialized
  bool isInitialized() const
  {
    return (_initialized);
  }

  // set the callback function
  void attach(notifyCallBack callBack)
  {
    _notify = callBack;
  }

  // remove callback function
  void detach()
  {
    _notify = nullptr;
  }

  // gps logic
  void process()
  {
    if (SettingsCache::gpsMode == gps_mode::on)
    {
      if (_resetTimestamp == 0)
      {
        Serial2.begin(getSpeed(SettingsCache::gpsSpeed), SERIAL_8N1, _pinRX, _pinTX);
        _uGPS.begin(Serial2);
        _uGPS.resetModule();
        _resetTimestamp = millis();
      }
      else
      {
        if (!_uGPS.isInitialized())
        {
          // check for elpased time since reset
          if (millis() - _resetTimestamp > 5000) // 5 seconds
          {
            // time to initialize
            _uGPS.requestVersion();
          }
        }
        else
        {
          if (!_initialized)
          {
            initialize();
          }
        }
      }
      _uGPS.process();
    }
    else
    {
      if (_initialized)
      {
        end();
      }
    }
  }

  // set the GPS sync interval
  void updateGPSSyncInterval()
  {
    _gpsSyncIntervalActive = SettingsCache::gpsSyncInterval;
  }

private:
  unsigned long _gpsMessageInterval;
  unsigned long _gpsSyncIntervalActive;
  unsigned long _gpsSyncTimestamp;
  unsigned long _resetTimestamp;
  uint8_t _pinRX;
  uint8_t _pinTX;
  ubGPSTime _uGPS;
  notifyCallBack _notify;
  bool _initialized;

  // return the serial comm speed
  int getSpeed(gps_speed::gps_speed speed) const
  {
    int value;

    switch (speed)
    {
    case gps_speed::br_2400:
      value = 2400;
      break;

    case gps_speed::br_4800:
      value = 4800;
      break;

    case gps_speed::br_9600:
      value = 9600;
      break;

    case gps_speed::br_19200:
      value = 19200;
      break;

    case gps_speed::br_38400:
      value = 38400;
      break;

    case gps_speed::br_57600:
      value = 57600;
      break;

    case gps_speed::br_115200:
      value = 115200;
      break;

    default:
      value = 9600;
      break;
    }
    return value;
  }

  // callback functions, called if a GPS message is received
  void onGPSMessage(const UBXMESSAGE *message)
  {
    if (message->msgClass == UBX_NAV)
    {
      switch (message->msgID)
      {
      case UBX_NAV_TIMEUTC:
        gpsTimeSync(_uGPS.getTimeUTC());
        break;
      }
    }
  }

  // sync the time with the GPS time
  void gpsTimeSync(TIMEUTC timeUTC)
  {
    TimeElements tm;
    if (millis() - _gpsSyncTimestamp > (_gpsSyncIntervalActive))
    {
      time_t utc = now();
      breakTime(utc, tm);
      if (timeUTC.timeOfWeekValid)
      {

        tm.Second = timeUTC.second;
        tm.Minute = timeUTC.minute;
        tm.Hour = timeUTC.hour;
      }
      if (timeUTC.weekNumberValid)
      {
        tm.Day = timeUTC.day;
        tm.Month = timeUTC.month;
        tm.Year = timeUTC.year - 1970;
      }
      if (timeUTC.timeOfWeekValid || timeUTC.weekNumberValid)
      {
        utc = makeTime(tm);
        setTime(utc);
        if (_notify)
        {
          _notify(utc);
        }
      }
      if (timeUTC.timeOfWeekValid && timeUTC.weekNumberValid)
      {
        _gpsSyncIntervalActive = SettingsCache::gpsSyncInterval;
      }
      else
      {
        _gpsSyncIntervalActive = GPS_SYNC_INTERVAL_SHORT;
      }
      // adjust because the GPS module only sends 1 message/min
      _gpsSyncTimestamp = millis() - 5000;
    }
  }
};
