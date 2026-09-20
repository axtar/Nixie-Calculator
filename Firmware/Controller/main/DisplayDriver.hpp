// DisplayDriver.hpp

// provides basic functions to drive
// the high voltage shift registers
// and functions to show numbers,
// decimal separators, the +/- signs
// and the LEDs

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <functional>
#include <cstring>
#include <Config.h>
#include <SettingDefs.h>
#include <LedStrip.hpp>
#include <DisplayHAL.hpp>
#include <DisplayHAL_IN12.hpp>
#include <DisplayHAL_IN16.hpp>
#include <DisplayHAL_IN17.hpp>
#include <DisplayHAL_B5870.hpp>
#include <DisplayHAL_LED.hpp>
#include <M7219Driver.hpp>

constexpr uint8_t DIGIT_OFF = 255;

// max number of special char digits
constexpr uint8_t MAX_SPECIAL_CHARS_DIGITS = 2;

// nixie dimming is done via PWM on the blank line
constexpr uint32_t NIXIE_BLANK_PWM_FREQ = 20000;  // Hz
constexpr uint8_t NIXIE_BLANK_PWM_RESOLUTION = 8; // bits, duty range 0-255
constexpr uint8_t NIXIE_BLANK_MIN_DUTY = 40;
constexpr uint8_t NIXIE_BLANK_MAX_DUTY = 255;

// shift transition
constexpr uint8_t SHIFT_BEGIN = HIGH;
constexpr uint8_t SHIFT_COMMIT = LOW;

// store transition
constexpr uint8_t STORE_BEGIN = LOW;
constexpr uint8_t STORE_COMMIT = HIGH;

// SPI
constexpr uint32_t NIXIE_SPI_CLOCK_HZ = 2000000; // 2 MHz, safe margin over bit-banged speed
constexpr uint8_t NIXIE_SPI_MODE = SPI_MODE1;

// crossfade fakes a blend by rapidly alternating old/new cathode selection over time
constexpr uint16_t DIGIT_FADE_DURATION_MS = 250;

// rolling spins through a full 10-19 frame lap, so it needs more time per frame than crossfade
constexpr uint16_t DIGIT_ROLL_DURATION_MS = 300;

constexpr uint32_t NIXIE_REFRESH_INTERVAL_US = 4000; // 250 Hz

enum class display_state
{
  off,
  on
};

enum class digit_content
{
  time,
  time2,
  date,
  temp,
  calc_minus,
  calc_plus,
  calc_error,
  exp_minus,
  exp_plus,
  none
};

