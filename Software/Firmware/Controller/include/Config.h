// Config.h

// Firmware configuration

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <HardwareInfo.h>

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
constexpr auto DISPLAY_TYPE = display_type::undefined;

// -------------------------------------------------------------------------------
// WebSocket server support
// -------------------------------------------------------------------------------
// 0 -> disable
// 1 -> enable
// -------------------------------------------------------------------------------
#define WEBSOCKET_SUPPORT 1

// -------------------------------------------------------------------------------
// Calculator type
// -------------------------------------------------------------------------------
// CALC_TYPE_ALG -> algebraic
// CALC_TYPE_RPN -> reverse polish notation
// -------------------------------------------------------------------------------
#define CALC_TYPE CALC_TYPE_UNDEFINED

// -------------------------------------------------------------------------------
// Calculator access point SSID and password
// -------------------------------------------------------------------------------
// Please note that this applies to the AP provided by the calculator. 
// The calculator itself does not connect to any AP.
// -------------------------------------------------------------------------------
constexpr auto AP_SSID = "NixieCALC";
constexpr auto AP_PWD = "NIXIESareGreat!";

// compile time configuration checks
static_assert(DISPLAY_TYPE == display_type::in12a ||
                  DISPLAY_TYPE == display_type::in12b ||
                  DISPLAY_TYPE == display_type::in16 ||
                  DISPLAY_TYPE == display_type::in17 ||
                  DISPLAY_TYPE == display_type::b5870 ||
                  DISPLAY_TYPE == display_type::led,
              "Display type configuration incorrect");

#if CALC_TYPE != CALC_TYPE_RPN && CALC_TYPE != CALC_TYPE_ALG
#error "CALC_TYPE configuration incorrect"
#endif

#if WEBSOCKET_SUPPORT != 0 && WEBSOCKET_SUPPORT != 1
#error "WEBSOCKET_SUPPORT configuration incorrect"
#endif

