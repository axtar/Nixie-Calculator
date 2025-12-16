// CalcWebSocketServer.hpp

// provides the calculator result via websocket

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <functional>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DebugDefs.h>
#include <IndexHTML.h>
#include <Config.h>

constexpr auto SERVER_PORT = 80;

class CalcWebSocketServer
{
protected:
  using connectionCallback = std::function<void(uint32_t id)>;
  using disconnectionCallback = std::function<void(uint32_t id)>;

public:
  CalcWebSocketServer(uint8_t netActPin) : _netActPin(netActPin),
                                           _server(SERVER_PORT),
                                           _ws(nullptr),
                                           _notifyConnection(nullptr),
                                           _notifyDisconnection(nullptr)
  {
    _initialized = false;
  }

  virtual ~CalcWebSocketServer()
  {
    if (_ws)
    {
      delete (_ws);
    }
  }

  // initialize and start AP and server
  void begin()
  {
    if (!_initialized)
    {
      // turn net activity LED on
      digitalWrite(_netActPin, HIGH);

      // start access point
      WiFi.softAP(AP_SSID, AP_PWD);
      _ip = WiFi.softAPIP();

      // initalize WebSocket and server only once
      if (!_ws)
      {
        // init WebSocket
        _ws = new AsyncWebSocket("/ws");
        _ws->onEvent(std::bind(&CalcWebSocketServer::onEvent, this,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               std::placeholders::_3,
                               std::placeholders::_4,
                               std::placeholders::_5,
                               std::placeholders::_6));

        // configure server
        _server.addHandler(_ws);
        _server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                   { request->send(200, "text/html", indexHTML); });
      }
      // Start server
      _server.begin();
      _initialized = true;
    }
  }

  // stop server and AP
  void end()
  {
    if (_initialized)
    {
      // close all but we keep the WebSocket
      _ws->closeAll();
      _ws->cleanupClients();

      // stop server
      _server.end();

      // stop access point
      WiFi.softAPdisconnect(true);

      // turn net activity LED off
      digitalWrite(_netActPin, LOW);
      _initialized = false;
    }
  }

  // set the callback function for client connection event
  void attachConnectionCb(connectionCallback callBack)
  {
    _notifyConnection = callBack;
  }

  // remove callback
  void detachConnectionCb()
  {
    _notifyConnection = nullptr;
  }

  // set the callback function for client connection event
  void attachDisconnectionCb(connectionCallback callBack)
  {
    _notifyDisconnection = callBack;
  }

  // remove callback
  void detachDisconnectionCb()
  {
    _notifyDisconnection = nullptr;
  }

  // clean up clients if needed
  void process()
  {
    if (_ws)
    {
      _ws->cleanupClients();
    }
  }

  // called on client connection
  void onClientConnect(uint32_t id)
  {
    _notifyConnection(id);
  }

  // called on client disconnection
  void onClientDisconnect(uint32_t id)
  {
    _notifyDisconnection(id);
  }

  // return IP addres of AP
  IPAddress getIP()
  {
    return (_ip);
  }

  // return client cout of WebSocket
  size_t getClientCount()
  {
    if (_ws)
    {
      return (_ws->count());
    }
    return (0);
  }

  // return if server is initialized
  bool isInitialized() const
  {
    return (_initialized);
  }

  // send message to all clients
  void updateClients(const String &regId, const String &value)
  {
    if (_ws)
    {
      if (_ws->count() > 0)
      {
        _ws->textAll(regId + value);
      }
    }
  }

  // set message to single client
  void updateClient(const String &regId, const String &value, uint32_t id)
  {
    if (_ws)
    {
      if (_ws->hasClient(id))
      {
        _ws->text(id, regId + value);
      }
    }
  }

private:
  uint8_t _netActPin;
  AsyncWebServer _server;
  AsyncWebSocket *_ws;
  IPAddress _ip;
  connectionCallback _notifyConnection;
  disconnectionCallback _notifyDisconnection;
  bool _initialized;

  // WebSocket event handler
  void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
               void *arg, uint8_t *data, size_t len)
  {
    switch (type)
    {
    case WS_EVT_CONNECT:
      D_printf("WebSocket client #%u connected\n", static_cast<unsigned int>(client->id())); //, s.c_str());
      onClientConnect(client->id());
      break;

    case WS_EVT_DISCONNECT:
      D_printf("WebSocket client #%u disconnected\n", static_cast<unsigned int>(client->id()));
      onClientDisconnect(client->id());
      break;

    case WS_EVT_PING:
    case WS_EVT_DATA:
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
    }
  }
};