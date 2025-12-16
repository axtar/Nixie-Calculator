// DisplayDriver.hpp

// provides basic functions to drive
// the high voltage shift registers
// and functions to show numbers,
// decimal separators, the +/- signs
// and the LEDs

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <Config.h>
#include <Adafruit_Neopixel.h>
#include <DisplayHAL.hpp>
#include <DisplayHAL_IN12.hpp>
#include <DisplayHAL_IN16.hpp>
#include <DisplayHAL_IN17.hpp>
#include <DisplayHAL_B5870.hpp>
#include <DisplayHAL_LED.hpp>
#include <M7219Driver.hpp>

constexpr uint8_t DIGIT_OFF = 255;

// max number of special char digits
constexpr uint8_t MAX_SPECIAL_CHARS_DIGITS = 2;

// shift transition
constexpr uint8_t SHIFT_BEGIN = HIGH;
constexpr uint8_t SHIFT_COMMIT = LOW;

// store transition
constexpr uint8_t STORE_BEGIN = LOW;
constexpr uint8_t STORE_COMMIT = HIGH;

enum class display_state
{
  off,
  on
};

enum class digit_content
{
  time,
  date,
  temp,
  calc_minus,
  calc_plus,
  calc_error,
  exp_minus,
  exp_plus,
  none
};

class DisplayDriver
{
public:
  DisplayDriver(display_type displayType, uint8_t dataPin,
                uint8_t storePin, uint8_t shiftPin, uint8_t blankPin,
                uint8_t ledCtlPin) : _displayType(displayType),
                                     _dataPin(dataPin),
                                     _storePin(storePin),
                                     _shiftPin(shiftPin),
                                     _blankPin(blankPin),
                                     _ledCtlPin(ledCtlPin),
                                     _dispHAL(nullptr),
                                     _leds(nullptr),
                                     _m7219drv(nullptr)
  {
    // select HAL
    switch (_displayType)
    {
    case display_type::in12a:
      _dispHAL = new DisplayHAL_IN12();
      _dispHAL->setDecimalSeparatorPosition(decimal_separator_position::right);
      break;

    case display_type::in12b:
      _dispHAL = new DisplayHAL_IN12();
      _dispHAL->setDecimalSeparatorPosition(decimal_separator_position::left);
      break;

    case display_type::in16:
      _dispHAL = new DisplayHAL_IN16();
      break;

    case display_type::in17:
      _dispHAL = new DisplayHAL_IN17();
      break;

    case display_type::b5870:
      _dispHAL = new DisplayHAL_B5870();
      break;

    case display_type::led:
      _dispHAL = new DisplayHAL_LED();
      _m7219drv = new M7219Driver(_dataPin, _shiftPin, _storePin, CHAIN_SIZE);
      break;

    case display_type::undefined:
      // not allowed
      break;

    default: // avoid warning
      break;
    }

    // frequently used hardware specific parameters
    _digitCount = _dispHAL->getDigitCount();
    _decimalSeparatorCount = _dispHAL->getDecimalSeparatorCount();
    _ledCount = _dispHAL->getLEDCount();

    // array of digits
    // digit 0 is the most left digit
    _digits = new uint8_t[_digitCount];
    _digitsContent = new digit_content[_digitCount];

    // used for IN-15A
    _specialChars = new uint8_t[MAX_SPECIAL_CHARS_DIGITS];

    // array of decimal separators
    _decimalSeparators = new display_state[_decimalSeparatorCount];

    // set decimal position offset
    _dspOffset = _dispHAL->getDecimalSeparatorPosition() == decimal_separator_position::left ? 1 : 0;
    // initialize the neopixel library according to the LED type used
    switch (_dispHAL->getLedType())
    {
    case led_type::smd:
      _leds = new Adafruit_NeoPixel(_ledCount, _ledCtlPin, NEO_GRB + NEO_KHZ800);
      break;

    case led_type::tht:
      _leds = new Adafruit_NeoPixel(_ledCount, _ledCtlPin, NEO_RGB + NEO_KHZ800);
      break;

    case led_type::none:
      _leds = nullptr;
      break;
    }
    if (_leds)
    {
      // array of LED colors
      _pixelColors = new uint32_t[_ledCount];
    }
    clear();
  };

  virtual ~DisplayDriver()
  {
    delete _digits;
    delete _specialChars;
    delete _digitsContent;
    delete _decimalSeparators;
    delete _dispHAL;
    if (_leds)
    {
      delete _leds;
      delete _pixelColors;
    }
    if (_m7219drv)
    {
      delete _m7219drv;
    }
  }

  // initialization
  void begin()
  {
    if (_leds)
    {
      // init LEDs
      _leds->begin();
      clearLEDs();
    }
  }

