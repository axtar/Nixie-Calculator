// WebHelper.hpp

// converts settings to/from the JSON representation used by the web configuration API

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
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
  float externalTemperature;
  String temperatureUnit;
  unsigned int uptimeSeconds;
  int presenceSeconds;
  String deviceMode;
  String displayType;
  bool highVoltageOn;
  unsigned long hvOnSeconds;
  int wifiClients;
  String apIP;
  unsigned long currentTime;
  String resetReason;
  String chipModel;
  int chipRevision;
  uint32_t flashSize;
  long lastGpsSync;
};

class WebHelper
{
public:
  // serialize all non-hidden settings as a flat JSON object, used by the web configuration API
  static String settingsToJSON(const Settings &settings)
  {
    String json = "{";
    bool first = true;
    for (const auto &entry : settings.getSettingsMap())
    {
      if (entry.second->isHidden())
      {
        continue;
      }
      if (!first)
      {
        json += ",";
      }
      first = false;
      json += "\"" + entry.second->getName() + "\":" + String(entry.second->get());
    }
    json += "}";
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
    char buffer[160];
    sprintf(buffer,
            "{\"controllerVersion\":\"%u.%u.%u\",\"keyboardVersion\":\"%u.%u.%u\","
            "\"boardTemperature\":%.2f,\"externalTemperature\":%.2f",
            info.controllerMajor, info.controllerMinor, info.controllerRevision,
            info.keyboardMajor, info.keyboardMinor, info.keyboardRevision,
            info.boardTemperature, info.externalTemperature);
    String json = String(buffer);
    json += ",\"temperatureUnit\":\"" + info.temperatureUnit + "\"";
    json += ",\"freeHeap\":" + String(esp_get_free_heap_size());
    json += ",\"minFreeHeap\":" + String(esp_get_minimum_free_heap_size());
    json += ",\"uptime\":" + String(info.uptimeSeconds);
    json += ",\"lastPresence\":" + String(info.presenceSeconds);
    json += ",\"deviceMode\":\"" + info.deviceMode + "\"";
    json += ",\"displayType\":\"" + info.displayType + "\"";
    json += ",\"highVoltageOn\":" + String(info.highVoltageOn ? "true" : "false");
    json += ",\"hvOnSeconds\":" + String(info.hvOnSeconds);
    json += ",\"wifiClients\":" + String(info.wifiClients);
    json += ",\"apIP\":\"" + info.apIP + "\"";
    json += ",\"currentTime\":" + String(info.currentTime);
    json += ",\"resetReason\":\"" + info.resetReason + "\"";
    json += ",\"chipModel\":\"" + info.chipModel + "\"";
    json += ",\"chipRevision\":" + String(info.chipRevision);
    json += ",\"flashSize\":" + String(info.flashSize);
    json += ",\"lastGpsSync\":" + String(info.lastGpsSync);
    json += "}";
    return (json);
  }
};
