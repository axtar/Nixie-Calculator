// KeyboardHandler.hpp

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Config.h>

#if CALC_TYPE == CALC_TYPE_RPN

// keys scan code                 primary operation
// --------------                 -----------------
constexpr uint8_t KEY_EXP = 1;    // exponent
constexpr uint8_t KEY_BACK = 2;   // backspace
constexpr uint8_t KEY_CLS = 3;    // clear stack
constexpr uint8_t KEY_SHIFT = 4;  // yellow operations
constexpr uint8_t KEY_F = 5;      // function key, blue operations
constexpr uint8_t KEY_POW = 6;    // power
constexpr uint8_t KEY_YROOT = 7;  // y root
constexpr uint8_t KEY_INV = 8;    // invert
constexpr uint8_t KEY_LN = 9;     // natural log
constexpr uint8_t KEY_LOGY = 10;  // log base y
constexpr uint8_t KEY_SIN = 11;   // sine
constexpr uint8_t KEY_COS = 12;   // cosine
constexpr uint8_t KEY_TAN = 13;   // tangent
constexpr uint8_t KEY_LOG = 14;   // log 10
constexpr uint8_t KEY_DEG = 15;   // deg<->rad
constexpr uint8_t KEY_CHS = 16;   // change sign +/-
constexpr uint8_t KEY_7 = 17;     // 7
constexpr uint8_t KEY_4 = 18;     // 4
constexpr uint8_t KEY_1 = 19;     // 1
constexpr uint8_t KEY_0 = 20;     // 0
constexpr uint8_t KEY_SQRT = 21;  // square root
constexpr uint8_t KEY_8 = 22;     // 8
constexpr uint8_t KEY_5 = 23;     // 5
constexpr uint8_t KEY_2 = 24;     // 2
constexpr uint8_t KEY_00 = 25;    // double 0
constexpr uint8_t KEY_PCT = 26;   // percent
constexpr uint8_t KEY_9 = 27;     // 9
constexpr uint8_t KEY_6 = 28;     // 6
constexpr uint8_t KEY_3 = 29;     // 3
constexpr uint8_t KEY_DOT = 30;   // decimal separator
constexpr uint8_t KEY_DIV = 31;   // division
constexpr uint8_t KEY_MUL = 32;   // multiplication
constexpr uint8_t KEY_MINUS = 33; // subsctraction
constexpr uint8_t KEY_PLUS = 34;  // addition
constexpr uint8_t KEY_ENTER = 35; // enter
constexpr uint8_t KEY_XY = 36;    // X<->Y
constexpr uint8_t KEY_ROLL = 37;  // roll stack
constexpr uint8_t KEY_CLR = 38;   // clear registers
constexpr uint8_t KEY_STO = 39;   // store register
constexpr uint8_t KEY_RCL = 40;   // recall register

#else

