// CalcTests.hpp

// self-test suite for ratpak/CalcMath, exercised through the /tests web page.
// build with CALC_TESTS enabled in Config.h; excluded from release builds by default.

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <vector>
#include <functional>
#include <ratpak.h>
#include <CalcDefs.h>
#include <CalcError.hpp>
#include <CalcMath.hpp>
#include <CalcIO.hpp>
#include <SettingsCache.hpp>

class CalcTests
{
public:
  // run the correctness + regression suite and return the results as a JSON array
  static String runCorrectnessJSON(uint32_t radix, int32_t precision)
  {
    std::vector<TestResult> results;
    for (const auto &test : correctnessTests())
    {
      execute(results, test, radix, precision);
    }
    return toJSON(results);
  }

  // run the known-slow cases (large root, factorial near the rat_max_fact limit) and
  // return the results as a JSON array; kept separate from the correctness suite since
  // it can take much longer and is only meant to be triggered explicitly
  static String runPerformanceJSON(uint32_t radix, int32_t precision)
  {
    std::vector<TestResult> results;
    for (const auto &test : performanceTests())
    {
      execute(results, test, radix, precision);
    }
    return toJSON(results);
  }

private:
  struct Outcome
  {
    bool pass;
    String detail;
  };

  struct TestResult
  {
    const char *name;
    const char *category;
    bool pass;
    String detail;
    unsigned long durationUs;
  };

  using TestFn = std::function<Outcome(uint32_t radix, int32_t precision)>;

  struct TestDef
  {
    const char *name;
    const char *category;
    TestFn fn;
  };

  // RAII wrapper around a PRAT so test bodies don't need explicit destroyrat()
  // bookkeeping; addr() hands out the PRAT* that ratpak's in/out functions expect,
  // and since those only ever replace *pa in place (never the outer PRAT* itself)
  // it stays valid to use after the call.
  class Rat
  {
  public:
    Rat() : _p(nullptr) {}
    explicit Rat(PRAT p) : _p(p) {}
    Rat(const Rat &) = delete;
    Rat &operator=(const Rat &) = delete;
    Rat(Rat &&other) noexcept : _p(other._p) { other._p = nullptr; }
    Rat &operator=(Rat &&other) noexcept
    {
      if (this != &other)
      {
        if (_p)
        {
          destroyrat(_p);
        }
        _p = other._p;
        other._p = nullptr;
      }
      return *this;
    }
    ~Rat()
    {
      if (_p)
      {
        destroyrat(_p);
      }
    }
    PRAT get() const { return _p; }
    PRAT *addr() { return &_p; }

    static Rat fromInt(int32_t v) { return Rat(i32torat(v)); }
    static Rat dup(PRAT src)
    {
      PRAT d = nullptr;
      DUPRAT(d, src);
      return Rat(d);
    }

  private:
    PRAT _p;
  };

  // parses a decimal literal such as "3248.7" or "-4", with an optional separate
  // exponent (e.g. mantissa "9", exponent "9999" -> 9e+9999)
  static Rat fromDecimal(const char *mantissa, uint32_t radix, int32_t precision, const char *exponent = nullptr)
  {
    bool mantissaNegative = (mantissa[0] == '-');
    std::string_view m(mantissaNegative ? mantissa + 1 : mantissa);
    std::string_view e(exponent ? exponent : "");
    return Rat(StringToRat(mantissaNegative, m, false, e, radix, precision));
  }

  static String ratStr(PRAT a, uint32_t radix, int32_t precision)
  {
    Rat t = Rat::dup(a);
    PRAT p = t.get();
    return String(RatToScientificString(p, radix, precision).c_str());
  }