  // turn off all the LEDs
  void clearLEDs()
  {
    if (_leds)
    {
      _leds->clear();
      _leds->show();
    }
  }

  // return display type
  display_type getDisplayType() const
  {
    return (_displayType);
  }

  // set the color of a specific LED by RGB values
  void setLED(uint16_t ledID, uint8_t red, uint8_t green, uint8_t blue)
  {
    if (_leds)
    {
      _leds->setPixelColor(ledID, red, green, blue);
    }
  }

  // set the color of a specific LED by the color value
  void setLED(uint16_t ledID, uint32_t color)
  {
    if (_leds)
    {
      _leds->setPixelColor(ledID, color);
    }
  }

  // return the current color of a specific LED
  uint32_t getLED(uint16_t ledID)
  {
    if (_leds)
    {
      return (_leds->getPixelColor(ledID));
    }
    else
    {
      return (0);
    }
  }

  // set the color of all LEDs
  void setAllLED(uint8_t red, uint8_t green, uint8_t blue)
  {
    for (int i = 0; i < _ledCount; i++)
    {
      setLED(i, red, green, blue);
    }
  }

  // turn off a single LED
  void clearLED(uint16_t ledID)
  {
    if (_leds)
    {
      _leds->setPixelColor(ledID, 0);
    }
  }

  // update the LEDs
  void updateLEDs()
  {
    if (_leds)
    {
      _leds->show();
    }
  }

  // return the number of digits on the display board
  uint8_t getDigitCount() const
  {
    return (_digitCount);
  }

  // return the number of decimal separators on the display board
  uint8_t getDecimalSeparatorCount() const
  {
    return (_decimalSeparatorCount);
  }

  // return the number of LEDs on the display board
  uint8_t getLedCount() const
  {
    return (_ledCount);
  }

  // return the decimal separator position
  decimal_separator_position getDecimalSeparatorPosition()
  {
    return (_dispHAL->getDecimalSeparatorPosition());
  }

  // return decimal separator position offset
  uint8_t getDspOffset()
  {
    return (_dspOffset);
  }

  // return if the display board has a plus sign
  bool hasPlusSign() const
  {
    return (_dispHAL->hasPlusSign());
  }

  // return if the display board as a menu sign, not used for now
  bool hasMenuSign() const
  {
    return (_dispHAL->hasMenuSign());
  }

  bool hasExpMinusSign() const
  {
    return (_dispHAL->hasExpMinusSign());
  }

  bool hasExpPlusSign() const
  {
    return (_dispHAL->hasExpPlusSign());
  }

  // set the value of a specific digit
  void setDigit(uint8_t digit, uint8_t value, digit_content content = digit_content::none)
  {
    if (digit < _digitCount)
    {
      _digits[digit] = value;
      setDigitContent(digit, content);
    }
  }

  // set special char
  void setSpecialChar(uint8_t digit, uint8_t value)
  {
    if (digit < MAX_SPECIAL_CHARS_DIGITS)
    {
      _specialChars[digit] = value;
    }
  }

  // return the value of a specific digit
  uint8_t getDigit(uint8_t digit) const
  {
    if (digit < _digitCount)
    {
      return (_digits[digit]);
    }
    return (DIGIT_OFF);
  }

  // set the type of content of the digit, used for lighting
  void setDigitContent(uint8_t digit, digit_content content)
  {
    if (digit < _digitCount)
    {
      _digitsContent[digit] = content;
    }
  }

  // return the type of content of the digit, used for lighting
  digit_content getDigitContent(uint8_t digit) const
  {
    if (digit < _digitCount)
    {
      return (_digitsContent[digit]);
    }
    return (digit_content::none);
  }

  // set the decimal separator status, on or off
  void setDecimalSeparator(uint8_t decimalSeparator, display_state state)
  {
    if (decimalSeparator < _decimalSeparatorCount)
    {
      _decimalSeparators[decimalSeparator] = state;
    }
  }

  // set the minus sign status, on or off
  void setMinusSign(display_state state)
  {
    _minusSign = state;
  }

  // set the plus sign status, on or off
  void setPlusSign(display_state state)
  {
    _plusSign = state;
  }

  // set the plus sign status, on or off
  void setExpMinusSign(display_state state)
  {
    _expMinusSign = state;
  }

  // set the plus sign status, on or off
  void setExpPlusSign(display_state state)
  {
    _expPlusSign = state;
  }

  // set the menu sign status, on or off
  void setMenuSign(display_state state)
  {
    _menuSign = state;
  }