constexpr uint8_t KEY_EXP = 1;     // exponent
constexpr uint8_t KEY_C = 2;       // clear
constexpr uint8_t KEY_AC = 3;      // all clear
constexpr uint8_t KEY_SHIFT = 4;   // yellow operations
constexpr uint8_t KEY_F = 5;       // function key, blue operations
constexpr uint8_t KEY_POW = 6;     // power
constexpr uint8_t KEY_YROOT = 7;   // y root
constexpr uint8_t KEY_INV = 8;     // invert
constexpr uint8_t KEY_LN = 9;      // natural log
constexpr uint8_t KEY_LOGY = 10;   // log base y
constexpr uint8_t KEY_SIN = 11;    // sine
constexpr uint8_t KEY_COS = 12;    // cosine
constexpr uint8_t KEY_TAN = 13;    // tangent
constexpr uint8_t KEY_LOG = 14;    // log 10
constexpr uint8_t KEY_DEG = 15;    // deg<->rad
constexpr uint8_t KEY_CHS = 16;    // change sign +/-
constexpr uint8_t KEY_7 = 17;      // 7
constexpr uint8_t KEY_4 = 18;      // 4
constexpr uint8_t KEY_1 = 19;      // 1
constexpr uint8_t KEY_0 = 20;      // 0
constexpr uint8_t KEY_SQRT = 21;   // square root
constexpr uint8_t KEY_8 = 22;      // 8
constexpr uint8_t KEY_5 = 23;      // 5
constexpr uint8_t KEY_2 = 24;      // 2
constexpr uint8_t KEY_00 = 25;     // double 0
constexpr uint8_t KEY_PCT = 26;    // percent
constexpr uint8_t KEY_9 = 27;      // 9
constexpr uint8_t KEY_6 = 28;      // 6
constexpr uint8_t KEY_3 = 29;      // 3
constexpr uint8_t KEY_DOT = 30;    // decimal separator
constexpr uint8_t KEY_DIV = 31;    // division
constexpr uint8_t KEY_MUL = 32;    // multiplication
constexpr uint8_t KEY_MINUS = 33;  // subsctraction
constexpr uint8_t KEY_PLUS = 34;   // addition
constexpr uint8_t KEY_EQUALS = 35; // =
constexpr uint8_t KEY_MC = 36;     // memory clear
constexpr uint8_t KEY_MR = 37;     // memory read
constexpr uint8_t KEY_MS = 38;     // memory store
constexpr uint8_t KEY_MPLUS = 39;  // add to memory
constexpr uint8_t KEY_MMINUS = 40; // subtract from memory

#endif

// keyboard key event identifier
constexpr uint8_t KEY_SYNC = 0xFE;

// size of key info
constexpr uint8_t KEY_INFO_SIZE = 3;

// keyboard transmission speed
constexpr unsigned long KEYBOARD_COMM_SPEED = 4800;

// keyboard I2C address
constexpr int KEYBOARD_I2C_ADDRESS = 2;

// keyboard command indentifier
constexpr char KEYBOARD_CMDIDENTIFIER = '@';

// keyboard commands
constexpr uint8_t KEYBOARD_CMD_RESET = 1;
constexpr uint8_t KEYBOARD_CMD_GETVERSION = 2;
constexpr uint8_t KEYBOARD_CMD_SETHOLDTIME = 3;
constexpr uint8_t KEYBOARD_CMD_SETDEBOUNCETIME = 4;
constexpr uint8_t KEYBOARD_CMD_SETAUTOREPEATINTERVAL = 5;
constexpr uint8_t KEYBOARD_CMD_SETFASTAUTOREPEATINTERVAL = 6;
constexpr uint8_t KEYBOARD_CMD_SETFASTAUTOREPEATDELAY = 7;

enum class key_state : uint8_t
{
  idle,
  pressed,
  hold,
  released,
  autorepeat
};

enum class keyboard_event_category : uint8_t
{
  numeric,
  decimal,
  operation,
  function
};

enum class special_keyboard_event : uint8_t
{
  none,
  menu_mode,
  mode_switch
};

class KeyboardHandler
{

protected:
  using notifyCallBack = std::function<void(uint8_t key, key_state keyState,
                                            bool functionKeyPressed,
                                            bool shiftKeyPressed,
                                            special_keyboard_event specialEvent)>;
  using notifyRawCallBack = std::function<void(uint8_t key, key_state keyState)>;

public:
  KeyboardHandler()
  {
    _serialPort = nullptr;
    _majorVersion = 0;
    _minorVersion = 0;
    _revision = 0;
    _functionKeyPressed = false;
    _shiftKeyPressed = false;
    _functionKeyHold = false;
    _keyPressed = false;
    _notify = nullptr;
    _notifyRaw = nullptr;
    _lastKeyTimestamp = millis();
  }

  virtual ~KeyboardHandler()
  {
  }

  // define the serial port for communication with the keyboard
  void begin(Stream &serialPort)
  {
    _serialPort = &serialPort;
  }

