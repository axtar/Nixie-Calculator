// DisplayHandler.hpp

// provides formatting und display functions

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <time.h>
#include <SettingDefs.h>
#include <DisplayDriver.hpp>
#include <CalcDefs.h>

enum class year_type : uint8_t
{
  none,
  partial,
  full
};

constexpr uint8_t ERROR_DIGIT = 6;

class DisplayHandler : public DisplayDriver
{
public:
  // inheriting constructor from base class
  using DisplayDriver::DisplayDriver;

  // refresh display
  void show()
  {
    refresh();
  }

  // show a C string
  void show(char *buffer, digit_content content = digit_content::none)
  {
    String s = buffer;
    show(s, content);
  }

  // format and show a string
  void show(const String &s, digit_content content = digit_content::none, bool showMenuSign = false)
  {
    bool prevDot = false;

    clear();

    int digit = getDigitCount() - 1;
    for (int i = s.length() - 1; i >= 0; i--)
    {
      switch (s[i])
      {
      case '-':
        setExpMinusSign(display_state::on); // use exponent sign on the right
        prevDot = false;
        break;

      case '+':
        setPlusSign(display_state::on);
        prevDot = false;
        break;

      case DECIMAL_SEPARATOR:
        if (prevDot)
        {
          digit--;
        }
        setDecimalSeparator(digit + getDspOffset(), display_state::on);
        // if content is error we have to set the digit content separately
        if (content == digit_content::calc_error)
        {
          setDigitContent(digit, content);
        }
        prevDot = true;
        break;

      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        setDigit(digit, s[i] - '0', content);
        digit--;
        prevDot = false;
        break;

      default:
        digit--;
        prevDot = false;
        break;
      }
    }
    if (showMenuSign)
    {
      setMenuSign(display_state::on);
    }
    else
    {
      setMenuSign(display_state::off);
    }
    refresh();
  }

  // show calculator input and results
  void showCalc(bool baseNegative, String base, bool exponentNegative, String exponent)
  {
    clear();
    int digit = 0;
    if (baseNegative)
    {
      setMinusSign(display_state::on);
    }
    // if input direction is right to left we may have to pad with leading spaces or zeros
    if (SettingsCache::calcInputDirec == calc_input_direc::rtl || SettingsCache::calcInputDirec == calc_input_direc::rtl_zero_padding)
    {
      bool hasDecimalPoint = false;
      if (base.indexOf(DECIMAL_SEPARATOR) != (-1))
      {
        hasDecimalPoint = true;
      }
      uint8_t pad = getDigitCount() - base.length() + (hasDecimalPoint ? 1 : 0);
      if (!exponent.isEmpty())
      {
        pad -= exponent.length();
        pad--;
      }
      if (SettingsCache::calcInputDirec == calc_input_direc::rtl)
      {
        // just adjust digit position
        digit += pad;
      }
      else
      {
        // left-pad with zeros
        for (int i = 0; i < pad; i++)
        {
          setDigit(digit, 0, baseNegative ? digit_content::calc_minus : digit_content::calc_plus);
          digit++;
        }
      }
    }
    for (int i = 0; i < base.length(); i++)
    {
      switch (base[i])
      {
      case DECIMAL_SEPARATOR:
        setDecimalSeparator(digit - 1 + getDspOffset(), display_state::on);
        break;

      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        setDigit(digit, base[i] - '0', baseNegative ? digit_content::calc_minus : digit_content::calc_plus);
        digit++;
        break;
      }
    }
    if (!exponent.isEmpty())
    {
      if (exponentNegative)
      {
        setExpMinusSign(display_state::on);
      }
      else
      {
        if (hasExpPlusSign())
        {
          if (exponent.toInt() != 0)
          {
            setExpPlusSign(display_state::on);
          }
        }
      }
      digit = getDigitCount() - exponent.length();
      for (int i = 0; i < exponent.length(); i++)
      {
        setDigit(digit, exponent[i] - '0', exponentNegative ? digit_content::exp_minus : digit_content::exp_plus);
        digit++;
      }
    }
    refresh();
  }

  // show calculator error
  void showCalcError(uint8_t error)
  {
    clear();
    setDecimalSeparator(ERROR_DIGIT - 1 + getDspOffset(), display_state::on);
    setDecimalSeparator(ERROR_DIGIT + 1 + getDspOffset(), display_state::on);
    if (error < 100)
    {
      setDigit(ERROR_DIGIT, error / 10, digit_content::calc_error);
      setDigit(ERROR_DIGIT + 1, error % 10, digit_content::calc_error);
    }
    refresh();
  }

