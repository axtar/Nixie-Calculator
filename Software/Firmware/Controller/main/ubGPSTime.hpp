// ubGPSTime.hpp

// get utc time from u-blox gps module

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <functional>
#include <DebugDefs.h>

constexpr uint8_t INIT_STEPS = 1;                 // only one init step for now
constexpr unsigned long WAIT_FOR_RESPONSE = 5000; // 5 seconds
constexpr unsigned long WAIT_FOR_RESET = 5000;    // 5 seconds

constexpr auto MAX_PAYLOAD = 512;
constexpr uint8_t MAX_EXTENSIONS = 4;
constexpr uint8_t EXTENSION_LEN = 30;

// UBX headers
constexpr uint8_t UBX_HEADER1 = 0xB5;
constexpr uint8_t UBX_HEADER2 = 0x62;

// UBX classes
constexpr uint8_t UBX_NAV = 0x01;
constexpr uint8_t UBX_ACK = 0x05;
constexpr uint8_t UBX_CFG = 0x06;
constexpr uint8_t UBX_MON = 0x0A;
constexpr uint8_t UBX_NMEA = 0xF0;

// UBX message IDs
// UBX config
constexpr uint8_t UBX_CFG_MSG = 0x01;
constexpr uint8_t UBX_CFG_RST = 0x04;

// UBX NMEA messages sent by default
constexpr uint8_t UBX_NMEA_GGA = 0x00;
constexpr uint8_t UBX_NMEA_GLL = 0x01;
constexpr uint8_t UBX_NMEA_GSA = 0x02;
constexpr uint8_t UBX_NMEA_GSV = 0x03;
constexpr uint8_t UBX_NMEA_RMC = 0x04;
constexpr uint8_t UBX_NMEA_VTG = 0x05;

// UBX MON
constexpr uint8_t UBX_MON_VER = 0x04;

// UBX NAV
constexpr uint8_t UBX_NAV_STATUS = 0x03;
constexpr uint8_t UBX_NAV_TIMEUTC = 0x21;

// ACK/NACK
constexpr uint8_t UBX_ACK_NACK = 0x00;
constexpr uint8_t UBX_ACK_ACK = 0x01;

// UBX message
typedef struct
{
  uint8_t header1;
  uint8_t header2;
  uint8_t msgClass;
  uint8_t msgID;
  uint16_t payloadLength;
  uint8_t *payload;
  uint8_t CK_A;
  uint8_t CK_B;
} UBXMESSAGE;

// date/time information
typedef struct
{
  uint32_t timeOfWeek;
  uint32_t accuracy;
  int32_t nanoSecond;
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  bool utcValid;
  bool timeOfWeekValid;
  bool weekNumberValid;
  uint32_t timestamp;
} TIMEUTC;

// GPS status information
typedef struct
{
  uint32_t timeOfWeek;
  uint8_t gpsFixType;
  bool gpsFixOk;
  bool diffApplied;
  bool timeOfWeekValid;
  bool weekNumberValid;
  uint32_t timestamp;
} GPSSTATUS;

// GPS module information
typedef struct
{
  String swVersion;
  String hwVersion;
  String extensions[MAX_EXTENSIONS];
} MODULEVERSION;

// checksums
typedef struct
{
  uint8_t CK_A;
  uint8_t CK_B;
} CHECKSUM;

// enums
enum class direction
{
  incoming,
  outgoing
};

enum class pending
{
  none,
  version,
  ack
};

class ubGPSTime
{

protected:
  using notifyCallBack = std::function<void(const UBXMESSAGE *message)>;

public:
  // constructor
  ubGPSTime() : _serialPort(nullptr),
                _verbose(false), _initialized(false),
                _pending(pending::none), _notify(nullptr),
                _timeUTC({}), _gpsStatus({}),
                _message({}), _fieldCounter(0),
                _payloadCounter(0),
                _disabledNMEA(false)
  {
  }

  // set a callback function for message notification
  void attach(notifyCallBack callBack)
  {
    _notify = callBack;
  }

  // stop getting message notifications
  void detach()
  {
    _notify = nullptr;
  }

  // define serial com port to GPS module
  void begin(Stream &serialPort)
  {
    _serialPort = &serialPort;
  }

  // stop
  void end()
  {
    _serialPort = nullptr;
    _initialized = false;
  }

