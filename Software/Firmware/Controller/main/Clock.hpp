// Clock.hpp

// provides clock mode functionality

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <Timezone.h>
#include <DS3232RTC.h>
#include <DisplayHandler.hpp>
#include <SettingsCache.hpp>
#include <KeyboardHandler.hpp>
#include <KeyboardDecoder.hpp>
#include <SettingsCache.hpp>
#include <Temperature.hpp>

constexpr uint32_t MAX_TIMER_INPUT = 8;
constexpr unsigned long MAX_TIMER_INTERVAL = (99 * 86400) + (23 * 3600) + (59 * 60) + 59;
constexpr unsigned long TIMER_NOTIFY_ON_INTERVAL = 200;   // ms
constexpr unsigned long TIMER_NOTIFY_OFF_INTERVAL = 1000; // ms
constexpr unsigned long TIME_SYNC_INTERVAL = 5 * 60 * 1000;

enum class stopwatch_mode : uint8_t
{
  zero,
  stopped,
  running
};

enum class timer_mode : uint8_t
{
  zero,
  set,
  stopped,
  running
};

enum class input_mode : uint8_t
{
  none,
  time,
  timer
};

class Clock
{
public:
  Clock(DisplayHandler *displayHandler)
      : _displayHandler(displayHandler)
  {
    _timeZone = new Timezone(SettingsCache::dstRule, SettingsCache::stdRule);
    _stopwatchMode = stopwatch_mode::zero;
    _timerMode = timer_mode::zero;
    _inputMode = input_mode::none;
    _timerMode = timer_mode::zero;
    _refreshLighting = false;
    _temperature = TEMP_UNDEFINED;
    _showDateSecond = random(1, 55);
    _hourParamSet = false;
    _timerEnded = false;
    _timerNotifyOn = false;
  }

  virtual ~Clock()
  {
    delete (_timeZone);
  }

  // initialize the clock
  void begin()
  {
    setTimeZone();
    _rtc.begin();
  }

  // return the real-time clock object
  DS3232RTC *getRTC()
  {
    return (&_rtc);
  }

  // called from the main loop,
  bool process(TimeElements *tm)
  {
    // set random show date second once an hour
    if (tm->Minute == 0)
    {
      if (!_hourParamSet)
      {
        _showDateSecond = random(1, 55);
        _hourParamSet = true;
      }
    }
    else
    {
      _hourParamSet = false;
    }
    switch (_inputMode)
    {
    case input_mode::none:
      displayTime(tm);
      break;

    default:
      showInput();
      break;
    }
    return (true);
  }

  // get if timer has ended
  bool getTimerEnded()
  {
    return (_timerEnded);
  }

  // stop end of timer notification
  void resetTimerEnded()
  {
    _timerEnded = false;
  }

  // calculate remaining time and notify end of timer if timer ended
  void processTimer()
  {
    if (_timerMode == timer_mode::running)
    {
      _remainingMillis = (_timerSetMillis - _timerElapsedMillis) - ((esp_timer_get_time() / 1000ULL) - _timerStartMillis);
      if (_remainingMillis > _timerSetMillis)
      {
        _timerMode = timer_mode::zero;
        _remainingMillis = 0;
        _timerEnded = true;
      }
    }
    if (_timerEnded || _timerNotifyOn)
    {
      if (SettingsCache::notifyTimer == notify_timer::on || _timerNotifyOn)
      {
        notifyTimer();
      }
    }
  }

  // notify end of timer
  void notifyTimer()
  {
    if (_timerNotifyOn)
    {
      // check if it's time to set notification off
      if (millis() - _timerNotifyChangeMillis > TIMER_NOTIFY_ON_INTERVAL)
      {
        _displayHandler->restoreLEDColors();
        _displayHandler->updateLEDs();
        _timerNotifyOn = false;
        _timerNotifyChangeMillis = millis();
      }
    }
    else
    {
      if (millis() - _timerNotifyChangeMillis > TIMER_NOTIFY_OFF_INTERVAL)
      {
        _displayHandler->saveLEDColors();
        _displayHandler->setAllLED(SettingsCache::timerColor.red, SettingsCache::timerColor.green, SettingsCache::timerColor.blue);
        _displayHandler->updateLEDs();
        _timerNotifyOn = true;
        _timerNotifyChangeMillis = millis();
      }
    }
  }

