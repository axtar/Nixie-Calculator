// Lighting.hpp

// provides LED backlight/underlight support

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <time.h>
#include <math.h>
#include <DeviceModes.h>
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
    _cycleInterval = 30; // in ms, also used as the color wheel/breathing step interval
    _forcedByShortcut = false;
    _wheelHue = 0;
    _breathPhase = 0;
    _breathScale = 1.0f;
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
  void process(const struct tm *tm, device_mode deviceMode)
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
        SettingsCache::calcRGBMode = calc_rgb_mode::by_content;
        break;

      case calc_rgb_mode::by_content:
        SettingsCache::calcRGBMode = calc_rgb_mode::by_content_all;
        break;

      case calc_rgb_mode::by_content_all:
        SettingsCache::calcRGBMode = calc_rgb_mode::random_on_only;
        break;

      case calc_rgb_mode::random_on_only:
        SettingsCache::calcRGBMode = calc_rgb_mode::full_random_on_only;
        break;

      case calc_rgb_mode::full_random_on_only:
        SettingsCache::calcRGBMode = calc_rgb_mode::fixed_all;
        break;

      case calc_rgb_mode::fixed_all:
        SettingsCache::calcRGBMode = calc_rgb_mode::random_all;
        break;

      case calc_rgb_mode::random_all:
        SettingsCache::calcRGBMode = calc_rgb_mode::full_random_all;
        break;

      case calc_rgb_mode::full_random_all:
        SettingsCache::calcRGBMode = calc_rgb_mode::wheel_on_only;
        break;

      case calc_rgb_mode::wheel_on_only:
        SettingsCache::calcRGBMode = calc_rgb_mode::wheel_all;
        break;

      case calc_rgb_mode::wheel_all:
        SettingsCache::calcRGBMode = calc_rgb_mode::rainbow_on_only;
        break;

      case calc_rgb_mode::rainbow_on_only:
        SettingsCache::calcRGBMode = calc_rgb_mode::rainbow_all;
        break;

      case calc_rgb_mode::rainbow_all:
        SettingsCache::calcRGBMode = calc_rgb_mode::off;
        break;
      }
      break;

    case device_mode::clock:
      switch (SettingsCache::clockRGBMode)
      {
      case clock_rgb_mode::off:
        SettingsCache::clockRGBMode = clock_rgb_mode::by_content;
        break;

      case clock_rgb_mode::by_content:
        SettingsCache::clockRGBMode = clock_rgb_mode::random_on_only;
        break;

      case clock_rgb_mode::random_on_only:
        SettingsCache::clockRGBMode = clock_rgb_mode::full_random_on_only;
        break;

      case clock_rgb_mode::full_random_on_only:
        SettingsCache::clockRGBMode = clock_rgb_mode::fixed_all;
        break;

      case clock_rgb_mode::fixed_all:
        SettingsCache::clockRGBMode = clock_rgb_mode::random_all;
        break;

      case clock_rgb_mode::random_all:
        SettingsCache::clockRGBMode = clock_rgb_mode::full_random_all;
        break;

      case clock_rgb_mode::full_random_all:
        SettingsCache::clockRGBMode = clock_rgb_mode::wheel_on_only;
        break;

      case clock_rgb_mode::wheel_on_only:
        SettingsCache::clockRGBMode = clock_rgb_mode::wheel_all;
        break;

      case clock_rgb_mode::wheel_all:
        SettingsCache::clockRGBMode = clock_rgb_mode::rainbow_on_only;
        break;

      case clock_rgb_mode::rainbow_on_only:
        SettingsCache::clockRGBMode = clock_rgb_mode::rainbow_all;
        break;

      case clock_rgb_mode::rainbow_all:
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
  uint16_t _wheelHue;
  uint16_t _breathPhase;
  float _breathScale;
  static constexpr uint8_t MAX_CACHED_LEDS = 32;
  uint8_t _baseRed[MAX_CACHED_LEDS] = {0};
  uint8_t _baseGreen[MAX_CACHED_LEDS] = {0};
  uint8_t _baseBlue[MAX_CACHED_LEDS] = {0};

  // advance the color wheel hue and the breathing phase by one step once the cycle interval has elapsed
  bool advanceAnimation()
  {
    unsigned long now = millis();
    if ((now - _lastCycleTimestamp) >= _cycleInterval)
    {
      _lastCycleTimestamp = now;
      _wheelHue = (_wheelHue + 1) % 360;
      _breathPhase = (_breathPhase + 3) % 360;
      // keep a floor so digits dim rather than fully blank out
      _breathScale = 0.05f + 0.95f * ((sinf(radians(_breathPhase)) + 1.0f) / 2.0f);
      return (true);
    }
    return (false);
  }

  // set a digit's LED color applying the breathing brightness envelope if enabled
  void setDigitLED(int index, uint8_t red, uint8_t green, uint8_t blue)
  {
    if (index >= 0 && index < MAX_CACHED_LEDS)
    {
      _baseRed[index] = red;
      _baseGreen[index] = green;
      _baseBlue[index] = blue;
    }
    if (SettingsCache::breathingMode == breathing_mode::on)
    {
      red = static_cast<uint8_t>(red * _breathScale);
      green = static_cast<uint8_t>(green * _breathScale);
      blue = static_cast<uint8_t>(blue * _breathScale);
    }
    _displayHandler->setLED(index, red, green, blue);
  }

  // redraw the previously cached base colors with the current breathing brightness, without recomputing them
  void reapplyBreathing(int offset, int digitCount)
  {
    for (int i = 0; i < digitCount; i++)
    {
      int index = i + offset;
      if (index < MAX_CACHED_LEDS)
      {
        setDigitLED(index, _baseRed[index], _baseGreen[index], _baseBlue[index]);
      }
    }
  }

  // force to ignore lighting time constraints
  void forcedOn()
  {
    _forcedByShortcut = true;
  }

  // check if lighting is required
  bool isLighting(const struct tm *tm) const
  {
    return ((SettingsCache::ledMode == led_mode::led_mode::always) ||
            (Helper::isInTimeRange(SettingsCache::ledStartTime.hour, SettingsCache::ledStartTime.minute,
                                   tm->tm_hour, tm->tm_min, SettingsCache::ledDuration)) ||
            (Helper::isInTimeRange(SettingsCache::ledStartTime2.hour, SettingsCache::ledStartTime2.minute,
                                   tm->tm_hour, tm->tm_min, SettingsCache::ledDuration2)) ||
            _forcedByShortcut);
  }

  // get color according to content
  void getCalcColorByContent(digit_content content, uint8_t *red, uint8_t *green, uint8_t *blue) const
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

    default:
      // content not relevant to calculator mode (e.g. clock content), don't leave a stale color
      *red = 0;
      *green = 0;
      *blue = 0;
      break;
    }
  }

  // get color according to content
  void getClockColorByContent(digit_content content, uint8_t *red, uint8_t *green, uint8_t *blue) const
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

    case digit_content::time2:
      *red = SettingsCache::timeColor2.red;
      *green = SettingsCache::timeColor2.green;
      *blue = SettingsCache::timeColor2.blue;
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

    default:
      // content not relevant to clock mode (e.g. calculator content), don't leave a stale color
      *red = 0;
      *green = 0;
      *blue = 0;
      break;
    }
  }

  // get color according to content
  void getCalcColorAllByContent(digit_content content, uint8_t *red, uint8_t *green, uint8_t *blue) const
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
  void getRandomColor(uint8_t *red, uint8_t *green, uint8_t *blue) const
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
      bool isWheelMode = (SettingsCache::calcRGBMode == calc_rgb_mode::wheel_on_only ||
                          SettingsCache::calcRGBMode == calc_rgb_mode::wheel_all ||
                          SettingsCache::calcRGBMode == calc_rgb_mode::rainbow_on_only ||
                          SettingsCache::calcRGBMode == calc_rgb_mode::rainbow_all);
      bool breathingOn = (SettingsCache::breathingMode == breathing_mode::on);
      bool contentRefresh = _refresh;
      bool tick = (isWheelMode || breathingOn) && advanceAnimation();

      // recompute colors on a real content change or every tick while animating the wheel/rainbow;
      if (contentRefresh || (isWheelMode && tick))
      {
        _refresh = true;
      }
      else if (breathingOn && tick)
      {
        reapplyBreathing(offset, _displayHandler->getDigitCount());
      }
      if (_refresh)
      {
        // define a random color
        uint8_t red, green, blue, allRed = 0, allGreen = 0, allBlue = 0;
        getRandomColor(&red, &green, &blue);

        // define the current color wheel color
        uint8_t wheelRed, wheelGreen, wheelBlue;
        Helper::hsvToRgb(_wheelHue, 255, 255, &wheelRed, &wheelGreen, &wheelBlue);

        // spacing (in degrees) between digits for the rainbow effect
        int digitCount = _displayHandler->getDigitCount();
        int rainbowSpacing = 360 / max(1, digitCount);

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
          case calc_rgb_mode::by_content:
            getCalcColorByContent(content, &tempRed, &tempGreen, &tempBlue);
            setDigitLED(i + offset, tempRed, tempGreen, tempBlue);
            break;

          case calc_rgb_mode::by_content_all:
            setDigitLED(i + offset, allRed, allGreen, allBlue);
            break;

          case calc_rgb_mode::random_on_only:
            if (content != digit_content::none)
            {
              setDigitLED(i + offset, red, green, blue);
            }
            else
            {
              setDigitLED(i + offset, 0, 0, 0);
            }
            break;

          case calc_rgb_mode::full_random_on_only:
            if (content != digit_content::none)
            {
              getRandomColor(&tempRed, &tempGreen, &tempBlue);
              setDigitLED(i + offset, tempRed, tempGreen, tempBlue);
            }
            else
            {
              setDigitLED(i + offset, 0, 0, 0);
            }
            break;

          case calc_rgb_mode::fixed_all:
            setDigitLED(i + offset, SettingsCache::fixedCalcColor.red, SettingsCache::fixedCalcColor.green, SettingsCache::fixedCalcColor.blue);
            break;

          case calc_rgb_mode::random_all:
            setDigitLED(i + offset, red, green, blue);
            break;

          case calc_rgb_mode::full_random_all:
            getRandomColor(&tempRed, &tempGreen, &tempBlue);
            setDigitLED(i + offset, tempRed, tempGreen, tempBlue);
            break;

          case calc_rgb_mode::wheel_on_only:
            if (content != digit_content::none)
            {
              setDigitLED(i + offset, wheelRed, wheelGreen, wheelBlue);
            }
            else
            {
              setDigitLED(i + offset, 0, 0, 0);
            }
            break;

          case calc_rgb_mode::wheel_all:
            setDigitLED(i + offset, wheelRed, wheelGreen, wheelBlue);
            break;

          case calc_rgb_mode::rainbow_on_only:
          case calc_rgb_mode::rainbow_all:
            if ((SettingsCache::calcRGBMode == calc_rgb_mode::rainbow_all) || (content != digit_content::none))
            {
              uint8_t rainbowRed, rainbowGreen, rainbowBlue;
              Helper::hsvToRgb((_wheelHue + i * rainbowSpacing) % 360, 255, 255, &rainbowRed, &rainbowGreen, &rainbowBlue);
              setDigitLED(i + offset, rainbowRed, rainbowGreen, rainbowBlue);
            }
            else
            {
              setDigitLED(i + offset, 0, 0, 0);
            }
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
      bool isWheelMode = (SettingsCache::clockRGBMode == clock_rgb_mode::wheel_on_only ||
                          SettingsCache::clockRGBMode == clock_rgb_mode::wheel_all ||
                          SettingsCache::clockRGBMode == clock_rgb_mode::rainbow_on_only ||
                          SettingsCache::clockRGBMode == clock_rgb_mode::rainbow_all);
      bool breathingOn = (SettingsCache::breathingMode == breathing_mode::on);
      bool contentRefresh = _refresh;
      bool tick = (isWheelMode || breathingOn) && advanceAnimation();

      // recompute colors on a real content change, or every tick while animating the wheel/rainbow;
      // a breathing-only tick just redraws the cached colors at the new brightness, so random modes don't reroll
      if (contentRefresh || (isWheelMode && tick))
      {
        _refresh = true;
      }
      else if (breathingOn && tick)
      {
        reapplyBreathing(offset, _displayHandler->getDigitCount());
      }
      if (_refresh)
      {
        // define a random color
        uint8_t red, green, blue;
        getRandomColor(&red, &green, &blue);

        // define the current color wheel color
        uint8_t wheelRed, wheelGreen, wheelBlue;
        Helper::hsvToRgb(_wheelHue, 255, 255, &wheelRed, &wheelGreen, &wheelBlue);

        // spacing (in degrees) between digits for the rainbow effect
        int digitCount = _displayHandler->getDigitCount();
        int rainbowSpacing = 360 / max(1, digitCount);

        // loop over all digits
        for (int i = 0; i < _displayHandler->getDigitCount(); i++)
        {
          content = _displayHandler->getDigitContent(i);
          switch (SettingsCache::clockRGBMode)
          {
          case clock_rgb_mode::by_content:
            getClockColorByContent(content, &tempRed, &tempGreen, &tempBlue);
            setDigitLED(i + offset, tempRed, tempGreen, tempBlue);
            if (content != digit_content::none)
            {
              _refresh = false;
            }
            break;

          case clock_rgb_mode::random_on_only:
            if (content == digit_content::none)
            {
              setDigitLED(i + offset, 0, 0, 0);
            }
            else
            {
              setDigitLED(i + offset, red, green, blue);
              _refresh = false;
            }
            break;

          case clock_rgb_mode::full_random_on_only:
            if (content == digit_content::none)
            {
              setDigitLED(i + offset, 0, 0, 0);
            }
            else
            {
              getRandomColor(&tempRed, &tempGreen, &tempBlue);
              setDigitLED(i + offset, tempRed, tempGreen, tempBlue);
              _refresh = false;
            }
            break;

          case clock_rgb_mode::fixed_all:
            setDigitLED(i + offset, SettingsCache::fixedColor.red, SettingsCache::fixedColor.green, SettingsCache::fixedColor.blue);
            _refresh = false;
            break;

          case clock_rgb_mode::random_all:
            setDigitLED(i + offset, red, green, blue);
            _refresh = false;
            break;

          case clock_rgb_mode::full_random_all:
            getRandomColor(&tempRed, &tempGreen, &tempBlue);
            setDigitLED(i + offset, tempRed, tempGreen, tempBlue);
            _refresh = false;
            break;

          case clock_rgb_mode::wheel_on_only:
            if (content == digit_content::none)
            {
              setDigitLED(i + offset, 0, 0, 0);
            }
            else
            {
              setDigitLED(i + offset, wheelRed, wheelGreen, wheelBlue);
              _refresh = false;
            }
            break;

          case clock_rgb_mode::wheel_all:
            setDigitLED(i + offset, wheelRed, wheelGreen, wheelBlue);
            _refresh = false;
            break;

          case clock_rgb_mode::rainbow_on_only:
            if (content == digit_content::none)
            {
              setDigitLED(i + offset, 0, 0, 0);
            }
            else
            {
              uint8_t rainbowRed, rainbowGreen, rainbowBlue;
              Helper::hsvToRgb((_wheelHue + i * rainbowSpacing) % 360, 255, 255, &rainbowRed, &rainbowGreen, &rainbowBlue);
              setDigitLED(i + offset, rainbowRed, rainbowGreen, rainbowBlue);
              _refresh = false;
            }
            break;

          case clock_rgb_mode::rainbow_all:
          {
            uint8_t rainbowRed, rainbowGreen, rainbowBlue;
            Helper::hsvToRgb((_wheelHue + i * rainbowSpacing) % 360, 255, 255, &rainbowRed, &rainbowGreen, &rainbowBlue);
            setDigitLED(i + offset, rainbowRed, rainbowGreen, rainbowBlue);
          }
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