  // ratpak's Taylor-series results (trig/log/exp) carry a few noisy tail digits at
  // full precision and are never bit-exact even for "nice" angles, so compare after
  // formatting both sides to a reduced number of significant digits instead of
  // using rat_equ()
  static bool approxEqual(PRAT a, PRAT b, uint32_t radix, int32_t precision)
  {
    int32_t comparePrecision = precision - 8;
    if (comparePrecision < 6)
    {
      comparePrecision = 6;
    }
    Rat ta = Rat::dup(a);
    Rat tb = Rat::dup(b);
    PRAT pa = ta.get();
    PRAT pb = tb.get();
    String sa(RatToString(pa, NumberFormat::Scientific, radix, comparePrecision).c_str());
    String sb(RatToString(pb, NumberFormat::Scientific, radix, comparePrecision).c_str());
    return sa == sb;
  }

  static Outcome expectEqual(PRAT actual, PRAT expected, uint32_t radix, int32_t precision, bool exact)
  {
    bool ok = exact ? rat_equ(actual, expected, precision) : approxEqual(actual, expected, radix, precision);
    if (ok)
    {
      return {true, String()};
    }
    return {false, "expected " + ratStr(expected, radix, precision) + " but got " + ratStr(actual, radix, precision)};
  }

  // runs body() and checks that it throws exactly expectedCode
  static Outcome expectThrow(const std::function<void()> &body, uint32_t expectedCode, const char *description)
  {
    try
    {
      body();
    }
    catch (uint32_t error)
    {
      if (error == expectedCode)
      {
        return {true, String()};
      }
      return {false, String("expected ") + description + " but got a different error code (0x" + String(error, HEX) + ")"};
    }
    return {false, String("expected ") + description + " but no error was thrown"};
  }

  // runs a CalcMath::calculate() call and checks the returned operation_return_code
  static Outcome expectCalcError(operation_return_code actual, operation_return_code expected)
  {
    if (actual == expected)
    {
      return {true, String()};
    }
    String actualText = (actual == operation_return_code::success) ? String("success") : CalcError::getErrorText(actual);
    return {false, "expected error \"" + CalcError::getErrorText(expected) + "\" but got \"" + actualText + "\""};
  }

  static void execute(std::vector<TestResult> &results, const TestDef &test, uint32_t radix, int32_t precision)
  {
    unsigned long start = micros();
    Outcome outcome{false, String()};
    try
    {
      outcome = test.fn(radix, precision);
    }
    catch (uint32_t error)
    {
      outcome.pass = false;
      outcome.detail = "unexpected error: " + CalcError::getErrorText(CalcError::toOperationReturnCode(error));
    }
    catch (...)
    {
      outcome.pass = false;
      outcome.detail = "unexpected exception";
    }
    unsigned long durationUs = micros() - start;
    results.push_back({test.name, test.category, outcome.pass, outcome.detail, durationUs});
  }

  // builds the maxTrig bound the same way Calculator::begin() -> setMaxTrig() does (10^100)
  static Rat maxTrigBound(uint32_t radix, int32_t precision)
  {
    Rat bound = Rat::fromInt(10);
    ratpowi32(bound.addr(), 100, precision);
    return bound;
  }

  // CalcIO::setNumber() reads SettingsCache::roundingMode directly (a live global the
  // running calculator also uses), so display-formatting tests that depend on rounding
  // behavior must pin it for the duration of the test and restore it afterward rather
  // than assume, or leave mutated, the user's actual setting.
  class RoundingModeGuard
  {
  public:
    explicit RoundingModeGuard(rounding_mode::rounding_mode mode) : _previous(SettingsCache::roundingMode)
    {
      SettingsCache::roundingMode = mode;
    }
    ~RoundingModeGuard()
    {
      SettingsCache::roundingMode = _previous;
    }

  private:
    rounding_mode::rounding_mode _previous;
  };

  struct ExpectedNumber
  {
    const char *base;
    bool baseNegative;
    const char *exponent; // "" means no exponent shown
    bool exponentNegative;
  };

  static String describeNumber(const String &base, bool baseNegative, bool exponentMode, const String &exponent, bool exponentNegative)
  {
    String s = baseNegative ? "-" : "";
    s += base;
    if (exponentMode)
    {
      s += "e";
      s += exponentNegative ? "-" : "+";
      s += exponent;
    }
    return s;
  }