  // the temperature is displayed in some clock modes
  void setTemperature(float temperature)
  {
    _temperature = temperature;
  }

  // update the RTC time
  void setSystemAndRTCTime(time_t utc)
  {
    setTime(utc);
    _rtc.set(utc);
  }

  // get the temperature of the RTC IC
  float getBoardTemperature()
  {
    return (static_cast<float>(_rtc.temperature() / 4.0f));
  }

  // called if a key has been pressed
  void onKeyboardEvent(uint8_t keyCode, key_state keyState, bool functionKeyPressed, bool shiftKeyPressed)
  {

    operation op;
    uint8_t digit;
    key_function_type function;

    if (keyState == key_state::pressed)
    {
      KeyboardDecoder::decode(keyCode, functionKeyPressed, shiftKeyPressed, &function, &op, &digit);

      switch (function)
      {
      case key_function_type::numeric:
        numericInput(digit);
        break;

      case key_function_type::numericx2:
        numericInput(digit);
        numericInput(digit);
        break;

      case key_function_type::control:
      case key_function_type::operation:
        operationInput(op);
        break;

      default: // avoid warning
        break;
      }
    }
  }

  // return current time
  void getCurrentTime(TimeElements *tm) const
  {
    time_t utc, local;
    utc = now();
    // convert to local time
    local = _timeZone->toLocal(utc);
    // to time elements
    breakTime(local, *tm);
  }

  // adjust time by +/- seconds
  void adjustTime(int seconds)
  {
    time_t t = _rtc.get() + seconds;
    setSystemAndRTCTime(t);
  }

  // return the value of the refresh lighting flag
  bool getRefreshLighting() const
  {
    return (_refreshLighting);
  }

  // reset the refresh lighting flag
  void resetRefreshLighting()
  {
    _refreshLighting = false;
  }

private:
  DS3232RTC _rtc;
  TimeChangeRule _dstRule{"DST", Last, Sun, Mar, 2, 120};
  TimeChangeRule _stdRule{"STD ", Last, Sun, Oct, 3, 60};
  Timezone *_timeZone;
  DisplayHandler *_displayHandler;
  String _display;
  stopwatch_mode _stopwatchMode;
  timer_mode _timerMode;
  input_mode _inputMode;
  float _temperature;
  bool _refreshLighting;
  long _showDateSecond;
  bool _hourParamSet;
  bool _timerEnded;
  bool _timerNotifyOn;

  // some state variables
  int _movingLastSecond = 0;
  bool _movingLeftDirection = true;
  int _movingPosition = 3;
  bool _scrollingIsDate = false;
  int _scrollingTimePosition = 2;
  int _scrollingDatePosition = 13;
  uint64_t _scrollingLastMillis = millis();
  uint64_t _swStartMillis = 0;
  uint64_t _swElapsedMillis = 0;
  bool _swRunning = false;
  unsigned long _inputLastMillis = millis();
  bool _inputShow = true;
  uint64_t _timerSetMillis = 0;
  uint64_t _timerStartMillis = 0;
  uint64_t _timerElapsedMillis = 0;
  uint64_t _remainingMillis = 0;
  uint64_t _timerNotifyChangeMillis = 0;

  // set time zone rules from settings
  void setTimeZone()
  {
    _timeZone->setRules(SettingsCache::dstRule, SettingsCache::stdRule);
  }

  // set the time from user input
  void setTimeFromInput()
  {
    time_t local, utc;
    tmElements_t tm;

    if (_display.length() == _displayHandler->getDigitCount())
    {
      int temp = 0;
      temp = _display.substring(0, 4).toInt();
      tm.Year = CalendarYrToTm(temp);
      temp = _display.substring(4, 6).toInt();
      tm.Month = temp;
      temp = _display.substring(6, 8).toInt();
      tm.Day = temp;
      temp = _display.substring(8, 10).toInt();
      tm.Hour = temp;
      temp = _display.substring(10, 12).toInt();
      tm.Minute = temp;
      temp = _display.substring(12, 14).toInt();
      tm.Second = temp;
      local = makeTime(tm);
      // convert user input to UTC
      utc = _timeZone->toUTC(local);
      // set time
      setSystemAndRTCTime(utc);
    }
  }

