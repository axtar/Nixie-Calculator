// Settings.hpp

// stores and retrieves non-volatile settings

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

// Settings.h

#pragma once

#include <Arduino.h>
#include <Timezone.h>
#include <nvs_flash.h>
#include <Preferences.h>
#include <Setting.hpp>
#include <map>
#include <SettingsCache.hpp>
#include <Helper.hpp>

// definitions
constexpr auto SETTINGS_NAMESPACE = "CalcSettings";

typedef std::map<setting_id::setting_id, Setting *> SETTINGSMAP;

class Settings
{
public:
  Settings()
  {
    // create the setting objects,
    // max allowed name length is 15 characters
    _settings[setting_id::startupmode] = new Setting(setting_id::startupmode, "startupmode", setting_type::numeric, startup_mode::calculator, startup_mode::calculator, startup_mode::clock);
    _settings[setting_id::showversion] = new Setting(setting_id::showversion, "showversion", setting_type::numeric, show_version::on, show_version::off, show_version::on);
    _settings[setting_id::autooffmode] = new Setting(setting_id::autooffmode, "autooffmode", setting_type::numeric, auto_off_mode::clock, auto_off_mode::off, auto_off_mode::clock);
    _settings[setting_id::autooffdelay] = new Setting(setting_id::autooffdelay, "autooffdelay", setting_type::numeric, 5, 1, 720);
    _settings[setting_id::clockmode] = new Setting(setting_id::clockmode, "clockmode", setting_type::numeric, clock_mode::time, clock_mode::time, clock_mode::stopwatch);
    _settings[setting_id::hourmode] = new Setting(setting_id::hourmode, "hourmode", setting_type::numeric, hour_mode::h24, hour_mode::h12, hour_mode::h24);
    _settings[setting_id::leadingzero] = new Setting(setting_id::leadingzero, "hourmode", setting_type::numeric, leading_zero::on, leading_zero::off, leading_zero::on);
    _settings[setting_id::dateformat] = new Setting(setting_id::dateformat, "dateformat", setting_type::numeric, date_format::ddmmyy, date_format::ddmmyy, date_format::mmddyy);
    _settings[setting_id::pirmode] = new Setting(setting_id::pirmode, "pirmode", setting_type::numeric, pir_mode::off, pir_mode::off, pir_mode::on);
    _settings[setting_id::pirdelay] = new Setting(setting_id::pirdelay, "pirdelay", setting_type::numeric, 5, 1, 720);
    _settings[setting_id::gpsmode] = new Setting(setting_id::gpsmode, "gpsmode", setting_type::numeric, gps_mode::off, gps_mode::off, gps_mode::on);
    _settings[setting_id::gpsspeed] = new Setting(setting_id::gpsspeed, "gpsspeed", setting_type::numeric, gps_speed::br_38400, gps_speed::br_2400, gps_speed::br_115200);
    _settings[setting_id::gpssyncinterval] = new Setting(setting_id::gpssyncinterval, "gpssyncinterval", setting_type::numeric, 10, 1, 720);
    _settings[setting_id::temperaturemode] = new Setting(setting_id::temperaturemode, "temperaturemode", setting_type::numeric, temperature_mode::off, temperature_mode::off, temperature_mode::on);
    _settings[setting_id::temperaturecf] = new Setting(setting_id::temperaturecf, "temperaturecf", setting_type::numeric, temperature_cf::celsius, temperature_cf::celsius, temperature_cf::fahrenheit);
    _settings[setting_id::ledmode] = new Setting(setting_id::ledmode, "ledmode", setting_type::numeric, led_mode::always, led_mode::time, led_mode::always);
    _settings[setting_id::calcrgbmode] = new Setting(setting_id::calcrgbmode, "calcrgbmode", setting_type::numeric, calc_rgb_mode::off, calc_rgb_mode::off, calc_rgb_mode::fullrandom);
    _settings[setting_id::clockrgbmode] = new Setting(setting_id::clockrgbmode, "clockrgbmode", setting_type::numeric, clock_rgb_mode::off, clock_rgb_mode::off, clock_rgb_mode::fullrandom);
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
    _settings[setting_id::datecolor] = new Setting(setting_id::datecolor, "datecolor", setting_type::rgb, 0, 0, MAX_RGB_INT);
    _settings[setting_id::tempcolor] = new Setting(setting_id::tempcolor, "tempcolor", setting_type::rgb, 0, 0, MAX_RGB_INT);
    _settings[setting_id::fixedcolor] = new Setting(setting_id::fixedcolor, "fixedcolor", setting_type::rgb, 0, 0, MAX_RGB_INT);
    _settings[setting_id::dstweek] = new Setting(setting_id::dstweek, "dstweek", setting_type::numeric, week_t::Last, week_t::Last, week_t::Fourth);
    _settings[setting_id::dstdow] = new Setting(setting_id::dstdow, "dstdow", setting_type::numeric, dow_t::Sun, dow_t::Sun, dow_t::Sat);
    _settings[setting_id::dstmonth] = new Setting(setting_id::dstmonth, "dstmonth", setting_type::numeric, month_t::Mar, month_t::Jan, month_t::Dec);
    _settings[setting_id::dsthour] = new Setting(setting_id::dsthour, "dsthour", setting_type::numeric, 2, 0, 23);
    _settings[setting_id::dstoffset] = new Setting(setting_id::dstoffset, "dstoffset", setting_type::numeric, 120, -720, 840);
    _settings[setting_id::stdweek] = new Setting(setting_id::stdweek, "stdweek", setting_type::numeric, week_t::Last, week_t::Last, week_t::Fourth);
    _settings[setting_id::stddow] = new Setting(setting_id::stddow, "stddow", setting_type::numeric, dow_t::Sun, dow_t::Sun, dow_t::Sat);
    _settings[setting_id::stdmonth] = new Setting(setting_id::stdmonth, "stdmonth", setting_type::numeric, month_t::Oct, month_t::Jan, month_t::Dec);
    _settings[setting_id::stdhour] = new Setting(setting_id::stdhour, "stdhour", setting_type::numeric, 3, 0, 23);
    _settings[setting_id::stdoffset] = new Setting(setting_id::stdoffset, "stdoffset", setting_type::numeric, 60, -720, 840);
    _settings[setting_id::gpsnotifysync] = new Setting(setting_id::gpsnotifysync, "gpsnotifsync", setting_type::numeric, gps_notify_sync::off, gps_notify_sync::off, gps_notify_sync::on);
    _settings[setting_id::gpssynccolor] = new Setting(setting_id::gpssynccolor, "gpssynccolor", setting_type::rgb, Helper::rgbToInt(255, 0, 0), 0, MAX_RGB_INT);
    _settings[setting_id::notifytimer] = new Setting(setting_id::notifytimer, "notifytimer", setting_type::numeric, notify_timer::on, notify_timer::off, notify_timer::on);
    _settings[setting_id::timercolor] = new Setting(setting_id::timercolor, "timercolor", setting_type::rgb, Helper::rgbToInt(255, 255, 255), 0, MAX_RGB_INT);
    _settings[setting_id::fixeddecimals] = new Setting(setting_id::fixeddecimals, "fixeddecimals", setting_type::numeric, fixed_decimals::off, fixed_decimals::off, fixed_decimals::eight);
    _settings[setting_id::anglemode] = new Setting(setting_id::anglemode, "anglemode", setting_type::numeric, angle_mode::degrees, angle_mode::degrees, angle_mode::radians);
    _settings[setting_id::showbusycalc] = new Setting(setting_id::showbusycalc, "showbusycalc", setting_type::numeric, show_busy_calc::moving_decimal_separator, show_busy_calc::off, show_busy_calc::digit_flickering);
    _settings[setting_id::maxexpdigits] = new Setting(setting_id::maxexpdigits, "maxexpdigits", setting_type::numeric, 4, 2, 4);
    _settings[setting_id::scrolldelay] = new Setting(setting_id::scrolldelay, "scrolldelay", setting_type::numeric, 5, 1, 20);
    _settings[setting_id::calcprecision] = new Setting(setting_id::calcprecision, "calcprecision", setting_type::numeric, 32, 20, 32);
    _settings[setting_id::brightness] = new Setting(setting_id::brightness, "brightness", setting_type::numeric, 8, 1, 15);
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
  bool getSetting(setting_id::setting_id id, int *result)
  {
    *result = 0;
    bool success = false;
    SETTINGSMAP::iterator it;
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
    getSetting(setting_id::datecolor, &value);
    Helper::intToRGB(value, &SettingsCache::dateColor.red, &SettingsCache::dateColor.green, &SettingsCache::dateColor.blue);
    getSetting(setting_id::tempcolor, &value);
    Helper::intToRGB(value, &SettingsCache::tempColor.red, &SettingsCache::tempColor.green, &SettingsCache::tempColor.blue);
    getSetting(setting_id::fixedcolor, &value);
    Helper::intToRGB(value, &SettingsCache::fixedColor.red, &SettingsCache::fixedColor.green, &SettingsCache::fixedColor.blue);
    strcpy(SettingsCache::dstRule.abbrev, "DST");
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
    strcpy(SettingsCache::stdRule.abbrev, "STD");
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
    getSetting(setting_id::brightness, &SettingsCache::brightness);
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
  int getSettingMax(setting_id::setting_id id)
  {
    SETTINGSMAP::iterator it;
    it = _settings.find(id);
    if (it != _settings.end())
    {
      return (it->second->getMax());
    }
    return (0);
  }

  // get min value of a setting
  int getSettingMin(setting_id::setting_id id)
  {
    SETTINGSMAP::iterator it;
    it = _settings.find(id);
    if (it != _settings.end())
    {
      return (it->second->getMin());
    }
    return (0);
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
