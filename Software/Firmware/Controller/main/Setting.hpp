// Setting.hpp

// holds seting value

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once
#include <Arduino.h>
#include <SettingEnum.h>

enum class setting_type
{
  time,
  rgb,
  numeric
};

class Setting
{
public:
  Setting(setting_id::setting_id id, String name, setting_type settingType, int defaultValue, int minValue, int maxValue)
      : _id(id),
        _name(name),
        _settingType(settingType),
        _defaultValue(defaultValue),
        _minValue(minValue),
        _maxValue(maxValue)
  {
    _value = _defaultValue;
    _modified = true;
    _hidden = false;
  }

  virtual ~Setting()
  {
  }

  // return the id of the setting
  uint getId() const
  {
    return (_id);
  }

  // return the name of the setting
  String getName() const
  {
    return (_name);
  }

  // set a temporary setting value
  void setTempValue(int value)
  {
    if ((value <= _maxValue) && (value >= _minValue))
    {
      _tempValue = value;
    }
  }

  // set the setting value
  void set(int value)
  {
    if (value != _value)
    {
      if ((value <= _maxValue) && (value >= _minValue))
      {
        _value = value;
        _modified = true;
      }
    }
  }

  // return the temporary setting value
  int getTempValue() const
  {
    return (_tempValue);
  }

  // return the setting value
  int get() const
  {
    return (_value);
  }

  // return the default setting value
  int getDefault() const
  {
    return (_defaultValue);
  }

  // return the minimum value of the setting
  int getMin() const
  {
    return (_minValue);
  }

  // return the maximum value of the setting
  int getMax() const
  {
    return (_maxValue);
  }

  // reset a setting to the default value
  void reset()
  {
    _value = _defaultValue;
    _modified = true;
  }

  // reset the modified flag
  void resetModified()
  {
    _modified = false;
  }

  // return if a setting was modified
  bool modified() const
  {
    return (_modified);
  }

  // return if a setting is hidden
  bool isHidden()
  {
    return (_hidden);
  }

  // hide setting
  void hide()
  {
    _hidden = true;
  }

  // unhide setting
  void unhide()
  {
    _hidden = false;
  }


  // return the type of a setting
  setting_type getSettingType() const
  {
    return (_settingType);
  }

private:
  setting_id::setting_id _id;
  String _name;
  setting_type _settingType;
  int _defaultValue;
  int _minValue;
  int _maxValue;
  int _value;
  int _tempValue;
  bool _modified;
  bool _hidden;
};