  // display the time according to the selected clock mode
  void displayTime(TimeElements *tm)
  {
    switch (SettingsCache::clockMode)
    {
    case clock_mode::time:
      showTimeDefault(tm);
      break;

    case clock_mode::time_no_seconds:
      showTimeNoSeconds(tm);
      break;

    case clock_mode::time_moving:
      showMovingTime(tm);
      break;

    case clock_mode::time_or_date:
      showDateOrTime(tm);
      break;

    case clock_mode::time_and_date:
      showTimeAndDate(tm);
      break;

    case clock_mode::time_and_temp:
      showTimeAndTemp(tm);
      break;

    case clock_mode::time_and_date_and_temp:
      showTimeAndDateAndTemp(tm);
      break;

    case clock_mode::date_and_time_raw:
      showDateTimeRaw(tm);
      break;

    case clock_mode::timer:
      showTimer();
      break;

    case clock_mode::stopwatch:
      showStopWatch();
      break;
    }
  }

  // display the time, clock mode 0 = time
  void showTimeDefault(TimeElements *tm) const
  {
    _displayHandler->showTime(tm, 3, SettingsCache::hourMode, SettingsCache::leadingZero, true, true);
  }

  // display the time, clock mode 1 = time, no seconds
  void showTimeNoSeconds(TimeElements *tm) const
  {
    _displayHandler->showTime(tm, 4, SettingsCache::hourMode, SettingsCache::leadingZero, false, true);
  }

  // display the time, clock mode 2
  void showMovingTime(TimeElements *tm)
  {
    if (tm->Second != _movingLastSecond)
    {
      _displayHandler->clearDisplay();
      _refreshLighting = true;
      _movingLastSecond = tm->Second;
      if (_movingLeftDirection)
      {
        if (_movingPosition == 6)
        {
          _movingLeftDirection = false;
          _movingPosition--;
        }
        else
        {
          _movingPosition++;
        }
      }
      else
      {
        int final = 0;
        if ((SettingsCache::leadingZero == leading_zero::off) && ((tm->Hour) / 10 == 0))
        {
          final = (-1);
        }

        if (_movingPosition == final)
        {
          _movingLeftDirection = true;
          _movingPosition++;
        }
        else
        {
          _movingPosition--;
        }
      }
    }
    _displayHandler->showTime(tm, _movingPosition, SettingsCache::hourMode, SettingsCache::leadingZero, true, true);
  }

  // display date or time, clock mode 3
  void showDateOrTime(TimeElements *tm)
  {
    if ((tm->Second > _showDateSecond) && (tm->Second < _showDateSecond + 5))
    {
      if (!_scrollingIsDate)
      {
        if (millis() - _scrollingLastMillis > 75)
        {
          _displayHandler->clearDisplay();
          _displayHandler->showTime(tm, _scrollingTimePosition, SettingsCache::hourMode, SettingsCache::leadingZero, true, true);
          _displayHandler->showDate(tm, _scrollingDatePosition, SettingsCache::dateFormat, year_type::full, true);
          _displayHandler->show();
          _scrollingTimePosition--;
          _scrollingDatePosition--;
          _refreshLighting = true;
          if (_scrollingDatePosition == (1))
          {
            _scrollingIsDate = true;
            _scrollingTimePosition = -9;
            _scrollingDatePosition = 3;
            _refreshLighting = true;
          }
          _scrollingLastMillis = millis();
        }
      }
    }
    else
    {
      if (_scrollingIsDate)
      {
        if (millis() - _scrollingLastMillis > 75)
        {
          _displayHandler->clearDisplay();
          _displayHandler->showDate(tm, _scrollingDatePosition, SettingsCache::dateFormat, year_type::full, true);
          _displayHandler->showTime(tm, _scrollingTimePosition, SettingsCache::hourMode, SettingsCache::leadingZero, true, true);
          _displayHandler->show();
          _scrollingDatePosition++;
          _scrollingTimePosition++;
          _refreshLighting = true;
          if (_scrollingTimePosition == 3)
          {
            _refreshLighting = true;
            _scrollingIsDate = false;
            _scrollingTimePosition = 2;
            _scrollingDatePosition = 13;
          }
          _scrollingLastMillis = millis();
        }
      }
      else
      {
        // clean up if no time to full scroll date
        if (_scrollingLastMillis != 2)
        {
          _scrollingLastMillis = 2;
          _displayHandler->clearDisplay();
          _refreshLighting = true;
        }
        _displayHandler->showTime(tm, 3, SettingsCache::hourMode, SettingsCache::leadingZero, true, true);
      }
    }
  }

