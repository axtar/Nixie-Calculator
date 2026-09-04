// Settings.hpp

// stores and retrieves non-volatile settings

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <Timezone.hpp>
#include <nvs_flash.h>
#include <Preferences.h>
#include <Setting.hpp>
#include <map>
#include <SettingsCache.hpp>
#include <Helper.hpp>
#include <Config.h>

// definitions
constexpr auto SETTINGS_NAMESPACE = "CalcSettings";
constexpr auto AP_PASSWORD_KEY = "appassword";
constexpr size_t AP_PASSWORD_MIN_LENGTH = 8;
constexpr size_t AP_PASSWORD_MAX_LENGTH = 63;
constexpr auto AP_SSID_KEY = "apssid";
constexpr size_t AP_SSID_MIN_LENGTH = 1;
constexpr size_t AP_SSID_MAX_LENGTH = 32;
constexpr auto RTC_REF_EPOCH_KEY = "rtcrefepoch";
constexpr auto OTA_PASSWORD_KEY = "otapassword";
constexpr size_t OTA_PASSWORD_MIN_LENGTH = 8;
constexpr size_t OTA_PASSWORD_MAX_LENGTH = 63;

typedef std::map<setting_id::setting_id, Setting *> SETTINGSMAP;

class Settings
{
public:
  Settings()
  {
    // adding a new setting requires touching all of the following:
    //  - add the id to the setting_id enum in SettingDefs.h
    //  - register it below with a new Setting(...) line
    //  - add a matching field to SettingsCache.hpp
    //  - read it into that field in updateCache() below
    //  - write it back from that field in updateFromCache() below
    //  - (if user-editable) add a row for it in include/HTMLConfig.h
    //  - document the new setting in the user manuals

    // create the setting objects,
    // max allowed name length is 15 characters
    _settings[setting_id::startupmode] = new Setting(setting_id::startupmode, "startupmode", setting_type::numeric, startup_mode::calculator, startup_mode::calculator, startup_mode::clock);
    _settings[setting_id::showversion] = new Setting(setting_id::showversion, "showversion", setting_type::numeric, show_version::on, show_version::off, show_version::on);
    _settings[setting_id::autooffmode] = new Setting(setting_id::autooffmode, "autooffmode", setting_type::numeric, auto_off_mode::clock, auto_off_mode::off, auto_off_mode::clock);
    _settings[setting_id::autooffdelay] = new Setting(setting_id::autooffdelay, "autooffdelay", setting_type::numeric, 5, 1, 720);
    _settings[setting_id::clockmode] = new Setting(setting_id::clockmode, "clockmode", setting_type::numeric, clock_mode::time, clock_mode::time, clock_mode::stopwatch);
    _settings[setting_id::hourmode] = new Setting(setting_id::hourmode, "hourmode", setting_type::numeric, hour_mode::h24, hour_mode::h12, hour_mode::h24);
    _settings[setting_id::leadingzero] = new Setting(setting_id::leadingzero, "leadingzero", setting_type::numeric, leading_zero::on, leading_zero::off, leading_zero::on);
    _settings[setting_id::timeseparator] = new Setting(setting_id::timeseparator, "timeseparator", setting_type::numeric, time_separator::blink, time_separator::off, time_separator::on);
    _settings[setting_id::dateformat] = new Setting(setting_id::dateformat, "dateformat", setting_type::numeric, date_format::ddmmyy, date_format::ddmmyy, date_format::yyddmm);
    _settings[setting_id::pirmode] = new Setting(setting_id::pirmode, "pirmode", setting_type::numeric, pir_mode::off, pir_mode::off, pir_mode::on);
    _settings[setting_id::pirdelay] = new Setting(setting_id::pirdelay, "pirdelay", setting_type::numeric, 5, 1, 720);
    _settings[setting_id::gpsmode] = new Setting(setting_id::gpsmode, "gpsmode", setting_type::numeric, gps_mode::off, gps_mode::off, gps_mode::on);
    _settings[setting_id::gpsspeed] = new Setting(setting_id::gpsspeed, "gpsspeed", setting_type::numeric, gps_speed::br_38400, gps_speed::br_2400, gps_speed::br_115200);
    _settings[setting_id::gpssyncinterval] = new Setting(setting_id::gpssyncinterval, "gpssyncinterval", setting_type::numeric, 10, 1, 720);
    _settings[setting_id::temperaturemode] = new Setting(setting_id::temperaturemode, "temperaturemode", setting_type::numeric, temperature_mode::off, temperature_mode::off, temperature_mode::on);
    _settings[setting_id::temperaturecf] = new Setting(setting_id::temperaturecf, "temperaturecf", setting_type::numeric, temperature_cf::celsius, temperature_cf::celsius, temperature_cf::fahrenheit);
    _settings[setting_id::ledmode] = new Setting(setting_id::ledmode, "ledmode", setting_type::numeric, led_mode::always, led_mode::time, led_mode::always);
    _settings[setting_id::calcrgbmode] = new Setting(setting_id::calcrgbmode, "calcrgbmode", setting_type::numeric, calc_rgb_mode::off, calc_rgb_mode::off, calc_rgb_mode::full_random_all);
    _settings[setting_id::clockrgbmode] = new Setting(setting_id::clockrgbmode, "clockrgbmode", setting_type::numeric, clock_rgb_mode::off, clock_rgb_mode::off, clock_rgb_mode::full_random_all);
    _settings[setting_id::trigcolorchange] = new Setting(setting_id::trigcolorchange, "trigcolorchange", setting_type::numeric, trig_color_change::off, trig_color_change::off, trig_color_change::every_hour);
    _settings[setting_id::ledstarttime] = new Setting(setting_id::ledstarttime, "ledstarttime", setting_type::time, 0, 0, MAX_TIME_INT);
    _settings[setting_id::ledduration] = new Setting(setting_id::ledduration, "ledduration", setting_type::numeric, 0, 0, 720);
    _settings[setting_id::ledstarttime2] = new Setting(setting_id::ledstarttime2, "ledstarttime2", setting_type::time, 0, 0, MAX_TIME_INT);
    _settings[setting_id::ledduration2] = new Setting(setting_id::ledduration2, "ledduration2", setting_type::numeric, 0, 0, 720);
    _settings[setting_id::acpstarttime] = new Setting(setting_id::acpstarttime, "acpstarttime", setting_type::time, 0, 0, MAX_TIME_INT);
    _settings[setting_id::acpduration] = new Setting(setting_id::acpduration, "acpduration", setting_type::numeric, 0, 0, 720);
    _settings[setting_id::acpforceon] = new Setting(setting_id::acpforceon, "acpforceon", setting_type::numeric, acp_force_on::on, acp_force_on::off, acp_force_on::on);
    _settings[setting_id::negativecolor] = new Setting(setting_id::negativecolor, "negativecolor", setting_type::rgb, 0, 0, MAX_RGB_INT);
    _settings[setting_id::positivecolor] = new Setting(setting_id::positivecolor, "positivecolor", setting_type::rgb, 0, 0, MAX_RGB_INT);
    _settings[setting_id::errorcolor] = new Setting(setting_id::errorcolor, "errorcolor", setting_type::rgb, 0, 0, MAX_RGB_INT);
    _settings[setting_id::negexpcolor] = new Setting(setting_id::negexpcolor, "negexpcolor", setting_type::rgb, 0, 0, MAX_RGB_INT);
    _settings[setting_id::posexpcolor] = new Setting(setting_id::posexpcolor, "posexpcolor", setting_type::rgb, 0, 0, MAX_RGB_INT);
    _settings[setting_id::fixedcalccolor] = new Setting(setting_id::fixedcalccolor, "fixedcalccolor", setting_type::rgb, 0, 0, MAX_RGB_INT);
    _settings[setting_id::timecolor] = new Setting(setting_id::timecolor, "timecolor", setting_type::rgb, 0, 0, MAX_RGB_INT);
    _settings[setting_id::timecolor2] = new Setting(setting_id::timecolor2, "timecolor2", setting_type::rgb, 0, 0, MAX_RGB_INT);
    _settings[setting_id::datecolor] = new Setting(setting_id::datecolor, "datecolor", setting_type::rgb, 0, 0, MAX_RGB_INT);
    _settings[setting_id::tempcolor] = new Setting(setting_id::tempcolor, "tempcolor", setting_type::rgb, 0, 0, MAX_RGB_INT);
    _settings[setting_id::fixedcolor] = new Setting(setting_id::fixedcolor, "fixedcolor", setting_type::rgb, 0, 0, MAX_RGB_INT);
    _settings[setting_id::dstweek] = new Setting(setting_id::dstweek, "dstweek", setting_type::numeric, static_cast<int>(week_t::Last), static_cast<int>(week_t::First), static_cast<int>(week_t::Last));
    _settings[setting_id::dstdow] = new Setting(setting_id::dstdow, "dstdow", setting_type::numeric, static_cast<int>(dow_t::Sun), static_cast<int>(dow_t::Sun), static_cast<int>(dow_t::Sat));
    _settings[setting_id::dstmonth] = new Setting(setting_id::dstmonth, "dstmonth", setting_type::numeric, static_cast<int>(month_t::Mar), static_cast<int>(month_t::Jan), static_cast<int>(month_t::Dec));
    _settings[setting_id::dsthour] = new Setting(setting_id::dsthour, "dsthour", setting_type::numeric, 2, 0, 23);
    _settings[setting_id::dstoffset] = new Setting(setting_id::dstoffset, "dstoffset", setting_type::numeric, 120, -720, 840);
    _settings[setting_id::stdweek] = new Setting(setting_id::stdweek, "stdweek", setting_type::numeric, static_cast<int>(week_t::Last), static_cast<int>(week_t::First), static_cast<int>(week_t::Last));
    _settings[setting_id::stddow] = new Setting(setting_id::stddow, "stddow", setting_type::numeric, static_cast<int>(dow_t::Sun), static_cast<int>(dow_t::Sun), static_cast<int>(dow_t::Sat));
    _settings[setting_id::stdmonth] = new Setting(setting_id::stdmonth, "stdmonth", setting_type::numeric, static_cast<int>(month_t::Oct), static_cast<int>(month_t::Jan), static_cast<int>(month_t::Dec));
    _settings[setting_id::stdhour] = new Setting(setting_id::stdhour, "stdhour", setting_type::numeric, 3, 0, 23);
    _settings[setting_id::stdoffset] = new Setting(setting_id::stdoffset, "stdoffset", setting_type::numeric, 60, -720, 840);
    _settings[setting_id::dstweek2] = new Setting(setting_id::dstweek2, "dstweek2", setting_type::numeric, static_cast<int>(week_t::Last), static_cast<int>(week_t::First), static_cast<int>(week_t::Last));
    _settings[setting_id::dstdow2] = new Setting(setting_id::dstdow2, "dstdow2", setting_type::numeric, static_cast<int>(dow_t::Sun), static_cast<int>(dow_t::Sun), static_cast<int>(dow_t::Sat));
    _settings[setting_id::dstmonth2] = new Setting(setting_id::dstmonth2, "dstmonth2", setting_type::numeric, static_cast<int>(month_t::Mar), static_cast<int>(month_t::Jan), static_cast<int>(month_t::Dec));
    _settings[setting_id::dsthour2] = new Setting(setting_id::dsthour2, "dsthour2", setting_type::numeric, 2, 0, 23);
    _settings[setting_id::dstoffset2] = new Setting(setting_id::dstoffset2, "dstoffset2", setting_type::numeric, 0, -720, 840);
    _settings[setting_id::stdweek2] = new Setting(setting_id::stdweek2, "stdweek2", setting_type::numeric, static_cast<int>(week_t::Last), static_cast<int>(week_t::First), static_cast<int>(week_t::Last));
    _settings[setting_id::stddow2] = new Setting(setting_id::stddow2, "stddow2", setting_type::numeric, static_cast<int>(dow_t::Sun), static_cast<int>(dow_t::Sun), static_cast<int>(dow_t::Sat));
    _settings[setting_id::stdmonth2] = new Setting(setting_id::stdmonth2, "stdmonth2", setting_type::numeric, static_cast<int>(month_t::Oct), static_cast<int>(month_t::Jan), static_cast<int>(month_t::Dec));
    _settings[setting_id::stdhour2] = new Setting(setting_id::stdhour2, "stdhour2", setting_type::numeric, 3, 0, 23);
    _settings[setting_id::stdoffset2] = new Setting(setting_id::stdoffset2, "stdoffset2", setting_type::numeric, 0, -720, 840);
    _settings[setting_id::gpsnotifysync] = new Setting(setting_id::gpsnotifysync, "gpsnotifysync", setting_type::numeric, gps_notify_sync::off, gps_notify_sync::off, gps_notify_sync::on);
    _settings[setting_id::gpssynccolor] = new Setting(setting_id::gpssynccolor, "gpssynccolor", setting_type::rgb, Helper::rgbToInt(255, 0, 0), 0, MAX_RGB_INT);
    _settings[setting_id::notifytimer] = new Setting(setting_id::notifytimer, "notifytimer", setting_type::numeric, notify_timer::on, notify_timer::off, notify_timer::on);
    _settings[setting_id::timercolor] = new Setting(setting_id::timercolor, "timercolor", setting_type::rgb, Helper::rgbToInt(255, 255, 255), 0, MAX_RGB_INT);
    _settings[setting_id::fixeddecimals] = new Setting(setting_id::fixeddecimals, "fixeddecimals", setting_type::numeric, fixed_decimals::off, fixed_decimals::off, fixed_decimals::eight);
    _settings[setting_id::anglemode] = new Setting(setting_id::anglemode, "anglemode", setting_type::numeric, angle_mode::degrees, angle_mode::degrees, angle_mode::radians);
    _settings[setting_id::showbusycalc] = new Setting(setting_id::showbusycalc, "showbusycalc", setting_type::numeric, show_busy_calc::moving_decimal_separator, show_busy_calc::off, show_busy_calc::digit_flickering);
    _settings[setting_id::maxexpdigits] = new Setting(setting_id::maxexpdigits, "maxexpdigits", setting_type::numeric, 4, 2, 4);
    _settings[setting_id::scrolldelay] = new Setting(setting_id::scrolldelay, "scrolldelay", setting_type::numeric, 5, 1, 20);
    _settings[setting_id::calcprecision] = new Setting(setting_id::calcprecision, "calcprecision", setting_type::numeric, 32, 20, 32);
    _settings[setting_id::calcinputdirec] = new Setting(setting_id::calcinputdirec, "calcinputdirec", setting_type::numeric, calc_input_direc::ltr, calc_input_direc::ltr, calc_input_direc::rtl_zero_padding);
    _settings[setting_id::inputblinking] = new Setting(setting_id::inputblinking, "inputblinking", setting_type::numeric, input_blinking::on, input_blinking::off, input_blinking::on);
    _settings[setting_id::brightness] = new Setting(setting_id::brightness, "brightness", setting_type::numeric, 15, 1, 15);
    _settings[setting_id::dimbrightness] = new Setting(setting_id::dimbrightness, "dimbrightness", setting_type::numeric, 1, 1, 15);
    _settings[setting_id::dimstarttime] = new Setting(setting_id::dimstarttime, "dimstarttime", setting_type::time, 0, 0, MAX_TIME_INT);
    _settings[setting_id::dimduration] = new Setting(setting_id::dimduration, "dimduration", setting_type::numeric, 0, 0, 720);
    _settings[setting_id::apautostart] = new Setting(setting_id::apautostart, "apautostart", setting_type::numeric, ap_auto_start::off, ap_auto_start::off, ap_auto_start::on);
    _settings[setting_id::rtcdriftcorr] = new Setting(setting_id::rtcdriftcorr, "rtcdriftcorr", setting_type::numeric, 0, -60, 60);
    _settings[setting_id::exttempcorr] = new Setting(setting_id::exttempcorr, "exttempcorr", setting_type::numeric, 0, -100, 100);
  }

