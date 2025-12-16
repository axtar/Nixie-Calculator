// Calculator.hpp

// provides calculator mode functionality

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <Config.h>
#include <KeyboardHandler.hpp>
#include <KeyboardDecoder.hpp>
#include <SettingsCache.hpp>
#include <CalcIO.hpp>
#if CALC_TYPE == CALC_TYPE_RPN
#include <CalcEngineRPN.hpp>
#else
#include <CalcEngineALG.hpp>
#endif

enum class long_operation
{
  begin,
  end
};

typedef struct
{
  String result;
  String padding;
  CALC_NUMBER number;
  uint8_t scrollLength;
  uint8_t scrollPos;
  bool initialized;
  unsigned long lastScrollTimeStamp;
  int decimalPos;
} SCROLL_INFO;

typedef std::map<String, String> REGISTERSTRINGMAP;

class Calculator
{
protected:
  using notifyLongOperationCb = std::function<void(long_operation lop)>;
  using notifyRegisterUpdateCb = std::function<void(String regId, String value)>;

public:
  Calculator()
  {
    _inputPending = false;
    _hasPlusSign = false;
    _cio = nullptr;
    _notifyLongOperation = nullptr;
    _notifyRegisterUpdate = nullptr;
    _forceScientific = false;
    resetScrollInfo();
  }

  virtual ~Calculator()
  {
    if (_cio != nullptr)
    {
      delete (_cio);
    }
  }

  // initialize
  void begin(uint8_t digitCount, uint8_t decimalSeparatorCount, bool hasPlusSign)
  {
    // set ratpak decimal separator char
    SetDecimalSeparator(DECIMAL_SEPARATOR);
    // intialize ratpak constants
    ChangeConstants(RAT_RADIX, SettingsCache::calcPrecision);

    // configure calc engine
    _calcEngine.setRadix(RAT_RADIX);
    _calcEngine.setPrecision(SettingsCache::calcPrecision);
    _calcEngine.setFixedDecimals(SettingsCache::fixedDecimals);
    _calcEngine.setMaxTrig();

    // clear calc engine
    _calcEngine.clear();

    // some calculator parameters
    _digitCount = digitCount;
    _decimalSeparatorCount = decimalSeparatorCount;
    _hasPlusSign = hasPlusSign;
    setAngleMode(SettingsCache::angleMode);

    // initialize calculator input/output
    _cio = new CalcIO(_digitCount, SettingsCache::maxExpDigits);
    updateNumber();
  }

  // get registers from calc engine and convert to string
  void getRegisterStrings(REGISTERSTRINGMAP &regStringMap)
  {
    REGISTERMAP regMap;
    _calcEngine.getRegisters(regMap);
    for (const auto &value : regMap)
    {
      if (getOperationReturnCode() != operation_return_code::success)
      {
        if (value.first == "X:")
        {
          regStringMap[value.first] = getCalculatorErrorText();
        }
        else
        {
          regStringMap[value.first] = _calcEngine.getRatString(value.second);
        }
      }
      else
      {
        regStringMap[value.first] = _calcEngine.getRatString(value.second);
      }
    }
  }

  // get error description
  String getErrorText(operation_return_code code)
  {
    return CalcError::getErrorText(code);
  }

  // after a calculation, the calculation flag is set
  bool isCalculation() const
  {
    return (_calcEngine.isCalculation());
  }

  // reset the calculation flag
  void resetCalculationFlag()
  {
    _calcEngine.resetCalculationFlag();
  }

  // return the return code of the last operation
  operation_return_code getOperationReturnCode()
  {
    return (_calcEngine.getOperationReturnCode());
  }

  // update the internal number
  void updateNumber() const
  {
    _cio->setNumber(_calcEngine.getResult(), RAT_RADIX, SettingsCache::calcPrecision, _calcEngine.getFixedDecimals(), _forceScientific);
  }