class DisplayDriver
{
protected:
  using commitCallback = std::function<void()>;

public:
  DisplayDriver(display_type displayType, uint8_t dataPin,
                uint8_t storePin, uint8_t shiftPin, uint8_t blankPin,
                uint8_t ledCtlPin) : _displayType(displayType),
                                     _dataPin(dataPin),
                                     _storePin(storePin),
                                     _shiftPin(shiftPin),
                                     _blankPin(blankPin),
                                     _ledCtlPin(ledCtlPin),
                                     _dispHAL(nullptr),
                                     _leds(nullptr),
                                     _m7219drv(nullptr),
                                     _blankPWMAttached(false),
                                     _spi(VSPI),
                                     _shiftBuffer(nullptr),
                                     _shiftBufferSize(0),
                                     _shiftBitIndex(0),
                                     _displayedDigit(nullptr),
                                     _transitionFrom(nullptr),
                                     _transitionStartMs(nullptr),
                                     _transitionDurationMs(nullptr),
                                     _transitionMode(nullptr),
                                     _ditherError(nullptr),
                                     _refreshTaskHandle(nullptr)
  {
    // select HAL
    switch (_displayType)
    {
    case display_type::in12a:
      _dispHAL = new DisplayHAL_IN12();
      _dispHAL->setDecimalSeparatorPosition(decimal_separator_position::right);
      break;

    case display_type::in12b:
      _dispHAL = new DisplayHAL_IN12();
      _dispHAL->setDecimalSeparatorPosition(decimal_separator_position::left);
      break;

    case display_type::in16:
      _dispHAL = new DisplayHAL_IN16();
      break;

    case display_type::in17:
      _dispHAL = new DisplayHAL_IN17();
      break;

    case display_type::b5870:
      _dispHAL = new DisplayHAL_B5870();
      break;

    case display_type::led:
      _dispHAL = new DisplayHAL_LED();
      _m7219drv = new M7219Driver(_spi, _dataPin, _shiftPin, _storePin, CHAIN_SIZE);
      break;

    case display_type::undefined:
      // not allowed
      break;

    default: // avoid warning
      break;
    }

    // nixie displays are dimmed via PWM on the blank line
    // the LED display uses the MAX7219's own intensity register instead
    if (_displayType != display_type::led)
    {
      pinMode(_blankPin, OUTPUT);
      digitalWrite(_blankPin, HIGH);
      pinMode(_storePin, OUTPUT);
      digitalWrite(_storePin, STORE_COMMIT);
      _spi.begin(_shiftPin, -1, _dataPin, -1);
      _shiftBufferSize = (_dispHAL->getRegisterCount() + 7) / 8;
      _shiftBuffer = new uint8_t[_shiftBufferSize];
      memset(_shiftBuffer, 0, _shiftBufferSize);
    }

    // frequently used hardware specific parameters
    _digitCount = _dispHAL->getDigitCount();
    _decimalSeparatorCount = _dispHAL->getDecimalSeparatorCount();
    _ledCount = _dispHAL->getLEDCount();

    // array of digits
    // digit 0 is the most left digit
    _digits = new uint8_t[_digitCount];
    _digitsContent = new digit_content[_digitCount];

    // per-digit transition state
    {
      _displayedDigit = new uint8_t[_digitCount];
      _transitionFrom = new uint8_t[_digitCount];
      _transitionStartMs = new unsigned long[_digitCount];
      _transitionDurationMs = new uint16_t[_digitCount];
      _transitionMode = new time_effects::time_effects[_digitCount];
      _ditherError = new float[_digitCount];
      memset(_displayedDigit, 0, _digitCount * sizeof(uint8_t));
      memset(_transitionFrom, 0, _digitCount * sizeof(uint8_t));
      memset(_transitionStartMs, 0, _digitCount * sizeof(unsigned long));
      memset(_transitionDurationMs, 0, _digitCount * sizeof(uint16_t));
      memset(_transitionMode, 0, _digitCount * sizeof(time_effects::time_effects));
      memset(_ditherError, 0, _digitCount * sizeof(float));
    }

    if (_displayType == display_type::led)
    {
      _ledShown = new uint8_t[_digitCount];
      memset(_ledShown, DIGIT_OFF, _digitCount * sizeof(uint8_t));
      _ledCommitMutex = xSemaphoreCreateMutex();
    }

    // used for IN-15A
    _specialChars = new uint8_t[MAX_SPECIAL_CHARS_DIGITS];

    // array of decimal separators
    _decimalSeparators = new display_state[_decimalSeparatorCount];

    // set decimal position offset
    _dspOffset = _dispHAL->getDecimalSeparatorPosition() == decimal_separator_position::left ? 1 : 0;
    // initialize the LED strip driver according to the LED type used
    switch (_dispHAL->getLedType())
    {
    case led_type::smd:
      _leds = new LedStrip(_ledCtlPin, _ledCount);
      break;

    case led_type::none:
      _leds = nullptr;
      break;
    }
    if (_leds)
    {
      // array of LED colors
      _pixelColors = new uint32_t[_ledCount];
      _currentColors = new uint32_t[_ledCount];
      memset(_currentColors, 0, _ledCount * sizeof(uint32_t));
    }
    clear();
  };