  virtual ~Settings()
  {
    // free memory
    for (const auto &value : _settings)
    {
      delete value.second;
    }
    _settings.clear();
  }

  // initialize preferences
  bool begin()
  {
    bool retVal = _preferences.begin(SETTINGS_NAMESPACE, false);
    if (!retVal)
    {

      // try to initialize
      nvs_flash_init();
      retVal = _preferences.begin(SETTINGS_NAMESPACE, false);
    }
    return (retVal);
  }

  // stop preferences
  void end()
  {
    _preferences.end();
  }

  // erase the entire settings namespace in NVS, including the AP password;
  bool factoryReset()
  {
    return (_preferences.clear());
  }

  // read the settings from the non-volatile storage
  void readSettings()
  {
    for (const auto &value : _settings)
    {
      int temp = readSetting(value.second->getName(), value.second->getDefault());
      if ((temp > value.second->getMax()) || (temp < value.second->getMin()))
      {
        // value is not valid, set to default
        temp = value.second->getDefault();
      }
      value.second->set(temp);
      value.second->setTempValue(temp);
      value.second->resetModified();
    }
    updateCache();
  }

  // return settings map
  const SETTINGSMAP &getSettingsMap() const
  {
    return (_settings);
  }

  // store the settings in the non-volatile storage
  void storeSettings()
  {
    for (const auto &value : _settings)
    {
      // store only if modified
      if (value.second->modified())
      {
        storeSetting(value.second->getName(), value.second->get());
        // reset modified flag
        value.second->resetModified();
      }
    }
    updateCache();
  }

