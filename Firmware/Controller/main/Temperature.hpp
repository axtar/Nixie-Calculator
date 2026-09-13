// Temperature.hpp

// provides temperature mode functionality

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <TempSensor.hpp>
#include <SettingsCache.hpp>
#include <Helper.hpp>

constexpr auto TEMP_UNDEFINED = (-255.0);

class Temperature
{
public:
  Temperature(uint8_t pinTemp) : _sensor(pinTemp)
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
  Settings *_settings;
  TempSensor _sensor;
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
        _sensor.requestTemperature();
        _temperatureCheckTimestamp = currentMillis;
        _requestPending = true;
      }
    }
    else
    {
      if (currentMillis - _temperatureCheckTimestamp > _temperatureConversionDelay)
      {
        float tempC;
        if (_sensor.getTempC(tempC))
        {
          tempC += SettingsCache::extTempCorr / 10.0f;
          _currentTemperature = SettingsCache::temperatureCF == temperature_cf::celsius ? tempC : Helper::celsiusToFahrenheit(tempC);
        }
        else
        {
          _currentTemperature = TEMP_UNDEFINED;
        }
        _requestPending = false;
      }
    }
  }
};
