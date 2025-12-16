// CalcEngineALG.hpp

// provides the algebraic calculator logic

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <ratpak.h>
#include <CalcDefs.h>
#include <MemRegister.hpp>
#include <CalcError.hpp>
#include <CalcEnums.h>
#include <CalcMath.hpp>

typedef std::map<String, PRAT> REGISTERMAP;

// calculator engine class
class CalcEngineALG
{
protected:
  using notifyRegisterUpdateCb = std::function<void(String regId, PRAT p)>;

public:
  // constructor
  CalcEngineALG() : _angleType(angle_type::deg),
                    _regX(nullptr),
                    _regY(nullptr),
                    _regT(nullptr),
                    _fixedDecimals(FLOAT_DECIMALS),
                    _operation(operation::none),
                    _notifyRegisterUpdate(nullptr)
  {
  }

  // clear all registers and memory
  void clear()
  {
    allClear();
    clearMemReg();
  }

  // clear all except memory
  void allClear()
  {
    setRegX(rat_zero);
    setRegY(rat_zero);
    setRegT(rat_zero);
    _operationReturnCode = operation_return_code::success;
    _operation = operation::none;
    _numberEntered = false;
    _equalsEntered = false;
    _calculationFlag = false;
  }

  // attach callback
  void attachNotifyRegisterUpdateCb(notifyRegisterUpdateCb callback)
  {
    _notifyRegisterUpdate = callback;
  }

  // detach callback
  void detachNotifyRegisterUpdateCb()
  {
    _notifyRegisterUpdate = nullptr;
  }

  // handle numeric input
  void handleNumericInput(PRAT p)
  {
    if (_operationReturnCode == operation_return_code::success)
    {
      if (_equalsEntered)
      {
        _equalsEntered = false;
        _operation = operation::none;
        setRegY(rat_zero);
        setRegT(rat_zero);
      }
      setRegX(p);
      _numberEntered = true;
      _calculationFlag = false;
    }
  }

  // return if a calculations could be slow
  bool isLongOperation(operation op) const
  {
    bool result = false;
    switch (op)
    {
    case operation::square_root:
    case operation::yroot:
    case operation::factorial:
    case operation::pow:
    case operation::pow2:
    case operation::pow3:
    case operation::exp:
    case operation::combinations:
    case operation::permutations:
      result = true;
      break;

    default: // avoid warning
      break;
    }
    return (result);
  }

  // return if the operation can recover from error
  bool isErrorRecoveryOperation(operation op)
  {
    bool result = false;
    switch (op)
    {
    case operation::clear:
    case operation::allclear:
      result = true;
      break;

    default: // avoid warning
      break;
    }
    return (result);
  }

  // clear error state
  void recoverFromError()
  {
    onOperation(operation::clear_error);
  }

  // generic function to clear the X register
  void clearResult()
  {
    onOperation(operation::clear);
  }

  // generic function to set the X register
  void setResult(PRAT p)
  {
    setRegX(p);
    if (_notifyRegisterUpdate)
    {
      _notifyRegisterUpdate("X:", p);
    }
  }

  // generic function that returns the X register
  PRAT getResult() const
  {
    return (_regX);
  }

  // generic function to change sign of X register
  void negateResult()
  {
    _regX->pp->sign *= -1;
    if (_notifyRegisterUpdate)
    {
      _notifyRegisterUpdate("X:", _regX);
    }
  }