  // asking about GPS module information
  // if we get a response, we assume that we are talking to a u-blox module
  void initialize(bool wait)
  {
    _initialized = false;

    _serialPort->flush();
    requestVersion();
    if (wait)
    {
      _pending = pending::version;
      if (waitForResponse(WAIT_FOR_RESPONSE))
      {
        _initialized = true;

        // bye bye NMEA spam!!!
        disableDefaultNMEA();
      }
    }
  }

  // enable debug information
  void enableVerbose()
  {
    _verbose = true;
  }

  // disable debug information
  void disableVerbose()
  {
    _verbose = false;
  }

  // read from serial port
  void process()
  {
    char c = 0;

    if (_serialPort)
    {
      while (_serialPort->available())
      {
        c = _serialPort->read();
        switch (_fieldCounter)
        {
        case 0: // header 1
          if (c == UBX_HEADER1)
          {
            _message.header1 = c;
            _fieldCounter++;
            // free memory if we missed a delete
            if (_message.payload)
            {
              delete[] _message.payload;
              _message.payload = nullptr;
            }
          }
          break;

        case 1: // header 2
          if (c == UBX_HEADER2)
          {
            _message.header2 = c;
            _fieldCounter++;
          }
          else
          {
            _fieldCounter = 0;
          }
          break;

        case 2: // class
          _message.msgClass = c;
          _fieldCounter++;
          break;

        case 3: // id
          _message.msgID = c;
          _fieldCounter++;
          break;

        case 4: // length (first of 2 bytes, little endian)
          _message.payloadLength = c;
          _fieldCounter++;
          break;

        case 5: // length (second of 2 bytes, little endian)
          _message.payloadLength |= c << 8;
          if (_message.payloadLength == 0)
          {
            // skip payload
            _fieldCounter += 2;
          }
          else if (_message.payloadLength > MAX_PAYLOAD)
          {
            // payload larger as max supported size
            // dismiss message and resync
            _fieldCounter = 0;
            _payloadCounter = 0;
          }
          else
          {
            // allocate memory
            _message.payload = new uint8_t[_message.payloadLength];
            _fieldCounter++;
          }
          break;

        case 6: // payload
          _message.payload[_payloadCounter] = c;
          _payloadCounter++;
          if (_payloadCounter == _message.payloadLength)
          {
            _payloadCounter = 0;
            _fieldCounter++;
          }
          break;

        case 7: // checksum A
          _message.CK_A = c;
          _fieldCounter++;
          break;

        case 8: // checksum B
          _message.CK_B = c;
          _fieldCounter = 0;
          _payloadCounter = 0;
          processMessage(&_message);
          // free memory after processing message
          if (_message.payload)
          {
            delete[] _message.payload;
            _message.payload = nullptr;
          }
          break;

        default:
          _fieldCounter = 0;
          _payloadCounter = 0;
          break;
        }
      }
    }
    else
    {
      if (_verbose)
      {
        D_println("Com port not defined. Call begin first");
      }
    }
  }

  // send a message to gps module
  void sendMessage(UBXMESSAGE *message) const
  {
    if (_serialPort)
    {
      CHECKSUM checksum = {};
      calculateChecksum(message, &checksum);

      message->CK_A = checksum.CK_A;
      message->CK_B = checksum.CK_B;

      if (_verbose)
      {
        printMessage(message, direction::outgoing);
      }

      _serialPort->write(message->header1);
      _serialPort->write(message->header2);
      _serialPort->write(message->msgClass);
      _serialPort->write(message->msgID);
      _serialPort->write(message->payloadLength & 0xFF);
      _serialPort->write(message->payloadLength >> 8);

      // Write payload.
      for (uint16_t i = 0; i < message->payloadLength; i++)
      {
        _serialPort->write(message->payload[i]);
      }

      // Write checksum
      _serialPort->write(message->CK_A);
      _serialPort->write(message->CK_B);
    }
    else
    {
      if (_verbose)
      {
        D_println("Com port not defined. Call begin first");
      }
    }
  }
  // disable default NMEA messages sent by GPS module
  void disableDefaultNMEA()
  {
    setMessageRate(UBX_NMEA, UBX_NMEA_GGA, 0);
    setMessageRate(UBX_NMEA, UBX_NMEA_GLL, 0);
    setMessageRate(UBX_NMEA, UBX_NMEA_GSA, 0);
    setMessageRate(UBX_NMEA, UBX_NMEA_GSV, 0);
    setMessageRate(UBX_NMEA, UBX_NMEA_RMC, 0);
    setMessageRate(UBX_NMEA, UBX_NMEA_VTG, 0);
  }

