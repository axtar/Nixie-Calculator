// LedStrip.hpp

// thin C++ wrapper around the espressif/led_strip RMT driver

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

#include <led_strip.h>
#include <esp_check.h>

// use more symbols
constexpr size_t LED_STRIP_RMT_MEM_BLOCK_SYMBOLS = 144;

class LedStrip
{
public:
  LedStrip(int gpioNum, uint32_t ledCount,
           led_pixel_format_t pixelFormat = LED_PIXEL_FORMAT_GRB,
           led_model_t ledModel = LED_MODEL_WS2812,
           uint32_t resolutionHz = 10 * 1000 * 1000)
  {
    led_strip_config_t stripConfig = {};
    stripConfig.strip_gpio_num = gpioNum;
    stripConfig.max_leds = ledCount;
    stripConfig.led_pixel_format = pixelFormat;
    stripConfig.led_model = ledModel;

    led_strip_rmt_config_t rmtConfig = {};
    rmtConfig.resolution_hz = resolutionHz;
    rmtConfig.mem_block_symbols = LED_STRIP_RMT_MEM_BLOCK_SYMBOLS;

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&stripConfig, &rmtConfig, &_handle));
  }

  ~LedStrip()
  {
    if (_handle)
    {
      led_strip_del(_handle);
    }
  }

  LedStrip(const LedStrip &) = delete;
  LedStrip &operator=(const LedStrip &) = delete;

  // set the color of a single pixel, does not take effect until refresh() is called
  void setPixel(uint32_t index, uint8_t red, uint8_t green, uint8_t blue)
  {
    led_strip_set_pixel(_handle, index, red, green, blue);
  }

  // turn off all pixels, does not take effect until refresh() is called
  void clear()
  {
    led_strip_clear(_handle);
  }

  // push the current pixel buffer out to the strip
  void refresh()
  {
    led_strip_refresh(_handle);
  }

private:
  led_strip_handle_t _handle = nullptr;
};
