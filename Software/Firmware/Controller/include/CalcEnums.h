// CalcEnums.h

// some system wide enums

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>

#if CALC_TYPE == CALC_TYPE_RPN

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