  // set update rate for messages in seconds, max 255,
  // use rate = 0 to stop the module from sending updates
  void setMessageRate(uint8_t msgClass, uint8_t msgID,
                      uint8_t rate, bool wait = true)
  {
    UBXMESSAGE message;
    uint8_t payLoad[3];

    message.payload = payLoad;
    message.header1 = UBX_HEADER1;
    message.header2 = UBX_HEADER2;
    message.msgClass = UBX_CFG;
    message.msgID = UBX_CFG_MSG;
    message.payloadLength = 3;
    message.payload[0] = msgClass;
    message.payload[1] = msgID;
    message.payload[2] = rate;
    sendMessage(&message);
    if (wait)
    {
      _pending = pending::ack;
      waitForResponse(WAIT_FOR_RESPONSE);
    }
  }

  // request a single message
  void pollMessage(uint8_t msgClass, uint8_t msgID) const
  {
    UBXMESSAGE message;

    message.header1 = UBX_HEADER1;
    message.header2 = UBX_HEADER2;
    message.msgClass = msgClass;
    message.msgID = msgID;
    message.payloadLength = 0;
    message.payload = 0;
    sendMessage(&message);
  }

  // request module version information
  void requestVersion() const
  {
    pollMessage(UBX_MON, UBX_MON_VER);
  }

  // request GPS status information
  void requestStatus() const
  {
    pollMessage(UBX_NAV, UBX_NAV_STATUS);
  }

  // request date/time information
  void requestTimeUTC() const
  {
    pollMessage(UBX_NAV, UBX_NAV_TIMEUTC);
  }

  // reset module
  void resetModule()
  {
    UBXMESSAGE message;
    uint8_t payLoad[4] = {0x00, 0x00, 0x01, 0x00};
    message.header1 = UBX_HEADER1;
    message.header2 = UBX_HEADER2;
    message.msgClass = UBX_CFG;
    message.msgID = UBX_CFG_RST;
    message.payloadLength = 4;
    message.payload = payLoad;
    sendMessage(&message);
    _initialized = false;
  }

  // subscribe to GPS status information
  void subscribeGPSStatus(uint8_t rate, bool wait = true)
  {
    setMessageRate(UBX_NAV, UBX_NAV_STATUS, rate, wait);
  }

  // subscribe to date/time information
  void subscribeTimeUTC(uint8_t rate, bool wait = true)
  {
    setMessageRate(UBX_NAV, UBX_NAV_TIMEUTC, rate, wait);
  }

  // provide access to the module version data
  MODULEVERSION getModuleVersion() const
  {
    return (_moduleVersion);
  }

  // provide access to last updated time data
  TIMEUTC getTimeUTC() const
  {
    return (_timeUTC);
  }

  // provide access to last updated GPS status
  GPSSTATUS getGPSStatus() const
  {
    return (_gpsStatus);
  }

  // return initialization status
  bool isInitialized() const
  {
    return (_initialized);
  }

private:
  Stream *_serialPort;
  bool _verbose;
  bool _initialized;
  pending _pending;
  notifyCallBack _notify;
  TIMEUTC _timeUTC;
  GPSSTATUS _gpsStatus;
  MODULEVERSION _moduleVersion;
  UBXMESSAGE _message;
  uint16_t _fieldCounter;
  uint16_t _payloadCounter;
  bool _disabledNMEA;

  // print a message on debug port
  void printMessage(const UBXMESSAGE *message, direction dir) const
  {
    if (_verbose)
    {
      switch (dir)
      {
      case direction::incoming:
        D_print("UBX Message <-- ");
        break;

      case direction::outgoing:
        D_print("UBX Message --> ");
        break;
      }
      printHEX(message->header1);
      printHEX(message->header2);
      printHEX(message->msgClass);
      printHEX(message->msgID);
      printHEX(message->payloadLength & 0xFF);
      printHEX(message->payloadLength >> 8);
      for (uint16_t i = 0; i < message->payloadLength; i++)
      {
        printHEX(message->payload[i]);
      }
      printHEX(message->CK_A);
      printHEX(message->CK_B);
      D_println();
    }
  }

  // print a byte in HEX format
  void printHEX(uint8_t value) const
  {
    if (value < 16)
    {
      D_print("0");
    }
    D_print(value, HEX);
    D_print(" ");
  }

  // process Ack messages
  void onAck(const UBXMESSAGE *message)
  {
    // not checking Ack for now
    _pending = pending::none;
    if (_verbose)
    {
      D_println("Received ack");
    }
  }

