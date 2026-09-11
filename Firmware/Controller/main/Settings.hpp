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
#include <type_traits>
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

// selects how a setting's raw int value maps to/from its SettingsCache field
enum class cached_as
{
  numeric, // plain int/int8_t and enum-typed fields
  ms,      // minutes-setting mirrored as milliseconds in the cache
  time,    // time setting mirrored as STARTTIME field in the cache
  rgb      // RGB setting mirrored as RGBCOLOR field in the cache
};

class Settings
{
public:
  Settings()
  {
    // adding a new setting requires touching all of the following:
    //  - add the id to the setting_id enum in SettingDefs.h
    //  - add a matching field to SettingsCache.hpp
    //  - register it below with a new Setting(...)
    //  - (if user-editable) add a row for it in include/HTMLConfig.h
    //  - document the new setting in the user manuals

    // create the setting objects,
    // max allowed name length is 15 characters

    // clang-format off
    _settings[setting_id::startupmode] = new Setting(setting_id::startupmode, "startupmode",
        setting_type::numeric, startup_mode::calculator, startup_mode::calculator, startup_mode::clock,
        syncToCache<&SettingsCache::startupMode>, syncFromCache<&SettingsCache::startupMode>);

    _settings[setting_id::showversion] = new Setting(setting_id::showversion, "showversion",
        setting_type::numeric, show_version::on, show_version::off, show_version::on,
        syncToCache<&SettingsCache::showVersion>, syncFromCache<&SettingsCache::showVersion>);

    _settings[setting_id::autooffmode] = new Setting(setting_id::autooffmode, "autooffmode",
        setting_type::numeric, auto_off_mode::clock, auto_off_mode::off, auto_off_mode::clock,
        syncToCache<&SettingsCache::autoOffMode>, syncFromCache<&SettingsCache::autoOffMode>);

    _settings[setting_id::autooffdelay] = new Setting(setting_id::autooffdelay, "autooffdelay",
        setting_type::numeric, 5, 1, 720,
        syncToCache<&SettingsCache::autoOffDelay, cached_as::ms>, syncFromCache<&SettingsCache::autoOffDelay, cached_as::ms>);

    _settings[setting_id::clockmode] = new Setting(setting_id::clockmode, "clockmode",
        setting_type::numeric, clock_mode::time, clock_mode::time, clock_mode::stopwatch,
        syncToCache<&SettingsCache::clockMode>, syncFromCache<&SettingsCache::clockMode>);

    _settings[setting_id::hourmode] = new Setting(setting_id::hourmode, "hourmode",
        setting_type::numeric, hour_mode::h24, hour_mode::h12, hour_mode::h24,
        syncToCache<&SettingsCache::hourMode>, syncFromCache<&SettingsCache::hourMode>);

    _settings[setting_id::leadingzero] = new Setting(setting_id::leadingzero, "leadingzero",
        setting_type::numeric, leading_zero::on, leading_zero::off, leading_zero::on,
        syncToCache<&SettingsCache::leadingZero>, syncFromCache<&SettingsCache::leadingZero>);

    _settings[setting_id::timeseparator] = new Setting(setting_id::timeseparator, "timeseparator",
        setting_type::numeric, time_separator::blink, time_separator::off, time_separator::on,
        syncToCache<&SettingsCache::timeSeparator>, syncFromCache<&SettingsCache::timeSeparator>);

    _settings[setting_id::dateformat] = new Setting(setting_id::dateformat, "dateformat",
        setting_type::numeric, date_format::ddmmyy, date_format::ddmmyy, date_format::yyddmm,
        syncToCache<&SettingsCache::dateFormat>, syncFromCache<&SettingsCache::dateFormat>);

    _settings[setting_id::pirmode] = new Setting(setting_id::pirmode, "pirmode",
        setting_type::numeric, pir_mode::off, pir_mode::off, pir_mode::on,
        syncToCache<&SettingsCache::pirMode>, syncFromCache<&SettingsCache::pirMode>);

    _settings[setting_id::pirdelay] = new Setting(setting_id::pirdelay, "pirdelay",
        setting_type::numeric, 5, 1, 720,
        syncToCache<&SettingsCache::pirDelay, cached_as::ms>, syncFromCache<&SettingsCache::pirDelay, cached_as::ms>);

    _settings[setting_id::gpsmode] = new Setting(setting_id::gpsmode, "gpsmode",
        setting_type::numeric, gps_mode::off, gps_mode::off, gps_mode::on,
        syncToCache<&SettingsCache::gpsMode>, syncFromCache<&SettingsCache::gpsMode>);

    _settings[setting_id::gpsspeed] = new Setting(setting_id::gpsspeed, "gpsspeed",
        setting_type::numeric, gps_speed::br_38400, gps_speed::br_2400, gps_speed::br_115200,
        syncToCache<&SettingsCache::gpsSpeed>, syncFromCache<&SettingsCache::gpsSpeed>);

    _settings[setting_id::gpssyncinterval] = new Setting(setting_id::gpssyncinterval, "gpssyncinterval",
        setting_type::numeric, 10, 1, 720,
        syncToCache<&SettingsCache::gpsSyncInterval, cached_as::ms>, syncFromCache<&SettingsCache::gpsSyncInterval, cached_as::ms>);

    _settings[setting_id::temperaturemode] = new Setting(setting_id::temperaturemode, "temperaturemode",
        setting_type::numeric, temperature_mode::off, temperature_mode::off, temperature_mode::on,
        syncToCache<&SettingsCache::temperatureMode>, syncFromCache<&SettingsCache::temperatureMode>);

    _settings[setting_id::temperaturecf] = new Setting(setting_id::temperaturecf, "temperaturecf",
        setting_type::numeric, temperature_cf::celsius, temperature_cf::celsius, temperature_cf::fahrenheit,
        syncToCache<&SettingsCache::temperatureCF>, syncFromCache<&SettingsCache::temperatureCF>);

    _settings[setting_id::ledmode] = new Setting(setting_id::ledmode, "ledmode",
        setting_type::numeric, led_mode::always, led_mode::time, led_mode::always,
        syncToCache<&SettingsCache::ledMode>, syncFromCache<&SettingsCache::ledMode>);

    _settings[setting_id::calcrgbmode] = new Setting(setting_id::calcrgbmode, "calcrgbmode",
        setting_type::numeric, calc_rgb_mode::off, calc_rgb_mode::off, calc_rgb_mode::rainbow_all,
        syncToCache<&SettingsCache::calcRGBMode>, syncFromCache<&SettingsCache::calcRGBMode>);

    _settings[setting_id::clockrgbmode] = new Setting(setting_id::clockrgbmode, "clockrgbmode",
        setting_type::numeric, clock_rgb_mode::off, clock_rgb_mode::off, clock_rgb_mode::rainbow_all,
        syncToCache<&SettingsCache::clockRGBMode>, syncFromCache<&SettingsCache::clockRGBMode>);

    _settings[setting_id::trigcolorchange] = new Setting(setting_id::trigcolorchange, "trigcolorchange",
        setting_type::numeric, trig_color_change::off, trig_color_change::off, trig_color_change::every_hour,
        syncToCache<&SettingsCache::trigColorChange>, syncFromCache<&SettingsCache::trigColorChange>);

    _settings[setting_id::ledstarttime] = new Setting(setting_id::ledstarttime, "ledstarttime",
        setting_type::time, 0, 0, MAX_TIME_INT,
        syncToCache<&SettingsCache::ledStartTime, cached_as::time>, syncFromCache<&SettingsCache::ledStartTime, cached_as::time>);

    _settings[setting_id::ledduration] = new Setting(setting_id::ledduration, "ledduration",
        setting_type::numeric, 0, 0, 720,
        syncToCache<&SettingsCache::ledDuration>, syncFromCache<&SettingsCache::ledDuration>);

    _settings[setting_id::ledstarttime2] = new Setting(setting_id::ledstarttime2, "ledstarttime2",
        setting_type::time, 0, 0, MAX_TIME_INT,
        syncToCache<&SettingsCache::ledStartTime2, cached_as::time>, syncFromCache<&SettingsCache::ledStartTime2, cached_as::time>);

    _settings[setting_id::ledduration2] = new Setting(setting_id::ledduration2, "ledduration2",
        setting_type::numeric, 0, 0, 720,
        syncToCache<&SettingsCache::ledDuration2>, syncFromCache<&SettingsCache::ledDuration2>);

    _settings[setting_id::acpstarttime] = new Setting(setting_id::acpstarttime, "acpstarttime",
        setting_type::time, 0, 0, MAX_TIME_INT,
        syncToCache<&SettingsCache::acpStartTime, cached_as::time>, syncFromCache<&SettingsCache::acpStartTime, cached_as::time>);

    _settings[setting_id::acpduration] = new Setting(setting_id::acpduration, "acpduration",
        setting_type::numeric, 0, 0, 720,
        syncToCache<&SettingsCache::acpDuration>, syncFromCache<&SettingsCache::acpDuration>);

    _settings[setting_id::acpforceon] = new Setting(setting_id::acpforceon, "acpforceon",
        setting_type::numeric, acp_force_on::on, acp_force_on::off, acp_force_on::on,
        syncToCache<&SettingsCache::acpForceOn>, syncFromCache<&SettingsCache::acpForceOn>);

    _settings[setting_id::negativecolor] = new Setting(setting_id::negativecolor, "negativecolor",
        setting_type::rgb, 0, 0, MAX_RGB_INT,
        syncToCache<&SettingsCache::negativeColor, cached_as::rgb>, syncFromCache<&SettingsCache::negativeColor, cached_as::rgb>);

    _settings[setting_id::positivecolor] = new Setting(setting_id::positivecolor, "positivecolor",
        setting_type::rgb, 0, 0, MAX_RGB_INT,
        syncToCache<&SettingsCache::positiveColor, cached_as::rgb>, syncFromCache<&SettingsCache::positiveColor, cached_as::rgb>);

    _settings[setting_id::errorcolor] = new Setting(setting_id::errorcolor, "errorcolor",
        setting_type::rgb, 0, 0, MAX_RGB_INT,
        syncToCache<&SettingsCache::errorColor, cached_as::rgb>, syncFromCache<&SettingsCache::errorColor, cached_as::rgb>);

    _settings[setting_id::negexpcolor] = new Setting(setting_id::negexpcolor, "negexpcolor",
        setting_type::rgb, 0, 0, MAX_RGB_INT,
        syncToCache<&SettingsCache::negExpColor, cached_as::rgb>, syncFromCache<&SettingsCache::negExpColor, cached_as::rgb>);

    _settings[setting_id::posexpcolor] = new Setting(setting_id::posexpcolor, "posexpcolor",
        setting_type::rgb, 0, 0, MAX_RGB_INT,
        syncToCache<&SettingsCache::posExpColor, cached_as::rgb>, syncFromCache<&SettingsCache::posExpColor, cached_as::rgb>);

    _settings[setting_id::fixedcalccolor] = new Setting(setting_id::fixedcalccolor, "fixedcalccolor",
        setting_type::rgb, 0, 0, MAX_RGB_INT,
        syncToCache<&SettingsCache::fixedCalcColor, cached_as::rgb>, syncFromCache<&SettingsCache::fixedCalcColor, cached_as::rgb>);

    _settings[setting_id::timecolor] = new Setting(setting_id::timecolor, "timecolor",
        setting_type::rgb, 0, 0, MAX_RGB_INT,
        syncToCache<&SettingsCache::timeColor, cached_as::rgb>, syncFromCache<&SettingsCache::timeColor, cached_as::rgb>);

    _settings[setting_id::timecolor2] = new Setting(setting_id::timecolor2, "timecolor2",
        setting_type::rgb, 0, 0, MAX_RGB_INT,
        syncToCache<&SettingsCache::timeColor2, cached_as::rgb>, syncFromCache<&SettingsCache::timeColor2, cached_as::rgb>);

    _settings[setting_id::datecolor] = new Setting(setting_id::datecolor, "datecolor",
        setting_type::rgb, 0, 0, MAX_RGB_INT,
        syncToCache<&SettingsCache::dateColor, cached_as::rgb>, syncFromCache<&SettingsCache::dateColor, cached_as::rgb>);

    _settings[setting_id::tempcolor] = new Setting(setting_id::tempcolor, "tempcolor",
        setting_type::rgb, 0, 0, MAX_RGB_INT,
        syncToCache<&SettingsCache::tempColor, cached_as::rgb>, syncFromCache<&SettingsCache::tempColor, cached_as::rgb>);

    _settings[setting_id::fixedcolor] = new Setting(setting_id::fixedcolor, "fixedcolor",
        setting_type::rgb, 0, 0, MAX_RGB_INT,
        syncToCache<&SettingsCache::fixedColor, cached_as::rgb>, syncFromCache<&SettingsCache::fixedColor, cached_as::rgb>);

    _settings[setting_id::dstweek] = new Setting(setting_id::dstweek, "dstweek",
        setting_type::numeric, static_cast<int>(week_t::Last), static_cast<int>(week_t::First), static_cast<int>(week_t::Last),
        syncToCache<&SettingsCache::dstRule.week>, syncFromCache<&SettingsCache::dstRule.week>);

    _settings[setting_id::dstdow] = new Setting(setting_id::dstdow, "dstdow",
        setting_type::numeric, static_cast<int>(dow_t::Sun), static_cast<int>(dow_t::Sun), static_cast<int>(dow_t::Sat),
        syncToCache<&SettingsCache::dstRule.dow>, syncFromCache<&SettingsCache::dstRule.dow>);

    _settings[setting_id::dstmonth] = new Setting(setting_id::dstmonth, "dstmonth",
        setting_type::numeric, static_cast<int>(month_t::Mar), static_cast<int>(month_t::Jan), static_cast<int>(month_t::Dec),
        syncToCache<&SettingsCache::dstRule.month>, syncFromCache<&SettingsCache::dstRule.month>);

    _settings[setting_id::dsthour] = new Setting(setting_id::dsthour, "dsthour",
        setting_type::numeric, 2, 0, 23,
        syncToCache<&SettingsCache::dstRule.hour>, syncFromCache<&SettingsCache::dstRule.hour>);

    _settings[setting_id::dstoffset] = new Setting(setting_id::dstoffset, "dstoffset",
        setting_type::numeric, 120, -720, 840,
        syncToCache<&SettingsCache::dstRule.offset>, syncFromCache<&SettingsCache::dstRule.offset>);

    _settings[setting_id::stdweek] = new Setting(setting_id::stdweek, "stdweek",
        setting_type::numeric, static_cast<int>(week_t::Last), static_cast<int>(week_t::First), static_cast<int>(week_t::Last),
        syncToCache<&SettingsCache::stdRule.week>, syncFromCache<&SettingsCache::stdRule.week>);

    _settings[setting_id::stddow] = new Setting(setting_id::stddow, "stddow",
        setting_type::numeric, static_cast<int>(dow_t::Sun), static_cast<int>(dow_t::Sun), static_cast<int>(dow_t::Sat),
        syncToCache<&SettingsCache::stdRule.dow>, syncFromCache<&SettingsCache::stdRule.dow>);

    _settings[setting_id::stdmonth] = new Setting(setting_id::stdmonth, "stdmonth",
        setting_type::numeric, static_cast<int>(month_t::Oct), static_cast<int>(month_t::Jan), static_cast<int>(month_t::Dec),
        syncToCache<&SettingsCache::stdRule.month>, syncFromCache<&SettingsCache::stdRule.month>);

    _settings[setting_id::stdhour] = new Setting(setting_id::stdhour, "stdhour",
        setting_type::numeric, 3, 0, 23,
        syncToCache<&SettingsCache::stdRule.hour>, syncFromCache<&SettingsCache::stdRule.hour>);

    _settings[setting_id::stdoffset] = new Setting(setting_id::stdoffset, "stdoffset",
        setting_type::numeric, 60, -720, 840,
        syncToCache<&SettingsCache::stdRule.offset>, syncFromCache<&SettingsCache::stdRule.offset>);

    _settings[setting_id::dstweek2] = new Setting(setting_id::dstweek2, "dstweek2",
        setting_type::numeric, static_cast<int>(week_t::Last), static_cast<int>(week_t::First), static_cast<int>(week_t::Last),
        syncToCache<&SettingsCache::dstRule2.week>, syncFromCache<&SettingsCache::dstRule2.week>);

    _settings[setting_id::dstdow2] = new Setting(setting_id::dstdow2, "dstdow2",
        setting_type::numeric, static_cast<int>(dow_t::Sun), static_cast<int>(dow_t::Sun), static_cast<int>(dow_t::Sat),
        syncToCache<&SettingsCache::dstRule2.dow>, syncFromCache<&SettingsCache::dstRule2.dow>);

    _settings[setting_id::dstmonth2] = new Setting(setting_id::dstmonth2, "dstmonth2",
        setting_type::numeric, static_cast<int>(month_t::Mar), static_cast<int>(month_t::Jan), static_cast<int>(month_t::Dec),
        syncToCache<&SettingsCache::dstRule2.month>, syncFromCache<&SettingsCache::dstRule2.month>);

    _settings[setting_id::dsthour2] = new Setting(setting_id::dsthour2, "dsthour2",
        setting_type::numeric, 2, 0, 23,
        syncToCache<&SettingsCache::dstRule2.hour>, syncFromCache<&SettingsCache::dstRule2.hour>);

    _settings[setting_id::dstoffset2] = new Setting(setting_id::dstoffset2, "dstoffset2",
        setting_type::numeric, 0, -720, 840,
        syncToCache<&SettingsCache::dstRule2.offset>, syncFromCache<&SettingsCache::dstRule2.offset>);

    _settings[setting_id::stdweek2] = new Setting(setting_id::stdweek2, "stdweek2",
        setting_type::numeric, static_cast<int>(week_t::Last), static_cast<int>(week_t::First), static_cast<int>(week_t::Last),
        syncToCache<&SettingsCache::stdRule2.week>, syncFromCache<&SettingsCache::stdRule2.week>);

    _settings[setting_id::stddow2] = new Setting(setting_id::stddow2, "stddow2",
        setting_type::numeric, static_cast<int>(dow_t::Sun), static_cast<int>(dow_t::Sun), static_cast<int>(dow_t::Sat),
        syncToCache<&SettingsCache::stdRule2.dow>, syncFromCache<&SettingsCache::stdRule2.dow>);

    _settings[setting_id::stdmonth2] = new Setting(setting_id::stdmonth2, "stdmonth2",
        setting_type::numeric, static_cast<int>(month_t::Oct), static_cast<int>(month_t::Jan), static_cast<int>(month_t::Dec),
        syncToCache<&SettingsCache::stdRule2.month>, syncFromCache<&SettingsCache::stdRule2.month>);

    _settings[setting_id::stdhour2] = new Setting(setting_id::stdhour2, "stdhour2",
        setting_type::numeric, 3, 0, 23,
        syncToCache<&SettingsCache::stdRule2.hour>, syncFromCache<&SettingsCache::stdRule2.hour>);

    _settings[setting_id::stdoffset2] = new Setting(setting_id::stdoffset2, "stdoffset2",
        setting_type::numeric, 0, -720, 840,
        syncToCache<&SettingsCache::stdRule2.offset>, syncFromCache<&SettingsCache::stdRule2.offset>);

    _settings[setting_id::gpsnotifysync] = new Setting(setting_id::gpsnotifysync, "gpsnotifysync",
        setting_type::numeric, gps_notify_sync::off, gps_notify_sync::off, gps_notify_sync::on,
        syncToCache<&SettingsCache::gpsNotifySync>, syncFromCache<&SettingsCache::gpsNotifySync>);

    _settings[setting_id::gpssynccolor] = new Setting(setting_id::gpssynccolor, "gpssynccolor",
        setting_type::rgb, Helper::rgbToInt(255, 0, 0), 0, MAX_RGB_INT,
        syncToCache<&SettingsCache::gpsSyncColor, cached_as::rgb>, syncFromCache<&SettingsCache::gpsSyncColor, cached_as::rgb>);

    _settings[setting_id::notifytimer] = new Setting(setting_id::notifytimer, "notifytimer",
        setting_type::numeric, notify_timer::on, notify_timer::off, notify_timer::on,
        syncToCache<&SettingsCache::notifyTimer>, syncFromCache<&SettingsCache::notifyTimer>);

    _settings[setting_id::timercolor] = new Setting(setting_id::timercolor, "timercolor",
        setting_type::rgb, Helper::rgbToInt(255, 255, 255), 0, MAX_RGB_INT,
        syncToCache<&SettingsCache::timerColor, cached_as::rgb>, syncFromCache<&SettingsCache::timerColor, cached_as::rgb>);

    _settings[setting_id::fixeddecimals] = new Setting(setting_id::fixeddecimals, "fixeddecimals",
        setting_type::numeric, fixed_decimals::off, fixed_decimals::off, fixed_decimals::eight,
        syncToCache<&SettingsCache::fixedDecimals>, syncFromCache<&SettingsCache::fixedDecimals>);

    _settings[setting_id::anglemode] = new Setting(setting_id::anglemode, "anglemode",
        setting_type::numeric, angle_mode::degrees, angle_mode::degrees, angle_mode::radians,
        syncToCache<&SettingsCache::angleMode>, syncFromCache<&SettingsCache::angleMode>);

    _settings[setting_id::showbusycalc] = new Setting(setting_id::showbusycalc, "showbusycalc",
        setting_type::numeric, show_busy_calc::moving_decimal_separator, show_busy_calc::off, show_busy_calc::digit_flickering,
        syncToCache<&SettingsCache::showBusyCalc>, syncFromCache<&SettingsCache::showBusyCalc>);

    _settings[setting_id::maxexpdigits] = new Setting(setting_id::maxexpdigits, "maxexpdigits",
        setting_type::numeric, 4, 2, 4,
        syncToCache<&SettingsCache::maxExpDigits>, syncFromCache<&SettingsCache::maxExpDigits>);

    _settings[setting_id::scrolldelay] = new Setting(setting_id::scrolldelay, "scrolldelay",
        setting_type::numeric, 5, 1, 20,
        syncToCache<&SettingsCache::scrollDelay>, syncFromCache<&SettingsCache::scrollDelay>);

    _settings[setting_id::calcprecision] = new Setting(setting_id::calcprecision, "calcprecision",
        setting_type::numeric, 32, 20, 32,
        syncToCache<&SettingsCache::calcPrecision>, syncFromCache<&SettingsCache::calcPrecision>);

    _settings[setting_id::calcinputdirec] = new Setting(setting_id::calcinputdirec, "calcinputdirec",
        setting_type::numeric, calc_input_direc::ltr, calc_input_direc::ltr, calc_input_direc::rtl_zero_padding,
        syncToCache<&SettingsCache::calcInputDirec>, syncFromCache<&SettingsCache::calcInputDirec>);

    _settings[setting_id::inputblinking] = new Setting(setting_id::inputblinking, "inputblinking",
        setting_type::numeric, input_blinking::on, input_blinking::off, input_blinking::on,
        syncToCache<&SettingsCache::inputBlinking>, syncFromCache<&SettingsCache::inputBlinking>);

    _settings[setting_id::brightness] = new Setting(setting_id::brightness, "brightness",
        setting_type::numeric, 15, 1, 15,
        syncToCache<&SettingsCache::brightness>, syncFromCache<&SettingsCache::brightness>);

    _settings[setting_id::dimbrightness] = new Setting(setting_id::dimbrightness, "dimbrightness",
        setting_type::numeric, 1, 1, 15,
        syncToCache<&SettingsCache::dimBrightness>, syncFromCache<&SettingsCache::dimBrightness>);

    _settings[setting_id::dimstarttime] = new Setting(setting_id::dimstarttime, "dimstarttime",
        setting_type::time, 0, 0, MAX_TIME_INT,
        syncToCache<&SettingsCache::dimStartTime, cached_as::time>, syncFromCache<&SettingsCache::dimStartTime, cached_as::time>);

    _settings[setting_id::dimduration] = new Setting(setting_id::dimduration, "dimduration",
        setting_type::numeric, 0, 0, 720,
        syncToCache<&SettingsCache::dimDuration>, syncFromCache<&SettingsCache::dimDuration>);

    _settings[setting_id::apautostart] = new Setting(setting_id::apautostart, "apautostart",
        setting_type::numeric, ap_auto_start::off, ap_auto_start::off, ap_auto_start::on,
        syncToCache<&SettingsCache::apAutoStart>, syncFromCache<&SettingsCache::apAutoStart>);

    _settings[setting_id::rtcdriftcorr] = new Setting(setting_id::rtcdriftcorr, "rtcdriftcorr",
        setting_type::numeric, 0, -60, 60,
        syncToCache<&SettingsCache::rtcDriftCorr>, syncFromCache<&SettingsCache::rtcDriftCorr>);

    _settings[setting_id::exttempcorr] = new Setting(setting_id::exttempcorr, "exttempcorr",
        setting_type::numeric, 0, -100, 100,
        syncToCache<&SettingsCache::extTempCorr>, syncFromCache<&SettingsCache::extTempCorr>);

    _settings[setting_id::breathingmode] = new Setting(setting_id::breathingmode, "breathingmode",
        setting_type::numeric, breathing_mode::off, breathing_mode::off, breathing_mode::on,
        syncToCache<&SettingsCache::breathingMode>, syncFromCache<&SettingsCache::breathingMode>);

    _settings[setting_id::roundingmode] = new Setting(setting_id::roundingmode, "roundingmode",
        setting_type::numeric, rounding_mode::five_four, rounding_mode::five_four, rounding_mode::cut,
        syncToCache<&SettingsCache::roundingMode>, syncFromCache<&SettingsCache::roundingMode>);
    // clang-format on
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

  // update the settings cache from the current setting values
  void updateCache()
  {
    for (const auto &value : _settings)
    {
      value.second->toCache();
    }
  }

  // write the current values held in the settings cache back into the settings
  void updateFromCache()
  {
    for (const auto &value : _settings)
    {
      value.second->fromCache();
    }
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

  // toCache/fromCache helpers for the Setting(...) registrations above
  template <auto Field, cached_as Kind = cached_as::numeric>
  static void syncToCache(Setting *s)
  {
    if constexpr (Kind == cached_as::ms)
    {
      *Field = s->get() * 60 * 1000;
    }
    else if constexpr (Kind == cached_as::time)
    {
      Helper::intToTime(s->get(), &Field->hour, &Field->minute);
    }
    else if constexpr (Kind == cached_as::rgb)
    {
      Helper::intToRGB(s->get(), &Field->red, &Field->green, &Field->blue);
    }
    else
    {
      *Field = static_cast<std::remove_reference_t<decltype(*Field)>>(s->get());
    }
  }

  template <auto Field, cached_as Kind = cached_as::numeric>
  static void syncFromCache(Setting *s)
  {
    if constexpr (Kind == cached_as::ms)
    {
      s->set(static_cast<int>(*Field / 60000));
    }
    else if constexpr (Kind == cached_as::time)
    {
      s->set(Helper::timeToInt(Field->hour, Field->minute));
    }
    else if constexpr (Kind == cached_as::rgb)
    {
      s->set(Helper::rgbToInt(Field->red, Field->green, Field->blue));
    }
    else
    {
      s->set(static_cast<int>(*Field));
    }
  }

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
