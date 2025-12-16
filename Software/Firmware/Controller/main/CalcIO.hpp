// CalcIO.hpp

// handles calculator numeric input and provides result formatting routines

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <ratpak.h>
#include <Helper.hpp>

typedef struct
{
  bool baseNegative;
  String base;
  bool exponentNegative;
  String exponent;
  bool exponentMode;
  bool decimalSeparator;
} CALC_NUMBER;

class CalcIO
{
public:
  CalcIO(uint8_t digitCount, uint8_t maxExpLength)
      : _digitCount(digitCount),
        _maxExpLength(maxExpLength)
  {
    clear();
  }

  virtual ~CalcIO()
  {
  }

  // clear internal number
  void clear()
  {
    clear(&_number);
  }

  // clear input
  void clear(CALC_NUMBER *number)
  {
    number->exponentMode = false;
    number->decimalSeparator = false;
    number->baseNegative = false;
    number->base = "0";
    number->exponentNegative = false;
    number->exponent.clear();
  }

  // handle numeric input
  void onDigit(uint8_t digit)
  {
    char c = digit + '0';
    if (!_number.exponentMode)
    {
      if (_number.base.length() == 1 && _number.base.toInt() == 0)
      {
        _number.base = c;
      }
      else
      {
        if (getAvailableBaseDigits() > 0)
        {
          _number.base += c;
        }
      }
    }
    else
    {
      if (_number.exponent.toInt() == 0)
      {
        _number.exponent = c;
      }
      else
      {
        if (getAvailableExponentDigits() > 0)
        {
          _number.exponent += c;
        }
      }
    }
  }

  // handle decimal separator input
  void onDecimalSeparator()
  {
    if (!_number.exponentMode)
    {
      if (_number.base.indexOf(DECIMAL_SEPARATOR) == (-1))
      {
        if (getAvailableBaseDigits() > 0)
        {
          _number.base += DECIMAL_SEPARATOR;
          _number.decimalSeparator = true;
        }
      }
    }
  }

  // handle backspace
  void onBackSpace()
  {
    if (_number.exponentMode)
    {
      if (_number.exponent == "0")
      {
        _number.exponent.clear();
        _number.exponentMode = false;
      }
      else if (_number.exponent.length() == 1)
      {
        _number.exponent = "0";
        _number.exponentNegative = false;
      }
      else
      {
        _number.exponent.remove(_number.exponent.length() - 1);
      }
    }
    else
    {
      if (_number.base.length() == 1)
      {
        _number.base = "0";
        _number.baseNegative = false;
      }
      else
      {
        if (_number.base[_number.base.length() - 1] == DECIMAL_SEPARATOR)
        {
          _number.decimalSeparator = false;
        }
        _number.base.remove(_number.base.length() - 1);
      }
    }
  }

  // handle base or exponent sign change
  void onChangeSign(const bool inputPending)
  {
    if (_number.exponentMode && inputPending)
    {
      if (_number.exponent != "0")
      {
        _number.exponentNegative = !_number.exponentNegative;
      }
    }
    else
    {
      if (_number.base != "0" && _number.base != "0.")
      {
        _number.baseNegative = !_number.baseNegative;
      }
    }
  }

  // switch to exponent mode if digits available
  void onExponent()
  {
    if (getAvailableExponentDigits() > 0)
    {
      _number.exponentMode = true;
      if (_number.exponent.isEmpty())
      {
        _number.exponent = "0";
      }
      if ((_number.base == "0") || (_number.base == "0."))
      {
        _number.base = "1";
      }
    }
  }

  // get PRAT from number
  void getPRAT(PRAT *p, uint32_t radix, int32_t precision) const
  {
    std::string_view bsw(_number.base.c_str());
    std::string_view esw(_number.exponent.c_str());
    *p = StringToRat(_number.baseNegative, bsw, _number.exponentNegative, esw, radix, precision);
  }

  // set number from PRAT and format for display
  uint32_t setNumber(PRAT p, uint32_t radix, int32_t precision, uint8_t fixedDecimals, bool forceScientific)
  {
    // get string in scientific format with the max
    // available display precision + 2 and without rounding
    std::string sw = RatToScientificString(p, radix, _digitCount + 2);
    String s = sw.c_str();

    // converto to internal number
    uint32_t result = numberFromString(s, &_number);
    if (result == 0)
    {
      // split base
      String intPart, fracPart;
      splitBase(intPart, fracPart);

      if (!forceScientific)
      {
        // decide if we want to keep the scientific format
        if (!keepExponent(intPart, fracPart, fixedDecimals))
        {
          removeExponent(intPart, fracPart);
        }
      }

      // format and round base
      splitBase(intPart, fracPart);
      result = formatBase(intPart, fracPart, fixedDecimals);
    }
    return result;
  }

