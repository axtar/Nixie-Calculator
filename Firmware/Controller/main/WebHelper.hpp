// WebHelper.hpp

// converts settings to/from the JSON representation used by the web configuration API

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <esp_heap_caps.h>
#include <time.h>
#include <Settings.hpp>

// values needed to build the status page JSON, gathered by the caller
struct StatusInfo
{
  unsigned int controllerMajor;
  unsigned int controllerMinor;
  unsigned int controllerRevision;
  unsigned int keyboardMajor;
  unsigned int keyboardMinor;
  unsigned int keyboardRevision;
  float boardTemperature;
  float mcuTemperature;
  float externalTemperature;
  String temperatureUnit;
  unsigned int uptimeSeconds;
  int presenceSeconds;
  String deviceMode;
  String calculatorMode;
  String displayType;
  bool highVoltageOn;
  unsigned long hvOnSeconds;
  int wifiClients;
  String apIP;
  unsigned long currentTime;
  String resetReason;
  String chipModel;
  int chipRevision;
  unsigned int cpuFreqMHz;
  uint32_t flashSize;
  long lastGpsSync;
};

class WebHelper
{
public:
  // serialize all non-hidden settings as a flat JSON object, used by the web configuration API
  static String settingsToJSON(const Settings &settings)
  {
    const auto &map = settings.getSettingsMap();
    String json;
    json.reserve(map.size() * 24 + 2);
    json.concat('{');
    bool first = true;
    for (const auto &entry : map)
    {
      if (entry.second->isHidden())
      {
        continue;
      }
      if (!first)
      {
        json.concat(',');
      }
      first = false;
      json.concat('"');
      json.concat(entry.second->getName());
      json.concat("\":");
      json.concat(entry.second->get());
    }
    json.concat('}');
    return (json);
  }

  // apply a flat JSON object of setting name/value pairs, used by the web configuration API
  static void settingsFromJSON(Settings &settings, const String &json)
  {
    int len = json.length();
    int i = 0;
    while (i < len)
    {
      int keyStart = json.indexOf('"', i);
      if (keyStart < 0)
      {
        break;
      }
      int keyEnd = json.indexOf('"', keyStart + 1);
      if (keyEnd < 0)
      {
        break;
      }
      String key = json.substring(keyStart + 1, keyEnd);
      int colon = json.indexOf(':', keyEnd);
      if (colon < 0)
      {
        break;
      }
      int valStart = colon + 1;
      while ((valStart < len) && (json[valStart] == ' '))
      {
        valStart++;
      }
      int valEnd = valStart;
      if ((valEnd < len) && (json[valEnd] == '-'))
      {
        valEnd++;
      }
      while ((valEnd < len) && isDigit(json[valEnd]))
      {
        valEnd++;
      }
      if (valEnd > valStart)
      {
        settings.setSettingByName(key, json.substring(valStart, valEnd).toInt());
      }
      i = valEnd;
    }
  }

  // extract a string field's value from a flat JSON object
  static String jsonGetString(const String &json, const String &key)
  {
    String pattern = "\"" + key + "\"";
    int keyPos = json.indexOf(pattern);
    if (keyPos < 0)
    {
      return (String());
    }
    int colon = json.indexOf(':', keyPos + pattern.length());
    if (colon < 0)
    {
      return (String());
    }
    int valStart = json.indexOf('"', colon);
    if (valStart < 0)
    {
      return (String());
    }
    valStart++;
    int len = json.length();
    int valEnd = valStart;
    String result;
    while ((valEnd < len) && (json[valEnd] != '"'))
    {
      if ((json[valEnd] == '\\') && (valEnd + 1 < len))
      {
        valEnd++;
      }
      result += json[valEnd];
      valEnd++;
    }
    return (result);
  }