  virtual ~DisplayDriver()
  {
    delete[] _digits;
    delete[] _specialChars;
    delete[] _digitsContent;
    delete[] _decimalSeparators;
    delete _dispHAL;
    if (_leds)
    {
      delete _leds;
      delete[] _pixelColors;
      delete[] _currentColors;
    }
    if (_m7219drv)
    {
      delete _m7219drv;
    }
    if (_refreshTaskHandle)
    {
      vTaskDelete(_refreshTaskHandle);
    }
    delete[] _shiftBuffer;
    delete[] _displayedDigit;
    delete[] _transitionFrom;
    delete[] _transitionStartMs;
    delete[] _transitionDurationMs;
    delete[] _transitionMode;
    delete[] _ditherError;
    delete[] _ledShown;
    if (_ledCommitMutex)
    {
      vSemaphoreDelete(_ledCommitMutex);
    }
  }

  // initialization
  void begin()
  {
    if (_leds)
    {
      // LEDs already initialized in the constructor
      clearLEDs();
    }

    // a dedicated background task to refresh the display
    if (!_refreshTaskHandle)
    {
      xTaskCreatePinnedToCore([](void *t)
                              { static_cast<DisplayDriver *>(t)->refreshTaskLoop(); },
                              "displayRefresh", (_displayType == display_type::led) ? 3072 : 2048, this, tskIDLE_PRIORITY + 5, &_refreshTaskHandle, 0);
    }
  }

  // turn off all the LEDs
  void clearLEDs()
  {
    if (_leds)
    {
      _leds->clear();
      memset(_currentColors, 0, _ledCount * sizeof(uint32_t));
    }
    notifyCommit();
  }

  // return display type
  display_type getDisplayType() const
  {
    return (_displayType);
  }

  // set the color of a specific LED by RGB values
  void setLED(uint16_t ledID, uint8_t red, uint8_t green, uint8_t blue)
  {
    if (_leds)
    {
      _leds->setPixel(ledID, red, green, blue);
      _currentColors[ledID] = ((uint32_t)red << 16) | ((uint32_t)green << 8) | blue;
    }
  }

  // set the color of a specific LED by the color value
  void setLED(uint16_t ledID, uint32_t color)
  {
    if (_leds)
    {
      setLED(ledID, (uint8_t)(color >> 16), (uint8_t)(color >> 8), (uint8_t)color);
    }
  }

  // return the current color of a specific LED
  uint32_t getLED(uint16_t ledID) const
  {
    if (_leds)
    {
      return (_currentColors[ledID]);
    }
    else
    {
      return (0);
    }
  }

  // set the color of all LEDs
  void setAllLED(uint8_t red, uint8_t green, uint8_t blue)
  {
    for (int i = 0; i < _ledCount; i++)
    {
      setLED(i, red, green, blue);
    }
  }

  // turn off a single LED
  void clearLED(uint16_t ledID)
  {
    if (_leds)
    {
      _leds->setPixel(ledID, 0, 0, 0);
      _currentColors[ledID] = 0;
    }
  }

  // update the LEDs
  void updateLEDs()
  {
    if (_leds)
    {
      _leds->refresh();
    }
    notifyCommit();
  }

  // return the number of digits on the display board
  uint8_t getDigitCount() const
  {
    return (_digitCount);
  }

  // return the number of decimal separators on the display board
  uint8_t getDecimalSeparatorCount() const
  {
    return (_decimalSeparatorCount);
  }

  // return the number of LEDs on the display board
  uint8_t getLedCount() const
  {
    return (_ledCount);
  }

  // return the decimal separator position
  decimal_separator_position getDecimalSeparatorPosition()
  {
    return (_dispHAL->getDecimalSeparatorPosition());
  }

  // return decimal separator position offset
  uint8_t getDspOffset() const
  {
    return (_dspOffset);
  }

  // return if the display board has a plus sign
  bool hasPlusSign() const
  {
    return (_dispHAL->hasPlusSign());
  }

  // return if the display board as a menu sign, not used for now
  bool hasMenuSign() const
  {
    return (_dispHAL->hasMenuSign());
  }

  bool hasExpMinusSign() const
  {
    return (_dispHAL->hasExpMinusSign());
  }