  // call to enter an operation
  void onOperation(operation op, uint8_t digit = 0)
  {
    _calculationFlag = false;

    switch (op)
    {
    case operation::percent:
      onPercentOperation(op);
      break;

    case operation::equals:
      onEqualsOperation(op);
      break;

    case operation::clear:
    case operation::allclear:
    case operation::clear_error:
      onClearOperation(op);
      break;

    case operation::pow2:
    case operation::square_root:
    case operation::pow3:
    case operation::factorial:
    case operation::ln:
    case operation::log10:
    case operation::invert:
    case operation::sin:
    case operation::asin:
    case operation::sinh:
    case operation::cos:
    case operation::acos:
    case operation::cosh:
    case operation::tan:
    case operation::atan:
    case operation::tanh:
    case operation::exp:
    case operation::integer:
      onSingleValueOperation(op);
      break;

    case operation::addition:
    case operation::subtraction:
    case operation::multiplication:
    case operation::division:
    case operation::pow:
    case operation::yroot:
    case operation::logy:
    case operation::percent_diff:
    case operation::modulo:
    case operation::permutations:
    case operation::combinations:
      onDualValueOperation(op);
      break;

    case operation::pi:
    case operation::e:
    case operation::rnd: // behaves as a constant operation
      onConstantOperation(op);
      break;

    case operation::memory_clear:
    case operation::memory_read:
    case operation::memory_store:
    case operation::memory_addition:
    case operation::memory_subtraction:
      onMemRegOperation(op);
      break;

    case operation::deg:
      changeAngleType();
      break;

    default: // avoid warning
      break;
    }
    // after an operation notify register changes
    notifyRegisterUpdate();
    _calculationFlag = true;
  }

  // give the engine the opportunity to process digit input
  bool handleDigitInput(uint8_t digit, uint8_t *index)
  {
    return (false);
  }

  // give the engine the opportunity to process control input
  bool handleControlInput(operation op)
  {
    return (false);
  }

  // return the operation return code: success or an error code
  operation_return_code getOperationReturnCode() const
  {
    return (_operationReturnCode);
  }

  // return angle mode, deg or rad
  angle_type getAngleType() const
  {
    return (_angleType);
  }

  // set the angle mode for trigonometric operations, deg (default) or rad
  void setAngleType(angle_type angleType)
  {
    _angleType = angleType;
  }

  // this flag is set after a calculation
  bool isCalculation() const
  {
    return (_calculationFlag);
  }

  // reset the calculation flag
  void resetCalculationFlag()
  {
    _calculationFlag = false;
  }

  // set operation return code from a ratpak error
  void setOperationReturnCodeFromRatError(uint32_t ratError)
  {
    _operationReturnCode = CalcError::toOperationReturnCode(ratError);
  }

  // set fixed decimals
  void setFixedDecimals(uint8_t decimals)
  {
    _fixedDecimals = decimals;
  }

  // set radix
  void setRadix(uint32_t radix)
  {
    _radix = radix;
  }

  // set precision
  void setPrecision(int32_t precision)
  {
    _precision = precision;
  }

  // set the biggest value for trigonometric operations
  void setMaxTrig()
  {
    PRAT p = i32torat(100);
    PRAT q = nullptr;
    DUPRAT(q, rat_ten);
    powrat(&q, p, _radix, _precision);
    DUPRAT(_maxTrig, q);
    destroyrat(q);
    destroyrat(p);
  }

  // get fixed decimals
  uint8_t getFixedDecimals() const
  {
    return (_fixedDecimals);
  }

  // set X register
  void setRegX(PRAT p)
  {
    DUPRAT(_regX, p);
  }

  // get X register
  PRAT getRegX() const
  {
    return (_regX);
  }

  // set Y register
  void setRegY(PRAT p)
  {
    DUPRAT(_regY, p);
  }

  // get Y register
  PRAT getRegY() const
  {
    return (_regY);
  }

  // set T register
  void setRegT(PRAT p)
  {
    DUPRAT(_regT, p);
  }

  // get T register
  PRAT getRegT() const
  {
    return (_regT);
  }

  // set memory register
  void setMemReg(PRAT p, uint8_t index)
  {
    if (index < MEM_REGISTER_COUNT)
    {
      _memReg[index].set(p);
    }
    // notify memory register update
    if (_notifyRegisterUpdate)
    {
      _notifyRegisterUpdate("M:", _memReg[index].get());
    }
  }

  // get memory register
  PRAT getMemReg(uint8_t index)
  {
    if (index < MEM_REGISTER_COUNT)
    {
      return (_memReg[index].get());
    }
    else
    {
      return (nullptr);
    }
  }

  // return string representation of a PRAT
  String getRatString(PRAT p, NumberFormat format = NumberFormat::Float)
  {
    String s;
    if (format == NumberFormat::Float)
    {
      s = RatToString(p, NumberFormat::Float, _radix, _precision).c_str();
    }
    else
    {
      s = RatToScientificString(p, _radix, _precision).c_str();
    }
    return (s);
  }

