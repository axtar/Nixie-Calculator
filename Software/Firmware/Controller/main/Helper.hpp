// Helper.hpp

// some helper functions

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>

class Helper
{
public:
  Helper() = delete;

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

  // function to fill a string with a char
  static String fillString(char c, int count)
  {
    String result("");
    for (int i = 0; i < count; ++i)
    {
      result += c;
    }
    return result;
  }
};