  // runs CalcIO::setNumber() on value and compares the resulting CALC_NUMBER fields
  // (what actually drives the display) against the expected base/exponent/signs
  static Outcome expectFormatted(PRAT value, uint32_t radix, int32_t precision, uint8_t digitCount, uint8_t maxExpLength,
                                  uint8_t fixedDecimals, bool forceScientific, const ExpectedNumber &expected)
  {
    CalcIO cio(digitCount, maxExpLength);
    uint32_t result = cio.setNumber(value, radix, precision, fixedDecimals, forceScientific);
    if (result != 0)
    {
      return {false, "setNumber returned error 0x" + String(result, HEX)};
    }
    CALC_NUMBER n = cio.getNumber();
    bool expectExponent = (expected.exponent[0] != '\0');
    bool ok = (n.base == expected.base) && (n.baseNegative == expected.baseNegative) &&
              (n.exponentMode == expectExponent) &&
              (!expectExponent || ((n.exponent == expected.exponent) && (n.exponentNegative == expected.exponentNegative)));
    if (ok)
    {
      return {true, String()};
    }
    String got = describeNumber(n.base, n.baseNegative, n.exponentMode, n.exponent, n.exponentNegative);
    String exp = describeNumber(String(expected.base), expected.baseNegative, expectExponent, String(expected.exponent), expected.exponentNegative);
    return {false, "expected " + exp + " but got " + got};
  }

