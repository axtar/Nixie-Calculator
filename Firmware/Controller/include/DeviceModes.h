// DeviceModes.h

// Device modes

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>

// device modes
enum class device_mode : uint8_t
{
  calculator,
  clock,
  menu,
  antipoisoning
};