  // show the time in the specified format
  void showTime(const struct tm *tm, uint8_t position, hour_mode::hour_mode hourMode, leading_zero::leading_zero leadingZero,
                bool showSeconds, bool space, time_separator::time_separator timeSeparator, bool dual = false)
  {
    int hour;

    if (hourMode == hour_mode::h12)
    {
      hour = convert24to12(tm->tm_hour);
    }
    else
    {
      hour = tm->tm_hour;
    }

    uint8_t currDigit = position;
    uint8_t seconds10 = tm->tm_sec / 10;
    uint8_t seconds01 = tm->tm_sec % 10;
    uint8_t minutes10 = tm->tm_min / 10;
    uint8_t minutes01 = tm->tm_min % 10;
    uint8_t hours10 = hour / 10;
    uint8_t hours01 = hour % 10;

    digit_content dc = dual == false ? digit_content::time : digit_content::time2;

    if ((leadingZero == leading_zero::on) || (hours10 != 0))
    {
      setDigit(currDigit, hours10, dc);
    }
    currDigit++;
    setDigit(currDigit, hours01, dc);
    if (!space)
    {
      switch (timeSeparator)
      {
      case time_separator::off:
        setDecimalSeparator(currDigit + getDspOffset(), display_state::off);
        break;

      case time_separator::blink:
        if (tm->tm_sec % 2 == 0)
        {
          setDecimalSeparator(currDigit + getDspOffset(), display_state::on);
        }
        else
        {
          setDecimalSeparator(currDigit + getDspOffset(), display_state::off);
        }
        break;

      case time_separator::on:
        setDecimalSeparator(currDigit + getDspOffset(), display_state::on);
        break;
      }
    }
    else
    {
      currDigit++;
    }
    currDigit++;
    setDigit(currDigit, minutes10, dc);
    currDigit++;
    setDigit(currDigit, minutes01, dc);
    if (!space && showSeconds)
    {
      switch (timeSeparator)
      {
      case time_separator::off:
        setDecimalSeparator(currDigit + getDspOffset(), display_state::off);
        break;

      case time_separator::blink:
        if (tm->tm_sec % 2 == 0)
        {
          setDecimalSeparator(currDigit + getDspOffset(), display_state::on);
        }
        else
        {
          setDecimalSeparator(currDigit + getDspOffset(), display_state::off);
        }
        break;

      case time_separator::on:
        setDecimalSeparator(currDigit + getDspOffset(), display_state::on);
        break;
      }
    }
    else
    {
      currDigit++;
    }
    currDigit++;
    if (showSeconds)
    {
      setDigit(currDigit, seconds10, dc);
      currDigit++;
      setDigit(currDigit, seconds01, dc);
    }
  }

