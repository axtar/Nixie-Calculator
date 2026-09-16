// Config.h

// Firmware configuration

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

#include <DisplayTypes.h>

// -------------------------------------------------------------------------------
// Display type
// -------------------------------------------------------------------------------
// display_type::in12a -> IN-12A or IN-12B nixie display using neon decimal points
// display_type::in12b -> IN-12B nixie display using nixie decimal points
// display_type::in16  -> IN-16 nixie display
// display_type::in17  -> IN-17 nixie display
// display_type::b5870 -> B5870 nixie display
// display_type::led   -> 7-segment LED display
// -------------------------------------------------------------------------------
constexpr auto DISPLAY_TYPE = display_type::b5870;

// -------------------------------------------------------------------------------
// Web server support for register view
// -------------------------------------------------------------------------------
// false -> disable
// true -> enable
// -------------------------------------------------------------------------------
#define WEB_SUPPORT true

// -------------------------------------------------------------------------------
// RPN mode
// -------------------------------------------------------------------------------
// false -> algebraic
// true  -> reverse polish notation
// -------------------------------------------------------------------------------
#define RPN_MODE true

// -------------------------------------------------------------------------------
// Calculator access point SSID and password
// -------------------------------------------------------------------------------
// Please note that this applies to the AP provided by the calculator.
// The calculator itself does not connect to any AP.
// -------------------------------------------------------------------------------
constexpr auto AP_SSID = "NixieCALC";
constexpr auto AP_PWD = "NIXIESareGreat!";

// -------------------------------------------------------------------------------

// compile time configuration checks
static_assert(DISPLAY_TYPE == display_type::in12a ||
                  DISPLAY_TYPE == display_type::in12b ||
                  DISPLAY_TYPE == display_type::in16 ||
                  DISPLAY_TYPE == display_type::in17 ||
                  DISPLAY_TYPE == display_type::b5870 ||
                  DISPLAY_TYPE == display_type::led,
              "Display type configuration incorrect");

#if RPN_MODE != false && RPN_MODE != true
#error "CALC_TYPE configuration incorrect"
#endif

#if WEB_SUPPORT != false && WEB_SUPPORT != true
#error "WEB_SUPPORT configuration incorrect"
#endif