  // get number
  CALC_NUMBER getNumber() const
  {
    return (_number);
  }

  void setMaxExponentLength(uint8_t length)
  {
    _maxExpLength = length;
  }

  // convert string to the internal number
  uint32_t numberFromString(String s, CALC_NUMBER *number)
  {
    // clear number
    clear(number);
    number->base.clear();

    //  build number
    for (int i = 0; i < s.length(); i++)
    {
      switch (s[i])
      {
      case '-':
        if (i == 0)
        {
          number->baseNegative = true;
        }
        else
        {
          number->exponentNegative = true;
        }
        break;

      case 'e':
        number->exponentMode = true;
        break;

      case DECIMAL_SEPARATOR:
        number->decimalSeparator = true;
        [[fallthrough]];
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        if (!number->exponentMode)
        {
          number->base += s[i];
        }
        else
        {
          number->exponent += s[i];
        }
        break;

      default: // avoid warning
        break;
      }
    }
    // check for exponent overflow
    if (number->exponent.length() > _maxExpLength)
    {
      clear(number);
      return (CALC_E_OVERFLOW);
    }
    return (0);
  }

private:
  uint8_t _digitCount;
  uint8_t _maxExpLength;
  CALC_NUMBER _number;

  // format and round base according to available display digits and fixed decimals
  uint32_t formatBase(String &intPart, String &fracPart, uint8_t fixedDecimals)
  {
    // round base
    uint8_t decimals = _digitCount - intPart.length();
    if (!_number.exponent.isEmpty())
    {
      // we have to reduce decimals by exponent length + 1
      decimals -= (_number.exponent.length() + 1);
    }
    if (fixedDecimals != 0)
    {
      if (decimals > fixedDecimals)
      {
        decimals = fixedDecimals;
      }
    }
    bool carry = roundUp(intPart, fracPart, decimals);

    // remove trailing zeros
    int lastNonZero = fracPart.length() - 1;
    while (lastNonZero >= 0 && fracPart.charAt(lastNonZero) == '0')
    {
      lastNonZero--;
    }
    fracPart = fracPart.substring(0, lastNonZero + 1);

    // handle carry
    if (carry)
    {
      int exponent = _number.exponent.toInt();
      if (exponent != 0)
      {
        // exponent is not empty, adjust exponent
        if (_number.exponentNegative)
        {
          exponent--;
        }
        else
        {
          exponent++;
        }
        if (exponent == 0)
        {
          _number.exponent.clear();
          _number.exponentNegative = false;
          _number.exponentMode = false;
        }
        else
        {
          _number.exponent = String(exponent);
        }
        intPart = 1;
      }
      else
      {
        // exponent is empty
        if (intPart.length() > _digitCount)
        {
          // we have too many digits in intPart after rounding, switch to scientific format
          if (_number.exponentNegative)
          {
            exponent = (intPart.length() - 1) * -1;
          }
          else
          {
            exponent = intPart.length() - 1;
          }
          intPart = 1;
          _number.exponent = String(abs(exponent));
          if (exponent < 0)
          {
            _number.exponentNegative = true;
          }
          else
          {
            _number.exponentNegative = false;
          }
          _number.exponentMode = true;
        }
      }
    }
    if (fixedDecimals != 0)
    {
      // pad with zeros if needed
      int paddingZeros = fixedDecimals - fracPart.length();
      for (int i = 0; i < paddingZeros; i++)
      {
        fracPart += '0';
      }
      // we may have to truncate the fracPart to fit
      int maxFracLength = _digitCount - intPart.length();
      if (fracPart.length() > maxFracLength)
      {
        fracPart = fracPart.substring(0, maxFracLength);
      }
    }
    // build base
    if (fracPart.isEmpty())
    {
      _number.base = intPart;
    }
    else
    {
      _number.base = intPart + DECIMAL_SEPARATOR + fracPart;
    }
    if (_number.exponent.length() > _maxExpLength)
    {
      return (CALC_E_OVERFLOW);
    }
    return (0);
  }

  // split base into integer and frac part
  void splitBase(String &intPart, String &fracPart)
  {
    int pos = _number.base.indexOf(DECIMAL_SEPARATOR);
    if (pos == -1)
    {
      intPart = _number.base;
      fracPart = "";
    }
    else
    {
      intPart = _number.base.substring(0, pos);
      fracPart = _number.base.substring(pos + 1);
    }
  }

