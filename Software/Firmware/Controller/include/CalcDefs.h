// CalcDefs.h

// Calculator definitions

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <Config.h>

// 0 for floating decimals, 1-8 for fixed decimals
constexpr uint8_t FLOAT_DECIMALS = 0;

// decimal separator char
constexpr char DECIMAL_SEPARATOR = '.';

// memory register count
#if CALC_TYPE == CALC_TYPE_RPN
constexpr int MEM_REGISTER_COUNT = 10;
#else
constexpr int MEM_REGISTER_COUNT = 1;
#endif

constexpr uint8_t MEM_REGISTER_NONE = 255;

// radix
constexpr uint32_t RAT_RADIX = 10;

// extra precision for rounding
constexpr int32_t EXTRA_PRECISION = 5;