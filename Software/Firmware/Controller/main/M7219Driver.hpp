// M7219Driver.hpp

// minimalistic 7-segment driver for MAX7219
// with support for cascaded ICs

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>

// registers
constexpr uint8_t REG_NO_OP = 0x00;
constexpr uint8_t REG_DIGIT_0 = 0x01;
constexpr uint8_t REG_DIGIT_1 = 0x02;
constexpr uint8_t REG_DIGIT_2 = 0x03;
constexpr uint8_t REG_DIGIT_3 = 0x04;
constexpr uint8_t REG_DIGIT_4 = 0x05;
constexpr uint8_t REG_DIGIT_5 = 0x06;
constexpr uint8_t REG_DIGIT_6 = 0x07;
constexpr uint8_t REG_DIGIT_7 = 0x08;
constexpr uint8_t REG_DECODE_MODE = 0x09;
constexpr uint8_t REG_INTENSITY = 0x0a;
constexpr uint8_t REG_SCAN_LIMIT = 0x0b;
constexpr uint8_t REG_SHUTDOWN = 0x0c;
constexpr uint8_t REG_DISPLAY_TEST = 0x0f;

// digits per IC
constexpr uint8_t MAX_DIGITS = 8;

// positions of non-numeric chars in character map
constexpr uint8_t CHAR_MINUS = 10;
constexpr uint8_t CHAR_BLANK = 11;
constexpr uint8_t CHAR_E = 12;
constexpr uint8_t CHAR_r = 13;
constexpr uint8_t CHAR_o = 14;

// size of character map
constexpr uint8_t CHARMAP_SIZE = 15;

class M7219Driver
{
public:
  M7219Driver(uint8_t dataPin, uint8_t clockPin, uint8_t loadPin, uint8_t chainSize) : _dataPin(dataPin),
                                                                                       _clockPin(clockPin),
                                                                                       _loadPin(loadPin),
                                                                                       _chainSize(chainSize)
  {
    // define pin mode
    pinMode(_dataPin, OUTPUT);
    pinMode(_clockPin, OUTPUT);
    pinMode(_loadPin, OUTPUT);

    // set scan limit to max
    setDigitCount(MAX_DIGITS);

    // clear display
    blank();

    // put in shutdown state
    off();
  }

  virtual ~M7219Driver()
  {
  }

  // shutdown all
  void off()
  {
    for (uint8_t i = 0; i < _chainSize; i++)
    {
      off(i);
    }
  }

  // shutdown the specified IC in the chain
  void off(uint8_t index)
  {
    send(index, REG_SHUTDOWN, 0x00);
  }

  // turn on all
  void on()
  {
    for (uint8_t i = 0; i < _chainSize; i++)
    {
      on(i);
    }
  }

  // turn on the specified IC in the chain
  void on(uint8_t index)
  {
    send(index, REG_SHUTDOWN, 0x01);
  }

  // clear all
  void blank()
  {
    for (uint8_t i = 0; i < _chainSize; i++)
    {
      blank(i);
    }
  }

  // clear the specified IC in the chain
  void blank(uint8_t index)
  {
    for (uint8_t i = REG_DIGIT_0; i <= REG_DIGIT_7; i++)
    {
      send(index, i, 0x00);
    }
  }

  // set brightness for all ICs
  void setBrightness(uint8_t value)
  {
    for (uint8_t i = 0; i < _chainSize; i++)
    {
      setBrightness(i, value);
    }
  }

  // set brightness for the specified IC in the chain
  void setBrightness(uint8_t index, uint8_t value)
  {
    value &= 0x0f;
    send(index, REG_INTENSITY, value);
  }

  // set scan limit for all ICs
  void setDigitCount(uint8_t value)
  {
    if (value > 0 && value <= MAX_DIGITS)
    {
      for (uint8_t i = 0; i < _chainSize; i++)
      {
        setDigitCount(i, value);
      }
    }
  }

  // set scan limit for the specified IC in the chain
  void setDigitCount(uint8_t index, uint8_t digits)
  {
    if (digits > 0 && digits <= MAX_DIGITS)
    {
      send(index, REG_SCAN_LIMIT, digits - 1);
    }
  }

  // displays a character
  void setChar(uint8_t index, uint8_t pos, uint8_t value, bool decimalPoint)
  {
    if (pos < MAX_DIGITS)
    {
      if (value < CHARMAP_SIZE)
      {
        uint8_t c = _charMap[value];
        if (decimalPoint)
        {
          c |= 0x80;
        }
        send(index, pos + 1, c);
      }
    }
  }

private:
  // SPI pins
  uint8_t _dataPin;
  uint8_t _clockPin;
  uint8_t _loadPin;

  // number of chained ICs
  uint8_t _chainSize;

  // 7-segment mapping
  uint8_t _charMap[CHARMAP_SIZE] = {0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x7B, 0x01, 0x00, 0x4F, 0x05, 0x1D};

  // send data
  void send(uint8_t index, uint8_t reg, uint8_t value)
  {
    if (index < _chainSize)
    {
      digitalWrite(_loadPin, LOW);

      for (uint8_t i = _chainSize; i > 0; i--)
      {
        if (index == (i - 1))
        {
          shiftOut(_dataPin, _clockPin, MSBFIRST, reg);
          shiftOut(_dataPin, _clockPin, MSBFIRST, value);
        }
        else
        {
          shiftOut(_dataPin, _clockPin, MSBFIRST, REG_NO_OP);
          shiftOut(_dataPin, _clockPin, MSBFIRST, 0x00);
        }
      }
      digitalWrite(_loadPin, HIGH);
    }
  }
};