  static const std::vector<TestDef> &correctnessTests()
  {
    static const std::vector<TestDef> tests = {
        // ------------------------------------------------------------ arithmetic
        {"2 + 3 = 5", "arithmetic", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(2);
           Rat expected = Rat::fromInt(5);
           addrat(a.addr(), Rat::fromInt(3).get(), precision);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"5 - 8 = -3", "arithmetic", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(5);
           Rat expected = Rat::fromInt(-3);
           subrat(a.addr(), Rat::fromInt(8).get(), precision);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"-4 * 6 = -24", "arithmetic", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(-4);
           Rat expected = Rat::fromInt(-24);
           mulrat(a.addr(), Rat::fromInt(6).get(), precision);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"7 / 2 = 3.5", "arithmetic", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(7);
           Rat expected = fromDecimal("3.5", radix, precision);
           divrat(a.addr(), Rat::fromInt(2).get(), precision);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"1 / 0 -> divide by zero", "arithmetic", [](uint32_t radix, int32_t precision) -> Outcome
         {
           return expectThrow([&]()
                               {
                                 Rat a = Rat::fromInt(1);
                                 divrat(a.addr(), Rat::fromInt(0).get(), precision);
                               },
                               CALC_E_DIVIDEBYZERO, "divide by zero");
         }},
        {"1/x of 4 = 0.25", "arithmetic", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(1);
           Rat expected = fromDecimal("0.25", radix, precision);
           divrat(a.addr(), Rat::fromInt(4).get(), precision);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"7 mod 3 = 1", "arithmetic", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(7);
           Rat expected = Rat::fromInt(1);
           modrat(a.addr(), Rat::fromInt(3).get());
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"-7 mod 3 = 2 (sign follows divisor)", "arithmetic", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(-7);
           Rat expected = Rat::fromInt(2);
           modrat(a.addr(), Rat::fromInt(3).get());
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"7 mod -3 = -2 (sign follows divisor)", "arithmetic", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(7);
           Rat expected = Rat::fromInt(-2);
           modrat(a.addr(), Rat::fromInt(-3).get());
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"integer(7.9) = 7", "arithmetic", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = fromDecimal("7.9", radix, precision);
           Rat expected = Rat::fromInt(7);
           intrat(a.addr(), radix, precision);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"integer(-7.9) = -7", "arithmetic", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = fromDecimal("-7.9", radix, precision);
           Rat expected = Rat::fromInt(-7);
           intrat(a.addr(), radix, precision);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},

        // ------------------------------------------------------------------ power/root
        {"2^10 = 1024", "power", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(2);
           Rat expected = Rat::fromInt(1024);
           powrat(a.addr(), Rat::fromInt(10).get(), radix, precision);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"(-2)^3 = -8", "power", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(-2);
           Rat expected = Rat::fromInt(-8);
           powrat(a.addr(), Rat::fromInt(3).get(), radix, precision);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"2^-3 = 0.125", "power", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(2);
           Rat expected = fromDecimal("0.125", radix, precision);
           powrat(a.addr(), Rat::fromInt(-3).get(), radix, precision);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"0^0 = 1", "power", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(0);
           Rat expected = Rat::fromInt(1);
           powrat(a.addr(), Rat::fromInt(0).get(), radix, precision);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"5^0 = 1", "power", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(5);
           Rat expected = Rat::fromInt(1);
           powrat(a.addr(), Rat::fromInt(0).get(), radix, precision);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"sqrt(4) = 2", "power", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(4);
           Rat expected = Rat::fromInt(2);
           rootrat(a.addr(), Rat::fromInt(2).get(), radix, precision);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"cbrt(-8) = -2", "power", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(-8);
           Rat expected = Rat::fromInt(-2);
           rootrat(a.addr(), Rat::fromInt(3).get(), radix, precision);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"sqrt(-4) -> domain error", "power", [](uint32_t radix, int32_t precision) -> Outcome
         {
           return expectThrow([&]()
                               {
                                 Rat a = Rat::fromInt(-4);
                                 rootrat(a.addr(), Rat::fromInt(2).get(), radix, precision);
                               },
                               CALC_E_DOMAIN, "domain error");
         }},
        {"regression: sqrt(2.25) - 1.5 = 0", "regression", [](uint32_t radix, int32_t precision) -> Outcome
         {
           // sqrt(2.25) is only an approximation of 1.5 internally; subrat()'s
           // _snaprat() call is what's expected to clean up the tiny residual
           // noise down to an exact zero (see its doc comment in rat.cpp).
           Rat a = fromDecimal("2.25", radix, precision);
           rootrat(a.addr(), Rat::fromInt(2).get(), radix, precision);
           subrat(a.addr(), fromDecimal("1.5", radix, precision).get(), precision);
           Rat expected = Rat::fromInt(0);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"regression: ln(tan(45 deg)) = 0", "regression", [](uint32_t radix, int32_t precision) -> Outcome
         {
           // tan(45deg) is only an approximation of 1 internally; lograt()'s
           // _snaprat() call is what's expected to snap the result to an exact
           // zero instead of a tiny residual near-zero value.
           Rat a = Rat::fromInt(45);
           tananglerat(a.addr(), AngleType::Degrees, radix, precision);
           lograt(a.addr(), precision);
           Rat expected = Rat::fromInt(0);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"regression: (-1)^INT_MIN = 1, no hang", "regression", [](uint32_t radix, int32_t precision) -> Outcome
         {
           // negating INT_MIN as int32_t overflows; ratpowi32() used to recurse on that
           // still-negative value forever (see conv.cpp). Reaching this point at all,
           // quickly, is most of what this test is checking.
           // Uses the pre-built rat_min_i32 constant (== -2^31) rather than
           // i32torat(INT32_MIN), which would itself negate INT32_MIN as int32_t.
           Rat a = Rat::fromInt(-1);
           Rat expected = Rat::fromInt(1);
           Rat exponent = Rat::dup(rat_min_i32);
           powrat(a.addr(), exponent.get(), radix, precision);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},

        // --------------------------------------------------------------- log/exp
        {"ln(1) = 0", "log-exp", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(1);
           Rat expected = Rat::fromInt(0);
           lograt(a.addr(), precision);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"exp(0) = 1", "log-exp", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(0);
           Rat expected = Rat::fromInt(1);
           exprat(a.addr(), radix, precision);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"exp(1) = e", "log-exp", [](uint32_t radix, int32_t precision) -> Outcome
         {
           // not an exact comparison: rat_exp is precomputed with one extra guard
           // digit (precision + g_ratio, see ChangeConstants()), so exprat(1) at
           // plain `precision` can differ from it in that last digit.
           Rat a = Rat::fromInt(1);
           exprat(a.addr(), radix, precision);
           return expectEqual(a.get(), rat_exp, radix, precision, false);
         }},
        {"ln(e) = 1", "log-exp", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::dup(rat_exp);
           Rat expected = Rat::fromInt(1);
           lograt(a.addr(), precision);
           return expectEqual(a.get(), expected.get(), radix, precision, false);
         }},
        {"log10(100) = 2", "log-exp", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(100);
           Rat expected = Rat::fromInt(2);
           log10rat(a.addr(), precision);
           return expectEqual(a.get(), expected.get(), radix, precision, false);
         }},
        {"ln(0) -> domain error", "log-exp", [](uint32_t radix, int32_t precision) -> Outcome
         {
           return expectThrow([&]()
                               {
                                 Rat a = Rat::fromInt(0);
                                 lograt(a.addr(), precision);
                               },
                               CALC_E_DOMAIN, "domain error");
         }},
        {"ln(-5) -> domain error", "log-exp", [](uint32_t radix, int32_t precision) -> Outcome
         {
           return expectThrow([&]()
                               {
                                 Rat a = Rat::fromInt(-5);
                                 lograt(a.addr(), precision);
                               },
                               CALC_E_DOMAIN, "domain error");
         }},
        {"exp(200000) -> domain error", "log-exp", [](uint32_t radix, int32_t precision) -> Outcome
         {
           return expectThrow([&]()
                               {
                                 Rat a = Rat::fromInt(200000);
                                 exprat(a.addr(), radix, precision);
                               },
                               CALC_E_DOMAIN, "domain error");
         }},

        // ------------------------------------------------------------------- trig
        {"sin(0 deg) = 0", "trig", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(0);
           Rat expected = Rat::fromInt(0);
           sinanglerat(a.addr(), AngleType::Degrees, radix, precision);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"cos(0 deg) = 1", "trig", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(0);
           Rat expected = Rat::fromInt(1);
           cosanglerat(a.addr(), AngleType::Degrees, radix, precision);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"sin(90 deg) = 1", "trig", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(90);
           Rat expected = Rat::fromInt(1);
           sinanglerat(a.addr(), AngleType::Degrees, radix, precision);
           return expectEqual(a.get(), expected.get(), radix, precision, false);
         }},
        {"cos(180 deg) = -1", "trig", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(180);
           Rat expected = Rat::fromInt(-1);
           cosanglerat(a.addr(), AngleType::Degrees, radix, precision);
           return expectEqual(a.get(), expected.get(), radix, precision, false);
         }},
        {"tan(45 deg) = 1", "trig", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(45);
           Rat expected = Rat::fromInt(1);
           tananglerat(a.addr(), AngleType::Degrees, radix, precision);
           return expectEqual(a.get(), expected.get(), radix, precision, false);
         }},
        {"asin(1) = 90 deg", "trig", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(1);
           Rat expected = Rat::fromInt(90);
           asinanglerat(a.addr(), AngleType::Degrees, radix, precision);
           return expectEqual(a.get(), expected.get(), radix, precision, false);
         }},
        {"acos(0) = 90 deg", "trig", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(0);
           Rat expected = Rat::fromInt(90);
           acosanglerat(a.addr(), AngleType::Degrees, radix, precision);
           return expectEqual(a.get(), expected.get(), radix, precision, false);
         }},
        {"atan(1) = 45 deg", "trig", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(1);
           Rat expected = Rat::fromInt(45);
           atananglerat(a.addr(), AngleType::Degrees, radix, precision);
           return expectEqual(a.get(), expected.get(), radix, precision, false);
         }},
        {"asin(1.5) -> domain error", "trig", [](uint32_t radix, int32_t precision) -> Outcome
         {
           return expectThrow([&]()
                               {
                                 Rat a = fromDecimal("1.5", radix, precision);
                                 asinanglerat(a.addr(), AngleType::Degrees, radix, precision);
                               },
                               CALC_E_DOMAIN, "domain error");
         }},
        {"tan(90 deg) -> domain error", "trig", [](uint32_t radix, int32_t precision) -> Outcome
         {
           return expectThrow([&]()
                               {
                                 Rat a = Rat::fromInt(90);
                                 tananglerat(a.addr(), AngleType::Degrees, radix, precision);
                               },
                               CALC_E_DOMAIN, "domain error");
         }},
        {"sinh(0) = 0", "trig", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(0);
           Rat expected = Rat::fromInt(0);
           sinhrat(a.addr(), radix, precision);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"cosh(0) = 1", "trig", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(0);
           Rat expected = Rat::fromInt(1);
           coshrat(a.addr(), radix, precision);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"tanh(0) = 0", "trig", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(0);
           Rat expected = Rat::fromInt(0);
           tanhrat(a.addr(), radix, precision);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"regression: sin(-9e+9999 deg) -> domain error, no hang", "regression", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat maxTrig = maxTrigBound(radix, precision);
           Rat a = fromDecimal("-9", radix, precision, "9999");
           operation_return_code rc = CalcMath::calculate(a.addr(), rat_zero, operation::sin, radix, precision, maxTrig.get(), angle_type::deg);
           return expectCalcError(rc, operation_return_code::domain);
         }},
        {"regression: cos(9e+9999 deg) -> domain error, no hang", "regression", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat maxTrig = maxTrigBound(radix, precision);
           Rat a = fromDecimal("9", radix, precision, "9999");
           operation_return_code rc = CalcMath::calculate(a.addr(), rat_zero, operation::cos, radix, precision, maxTrig.get(), angle_type::deg);
           return expectCalcError(rc, operation_return_code::domain);
         }},

        // ------------------------------------------------------- factorial/combinatorics
        {"0! = 1", "factorial", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(0);
           Rat expected = Rat::fromInt(1);
           factrat(a.addr(), radix, precision);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"5! = 120", "factorial", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = Rat::fromInt(5);
           Rat expected = Rat::fromInt(120);
           factrat(a.addr(), radix, precision);
           return expectEqual(a.get(), expected.get(), radix, precision, true);
         }},
        {"(-3)! -> domain error", "factorial", [](uint32_t radix, int32_t precision) -> Outcome
         {
           return expectThrow([&]()
                               {
                                 Rat a = Rat::fromInt(-3);
                                 factrat(a.addr(), radix, precision);
                               },
                               CALC_E_DOMAIN, "domain error");
         }},
        {"3250! -> overflow error", "factorial", [](uint32_t radix, int32_t precision) -> Outcome
         {
           return expectThrow([&]()
                               {
                                 Rat a = Rat::fromInt(3250);
                                 factrat(a.addr(), radix, precision);
                               },
                               CALC_E_OVERFLOW, "overflow error");
         }},
        {"-2000! -> overflow error", "factorial", [](uint32_t radix, int32_t precision) -> Outcome
         {
           return expectThrow([&]()
                               {
                                 Rat a = Rat::fromInt(-2000);
                                 factrat(a.addr(), radix, precision);
                               },
                               CALC_E_OVERFLOW, "overflow error");
         }},
        {"5 P 2 = 20", "factorial", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat r = Rat::fromInt(2);
           Rat expected = Rat::fromInt(20);
           operation_return_code rc = CalcMath::calculate(r.addr(), Rat::fromInt(5).get(), operation::permutations, radix, precision);
           if (rc != operation_return_code::success)
           {
             return expectCalcError(rc, operation_return_code::success);
           }
           return expectEqual(r.get(), expected.get(), radix, precision, true);
         }},
        {"5 C 2 = 10", "factorial", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat r = Rat::fromInt(2);
           Rat expected = Rat::fromInt(10);
           operation_return_code rc = CalcMath::calculate(r.addr(), Rat::fromInt(5).get(), operation::combinations, radix, precision);
           if (rc != operation_return_code::success)
           {
             return expectCalcError(rc, operation_return_code::success);
           }
           return expectEqual(r.get(), expected.get(), radix, precision, true);
         }},
        {"permutations r>n -> domain error", "factorial", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat r = Rat::fromInt(5);
           operation_return_code rc = CalcMath::calculate(r.addr(), Rat::fromInt(2).get(), operation::permutations, radix, precision);
           return expectCalcError(rc, operation_return_code::domain);
         }},

        // --------------------------------------------- display formatting (CalcIO)
        // digitCount=14 matches every Nixie/LED display HAL; maxExpLength=4 is the
        // default maxexpdigits setting.
        {"display: 5 -> \"5\"", "display", [](uint32_t radix, int32_t precision) -> Outcome
         {
           RoundingModeGuard guard(rounding_mode::five_four);
           Rat v = Rat::fromInt(5);
           return expectFormatted(v.get(), radix, precision, 14, 4, 0, false, {"5", false, "", false});
         }},
        {"display: 3.14 -> \"3.14\"", "display", [](uint32_t radix, int32_t precision) -> Outcome
         {
           RoundingModeGuard guard(rounding_mode::five_four);
           Rat v = fromDecimal("3.14", radix, precision);
           return expectFormatted(v.get(), radix, precision, 14, 4, 0, false, {"3.14", false, "", false});
         }},
        {"display: 0 -> \"0\"", "display", [](uint32_t radix, int32_t precision) -> Outcome
         {
           RoundingModeGuard guard(rounding_mode::five_four);
           Rat v = Rat::fromInt(0);
           return expectFormatted(v.get(), radix, precision, 14, 4, 0, false, {"0", false, "", false});
         }},
        {"display: -5 -> \"-5\"", "display", [](uint32_t radix, int32_t precision) -> Outcome
         {
           RoundingModeGuard guard(rounding_mode::five_four);
           Rat v = Rat::fromInt(-5);
           return expectFormatted(v.get(), radix, precision, 14, 4, 0, false, {"5", true, "", false});
         }},
        {"display: -3.14 -> \"-3.14\"", "display", [](uint32_t radix, int32_t precision) -> Outcome
         {
           RoundingModeGuard guard(rounding_mode::five_four);
           Rat v = fromDecimal("-3.14", radix, precision);
           return expectFormatted(v.get(), radix, precision, 14, 4, 0, false, {"3.14", true, "", false});
         }},
        {"display: 5 with fixedDecimals=2 -> \"5.00\"", "display", [](uint32_t radix, int32_t precision) -> Outcome
         {
           RoundingModeGuard guard(rounding_mode::five_four);
           Rat v = Rat::fromInt(5);
           return expectFormatted(v.get(), radix, precision, 14, 4, 2, false, {"5.00", false, "", false});
         }},
        {"display: 3.14159 with fixedDecimals=2 -> \"3.14\"", "display", [](uint32_t radix, int32_t precision) -> Outcome
         {
           RoundingModeGuard guard(rounding_mode::five_four);
           Rat v = fromDecimal("3.14159", radix, precision);
           return expectFormatted(v.get(), radix, precision, 14, 4, 2, false, {"3.14", false, "", false});
         }},
        {"display: 1.25 fixedDecimals=1, five_four -> \"1.3\"", "display", [](uint32_t radix, int32_t precision) -> Outcome
         {
           RoundingModeGuard guard(rounding_mode::five_four);
           Rat v = fromDecimal("1.25", radix, precision);
           return expectFormatted(v.get(), radix, precision, 14, 4, 1, false, {"1.3", false, "", false});
         }},
        {"display: 1.25 fixedDecimals=1, cut -> \"1.2\"", "display", [](uint32_t radix, int32_t precision) -> Outcome
         {
           RoundingModeGuard guard(rounding_mode::cut);
           Rat v = fromDecimal("1.25", radix, precision);
           return expectFormatted(v.get(), radix, precision, 14, 4, 1, false, {"1.2", false, "", false});
         }},
        {"display: 1.995 fixedDecimals=2, five_four carries -> \"2.00\"", "display", [](uint32_t radix, int32_t precision) -> Outcome
         {
           RoundingModeGuard guard(rounding_mode::five_four);
           Rat v = fromDecimal("1.995", radix, precision);
           return expectFormatted(v.get(), radix, precision, 14, 4, 2, false, {"2.00", false, "", false});
         }},
        {"display: 99999999999999.6 rounds and overflows to scientific -> \"1e+14\"", "display", [](uint32_t radix, int32_t precision) -> Outcome
         {
           // carry propagates through all 14 nines of the integer part, growing it to
           // 15 digits, which no longer fits digitCount=14 and must switch to scientific
           RoundingModeGuard guard(rounding_mode::five_four);
           Rat v = fromDecimal("99999999999999.6", radix, precision);
           return expectFormatted(v.get(), radix, precision, 14, 4, 0, false, {"1", false, "14", false});
         }},
        {"display: exponent overflow (1e+99999) -> error", "display", [](uint32_t radix, int32_t precision) -> Outcome
         {
           // maxExpLength=4 can't hold a 5-digit exponent. Feed the string straight to
           // numberFromString() (the method with the actual overflow guard) instead of
           // building a real 10^99999 PRAT via StringToRat/fromDecimal: that would
           // materialize an unbounded, un-trimmed ~10000-digit number just to construct
           // a test fixture, and a real keypad input can never produce this case anyway
           // (typed exponents are already capped to maxExpLength digits as you type).
           CalcIO cio(14, 4);
           CALC_NUMBER number;
           uint32_t result = cio.numberFromString("1e99999", &number);
           if (result == 0)
           {
             return {false, "expected an overflow error but got base=" + number.base};
           }
           return {true, String()};
         }},
        {"display: forceScientific keeps 5 as \"5e+0\"", "display", [](uint32_t radix, int32_t precision) -> Outcome
         {
           RoundingModeGuard guard(rounding_mode::five_four);
           Rat v = Rat::fromInt(5);
           return expectFormatted(v.get(), radix, precision, 14, 4, 0, true, {"5", false, "0", false});
         }},
    };
    return tests;
  }

  static const std::vector<TestDef> &performanceTests()
  {
    static const std::vector<TestDef> tests = {
        {"sqrt(9e+9999)", "performance", [](uint32_t radix, int32_t precision) -> Outcome
         {
           Rat a = fromDecimal("9", radix, precision, "9999");
           rootrat(a.addr(), Rat::fromInt(2).get(), radix, precision);
           return {true, "result " + ratStr(a.get(), radix, precision)};
         }},
        {"3248.7!", "performance", [](uint32_t radix, int32_t precision) -> Outcome
         {
           // rat_max_fact is 3249, so this sits right below the cutoff: the
           // slowest factorial value that's still accepted.
           Rat a = fromDecimal("3248.7", radix, precision);
           factrat(a.addr(), radix, precision);
           return {true, "result " + ratStr(a.get(), radix, precision)};
         }},
    };
    return tests;
  }

  static String jsonEscape(const String &s)
  {
    String out;
    out.reserve(s.length() + 4);
    for (size_t i = 0; i < s.length(); i++)
    {
      char c = s[i];
      if ((c == '"') || (c == '\\'))
      {
        out += '\\';
      }
      out += c;
    }
    return out;
  }

  static String toJSON(const std::vector<TestResult> &results)
  {
    String json;
    json.reserve(results.size() * 96 + 16);
    json += '[';
    bool first = true;
    for (const auto &r : results)
    {
      if (!first)
      {
        json += ',';
      }
      first = false;
      json += "{\"name\":\"";
      json += jsonEscape(r.name);
      json += "\",\"category\":\"";
      json += jsonEscape(r.category);
      json += "\",\"pass\":";
      json += r.pass ? "true" : "false";
      json += ",\"detail\":\"";
      json += jsonEscape(r.detail);
      json += "\",\"durationUs\":";
      json += r.durationUs;
      json += '}';
    }
    json += ']';
    return json;
  }
};