  // display date and time, clock mode 4
  void showTimeAndDate(TimeElements *tm) const
  {
    _displayHandler->showTime(tm, 0, SettingsCache::hourMode, SettingsCache::leadingZero, true, false);
    _displayHandler->showDate(tm, 8, SettingsCache::dateFormat, year_type::partial, false);
  }

  // display time and temperature, clock mode 5
  void showTimeAndTemp(TimeElements *tm)
  {
    _displayHandler->showTime(tm, 0, SettingsCache::hourMode, SettingsCache::leadingZero, true, true);
    showTemp(11);
  }

  // display time, date and temperature, clock mode 6
  void showTimeAndDateAndTemp(TimeElements *tm)
  {
    _displayHandler->showTime(tm, 0, SettingsCache::hourMode, SettingsCache::leadingZero, false, false);
    _displayHandler->showDate(tm, 5, SettingsCache::dateFormat, year_type::none, false);
    showTemp(11);
  }

  // display the temperature
  void showTemp(uint8_t position)
  {
    digit_content content = _displayHandler->getDigitContent(11);
    if (_temperature != TEMP_UNDEFINED)
    {
      if (content != digit_content::temp)
      {
        _refreshLighting = true;
      }
      _displayHandler->showTemperature(position, _temperature);
    }
    else
    {
      if (content == digit_content::temp)
      {
        _refreshLighting = true;
      }
      _displayHandler->clearTemperature(position);
    }
  }

  // display date and time in the format YYYYMMDDhhmmss, clock mode 7
  void showDateTimeRaw(TimeElements *tm) const
  {
    int year = tm->Year + 1970;
    uint8_t day10 = tm->Day / 10;
    uint8_t day01 = tm->Day % 10;
    uint8_t month10 = tm->Month / 10;
    uint8_t month01 = tm->Month % 10;
    uint8_t year0001 = year % 10;
    year /= 10;
    uint8_t year0010 = year % 10;
    year /= 10;
    uint8_t year0100 = year % 10;
    uint8_t year1000 = year / 10;
    uint8_t seconds10 = tm->Second / 10;
    uint8_t seconds01 = tm->Second % 10;
    uint8_t minutes10 = tm->Minute / 10;
    uint8_t minutes01 = tm->Minute % 10;
    uint8_t hours10 = tm->Hour / 10;
    uint8_t hours01 = tm->Hour % 10;
    _displayHandler->setDigit(0, year1000, digit_content::date);
    _displayHandler->setDigit(1, year0100, digit_content::date);
    _displayHandler->setDigit(2, year0010, digit_content::date);
    _displayHandler->setDigit(3, year0001, digit_content::date);
    _displayHandler->setDigit(4, month10, digit_content::date);
    _displayHandler->setDigit(5, month01, digit_content::date);
    _displayHandler->setDigit(6, day10, digit_content::date);
    _displayHandler->setDigit(7, day01, digit_content::date);
    _displayHandler->setDigit(8, hours10, digit_content::time);
    _displayHandler->setDigit(9, hours01, digit_content::time);
    _displayHandler->setDigit(10, minutes10, digit_content::time);
    _displayHandler->setDigit(11, minutes01, digit_content::time);
    _displayHandler->setDigit(12, seconds10, digit_content::time);
    _displayHandler->setDigit(13, seconds01, digit_content::time);
  }