  // set the callback function for keyboard event
  void attach(notifyCallBack callBack)
  {
    _notify = callBack;
  }

  // set the callback function for the event as received by the keyboard
  void attachRaw(notifyRawCallBack callBack)
  {
    _notifyRaw = callBack;
  }

  // remove callback
  void detach()
  {
    _notify = nullptr;
  }

  // return the timestamp of the last keyboard event
  unsigned long getLastKeyTimestamp() const
  {
    return (_lastKeyTimestamp);
  }

  // remove callback
  void detachRaw()
  {
    _notifyRaw = nullptr;
  }

  // read and process serial data received from the keyboard
  void process()
  {
    uint8_t buffer[KEY_INFO_SIZE];
    if (_serialPort)
    {
      while (_serialPort->available())
      {
        // check for sync byte
        if (_serialPort->peek() == KEY_SYNC)
        {
          // we got sync byte
          if (_serialPort->available() >= KEY_INFO_SIZE)
          {
            // we have enough bytes to read
            _serialPort->readBytes(buffer, KEY_INFO_SIZE);
            if (_notifyRaw)
            {
              _notifyRaw(buffer[1], (key_state)buffer[2]);
            }
            if (_notify)
            {
              notifyKeyboardEvent(buffer[1], (key_state)buffer[2]);
            }
          }
        }
        else
        {
          // we are out of sync, dismiss byte
          _serialPort->read();
        }
      }
    }
  }

  // provide extended information about the keyboard event and notify
  void notifyKeyboardEvent(uint8_t key, key_state state)
  {
    special_keyboard_event specialEvent = special_keyboard_event::none;
    switch (key)
    {
    // check if function key is pressed
    case KEY_F:
      switch (state)
      {
      case key_state::released:
        if (!_functionKeyHold)
        {
          if (!_keyPressed)
          {
            specialEvent = special_keyboard_event::mode_switch;
          }
        }
        _functionKeyPressed = false;
        _functionKeyHold = false;
        _keyPressed = false;
        _lastKeyTimestamp = millis();
        break;

      case key_state::pressed:
        _functionKeyPressed = true;
        break;

      case key_state::hold:
        _functionKeyHold = true;
        if (!_keyPressed)
        {
          specialEvent = special_keyboard_event::menu_mode;
        }
        break;

      case key_state::autorepeat:
        _functionKeyPressed = true;
        break;

      default:
        _functionKeyPressed = false;
        break;
      }
      break;

    case KEY_SHIFT:
      switch (state)
      {
      case key_state::pressed:
        _shiftKeyPressed = true;
        break;

      default:
        _shiftKeyPressed = false;
        break;
      }
      break;

    default:
      switch (state)
      {
      case key_state::pressed:
        if (_functionKeyPressed)
        {
          _keyPressed = true;
        }
        break;

      case key_state::released:
        _lastKeyTimestamp = millis();
        break;

      case key_state::autorepeat:
      case key_state::hold:
      case key_state::idle:
        // do nothing
        break;
      }
      break;
    }
    _notify(key, state, _functionKeyPressed, _shiftKeyPressed, specialEvent);
  }

  // set the time (ms) after a pressed key changes to hold state
  bool setHoldTime(uint16_t holdTime) const
  {
    bool returnValue = false;
    Wire.beginTransmission(KEYBOARD_I2C_ADDRESS);
    Wire.write(KEYBOARD_CMDIDENTIFIER);
    Wire.write(KEYBOARD_CMD_SETHOLDTIME);
    writeUInt(holdTime);
    byte error = Wire.endTransmission();
    if (error == 0)
    {
      returnValue = true;
    }
    return returnValue;
  }

  // set the debounce time (ms), modify only if you get more events as expected
  bool setDebounceTime(uint16_t debounceTime) const
  {
    bool returnValue = false;
    Wire.beginTransmission(KEYBOARD_I2C_ADDRESS);
    Wire.write(KEYBOARD_CMDIDENTIFIER);
    Wire.write(KEYBOARD_CMD_SETDEBOUNCETIME);
    writeUInt(debounceTime);
    byte error = Wire.endTransmission();
    if (error == 0)
    {
      returnValue = true;
    }
    return returnValue;
  }

