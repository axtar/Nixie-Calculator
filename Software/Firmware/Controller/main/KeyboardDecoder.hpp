// KeyboardDecoder.hpp

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <Config.h>
#include <KeyboardHandler.hpp>
#if CALC_TYPE == CALC_TYPE_RPN
#include <CalcEngineRPN.hpp>
#else
#include <CalcEngineALG.hpp>
#endif

enum class key_function_type
{
  unknown,
  numeric,
  numericx2,
  operation,
  control
};

class KeyboardDecoder
{
public:
  KeyboardDecoder() = delete;

#if CALC_TYPE == CALC_TYPE_RPN

  // provide summarized information about a keyboard event (RPN mode)
  static void decode(uint8_t keyCode, bool functionKeyPressed, bool shiftKeyPressed, key_function_type *function, operation *op, uint8_t *digit)
  {

    *op = operation::none;
    *digit = -1;
    *function = key_function_type::unknown;

    if (!functionKeyPressed && !shiftKeyPressed)
    {
      // no modifier key pressed
      switch (keyCode)
      {
      case KEY_0:
        *function = key_function_type::numeric;
        *digit = 0;
        break;

      case KEY_1:
        *function = key_function_type::numeric;
        *digit = 1;
        break;

      case KEY_2:
        *function = key_function_type::numeric;
        *digit = 2;
        break;
      case KEY_3:
        *function = key_function_type::numeric;
        *digit = 3;
        break;

      case KEY_4:
        *function = key_function_type::numeric;
        *digit = 4;
        break;

      case KEY_5:
        *function = key_function_type::numeric;
        *digit = 5;
        break;

      case KEY_6:
        *function = key_function_type::numeric;
        *digit = 6;
        break;

      case KEY_7:
        *function = key_function_type::numeric;
        *digit = 7;
        break;

      case KEY_8:
        *function = key_function_type::numeric;
        *digit = 8;
        break;

      case KEY_9:
        *function = key_function_type::numeric;
        *digit = 9;
        break;

      case KEY_00:
        *function = key_function_type::numericx2;
        *digit = 0;
        break;

      case KEY_DEG:
        *function = key_function_type::operation;
        *op = operation::deg;
        break;

      case KEY_BACK:
        *function = key_function_type::control;
        *op = operation::backspace;
        break;

      case KEY_CLS:
        *function = key_function_type::operation;
        *op = operation::clear_stack;
        break;

      case KEY_SHIFT:
        // modifier key, handled separately
        break;

      case KEY_F:
        // modifier key, handled separately
        break;

      case KEY_POW:
        *function = key_function_type::operation;
        *op = operation::pow;
        break;

      case KEY_YROOT:
        *function = key_function_type::operation;
        *op = operation::yroot;
        break;

      case KEY_INV:
        *function = key_function_type::operation;
        *op = operation::invert;
        break;

      case KEY_LN:
        *function = key_function_type::operation;
        *op = operation::ln;
        break;

      case KEY_LOGY:
        *function = key_function_type::operation;
        *op = operation::logy;
        break;

      case KEY_SIN:
        *function = key_function_type::operation;
        *op = operation::sin;
        break;

      case KEY_COS:
        *function = key_function_type::operation;
        *op = operation::cos;
        break;

      case KEY_TAN:
        *function = key_function_type::operation;
        *op = operation::tan;
        break;

      case KEY_LOG:
        *function = key_function_type::operation;
        *op = operation::log10;
        break;

      case KEY_EXP:
        *function = key_function_type::control;
        *op = operation::exponent;
        break;

      case KEY_CHS:
        *function = key_function_type::control;
        *op = operation::change_sign;
        break;

      case KEY_SQRT:
        *function = key_function_type::operation;
        *op = operation::square_root;
        break;

      case KEY_PCT:
        *function = key_function_type::operation;
        *op = operation::percent;
        break;

      case KEY_DOT:
        *function = key_function_type::control;
        *op = operation::decimal_separator;
        break;

      case KEY_DIV:
        *function = key_function_type::operation;
        *op = operation::division;
        break;

      case KEY_MUL:
        *function = key_function_type::operation;
        *op = operation::multiplication;
        break;

      case KEY_MINUS:
        *function = key_function_type::operation;
        *op = operation::subtraction;
        break;

      case KEY_PLUS:
        *function = key_function_type::operation;
        *op = operation::addition;
        break;

      case KEY_ENTER:
        *function = key_function_type::operation;
        *op = operation::enter;
        break;

      case KEY_XY:
        *function = key_function_type::operation;
        *op = operation::swap_xy;
        break;

      case KEY_ROLL:
        *function = key_function_type::operation;
        *op = operation::roll_down;
        break;

      case KEY_CLR:
        *function = key_function_type::operation;
        *op = operation::clear_memory;
        break;

      case KEY_STO:
        *function = key_function_type::operation;
        *op = operation::store;
        break;

      case KEY_RCL:
        *function = key_function_type::operation;
        *op = operation::recall;
        break;
      }
    }
    else if (functionKeyPressed && shiftKeyPressed)
    {
      // both modifier keys pressed, nothing defined for now
    }
    else if (functionKeyPressed)
    {
      switch (keyCode)
      {
      case KEY_0:
        *function = key_function_type::numeric;
        *digit = 0;
        break;

      case KEY_1:
        *function = key_function_type::numeric;
        *digit = 1;
        break;

      case KEY_2:
        *function = key_function_type::numeric;
        *digit = 2;
        break;
      case KEY_3:
        *function = key_function_type::numeric;
        *digit = 3;
        break;

      case KEY_4:
        *function = key_function_type::numeric;
        *digit = 4;
        break;

      case KEY_5:
        *function = key_function_type::numeric;
        *digit = 5;
        break;

      case KEY_6:
        *function = key_function_type::numeric;
        *digit = 6;
        break;

      case KEY_7:
        *function = key_function_type::numeric;
        *digit = 7;
        break;

      case KEY_8:
        *function = key_function_type::numeric;
        *digit = 8;
        break;

      case KEY_9:
        // not defined
        break;

      case KEY_00:
        // used as shortcut in calculator mode
        break;

      case KEY_DEG:
        *function = key_function_type::operation;
        *op = operation::percent_diff;
        break;

      case KEY_BACK:
        // used as shortcut in all modes
        break;

      case KEY_CLS:
        // used as shortcut in all modes
        break;

      case KEY_SHIFT:
        // modifier key, handled separately
        break;

      case KEY_F:
        // modifier key, handled separately
        break;

      case KEY_POW:
        *function = key_function_type::operation;
        *op = operation::pow2;
        break;

      case KEY_YROOT:
        *function = key_function_type::operation;
        *op = operation::pow3;
        break;

      case KEY_INV:
        *function = key_function_type::operation;
        *op = operation::exp;
        break;

      case KEY_LN:
        *function = key_function_type::operation;
        *op = operation::modulo;
        break;

      case KEY_LOGY:
        *function = key_function_type::operation;
        *op = operation::integer;
        break;

      case KEY_SIN:
        *function = key_function_type::operation;
        *op = operation::sinh;
        break;

      case KEY_COS:
        *function = key_function_type::operation;
        *op = operation::cosh;
        break;

      case KEY_TAN:
        *function = key_function_type::operation;
        *op = operation::tanh;
        break;

      case KEY_LOG:
        *function = key_function_type::operation;
        *op = operation::rnd;
        break;

      case KEY_EXP:
        // used as shortcut in calculator mode
        break;

      case KEY_CHS:
        // not defined
        break;

      case KEY_SQRT:
        // not defined
        break;

      case KEY_PCT:
        // not defined
        break;

      case KEY_DOT:
        // used as shortcut in calculator mode
        break;

      case KEY_DIV:
        // not defined
        break;

      case KEY_MUL:
        // not defined
        break;

      case KEY_MINUS:
        // used as shortcut in clock mode
        break;

      case KEY_PLUS:
        // used as shortcut in clock mode
        break;

      case KEY_ENTER:
        // used as shortcut in calculator mode
        break;

      case KEY_XY:
        // not defined
        break;

      case KEY_ROLL:
        // not defined
        break;

      case KEY_CLR:
        // used as shortcut in menu mode
        break;

      case KEY_STO:
        // used as shortcut in clock mode
        // used as shortcut in calculator mode
        break;

      case KEY_RCL:
        // used as shortcut in calculator mode
        break;
      }
    }
    else if (shiftKeyPressed)
    {
      switch (keyCode)
      {
      case KEY_0:
        // not defined
        break;

      case KEY_1:
        // not defined
        break;

      case KEY_2:
        // not defined
        break;
      case KEY_3:
        // not defined
        break;

      case KEY_4:
        // not defined
        break;

      case KEY_5:
        // not defined
        break;

      case KEY_6:
        // not defined
        break;

      case KEY_7:
        // not defined
        break;

      case KEY_8:
        // not defined
        break;

      case KEY_9:
        // not defined
        break;

      case KEY_00:
        // not defined
        break;

      case KEY_DEG:
        *function = key_function_type::operation;
        *op = operation::combinations;
        break;

      case KEY_BACK:
        *function = key_function_type::operation;
        *op = operation::clear_x;
        break;

      case KEY_CLS:
        // not defined
        break;

      case KEY_SHIFT:
        // modifier key, handled separately
        break;

      case KEY_F:
        // modifier key, handled separately
        break;

      case KEY_POW:
        // not defined
        break;

      case KEY_YROOT:
        // not defined
        break;

      case KEY_INV:
        *function = key_function_type::operation;
        *op = operation::factorial;
        break;

      case KEY_LN:
        *function = key_function_type::operation;
        *op = operation::e;
        break;

      case KEY_LOGY:
        *function = key_function_type::operation;
        *op = operation::permutations;
        break;

      case KEY_SIN:
        *function = key_function_type::operation;
        *op = operation::asin;
        break;

      case KEY_COS:
        *function = key_function_type::operation;
        *op = operation::acos;
        break;

      case KEY_TAN:
        *function = key_function_type::operation;
        *op = operation::atan;
        break;

      case KEY_LOG:
        *function = key_function_type::operation;
        *op = operation::pi;
        break;

      case KEY_EXP:
        // not defined
        break;

      case KEY_CHS:
        // not defined
        break;

      case KEY_SQRT:
        // not defined
        break;

      case KEY_PCT:
        // not defined
        break;

      case KEY_DOT:
        // not defined
        break;

      case KEY_DIV:
        // not defined
        break;

      case KEY_MUL:
        // not defined
        break;

      case KEY_MINUS:
        // not defined
        break;

      case KEY_PLUS:
        // not defined
        break;

      case KEY_ENTER:
        // not defined
        break;

      case KEY_XY:
        *function = key_function_type::operation;
        *op = operation::last_x;
        break;

      case KEY_ROLL:
        *function = key_function_type::operation;
        *op = operation::roll_up;
        break;

      case KEY_CLR:
        // not defined
        break;

      case KEY_STO:
        // not defined
        break;

      case KEY_RCL:
        // not defined
        break;
      }
    }
  }

#else

