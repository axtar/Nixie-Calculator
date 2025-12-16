// CalcMath.hpp

// provides higher level math operations
// uses arbitrary-precision arithmetic (experimental)

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <ratpak.h>
#include <CalcEnums.h>
#include <CalcError.hpp>

class CalcMath
{
public:
  CalcMath() = delete;

  // do the math and store the result in px
  // maxTrig and angletype are needed for trigonometric operations
  static operation_return_code calculate(PRAT *px, PRAT py, operation op, uint32_t radix, int32_t precision, PRAT maxTrig = rat_zero, angle_type angleType = angle_type::deg)
  {
    operation_return_code result = operation_return_code::success;
    PRAT p = nullptr;
    PRAT q = nullptr;
    switch (op)
    {
    case operation::ln: // natural logarithm
      try
      {
        lograt(px, precision);
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::log10: // logarithm base 10
      try
      {
        log10rat(px, precision);
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::logy: // logarithm base y
      try
      {
        DUPRAT(p, py);
        DUPRAT(q, *px);
        lograt(&p, precision);
        lograt(&q, precision);
        divrat(&p, q, precision);
        DUPRAT(*px, p);
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::integer: // remove fract part
      try
      {
        intrat(px, radix, precision);
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::square_root: // square root
    {
      try
      {
        if (SIGN(*px) == 1)
        {
          rootrat(px, rat_two, radix, precision);
        }
        else
        {
          throw(CALC_E_DOMAIN);
        }
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
    }
    break;

    case operation::yroot: // y-th root
      try
      {
        DUPRAT(p, py);
        rootrat(&p, *px, radix, precision);
        DUPRAT(*px, p);
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::exp: // exponential
      try
      {
        DUPRAT(p, rat_exp);
        powrat(&p, *px, radix, precision);
        DUPRAT(*px, p);
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::pow: // power
      try
      {
        DUPRAT(p, py);
        powrat(&p, *px, radix, precision);
        DUPRAT(*px, p);
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::pow2: // square
      try
      {
        powrat(px, rat_two, radix, precision);
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::pow3: // cubic
      try
      {
        p = i32torat(3);
        powrat(px, p, radix, precision);
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::factorial: // factorial
      try
      {
        factrat(px, radix, precision);
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::modulo: // modulo
      try
      {
        DUPRAT(p, py);
        modrat(&p, *px);
        DUPRAT(*px, p);
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::addition: // addition
      try
      {
        addrat(px, py, precision);
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::subtraction: // subtraction
      try
      {
        DUPRAT(p, py);
        subrat(&p, *px, precision);
        DUPRAT(*px, p);
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::multiplication: // multiplication
      try
      {
        mulrat(px, py, precision);
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::division: // division
      try
      {
        DUPRAT(p, py);
        divrat(&p, *px, precision);
        DUPRAT(*px, p);
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::invert: // reciprocal
      try
      {
        DUPRAT(p, rat_one);
        divrat(&p, *px, precision);
        DUPRAT(*px, p);
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::percent: // percent
      try
      {
        p = i32torat(100);
        mulrat(px, py, precision);
        divrat(px, p, precision);
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::percent_diff: // percent difference
      try
      {
        DUPRAT(p, py);
        DUPRAT(q, *px);
        subrat(&q, p, precision);
        DUPRAT(p, py);
        divrat(&q, p, precision);
        DUPRAT(*px, q);
        destroyrat(p);
        p = i32torat(100);
        mulrat(px, p, precision);
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::sin: // sine
      try
      {
        if (rat_lt(*px, maxTrig, precision))
        {
          sinanglerat(px, angleType == angle_type::deg ? AngleType::Degrees : AngleType::Radians, radix, precision);
        }
        else
        {
          throw(CALC_E_DOMAIN);
        }
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::asin: // arcsine
      try
      {
        if (rat_lt(*px, maxTrig, precision))
        {
          asinanglerat(px, angleType == angle_type::deg ? AngleType::Degrees : AngleType::Radians, radix, precision);
        }
        else
        {
          throw(CALC_E_DOMAIN);
        }
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::sinh: // hyperbolic sine
      try
      {
        if (rat_lt(*px, maxTrig, precision))
        {
          sinhrat(px, radix, precision);
        }
        else
        {
          throw(CALC_E_DOMAIN);
        }
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::cos: // cosine
      try
      {
        if (rat_lt(*px, maxTrig, precision))
        {
          cosanglerat(px, angleType == angle_type::deg ? AngleType::Degrees : AngleType::Radians, radix, precision);
        }
        else
        {
          throw(CALC_E_DOMAIN);
        }
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::acos: // arcosine
      try
      {
        if (rat_lt(*px, maxTrig, precision))
        {
          acosanglerat(px, angleType == angle_type::deg ? AngleType::Degrees : AngleType::Radians, radix, precision);
        }
        else
        {
          throw(CALC_E_DOMAIN);
        }
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::cosh: // hyperbolic cosine
      try
      {
        if (rat_lt(*px, maxTrig, precision))
        {
          coshrat(px, radix, precision);
        }
        else
        {
          throw(CALC_E_DOMAIN);
        }
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::tan: // tangent
      try
      {
        if (rat_lt(*px, maxTrig, precision))
        {
          tananglerat(px, angleType == angle_type::deg ? AngleType::Degrees : AngleType::Radians, radix, precision);
        }
        else
        {
          throw(CALC_E_DOMAIN);
        }
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::atan: // arctangent
      try
      {
        if (rat_lt(*px, maxTrig, precision))
        {
          atananglerat(px, angleType == angle_type::deg ? AngleType::Degrees : AngleType::Radians, radix, precision);
        }
        else
        {
          throw(CALC_E_DOMAIN);
        }
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::tanh: // hyperbolic tangent
      try
      {
        if (rat_lt(*px, maxTrig, precision))
        {
          tanhrat(px, radix, precision);
        }
        else
        {
          throw(CALC_E_DOMAIN);
        }
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::permutations: // permutations
      try
      {
        // check for positive integers and y > x
        DUPRAT(p, *px);
        fracrat(&p, radix, precision);
        DUPRAT(q, py);
        fracrat(&q, radix, precision);
        if (!zerrat(p) || !zerrat(q))
        {
          throw(CALC_E_DOMAIN);
        }
        if ((SIGN(*px) == (-1)) || (SIGN(py) == (-1)) || (rat_lt(py, *px, precision)))
        {
          throw(CALC_E_DOMAIN);
        }

        // calculate permutations,
        int32_t r = rattoi32(*px, radix, precision);
        // we have to put a limit to the loop, calculation is slow on a MC
        if (r > 1000 || r < 0)
        {
          throw(CALC_E_DOMAIN);
        }
        DUPRAT(*px, rat_one);
        for (int32_t i = 0; i < r; i++)
        {
          DUPRAT(q, py);
          destroyrat(p);
          p = i32torat(i);
          subrat(&q, p, precision);
          mulrat(px, q, precision);
        }
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    case operation::combinations: // combinations
      try
      {
        // check for positive integers and y > x
        DUPRAT(p, *px);
        fracrat(&p, radix, precision);
        DUPRAT(q, py);
        fracrat(&q, radix, precision);
        if (!zerrat(p) || !zerrat(q))
        {
          throw(CALC_E_DOMAIN);
        }
        if ((SIGN(*px) == (-1)) || (SIGN(py) == (-1)) || (rat_lt(py, *px, precision)))
        {
          throw(CALC_E_DOMAIN);
        }

        // optimize loop
        int32_t r1 = rattoi32(*px, radix, precision);
        DUPRAT(p, py);
        subrat(&p, *px, precision);
        int32_t r2 = rattoi32(p, radix, precision);
        int32_t r = std::min(r1, r2);

        // we have to put a limit to the loop, calculation is slow on a MC
        if (r > 5000 || r < 0)
        {
          throw(CALC_E_DOMAIN);
        }

        // calculate
        DUPRAT(*px, rat_one);
        for (int32_t i = 0; i < r; i++)
        {
          DUPRAT(q, py);
          destroyrat(p);
          p = i32torat(i);
          subrat(&q, p, precision);
          mulrat(px, q, precision);
          destroyrat(p);
          p = i32torat(i + 1);
          divrat(px, p, precision);
        }
      }
      catch (uint32_t error)
      {
        result = CalcError::toOperationReturnCode(error);
      }
      break;

    default: // avoid warning
      break;
    }
    destroyrat(p);
    destroyrat(q);
    roundCalculation(px, radix, precision);
    return (result);
  }

  // get some special values
  static void getSpecialValue(PRAT *px, operation op, uint32_t radix, int32_t precision)
  {
    switch (op)
    {
    case operation::pi:
      DUPRAT(*px, pi);
      break;

    case operation::e:
      DUPRAT(*px, rat_exp);
      break;

    case operation::rnd:
      getRandomPrat(px, radix, precision);
      break;

    default:
      break;
    }
  }

  // round the calculation to avoid strange result for some special cases
  // e.g. log(tan(45)) or sqrt(2.25) - 1.5
  static void roundCalculation(PRAT *px, uint32_t radix, int32_t precision)
  {
    PNUMBER n = RatToNumber(*px, radix, precision + 2);
    if (!zernum(n) && (n->cdigit > precision + EXTRA_PRECISION) && (abs(n->exp) > (precision + EXTRA_PRECISION)) && (abs(n->cdigit + n->exp) < precision) && (n->exp < 0))
    {
      roundnum(&n, radix, precision + EXTRA_PRECISION, 0);
      cutdigits(&n, precision + EXTRA_PRECISION);
      PRAT p = numtorat(n, radix);
      DUPRAT(*px, p);
      destroyrat(p);
    }
    destroynum(n);
  }

  // generate a random number
  static void getRandomPrat(PRAT *px, uint32_t radix, int32_t precision)
  {
    String s("0.");
    for (int i = 0; i < precision; i++)
    {
      long r = random(0, 10);
      s += String(r);
    }
    std::string_view sm(s.c_str());
    std::string_view se("0");
    destroyrat(*px);
    *px = StringToRat(false, sm, false, se, radix, precision);
  }
};