  bool hasExpPlusSign() const
  {
    return (_dispHAL->hasExpPlusSign());
  }

  // set the value of a specific digit
  bool setDigit(uint8_t digit, uint8_t value, digit_content content = digit_content::none)
  {
    bool changed = false;
    if (digit < _digitCount)
    {
      if (getDigit(digit) != value)
      {
        changed = true;
      }
      _digits[digit] = value;
      setDigitContent(digit, content);

      // instant set: cancel any in-flight crossfade and show the new value right away
      if (_displayedDigit)
      {
        portENTER_CRITICAL(&_fadeMux);
        _transitionDurationMs[digit] = 0;
        _displayedDigit[digit] = value;
        portEXIT_CRITICAL(&_fadeMux);
      }
    }
    return (changed);
  }

  // set a digit transitioning to the new value via crossfade or rolling
  bool setDigitFaded(uint8_t digit, uint8_t value, uint16_t durationMs,
                     time_effects::time_effects mode, digit_content content = digit_content::none,
                     int explicitFrom = -1, bool force = false)
  {
    bool changed = false;
    if ((digit < _digitCount) && _displayedDigit)
    {
      if (force || (getDigit(digit) != value))
      {
        changed = true;
        portENTER_CRITICAL(&_fadeMux);
        uint8_t fromValue = (explicitFrom >= 0) ? static_cast<uint8_t>(explicitFrom) : _displayedDigit[digit];
        if (fromValue > 9)
        {
          // nothing valid to transition from - show the new value directly
          _transitionDurationMs[digit] = 0;
          _displayedDigit[digit] = value;
        }
        else
        {
          _transitionFrom[digit] = fromValue;
          _displayedDigit[digit] = fromValue;
          _transitionStartMs[digit] = millis();
          _transitionDurationMs[digit] = durationMs;
          _transitionMode[digit] = mode;
          _ditherError[digit] = 0.0f;
        }
        _digits[digit] = value;
        portEXIT_CRITICAL(&_fadeMux);
      }
      setDigitContent(digit, content);
    }
    return (changed);
  }

  // set special char
  void setSpecialChar(uint8_t digit, uint8_t value)
  {
    if (digit < MAX_SPECIAL_CHARS_DIGITS)
    {
      _specialChars[digit] = value;
    }
  }

  // return the value of a specific digit
  uint8_t getDigit(uint8_t digit) const
  {
    if (digit < _digitCount)
    {
      return (_digits[digit]);
    }
    return (DIGIT_OFF);
  }

  // set the type of content of the digit, used for lighting
  void setDigitContent(uint8_t digit, digit_content content)
  {
    if (digit < _digitCount)
    {
      _digitsContent[digit] = content;
    }
  }

  // return the type of content of the digit, used for lighting
  digit_content getDigitContent(uint8_t digit) const
  {
    if (digit < _digitCount)
    {
      return (_digitsContent[digit]);
    }
    return (digit_content::none);
  }

  // return the decimal separator status
  display_state getDecimalSeparator(uint8_t decimalSeparator) const
  {
    if (decimalSeparator < _decimalSeparatorCount)
    {
      return (_decimalSeparators[decimalSeparator]);
    }
    return (display_state::off);
  }

  // return the special char value
  uint8_t getSpecialChar(uint8_t digit) const
  {
    if (digit < MAX_SPECIAL_CHARS_DIGITS)
    {
      return (_specialChars[digit]);
    }
    return (DIGIT_OFF);
  }

  // return the minus sign status
  bool isMinusSignOn() const
  {
    return (_minusSign == display_state::on);
  }

  // return the plus sign status
  bool isPlusSignOn() const
  {
    return (_plusSign == display_state::on);
  }

  // return the menu sign status
  bool isMenuSignOn() const
  {
    return (_menuSign == display_state::on);
  }

  // return the exponent minus sign status
  bool isExpMinusSignOn() const
  {
    return (_expMinusSign == display_state::on);
  }

