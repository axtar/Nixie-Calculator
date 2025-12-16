// HardwareInfo.h

// Provides information about availabe hardware support

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

// available display types
enum class display_type
{
  undefined,
  in12a, // if using neon bulbs as decimal point
  in12b, // if using IN-12B decimal point
  in16,
  in17,
  b5870,
  led
};

// calculator types
#define CALC_TYPE_UNDEFINED 0
#define CALC_TYPE_RPN 1
#define CALC_TYPE_ALG 2