  // display the timer, clock mode 8
  void showTimer()
  {
    uint8_t position = 1;

    switch (_timerMode)
    {
    case timer_mode::set:
      _remainingMillis = _timerSetMillis;
      _timerElapsedMillis = 0;
      break;

    case timer_mode::zero:
      _remainingMillis = 0;
      break;

    case timer_mode::running:
      // do nothing
      break;

    case timer_mode::stopped:
      _remainingMillis = _timerSetMillis - _timerElapsedMillis;
      break;
    }

    unsigned long ts = (_remainingMillis / 1000) % 60;
    unsigned long tm = (_remainingMillis / 60000) % 60;
    unsigned long th = (_remainingMillis / 3600000) % 24;
    unsigned long td = (_remainingMillis / 86400000) % 100;

    _displayHandler->setDigit(position + 0, td / 10, digit_content::time);
    _displayHandler->setDigit(position + 1, td % 10, digit_content::time);
    _displayHandler->setDigit(position + 3, th / 10, digit_content::time);
    _displayHandler->setDigit(position + 4, th % 10, digit_content::time);
    _displayHandler->setDigit(position + 6, tm / 10, digit_content::time);
    _displayHandler->setDigit(position + 7, tm % 10, digit_content::time);
    _displayHandler->setDigit(position + 9, ts / 10, digit_content::time);
    _displayHandler->setDigit(position + 10, ts % 10, digit_content::time);
  }

  // display the stopwatch, clock mode 9
  void showStopWatch()
  {
    int position = 1;

    switch (_stopwatchMode)
    {
    case stopwatch_mode::zero:
      _swElapsedMillis = 0;
      _swStartMillis = 0;
      _swRunning = false;
      break;

    case stopwatch_mode::running:
      if (_swRunning)
      {
        _swElapsedMillis = (esp_timer_get_time() / 1000ULL) - _swStartMillis;
      }
      else
      {
        _swRunning = true;
        if (_swStartMillis == 0)
        {
          _swStartMillis = (esp_timer_get_time() / 1000ULL);
          _swElapsedMillis = 0;
        }
      }
      break;

    case stopwatch_mode::stopped:
      if (_swRunning)
      {
        _swRunning = false;
        _swElapsedMillis = (esp_timer_get_time() / 1000ULL) - _swStartMillis;
      }
      break;
    }

    // show elapsed
    unsigned long tc = _swElapsedMillis % 1000;
    unsigned long ts = (_swElapsedMillis / 1000) % 60;
    unsigned long tm = (_swElapsedMillis / 60000) % 60;
    unsigned long th = (_swElapsedMillis / 3600000) % 24;

    _displayHandler->setDigit(position, th / 10, digit_content::time);
    _displayHandler->setDigit(position + 1, th % 10, digit_content::time);
    _displayHandler->setDigit(position + 3, tm / 10, digit_content::time);
    _displayHandler->setDigit(position + 4, tm % 10, digit_content::time);
    _displayHandler->setDigit(position + 6, ts / 10, digit_content::time);
    _displayHandler->setDigit(position + 7, ts % 10, digit_content::time);
    _displayHandler->setDigit(position + 9, (tc / 10) / 10, digit_content::time);
    _displayHandler->setDigit(position + 10, (tc / 10) % 10, digit_content::time);
  }

  // display blinking user input for the timer
  void showInput()
  {
    if (millis() - _inputLastMillis > 250)
    {
      _inputShow = !_inputShow;
      _inputLastMillis = millis();
    }
    if (_inputShow)
    {
      _displayHandler->show(_display);
    }
    else
    {
      _displayHandler->clearDisplay();
    }
  }

  // called if a numeric key is pressed in timer input mode
  void numericInput(uint8_t digit)
  {
    // during time input, process numeric input
    switch (_inputMode)
    {
    case input_mode::time:
      // avoid leading zeros
      if (_display.equals("0"))
      {
        _display = static_cast<char>(digit + 48);
      }
      else
      {
        if (_display.length() < _displayHandler->getDigitCount())
        {
          _display += static_cast<char>(digit + 48);
        }
      }
      break;

    case input_mode::timer:
      if (_display.equals("0"))
      {
        _display = static_cast<char>(digit + 48);
      }
      else
      {
        if (_display.length() < MAX_TIMER_INPUT)
        {
          _display += static_cast<char>(digit + 48);
        }
      }
      break;

    case input_mode::none:
      switch (digit)
      {
      case clock_mode::time:
      case clock_mode::time_no_seconds:
      case clock_mode::time_moving:
      case clock_mode::time_or_date:
      case clock_mode::time_and_date:
      case clock_mode::time_and_temp:
      case clock_mode::time_and_date_and_temp:
      case clock_mode::date_and_time_raw:
      case clock_mode::timer:
      case clock_mode::stopwatch:
        _displayHandler->clearDisplay();
        SettingsCache::clockMode = (clock_mode::clock_mode)digit;
        _refreshLighting = true;
        break;
      }
      break;
    }
  }

