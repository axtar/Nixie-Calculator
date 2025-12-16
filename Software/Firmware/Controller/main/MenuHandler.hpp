// MenuHandler.hpp

// menu logic and navigation

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <map>
#include <Settings.hpp>
#include <KeyboardHandler.hpp>
#include <KeyboardDecoder.hpp>
#include <DisplayHandler.hpp>
#include <Helper.hpp>

enum class rgb_part
{
  red,
  green,
  blue
};

enum class time_part
{
  hours,
  minutes
};

class MenuHandler
{
public:
  MenuHandler(Settings *settings, decimal_separator_position dsp) : _settings(settings),
                                                                    _dsp(dsp),
                                                                    _settingsMap(_settings->getSettingsMap())
  {
    _display.clear();
    _digitCount = 0;
    _lastMillis = millis();
    _displayBlink = true;
  }

  virtual ~MenuHandler()
  {
  }

  // initialize the menue mode
  void begin(uint8_t digitCount)
  {
    _it = _settingsMap.begin();
    _digitCount = digitCount;
    _it->second->setTempValue(_it->second->get());
    formatDisplay(_it->second);
    _rgbPart = rgb_part::red;
    _timePart = time_part::hours;
  }

  // return the red value of the current setting
  uint8_t getRed() const
  {
    return (_red);
  }

  // return the green value of the current setting
  uint8_t getGreen() const
  {
    return (_green);
  }

  // return the blue value of the current setting
  uint8_t getBlue() const
  {
    return (_blue);
  }

  // return the current displayed values as a string
  String getDisplay() const
  {
    return (_display);
  }

  // used for blinking of changed setting value
  bool updateDisplayNeeded()
  {
    bool update = true;

    if (millis() - _lastMillis > 250)
    {
      _displayBlink = !_displayBlink;
      _lastMillis = millis();
      update = true;
    }
    else
    {
      update = false;
    }
    formatDisplay(_it->second, _displayBlink);
    return (update);
  }

#if CALC_TYPE == CALC_TYPE_RPN

  // handle keyboard events
  void onKeyboardEvent(uint8_t keyCode, key_state keyState, bool functionKeyPressed)
  {
    if ((keyState == key_state::pressed) || (keyState == key_state::autorepeat))
    {
      switch (keyCode)
      {
      case KEY_STO:
        setNextSetting();
        break;

      case KEY_RCL:
        setPrevSetting();
        break;

      case KEY_MINUS:
        setPrevValue();
        break;

      case KEY_PLUS:
        setNextValue();
        break;

      case KEY_ENTER:
        commitValue();
        break;

      case KEY_BACK:
        revertValue();
        break;

      case KEY_CLS:
        resetValue();
        break;
      }
    }
  }

#else

  // handle keyboard events
  void onKeyboardEvent(uint8_t keyCode, key_state keyState, bool functionKeyPressed)
  {
    if ((keyState == key_state::pressed) || (keyState == key_state::autorepeat))
    {
      switch (keyCode)
      {
      case KEY_MPLUS:
        setNextSetting();
        break;

      case KEY_MMINUS:
        setPrevSetting();
        break;

      case KEY_MINUS:
        setPrevValue();
        break;

      case KEY_PLUS:
        setNextValue();
        break;

      case KEY_EQUALS:
        commitValue();
        break;

      case KEY_C:
        revertValue();
        break;

      case KEY_AC:
        resetValue();
        break;
      }
    }
  }
#endif

  // set a setting to his default value
  void resetValue()
  {
    _it->second->reset();
    revertValue();
  }

  // revert to previously stored value
  void revertValue()
  {
    _it->second->setTempValue(_it->second->get());
    formatDisplay(_it->second);
  }

private:
  String _display;
  Settings *_settings;
  decimal_separator_position _dsp;
  const SETTINGSMAP &_settingsMap;
  SETTINGSMAP::const_iterator _it;
  uint8_t _digitCount;
  rgb_part _rgbPart;
  time_part _timePart;
  uint8_t _red;
  uint8_t _green;
  uint8_t _blue;
  unsigned long _lastMillis;
  bool _displayBlink;

