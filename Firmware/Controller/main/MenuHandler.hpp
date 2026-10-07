// MenuHandler.hpp

// menu logic and navigation

// Copyright (C) 2020-2026 highvoltglow
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
    _inputBuffer.clear();
    _digitCount = 0;
    _lastMillis = millis();
    _displayBlink = true;
  }

  virtual ~MenuHandler()
  {
  }

  // initialize the menu mode
  void begin(uint8_t digitCount)
  {
    _it = _settingsMap.begin();
    _digitCount = digitCount;
    _it->second->setTempValue(_it->second->get());
    _inputBuffer.clear();
    formatDisplay(_it->second);
    _rgbPart = rgb_part::red;
    _timePart = time_part::hours;
    _dayPart = 0;
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
    if (SettingsCache::inputBlinking == input_blinking::off)
    {
      _displayBlink = false;
    }
    formatDisplay(_it->second, _displayBlink);
    return (update);
  }

#if RPN_MODE

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
        _inputBuffer.clear();
        setPrevValue();
        break;

      case KEY_PLUS:
        _inputBuffer.clear();
        setNextValue();
        break;

      case KEY_ENTER:
        commitValue();
        break;

      case KEY_BACK:
        if (_inputBuffer.length() > 0)
        {
          _inputBuffer.remove(_inputBuffer.length() - 1);
          if (_inputBuffer == "-")
          {
            _inputBuffer.clear();
          }
          formatDisplay(_it->second);
        }
        else
        {
          revertValue();
        }
        break;

      case KEY_CLS:
        resetValue();
        break;

      case KEY_0:
        digitInput(0);
        break;

      case KEY_1:
        digitInput(1);
        break;

      case KEY_2:
        digitInput(2);
        break;

      case KEY_3:
        digitInput(3);
        break;

      case KEY_4:
        digitInput(4);
        break;

      case KEY_5:
        digitInput(5);
        break;

      case KEY_6:
        digitInput(6);
        break;

      case KEY_7:
        digitInput(7);
        break;

      case KEY_8:
        digitInput(8);
        break;

      case KEY_9:
        digitInput(9);
        break;

      case KEY_00:
        digitInput(0);
        digitInput(0);
        break;

      case KEY_CHS:
        toggleInputSign();
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
        _inputBuffer.clear();
        setPrevValue();
        break;

      case KEY_PLUS:
        _inputBuffer.clear();
        setNextValue();
        break;

      case KEY_EQUALS:
        commitValue();
        break;

      case KEY_C:
        if (_inputBuffer.length() > 0)
        {
          _inputBuffer.remove(_inputBuffer.length() - 1);
          if (_inputBuffer == "-")
          {
            _inputBuffer.clear();
          }
          formatDisplay(_it->second);
        }
        else
        {
          revertValue();
        }
        break;

      case KEY_AC:
        resetValue();
        break;

      case KEY_0:
        digitInput(0);
        break;

      case KEY_1:
        digitInput(1);
        break;

      case KEY_2:
        digitInput(2);
        break;

      case KEY_3:
        digitInput(3);
        break;

      case KEY_4:
        digitInput(4);
        break;

      case KEY_5:
        digitInput(5);
        break;

      case KEY_6:
        digitInput(6);
        break;

      case KEY_7:
        digitInput(7);
        break;

      case KEY_8:
        digitInput(8);
        break;

      case KEY_9:
        digitInput(9);
        break;

      case KEY_00:
        digitInput(0);
        digitInput(0);
        break;

      case KEY_CHS:
        toggleInputSign();
        break;
      }
    }
  }
#endif

  // set a setting to its default value
  void resetValue()
  {
    _inputBuffer.clear();
    _it->second->reset();
    revertValue();
  }

  // revert to previously stored value
  void revertValue()
  {
    _inputBuffer.clear();
    _it->second->setTempValue(_it->second->get());
    formatDisplay(_it->second);
  }