  // return the exponent plus sign status
  bool isExpPlusSignOn() const
  {
    return (_expPlusSign == display_state::on);
  }

  // set the decimal separator status, on or off
  void setDecimalSeparator(uint8_t decimalSeparator, display_state state)
  {
    if (decimalSeparator < _decimalSeparatorCount)
    {
      _decimalSeparators[decimalSeparator] = state;
    }
  }

  // set the minus sign status, on or off
  void setMinusSign(display_state state)
  {
    _minusSign = state;
  }

  // set the plus sign status, on or off
  void setPlusSign(display_state state)
  {
    _plusSign = state;
  }

  // set the plus sign status, on or off
  void setExpMinusSign(display_state state)
  {
    _expMinusSign = state;
  }

  // set the plus sign status, on or off
  void setExpPlusSign(display_state state)
  {
    _expPlusSign = state;
  }

  // set the menu sign status, on or off
  void setMenuSign(display_state state)
  {
    _menuSign = state;
  }

  // store LED colors in memory
  void saveLEDColors()
  {
    if (_leds)
    {
      for (int i = 0; i < _ledCount; i++)
      {
        _pixelColors[i] = _currentColors[i];
      }
    }
  }

  // restore LED colors from memory
  void restoreLEDColors()
  {
    if (_leds)
    {
      for (int i = 0; i < _ledCount; i++)
      {
        setLED(i, _pixelColors[i]);
      }
    }
  }

  // refresh the display
  void refresh()
  {
    commitToDisplay();
    notifyCommit();
  }

  // set a callback invoked after every refresh() kept intentionally lightweight
  void attachCommitCb(commitCallback cb)
  {
    _onCommit = cb;
  }

  // clear all the values
  void clear()
  {
    clearDecimalSeparators();
    clearDigits();
    clearSpecialChars();
    clearDigitsContent();
    clearMinusSign();
    clearExpMinusSign();
    clearExpPlusSign();
    clearPlusSign();
    clearMenuSign();
  }

  // clear the display
  void clearDisplay()
  {
    clear();
    refresh();
  }

  // clear all the decimal separators
  void clearDecimalSeparators()
  {
    for (uint8_t i = 0; i < _decimalSeparatorCount; i++)
    {
      _decimalSeparators[i] = display_state::off;
    }
  }

  // clear all the digits
  void clearDigits()
  {
    for (uint8_t i = 0; i < _digitCount; i++)
    {
      _digits[i] = DIGIT_OFF;
    }

    // keep the hardware-facing view in sync, otherwise a cleared digit stays lit
    if (_displayedDigit)
    {
      portENTER_CRITICAL(&_fadeMux);
      for (uint8_t i = 0; i < _digitCount; i++)
      {
        _transitionDurationMs[i] = 0;
        _displayedDigit[i] = DIGIT_OFF;
      }
      portEXIT_CRITICAL(&_fadeMux);
    }
  }

  // clear all the special chars
  void clearSpecialChars()
  {
    for (uint8_t i = 0; i < MAX_SPECIAL_CHARS_DIGITS; i++)
    {
      _specialChars[i] = DIGIT_OFF;
    }
  }

  // clear digit content
  void clearDigitsContent()
  {
    for (uint8_t i = 0; i < _digitCount; i++)
    {
      _digitsContent[i] = digit_content::none;
    }
  }

  // clear the minus sign
  void clearMinusSign()
  {
    setMinusSign(display_state::off);
  }

  // clear the exponent sign
  void clearExpMinusSign()
  {
    setExpMinusSign(display_state::off);
  }

  // clear the exponent sign
  void clearExpPlusSign()
  {
    setExpPlusSign(display_state::off);
  }

  // clear the menu sign
  void clearMenuSign()
  {
    setMenuSign(display_state::off);
  }

  // clear the plus sign
  void clearPlusSign()
  {
    setPlusSign(display_state::off);
  }

  // return if a digit is on
  bool isDigitOn(uint8_t digit) const
  {
    bool retVal = false;
    if (digit < _digitCount)
    {
      if (_digits[digit] != DIGIT_OFF)
      {
        retVal = true;
      }
    }
    return (retVal);
  }

