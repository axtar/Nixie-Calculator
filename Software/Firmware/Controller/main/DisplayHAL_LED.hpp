// displayHAL_IN17.hpp

// implements the hardware abstraction layer interface
// for the 7-seg LED display

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <displayHAL.hpp>

constexpr uint8_t LED_REGISTERCOUNT = 0;
constexpr uint8_t LED_DIGITCOUNT = 14;
constexpr uint8_t LED_DECIMALSEPARATORCOUNT = 14;
constexpr uint8_t LED_LEDCOUNT = 0;
constexpr auto LED_DSP = decimal_separator_position::right;

constexpr uint8_t BASE_SIGN = 0;
constexpr uint8_t EXPONENT_SIGN = 15;
constexpr uint8_t CHAIN_SIZE = 2; // 2 chained ICs

class DisplayHAL_LED : public DisplayHAL
{
public:
  DisplayHAL_LED()
  {
    initAddressTable();
  }

  virtual ~DisplayHAL_LED()
  {
    free(_addressTable);
  }

  uint8_t getRegisterCount() const
  {
    return (LED_REGISTERCOUNT);
  }

  uint8_t getDigitCount() const
  {
    return (LED_DIGITCOUNT);
  }

  uint8_t getDecimalSeparatorCount() const
  {
    return (LED_DECIMALSEPARATORCOUNT);
  }

  uint8_t getLEDCount() const
  {
    return (LED_LEDCOUNT);
  }

  bool hasPlusSign() const
  {
    return (false);
  }

  bool hasExpMinusSign() const
  {
    return (true);
  }

  bool hasExpPlusSign() const
  {
    return (false);
  }

  bool hasMenuSign() const
  {
    return (false);
  }

  led_type getLedType() const
  {
    return (led_type::none);
  }

  // not used for LED display
  register_type getRegisterInfo(uint8_t registerNumber, uint8_t *digit, uint8_t *number) const
  {
    *digit = 0;
    *number = 0;

    return (register_type::not_used);
  }

  // return the address of a digit
  void getDigitAddress(uint8_t digit, uint8_t *index, uint8_t *position) const
  {
    if (digit < (LED_DIGITCOUNT + 2))
    {
      *index = _addressTable[digit].index;
      *position = _addressTable[digit].pos;
    }
  }

  // return the decimal separator position
  decimal_separator_position getDecimalSeparatorPosition() const
  {
    return (LED_DSP);
  }

  // not used
  void setDecimalSeparatorPosition(decimal_separator_position dsp)
  {
  }


private:
  DIGIT_ADDRESS *_addressTable;

  // this table contains the address (IC and digit) of a calculator digit in the driver chain
  void initAddressTable()
  {
    // 14 digits and 2 signs
    _addressTable = (DIGIT_ADDRESS *)malloc(sizeof(DIGIT_ADDRESS) * (LED_DIGITCOUNT + 2));

    _addressTable[BASE_SIGN] = {0, 0}; // the address of the calculator base sign is digit 0 of IC 0
    _addressTable[1] = {0, 1};         // the address of the first calculator digit is digit 1 of IC 0
    _addressTable[2] = {0, 2};
    _addressTable[3] = {0, 3};
    _addressTable[4] = {0, 4};
    _addressTable[5] = {0, 5};
    _addressTable[6] = {0, 6};
    _addressTable[7] = {0, 7};
    _addressTable[8] = {1, 0};
    _addressTable[9] = {1, 1};
    _addressTable[10] = {1, 2};
    _addressTable[11] = {1, 3};
    _addressTable[12] = {1, 4};
    _addressTable[13] = {1, 5};
    _addressTable[14] = {1, 6};
    _addressTable[EXPONENT_SIGN] = {1, 7};
  }
};
