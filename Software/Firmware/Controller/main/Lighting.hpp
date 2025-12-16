// Lighting.hpp

// provides LED backlight/underlight support

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <GlobalEnums.h>
#include <Config.h>
#include <SettingsCache.hpp>
#include <DisplayHandler.hpp>
#include <Helper.hpp>

class Lighting
{

public:
  Lighting(DisplayHandler *displayHandler)
      : _displayHandler(displayHandler)
  {
    _on = false;
    _refresh = true;
    _lastCycleTimestamp = 0;
    _cycleInterval = 10; // in ms
    _forcedByShortcut = false;
  }

  virtual ~Lighting()
  {
  }

  // nothing to initialize,
  void begin()
  {
  }

  // return if lighting is on or off
  bool isOn() const
  {
    return (_on);
  }

  // set the lighting according to the device mode
  void process(TimeElements *tm, device_mode deviceMode)
  {
    // check if lighting should be on or off
    if (isLighting(tm))
    {
      switch (deviceMode)
      {
      case device_mode::calculator:
        setCalcLighting();
        break;

      case device_mode::clock:
        setClockLighting();
        break;

      case device_mode::menu:
        // do nothing
        break;

      case device_mode::antipoisoning:
        // set lighting off
        off();
        break;
      }
    }
    else
    {
      off();
    }
  }

  // set the refresh flag
  void refresh()
  {
    // refresh needed
    _refresh = true;
  }

  // turn lighting off
  void off()
  {
    if (_on)
    {
      _displayHandler->clearLEDs();
      _on = false;
    }
  }

  // turn lighting on
  void on()
  {
    if (!_on)
    {
      _refresh = true;
    }
    _on = true;
  }

  // handle shortcut, rotate through lighting modes
  void switchLightingMode(device_mode deviceMode)
  {
    switch (deviceMode)
    {
    case device_mode::calculator:
      switch (SettingsCache::calcRGBMode)
      {
      case calc_rgb_mode::off:
        SettingsCache::calcRGBMode = calc_rgb_mode::ifnixieon;
        break;

      case calc_rgb_mode::ifnixieon:
        SettingsCache::calcRGBMode = calc_rgb_mode::all;
        break;

      case calc_rgb_mode::all:
        SettingsCache::calcRGBMode = calc_rgb_mode::ifnixieonrandom;
        break;

      case calc_rgb_mode::ifnixieonrandom:
        SettingsCache::calcRGBMode = calc_rgb_mode::ifnixieonfullrandom;
        break;

      case calc_rgb_mode::ifnixieonfullrandom:
        SettingsCache::calcRGBMode = calc_rgb_mode::fixed;
        break;

      case calc_rgb_mode::fixed:
        SettingsCache::calcRGBMode = calc_rgb_mode::random;
        break;

      case calc_rgb_mode::random:
        SettingsCache::calcRGBMode = calc_rgb_mode::fullrandom;
        break;

      case calc_rgb_mode::fullrandom:
        SettingsCache::calcRGBMode = calc_rgb_mode::off;
        break;
      }
      break;

    case device_mode::clock:
      switch (SettingsCache::clockRGBMode)
      {
      case clock_rgb_mode::off:
        SettingsCache::clockRGBMode = clock_rgb_mode::split;
        break;

      case clock_rgb_mode::split:
        SettingsCache::clockRGBMode = clock_rgb_mode::splitrandom;
        break;

      case clock_rgb_mode::splitrandom:
        SettingsCache::clockRGBMode = clock_rgb_mode::splitfullrandom;
        break;

      case clock_rgb_mode::splitfullrandom:
        SettingsCache::clockRGBMode = clock_rgb_mode::fixed;
        break;

      case clock_rgb_mode::fixed:
        SettingsCache::clockRGBMode = clock_rgb_mode::random;
        break;

      case clock_rgb_mode::random:
        SettingsCache::clockRGBMode = clock_rgb_mode::fullrandom;
        break;

      case clock_rgb_mode::fullrandom:
        SettingsCache::clockRGBMode = clock_rgb_mode::off;
        break;
      }
      break;

    case device_mode::antipoisoning:
    case device_mode::menu:
      // do nothing
      break;
    }
    forcedOn();
  }

  // stop ignoring lighting time constraints
  void forcedOff()
  {
    _forcedByShortcut = false;
  }

private:
  DisplayHandler *_displayHandler;
  bool _on;
  bool _refresh;
  unsigned long _lastCycleTimestamp;
  uint _cycleInterval;
  bool _forcedByShortcut;

  // force to ignore lighting time constraints
  void forcedOn()
  {
    _forcedByShortcut = true;
  }

