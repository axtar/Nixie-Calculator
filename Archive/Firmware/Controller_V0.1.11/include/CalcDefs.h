// CalcDefs.h

// Calculator definitions and enums

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>

// 0 for floating decimals, 1-8 for fixed decimals
constexpr uint8_t FLOAT_DECIMALS = 0;

// decimal separator char
constexpr char DECIMAL_SEPARATOR = '.';

// memory register count
#if RPN_MODE
constexpr int MEM_REGISTER_COUNT = 10;
#else
constexpr int MEM_REGISTER_COUNT = 1;
#endif

constexpr uint8_t MEM_REGISTER_NONE = 255;

// radix
constexpr uint32_t RAT_RADIX = 10;

#if RPN_MODE

// operations
enum class operation : uint8_t
{
  none,
  deg,
  rad,
  clear_x,
  clear_stack,
  pow,
  pow2,
  yroot,
  pow3,
  invert,
  factorial,
  exp,
  ln,
  e,
  modulo,
  logy,
  permutations,
  sin,
  asin,
  sinh,
  cos,
  acos,
  cosh,
  tan,
  atan,
  tanh,
  log10,
  pi,
  rnd,
  integer,
  combinations,
  percent_diff,
  square_root,
  percent,
  division,
  multiplication,
  subtraction,
  addition,
  enter,
  swap_xy,
  last_x,
  roll_down,
  roll_up,
  clear_memory,
  store,
  recall,
  backspace,
  exponent,
  change_sign,
  decimal_separator,
  clear_error,
  store_addition,
  store_subtraction,
  store_multiplication,
  store_division,
  recall_addition,
  recall_subtracion,
  recall_multiplication,
  recall_division
};

#else

enum class operation : uint8_t
{
  none,
  deg,
  rad,
  clear,
  allclear,
  pow,
  pow2,
  yroot,
  pow3,
  invert,
  factorial,
  exp,
  ln,
  e,
  modulo,
  logy,
  permutations,
  sin,
  asin,
  sinh,
  cos,
  acos,
  cosh,
  tan,
  atan,
  tanh,
  log10,
  pi,
  rnd,
  integer,
  combinations,
  percent_diff,
  square_root,
  percent,
  division,
  multiplication,
  subtraction,
  addition,
  equals,
  memory_clear,
  memory_read,
  memory_store,
  memory_addition,
  memory_subtraction,
  backspace,
  exponent,
  change_sign,
  decimal_separator,
  clear_error,
};

#endif

// angle types
enum class angle_type : uint8_t
{
  deg,
  rad
};