  // called if a key is pressed
  bool onKeyboardEvent(uint8_t keyCode, key_state keyState, bool functionKeyPressed, bool shiftKeyPressed)
  {
    operation op;
    uint8_t digit;
    key_function_type function;
    _calcEngine.resetCalculationFlag();
    bool result = true;
    if (keyState == key_state::pressed)
    {
      KeyboardDecoder::decode(keyCode, functionKeyPressed, shiftKeyPressed, &function, &op, &digit);

      switch (function)
      {
      case key_function_type::numeric:
        if (!functionKeyPressed)
        {
          digitInput(digit);
        }
        else
        {
          setDecimals(digit);
          // we have to update the display after modifying decimals
          if (!_inputPending)
          {
            updateNumber();
          }
        }
        break;

      case key_function_type::numericx2:
        digitInput(digit);
        digitInput(digit);
        break;

      case key_function_type::control:
        controlInput(op);
        break;

      case key_function_type::operation:
      {
        if (_inputPending)
        {
          numericInput();
        }
        operationInput(op);
        // prepare for display
        processResult();

        _inputPending = false;
      }
      break;

      case key_function_type::unknown:
        result = false;
        break;
      }
    }
    return (result);
  }

  // turn forced scientific notation on or off
  void switchForceScientific()
  {
    _forceScientific = !_forceScientific;
    if (!_inputPending)
    {
      updateNumber();
    }
  }

  // set fixed decimals, 0 = floating
  void setDecimals(uint8_t digit)
  {
    SettingsCache::fixedDecimals = (fixed_decimals::fixed_decimals)digit;
    // set the decimals in calc engine
    _calcEngine.setFixedDecimals(digit);
  }

  // set max exponent length
  void setMaxExponentLength(uint8_t length)
  {
    _cio->setMaxExponentLength(length);
  }

  // degrees or radians
  void setAngleMode(angle_mode::angle_mode angleMode)
  {
    _calcEngine.setAngleType(angleMode == angle_mode::degrees ? angle_type::deg : angle_type::rad);
  }

  // return if input is pending
  bool isInputPending() const
  {
    return (_inputPending);
  }

  // return the internal number
  CALC_NUMBER getNumber() const
  {
    return (_cio->getNumber());
  }

  // set the callback function
  void attachLongOperationCb(notifyLongOperationCb callBack)
  {
    _notifyLongOperation = callBack;
  }

  // remove callback function
  void detachLongOperationCb()
  {
    _notifyLongOperation = nullptr;
  }

  // set the callback function
  void attachRegisterUpadteCb(notifyRegisterUpdateCb callBack)
  {
    _notifyRegisterUpdate = callBack;
    _calcEngine.attachNotifyRegisterUpdateCb(std::bind(&Calculator::onRegisterUpdate, this, std::placeholders::_1, std::placeholders::_2));
  }

  // remove callback function
  void detachRegisterUpdateCb()
  {
    _notifyRegisterUpdate = nullptr;
    _calcEngine.detachNotifyRegisterUpdateCb();
  }

  // called if a register is updated
  void onRegisterUpdate(String regId, PRAT p)
  {
    if (p)
    {
      // not in error state, notify register
      _notifyRegisterUpdate(regId, _calcEngine.getRatString(p));
    }
    else
    {
      // we are in error state, notify error
      _notifyRegisterUpdate(regId, getCalculatorErrorText());
    }
  }

  // return an error description string
  String getCalculatorErrorText()
  {
    return ("Error (" + String(static_cast<int>(_calcEngine.getOperationReturnCode())) + ") " + CalcError::getErrorText(_calcEngine.getOperationReturnCode()));
  }

  // get reg X string
  String getResultString(NumberFormat format = NumberFormat::Float)
  {
    return (_calcEngine.getRatString(_calcEngine.getResult(), format));
  }