  // provide summarized information about a keyboard event (ALG mode)
  static void decode(uint8_t keyCode, bool functionKeyPressed, bool shiftKeyPressed, key_function_type *function, operation *op, uint8_t *digit)
  {

    *op = operation::none;
    *digit = -1;
    *function = key_function_type::unknown;

    if (!functionKeyPressed && !shiftKeyPressed)
    {
      // no modifier key pressed
      switch (keyCode)
      {
      case KEY_0:
        *function = key_function_type::numeric;
        *digit = 0;
        break;

      case KEY_1:
        *function = key_function_type::numeric;
        *digit = 1;
        break;

      case KEY_2:
        *function = key_function_type::numeric;
        *digit = 2;
        break;
      case KEY_3:
        *function = key_function_type::numeric;
        *digit = 3;
        break;

      case KEY_4:
        *function = key_function_type::numeric;
        *digit = 4;
        break;

      case KEY_5:
        *function = key_function_type::numeric;
        *digit = 5;
        break;

      case KEY_6:
        *function = key_function_type::numeric;
        *digit = 6;
        break;

      case KEY_7:
        *function = key_function_type::numeric;
        *digit = 7;
        break;

      case KEY_8:
        *function = key_function_type::numeric;
        *digit = 8;
        break;

      case KEY_9:
        *function = key_function_type::numeric;
        *digit = 9;
        break;

      case KEY_00:
        *function = key_function_type::numericx2;
        *digit = 0;
        break;

      case KEY_DEG:
        *function = key_function_type::operation;
        *op = operation::deg;
        break;

      case KEY_C:
        *function = key_function_type::control;
        *op = operation::clear;
        break;

      case KEY_AC:
        *function = key_function_type::operation;
        *op = operation::allclear;
        break;

      case KEY_SHIFT:
        // modifier key, handled separately
        break;

      case KEY_F:
        // modifier key, handled separately
        break;

      case KEY_POW:
        *function = key_function_type::operation;
        *op = operation::pow;
        break;

      case KEY_YROOT:
        *function = key_function_type::operation;
        *op = operation::yroot;
        break;

      case KEY_INV:
        *function = key_function_type::operation;
        *op = operation::invert;
        break;

      case KEY_LN:
        *function = key_function_type::operation;
        *op = operation::ln;
        break;

      case KEY_LOGY:
        *function = key_function_type::operation;
        *op = operation::logy;
        break;

      case KEY_SIN:
        *function = key_function_type::operation;
        *op = operation::sin;
        break;

      case KEY_COS:
        *function = key_function_type::operation;
        *op = operation::cos;
        break;

      case KEY_TAN:
        *function = key_function_type::operation;
        *op = operation::tan;
        break;

      case KEY_LOG:
        *function = key_function_type::operation;
        *op = operation::log10;
        break;

      case KEY_EXP:
        *function = key_function_type::control;
        *op = operation::exponent;
        break;

      case KEY_CHS:
        *function = key_function_type::control;
        *op = operation::change_sign;
        break;

      case KEY_SQRT:
        *function = key_function_type::operation;
        *op = operation::square_root;
        break;

      case KEY_PCT:
        *function = key_function_type::operation;
        *op = operation::percent;
        break;

      case KEY_DOT:
        *function = key_function_type::control;
        *op = operation::decimal_separator;
        break;

      case KEY_DIV:
        *function = key_function_type::operation;
        *op = operation::division;
        break;

      case KEY_MUL:
        *function = key_function_type::operation;
        *op = operation::multiplication;
        break;

      case KEY_MINUS:
        *function = key_function_type::operation;
        *op = operation::subtraction;
        break;

      case KEY_PLUS:
        *function = key_function_type::operation;
        *op = operation::addition;
        break;

      case KEY_EQUALS:
        *function = key_function_type::operation;
        *op = operation::equals;
        break;

      case KEY_MC:
        *function = key_function_type::operation;
        *op = operation::memory_clear;
        break;

      case KEY_MR:
        *function = key_function_type::operation;
        *op = operation::memory_read;
        break;

      case KEY_MS:
        *function = key_function_type::operation;
        *op = operation::memory_store;
        break;

      case KEY_MPLUS:
        *function = key_function_type::operation;
        *op = operation::memory_addition;
        break;

      case KEY_MMINUS:
        *function = key_function_type::operation;
        *op = operation::memory_subtraction;
        break;
      }
    }
    else if (functionKeyPressed && shiftKeyPressed)
    {
      // both modifier keys pressed, nothing defined for now
    }
    else if (functionKeyPressed)
    {
      switch (keyCode)
      {
      case KEY_0:
        *function = key_function_type::numeric;
        *digit = 0;
        break;

      case KEY_1:
        *function = key_function_type::numeric;
        *digit = 1;
        break;

      case KEY_2:
        *function = key_function_type::numeric;
        *digit = 2;
        break;
      case KEY_3:
        *function = key_function_type::numeric;
        *digit = 3;
        break;

      case KEY_4:
        *function = key_function_type::numeric;
        *digit = 4;
        break;

      case KEY_5:
        *function = key_function_type::numeric;
        *digit = 5;
        break;

      case KEY_6:
        *function = key_function_type::numeric;
        *digit = 6;
        break;

      case KEY_7:
        *function = key_function_type::numeric;
        *digit = 7;
        break;

      case KEY_8:
        *function = key_function_type::numeric;
        *digit = 8;
        break;

      case KEY_9:
        // not defined
        break;

      case KEY_00:
        // not defined
        break;

      case KEY_DEG:
        *function = key_function_type::operation;
        *op = operation::percent_diff;
        break;

      case KEY_C:
        // used as shortcut in all modes
        break;

      case KEY_AC:
        // used as shortcut in all modes
        break;

      case KEY_SHIFT:
        // modifier key, handled separately
        break;

      case KEY_F:
        // modifier key, handled separately
        break;

      case KEY_POW:
        *function = key_function_type::operation;
        *op = operation::pow2;
        break;

      case KEY_YROOT:
        *function = key_function_type::operation;
        *op = operation::pow3;
        break;

      case KEY_INV:
        *function = key_function_type::operation;
        *op = operation::exp;
        break;

      case KEY_LN:
        *function = key_function_type::operation;
        *op = operation::modulo;
        break;

      case KEY_LOGY:
        *function = key_function_type::operation;
        *op = operation::integer;
        break;

      case KEY_SIN:
        *function = key_function_type::operation;
        *op = operation::sinh;
        break;

      case KEY_COS:
        *function = key_function_type::operation;
        *op = operation::cosh;
        break;

      case KEY_TAN:
        *function = key_function_type::operation;
        *op = operation::tanh;
        break;

      case KEY_LOG:
        *function = key_function_type::operation;
        *op = operation::rnd;
        break;

      case KEY_EXP:
        // used as shortcut in calculator mode
        break;

      case KEY_CHS:
        // not defined
        break;

      case KEY_SQRT:
        // not defined
        break;

      case KEY_PCT:
        // not defined
        break;

      case KEY_DOT:
        // used as shortcut in calculator mode
        break;

      case KEY_DIV:
        // not defined
        break;

      case KEY_MUL:
        // not defined
        break;

      case KEY_MINUS:
        // used as shortcut in clock mode
        break;

      case KEY_PLUS:
        // used as shortcut in clock mode
        break;

      case KEY_EQUALS:
        // used as shortcut in calculator mode
        break;

      case KEY_MC:
        // not defined
        break;

      case KEY_MR:
        // not defined
        break;

      case KEY_MS:
        // used as shortcut in menu mode
        break;

      case KEY_MPLUS:
        // used as shortcut in clock mode
        // used as shortcut in calculator mode
        break;

      case KEY_MMINUS:
        // used as shortcut in calculator mode
        break;
      }
    }
    else if (shiftKeyPressed)
    {
      switch (keyCode)
      {
      case KEY_0:
        // not defined
        break;

      case KEY_1:
        // not defined
        break;

      case KEY_2:
        // not defined
        break;
      case KEY_3:
        // not defined
        break;

      case KEY_4:
        // not defined
        break;

      case KEY_5:
        // not defined
        break;

      case KEY_6:
        // not defined
        break;

      case KEY_7:
        // not defined
        break;

      case KEY_8:
        // not defined
        break;

      case KEY_9:
        // not defined
        break;

      case KEY_00:
        // not defined
        break;

      case KEY_DEG:
        *function = key_function_type::operation;
        *op = operation::combinations;
        break;

      case KEY_C:
        // not defined
        break;

      case KEY_AC:
        // not defined
        break;

      case KEY_SHIFT:
        // modifier key, handled separately
        break;

      case KEY_F:
        // modifier key, handled separately
        break;

      case KEY_POW:
        // not defined
        break;

      case KEY_YROOT:
        // not defined
        break;

      case KEY_INV:
        *function = key_function_type::operation;
        *op = operation::factorial;
        break;

      case KEY_LN:
        *function = key_function_type::operation;
        *op = operation::e;
        break;

      case KEY_LOGY:
        *function = key_function_type::operation;
        *op = operation::permutations;
        break;

      case KEY_SIN:
        *function = key_function_type::operation;
        *op = operation::asin;
        break;

      case KEY_COS:
        *function = key_function_type::operation;
        *op = operation::acos;
        break;

      case KEY_TAN:
        *function = key_function_type::operation;
        *op = operation::atan;
        break;

      case KEY_LOG:
        *function = key_function_type::operation;
        *op = operation::pi;
        break;

      case KEY_EXP:
        // not defined
        break;

      case KEY_CHS:
        // not defined
        break;

      case KEY_SQRT:
        // not defined
        break;

      case KEY_PCT:
        // not defined
        break;

      case KEY_DOT:
        // not defined
        break;

      case KEY_DIV:
        // not defined
        break;

      case KEY_MUL:
        // not defined
        break;

      case KEY_MINUS:
        // not defined
        break;

      case KEY_PLUS:
        // not defined
        break;

      case KEY_EQUALS:
        // not defined
        break;

      case KEY_MC:
        // not defined
        break;

      case KEY_MR:
        // not defined
        break;

      case KEY_MS:
        // not defined
        break;

      case KEY_MPLUS:
        // not defined
        break;

      case KEY_MMINUS:
        // not defined
        break;
      }
    }
  }

#endif
};