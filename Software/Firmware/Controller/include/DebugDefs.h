// DebugDefs.h

// definitions for serial debugging

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

// SET TO 0 TO REMOVE TRACES
// SET TO 1 TO USE TRACES
#define DEBUG 0

#if DEBUG
#define D_Begin(...) Serial.begin(__VA_ARGS__);
#define D_print(...) Serial.print(__VA_ARGS__)
#define D_write(...) Serial.write(__VA_ARGS__)
#define D_println(...) Serial.println(__VA_ARGS__)
#define D_printf(...) Serial.printf(__VA_ARGS__)
#else
#define D_Begin(...)
#define D_print(...)
#define D_write(...)
#define D_println(...)
#define D_printf(...)
#endif