  // provide information for result scrolling
  bool getScrollInfo(bool *baseNegative, String &scrollString, int *decimalPos, bool *exponenentNegative, String &exponent)
  {
    bool result = false;
    // scroll only if the calc engine is not in error state
    if ((_calcEngine.getOperationReturnCode() == operation_return_code::success) && !_inputPending)
    {
      // initialize scrolling if needed
      if (!_scrollInfo.initialized)
      {
        resetScrollInfo();
        // get result string
        if (!_forceScientific)
        {
          _scrollInfo.result = getResultString();
        }
        else
        {
          _scrollInfo.result = getResultString(NumberFormat::Scientific);
        }

        // convert to number
        _cio->numberFromString(_scrollInfo.result, &_scrollInfo.number);

        // get position of decimal separator
        _scrollInfo.decimalPos = _scrollInfo.number.base.indexOf(DECIMAL_SEPARATOR);
        if (_scrollInfo.decimalPos != -1)
        {
          // remove decimal separator
          _scrollInfo.number.base.remove(_scrollInfo.decimalPos, 1);
          _scrollInfo.decimalPos--;
        }
        // get available digits for showing the result
        if ((_scrollInfo.number.exponent.toInt() == 0) && (!_forceScientific))
        {
          _scrollInfo.scrollLength = _digitCount;
        }
        else
        {
          _scrollInfo.scrollLength = _digitCount - 1 - _scrollInfo.number.exponent.length();
        }
        // make padding string
        _scrollInfo.padding = "";
        for (int i = 0; i < _scrollInfo.scrollLength; i++)
        {
          _scrollInfo.padding += " ";
        }
        _scrollInfo.initialized = true;
      }
      // check if base length is larger than available display digits
      if (_scrollInfo.number.base.length() > _scrollInfo.scrollLength)
      {
        scrollString = "";
        // check if it's time to scroll
        if (millis() - _scrollInfo.lastScrollTimeStamp > (SettingsCache::scrollDelay * 100))
        {
          String s = _scrollInfo.number.base + _scrollInfo.padding;

          // make partial string
          scrollString = s.substring(_scrollInfo.scrollPos, _scrollInfo.scrollPos + _scrollInfo.scrollLength);
          *baseNegative = _scrollInfo.number.baseNegative;
          *decimalPos = _scrollInfo.decimalPos;
          *exponenentNegative = _scrollInfo.number.exponentNegative;
          exponent = _scrollInfo.number.exponent;
          _scrollInfo.scrollPos++;
          _scrollInfo.decimalPos--;
          _scrollInfo.lastScrollTimeStamp = millis();
        }
        // check if we still have to scroll
        if ((scrollString.length() == _scrollInfo.scrollLength) || scrollString.isEmpty())
        {
          result = true;
        }
      }
    }
    if (!result)
    {
      _scrollInfo.initialized = false;
    }
    return (result);
  }

  // clear scroll information
  void resetScrollInfo()
  {
    _scrollInfo.initialized = false;
    _scrollInfo.scrollPos = 0;
    _scrollInfo.scrollLength = 0;
    _scrollInfo.lastScrollTimeStamp = 0;
    _scrollInfo.decimalPos = -1;
  }

private:
#if CALC_TYPE == CALC_TYPE_RPN
  CalcEngineRPN _calcEngine;
#else
  CalcEngineALG _calcEngine;
#endif
  Settings *_settings;
  uint8_t _digitCount;
  uint8_t _decimalSeparatorCount;
  bool _inputPending;
  bool _hasPlusSign;
  CalcIO *_cio;
  notifyLongOperationCb _notifyLongOperation;
  notifyRegisterUpdateCb _notifyRegisterUpdate;
  bool _forceScientific;
  SCROLL_INFO _scrollInfo;

  // process the result and prepare for display
  uint32_t processResult()
  {
    uint32_t result = _cio->setNumber(_calcEngine.getResult(), RAT_RADIX, SettingsCache::calcPrecision, _calcEngine.getFixedDecimals(), _forceScientific);
    if (result != 0)
    {
      // we got an overflow error in the result
      _calcEngine.setOperationReturnCodeFromRatError(result);
      // clear result
      _calcEngine.setResult(rat_zero);
      _cio->setNumber(rat_zero, RAT_RADIX, SettingsCache::calcPrecision, _calcEngine.getFixedDecimals(), _forceScientific);
    }
    return (result);
  }

