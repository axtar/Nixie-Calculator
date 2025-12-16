// CalcError.hpp

// error codes and mapping

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <ratpak.h>

// operation return codes
enum class operation_return_code : uint8_t
{
  success,
  overflow,
  divideByZero,
  domain,
  outofmemory,
  indefinite,
  invalidrange,
  unknownoperation,
  unknown
};

class CalcError
{

public:
  CalcError() = delete;

  // map ratpak error codes
  static operation_return_code toOperationReturnCode(uint32_t errorCode)
  {
    operation_return_code result;
    switch (errorCode)
    {
    case CALC_E_DIVIDEBYZERO:
      result = operation_return_code::divideByZero;
      break;

    case CALC_E_DOMAIN:
      result = operation_return_code::domain;
      break;

    case CALC_E_INDEFINITE:
      result = operation_return_code::indefinite;
      break;

    case CALC_E_INVALIDRANGE:
      result = operation_return_code::invalidrange;
      break;

    case CALC_E_OUTOFMEMORY:
      result = operation_return_code::outofmemory;
      break;

    case CALC_E_OVERFLOW:
      result = operation_return_code::overflow;
      break;

    default:
      result = operation_return_code::unknown;
      break;
    }
    return (result);
  }

  // get error information
  static String getErrorText(operation_return_code code)
  {
    String s;
    switch (code)
    {
    case operation_return_code::divideByZero:
      s = "Divide by zero";
      break;

    case operation_return_code::domain:
      s = "Invalid input / domain";
      break;

    case operation_return_code::indefinite:
      s = "Result is undefined";
      break;

    case operation_return_code::invalidrange:
      s = "Invalid range";
      break;

    case operation_return_code::outofmemory:
      s = "Out of memory";
      break;

    case operation_return_code::overflow:
      s = "Overflow";
      break;

    case operation_return_code::unknown:
      s = "Unknown error";
      break;

    case operation_return_code::unknownoperation:
      s = "Unknown operation";
      break;

    default: // avoid warning
      break;
    }
    return (s);
  }
};