  // get a setting by the setting id
  bool getSetting(setting_id::setting_id id, int *result) const
  {
    *result = 0;
    bool success = false;
    SETTINGSMAP::const_iterator it;
    it = _settings.find(id);
    if (it != _settings.end())
    {
      *result = it->second->get();
      success = true;
    }
    return (success);
  }

  // set a setting by the setting id
  bool setSetting(setting_id::setting_id id, int value)
  {
    bool success = false;
    SETTINGSMAP::iterator it;
    it = _settings.find(id);
    if (it != _settings.end())
    {
      it->second->set(value);
      success = true;
    }
    return (success);
  }

  // hide/unhide a setting
  bool hideSetting(setting_id::setting_id id, bool hide)
  {
    bool success = false;
    SETTINGSMAP::iterator it;
    it = _settings.find(id);
    if (it != _settings.end())
    {
      if (hide)
      {
        it->second->hide();
      }
      else
      {
        it->second->unhide();
      }
      success = true;
    }
    return (success);
  }

  // update the settings cache
  void updateCache()
  {
    int value;
    getSetting(setting_id::startupmode, reinterpret_cast<int *>(&SettingsCache::startupMode));
    getSetting(setting_id::showversion, reinterpret_cast<int *>(&SettingsCache::showVersion));
    getSetting(setting_id::autooffmode, reinterpret_cast<int *>(&SettingsCache::autoOffMode));
    getSetting(setting_id::autooffdelay, &value);
    SettingsCache::autoOffDelay = value * 60 * 1000;
    getSetting(setting_id::clockmode, reinterpret_cast<int *>(&SettingsCache::clockMode));
    getSetting(setting_id::hourmode, reinterpret_cast<int *>(&SettingsCache::hourMode));
    getSetting(setting_id::leadingzero, reinterpret_cast<int *>(&SettingsCache::leadingZero));
    getSetting(setting_id::timeseparator, reinterpret_cast<int *>(&SettingsCache::timeSeparator));
    getSetting(setting_id::dateformat, reinterpret_cast<int *>(&SettingsCache::dateFormat));
    getSetting(setting_id::pirmode, reinterpret_cast<int *>(&SettingsCache::pirMode));
    getSetting(setting_id::pirdelay, &value);
    SettingsCache::pirDelay = value * 60 * 1000;
    getSetting(setting_id::gpsmode, reinterpret_cast<int *>(&SettingsCache::gpsMode));
    getSetting(setting_id::gpsspeed, reinterpret_cast<int *>(&SettingsCache::gpsSpeed));
    getSetting(setting_id::gpssyncinterval, &value);
    SettingsCache::gpsSyncInterval = value * 60 * 1000;
    getSetting(setting_id::temperaturemode, reinterpret_cast<int *>(&SettingsCache::temperatureMode));
    getSetting(setting_id::temperaturecf, reinterpret_cast<int *>(&SettingsCache::temperatureCF));
    getSetting(setting_id::ledmode, reinterpret_cast<int *>(&SettingsCache::ledMode));
    getSetting(setting_id::calcrgbmode, reinterpret_cast<int *>(&SettingsCache::calcRGBMode));
    getSetting(setting_id::clockrgbmode, reinterpret_cast<int *>(&SettingsCache::clockRGBMode));
    getSetting(setting_id::trigcolorchange, reinterpret_cast<int *>(&SettingsCache::trigColorChange));
    getSetting(setting_id::ledstarttime, &value);
    Helper::intToTime(value, &SettingsCache::ledStartTime.hour, &SettingsCache::ledStartTime.minute);
    getSetting(setting_id::ledduration, &SettingsCache::ledDuration);
    getSetting(setting_id::ledstarttime2, &value);
    Helper::intToTime(value, &SettingsCache::ledStartTime2.hour, &SettingsCache::ledStartTime2.minute);
    getSetting(setting_id::ledduration2, &SettingsCache::ledDuration2);
    getSetting(setting_id::acpstarttime, &value);
    Helper::intToTime(value, &SettingsCache::acpStartTime.hour, &SettingsCache::acpStartTime.minute);
    getSetting(setting_id::acpduration, &SettingsCache::acpDuration);
    getSetting(setting_id::acpforceon, reinterpret_cast<int *>(&SettingsCache::acpForceOn));
    getSetting(setting_id::negativecolor, &value);
    Helper::intToRGB(value, &SettingsCache::negativeColor.red, &SettingsCache::negativeColor.green, &SettingsCache::negativeColor.blue);
    getSetting(setting_id::positivecolor, &value);
    Helper::intToRGB(value, &SettingsCache::positiveColor.red, &SettingsCache::positiveColor.green, &SettingsCache::positiveColor.blue);
    getSetting(setting_id::errorcolor, &value);
    Helper::intToRGB(value, &SettingsCache::errorColor.red, &SettingsCache::errorColor.green, &SettingsCache::errorColor.blue);
    getSetting(setting_id::negexpcolor, &value);
    Helper::intToRGB(value, &SettingsCache::negExpColor.red, &SettingsCache::negExpColor.green, &SettingsCache::negExpColor.blue);
    getSetting(setting_id::posexpcolor, &value);
    Helper::intToRGB(value, &SettingsCache::posExpColor.red, &SettingsCache::posExpColor.green, &SettingsCache::posExpColor.blue);
    getSetting(setting_id::fixedcalccolor, &value);
    Helper::intToRGB(value, &SettingsCache::fixedCalcColor.red, &SettingsCache::fixedCalcColor.green, &SettingsCache::fixedCalcColor.blue);
    getSetting(setting_id::timecolor, &value);
    Helper::intToRGB(value, &SettingsCache::timeColor.red, &SettingsCache::timeColor.green, &SettingsCache::timeColor.blue);
    getSetting(setting_id::timecolor2, &value);
    Helper::intToRGB(value, &SettingsCache::timeColor2.red, &SettingsCache::timeColor2.green, &SettingsCache::timeColor2.blue);
    getSetting(setting_id::datecolor, &value);
    Helper::intToRGB(value, &SettingsCache::dateColor.red, &SettingsCache::dateColor.green, &SettingsCache::dateColor.blue);
    getSetting(setting_id::tempcolor, &value);
    Helper::intToRGB(value, &SettingsCache::tempColor.red, &SettingsCache::tempColor.green, &SettingsCache::tempColor.blue);
    getSetting(setting_id::fixedcolor, &value);
    Helper::intToRGB(value, &SettingsCache::fixedColor.red, &SettingsCache::fixedColor.green, &SettingsCache::fixedColor.blue);
    getSetting(setting_id::dstdow, &value);
    SettingsCache::dstRule.dow = value;
    getSetting(setting_id::dsthour, &value);
    SettingsCache::dstRule.hour = value;
    getSetting(setting_id::dstmonth, &value);
    SettingsCache::dstRule.month = value;
    getSetting(setting_id::dstoffset, &value);
    SettingsCache::dstRule.offset = value;
    getSetting(setting_id::dstweek, &value);
    SettingsCache::dstRule.week = value;
    getSetting(setting_id::stddow, &value);
    SettingsCache::stdRule.dow = value;
    getSetting(setting_id::stdhour, &value);
    SettingsCache::stdRule.hour = value;
    getSetting(setting_id::stdmonth, &value);
    SettingsCache::stdRule.month = value;
    getSetting(setting_id::stdoffset, &value);
    SettingsCache::stdRule.offset = value;
    getSetting(setting_id::stdweek, &value);
    SettingsCache::stdRule.week = value;
    getSetting(setting_id::dstdow2, &value);
    SettingsCache::dstRule2.dow = value;
    getSetting(setting_id::dsthour2, &value);
    SettingsCache::dstRule2.hour = value;
    getSetting(setting_id::dstmonth2, &value);
    SettingsCache::dstRule2.month = value;
    getSetting(setting_id::dstoffset2, &value);
    SettingsCache::dstRule2.offset = value;
    getSetting(setting_id::dstweek2, &value);
    SettingsCache::dstRule2.week = value;
    getSetting(setting_id::stddow2, &value);
    SettingsCache::stdRule2.dow = value;
    getSetting(setting_id::stdhour2, &value);
    SettingsCache::stdRule2.hour = value;
    getSetting(setting_id::stdmonth2, &value);
    SettingsCache::stdRule2.month = value;
    getSetting(setting_id::stdoffset2, &value);
    SettingsCache::stdRule2.offset = value;
    getSetting(setting_id::stdweek2, &value);
    SettingsCache::stdRule2.week = value;
    getSetting(setting_id::gpsnotifysync, reinterpret_cast<int *>(&SettingsCache::gpsNotifySync));
    getSetting(setting_id::gpssynccolor, &value);
    Helper::intToRGB(value, &SettingsCache::gpsSyncColor.red, &SettingsCache::gpsSyncColor.green, &SettingsCache::gpsSyncColor.blue);
    getSetting(setting_id::notifytimer, reinterpret_cast<int *>(&SettingsCache::notifyTimer));
    getSetting(setting_id::timercolor, &value);
    Helper::intToRGB(value, &SettingsCache::timerColor.red, &SettingsCache::timerColor.green, &SettingsCache::timerColor.blue);
    getSetting(setting_id::fixeddecimals, reinterpret_cast<int *>(&SettingsCache::fixedDecimals));
    getSetting(setting_id::anglemode, reinterpret_cast<int *>(&SettingsCache::angleMode));
    getSetting(setting_id::showbusycalc, reinterpret_cast<int *>(&SettingsCache::showBusyCalc));
    getSetting(setting_id::maxexpdigits, &SettingsCache::maxExpDigits);
    getSetting(setting_id::scrolldelay, &SettingsCache::scrollDelay);
    getSetting(setting_id::calcprecision, &SettingsCache::calcPrecision);
    getSetting(setting_id::calcinputdirec, reinterpret_cast<int *>(&SettingsCache::calcInputDirec));
    getSetting(setting_id::inputblinking, reinterpret_cast<int *>(&SettingsCache::inputBlinking));
    getSetting(setting_id::brightness, &SettingsCache::brightness);
    getSetting(setting_id::dimbrightness, &SettingsCache::dimBrightness);
    getSetting(setting_id::dimstarttime, &value);
    Helper::intToTime(value, &SettingsCache::dimStartTime.hour, &SettingsCache::dimStartTime.minute);
    getSetting(setting_id::dimduration, &SettingsCache::dimDuration);
    getSetting(setting_id::apautostart, reinterpret_cast<int *>(&SettingsCache::apAutoStart));
    getSetting(setting_id::rtcdriftcorr, &SettingsCache::rtcDriftCorr);
    getSetting(setting_id::exttempcorr, &SettingsCache::extTempCorr);
  }

