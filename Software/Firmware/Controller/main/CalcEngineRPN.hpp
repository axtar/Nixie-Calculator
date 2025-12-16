// CalcEngineRPN.hpp

// provides the RPN calculator logic

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
class CalcEngineRPN
{
protected:
  using notifyRegisterUpdateCb = std::function<void(String regId, PRAT p)>;

public:
  // constructor
  CalcEngineRPN() : _angleType(angle_type::deg),
                    _regX(nullptr),
                    _regY(nullptr),
                    _regZ(nullptr),
                    _regT(nullptr),
                    _regLastX(nullptr)
  {
    _fixedDecimals = FLOAT_DECIMALS;
    _storePending = false;
    _recallPending = false;
    _disableStackLift = false;
    _pendingMemMathOperation = operation::none;
    _notifyRegisterUpdate = nullptr;
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
    if (!getDisableStackLift())
    {
      stackLift();
    }
    else
    {
      setRegY(getRegX());
    }
    setRegX(p);
  }

  // clear all
  void clear()
  {
    onOperation(operation::clear_stack);
    clearMemReg();
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
    case operation::clear_x:
    case operation::clear_stack:
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
    onOperation(operation::clear_x);
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
    switch (op)
    {
    case operation::percent: // actually a dual value op but behaves as a single value op
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
      resetMemRegOperation();
      resetMemMathOperation();
      onSingleValueOperation(op);
      break;

    case operation::addition:
      if (_recallPending)
      {
        _pendingMemMathOperation = operation::recall_addition;
        break;
      }
      if (_storePending)
      {
        _pendingMemMathOperation = operation::store_addition;
        break;
      }
      [[fallthrough]];
    case operation::subtraction:
      if (_recallPending)
      {
        _pendingMemMathOperation = operation::recall_subtracion;
        break;
      }
      if (_storePending)
      {
        _pendingMemMathOperation = operation::store_subtraction;
        break;
      }
      [[fallthrough]];
    case operation::multiplication:
      if (_recallPending)
      {
        _pendingMemMathOperation = operation::recall_multiplication;
        break;
      }
      if (_storePending)
      {
        _pendingMemMathOperation = operation::store_multiplication;
        break;
      }
      [[fallthrough]];
    case operation::division:
      if (_recallPending)
      {
        _pendingMemMathOperation = operation::recall_division;
        break;
      }
      if (_storePending)
      {
        _pendingMemMathOperation = operation::store_division;
        break;
      }
      [[fallthrough]];
    case operation::pow:
    case operation::yroot:
    case operation::logy:
    case operation::percent_diff:
    case operation::modulo:
    case operation::permutations:
    case operation::combinations:
      resetMemRegOperation();
      resetMemMathOperation();
      onDualValueOperation(op);
      break;

    case operation::pi:
    case operation::e:
    case operation::rnd: // behaves as a constant operation
      resetMemRegOperation();
      resetMemMathOperation();
      onConstantOperation(op);
      break;

    case operation::clear_memory:
    case operation::recall:
    case operation::store:
      resetMemRegOperation();
      resetMemMathOperation();
      onMemRegOperation(op);
      break;

    case operation::store_addition:
    case operation::store_subtraction:
    case operation::store_multiplication:
    case operation::store_division:
    case operation::recall_addition:
    case operation::recall_subtracion:
    case operation::recall_multiplication:
    case operation::recall_division:
      resetMemRegOperation();
      resetMemMathOperation();
      onMemRegMathOperation(op, digit);
      break;

    case operation::clear_error:
    case operation::clear_x:
    case operation::enter:
    case operation::clear_stack:
    case operation::swap_xy:
    case operation::roll_down:
    case operation::roll_up:
    case operation::last_x:
      resetMemRegOperation();
      resetMemMathOperation();
      onStackOperation(op);
      break;

    case operation::deg:
      resetMemRegOperation();
      resetMemMathOperation();
      changeAngleType();
      break;

    default: // avoid warning
      break;
    }
    // after an operation notify stack register changes
    notifyStackUpdate();
  }

  // give the engine the opportunity to process pending store and recall operations
  bool handleDigitInput(uint8_t digit, uint8_t *index)
  {
    bool result = false;
    if (getStorePending())
    {
      operation op = getPendingMemMathOperation();
      if (op == operation::none)
      {
        // store is pending, save X register into memory register
        setMemReg(getRegX(), digit);
      }
      else
      {
        // do store memory math operation, the result is stored in memory register
        onOperation(op, digit);
        *index = digit;
      }
      result = true;
    }
    else if (getRecallPending())
    {
      if (!getDisableStackLift())
      {
        stackLift();
      }
      operation op = getPendingMemMathOperation();
      if (op == operation::none)
      {
        // recall is pending, copy memory register to X register
        setRegX(getMemReg(digit));
        *index = digit;
      }
      else
      {
        // do recall memory math operation, result is stored in the X register
        onOperation(op, digit);
      }
      result = true;
    }

    // clean up all pending memory operations if any
    resetMemRegOperation();
    resetMemMathOperation();
    if (result)
    {
      notifyStackUpdate();
    }
    return (result);
  }