  // store LED colors in memory
  void saveLEDColors()
  {
    if (_leds)
    {
      for (int i = 0; i < _ledCount; i++)
      {
        _pixelColors[i] = _leds->getPixelColor(i);
      }
    }
  }

  // restore LED colors from memory
  void restoreLEDColors()
  {
    if (_leds)
    {
      for (int i = 0; i < _ledCount; i++)
      {
        _leds->setPixelColor(i, _pixelColors[i]);
      }
    }
  }

  // refresh the display
  void refresh()
  {
    commitToDisplay();
  }

  // clear all the values
  void clear()
  {
    clearDecimalSeparators();
    clearDigits();
    clearSpecialChars();
    clearDigitsContent();
    clearMinusSign();
    clearExpMinusSign();
    clearExpPlusSign();
    clearPlusSign();
    clearMenuSign();
    showErrorString(false);
  }

  // clear the display
  void clearDisplay()
  {
    clear();
    refresh();
  }

  // clear all the decimal separators
  void clearDecimalSeparators()
  {
    for (uint8_t i = 0; i < _decimalSeparatorCount; i++)
    {
      _decimalSeparators[i] = display_state::off;
    }
  }

  // clear all the digits
  void clearDigits()
  {
    for (uint8_t i = 0; i < _digitCount; i++)
    {
      _digits[i] = DIGIT_OFF;
    }
  }

  // clear all the special chars
  void clearSpecialChars()
  {
    for (uint8_t i = 0; i < MAX_SPECIAL_CHARS_DIGITS; i++)
    {
      _specialChars[i] = DIGIT_OFF;
    }
  }

  // clear digit content
  void clearDigitsContent()
  {
    for (uint8_t i = 0; i < _digitCount; i++)
    {
      _digitsContent[i] = digit_content::none;
    }
  }

  // clear the minus sign
  void clearMinusSign()
  {
    setMinusSign(display_state::off);
  }

  // clear the exponent sign
  void clearExpMinusSign()
  {
    setExpMinusSign(display_state::off);
  }

  // clear the exponent sign
  void clearExpPlusSign()
  {
    setExpPlusSign(display_state::off);
  }

  // clear the menu sign
  void clearMenuSign()
  {
    setMenuSign(display_state::off);
  }

  // clear the plus sign
  void clearPlusSign()
  {
    setPlusSign(display_state::off);
  }

  // return if a digit is on
  bool isDigitOn(int8_t digit) const
  {
    bool retVal = false;
    if (_digits[digit] != DIGIT_OFF)
    {
      retVal = true;
    }
    return (retVal);
  }

  // set display brightness
  void setDisplayBrightness(int value)
  {
    if (_m7219drv)
    {
      _m7219drv->setBrightness(static_cast<uint8_t>(value));
    }
    else
    {
      // not implemented for nixies
    }
  }

  // set true to display "Error" string, only for 7-seg LED display
  void showErrorString(bool value)
  {
    _showErrorString = value;
  }

  // returns LED driver
  M7219Driver *getLEDDriver()
  {
    return (_m7219drv);
  }

private:
  uint8_t _digitCount;
  uint8_t _decimalSeparatorCount;
  uint8_t _ledCount;
  display_type _displayType;
  uint8_t *_digits;
  uint8_t *_specialChars;
  digit_content *_digitsContent;
  display_state *_decimalSeparators;
  display_state _minusSign;
  display_state _plusSign;
  display_state _expMinusSign;
  display_state _expPlusSign;
  display_state _menuSign;
  uint32_t *_pixelColors;
  uint8_t _dataPin;
  uint8_t _storePin;
  uint8_t _shiftPin;
  uint8_t _blankPin;
  uint8_t _ledCtlPin;
  DisplayHAL *_dispHAL;
  Adafruit_NeoPixel *_leds;
  M7219Driver *_m7219drv;
  bool _showErrorString;
  uint8_t _dspOffset;

  // commit a digit to the shift registers
  void commitDigitNumber(uint8_t digit, uint8_t number) const
  {
    commitBit(_digits[digit] == number ? HIGH : LOW);
  }

  // commit a special char to the shift registers
  void commitSpecialChar(uint8_t digit, uint8_t number) const
  {
    commitBit(_specialChars[digit] == number ? HIGH : LOW);
  }

  // return if special char should be displayed
  bool checkSpecialChar(uint8_t digit, uint8_t number) const
  {
    return (_specialChars[digit] == number ? true : false);
  }

  // commit a decimal separator to the shift registers
  void commitDecimalSeparator(int8_t decimalSeparator) const
  {
    commitBit(_decimalSeparators[decimalSeparator] == display_state::off ? LOW : HIGH);
  }

  // commit the minus sign to the shift registers
  void commitMinusSign() const
  {
    commitBit(_minusSign == display_state::off ? LOW : HIGH);
  }