  // format the current setting as a string to be displayed
  void formatDisplay(const Setting *setting, bool blink = false)
  {
    char buffer[2 * _digitCount];
    uint8_t hours;
    uint8_t minutes;
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;

    switch (setting->getSettingType())
    {
    case setting_type::numeric:
      if (blink && (setting->get() != setting->getTempValue()))
      {
        if (setting->getTempValue() < 0)
        {
          sprintf(buffer, "-%02d%*s", setting->getId(), _digitCount - 2, " ");
        }
        else
        {
          sprintf(buffer, "%02d%*s", setting->getId(), _digitCount - 2, " ");
        }
        if (setting->getTempValue() < 0)
        {
          _display = "-";
        }
        else
        {
          _display.clear();
        }
      }
      else
      {
        if (setting->getTempValue() < 0)
        {
          sprintf(buffer, "-%02d%*s%3d", setting->getId(), _digitCount - 5, " ", abs(setting->getTempValue()));
        }
        else
        {

          sprintf(buffer, "%02d%*s%3d", setting->getId(), _digitCount - 5, " ", setting->getTempValue());
        }
        if (setting->getTempValue() < 0)
        {
          _display = "-";
        }
        else
        {
          _display.clear();
        }
      }
      _display += buffer;
      break;

    case setting_type::time:
      Helper::intToTime(setting->getTempValue(), &hours, &minutes);
      if (blink && (setting->get() != setting->getTempValue()))
      {
        switch (_timePart)
        {
        case time_part::hours:
          sprintf(buffer, "%02d%*s %02d", setting->getId(), _digitCount - 5, " ", minutes);
          break;

        case time_part::minutes:
          sprintf(buffer, "%02d%*s%02d %2s", setting->getId(), _digitCount - 7, " ", hours, " ");
          break;
        }
      }
      else
      {
        switch (_timePart)
        {
        case time_part::hours:
          sprintf(buffer, _dsp == decimal_separator_position::right ? "%02d%*s%02d. %02d" : "%02d%*s.%02d %02d", setting->getId(), _digitCount - 7, " ", hours, minutes);
          break;

        case time_part::minutes:
          sprintf(buffer, _dsp == decimal_separator_position::right ? "%02d%*s%02d %02d." : "%02d%*s%02d .%02d", setting->getId(), _digitCount - 7, " ", hours, minutes);
          break;
        }
      }
      _display = buffer;
      break;

    case setting_type::rgb:
      Helper::intToRGB(setting->getTempValue(), &red, &green, &blue);
      if (blink && (setting->get() != setting->getTempValue()))
      {
        switch (_rgbPart)
        {
        case rgb_part::red:
          sprintf(buffer, "%02d %3s %03d %03d", setting->getId(), " ", green, blue);
          break;

        case rgb_part::green:
          sprintf(buffer, "%02d %03d %3s %03d", setting->getId(), red, " ", blue);
          break;

        case rgb_part::blue:
          sprintf(buffer, "%02d %03d %03d %3s", setting->getId(), red, green, " ");
          break;
        }
      }
      else
      {
        switch (_rgbPart)
        {
        case rgb_part::red:
          sprintf(buffer, _dsp == decimal_separator_position::right ? "%02d %03d. %03d %03d" : "%02d .%03d %03d %03d", setting->getId(), red, green, blue);
          break;

        case rgb_part::green:
          sprintf(buffer, _dsp == decimal_separator_position::right ? "%02d %03d %03d. %03d" : "%02d %03d .%03d %03d", setting->getId(), red, green, blue);
          break;

        case rgb_part::blue:
          sprintf(buffer, _dsp == decimal_separator_position::right ? "%02d %03d %03d %03d." : "%02d %03d %03d .%03d", setting->getId(), red, green, blue);
          break;
        }
      }
      _display = buffer;
      break;
    }
    _red = red;
    _green = green;
    _blue = blue;
  }

  // move to next non-hidden setting
  void setNextSetting()
  {
    do
    {
      if (_it != _settingsMap.end())
      {
        _it++;
      }
      if (_it == _settingsMap.end())
      {
        _it = _settingsMap.begin();
      }
    } while (_it->second->isHidden());

    _it->second->setTempValue(_it->second->get());
    _rgbPart = rgb_part::red;
    _timePart = time_part::hours;
    formatDisplay(_it->second);
  }

  // move to previous non-hidden setting
  void setPrevSetting()
  {
    do
    {
      if (_it != _settingsMap.begin())
      {
        _it--;
      }
      else
      {
        _it = _settingsMap.end();
        _it--;
      }
    } while (_it->second->isHidden());

    _it->second->setTempValue(_it->second->get());
    _rgbPart = rgb_part::red;
    _timePart = time_part::hours;
    formatDisplay(_it->second);
  }