  // show the date in the specified format
  void showDate(const struct tm *tm, uint8_t position, date_format::date_format dateFormat, year_type yearType, bool space)
  {
    int year = tm->tm_year + 1900;
    int month = tm->tm_mon + 1;
    uint8_t currDigit = position;

    uint8_t day10 = tm->tm_mday / 10;
    uint8_t day01 = tm->tm_mday % 10;
    uint8_t month10 = month / 10;
    uint8_t month01 = month % 10;
    uint8_t year0001 = year % 10;
    year /= 10;
    uint8_t year0010 = year % 10;
    year /= 10;
    uint8_t year0100 = year % 10;
    uint8_t year1000 = year / 10;

    switch (dateFormat)
    {
    case date_format::ddmmyy:
      setDigit(currDigit, day10, digit_content::date);
      currDigit++;
      setDigit(currDigit, day01, digit_content::date);
      if (space)
      {
        currDigit++;
      }
      else
      {
        setDecimalSeparator(currDigit + getDspOffset(), display_state::on);
      }
      currDigit++;
      setDigit(currDigit, month10, digit_content::date);
      currDigit++;
      setDigit(currDigit, month01, digit_content::date);
      if (space)
      {
        currDigit++;
      }
      else
      {
        if (yearType != year_type::none)
        {
          setDecimalSeparator(currDigit + getDspOffset(), display_state::on);
        }
      }
      currDigit++;
      switch (yearType)
      {
      case year_type::full:
        setDigit(currDigit, year1000, digit_content::date);
        currDigit++;
        setDigit(currDigit, year0100, digit_content::date);
        currDigit++;
        setDigit(currDigit, year0010, digit_content::date);
        currDigit++;
        setDigit(currDigit, year0001, digit_content::date);
        break;

      case year_type::partial:
        setDigit(currDigit, year0010, digit_content::date);
        currDigit++;
        setDigit(currDigit, year0001, digit_content::date);
        break;

      case year_type::none:
        // do nothing
        break;
      }
      break;

    case date_format::mmddyy:
      setDigit(currDigit, month10, digit_content::date);
      currDigit++;
      setDigit(currDigit, month01, digit_content::date);
      if (space)
      {
        currDigit++;
      }
      else
      {
        setDecimalSeparator(currDigit + getDspOffset(), display_state::on);
      }
      currDigit++;
      setDigit(currDigit, day10, digit_content::date);
      currDigit++;
      setDigit(currDigit, day01, digit_content::date);
      if (space)
      {
        currDigit++;
      }
      else
      {
        if (yearType != year_type::none)
        {
          setDecimalSeparator(currDigit + getDspOffset(), display_state::on);
        }
      }
      currDigit++;
      switch (yearType)
      {
      case year_type::full:
        setDigit(currDigit, year1000, digit_content::date);
        currDigit++;
        setDigit(currDigit, year0100, digit_content::date);
        currDigit++;
        setDigit(currDigit, year0010, digit_content::date);
        currDigit++;
        setDigit(currDigit, year0001, digit_content::date);
        break;

      case year_type::partial:
        setDigit(currDigit, year0010, digit_content::date);
        currDigit++;
        setDigit(currDigit, year0001, digit_content::date);
        break;

      case year_type::none:
        // do nothing
        break;
      }
      break;

    case date_format::yymmdd:
      switch (yearType)
      {
      case year_type::full:
        setDigit(currDigit, year1000, digit_content::date);
        currDigit++;
        setDigit(currDigit, year0100, digit_content::date);
        currDigit++;
        setDigit(currDigit, year0010, digit_content::date);
        currDigit++;
        setDigit(currDigit, year0001, digit_content::date);
        break;

      case year_type::partial:
        setDigit(currDigit, year0010, digit_content::date);
        currDigit++;
        setDigit(currDigit, year0001, digit_content::date);
        break;

      case year_type::none:
        // do nothing
        break;
      }
      if (space)
      {
        currDigit++;
      }
      else
      {
        if (yearType != year_type::none)
        {
          setDecimalSeparator(currDigit + getDspOffset(), display_state::on);
        }
      }
      currDigit++;
      setDigit(currDigit, month10, digit_content::date);
      currDigit++;
      setDigit(currDigit, month01, digit_content::date);
      if (space)
      {
        currDigit++;
      }
      else
      {
        setDecimalSeparator(currDigit + getDspOffset(), display_state::on);
      }
      currDigit++;
      setDigit(currDigit, day10, digit_content::date);
      currDigit++;
      setDigit(currDigit, day01, digit_content::date);
      break;

    case date_format::yyddmm:
      switch (yearType)
      {
      case year_type::full:
        setDigit(currDigit, year1000, digit_content::date);
        currDigit++;
        setDigit(currDigit, year0100, digit_content::date);
        currDigit++;
        setDigit(currDigit, year0010, digit_content::date);
        currDigit++;
        setDigit(currDigit, year0001, digit_content::date);
        break;

      case year_type::partial:
        setDigit(currDigit, year0010, digit_content::date);
        currDigit++;
        setDigit(currDigit, year0001, digit_content::date);
        break;

      case year_type::none:
        // do nothing
        break;
      }
      if (space)
      {
        currDigit++;
      }
      else
      {
        if (yearType != year_type::none)
        {
          setDecimalSeparator(currDigit + getDspOffset(), display_state::on);
        }
      }
      currDigit++;
      setDigit(currDigit, day10, digit_content::date);
      currDigit++;
      setDigit(currDigit, day01, digit_content::date);
      if (space)
      {
        currDigit++;
      }
      else
      {
        setDecimalSeparator(currDigit + getDspOffset(), display_state::on);
      }
      currDigit++;
      setDigit(currDigit, month10, digit_content::date);
      currDigit++;
      setDigit(currDigit, month01, digit_content::date);
      break;
    }
  }

  // format and show the temperature, used in some of the clock modes
  void showTemperature(uint8_t position, float temperature)
  {
    uint8_t currDigit = position;
    char buffer[10];

    if ((temperature > (-100)) && (temperature < 200))
    {
      if (temperature < 0)
      {
        setMinusSign(display_state::on);
      }
      else
      {
        setMinusSign(display_state::off);
      }
      if (temperature < 100)
      {
        sprintf(buffer, "%4.1f", abs(temperature));
      }
      else
      {
        sprintf(buffer, "%3.0f", abs(temperature));
      }
      String s = buffer;
      for (int i = 0; i < strlen(buffer); i++)
      {
        char c = buffer[i];
        if (c == DECIMAL_SEPARATOR)
        {
          setDecimalSeparator(currDigit - 1 + getDspOffset(), display_state::on);
        }
        else
        {
          setDigit(currDigit, static_cast<uint8_t>(c - 48), digit_content::temp);
          currDigit++;
        }
      }
    }
  }