  // commit the plus sign to the shift registers
  void commitPlusSign() const
  {
    commitBit(_plusSign == display_state::off ? LOW : HIGH);
  }

  // commit the exponent minus sign to the shift registers
  void commitExpMinusSign() const
  {
    commitBit(_expMinusSign == display_state::off ? LOW : HIGH);
  }

  // commit the exponent plus sign to the shift registers
  void commitExpPlusSign() const
  {
    commitBit(_expPlusSign == display_state::off ? LOW : HIGH);
  }

  // commit the menu sign to the shift registers
  void commitMenuSign() const
  {
    commitBit(_menuSign == display_state::off ? LOW : HIGH);
  }

  // commit a bit to the shift registers
  void commitBit(uint8_t value) const
  {
    digitalWrite(_shiftPin, SHIFT_BEGIN);
    digitalWrite(_dataPin, value);
    digitalWrite(_shiftPin, SHIFT_COMMIT);
  }

  // commit values to the display
  void commitToDisplay()
  {
    if (_displayType == display_type::led)
    {
      // commit to 7-seg LED display
      uint8_t index = 0;
      uint8_t pos = 0;
      if (_m7219drv)
      {
        // commit base sign
        _dispHAL->getDigitAddress(BASE_SIGN, &index, &pos);
        if (_minusSign == display_state::on)
        {
          _m7219drv->setChar(index, pos, CHAR_MINUS, false);
        }
        else
        {
          _m7219drv->setChar(index, pos, CHAR_BLANK, false);
        }
        // commit digits and decimal points
        for (uint8_t i = 0; i < LED_DIGITCOUNT; i++)
        {
          _dispHAL->getDigitAddress(i + 1, &index, &pos);
          bool dp = (_decimalSeparators[i] == display_state::on);
          if (_digits[i] == DIGIT_OFF)
          {
            _m7219drv->setChar(index, pos, CHAR_BLANK, dp);
          }
          else
          {
            _m7219drv->setChar(index, pos, _digits[i], dp);
          }
        }
        // commit exponent sign
        _dispHAL->getDigitAddress(EXPONENT_SIGN, &index, &pos);
        if (_expMinusSign == display_state::on)
        {
          _m7219drv->setChar(index, pos, CHAR_MINUS, false);
        }
        else
        {
          _m7219drv->setChar(index, pos, CHAR_BLANK, false);
        }
        // commmit error string
        if (_showErrorString)
        {
          _m7219drv->setChar(0, 1, CHAR_E, false);
          _m7219drv->setChar(0, 2, CHAR_r, false);
          _m7219drv->setChar(0, 3, CHAR_r, false);
          _m7219drv->setChar(0, 4, CHAR_o, false);
          _m7219drv->setChar(0, 5, CHAR_r, false);
        }
      }
    }
    else
    {
      // commit to nixie display
      register_type regType;
      uint8_t digit = 0;
      uint8_t number = 0;
      digitalWrite(_storePin, STORE_BEGIN);

      for (uint8_t i = _dispHAL->getRegisterCount(); i > 0; i--)
      {
        regType = _dispHAL->getRegisterInfo(i, &digit, &number);
        switch (regType)
        {
        case register_type::unknown:
          break;

        case register_type::minus_sign:
          if (checkSpecialChar(digit, number))
          {
            commitSpecialChar(digit, number);
          }
          else
          {
            commitMinusSign();
          }
          break;

        case register_type::plus_sign:
          if (checkSpecialChar(digit, number))
          {
            commitSpecialChar(digit, number);
          }
          else
          {
            commitPlusSign();
          }
          break;

        case register_type::exp_minus_sign:
          if (checkSpecialChar(digit, number))
          {
            commitSpecialChar(digit, number);
          }
          else
          {
            commitExpMinusSign();
          }
          break;

        case register_type::exp_plus_sign:
          if (checkSpecialChar(digit, number))
          {
            commitSpecialChar(digit, number);
          }
          else
          {
            commitExpPlusSign();
          }
          break;

        case register_type::menu_sign:
          if (checkSpecialChar(digit, number))
          {
            commitSpecialChar(digit, number);
          }
          else
          {
            commitMenuSign();
          }
          break;

        case register_type::decimal_separator:
          commitDecimalSeparator(digit);
          break;

        case register_type::number:
          commitDigitNumber(digit, number);
          break;

        case register_type::special_char:
          commitSpecialChar(digit, number);
          break;

        case register_type::not_used:
        case register_type::not_connected:
          commitBit(LOW);
          break;
        }
      }
      digitalWrite(_storePin, STORE_COMMIT);
    }
  }
};
