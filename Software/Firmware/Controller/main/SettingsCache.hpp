// SettingsCache.hpp

// contains the current values of the settings
// allows fast access to settings

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <SettingEnum.h>
#include <Timezone.h>

class SettingsCache
{
public:
  SettingsCache() = delete;

  inline static startup_mode::startup_mode startupMode;
  inline static show_version::show_version showVersion;
  inline static auto_off_mode::auto_off_mode autoOffMode;
  inline static unsigned long autoOffDelay;
  inline static clock_mode::clock_mode clockMode;
  inline static hour_mode::hour_mode hourMode;
  inline static leading_zero::leading_zero leadingZero;
  inline static date_format::date_format dateFormat;
  inline static pir_mode::pir_mode pirMode;
  inline static unsigned long pirDelay;
  inline static gps_mode::gps_mode gpsMode;
  inline static gps_speed::gps_speed gpsSpeed;
  inline static unsigned long gpsSyncInterval;
  inline static temperature_mode::temperature_mode temperatureMode;
  inline static temperature_cf::temperature_cf temperatureCF;
  inline static led_mode::led_mode ledMode;
  inline static calc_rgb_mode::calc_rgb_mode calcRGBMode;
  inline static clock_rgb_mode::clock_rgb_mode clockRGBMode;
  inline static STARTTIME ledStartTime;
  inline static int ledDuration;
  inline static STARTTIME ledStartTime2;
  inline static int ledDuration2;
  inline static STARTTIME acpStartTime;
  inline static int acpDuration;
  inline static acp_force_on::acp_force_on acpForceOn;
  inline static RGBCOLOR negativeColor;
  inline static RGBCOLOR positiveColor;
  inline static RGBCOLOR errorColor;
  inline static RGBCOLOR negExpColor;
  inline static RGBCOLOR posExpColor;
  inline static RGBCOLOR fixedCalcColor;
  inline static RGBCOLOR timeColor;
  inline static RGBCOLOR dateColor;
  inline static RGBCOLOR tempColor;
  inline static RGBCOLOR fixedColor;
  inline static TimeChangeRule dstRule;
  inline static TimeChangeRule stdRule;
  inline static gps_notify_sync::gps_notify_sync gpsNotifySync;
  inline static RGBCOLOR gpsSyncColor;
  inline static notify_timer::notify_timer notifyTimer;
  inline static RGBCOLOR timerColor;
  inline static fixed_decimals::fixed_decimals fixedDecimals;
  inline static angle_mode::angle_mode angleMode;
  inline static show_busy_calc::show_busy_calc showBusyCalc;
  inline static int maxExpDigits;
  inline static int scrollDelay;
  inline static int calcPrecision;
  inline static int brightness;
};