  // check if lighting is required
  bool isLighting(TimeElements *tm) const
  {
    return ((SettingsCache::ledMode == led_mode::led_mode::always) ||
            (Helper::isInTimeRange(SettingsCache::ledStartTime.hour, SettingsCache::ledStartTime.minute,
                                   tm->Hour, tm->Minute, SettingsCache::ledDuration)) ||
            (Helper::isInTimeRange(SettingsCache::ledStartTime2.hour, SettingsCache::ledStartTime2.minute,
                                   tm->Hour, tm->Minute, SettingsCache::ledDuration2)) ||
            _forcedByShortcut);
  }

  // get color according to content
  void getCalcColorByContent(digit_content content, uint8_t *red, uint8_t *green, uint8_t *blue)
  {

    switch (content)
    {
    case digit_content::calc_plus:
      *red = SettingsCache::positiveColor.red;
      *green = SettingsCache::positiveColor.green;
      *blue = SettingsCache::positiveColor.blue;
      break;

    case digit_content::calc_minus:
      *red = SettingsCache::negativeColor.red;
      *green = SettingsCache::negativeColor.green;
      *blue = SettingsCache::negativeColor.blue;
      break;

    case digit_content::calc_error:
      *red = SettingsCache::errorColor.red;
      *green = SettingsCache::errorColor.green;
      *blue = SettingsCache::errorColor.blue;
      break;

    case digit_content::exp_minus:
      *red = SettingsCache::negExpColor.red;
      *green = SettingsCache::negExpColor.green;
      *blue = SettingsCache::negExpColor.blue;
      break;

    case digit_content::exp_plus:
      *red = SettingsCache::posExpColor.red;
      *green = SettingsCache::posExpColor.green;
      *blue = SettingsCache::posExpColor.blue;
      break;

    case digit_content::none:
      *red = 0;
      *green = 0;
      *blue = 0;
      break;

    default: // avoid warning
      break;
    }
  }

  // get color according to content
  void getClockColorByContent(digit_content content, uint8_t *red, uint8_t *green, uint8_t *blue)
  {

    switch (content)
    {
    case digit_content::date:
      *red = SettingsCache::dateColor.red;
      *green = SettingsCache::dateColor.green;
      *blue = SettingsCache::dateColor.blue;
      break;

    case digit_content::time:
      *red = SettingsCache::timeColor.red;
      *green = SettingsCache::timeColor.green;
      *blue = SettingsCache::timeColor.blue;
      break;

    case digit_content::temp:
      *red = SettingsCache::tempColor.red;
      *green = SettingsCache::tempColor.green;
      *blue = SettingsCache::tempColor.blue;
      break;

    case digit_content::none:
      *red = 0;
      *green = 0;
      *blue = 0;
      break;

    default: // avoid warning
      break;
    }
  }

  // get color according to content
  void getCalcColorAllByContent(digit_content content, uint8_t *red, uint8_t *green, uint8_t *blue)
  {

    switch (content)
    {
    case digit_content::calc_plus:
      *red = SettingsCache::positiveColor.red;
      *green = SettingsCache::positiveColor.green;
      *blue = SettingsCache::positiveColor.blue;
      break;

    case digit_content::calc_minus:
      *red = SettingsCache::negativeColor.red;
      *green = SettingsCache::negativeColor.green;
      *blue = SettingsCache::negativeColor.blue;
      break;

    case digit_content::calc_error:
      *red = SettingsCache::errorColor.red;
      *green = SettingsCache::errorColor.green;
      *blue = SettingsCache::errorColor.blue;
      break;

    default: // avoid warning
      break;
    }
  }

  // generate a random color
  void getRandomColor(uint8_t *red, uint8_t *green, uint8_t *blue)
  {
    *red = random(0, 256);
    *green = random(0, 256);
    *blue = random(0, 256);
  }