  // check memory register overflow
  uint32_t checkMemResult(uint8_t index)
  {
    uint32_t result = _cio->setNumber(_calcEngine.getMemReg(index), RAT_RADIX, SettingsCache::calcPrecision, _calcEngine.getFixedDecimals(), _forceScientific);
    if (result != 0)
    {
      // clear memory register
      _calcEngine.setMemReg(rat_zero, index);
    }
    // back to result
    result = _cio->setNumber(_calcEngine.getResult(), RAT_RADIX, SettingsCache::calcPrecision, _calcEngine.getFixedDecimals(), _forceScientific);
    return (result);
  }

  // process user numeric input
  void numericInput()
  {
    PRAT p = nullptr;
    _cio->getPRAT(&p, RAT_RADIX, SettingsCache::calcPrecision);
    _calcEngine.handleNumericInput(p);
    destroyrat(p);
  }

  // called if a numeric key was pressed
  void digitInput(uint8_t digit)
  {
    // only accept input if calc engine not in error state
    if (_calcEngine.getOperationReturnCode() == operation_return_code::success)
    {
      uint8_t index = MEM_REGISTER_NONE;
      if (_calcEngine.handleDigitInput(digit, &index))
      {
        _inputPending = false;

        // the calc engine handled the input
        // because of a pending operation
        // we have to check the result
        processResult();
        if (index != MEM_REGISTER_NONE)
        {
          checkMemResult(index);
        }
      }
      else
      {
        if (!_inputPending)
        {
          // new input, clear
          _cio->clear();
        }
        // handle numeric input
        _cio->onDigit(digit);
        _inputPending = true;
      }
    }
    else
    {
      // recover from error
      _calcEngine.recoverFromError();
      _cio->clear();
      // handler numeric input
      _cio->onDigit(digit);
      _inputPending = true;
    }
  }

  // called if a control key was pressed
  void controlInput(operation op)
  {
    _calcEngine.handleControlInput(op);
    // only accept input if calc engine not in error state
    if (_calcEngine.getOperationReturnCode() == operation_return_code::success)
    {
      switch (op)
      {
#if CALC_TYPE == CALC_TYPE_RPN
      case operation::backspace:
#else
      case operation::clear:
#endif
        if (!_inputPending)
        {
          // just clear the result
          _calcEngine.clearResult();
          updateNumber();
        }
        else
        {
          _cio->onBackSpace();
        }
        break;

      case operation::change_sign:
        _cio->onChangeSign(_inputPending);
        if (!_inputPending)
        {
          _calcEngine.negateResult();
        }
        break;

      case operation::decimal_separator:
        if (!_inputPending)
        {
          _cio->clear();
          _inputPending = true;
        }
        _cio->onDecimalSeparator();
        break;

      case operation::exponent:
        if (!_inputPending)
        {
          _cio->clear();
          _inputPending = true;
        }
        _cio->onExponent();
        break;

      default: // avoid warning
        break;
      }
    }
  }

  // called if an operator key was pressed
  void operationInput(operation op)
  {
    if (_calcEngine.getOperationReturnCode() == operation_return_code::success)
    {
      if (_calcEngine.isLongOperation(op) && SettingsCache::showBusyCalc != show_busy_calc::off)
      {
        notifyLongOperation(long_operation::begin);
      }
      _calcEngine.onOperation(op);
      if (_calcEngine.isLongOperation(op) && SettingsCache::showBusyCalc != show_busy_calc::off)
      {
        notifyLongOperation(long_operation::end);
      }
    }
    else
    {
      // only accept specific operations if in error state
      if (_calcEngine.isErrorRecoveryOperation(op))
      {
        _calcEngine.onOperation(op);
      }
    }
  }

  // notify long operation events
  void notifyLongOperation(long_operation value)
  {
    if (_notifyLongOperation)
    {
      _notifyLongOperation(value);
    }
  }
};