  // process Nack messages
  void onNack(const UBXMESSAGE *message)
  {
    // not checking Nack for now
    _pending = pending::none;
    if (_verbose)
    {
      D_println("Received nack");
    }
  }

  // process GPS status messages and update internal data structure
  void onStatus(const UBXMESSAGE *message)
  {
    _gpsStatus.timeOfWeek = getU4(message, 0);
    _gpsStatus.gpsFixType = getU1(message, 4);
    _gpsStatus.gpsFixOk = getFlag(message, 5, 0);
    _gpsStatus.diffApplied = getFlag(message, 5, 1);
    _gpsStatus.timeOfWeekValid = getFlag(message, 5, 2);
    _gpsStatus.weekNumberValid = getFlag(message, 5, 3);
    _gpsStatus.timestamp = millis();
    if (_verbose)
    {
      D_print("Time of week:        ");
      D_println(_gpsStatus.timeOfWeek);
      D_print("GPS fix type:        ");
      D_println(_gpsStatus.gpsFixType);
      D_print("GPS fix  OK:         ");
      D_println(_gpsStatus.gpsFixOk);
      D_print("Corrections applied: ");
      D_println(_gpsStatus.diffApplied);
      D_print("ToW valid:           ");
      D_println(_gpsStatus.timeOfWeekValid);
      D_print("Week number valid:   ");
      D_println(_gpsStatus.weekNumberValid);
    }
  }

  // process module version messages and update internal data structure
  void onVersion(const UBXMESSAGE *message)
  {
    // got a version message, assuming initialization succeeded
    _initialized = true;
    uint16_t offset = 0;
    _moduleVersion.swVersion = getString(message, offset, 30);
    offset += 30;
    _moduleVersion.hwVersion = getString(message, offset, 10);
    offset += 10;
    for (uint8_t i = 0; i < MAX_EXTENSIONS; i++)
    {
      if (message->payloadLength >= offset + EXTENSION_LEN)
      {
        _moduleVersion.extensions[i] = getString(message, offset, EXTENSION_LEN);
        offset += EXTENSION_LEN;
      }
      else
      {
        _moduleVersion.extensions[i] = "N/A";
      }
    }
    _pending = pending::none;
    if (_verbose)
    {
      D_print("Software version: ");
      D_println(_moduleVersion.swVersion);
      D_print("Hardware version: ");
      D_println(_moduleVersion.hwVersion);
      for (uint8_t i = 0; i < MAX_EXTENSIONS; i++)
      {
        D_printf("Extension %u: ", i + 1);
        D_println(_moduleVersion.extensions[i]);
      }
    }
  }

  // process date/time messages and update data structure
  void onTimeUTC(const UBXMESSAGE *message)
  {
    _timeUTC.timeOfWeek = getU4(message, 0);
    _timeUTC.accuracy = getU4(message, 4);
    _timeUTC.nanoSecond = getI4(message, 8);
    _timeUTC.year = getU2(message, 12);
    _timeUTC.month = getU1(message, 14);
    _timeUTC.day = getU1(message, 15);
    _timeUTC.hour = getU1(message, 16);
    _timeUTC.minute = getU1(message, 17);
    _timeUTC.second = getU1(message, 18);
    _timeUTC.timeOfWeekValid = (bool)getFlag(message, 19, 0);
    _timeUTC.weekNumberValid = (bool)getFlag(message, 19, 1);
    _timeUTC.utcValid = (bool)getFlag(message, 19, 2);
    _timeUTC.timestamp = millis();
    if (_verbose)
    {
      D_print("Time of week:       ");
      D_println(_timeUTC.timeOfWeek);
      D_print("accuracy:           ");
      D_println(_timeUTC.accuracy);
      D_print("Nanoseconds:        ");
      D_println(_timeUTC.nanoSecond);
      D_print("Year:               ");
      D_println(_timeUTC.year);
      D_print("Month:              ");
      D_println(_timeUTC.month);
      D_print("Day:                ");
      D_println(_timeUTC.day);
      D_print("Hour:               ");
      D_println(_timeUTC.hour);
      D_print("Minute:             ");
      D_println(_timeUTC.minute);
      D_print("Second:             ");
      D_println(_timeUTC.second);
      D_print("Time of week valid: ");
      D_println(_timeUTC.timeOfWeekValid);
      D_print("Week number valid:  ");
      D_println(_timeUTC.weekNumberValid);
      D_print("UTC valid:          ");
      D_println(_timeUTC.utcValid);
      D_print("Timestamp:          ");
      D_println(_timeUTC.timestamp);
    }
  }

