// Nixie Calculator Keyboard Firmware

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

// the keyboard receives commands from the controller using I2C
// keyboard events are sent to the controller using a serial interface

// commands:
// CMD_RESET                      -> resets the keyboard
// CMD_GETVERSION                 -> prepares for version request
// CMD_SETHOLDTIME                -> sets the time (ms) after a pressed key changes to hold state
// CMD_SETDEBOUNCETIME            -> sets the debounce time (ms), modify only if you get more events as expected
// CMD_SETAUTOREPEATINTERVAL      -> sets the time (ms) between autorepeat events if a key is hold
// CMD_SETFASTAUTOREPEATINTERVAL  -> sets the time (ms) between autorepeat events after the fast autorepeat delay
// CMD_SETFASTAUTOREPEATDELAY     -> sets the number of autorepeat events before changing to the fast autorepeat interval

// default event sequence:
// key pressed  -> "pressed" event
// key released -> "released" event, "idle" event

// event sequence with default values and hold:
// ------------------------------------------------------------------------------------------------
// | holdTime = 1000, autoRepeatInterval = 0, fastAutoRepeatInterval = 0, fastAutoRepeatDelay = 0 |
// ------------------------------------------------------------------------------------------------
// key pressed  -> "pressed" event
// time +1000   -> "hold" event
// ...
// key released -> "released" event, "idle" event

// event sequence example with auto repeat:
// ----------------------------------------------------------------------------------------------------
// | holdTime = 1000, autoRepeatInterval = 500, fastAutoRepeatInterval = 250, fastAutoRepeatDelay = 3 |
// ----------------------------------------------------------------------------------------------------
// key pressed  -> "pressed" event
// time +1000   -> "hold" event, "autorepeat" event
// time +1500   -> "autorepeat" event
// time +2000   -> "autorepeat" event
// time +2250   -> "autorepeat" event (fast)
// time +2500   -> "autorepeat" event (fast)
// ...
// key released -> "released" event, "idle" event

// includes
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Keypad.h>

// version information
constexpr uint8_t MAJOR_VERSION = 0;
constexpr uint8_t MINOR_VERSION = 9;
constexpr uint8_t REVISION = 4;
constexpr auto FW_STATUS = "beta";

// set to 0 to disable traces, set to 1 to enable traces
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

// I2C address
constexpr uint8_t I2C_ADDRESS = 2;

// commands
constexpr uint8_t CMDINDENTIFIER = '@';
constexpr uint8_t CMD_RESET = 1;
constexpr uint8_t CMD_GETVERSION = 2;
constexpr uint8_t CMD_SETHOLDTIME = 3;
constexpr uint8_t CMD_SETDEBOUNCETIME = 4;
constexpr uint8_t CMD_SETAUTOREPEATINTERVAL = 5;
constexpr uint8_t CMD_SETFASTAUTOREPEATINTERVAL = 6;
constexpr uint8_t CMD_SETFASTAUTOREPEATDELAY = 7;

// keys arranged in 8 x 5 matrix
constexpr uint8_t ROWS = 8;
constexpr uint8_t COLS = 5;

// pins for serial communication, RX pin is not used
constexpr uint8_t PIN_KINT = 17;
constexpr uint8_t PIN_DUMMY_RX = 2;

// this is an additional KeyState used
// for generated repeat events
constexpr auto KEYSTATE_AUTOREPEAT = 4;

// keyboard key event identifier
constexpr uint8_t KEY_SYNC = 0xFE;

// size of key event
constexpr size_t KEY_INFO_SIZE = 3;

// keyboard transmission speed
constexpr long KEYBOARD_COMM_SPEED = 4800;

// used for auto-repeat handling
typedef struct
{
  uint8_t keyCode;
  unsigned long holdTimestamp;
  uint autoRepeatCount;
} HOLD_INFO;

// key values
char keys[ROWS][COLS] = {{1, 2, 3, 4, 5},
                         {6, 7, 8, 9, 10},
                         {11, 12, 13, 14, 15},
                         {16, 17, 18, 19, 20},
                         {21, 22, 23, 24, 25},
                         {26, 27, 28, 29, 30},
                         {31, 32, 33, 34, 35},
                         {36, 37, 38, 39, 40}};