  // return a map with all the registers
  void getRegisters(REGISTERMAP &regmap)
  {
    regmap["X:"] = _regX;
    regmap["Y:"] = _regY;
    regmap["T:"] = _regT;
    for (uint8_t i = 0; i < MEM_REGISTER_COUNT; i++)
    {
      regmap["M:"] = getMemReg(i);
    }
  }

private:
  uint32_t _radix;
  int32_t _precision;

  // angle mode
  angle_type _angleType;

  // calc registers
  PRAT _regX;
  PRAT _regY;
  PRAT _regT;

  // memory registers
  MemRegister _memReg[MEM_REGISTER_COUNT];

  // number of fixed decimals, default is floating
  uint8_t _fixedDecimals;

  // current operation
  operation _operation;

  // return code of math operations
  operation_return_code _operationReturnCode;

  // state variables
  bool _calculationFlag;
  bool _equalsEntered;
  bool _numberEntered;

  // max value for trigonometric operations
  PRAT _maxTrig;

  // callback
  notifyRegisterUpdateCb _notifyRegisterUpdate;

  // equals operation
  void onEqualsOperation(operation op)
  {
    if (_operation != operation::none)
    {
      // we have an operation
      if (!_equalsEntered)
      {
        // first equals
        setRegT(_regX);
        _operationReturnCode = CalcMath::calculate(&_regX, _regY, _operation, _radix, _precision, _maxTrig, _angleType);
        _equalsEntered = true;
      }
      else
      {
        // equals after equals, repeat previous operation
        setRegY(_regX);
        PRAT p = nullptr;
        DUPRAT(p, _regT);
        _operationReturnCode = CalcMath::calculate(&p, _regX, _operation, _radix, _precision, _maxTrig, _angleType);
        setRegX(p);
        destroyrat(p);
      }
    }
    _numberEntered = false;
  }

  // separate function for percent, percent has a special behavior
  void onPercentOperation(operation op)
  {
    PRAT p100 = nullptr;
    PRAT p = nullptr;
    p100 = i32torat(100);

    if ((_operation == operation::none) || _equalsEntered)
    {
      DUPRAT(p, p100)
      // no previous operation, just divide by 100
      _operationReturnCode = CalcMath::calculate(&p, _regX, operation::division, _radix, _precision);
      setRegX(p);
      setRegY(_regX);
      _numberEntered = false;
    }
    else
    {
      // if previous operation
      switch (_operation)
      {
      case operation::addition:
      case operation::subtraction:

        DUPRAT(p, p100);
        _operationReturnCode = CalcMath::calculate(&p, _regX, operation::division, _radix, _precision);
        if (_operationReturnCode == operation_return_code::success)
        {
          _operationReturnCode = CalcMath::calculate(&p, _regY, operation::multiplication, _radix, _precision);
          setRegX(p);
        }
        break;

      case operation::multiplication:
      case operation::division:
        DUPRAT(p, p100);
        _operationReturnCode = CalcMath::calculate(&p, _regX, operation::division, _radix, _precision);
        if (_operationReturnCode == operation_return_code::success)
        {
          setRegX(p);
        }
        break;

      default: // avoid warnings
        break;
      }
    }
    destroyrat(p100);
    destroyrat(p);
  }

  // clear operations
  void onClearOperation(operation op)
  {
    switch (op)
    {
    case operation::allclear:
      allClear();
      break;

    case operation::clear:
      setRegX(rat_zero);
      break;

    case operation::clear_error:
      _operationReturnCode = operation_return_code::success;
      break;

    default: // avoid warnings
      break;
    }
  }

  // perform an operation with a single value
  void onSingleValueOperation(operation op)
  {
    if (_operation == operation::none)
    {
      _operationReturnCode = CalcMath::calculate(&_regX, _regY, op, _radix, _precision, _maxTrig, _angleType);
      setRegY(_regX);
    }
    else
    {
      _operationReturnCode = CalcMath::calculate(&_regX, _regY, op, _radix, _precision, _maxTrig, _angleType);
    }
  }