private:
  String _display;
  String _inputBuffer; // digits typed so far for direct numeric entry, empty when not typing
  Settings *_settings;
  decimal_separator_position _dsp;
  const SETTINGSMAP &_settingsMap;
  SETTINGSMAP::const_iterator _it;
  uint8_t _digitCount;
  rgb_part _rgbPart;
  time_part _timePart;
  uint8_t _dayPart;
  uint8_t _red;
  uint8_t _green;
  uint8_t _blue;
  unsigned long _lastMillis;
  bool _displayBlink;

  // append a typed digit to the input buffer
  void digitInput(uint8_t digit)
  {
    setting_type type = _it->second->getSettingType();
    if (type == setting_type::dayofweek)
    {
      return;
    }

    uint8_t maxLen;
    int lower, upper;
    if (type == setting_type::numeric)
    {
      lower = _it->second->getMin();
      upper = _it->second->getMax();
      // just enough digits for this setting's range, sign excluded (handled separately)
      maxLen = String(max(abs(lower), abs(upper))).length();
    }
    else if (type == setting_type::time)
    {
      maxLen = 2;
      lower = 0;
      upper = (_timePart == time_part::hours) ? 23 : 59;
    }
    else
    {
      maxLen = 3;
      lower = 0;
      upper = 255;
    }

    bool negative = _inputBuffer.startsWith("-");
    String digits = negative ? _inputBuffer.substring(1) : _inputBuffer;
    String candidate;
    if (digits.isEmpty() || digits.equals("0"))
    {
      candidate = static_cast<char>(digit + '0');
    }
    else if (digits.length() < maxLen)
    {
      candidate = digits + static_cast<char>(digit + '0');
    }
    else
    {
      return;
    }
    int candidateValue = candidate.toInt() * (negative ? -1 : 1);
    if (negative ? (candidateValue < lower) : (candidateValue > upper))
    {
      return;
    }

    _inputBuffer = (negative ? "-" : "") + candidate;
    formatDisplay(_it->second);
  }

  // toggle a leading minus sign on the input buffer
  void toggleInputSign()
  {
    if (_it->second->getSettingType() != setting_type::numeric)
    {
      return;
    }
    bool becomesNegative = !_inputBuffer.startsWith("-");
    String flipped = becomesNegative ? ("-" + _inputBuffer) : _inputBuffer.substring(1);
    int flippedValue = flipped.toInt();
    if (becomesNegative ? (flippedValue < _it->second->getMin()) : (flippedValue > _it->second->getMax()))
    {
      return;
    }
    _inputBuffer = flipped;
    formatDisplay(_it->second);
  }

  // parse the typed input buffer
  void applyInputBuffer()
  {
    int typed = _inputBuffer.toInt();
    uint8_t hours, minutes, red, green, blue;

    switch (_it->second->getSettingType())
    {
    case setting_type::numeric:
      typed = constrain(typed, _it->second->getMin(), _it->second->getMax());
      _it->second->setTempValue(typed);
      break;

    case setting_type::time:
      Helper::intToTime(_it->second->getTempValue(), &hours, &minutes);
      typed = constrain(typed, 0, (_timePart == time_part::hours) ? 23 : 59);
      if (_timePart == time_part::hours)
      {
        hours = typed;
      }
      else
      {
        minutes = typed;
      }
      _it->second->setTempValue(Helper::timeToInt(hours, minutes));
      break;

    case setting_type::rgb:
      Helper::intToRGB(_it->second->getTempValue(), &red, &green, &blue);
      typed = constrain(typed, 0, 255);
      switch (_rgbPart)
      {
      case rgb_part::red:
        red = typed;
        break;

      case rgb_part::green:
        green = typed;
        break;

      case rgb_part::blue:
        blue = typed;
        break;
      }
      _it->second->setTempValue(Helper::rgbToInt(red, green, blue));
      break;

    case setting_type::dayofweek:
      break;
    }
  }

  // format the current setting as a string to be displayed
  void formatDisplay(const Setting *setting, bool blink = false)
  {
    char buffer[2 * _digitCount];
    uint8_t hours;
    uint8_t minutes;
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;

    bool typing = (_inputBuffer.length() > 0);
    int typedValue = typing ? _inputBuffer.toInt() : 0;

    switch (setting->getSettingType())
    {
    case setting_type::numeric:
    {
      int value = typing ? typedValue : setting->getTempValue();
      if (value < 0)
      {
        sprintf(buffer, "-%02d%*s%3d", setting->getId(), _digitCount - 5, " ", abs(value));
        _display = "-";
      }
      else
      {
        sprintf(buffer, "%02d%*s%3d", setting->getId(), _digitCount - 5, " ", value);
        _display.clear();
      }
      _display += buffer;
      break;
    }

    case setting_type::time:
      Helper::intToTime(setting->getTempValue(), &hours, &minutes);
      if (typing)
      {
        if (_timePart == time_part::hours)
        {
          hours = typedValue;
        }
        else
        {
          minutes = typedValue;
        }
      }
      switch (_timePart)
      {
      case time_part::hours:
        sprintf(buffer, _dsp == decimal_separator_position::right ? "%02d%*s%02d. %02d" : "%02d%*s.%02d %02d", setting->getId(), _digitCount - 7, " ", hours, minutes);
        break;

      case time_part::minutes:
        sprintf(buffer, _dsp == decimal_separator_position::right ? "%02d%*s%02d %02d." : "%02d%*s%02d .%02d", setting->getId(), _digitCount - 7, " ", hours, minutes);
        break;
      }
      _display = buffer;
      break;

    case setting_type::rgb:
      Helper::intToRGB(setting->getTempValue(), &red, &green, &blue);
      if (typing)
      {
        switch (_rgbPart)
        {
        case rgb_part::red:
          red = typedValue;
          break;

        case rgb_part::green:
          green = typedValue;
          break;

        case rgb_part::blue:
          blue = typedValue;
          break;
        }
      }
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
      _display = buffer;
      break;

    case setting_type::dayofweek:
    {
      // one digit per day (0=Sunday..6=Saturday); selected days blink, "." marks the +/- cursor
      char days[9];
      uint8_t pos = 0;
      for (uint8_t d = 0; d < 7; d++)
      {
        bool selected = Helper::isDaySelected(setting->getTempValue(), d);
        days[pos++] = (blink && selected) ? ' ' : static_cast<char>('0' + d);
        if (d == _dayPart)
        {
          days[pos++] = '.';
        }
      }
      days[pos] = '\0';
      sprintf(buffer, "%02d%*s%s", setting->getId(), _digitCount - 2 - pos, " ", days);
      _display = buffer;
      break;
    }
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
    _inputBuffer.clear();
    _rgbPart = rgb_part::red;
    _timePart = time_part::hours;
    _dayPart = 0;
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
    _inputBuffer.clear();
    _rgbPart = rgb_part::red;
    _timePart = time_part::hours;
    _dayPart = 0;
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

    case setting_type::dayofweek:
      _it->second->setTempValue(Helper::toggleDay(_it->second->getTempValue(), _dayPart));
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

    case setting_type::dayofweek:
      _it->second->setTempValue(Helper::toggleDay(_it->second->getTempValue(), _dayPart));
      formatDisplay(_it->second);
      break;
    }
  }

  // temporarily store setting value
  void commitValue()
  {
    if ((_inputBuffer.length() > 0) && (_inputBuffer != "-"))
    {
      applyInputBuffer();
    }
    _inputBuffer.clear();

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
      break;

    case setting_type::dayofweek:
      _it->second->set(_it->second->getTempValue());
      _dayPart = (_dayPart + 1) % 7;
      break;
    }
    formatDisplay(_it->second);
  }
};