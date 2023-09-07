#ifndef MIKROSKOP_WEBSOCKET_H
#define MIKROSKOP_WEBSOCKET_H

#include <AsyncWebSocket.h>
#include <ESPAsyncWebServer.h>
#include "Logger.h"

class WebSocket {
  AsyncWebSocket m_ws;
  AsyncWebServer m_server;

public:
  WebSocket(uint16_t port, std::string_view uri);
  ~WebSocket();

  WebSocket(const WebSocket&)            = delete;
  WebSocket(WebSocket&&)                 = delete;
  WebSocket& operator=(const WebSocket&) = delete;
  WebSocket& operator=(WebSocket&&)      = delete;

  void begin();
  void end();

private:
  void onConnect(AsyncWebSocketClient* client);
  void onDisconnect(AsyncWebSocketClient* client);
  void onData(AsyncWebSocketClient* client, uint8_t* data, size_t len);
  void onError(AsyncWebSocketClient* client);
  void onPing(AsyncWebSocketClient* client);

  void onEvent(
    AsyncWebSocket*       server,
    AsyncWebSocketClient* client,
    AwsEventType          type,
    void*                 arg,
    uint8_t*              data,
    size_t                len);
};

#endif // MIKROSKOP_WEBSOCKET_H