  // extract a numeric field's value from a flat JSON object
  static time_t jsonGetLong(const String &json, const String &key)
  {
    String pattern = "\"" + key + "\"";
    int keyPos = json.indexOf(pattern);
    if (keyPos < 0)
    {
      return (0);
    }
    int colon = json.indexOf(':', keyPos + pattern.length());
    if (colon < 0)
    {
      return (0);
    }
    int len = json.length();
    int valStart = colon + 1;
    while ((valStart < len) && (json[valStart] == ' '))
    {
      valStart++;
    }
    int valEnd = valStart;
    if ((valEnd < len) && (json[valEnd] == '-'))
    {
      valEnd++;
    }
    while ((valEnd < len) && isDigit(json[valEnd]))
    {
      valEnd++;
    }
    if (valEnd == valStart)
    {
      return (0);
    }
    return (json.substring(valStart, valEnd).toInt());
  }

  // build the status page JSON, called from the async web server task
  static String buildStatusJSON(const StatusInfo &info)
  {
    char buffer[200];
    sprintf(buffer,
            "{\"controllerVersion\":\"%u.%u.%u\",\"keyboardVersion\":\"%u.%u.%u\","
            "\"boardTemperature\":%.2f,\"externalTemperature\":%.2f,\"mcuTemperature\":%.2f",
            info.controllerMajor, info.controllerMinor, info.controllerRevision,
            info.keyboardMajor, info.keyboardMinor, info.keyboardRevision,
            info.boardTemperature, info.externalTemperature, info.mcuTemperature);

    String json;
    json.reserve(strlen(buffer) + 400 + info.temperatureUnit.length() + info.deviceMode.length() +
                 info.calculatorMode.length() + info.displayType.length() + info.apIP.length() +
                 info.resetReason.length() + info.chipModel.length());
    json.concat(buffer);
    json.concat(",\"temperatureUnit\":\"");
    json.concat(info.temperatureUnit);
    uint32_t freeHeap = esp_get_free_heap_size();
    uint32_t largestFreeBlock = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
    unsigned int heapFragmentation = freeHeap > 0 ? (100 - (largestFreeBlock * 100 / freeHeap)) : 0;
    json.concat("\",\"freeHeap\":");
    json.concat(freeHeap);
    json.concat(",\"minFreeHeap\":");
    json.concat(esp_get_minimum_free_heap_size());
    json.concat(",\"largestFreeBlock\":");
    json.concat(largestFreeBlock);
    json.concat(",\"heapFragmentation\":");
    json.concat(heapFragmentation);
    json.concat(",\"uptime\":");
    json.concat(info.uptimeSeconds);
    json.concat(",\"lastPresence\":");
    json.concat(info.presenceSeconds);
    json.concat(",\"deviceMode\":\"");
    json.concat(info.deviceMode);
    json.concat("\",\"calculatorMode\":\"");
    json.concat(info.calculatorMode);
    json.concat("\",\"displayType\":\"");
    json.concat(info.displayType);
    json.concat("\",\"highVoltageOn\":");
    json.concat(info.highVoltageOn ? "true" : "false");
    json.concat(",\"hvOnSeconds\":");
    json.concat(info.hvOnSeconds);
    json.concat(",\"wifiClients\":");
    json.concat(info.wifiClients);
    json.concat(",\"apIP\":\"");
    json.concat(info.apIP);
    json.concat("\",\"currentTime\":");
    json.concat(info.currentTime);
    json.concat(",\"resetReason\":\"");
    json.concat(info.resetReason);
    json.concat("\",\"chipModel\":\"");
    json.concat(info.chipModel);
    json.concat("\",\"chipRevision\":");
    json.concat(info.chipRevision);
    json.concat(",\"cpuFreqMHz\":");
    json.concat(info.cpuFreqMHz);
    json.concat(",\"flashSize\":");
    json.concat(info.flashSize);
    json.concat(",\"lastGpsSync\":");
    json.concat(info.lastGpsSync);
    json.concat('}');
    return (json);
  }
};
