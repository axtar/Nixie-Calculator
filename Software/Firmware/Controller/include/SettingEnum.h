// SettingEnum.h

// settings related enums

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>

// define upper numeric limit for RGB and time settings
constexpr int32_t MAX_RGB_INT = 16777215;
constexpr int32_t MAX_TIME_INT = 1439;

// elements of a RGB setting
typedef struct
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} RGBCOLOR;

// elements of a time setting
typedef struct
{
  uint8_t hour;
  uint8_t minute;
} STARTTIME;

// enums
// using namespaces to avoid ambiguities

// settings
namespace setting_id
{
  // don't change without updating the manual
  enum setting_id
  {
    startupmode = 1, // Start in calculator or in clock mode
    showversion,     // Show version during start-up
    autooffmode,     // Shutdown high voltage or switch to clock mode after a period of no keyboard activity
    autooffdelay,    // Delay in minutes for auto off mode
    clockmode,       // Predefined display format in clock mode
    hourmode,        // 12 or 24 hours mode
    leadingzero,     // Hours leading zero off or on
    dateformat,      // Date format
    pirmode,         // PIR off or on
    pirdelay,        // PIR delay time in minutes before shutting down the high voltage
    gpsmode,         // GPS time sync off or on
    gpsspeed,        // GPS communication baud rate
    gpssyncinterval, // GPS time sync interval in minutes
    temperaturemode, // Temperature sensor off or on
    temperaturecf,   // Temperature in C or F
    ledmode,         // LEDs on by time or always
    calcrgbmode,     // Fixed colors, wheel, or random colors in calculator mode
    clockrgbmode,    // Fixed colors, wheel, or random colors in clock mode
    ledstarttime,    // Start time of LED lighting
    ledduration,     // Duration in minutes of LED lighting
    ledstarttime2,   // Start time of LED lighting
    ledduration2,    // Duration in minutes of LED lighting
    acpstarttime,    // Start time of cathode poisoning prevention
    acpduration,     // Duration in minutes of cathode poisoning prevention
    acpforceon,      // Force turning nixies on during cathode poisoning prevention
    negativecolor,   // RGB LED color for negative numbers in calculator mode
    positivecolor,   // RGB LED color for positive numbers in calculator mode
    errorcolor,      // RGB LED color for error in calculator mode
    negexpcolor,     // RGB LED color for negative exponents in calculator mode
    posexpcolor,     // RGB LED color for positive exponents in calculator mode
    fixedcalccolor,  // RGB LED fixed color in calculator mode
    timecolor,       // RGB LED color for time in clock mode
    datecolor,       // RGB LED color for date in clock mode
    tempcolor,       // RGB LED color for temperature in clock mode
    fixedcolor,      // RGB LED fixed color in clock mode
    dstweek,         // Daylight saving time change, week of month
    dstdow,          // Daylight saving time change, day of week
    dstmonth,        // Daylight saving time change, month
    dsthour,         // Daylight saving time change, hour
    dstoffset,       // Daylight saving time change, offset to UTC in minutes
    stdweek,         // Standard time change, week of month
    stddow,          // Standard time change, day of week
    stdmonth,        // Standard time change, month
    stdhour,         // Standard time change, hour
    stdoffset,       // Standard time change, offset to UTC in minutes
    gpsnotifysync,   // Notify GPS time sync with a short LED flash off or on
    gpssynccolor,    // RGB LED color for notifying GPS time sync
    notifytimer,     // notify end of timer off or on
    timercolor,      // RGB LED color for notifying end of timer
    fixeddecimals,   // Number of fixed decimals, 0 = floating
    anglemode,       // Startup angle mode
    showbusycalc,    // Show animation during long calculations
    maxexpdigits,    // Max exponent digits
    scrolldelay,     // Interval while scrolling result in 1/10 of seconds
    calcprecision,   // Precision of the calculations
    brightness       // 7-segment LED brightness
  };
}

namespace startup_mode
{
  enum startup_mode
  {
    calculator,
    clock
  };
}
namespace show_version
{
  enum show_version
  {
    off,
    on
  };
}

namespace auto_off_mode
{
  enum auto_off_mode
  {
    off,
    on,
    clock
  };
}

namespace clock_mode
{
  enum clock_mode
  {
    time,
    time_no_seconds,
    time_moving,
    time_or_date,
    time_and_date,
    time_and_temp,
    time_and_date_and_temp,
    date_and_time_raw,
    timer,
    stopwatch
  };
}

namespace hour_mode
{
  enum hour_mode
  {
    h12,
    h24
  };
}

namespace leading_zero
{
  enum leading_zero
  {
    off,
    on
  };
}

namespace date_format
{
  enum date_format
  {
    ddmmyy,
    yymmdd,
    mmddyy
  };
}

namespace pir_mode
{
  enum pir_mode
  {
    off,
    on
  };
}

namespace gps_mode
{
  enum gps_mode
  {
    off,
    on
  };
}

namespace gps_notify_sync
{
  enum gps_notify_sync
  {
    off,
    on
  };
}

namespace gps_speed
{
  enum gps_speed
  {
    br_2400,
    br_4800,
    br_9600,
    br_19200,
    br_38400,
    br_57600,
    br_115200
  };
}

namespace temperature_mode
{
  enum temperature_mode
  {
    off,
    on
  };
}

namespace temperature_cf
{
  enum temperature_cf
  {
    celsius,
    fahrenheit
  };
}

namespace led_mode
{
  enum led_mode
  {
    time,
    always
  };
}

namespace calc_rgb_mode
{
  enum calc_rgb_mode
  {
    off,
    ifnixieon,
    all,
    ifnixieonrandom,
    ifnixieonfullrandom,
    fixed,
    random,
    fullrandom
  };
}

namespace clock_rgb_mode
{
  enum clock_rgb_mode
  {
    off,
    split,
    splitrandom,
    splitfullrandom,
    fixed,
    random,
    fullrandom
  };
}

namespace acp_force_on
{
  enum acp_force_on
  {
    off,
    on
  };
}

namespace fixed_decimals
{
  enum fixed_decimals
  {
    off,
    one,
    two,
    three,
    four,
    five,
    six,
    seven,
    eight
  };
}

namespace notify_timer
{
  enum notify_timer
  {
    off,
    on
  };
}

namespace angle_mode
{
  enum angle_mode
  {
    degrees,
    radians
  };
}

namespace show_busy_calc
{
  enum show_busy_calc
  {
    off,
    moving_decimal_separator,
    digit_flickering
  };
}