  // round to n decimal places
  bool roundUp(String &intPart, String &fracPart, int decimals)
  {
    bool carry = fracPart.charAt(decimals) >= '5' ? true : false;
    fracPart = fracPart.substring(0, decimals);

    for (int i = decimals - 1; i >= 0; --i)
    {
      // adjust frac part
      if (carry)
      {
        if (fracPart.charAt(i) == '9')
        {
          fracPart.setCharAt(i, '0');
        }
        else
        {
          fracPart.setCharAt(i, fracPart.charAt(i) + 1);
          carry = false;
        }
      }
      else
      {
        break;
      }
    }
    if (carry)
    {
      // adjust integer part
      for (int i = intPart.length() - 1; i >= 0; --i)
      {
        if (intPart.charAt(i) == '9')
        {
          intPart.setCharAt(i, '0');
        }
        else
        {
          intPart.setCharAt(i, intPart.charAt(i) + 1);
          carry = false;
          break;
        }
      }
      if (carry)
      {
        intPart = "1" + intPart;
      }
    }
    return (carry);
  }

  // remove exponent adjusting the base
  void removeExponent(String &intPart, String &fracPart)
  {
    // check if we have an exponent
    if (!_number.exponent.isEmpty())
    {
      // get exponent
      int exponent = _number.exponent.toInt();
      if (_number.exponentNegative)
      {
        exponent *= (-1);
      }
      _number.exponent.clear();
      _number.exponentNegative = false;
      if (exponent != 0)
      {
        // clear number exponent
        if (_number.base != "0")
        {
          // remove the decimal point from the base
          String normalizedBase = intPart + fracPart;

          // adjust the position of the decimal point based on the exponent
          int decimalPos = intPart.length() + exponent;

          if (decimalPos <= 0)
          {
            // pad with leading zeros for negative decimal position
            String padding = "";
            for (int i = 0; i < -decimalPos + 1; i++)
            {
              padding += "0";
            }
            normalizedBase = padding + normalizedBase;
            decimalPos = 1;
          }
          else if (decimalPos >= normalizedBase.length())
          {
            // pad with trailing zeros for positive decimal position
            String padding = "";
            for (int i = 0; i < decimalPos - normalizedBase.length(); i++)
            {
              padding += "0";
            }
            normalizedBase += padding;
          }

          // insert the decimal point
          String result = normalizedBase.substring(0, decimalPos) + DECIMAL_SEPARATOR + normalizedBase.substring(decimalPos);

          // remove trailing decimal separator if needed
          if (result.endsWith(String(DECIMAL_SEPARATOR)))
          {
            result = result.substring(0, result.length() - 1);
          }
          _number.base = result;
        }
      }
    }
  }

  // decide if we stay in exponent mode
  bool keepExponent(String &intPart, String &fracPart, uint8_t fixedDecimals)
  {
    bool result = false;
    int exponent = _number.exponent.toInt();
    if (_number.exponentNegative)
    {
      exponent *= (-1);
    }
    // check if exponent is to big or to small
    if (exponent >= _digitCount || exponent <= ((_digitCount) * (-1)))
    {
      result = true;
    }

    if (_number.exponentNegative)
    {
      // check if we can show more decimals if we keep the scientific mode
      if ((fracPart.length() >= _digitCount + exponent) && exponent < -3)
      {
        result = true;
      }
    }
    return (result);
  }

  // get available digits for the base
  int getAvailableBaseDigits()
  {
    int result = 0;
    if (_number.exponent.isEmpty())
    {
      result = _digitCount - getBaseLength();
    }
    else
    {
      result = _digitCount - getBaseLength() - _number.exponent.length() - 1;
    }
    return (result);
  }

  // get available digits for the exponent
  int getAvailableExponentDigits()
  {
    int result = 0;

    if (_number.exponent.length() < _maxExpLength)
    {
      result = _digitCount - getBaseLength() - _number.exponent.length() - 1;
      if (result > _maxExpLength)
      {
        result = _maxExpLength;
      }
    }
    return (result);
  }

  // get number of digits of the base
  int getBaseLength()
  {
    if (_number.decimalSeparator)
    {
      return (_number.base.length() - 1);
    }
    else
    {
      return (_number.base.length());
    }
  }

  // print all parts of the number
  void printNumber()
  {
    D_println("Base:          " + _number.base);
    D_println("Base sign:     " + String(_number.baseNegative ? "-" : "+"));
    D_println("Exponent:      " + _number.exponent);
    D_println("Exponent sign: " + String(_number.exponentNegative ? "-" : "+"));
  }
};
