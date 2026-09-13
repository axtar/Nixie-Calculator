// RTC.hpp

// thin C++ wrapper around the DS3231/DS3232 real-time clock

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <time.h>
#include <Timezone.hpp>

constexpr uint8_t TM_NBR_FIELDS = 7; // Second, Minute, Hour, Wday, Day, Month, Year

constexpr uint8_t RTC_I2C_ADDR = 0x68;
constexpr uint8_t REG_SECONDS = 0x00;
constexpr uint8_t REG_STATUS = 0x0F;
constexpr uint8_t REG_TEMP_MSB = 0x11;
constexpr uint8_t BIT_OSF = 7;     // oscillator stop flag, status register
constexpr uint8_t BIT_CH = 7;      // clock halt bit, seconds register
constexpr uint8_t BIT_HR1224 = 6;  // 12/24 hour mode bit, hours register
constexpr uint8_t BIT_CENTURY = 7; // century bit, month register

class RTC
{
public:
  // Wire is initialized (and shared) by the caller
  void begin()
  {
  }

  // read the current time from the RTC (the RTC stores UTC)
  time_t get()
  {
    struct tm tm;
    if (readTime(tm))
    {
      return (0);
    }
    return (timegmCompat(&tm));
  }

  // set the RTC to the given UTC time and clear the oscillator stop flag,
  uint8_t set(const time_t t)
  {
    struct tm tm;
    gmtime_r(&t, &tm);
    return (writeTime(tm));
  }

  // return the RTC temperature in degrees Celsius, times four
  int16_t temperature()
  {
    Wire.beginTransmission(RTC_I2C_ADDR);
    Wire.write(REG_TEMP_MSB);
    if (Wire.endTransmission())
    {
      return (0);
    }
    Wire.requestFrom(RTC_I2C_ADDR, (uint8_t)2);
    int8_t msb = (int8_t)Wire.read();
    uint8_t lsb = Wire.read();
    return ((int16_t)(((int16_t)msb << 8) | lsb) / 64);
  }

private:
  // read the current time into a struct tm, returns the I2C status
  uint8_t readTime(struct tm &tm)
  {
    Wire.beginTransmission(RTC_I2C_ADDR);
    Wire.write(REG_SECONDS);
    if (uint8_t e = Wire.endTransmission())
    {
      return (e);
    }
    Wire.requestFrom(RTC_I2C_ADDR, TM_NBR_FIELDS);
    tm.tm_sec = bcd2dec(Wire.read() & ~(1 << BIT_CH));
    tm.tm_min = bcd2dec(Wire.read());
    tm.tm_hour = bcd2dec(Wire.read() & ~(1 << BIT_HR1224)); // assumes 24 hour clock
    tm.tm_wday = Wire.read() - 1;                           // register is 1-7, struct tm wants 0-6
    tm.tm_mday = bcd2dec(Wire.read());
    tm.tm_mon = bcd2dec(Wire.read() & ~(1 << BIT_CENTURY)) - 1; // struct tm months are 0-based
    tm.tm_year = 100 + bcd2dec(Wire.read());                    // register is 2 digits, 2000-2099
    return (0);
  }

  // write a struct tm to the RTC and clear the oscillator stop flag,
  uint8_t writeTime(const struct tm &tm)
  {
    Wire.beginTransmission(RTC_I2C_ADDR);
    Wire.write(REG_SECONDS);
    Wire.write(dec2bcd(tm.tm_sec));
    Wire.write(dec2bcd(tm.tm_min));
    Wire.write(dec2bcd(tm.tm_hour)); // sets 24 hour format (bit 6 == 0)
    Wire.write(tm.tm_wday + 1);      // register is 1-7, struct tm gives 0-6
    Wire.write(dec2bcd(tm.tm_mday));
    Wire.write(dec2bcd(tm.tm_mon + 1));
    Wire.write(dec2bcd(tm.tm_year - 100));
    uint8_t ret = Wire.endTransmission();

    uint8_t status = readRegister(REG_STATUS);
    writeRegister(REG_STATUS, status & ~(1 << BIT_OSF));
    return (ret);
  }

  // read a single register, returns 0 if an I2C error occurred
  uint8_t readRegister(uint8_t addr)
  {
    Wire.beginTransmission(RTC_I2C_ADDR);
    Wire.write(addr);
    if (Wire.endTransmission())
    {
      return (0);
    }
    Wire.requestFrom(RTC_I2C_ADDR, (uint8_t)1);
    return (Wire.read());
  }

  // write a single register
  void writeRegister(uint8_t addr, uint8_t value)
  {
    Wire.beginTransmission(RTC_I2C_ADDR);
    Wire.write(addr);
    Wire.write(value);
    Wire.endTransmission();
  }

  // decimal to BCD conversion
  static uint8_t dec2bcd(uint8_t value)
  {
    return (value + 6 * (value / 10));
  }

  // BCD to decimal conversion
  static uint8_t bcd2dec(uint8_t value)
  {
    return (value - 6 * (value >> 4));
  }
};