  // write the current values held in the settings cache back into the settings
  void updateFromCache()
  {
    setSetting(setting_id::startupmode, static_cast<int>(SettingsCache::startupMode));
    setSetting(setting_id::showversion, static_cast<int>(SettingsCache::showVersion));
    setSetting(setting_id::autooffmode, static_cast<int>(SettingsCache::autoOffMode));
    setSetting(setting_id::autooffdelay, static_cast<int>(SettingsCache::autoOffDelay / 60000));
    setSetting(setting_id::clockmode, static_cast<int>(SettingsCache::clockMode));
    setSetting(setting_id::hourmode, static_cast<int>(SettingsCache::hourMode));
    setSetting(setting_id::leadingzero, static_cast<int>(SettingsCache::leadingZero));
    setSetting(setting_id::timeseparator, static_cast<int>(SettingsCache::timeSeparator));
    setSetting(setting_id::dateformat, static_cast<int>(SettingsCache::dateFormat));
    setSetting(setting_id::pirmode, static_cast<int>(SettingsCache::pirMode));
    setSetting(setting_id::pirdelay, static_cast<int>(SettingsCache::pirDelay / 60000));
    setSetting(setting_id::gpsmode, static_cast<int>(SettingsCache::gpsMode));
    setSetting(setting_id::gpsspeed, static_cast<int>(SettingsCache::gpsSpeed));
    setSetting(setting_id::gpssyncinterval, static_cast<int>(SettingsCache::gpsSyncInterval / 60000));
    setSetting(setting_id::temperaturemode, static_cast<int>(SettingsCache::temperatureMode));
    setSetting(setting_id::temperaturecf, static_cast<int>(SettingsCache::temperatureCF));
    setSetting(setting_id::ledmode, static_cast<int>(SettingsCache::ledMode));
    setSetting(setting_id::calcrgbmode, static_cast<int>(SettingsCache::calcRGBMode));
    setSetting(setting_id::clockrgbmode, static_cast<int>(SettingsCache::clockRGBMode));
    setSetting(setting_id::trigcolorchange, static_cast<int>(SettingsCache::trigColorChange));
    setSetting(setting_id::ledstarttime, Helper::timeToInt(SettingsCache::ledStartTime.hour, SettingsCache::ledStartTime.minute));
    setSetting(setting_id::ledduration, SettingsCache::ledDuration);
    setSetting(setting_id::ledstarttime2, Helper::timeToInt(SettingsCache::ledStartTime2.hour, SettingsCache::ledStartTime2.minute));
    setSetting(setting_id::ledduration2, SettingsCache::ledDuration2);
    setSetting(setting_id::acpstarttime, Helper::timeToInt(SettingsCache::acpStartTime.hour, SettingsCache::acpStartTime.minute));
    setSetting(setting_id::acpduration, SettingsCache::acpDuration);
    setSetting(setting_id::acpforceon, static_cast<int>(SettingsCache::acpForceOn));
    setSetting(setting_id::negativecolor, Helper::rgbToInt(SettingsCache::negativeColor.red, SettingsCache::negativeColor.green, SettingsCache::negativeColor.blue));
    setSetting(setting_id::positivecolor, Helper::rgbToInt(SettingsCache::positiveColor.red, SettingsCache::positiveColor.green, SettingsCache::positiveColor.blue));
    setSetting(setting_id::errorcolor, Helper::rgbToInt(SettingsCache::errorColor.red, SettingsCache::errorColor.green, SettingsCache::errorColor.blue));
    setSetting(setting_id::negexpcolor, Helper::rgbToInt(SettingsCache::negExpColor.red, SettingsCache::negExpColor.green, SettingsCache::negExpColor.blue));
    setSetting(setting_id::posexpcolor, Helper::rgbToInt(SettingsCache::posExpColor.red, SettingsCache::posExpColor.green, SettingsCache::posExpColor.blue));
    setSetting(setting_id::fixedcalccolor, Helper::rgbToInt(SettingsCache::fixedCalcColor.red, SettingsCache::fixedCalcColor.green, SettingsCache::fixedCalcColor.blue));
    setSetting(setting_id::timecolor, Helper::rgbToInt(SettingsCache::timeColor.red, SettingsCache::timeColor.green, SettingsCache::timeColor.blue));
    setSetting(setting_id::timecolor2, Helper::rgbToInt(SettingsCache::timeColor2.red, SettingsCache::timeColor2.green, SettingsCache::timeColor2.blue));
    setSetting(setting_id::datecolor, Helper::rgbToInt(SettingsCache::dateColor.red, SettingsCache::dateColor.green, SettingsCache::dateColor.blue));
    setSetting(setting_id::tempcolor, Helper::rgbToInt(SettingsCache::tempColor.red, SettingsCache::tempColor.green, SettingsCache::tempColor.blue));
    setSetting(setting_id::fixedcolor, Helper::rgbToInt(SettingsCache::fixedColor.red, SettingsCache::fixedColor.green, SettingsCache::fixedColor.blue));
    setSetting(setting_id::dstdow, SettingsCache::dstRule.dow);
    setSetting(setting_id::dsthour, SettingsCache::dstRule.hour);
    setSetting(setting_id::dstmonth, SettingsCache::dstRule.month);
    setSetting(setting_id::dstoffset, SettingsCache::dstRule.offset);
    setSetting(setting_id::dstweek, SettingsCache::dstRule.week);
    setSetting(setting_id::stddow, SettingsCache::stdRule.dow);
    setSetting(setting_id::stdhour, SettingsCache::stdRule.hour);
    setSetting(setting_id::stdmonth, SettingsCache::stdRule.month);
    setSetting(setting_id::stdoffset, SettingsCache::stdRule.offset);
    setSetting(setting_id::stdweek, SettingsCache::stdRule.week);
    setSetting(setting_id::dstdow2, SettingsCache::dstRule2.dow);
    setSetting(setting_id::dsthour2, SettingsCache::dstRule2.hour);
    setSetting(setting_id::dstmonth2, SettingsCache::dstRule2.month);
    setSetting(setting_id::dstoffset2, SettingsCache::dstRule2.offset);
    setSetting(setting_id::dstweek2, SettingsCache::dstRule2.week);
    setSetting(setting_id::stddow2, SettingsCache::stdRule2.dow);
    setSetting(setting_id::stdhour2, SettingsCache::stdRule2.hour);
    setSetting(setting_id::stdmonth2, SettingsCache::stdRule2.month);
    setSetting(setting_id::stdoffset2, SettingsCache::stdRule2.offset);
    setSetting(setting_id::stdweek2, SettingsCache::stdRule2.week);
    setSetting(setting_id::gpsnotifysync, static_cast<int>(SettingsCache::gpsNotifySync));
    setSetting(setting_id::gpssynccolor, Helper::rgbToInt(SettingsCache::gpsSyncColor.red, SettingsCache::gpsSyncColor.green, SettingsCache::gpsSyncColor.blue));
    setSetting(setting_id::notifytimer, static_cast<int>(SettingsCache::notifyTimer));
    setSetting(setting_id::timercolor, Helper::rgbToInt(SettingsCache::timerColor.red, SettingsCache::timerColor.green, SettingsCache::timerColor.blue));
    setSetting(setting_id::fixeddecimals, static_cast<int>(SettingsCache::fixedDecimals));
    setSetting(setting_id::anglemode, static_cast<int>(SettingsCache::angleMode));
    setSetting(setting_id::showbusycalc, static_cast<int>(SettingsCache::showBusyCalc));
    setSetting(setting_id::maxexpdigits, SettingsCache::maxExpDigits);
    setSetting(setting_id::scrolldelay, SettingsCache::scrollDelay);
    setSetting(setting_id::calcprecision, SettingsCache::calcPrecision);
    setSetting(setting_id::calcinputdirec, static_cast<int>(SettingsCache::calcInputDirec));
    setSetting(setting_id::inputblinking, static_cast<int>(SettingsCache::inputBlinking));
    setSetting(setting_id::brightness, SettingsCache::brightness);
    setSetting(setting_id::dimbrightness, SettingsCache::dimBrightness);
    setSetting(setting_id::dimstarttime, Helper::timeToInt(SettingsCache::dimStartTime.hour, SettingsCache::dimStartTime.minute));
    setSetting(setting_id::dimduration, SettingsCache::dimDuration);
    setSetting(setting_id::apautostart, static_cast<int>(SettingsCache::apAutoStart));
    setSetting(setting_id::rtcdriftcorr, SettingsCache::rtcDriftCorr);
    setSetting(setting_id::exttempcorr, SettingsCache::extTempCorr);
  }

