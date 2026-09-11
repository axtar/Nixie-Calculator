// SerialPrint.h

// definitions for serial debug printing

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

// Set to false to disable custom serial print
// Set to true to enable custom serial print
#define ENABLE_SERIAL_PRINT false

#if ENABLE_SERIAL_PRINT
#define __serial_begin(...) Serial.begin(__VA_ARGS__)
#define __serial_print(...) Serial.print(__VA_ARGS__)
#define __serial_println(...) Serial.println(__VA_ARGS__)
#define __serial_printf(...) Serial.printf(__VA_ARGS__)
#else
#define __serial_begin(...)
#define __serial_print(...)
#define __serial_println(...)
#define __serial_printf(...)
#endif