  // set display brightness
  void setDisplayBrightness(int value)
  {
    if (_m7219drv)
    {
      _m7219drv->setBrightness(static_cast<uint8_t>(value));
    }
    else
    {
      int validValue = constrain(value, 1, 15);
      if (validValue >= 15)
      {
        // max brightness: drive the blank line statically HIGH, no PWM switching
        if (_blankPWMAttached)
        {
          ledcDetach(_blankPin);
          _blankPWMAttached = false;
          pinMode(_blankPin, OUTPUT);
        }
        digitalWrite(_blankPin, HIGH);
      }
      else
      {
        if (!_blankPWMAttached)
        {
          ledcAttach(_blankPin, NIXIE_BLANK_PWM_FREQ, NIXIE_BLANK_PWM_RESOLUTION);
          _blankPWMAttached = true;
        }
        // map the setting range onto the usable PWM duty cycle range
        uint8_t duty = static_cast<uint8_t>(map(validValue, 1, 14, NIXIE_BLANK_MIN_DUTY, NIXIE_BLANK_MAX_DUTY));
        ledcWrite(_blankPin, duty);
      }
    }
  }

  // returns LED driver
  M7219Driver *getLEDDriver()
  {
    return (_m7219drv);
  }

  // temporary: returns the nixie refresh task handle, used to check its stack high water mark from the status page
  TaskHandle_t getRefreshTaskHandle() const
  {
    return (_refreshTaskHandle);
  }

private:
  uint8_t _digitCount;
  uint8_t _decimalSeparatorCount;
  uint8_t _ledCount;
  display_type _displayType;
  uint8_t *_digits;
  uint8_t *_specialChars;
  digit_content *_digitsContent;
  display_state *_decimalSeparators;
  display_state _minusSign;
  display_state _plusSign;
  display_state _expMinusSign;
  display_state _expPlusSign;
  display_state _menuSign;
  uint32_t *_pixelColors;
  uint32_t *_currentColors;
  uint8_t _dataPin;
  uint8_t _storePin;
  uint8_t _shiftPin;
  uint8_t _blankPin;
  uint8_t _ledCtlPin;
  DisplayHAL *_dispHAL;
  LedStrip *_leds;
  M7219Driver *_m7219drv;
  bool _blankPWMAttached;
  uint8_t _dspOffset;
  commitCallback _onCommit;
  SPIClass _spi;
  uint8_t *_shiftBuffer;
  uint16_t _shiftBufferSize;
  uint16_t _shiftBitIndex;

  // per-digit crossfade state (nixie only), guarded by _fadeMux across the two tasks
  uint8_t *_displayedDigit;
  uint8_t *_transitionFrom;
  unsigned long *_transitionStartMs;
  uint16_t *_transitionDurationMs;
  time_effects::time_effects *_transitionMode;
  float *_ditherError;
  uint8_t *_ledShown = nullptr;
  SemaphoreHandle_t _ledCommitMutex = nullptr;
  TaskHandle_t _refreshTaskHandle;
  portMUX_TYPE _fadeMux = portMUX_INITIALIZER_UNLOCKED;

  // invoked whenever anything visible changes
  void notifyCommit() const
  {
    if (_onCommit)
    {
      _onCommit();
    }
  }

  // commit a digit; nixie displays serialize the fade-resolved value, not the logical target
  void commitDigitNumber(uint8_t digit, uint8_t number)
  {
    uint8_t shownValue = _displayedDigit ? _displayedDigit[digit] : _digits[digit];
    commitBit(shownValue == number ? HIGH : LOW);
  }

  // commit a special char to the shift registers
  void commitSpecialChar(uint8_t digit, uint8_t number)
  {
    commitBit(_specialChars[digit] == number ? HIGH : LOW);
  }

  // return if special char should be displayed
  bool checkSpecialChar(uint8_t digit, uint8_t number) const
  {
    return (_specialChars[digit] == number ? true : false);
  }