// matrix pins
byte rowPins[ROWS] = {16, 15, 14, 13, 7, 6, 5, 4};
byte colPins[COLS] = {12, 11, 10, 9, 8};

// keypad instance
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// serial interface for keyboard events
SoftwareSerial kSerial(PIN_DUMMY_RX, PIN_KINT);

// key hold timestamp array for auto repeat function
HOLD_INFO keyHoldInfo[LIST_MAX];

volatile uint16_t holdTime = 1000;
volatile uint16_t debounceTime = 10;
volatile uint16_t autoRepeatInterval = 0;
volatile uint16_t fastAutoRepeatInterval = 0;
volatile uint16_t fastAutoRepeatDelay = 0;
volatile int pendingRequest = -1;

// forward declarations
void (*reset)(void) = 0;
void receiveEvent(int count);
void requestEvent();
void onReset();
void onGetVersion();
void onSetHoldTime();
void onSetDebounceTime();
void onSetAutoRepeatInterval();
void onSetFastAutoRepeatInterval();
void onSetFastAutoRepeatDelay();

void deleteKeyHoldInfo(uint8_t keyCode);
void setKeyHoldInfo(uint8_t keyCode);
void initKeyHoldInfo();
uint16_t readUInt();

void setup()
{
  D_Begin(9600);

  // init serial connection
  kSerial.begin(KEYBOARD_COMM_SPEED);

  // init the hold timestamp table
  initKeyHoldInfo();

  // set default values
  keypad.setHoldTime(holdTime);
  keypad.setDebounceTime(debounceTime);

  // init I2C connection
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void loop()
{
  uint8_t buffer[KEY_INFO_SIZE];

  // check for keys
  if (keypad.getKeys())
  {
    for (uint8_t i = 0; i < LIST_MAX; i++)
    {
      if (keypad.key[i].stateChanged)
      {

        // some debug information
        D_print("Millis: ");
        D_print(millis());
        D_print(" ");
        D_print("Key: ");
        D_print((int)keypad.key[i].kchar);
        D_print(" ");
        D_print("State: ");
        D_println(keypad.key[i].kstate);

        // state changed, send key and state
        buffer[0] = KEY_SYNC;
        buffer[1] = keypad.key[i].kchar;
        buffer[2] = keypad.key[i].kstate;
        kSerial.write(buffer, KEY_INFO_SIZE);
        // set/delete hold timestamp
        if (autoRepeatInterval > 0)
        {
          switch (keypad.key[i].kstate)
          {
          case KeyState::HOLD:
            setKeyHoldInfo((uint8_t)keypad.key[i].kchar);
            break;

          case KeyState::RELEASED:
            deleteKeyHoldInfo((uint8_t)keypad.key[i].kchar);
            break;

          default:
            break;
          }
        }
      }
    }
  }
  // check for auto repeat
  if (autoRepeatInterval > 0)
  {
    unsigned long currentMillis = millis();
    for (uint8_t i = 0; i < LIST_MAX; i++)
    {

      if (keyHoldInfo[i].keyCode != 0)
      {
        bool genEvent = false;
        if ((keyHoldInfo[i].autoRepeatCount < fastAutoRepeatDelay) || (fastAutoRepeatDelay == 0))
        {
          // check if it is time to generate a key repeat event
          if (currentMillis - keyHoldInfo[i].holdTimestamp > autoRepeatInterval)
          {
            genEvent = true;
          }
        }
        else
        {
          // check if it is time to generate a key fast repeat event
          if (fastAutoRepeatInterval > 0)
          {
            if (currentMillis - keyHoldInfo[i].holdTimestamp > fastAutoRepeatInterval)
            {
              genEvent = true;
            }
          }
        }
        if (genEvent)
        {

          D_print("Millis: ");
          D_print(millis());
          D_print(" ");
          D_print("Key: ");
          D_print(keyHoldInfo[i].keyCode);
          D_print(" ");
          D_print("State: ");
          D_println(KEYSTATE_AUTOREPEAT);

          buffer[0] = KEY_SYNC;
          buffer[1] = keyHoldInfo[i].keyCode;
          buffer[2] = KEYSTATE_AUTOREPEAT;
          kSerial.write(buffer, KEY_INFO_SIZE);
          keyHoldInfo[i].holdTimestamp = currentMillis;
          if (fastAutoRepeatInterval > 0)
          {
            keyHoldInfo[i].autoRepeatCount++;
          }
        }
      }
    }
  }
}

// event handler for I2C commands
void receiveEvent(int count)
{
  while (Wire.available())
  {
    char c = Wire.read();
    if (c == CMDINDENTIFIER)
    {
      int command = Wire.read();
      switch (command)
      {
      case CMD_RESET:
        onReset();
        break;

      case CMD_GETVERSION:
        onGetVersion();
        break;

      case CMD_SETHOLDTIME:
        onSetHoldTime();
        break;

      case CMD_SETDEBOUNCETIME:
        onSetDebounceTime();
        break;

      case CMD_SETAUTOREPEATINTERVAL:
        onSetAutoRepeatInterval();
        break;

      case CMD_SETFASTAUTOREPEATINTERVAL:
        onSetFastAutoRepeatInterval();
        break;

      case CMD_SETFASTAUTOREPEATDELAY:
        onSetFastAutoRepeatDelay();
        break;

      default:
        break;
      }
    }
  }
}

// request handler
void requestEvent()
{
  switch (pendingRequest)
  {
  case CMD_GETVERSION:
    Wire.write(MAJOR_VERSION);
    Wire.write(MINOR_VERSION);
    Wire.write(REVISION);
    pendingRequest = -1;
    break;
  }
}

// reset the keyboard
void onReset()
{
  reset();
}

// prepare to answer version request
void onGetVersion()
{
  pendingRequest = CMD_GETVERSION;
}

// set new hold time value
void onSetHoldTime()
{
  holdTime = readUInt();
  keypad.setHoldTime(holdTime);
}

// set new debounce time value
void onSetDebounceTime()
{
  debounceTime = readUInt();
  keypad.setDebounceTime(debounceTime);
}

// set the key repeat interval
// set to 0 to disable auto repeat
void onSetAutoRepeatInterval()
{
  autoRepeatInterval = readUInt();
  initKeyHoldInfo();
  D_println(autoRepeatInterval);
}

// set the key fast repeat interval
// set to 0 to disable fast auto repeat
void onSetFastAutoRepeatInterval()
{
  fastAutoRepeatInterval = readUInt();
}

// sets the delay before changing to fast repeat interval
void onSetFastAutoRepeatDelay()
{
  fastAutoRepeatDelay = readUInt();
}

// delete the hold timestamp for this key
void deleteKeyHoldInfo(uint8_t keyCode)
{
  for (int i = 0; i < LIST_MAX; i++)
  {
    if (keyHoldInfo[i].keyCode == keyCode)
    {
      keyHoldInfo[i].keyCode = 0;
      keyHoldInfo[i].holdTimestamp = 0;
      keyHoldInfo[i].autoRepeatCount = 0;
      break;
    }
  }
}

// set the hold timestamp for this key
void setKeyHoldInfo(uint8_t keyCode)
{
  for (int i = 0; i < LIST_MAX; i++)
  {
    if (keyHoldInfo[i].keyCode == 0)
    {
      keyHoldInfo[i].keyCode = keyCode;
      keyHoldInfo[i].holdTimestamp = millis() + autoRepeatInterval;
      keyHoldInfo[i].autoRepeatCount = 0;
      break;
    }
  }
}

// initialize the key hold timestamp table
void initKeyHoldInfo()
{
  for (int i = 0; i < LIST_MAX; i++)
  {
    keyHoldInfo[i].keyCode = 0;
    keyHoldInfo[i].holdTimestamp = 0;
    keyHoldInfo[i].autoRepeatCount = 0;
  }
}

// read a 2 byte unsigned integer from the wire
uint16_t readUInt()
{
  uint16_t result = 0;
  uint8_t loByte;
  uint8_t hiByte;

  hiByte = Wire.read();
  loByte = Wire.read();

  result = hiByte << 8 | loByte;
  return (result);
}
