// Helper.hpp

// some helper functions

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <esp_system.h>
#include <DeviceModes.h>
#include <DisplayTypes.h>

class Helper
{
public:
  Helper() = delete;

  // return a human readable name for the given reset reason
  static const char *resetReasonToString(esp_reset_reason_t reason)
  {
    switch (reason)
    {
    case ESP_RST_POWERON:
      return ("Power on");
    case ESP_RST_EXT:
      return ("External pin");
    case ESP_RST_SW:
      return ("Software reset");
    case ESP_RST_PANIC:
      return ("Panic/exception");
    case ESP_RST_INT_WDT:
      return ("Interrupt watchdog");
    case ESP_RST_TASK_WDT:
      return ("Task watchdog");
    case ESP_RST_WDT:
      return ("Other watchdog");
    case ESP_RST_DEEPSLEEP:
      return ("Deep sleep wakeup");
    case ESP_RST_BROWNOUT:
      return ("Brownout");
    case ESP_RST_SDIO:
      return ("SDIO");
    default:
      return ("Unknown");
    }
  }

  // return a human readable name for the given device mode
  static const char *deviceModeToString(device_mode mode)
  {
    switch (mode)
    {
    case device_mode::calculator:
      return ("Calculator");
    case device_mode::clock:
      return ("Clock");
    case device_mode::menu:
      return ("Menu");
    case device_mode::antipoisoning:
      return ("Antipoisoning");
    default:
      return ("Unknown");
    }
  }

  // return a human readable name for the given display type
  static const char *displayTypeToString(display_type type)
  {
    switch (type)
    {
    case display_type::in12a:
      return ("IN-12A");
    case display_type::in12b:
      return ("IN-12B");
    case display_type::in16:
      return ("IN-16");
    case display_type::in17:
      return ("IN-17");
    case display_type::b5870:
      return ("B-5870");
    case display_type::led:
      return ("7-segment LED");
    default:
      return ("Undefined");
    }
  }

  // convert rgb values to an integer
  static int rgbToInt(const uint8_t red, const uint8_t green, const uint8_t blue)
  {
    return ((red * 256 * 256) + (green * 256) + blue);
  }

  // convert an integer to rgb values
  static void intToRGB(const int value, uint8_t *red, uint8_t *green, uint8_t *blue)
  {
    *blue = value & 255;
    *green = (value >> 8) & 255;
    *red = (value >> 16) & 255;
  }

  // convert time (hours and minutes) to an integer
  static int timeToInt(const uint8_t hours, const uint8_t minutes)
  {
    return ((hours * 60) + minutes);
  }

  // convert an integer to time (hours and minutes)
  static void intToTime(const int value, uint8_t *hours, uint8_t *minutes)
  {
    *minutes = value % 60;
    *hours = value / 60;
  }

  // check if in time range
  static bool isInTimeRange(uint8_t startHour, uint8_t startMinute, uint8_t currentHour, uint8_t currentMinute, int duration)
  {
    int start = timeToInt(startHour, startMinute);
    int current = timeToInt(currentHour, currentMinute);

    if ((current - start) < 0)
    {
      current += 1440; // add one day
    }
    return (current - start < duration);
  }

  // convert a temperature from degrees Celsius to degrees Fahrenheit
  static float celsiusToFahrenheit(float celsius)
  {
    return (32.0f + celsius * 1.8f);
  }

  // convert HSV to RGB
  static void hsvToRgb(uint16_t hue, uint8_t sat, uint8_t val, uint8_t *red, uint8_t *green, uint8_t *blue)
  {
    uint8_t region = hue / 60;
    uint8_t remainder = (hue % 60) * 255 / 60;

    uint8_t p = (val * (255 - sat)) / 255;
    uint8_t q = (val * (255 - ((sat * remainder) / 255))) / 255;
    uint8_t t = (val * (255 - ((sat * (255 - remainder)) / 255))) / 255;

    switch (region)
    {
    case 0:
      *red = val; *green = t; *blue = p;
      break;
    case 1:
      *red = q; *green = val; *blue = p;
      break;
    case 2:
      *red = p; *green = val; *blue = t;
      break;
    case 3:
      *red = p; *green = q; *blue = val;
      break;
    case 4:
      *red = t; *green = p; *blue = val;
      break;
    default:
      *red = val; *green = p; *blue = q;
      break;
    }
  }
};