  // commit a decimal separator to the shift registers
  void commitDecimalSeparator(int8_t decimalSeparator)
  {
    commitBit(_decimalSeparators[decimalSeparator] == display_state::off ? LOW : HIGH);
  }

  // commit the minus sign to the shift registers
  void commitMinusSign()
  {
    commitBit(_minusSign == display_state::off ? LOW : HIGH);
  }

  // commit the plus sign to the shift registers
  void commitPlusSign()
  {
    commitBit(_plusSign == display_state::off ? LOW : HIGH);
  }

  // commit the exponent minus sign to the shift registers
  void commitExpMinusSign()
  {
    commitBit(_expMinusSign == display_state::off ? LOW : HIGH);
  }

  // commit the exponent plus sign to the shift registers
  void commitExpPlusSign()
  {
    commitBit(_expPlusSign == display_state::off ? LOW : HIGH);
  }

  // commit the menu sign to the shift registers
  void commitMenuSign()
  {
    commitBit(_menuSign == display_state::off ? LOW : HIGH);
  }

  // pack a bit into the pending SPI transfer buffer
  void commitBit(uint8_t value)
  {
    uint16_t byteIndex = _shiftBitIndex >> 3;
    uint8_t bitMask = 0x80 >> (_shiftBitIndex & 0x07);
    if (value)
    {
      _shiftBuffer[byteIndex] |= bitMask;
    }
    else
    {
      _shiftBuffer[byteIndex] &= ~bitMask;
    }
    _shiftBitIndex++;
  }

  // commit values to the display
  void commitToDisplay(bool changedOnly = false)
  {
    if (_displayType == display_type::led)
    {
      // commit to 7-seg LED display
      uint8_t index = 0;
      uint8_t pos = 0;
      if (_m7219drv)
      {
        xSemaphoreTake(_ledCommitMutex, portMAX_DELAY);
        if (!changedOnly)
        {
          // commit base sign
          _dispHAL->getDigitAddress(BASE_SIGN, &index, &pos);
          if (_minusSign == display_state::on)
          {
            _m7219drv->setChar(index, pos, CHAR_MINUS, false);
          }
          else
          {
            _m7219drv->setChar(index, pos, CHAR_BLANK, false);
          }
        }

        for (uint8_t i = 0; i < LED_DIGITCOUNT; i++)
        {
          uint8_t shown = _displayedDigit[i];
          if (changedOnly && (_ledShown[i] == shown))
          {
            continue;
          }
          _ledShown[i] = shown;
          _dispHAL->getDigitAddress(i + 1, &index, &pos);
          bool dp = (_decimalSeparators[i] == display_state::on);
          if (shown == DIGIT_OFF)
          {
            _m7219drv->setChar(index, pos, CHAR_BLANK, dp);
          }
          else
          {
            _m7219drv->setChar(index, pos, shown, dp);
          }
        }
        if (!changedOnly)
        {
          // commit exponent sign
          _dispHAL->getDigitAddress(EXPONENT_SIGN, &index, &pos);
          if (_expMinusSign == display_state::on)
          {
            _m7219drv->setChar(index, pos, CHAR_MINUS, false);
          }
          else
          {
            _m7219drv->setChar(index, pos, CHAR_BLANK, false);
          }
        }
        xSemaphoreGive(_ledCommitMutex);
      }
    }
  }

