// Temperature.hpp

// provides temperature mode functionality

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <DS18B20.h>
#include <SettingsCache.hpp>

constexpr auto TEMP_UNDEFINED = (-255.0);

class Temperature
{
public:
  Temperature(uint8_t pinTemp) : _oneWire(pinTemp),
                                 _sensor(&_oneWire)

  {
    _currentTemperature = TEMP_UNDEFINED;
    _temperatureCheckInterval = 5000;   // 5 seconds
    _temperatureConversionDelay = 1000; // 1 second
    _temperatureCheckTimestamp = millis() + _temperatureCheckInterval;
    _requestPending = false;
    _initialized = false;
  }

  virtual ~Temperature()
  {
  }

  // initialize the temperature sensor
  void begin()
  {
    _sensor.begin();
    _sensor.setResolution(12);
    _initialized = true;
  }

  // stop
  void end()
  {
    _initialized = false;
    _requestPending = false;
    _currentTemperature = TEMP_UNDEFINED;
  }

  // return the current temperature
  float getTemperature() const
  {
    return (_currentTemperature);
  }

  // request the temperature
  void process()
  {
    if (SettingsCache::temperatureMode == temperature_mode::on)
    {
      if (!isInitialized())
      {
        begin();
      }
      checkTemperature();
    }
    else
    {
      if (isInitialized())
      {
        end();
      }
      _currentTemperature = TEMP_UNDEFINED;
    }
  }

  // return if the temperature sensor has been initialized
  bool isInitialized() const
  {
    return (_initialized);
  }

private:
  float _currentTemperature;
  OneWire _oneWire;
  Settings *_settings;
  DS18B20 _sensor;
  unsigned long _temperatureCheckTimestamp;
  unsigned long _temperatureCheckInterval;
  unsigned long _temperatureConversionDelay;
  bool _requestPending;
  bool _initialized;

  // request the temperature from the sensor
  void checkTemperature()
  {
    unsigned long currentMillis = millis();
    if (!_requestPending)
    {
      if (currentMillis - _temperatureCheckTimestamp > _temperatureCheckInterval)
      {
        _sensor.requestTemperatures();
        _temperatureCheckTimestamp = currentMillis;
        _requestPending = true;
      }
    }
    else
    {
      if (currentMillis - _temperatureCheckTimestamp > _temperatureConversionDelay)
      {
        if (SettingsCache::temperatureCF == temperature_cf::celsius)
        {
          _currentTemperature = _sensor.getTempC();
          if (_currentTemperature == DEVICE_DISCONNECTED)
          {
            _currentTemperature = TEMP_UNDEFINED;
          }
        }
        else
        {
          _currentTemperature = _sensor.getTempF();
          if (_currentTemperature == DEVICE_DISCONNECTED)
          {
            _currentTemperature = TEMP_UNDEFINED;
          }
        }
        _requestPending = false;
      }
    }
  }
};