  // perform operation with 2 values
  void onDualValueOperation(operation op)
  {
    if (_operation == operation::none)
    {
      // no operation entered yet
      setRegY(_regX);
      _operation = op;
    }
    else if (!_numberEntered)
    {
      if (_equalsEntered)
      {
        // new operation after equals
        setRegY(_regX);
      }
      // no new number entered, just change operation
      _operation = op;
    }
    else
    {
      // do calculation
      _operationReturnCode = CalcMath::calculate(&_regX, _regY, _operation, _radix, _precision, _maxTrig, _angleType);
      setRegY(_regX);
      _operation = op;
    }
    _numberEntered = false;
    _equalsEntered = false;
  }

  // perform operations with constants
  void onConstantOperation(operation op)
  {
    switch (op)
    {
    case operation::pi:
    case operation::e:
    case operation::rnd:
      CalcMath::getSpecialValue(&_regX, op, _radix, _precision);
      break;

    default: // avoid warning
      break;
    }
  }

  // handle memory register operations
  void onMemRegOperation(operation op)
  {
    PRAT p = nullptr;
    switch (op)
    {
    case operation::memory_clear:
      setMemReg(rat_zero, 0);
      break;

    case operation::memory_store:
      setMemReg(_regX, 0);
      break;

    case operation::memory_read:
      setRegX(getMemReg(0));
      break;

    case operation::memory_addition:
      p = getMemReg(0);
      _operationReturnCode = CalcMath::calculate(&p, _regX, operation::addition, _radix, _precision);
      break;

    case operation::memory_subtraction:
      DUPRAT(p, _regX);
      _operationReturnCode = CalcMath::calculate(&p, getMemReg(0), operation::subtraction, _radix, _precision);
      setMemReg(p, 0);
      destroyrat(p);
      break;

    default: // avoid warning
      break;
    }
    if (_notifyRegisterUpdate)
    {
      _notifyRegisterUpdate("M:", getMemReg(0));
    }
  }

  // change angle mode, deg<->rad
  void changeAngleType()
  {
    if (_angleType == angle_type::deg)
    {
      _angleType = angle_type::rad;
    }
    else
    {
      _angleType = angle_type::deg;
    }
  }

  // clear memory registers
  void clearMemReg()
  {
    for (int i = 0; i < MEM_REGISTER_COUNT; i++)
    {
      _memReg[i].clear();
    }
  }

  // notify register changes
  void notifyRegisterUpdate()
  {
    if (_notifyRegisterUpdate)
    {
      D_println("notifyStackUpdate");
      if (_operationReturnCode != operation_return_code::success)
      {
        PRAT p = nullptr;
        _notifyRegisterUpdate("X:", p);
      }
      else
      {
        _notifyRegisterUpdate("X:", _regX);
      }
      _notifyRegisterUpdate("Y:", _regY);
      _notifyRegisterUpdate("T:", _regT);
    }
  }

  // notify memory register changes
  void notifyMemRegUpdate()
  {
    if (_notifyRegisterUpdate)
    {
      D_println("notifyMemRegUpdate");
      for (uint8_t i = 0; i < MEM_REGISTER_COUNT; i++)
      {
        _notifyRegisterUpdate(String(i) + ":", getMemReg(i));
      }
    }
  }

  // print registers for debugging
  void printStack() const
  {
    D_print("T:      ");
    printRat(_regT);
    D_print("Y:      ");
    printRat(_regY);
    D_print("X:      ");
    printRat(_regX);
    D_print("Heap:   ");
    D_println(esp_get_free_heap_size());
    D_print("MinHeap:");
    D_println(esp_get_minimum_free_heap_size());
  }

  // print memory registers for debugging
  void printMemReg()
  {
    for (int i = 0; i < MEM_REGISTER_COUNT; i++)
    {
      D_print("Mem" + String(i) + ": ");
      printRat(getMemReg(i));
    }
  }

  // print a PRAT
  void printRat(PRAT p) const
  {
    D_println(std::string(RatToString(p, NumberFormat::Float, _radix, _precision)).c_str());
  }
};