  // set the time (ms) between autorepeat events if a key is hold
  bool setAutoRepeatInterval(uint16_t interval) const
  {
    bool returnValue = false;
    Wire.beginTransmission(KEYBOARD_I2C_ADDRESS);
    Wire.write(KEYBOARD_CMDIDENTIFIER);
    Wire.write(KEYBOARD_CMD_SETAUTOREPEATINTERVAL);
    writeUInt(interval);
    byte error = Wire.endTransmission();
    if (error == 0)
    {
      returnValue = true;
    }
    return returnValue;
  }

  // set the time (ms) between autorepeat events after the fast autorepeat delay
  bool setFastAutoRepeatInterval(uint16_t interval) const
  {
    bool returnValue = false;
    Wire.beginTransmission(KEYBOARD_I2C_ADDRESS);
    Wire.write(KEYBOARD_CMDIDENTIFIER);
    Wire.write(KEYBOARD_CMD_SETFASTAUTOREPEATINTERVAL);
    writeUInt(interval);
    byte error = Wire.endTransmission();
    if (error == 0)
    {
      returnValue = true;
    }
    return returnValue;
  }

  // set the number of autorepeat events before changing to the fast autorepeat interval
  bool setFastAutoRepeatDelay(uint16_t interval) const
  {
    bool returnValue = false;
    Wire.beginTransmission(KEYBOARD_I2C_ADDRESS);
    Wire.write(KEYBOARD_CMDIDENTIFIER);
    Wire.write(KEYBOARD_CMD_SETFASTAUTOREPEATDELAY);
    writeUInt(interval);
    byte error = Wire.endTransmission();
    if (error == 0)
    {
      returnValue = true;
    }
    return returnValue;
  }

  // reset the keyboard
  boolean resetKeyboard() const
  {
    bool returnValue = false;
    Wire.beginTransmission(KEYBOARD_I2C_ADDRESS);
    Wire.write(KEYBOARD_CMDIDENTIFIER);
    Wire.write(KEYBOARD_CMD_RESET);
    byte error = Wire.endTransmission();
    if (error == 0)
    {
      returnValue = true;
    }
    return returnValue;
  }

  // request the firmware version of the keyboard
  void requestVersion()
  {
    Wire.beginTransmission(KEYBOARD_I2C_ADDRESS);
    Wire.write(KEYBOARD_CMDIDENTIFIER);
    Wire.write(KEYBOARD_CMD_GETVERSION);
    Wire.endTransmission();
    Wire.requestFrom(KEYBOARD_I2C_ADDRESS, (int)3);
    if (Wire.available())
    {
      _majorVersion = Wire.read();
      _minorVersion = Wire.read();
      _revision = Wire.read();
    }
  }

  // return the major version of the keyboard firmware
  int getMajorVersion() const
  {
    return (_majorVersion);
  }

  // return the minor version of the keyboard firmware
  int getMinorVersion() const
  {
    return (_minorVersion);
  }

  // return the revision of the keyboard firmware
  int getRevision() const
  {
    return (_revision);
  }

private:
  Stream *_serialPort;
  unsigned int _autoRepeatInterval;
  notifyCallBack _notify;
  notifyRawCallBack _notifyRaw;
  int _majorVersion;
  int _minorVersion;
  int _revision;
  bool _functionKeyPressed;
  bool _shiftKeyPressed;
  bool _functionKeyHold;
  bool _keyPressed;
  unsigned long _lastKeyTimestamp;

  // helper function to send a 2 byte value to the keyboard
  void writeUInt(uint16_t value) const
  {
    byte hiByte = (value) >> 8;
    byte loByte = value & 0x00FF;
    Wire.write(hiByte);
    Wire.write(loByte);
  }
};