  // set the lighting for the calculator mode
  void setCalcLighting()
  {
    int offset = 0;
    uint8_t tempRed = 0, tempGreen = 0, tempBlue = 0;
    digit_content content, contentAll;

    if (_displayHandler->getDisplayType() == display_type::in12a ||
        _displayHandler->getDisplayType() == display_type::in12b)
    {
      offset = 1;
    }
    if (SettingsCache::calcRGBMode == calc_rgb_mode::off)
    {
      off();
    }
    else
    {
      on();
      if (_refresh)
      {
        // define a random color
        uint8_t red, green, blue, allRed = 0, allGreen = 0, allBlue = 0;
        getRandomColor(&red, &green, &blue);

        // check first digit to get content in all mode
        contentAll = _displayHandler->getDigitContent(0);
        if (contentAll == digit_content::none)
        {
          contentAll = digit_content::calc_error;
        }
        getCalcColorAllByContent(contentAll, &allRed, &allGreen, &allBlue);

        // loop over all digits
        for (int i = 0; i < _displayHandler->getDigitCount(); i++)
        {
          content = _displayHandler->getDigitContent(i);
          switch (SettingsCache::calcRGBMode)
          {
          case calc_rgb_mode::ifnixieon:
            getCalcColorByContent(content, &tempRed, &tempGreen, &tempBlue);
            _displayHandler->setLED(i + offset, tempRed, tempGreen, tempBlue);
            break;

          case calc_rgb_mode::all:
            _displayHandler->setLED(i + offset, allRed, allGreen, allBlue);
            break;

          case calc_rgb_mode::ifnixieonrandom:
            if (content != digit_content::none)
            {
              _displayHandler->setLED(i + offset, red, green, blue);
            }
            else
            {
              _displayHandler->setLED(i + offset, 0, 0, 0);
            }
            break;

          case calc_rgb_mode::ifnixieonfullrandom:
            if (content != digit_content::none)
            {
              getRandomColor(&tempRed, &tempGreen, &tempBlue);
              _displayHandler->setLED(i + offset, tempRed, tempGreen, tempBlue);
            }
            else
            {
              _displayHandler->setLED(i + offset, 0, 0, 0);
            }
            break;

          case calc_rgb_mode::fixed:
            _displayHandler->setLED(i + offset, SettingsCache::fixedCalcColor.red, SettingsCache::fixedCalcColor.green, SettingsCache::fixedCalcColor.blue);
            break;

          case calc_rgb_mode::random:
            _displayHandler->setLED(i + offset, red, green, blue);
            break;

          case calc_rgb_mode::fullrandom:
            getRandomColor(&tempRed, &tempGreen, &tempBlue);
            _displayHandler->setLED(i + offset, tempRed, tempGreen, tempBlue);
            break;

          default: // avoid warning
            break;
          }
        }
      }
    }
    _displayHandler->updateLEDs();
    _refresh = false;
  }

  // set the lighting for the calculator mode
  void setClockLighting()
  {
    int offset = 0;
    uint8_t tempRed = 0, tempGreen = 0, tempBlue = 0;
    digit_content content;
    if (_displayHandler->getDisplayType() == display_type::in12a ||
        _displayHandler->getDisplayType() == display_type::in12b)
    {
      offset = 1;
    }

    if (SettingsCache::clockRGBMode == clock_rgb_mode::off)
    {
      off();
    }
    else
    {
      on();
      if (_refresh)
      {
        // define a random color
        uint8_t red, green, blue;
        getRandomColor(&red, &green, &blue);

        // loop over all digits
        for (int i = 0; i < _displayHandler->getDigitCount(); i++)
        {
          content = _displayHandler->getDigitContent(i);
          switch (SettingsCache::clockRGBMode)
          {
          case clock_rgb_mode::split:
            getClockColorByContent(content, &tempRed, &tempGreen, &tempBlue);
            _displayHandler->setLED(i + offset, tempRed, tempGreen, tempBlue);
            if (content != digit_content::none)
            {
              _refresh = false;
            }
            break;

          case clock_rgb_mode::splitrandom:
            if (content == digit_content::none)
            {
              _displayHandler->setLED(i + offset, 0, 0, 0);
            }
            else
            {
              _displayHandler->setLED(i + offset, red, green, blue);
              _refresh = false;
            }
            break;

          case clock_rgb_mode::splitfullrandom:
            if (content == digit_content::none)
            {
              _displayHandler->setLED(i + offset, 0, 0, 0);
            }
            else
            {
              getRandomColor(&tempRed, &tempGreen, &tempBlue);
              _displayHandler->setLED(i + offset, tempRed, tempGreen, tempBlue);
              _refresh = false;
            }
            break;

          case clock_rgb_mode::fixed:
            _displayHandler->setLED(i + offset, SettingsCache::fixedColor.red, SettingsCache::fixedColor.green, SettingsCache::fixedColor.blue);
            _refresh = false;
            break;

          case clock_rgb_mode::random:
            _displayHandler->setLED(i + offset, red, green, blue);
            _refresh = false;
            break;

          case clock_rgb_mode::fullrandom:
            getRandomColor(&tempRed, &tempGreen, &tempBlue);
            _displayHandler->setLED(i + offset, tempRed, tempGreen, tempBlue);
            _refresh = false;
            break;

          default: // avoid warning
            break;
          }
        }
      }
    }
    _displayHandler->updateLEDs();
  }
};