  // change setting to the next lower value
  void setPrevValue()
  {
    uint8_t hours;
    uint8_t minutes;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    switch (_it->second->getSettingType())
    {
    case setting_type::numeric:
      if (_it->second->getTempValue() > _it->second->getMin())
      {
        _it->second->setTempValue(_it->second->getTempValue() - 1);
      }
      else
      {
        _it->second->setTempValue(_it->second->getMax());
      }
      formatDisplay(_it->second);
      break;

    case setting_type::time:
      Helper::intToTime(_it->second->getTempValue(), &hours, &minutes);
      switch (_timePart)
      {
      case time_part::hours:
        if (hours > 0)
        {
          hours--;
        }
        else
        {
          hours = 23;
        }
        break;

      case time_part::minutes:
        if (minutes > 0)
        {
          minutes--;
        }
        else
        {
          minutes = 59;
        }
        break;
      }
      _it->second->setTempValue(Helper::timeToInt(hours, minutes));
      formatDisplay(_it->second);
      break;

    case setting_type::rgb:
      Helper::intToRGB(_it->second->getTempValue(), &red, &green, &blue);
      switch (_rgbPart)
      {
      case rgb_part::red:
        if (red > 0)
        {
          red--;
        }
        else
        {
          red = 255;
        }
        break;

      case rgb_part::green:
        if (green > 0)
        {
          green--;
        }
        else
        {
          green = 255;
        }
        break;

      case rgb_part::blue:
        if (blue > 0)
        {
          blue--;
        }
        else
        {
          blue = 255;
        }
        break;
      }
      _it->second->setTempValue(Helper::rgbToInt(red, green, blue));
      formatDisplay(_it->second);
      break;
    }
  }

  // change setting to the next larger value
  void setNextValue()
  {
    uint8_t hours;
    uint8_t minutes;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    switch (_it->second->getSettingType())
    {
    case setting_type::numeric:
      if (_it->second->getTempValue() < _it->second->getMax())
      {
        _it->second->setTempValue(_it->second->getTempValue() + 1);
      }
      else
      {
        _it->second->setTempValue(_it->second->getMin());
      }
      formatDisplay(_it->second);
      break;

    case setting_type::time:
      Helper::intToTime(_it->second->getTempValue(), &hours, &minutes);
      switch (_timePart)
      {
      case time_part::hours:
        if (hours < 23)
        {
          hours++;
        }
        else
        {
          hours = 0;
        }
        break;

      case time_part::minutes:
        if (minutes < 59)
        {
          minutes++;
        }
        else
        {
          minutes = 0;
        }
        break;
      }
      _it->second->setTempValue(Helper::timeToInt(hours, minutes));
      formatDisplay(_it->second);
      break;

    case setting_type::rgb:
      Helper::intToRGB(_it->second->getTempValue(), &red, &green, &blue);
      switch (_rgbPart)
      {
      case rgb_part::red:
        if (red < 255)
        {
          red++;
        }
        else
        {
          red = 0;
        }
        break;

      case rgb_part::green:
        if (green < 255)
        {
          green++;
        }
        else
        {
          green = 0;
        }
        break;

      case rgb_part::blue:
        if (blue < 255)
        {
          blue++;
        }
        else
        {
          blue = 0;
        }
        break;
      }
      _it->second->setTempValue(Helper::rgbToInt(red, green, blue));
      formatDisplay(_it->second);
      break;
    }
  }

  // temporarily store setting value
  void commitValue()
  {
    switch (_it->second->getSettingType())
    {
    case setting_type::numeric:
      _it->second->set(_it->second->getTempValue());
      break;

    case setting_type::time:
      _it->second->set(_it->second->getTempValue());
      if (_timePart == time_part::hours)
      {
        _timePart = time_part::minutes;
      }
      else
      {
        _timePart = time_part::hours;
      }
      break;

    case setting_type::rgb:
      _it->second->set(_it->second->getTempValue());
      switch (_rgbPart)
      {
      case rgb_part::red:
        _rgbPart = rgb_part::green;
        break;

      case rgb_part::green:
        _rgbPart = rgb_part::blue;
        break;

      case rgb_part::blue:
        _rgbPart = rgb_part::red;
        break;
      }
    }
    formatDisplay(_it->second);
  }
};