  // called if a operator key is pressed in clock mode
  void operationInput(operation op)
  {
    switch (op)
    {
#if CALC_TYPE == CALC_TYPE_RPN
    case operation::clear_stack:
#else
    case operation::allclear:
#endif
      switch (SettingsCache::clockMode)
      {
      case clock_mode::stopwatch:
        break;

      case clock_mode::timer:
        if (_inputMode == input_mode::none)
        {
          _inputMode = input_mode::timer;
          _display = "0";
          _displayHandler->show(_display);
          _refreshLighting = true;
        }
        else
        {
          _inputMode = input_mode::none;
          _displayHandler->clearDigits();
        }
        break;

        break;

      default:
        if (_inputMode == input_mode::none)
        {
          _inputMode = input_mode::time;
          _display = "0";
          _displayHandler->show(_display);
          _refreshLighting = true;
        }
        else
        {
          _inputMode = input_mode::none;
          _displayHandler->clearDigits();
        }
        break;
      }
      break;

#if CALC_TYPE == CALC_TYPE_RPN
    case operation::backspace:
#else
    case operation::clear:
#endif
      switch (SettingsCache::clockMode)
      {
      case clock_mode::stopwatch:
        _stopwatchMode = stopwatch_mode::zero;
        break;

      case clock_mode::timer:
        if (_inputMode != input_mode::none)
        {
          _display.remove(_display.length() - 1);
          if (_display.length() == 0)
          {
            _display = "0";
          }
          _displayHandler->show(_display);
        }
        else
        {
          _timerMode = timer_mode::set;
          _timerElapsedMillis = 0;
        }
        break;

      default:
        if (_inputMode != input_mode::none)
        {
          _display.remove(_display.length() - 1);
          if (_display.length() == 0)
          {
            _display = "0";
          }
          _displayHandler->show(_display);
        }
        break;
      }
      break;

#if CALC_TYPE == CALC_TYPE_RPN
    case operation::enter:
#else
    case operation::equals:
#endif
      switch (SettingsCache::clockMode)
      {
      case clock_mode::stopwatch:
        if (_stopwatchMode == stopwatch_mode::running)
        {
          _stopwatchMode = stopwatch_mode::stopped;
        }
        else
        {
          _stopwatchMode = stopwatch_mode::running;
        }
        break;

      case clock_mode::timer:
        if (_inputMode == input_mode::timer)
        {
          _inputMode = input_mode::none;
          _displayHandler->clearDigits();
          setTimer();
        }
        else
        {
          switch (_timerMode)
          {
          case timer_mode::running:
            _timerMode = timer_mode::stopped;
            _timerElapsedMillis += (esp_timer_get_time() / 1000ULL) - _timerStartMillis;
            break;

          case timer_mode::set:
          case timer_mode::stopped:
            _timerMode = timer_mode::running;
            _timerStartMillis = esp_timer_get_time() / 1000ULL;
            break;

          case timer_mode::zero:
            // do nothing
            break;
          }
        }
        break;

      default:
        switch (_inputMode)
        {
        case input_mode::time:
          _inputMode = input_mode::none;
          _displayHandler->clearDigits();
          setTimeFromInput();
          break;

        default: // avoid warning
          break;
        }
        break;
      }
    default: // avoid warning
      break;
    }
  }

  // set the timer according user input
  void setTimer()
  {
    u_int64_t interval = 0;
    if (_display.length() <= MAX_TIMER_INPUT)
    {
      int count = MAX_TIMER_INPUT - _display.length();
      for (int i = 0; i < count; i++)
      {
        _display = "0" + _display;
      }
      int temp = 0;
      temp = _display.substring(0, 2).toInt();
      interval += temp * 86400;
      temp = _display.substring(2, 4).toInt();
      interval += temp * 3600;
      temp = _display.substring(4, 6).toInt();
      interval += temp * 60;
      temp = _display.substring(6, 8).toInt();
      interval += temp;
      if (interval <= MAX_TIMER_INTERVAL)
      {
        interval *= 1000;
        interval += 999;
        _timerSetMillis = interval;
        _timerElapsedMillis = 0;
        _timerMode = timer_mode::set;
      }
    }
  }
};