  // reset all settings to the default value
  void resetDefaults() const
  {
    for (const auto &value : _settings)
    {
      value.second->reset();
    }
  }

  // get max value of a setting
  int getSettingMax(setting_id::setting_id id) const
  {
    SETTINGSMAP::const_iterator it;
    it = _settings.find(id);
    if (it != _settings.end())
    {
      return (it->second->getMax());
    }
    return (0);
  }

  // get min value of a setting
  int getSettingMin(setting_id::setting_id id) const
  {
    SETTINGSMAP::const_iterator it;
    it = _settings.find(id);
    if (it != _settings.end())
    {
      return (it->second->getMin());
    }
    return (0);
  }

  // return the access point password, or the configured default if none was ever stored
  String getApPassword()
  {
    String pwd = _preferences.getString(AP_PASSWORD_KEY, "");
    if (pwd.length() == 0)
    {
      return (String(AP_PWD));
    }
    return (pwd);
  }

  // validate and store a new access point password
  bool setApPassword(const String &pwd)
  {
    if ((pwd.length() < AP_PASSWORD_MIN_LENGTH) || (pwd.length() > AP_PASSWORD_MAX_LENGTH))
    {
      return (false);
    }
    _preferences.putString(AP_PASSWORD_KEY, pwd);
    return (true);
  }

