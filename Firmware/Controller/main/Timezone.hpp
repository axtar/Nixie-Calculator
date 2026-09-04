// Timezone.hpp

// converts between UTC and local time given a pair of DST/STD timezone rules

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

#include <time.h>
#include <cstdint>

// no valid calendar year is negative
constexpr int INVALID_YEAR = -1;

// Howard Hinnant's days_from_civil algorithm, see:
// https://howardhinnant.github.io/date_algorithms.html#days_from_civil
inline time_t timegmCompat(const struct tm *t)
{
  int y = t->tm_year + 1900;
  int m = t->tm_mon + 1; // 1-12
  int d = t->tm_mday;
  y -= m <= 2;
  long era = (y >= 0 ? y : y - 399) / 400;
  unsigned yoe = static_cast<unsigned>(y - era * 400);
  unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
  unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  long days = era * 146097 + static_cast<long>(doe) - 719468;

  return static_cast<time_t>(days) * 86400 + t->tm_hour * 3600 + t->tm_min * 60 + t->tm_sec;
}

// matches the TZ environment variable's rule format
enum class week_t : int8_t
{
  First = 1,
  Second,
  Third,
  Fourth,
  Last
};

// matches struct tm's tm_wday (0=Sunday..6=Saturday)
enum class dow_t : int8_t
{
  Sun,
  Mon,
  Tue,
  Wed,
  Thu,
  Fri,
  Sat
};

// matches struct tm's tm_mon (0=January..11=December)
enum class month_t : int8_t
{
  Jan,
  Feb,
  Mar,
  Apr,
  May,
  Jun,
  Jul,
  Aug,
  Sep,
  Oct,
  Nov,
  Dec
};

// describes when a DST or standard time change takes effect
struct timezoneRule
{
  int8_t week;
  int8_t dow;
  int8_t month;
  int8_t hour;
  int offset;

  timezoneRule() = default;

  timezoneRule(int8_t week, int8_t dow, int8_t month, int8_t hour, int offset)
      : week(week), dow(dow), month(month), hour(hour), offset(offset)
  {
  }
};

class Timezone
{
public:
  // constructs a timezone from its DST and standard time change rules
  Timezone(timezoneRule dstRule, timezoneRule stdRule) : m_dst(dstRule), m_std(stdRule)
  {
  }

  // replaces the DST and standard time change rules
  void defineRules(timezoneRule dstRule, timezoneRule stdRule)
  {
    m_dst = dstRule;
    m_std = stdRule;
    m_cachedYear = INVALID_YEAR; // force the transition instants to be recomputed
  }

  // converts a UTC time_t to local time
  time_t toLocal(time_t utc) const
  {
    return utc + (isDST(utc, true) ? m_dst.offset : m_std.offset) * 60;
  }

  // converts a local time_t to UTC
  time_t toUTC(time_t local) const
  {
    return local - (isDST(local, false) ? m_dst.offset : m_std.offset) * 60;
  }

private:
  timezoneRule m_dst;
  timezoneRule m_std;
  mutable int m_cachedYear = INVALID_YEAR;
  mutable time_t m_dstWall = 0;
  mutable time_t m_stdWall = 0;

  // recompute the cached transition instants if the year has changed
  void ensureCached(int year) const
  {
    if (year != m_cachedYear)
    {
      m_dstWall = wallClockInstant(m_dst, year);
      m_stdWall = wallClockInstant(m_std, year);
      m_cachedYear = year;
    }
  }

  // true if the given calendar year is a leap year
  static bool isLeapYear(int year)
  {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
  }

  // the number of days in the given month (0=Jan..11=Dec) of the given year
  static int daysInMonth(int year, int month)
  {
    static constexpr uint8_t days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return (month == static_cast<int>(month_t::Feb) && isLeapYear(year)) ? 29 : days[month];
  }

  // 0=Sunday..6=Saturday, for the given civil date
  static int weekdayOf(int year, int month, int day)
  {
    struct tm t = {};
    t.tm_year = year - 1900;
    t.tm_mon = month;
    t.tm_mday = day;
    time_t instant = timegmCompat(&t);
    struct tm r;
    gmtime_r(&instant, &r);
    return r.tm_wday;
  }

  // the day-of-month on which the rule's weekday occurs, for the
  // requested occurrence (First..Fourth, or the last one in the month)
  static int ruleDayOfMonth(const timezoneRule &r, int year)
  {
    int last = daysInMonth(year, r.month);

    if (r.week == static_cast<int8_t>(week_t::Last))
    {
      for (int day = last; day > last - 7; day--)
      {
        if (weekdayOf(year, r.month, day) == r.dow)
        {
          return day;
        }
      }
    }
    else
    {
      int occurrence = 0;
      for (int day = 1; day <= last; day++)
      {
        if (weekdayOf(year, r.month, day) == r.dow && ++occurrence == r.week)
        {
          return day;
        }
      }
    }
    return 1; // unreachable for a valid rule
  }

  // the transition instant read on the wall clock
  static time_t wallClockInstant(const timezoneRule &r, int year)
  {
    struct tm t = {};
    t.tm_year = year - 1900;
    t.tm_mon = r.month;
    t.tm_mday = ruleDayOfMonth(r, year);
    t.tm_hour = r.hour;
    return timegmCompat(&t);
  }

  // instant is a UTC time_t if isUtc is true, or a local time_t otherwise
  bool isDST(time_t instant, bool isUtc) const
  {
    if (m_dst.offset == m_std.offset)
    {
      return false;
    }

    struct tm t;
    gmtime_r(&instant, &t);
    int year = t.tm_year + 1900;
    ensureCached(year);

    time_t dstTime = m_dstWall;
    time_t stdTime = m_stdWall;
    if (isUtc)
    {
      dstTime -= m_std.offset * 60;
      stdTime -= m_dst.offset * 60;
    }

    if (dstTime < stdTime)
    {
      // DST falls inside the calendar year (northern hemisphere)
      return (instant >= dstTime && instant < stdTime);
    }
    // DST extends across the year boundary (southern hemisphere)
    return !(instant >= stdTime && instant < dstTime);
  }
};