  // clear temperature information
  void clearTemperature(uint8_t position)
  {
    setMinusSign(display_state::off);
    for (int i = position; i < getDigitCount(); i++)
    {
      setDecimalSeparator(i + getDspOffset(), display_state::off);
      setDigit(i, DIGIT_OFF);
      setDigitContent(i, digit_content::none);
    }
  }

  // create task for long calculation animation
  void createBusyCalcTask()
  {
    _taskRun = true;
    _taskEnd = false;
    if (SettingsCache::showBusyCalc == show_busy_calc::moving_decimal_separator)
    {
      xTaskCreate([](void *t)
                  { static_cast<DisplayHandler *>(t)->showBusyCalcMovingDecimalPoint(); },
                  "busy", 4096, this, tskIDLE_PRIORITY + 5, NULL);
    }
    else
    {
      xTaskCreate([](void *t)
                  { static_cast<DisplayHandler *>(t)->showBusyCalcDigitFlickering(); },
                  "busy", 4096, this, tskIDLE_PRIORITY + 5, NULL);
    }
  }

  // stop busy animation task
  void stopBusyCalcTask()
  {
    _taskRun = false;
    // wait for busy task end
    while (!_taskEnd)
    {
    }
  }

  // set a callback invoked from the busy-calc animation task on every animation frame
  void attachBusyTickCb(commitCallback cb)
  {
    _onBusyTick = cb;
  }

  // remove callback
  void detachBusyTickCb()
  {
    _onBusyTick = nullptr;
  }

  // display the partial result string
  void showScrollingResult(bool baseNegative, String scrollString, int decimalPos, bool exponentNegative, String exponent)
  {
    String s;
    if (decimalPos >= 0 && decimalPos < getDigitCount())
    {
      s = scrollString.substring(0, decimalPos + 1) + DECIMAL_SEPARATOR + scrollString.substring(decimalPos + 1);
    }
    else
    {
      s = scrollString;
    }
    showCalc(baseNegative, s, exponentNegative, exponent);
  }

private:
  volatile bool _taskRun;
  volatile bool _taskEnd;
  uint8_t _decimalPositionOffset;
  commitCallback _onBusyTick;

  // convert to 12 hour format
  int convert24to12(int hour24) const
  {
    int hour12;

    hour12 = hour24 % 12;
    if (hour12 == 0)
    {
      hour12 += 12;
    }
    return (hour12);
  }

  // display busy calc animation
  void showBusyCalcMovingDecimalPoint()
  {
    int decimalSeparator = 0;
    // wait 250 milliseconds before starting
    for (int i = 0; i < 25; i++)
    {
      if (_taskRun)
      {
        vTaskDelay(10 / portTICK_PERIOD_MS);
      }
      else
      {
        break;
      }
    }
    if (_taskRun)
    {
      clearDecimalSeparators();
      for (;;)
      {
        if (_taskRun)
        {
          setDecimalSeparator(decimalSeparator, display_state::on);
          show();
          if (_onBusyTick)
          {
            _onBusyTick();
          }
        }
        else
        {
          break;
        }
        vTaskDelay(75 / portTICK_PERIOD_MS);
        if (_taskRun)
        {
          setDecimalSeparator(decimalSeparator, display_state::off);
          show();
          if (_onBusyTick)
          {
            _onBusyTick();
          }
        }
        else
        {
          break;
        }
        decimalSeparator++;
        if (decimalSeparator >= getDecimalSeparatorCount())
        {
          decimalSeparator = 0;
        }
      }
    }
    _taskEnd = true;
    // deletes itself
    vTaskDelete(NULL);
  }

  // display busy calc animation
  void showBusyCalcDigitFlickering()
  {
    // wait 250 milliseconds before starting
    for (int i = 0; i < 25; i++)
    {
      if (_taskRun)
      {
        vTaskDelay(10 / portTICK_PERIOD_MS);
      }
      else
      {
        break;
      }
    }
    for (;;)
    {
      if (_taskRun)
      {
        int digits = random(0, getDigitCount());
        for (uint8_t i = 0; i <= digits; i++)
        {
          setDigit(i, static_cast<uint8_t>(random(0, 10)));
        }
        refresh();
        if (_onBusyTick)
        {
          _onBusyTick();
        }
      }
      else
      {
        break;
      }
      if (_taskRun)
      {
        vTaskDelay(10 / portTICK_PERIOD_MS);
      }
      if (_taskRun)
      {
        clearDisplay();
        if (_onBusyTick)
        {
          _onBusyTick();
        }
        vTaskDelay(25 / portTICK_PERIOD_MS);
      }
    }
    _taskEnd = true;
    // deletes itself
    vTaskDelete(NULL);
  }
};