  // remove the stored access point password so getApPassword() falls back to AP_PWD
  void resetApPassword()
  {
    _preferences.remove(AP_PASSWORD_KEY);
  }

  // return the access point SSID, or the configured default if none was ever stored
  String getApSsid()
  {
    String ssid = _preferences.getString(AP_SSID_KEY, "");
    if (ssid.length() == 0)
    {
      return (String(AP_SSID));
    }
    return (ssid);
  }

  // validate and store a new access point SSID
  bool setApSsid(const String &ssid)
  {
    if ((ssid.length() < AP_SSID_MIN_LENGTH) || (ssid.length() > AP_SSID_MAX_LENGTH))
    {
      return (false);
    }
    _preferences.putString(AP_SSID_KEY, ssid);
    return (true);
  }

  // remove the stored access point SSID so getApSsid() falls back to AP_SSID
  void resetApSsid()
  {
    _preferences.remove(AP_SSID_KEY);
  }

  // return the firmware update (OTA) password, or the configured default if none was ever stored
  String getOtaPassword()
  {
    String pwd = _preferences.getString(OTA_PASSWORD_KEY, "");
    if (pwd.length() == 0)
    {
      return (String(OTA_PWD));
    }
    return (pwd);
  }

  // validate and store a new firmware update password
  bool setOtaPassword(const String &pwd)
  {
    if ((pwd.length() < OTA_PASSWORD_MIN_LENGTH) || (pwd.length() > OTA_PASSWORD_MAX_LENGTH))
    {
      return (false);
    }
    _preferences.putString(OTA_PASSWORD_KEY, pwd);
    return (true);
  }

