// PIR.hpp

// provides PIR functionality

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <FunctionalInterrupt.h>
#include <Settings.hpp>

class PIR
{

public:
  PIR(uint8_t pinPIR)
      : _pinPIR(pinPIR)
  {
    _pirTimestamp = 0;
    _initialized = false;
    _presenceDetected = true;
  }

  virtual ~PIR()
  {
  }

  // initialize PIR
  void begin()
  {
    attachInterrupt(_pinPIR, std::bind(&PIR::handlePIRTimeout, this), HIGH);
    _initialized = true;
    _pirTimestamp = millis();
  }

  // stop PIR
  void end()
  {
    detachInterrupt(_pinPIR);
    _initialized = false;
  }

  // if a key is pressed, we assume that someone is there
  void onKeyPressed()
  {
    _pirTimestamp = millis();
  }

  // interrupt service routine, called if presence was detected by the PIR
  void handlePIRTimeout()
  {
    _pirTimestamp = millis();
  }

  // return the PIR status
  bool getPresenceDetected()
  {
    return (_presenceDetected);
  }

  // check if the PIR has timed out
  void process()
  {
    if (SettingsCache::pirMode == pir_mode::on)
    {
      if (!isInitialized())
      {
        begin();
      }
      if (millis() - _pirTimestamp > SettingsCache::pirDelay)
      {
        _presenceDetected = false;
      }
      else
      {
        _presenceDetected = true;
      }
    }
    else
    {
      if (isInitialized())
      {
        end();
      }
    }
  }

  // return true if the PIR has been initialized
  bool isInitialized() const
  {
    return (_initialized);
  }

private:
  uint8_t _pinPIR;
  volatile unsigned long _pirTimestamp;
  bool _initialized;
  volatile bool _presenceDetected;
};
