// DisplayHAL.hpp

// provides an interface to the different
// hardware versions of the display
// and driver boards.

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>

// register type definition, used in translation table
enum class register_type : uint8_t
{
  unknown,
  not_connected,
  not_used,
  decimal_separator,
  number,
  minus_sign,
  exp_minus_sign,
  plus_sign,
  exp_plus_sign,
  menu_sign,
  special_char
};

// type of LEDs, smd or through-hole
enum class led_type : uint8_t
{
  none,
  smd,
  tht
};

// decimal separator position
enum class decimal_separator_position
{
  left,
  right
};

// element in translation table
typedef struct
{
  register_type rt;
  uint8_t digit;
  uint8_t number;
} TRANSLATION_TABLE_ENTRY;

typedef struct
{
  uint8_t index;
  uint8_t pos;
} DIGIT_ADDRESS;

class DisplayHAL
{
public:
  DisplayHAL()
  {
  }

  virtual ~DisplayHAL()
  {
  }

  // HAL interface
  virtual register_type getRegisterInfo(uint8_t registerNumber, uint8_t *digit, uint8_t *number) const = 0;
  virtual void getDigitAddress(uint8_t digit, uint8_t *index, uint8_t *position) const = 0;
  virtual uint8_t getRegisterCount() const = 0;
  virtual uint8_t getDigitCount() const = 0;
  virtual uint8_t getDecimalSeparatorCount() const = 0;
  virtual bool hasPlusSign() const = 0;
  virtual bool hasMenuSign() const = 0;
  virtual bool hasExpMinusSign() const = 0;
  virtual bool hasExpPlusSign() const = 0;
  virtual uint8_t getLEDCount() const = 0;
  virtual led_type getLedType() const = 0;
  virtual decimal_separator_position getDecimalSeparatorPosition() const = 0;
  virtual void setDecimalSeparatorPosition(decimal_separator_position dsp) = 0; 
};