  // process some incoming messages
  void processMessage(const UBXMESSAGE *message)
  {
    bool corrupted = false;
    if (_verbose)
    {
      printMessage(message, direction::incoming);
    }
    switch (message->msgClass)
    {
    case UBX_ACK:
      switch (message->msgID)
      {
      case UBX_ACK_ACK:
        onAck(message);
        break;

      case UBX_ACK_NACK:
        onNack(message);
        break;
      }
      break;

    case UBX_MON:
      switch (message->msgID)
      {
      case UBX_MON_VER:
        onVersion(message);
        break;
      }
      break;

    case UBX_NAV:
      switch (message->msgID)
      {
      case UBX_NAV_STATUS:
        onStatus(message);
        break;

      case UBX_NAV_TIMEUTC:
        // validate checksum for utc time messages
        if (validateChecksum(message))
        {
          onTimeUTC(message);
        }
        else
        {
          corrupted = true;
        }
        break;
      }
      break;
    }
    if (!corrupted)
    {
      onMessageEvent(message);
    }
  }

  // callback message notification
  void onMessageEvent(const UBXMESSAGE *message) const
  {
    if (_notify)
    {
      _notify(message);
    }
  }

  // wait while processing GPS serial data
  bool waitForResponse(uint32_t timeout)
  {
    uint32_t timestamp = millis();
    while (millis() - timestamp < timeout)
    {
      process();
      if (_pending == pending::none)
      {
        return (true);
      }
    }
    return (false);
  }

  // calculate the checksums for outgoing messages
  void calculateChecksum(const UBXMESSAGE *message, CHECKSUM *checksum) const
  {
    checksum->CK_A = 0;
    checksum->CK_B = 0;
    stepChecksum(message->msgClass, checksum);
    stepChecksum(message->msgID, checksum);
    stepChecksum(message->payloadLength & 0xFF, checksum);
    stepChecksum(message->payloadLength >> 8, checksum);
    for (uint16_t i = 0; i < message->payloadLength; i++)
    {
      stepChecksum(message->payload[i], checksum);
    }
  }

  // step in checksum calcualtion
  void stepChecksum(const uint8_t value, CHECKSUM *checksum) const
  {
    checksum->CK_A += value;
    checksum->CK_B += checksum->CK_A;
  }

  // validate checksum for some messages
  bool validateChecksum(const UBXMESSAGE *message) const
  {
    CHECKSUM checksum = {};
    calculateChecksum(message, &checksum);
    return ((message->CK_A == checksum.CK_A) && (message->CK_B == checksum.CK_B));
  }

  // field extraction functions
  // extract an U1 field from payload and return as uint8_t
  uint8_t getU1(const UBXMESSAGE *message, uint16_t offset) const
  {
    return (static_cast<uint8_t>(message->payload[offset]));
  }

  // extract an U2 field from payload and return as uint16_t
  uint16_t getU2(const UBXMESSAGE *message, uint16_t offset) const
  {
    uint16_t value = 0;
    value |= static_cast<uint16_t>(message->payload[offset]);
    value |= static_cast<uint16_t>(message->payload[offset + 1] << 8);
    return (value);
  }

  // extract an U4 field from payload and return as uint32_t
  uint32_t getU4(const UBXMESSAGE *message, uint16_t offset) const
  {
    uint32_t value = 0;
    value |= static_cast<uint32_t>(message->payload[offset]);
    value |= static_cast<uint32_t>(message->payload[offset + 1] << 8);
    value |= static_cast<uint32_t>(message->payload[offset + 2] << 16);
    value |= static_cast<uint32_t>(message->payload[offset + 3] << 24);
    return (value);
  }

  // extract an I4 field from payload and return as int32_t
  int32_t getI4(const UBXMESSAGE *message, uint16_t offset) const
  {
    return (static_cast<uint32_t>(getU4(message, offset)));
  }

  // extract a bit from payload and return as uint8_t
  uint8_t getFlag(const UBXMESSAGE *message, uint16_t offset, uint8_t bit) const
  {
    uint8_t flags = getU1(message, offset);
    return ((flags >> bit) & 0x01);
  }

  // extract and return a string from payload
  String getString(const UBXMESSAGE *message, uint16_t offset, uint16_t length) const
  {
    String s;
    for (uint16_t i = offset; i < offset + length; i++)
    {
      if (message->payload[i] != 0)
      {
        s += (char)message->payload[i];
      }
      else
      {
        break;
      }
    }
    return (s);
  }
};
