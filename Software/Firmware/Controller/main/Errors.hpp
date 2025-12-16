// Errors.hpp

// Definition of error codes

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>

constexpr auto ERR_SUCCESS = 0;
constexpr auto ERR_INITSETTINGS = 1;

class Errors
{
public:
  Errors() = delete;

  // return error description
  static String getErrorText(int errorCode)
  {
    String text;

    switch (errorCode)
    {
    case ERR_SUCCESS:
      text = "Success";
      break;

    case ERR_INITSETTINGS:
      text = "Failed to initialize settings";
      break;
    }
    return (text);
  }
};