  // serialize the display state and push it over SPI; called only from the refresh task
  void pushNixieFrame()
  {
    register_type regType;
    uint8_t digit = 0;
    uint8_t number = 0;
    _shiftBitIndex = 0;

    for (uint8_t i = _dispHAL->getRegisterCount(); i > 0; i--)
    {
      regType = _dispHAL->getRegisterInfo(i, &digit, &number);
      switch (regType)
      {
      case register_type::unknown:
        break;

      case register_type::minus_sign:
        if (checkSpecialChar(digit, number))
        {
          commitSpecialChar(digit, number);
        }
        else
        {
          commitMinusSign();
        }
        break;

      case register_type::plus_sign:
        if (checkSpecialChar(digit, number))
        {
          commitSpecialChar(digit, number);
        }
        else
        {
          commitPlusSign();
        }
        break;

      case register_type::exp_minus_sign:
        if (checkSpecialChar(digit, number))
        {
          commitSpecialChar(digit, number);
        }
        else
        {
          commitExpMinusSign();
        }
        break;

      case register_type::exp_plus_sign:
        if (checkSpecialChar(digit, number))
        {
          commitSpecialChar(digit, number);
        }
        else
        {
          commitExpPlusSign();
        }
        break;

      case register_type::menu_sign:
        if (checkSpecialChar(digit, number))
        {
          commitSpecialChar(digit, number);
        }
        else
        {
          commitMenuSign();
        }
        break;

      case register_type::decimal_separator:
        commitDecimalSeparator(digit);
        break;

      case register_type::number:
        commitDigitNumber(digit, number);
        break;

      case register_type::special_char:
        commitSpecialChar(digit, number);
        break;

      case register_type::not_used:
      case register_type::not_connected:
        commitBit(LOW);
        break;
      }
    }

    // push the packed pattern over SPI
    digitalWrite(_storePin, STORE_BEGIN);
    _spi.beginTransaction(SPISettings(NIXIE_SPI_CLOCK_HZ, MSBFIRST, NIXIE_SPI_MODE));
    _spi.transferBytes(_shiftBuffer, nullptr, _shiftBufferSize);
    _spi.endTransaction();
    digitalWrite(_storePin, STORE_COMMIT);
  }

  // advance every in-flight digit crossfade by one tick and resolve _displayedDigit[]
  void updateFadeState()
  {
    unsigned long now = millis();
    portENTER_CRITICAL(&_fadeMux);
    for (uint8_t i = 0; i < _digitCount; i++)
    {
      if (_transitionDurationMs[i] == 0)
      {
        continue;
      }
      float progress = static_cast<float>(now - _transitionStartMs[i]) / static_cast<float>(_transitionDurationMs[i]);
      if (progress >= 1.0f)
      {
        _displayedDigit[i] = _digits[i];
        _transitionDurationMs[i] = 0;
        _ditherError[i] = 0.0f;
        continue;
      }

      if (_transitionMode[i] != time_effects::crossfade)
      {
        // odometer-style (any rolling1/2/3 variant): always spin a full 0-9 lap plus the real distance, e.g. 0->1 rolls 1,2,...,9,0,1
        uint8_t minimalSteps = (_digits[i] + 10 - _transitionFrom[i]) % 10;
        uint8_t steps = 10 + minimalSteps;

        // divide the fade into (steps + 1) equal slices so the roll lands exactly on the target
        uint8_t currentStep = static_cast<uint8_t>(progress * (steps + 1));
        if (currentStep > steps)
        {
          currentStep = steps;
        }
        _displayedDigit[i] = (_transitionFrom[i] + currentStep) % 10;
      }
      else
      {
        // crossfade: dither between old/new cathode selection in proportion to progress
        _ditherError[i] += progress;
        if (_ditherError[i] >= 1.0f)
        {
          _ditherError[i] -= 1.0f;
          _displayedDigit[i] = _digits[i];
        }
        else
        {
          _displayedDigit[i] = _transitionFrom[i];
        }
      }
    }
    portEXIT_CRITICAL(&_fadeMux);
  }

  // background task body: continuously resolves crossfades and pushes fresh frames
  void refreshTaskLoop()
  {
    TickType_t lastWake = xTaskGetTickCount();
    const TickType_t period = pdMS_TO_TICKS(NIXIE_REFRESH_INTERVAL_US / 1000);
    while (true)
    {
      updateFadeState();
      if (_displayType == display_type::led)
      {
        commitToDisplay(true);
      }
      else
      {
        pushNixieFrame();
      }
      vTaskDelayUntil(&lastWake, period > 0 ? period : 1);
    }
  }
};