  // remove the stored OTA password so getOtaPassword() falls back to OTA_PWD
  void resetOtaPassword()
  {
    _preferences.remove(OTA_PASSWORD_KEY);
  }

  // return the epoch (UTC seconds) the RTC was last deliberately set to
  time_t getRtcRefEpoch()
  {
    return (static_cast<time_t>(_preferences.getULong64(RTC_REF_EPOCH_KEY, 0)));
  }

  // record the epoch (UTC seconds) the RTC was just set to
  void setRtcRefEpoch(time_t epoch)
  {
    _preferences.putULong64(RTC_REF_EPOCH_KEY, static_cast<uint64_t>(epoch));
  }

  // set a setting by its name
  bool setSettingByName(const String &name, int value)
  {
    for (const auto &entry : _settings)
    {
      if (entry.second->getName() == name)
      {
        entry.second->set(value);
        return (true);
      }
    }
    return (false);
  }

private:
  Preferences _preferences;
  SETTINGSMAP _settings;

  // reads a single setting from the non-volatile storage
  int readSetting(String name, int defValue)
  {
    return (_preferences.getInt(name.c_str(), defValue));
  }

  // stores a single setting in the non-volatile storage
  void storeSetting(String name, int value)
  {
    _preferences.putInt(name.c_str(), value);
  }
};
