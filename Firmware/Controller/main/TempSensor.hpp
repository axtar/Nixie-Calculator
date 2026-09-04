// TempSensor.hpp

// thin C++ wrapper around the espressif/onewire_bus and espressif/ds18b20
// drivers, assuming a single DS18B20 sensor on the bus

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

#include <onewire_bus.h>
#include <onewire_cmd.h>
#include <ds18b20.h>

constexpr uint8_t DS18B20_CMD_CONVERT_TEMP = 0x44;

class TempSensor
{
public:
  explicit TempSensor(int gpioNum) : _gpioNum(gpioNum)
  {
  }

  ~TempSensor()
  {
    if (_device)
    {
      ds18b20_del_device(_device);
    }
    if (_bus)
    {
      onewire_bus_del(_bus);
    }
  }

  TempSensor(const TempSensor &) = delete;
  TempSensor &operator=(const TempSensor &) = delete;

  // create the 1-Wire bus and the DS18B20 device on it, returns true on success
  bool begin()
  {
    onewire_bus_config_t busConfig = {};
    busConfig.bus_gpio_num = _gpioNum;
    // the board provides an external pull-up resistor on the data line

    onewire_bus_rmt_config_t rmtConfig = {};
    rmtConfig.max_rx_bytes = 10; // 1 byte ROM command + 8 byte ROM number + 1 byte device command

    if (onewire_new_bus_rmt(&busConfig, &rmtConfig, &_bus) != ESP_OK)
    {
      return (false);
    }

    ds18b20_config_t sensorConfig = {};
    if (ds18b20_new_device_from_bus(_bus, &sensorConfig, &_device) != ESP_OK)
    {
      return (false);
    }

    ds18b20_set_resolution(_device, DS18B20_RESOLUTION_12B);
    return (true);
  }

  // start a temperature conversion without blocking the caller
  void requestTemperature()
  {
    if (_bus)
    {
      onewire_bus_reset(_bus);
      uint8_t txBuffer[2] = {ONEWIRE_CMD_SKIP_ROM, DS18B20_CMD_CONVERT_TEMP};
      onewire_bus_write_bytes(_bus, txBuffer, sizeof(txBuffer));
    }
  }

  // read back the result of the last requested conversion, returns true on success
  bool getTempC(float &tempC)
  {
    if (!_device)
    {
      return (false);
    }
    return (ds18b20_get_temperature(_device, &tempC) == ESP_OK);
  }

private:
  int _gpioNum;
  onewire_bus_handle_t _bus = nullptr;
  ds18b20_device_handle_t _device = nullptr;
};