  // if a control key was pressed just reset pending memory operations
  bool handleControlInput(operation op)
  {
    // clean up all pending memory operations if any
    resetMemRegOperation();
    resetMemMathOperation();
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

  bool getDisableStackLift()
  {
    return (_disableStackLift);
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

  // set Z register
  void setRegZ(PRAT p)
  {
    DUPRAT(_regZ, p);
  }

  // get Z register
  PRAT getRegZ() const
  {
    return (_regZ);
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

  // set lastX register
  void setRegLastX(PRAT p)
  {
    DUPRAT(_regLastX, p);
  }

  // get lastX register
  PRAT getRegLastX() const
  {
    return (_regLastX);
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
      _notifyRegisterUpdate(String(index) + ":", _memReg[index].get());
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
    regmap["Z:"] = _regZ;
    regmap["T:"] = _regT;
    regmap["L:"] = _regLastX;
    for (uint8_t i = 0; i < MEM_REGISTER_COUNT; i++)
    {
      regmap[String(i) + ":"] = getMemReg(i);
    }
  }

private:
  uint32_t _radix;
  int32_t _precision;

  // angle mode
  angle_type _angleType;

  // stack registers
  PRAT _regX;
  PRAT _regY;
  PRAT _regZ;
  PRAT _regT;
  PRAT _regLastX;

  // memory registers
  MemRegister _memReg[MEM_REGISTER_COUNT];

  // number of fixed decimals, default is floating
  uint8_t _fixedDecimals;

  // return code of math operations
  operation_return_code _operationReturnCode;

  // state variables
  bool _calculationFlag;
  bool _storePending;
  bool _recallPending;
  bool _disableStackLift;
  operation _pendingMemMathOperation;

  // max value for trigonometric operations
  PRAT _maxTrig;

  notifyRegisterUpdateCb _notifyRegisterUpdate;

  // perform an operation with a single value
  void onSingleValueOperation(operation op)
  {
    stackSetLastX();
    // calculateValue stores the result in the X register
    _operationReturnCode = CalcMath::calculate(&_regX, _regY, op, _radix, _precision, _maxTrig, _angleType);
    _disableStackLift = false;
  }

  // perform operation with 2 values
  void onDualValueOperation(operation op)
  {
    stackSetLastX();
    // calculateValue stores the result in the X register
    _operationReturnCode = CalcMath::calculate(&_regX, _regY, op, _radix, _precision, _maxTrig, _angleType);
    stackDrop();
    _disableStackLift = false;
  }

  // perform operations with constants
  void onConstantOperation(operation op)
  {
    if (!_disableStackLift)
    {
      stackLift();
    }
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
    _disableStackLift = false;
  }

  // handle memory register operations
  void onMemRegOperation(operation op)
  {
    switch (op)
    {
    case operation::clear_memory:
      clearMemReg();
      notifyMemRegUpdate();
      break;

    case operation::store:
      _storePending = true;
      break;

    case operation::recall:
      _recallPending = true;
      break;

    default: // avoid warning
      break;
    }
    _disableStackLift = false;
  }

  // handle stack operations
  void onStackOperation(operation op)
  {
    _disableStackLift = false;
    switch (op)
    {

    case operation::enter:
      stackLift();
      _disableStackLift = true; // this operation disables stack lift
      break;

    case operation::clear_x:
      stackClearX();
      _disableStackLift = true; // this operation disables stack lift
      break;

    case operation::clear_error:
      // do nothing, just clear the error code
      break;

    case operation::clear_stack:
      stackClear();
      break;

    case operation::last_x:
      stackGetLastX();
      break;

    case operation::swap_xy:
      stackSwapXY();
      break;

    case operation::roll_down:
      stackRollDown();
      break;

    case operation::roll_up:
      stackRollUp();
      break;

    default: // avoid warning
      break;
    }
    _operationReturnCode = operation_return_code::success;
  }

  // do the memory math operations
  void onMemRegMathOperation(operation op, uint8_t digit)
  {
    _disableStackLift = true;
    _operationReturnCode = calculateMemMath(op, digit);
  }

  // stack operations:

  // stack lift
  void stackLift()
  {
    DUPRAT(_regT, _regZ);
    DUPRAT(_regZ, _regY);
    DUPRAT(_regY, _regX);
  }

  // adjust stack after an operation with 2 values
  void stackDrop()
  {
    DUPRAT(_regY, _regZ);
    DUPRAT(_regZ, _regT);
  }

  // clear X register
  void stackClearX()
  {
    DUPRAT(_regX, rat_zero);
  }

  // clear all stack registers
  void stackClear()
  {
    DUPRAT(_regX, rat_zero);
    DUPRAT(_regY, rat_zero);
    DUPRAT(_regZ, rat_zero);
    DUPRAT(_regT, rat_zero);
    DUPRAT(_regLastX, rat_zero);
  }

  // store X into lastX register
  void stackSetLastX()
  {
    DUPRAT(_regLastX, _regX);
  }

  // store lastX into X register
  void stackGetLastX()
  {
    if (!_disableStackLift)
    {
      stackLift();
    }
    DUPRAT(_regX, _regLastX);
  }

  // swap X and Y registers
  void stackSwapXY()
  {
    PRAT p = nullptr;
    DUPRAT(p, _regX);
    DUPRAT(_regX, _regY);
    DUPRAT(_regY, p);
    destroyrat(p);
  }

  // stack roll down
  void stackRollDown()
  {
    PRAT p = nullptr;
    DUPRAT(p, _regX);
    DUPRAT(_regX, _regY);
    DUPRAT(_regY, _regZ);
    DUPRAT(_regZ, _regT);
    DUPRAT(_regT, p);
    destroyrat(p);
  }

  // stack roll up
  void stackRollUp()
  {
    PRAT p = nullptr;
    DUPRAT(p, _regT);
    DUPRAT(_regT, _regZ);
    DUPRAT(_regZ, _regY);
    DUPRAT(_regY, _regX);
    DUPRAT(_regX, p);
    destroyrat(p);
  }

  // do the memory register math
  operation_return_code calculateMemMath(operation op, uint8_t digit)
  {
    PRAT p = nullptr;
    operation_return_code result = operation_return_code::success;
    switch (op)
    {
    case operation::store_addition:
      DUPRAT(p, getMemReg(digit));
      result = CalcMath::calculate(&p, _regX, operation::addition, _radix, _precision);
      if (result == operation_return_code::success)
      {
        setMemReg(p, digit);
      }
      break;

    case operation::store_subtraction:
      DUPRAT(p, getMemReg(digit));
      result = CalcMath::calculate(&p, _regX, operation::subtraction, _radix, _precision);
      if (result == operation_return_code::success)
      {
        setMemReg(p, digit);
      }
      break;

    case operation::store_multiplication:
      DUPRAT(p, getMemReg(digit));
      result = CalcMath::calculate(&p, _regX, operation::multiplication, _radix, _precision);
      if (result == operation_return_code::success)
      {
        setMemReg(p, digit);
      }
      break;

    case operation::store_division:
      DUPRAT(p, getMemReg(digit));
      result = CalcMath::calculate(&p, _regX, operation::division, _radix, _precision);
      if (result == operation_return_code::success)
      {
        setMemReg(p, digit);
      }
      break;

    case operation::recall_addition:
      result = CalcMath::calculate(&_regX, getMemReg(digit), operation::addition, _radix, _precision);
      break;

    case operation::recall_subtracion:
      result = CalcMath::calculate(&_regX, getMemReg(digit), operation::subtraction, _radix, _precision);
      break;

    case operation::recall_multiplication:
      result = CalcMath::calculate(&_regX, getMemReg(digit), operation::multiplication, _radix, _precision);
      break;

    case operation::recall_division:
      result = CalcMath::calculate(&_regX, getMemReg(digit), operation::division, _radix, _precision);
      break;

    default: // avoid warning
      break;
    }
    destroyrat(p);
    return (result);
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

  // true if store operation is pending
  bool getStorePending() const
  {
    return (_storePending);
  }

  // true if recall operation is pending
  bool getRecallPending() const
  {
    return (_recallPending);
  }

  // clear pending memory operations
  void resetMemRegOperation()
  {
    _storePending = false;
    _recallPending = false;
  }

  // get pending store or recall math operation
  operation getPendingMemMathOperation()
  {
    return (_pendingMemMathOperation);
  }

  // clear pending memory math operation
  void resetMemMathOperation()
  {
    _pendingMemMathOperation = operation::none;
  }

  // notify stack changes
  void notifyStackUpdate()
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
      _notifyRegisterUpdate("Z:", _regZ);
      _notifyRegisterUpdate("T:", _regT);
      _notifyRegisterUpdate("L:", _regLastX);
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

  // print stack registers for debugging
  void printStack() const
  {
    D_print("T:      ");
    printRat(_regT);
    D_print("Z:      ");
    printRat(_regZ);
    D_print("Y:      ");
    printRat(_regY);
    D_print("X:      ");
    printRat(_regX);
    D_print("lastX:  ");
    printRat(_regLastX);
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
