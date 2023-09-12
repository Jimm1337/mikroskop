#ifndef MIKROSKOP_WEBSOCKET_H
#define MIKROSKOP_WEBSOCKET_H

#include <AsyncWebSocket.h>
#include <ESPAsyncWebServer.h>
#include <functional>
#include <unordered_map>
#include "Global.h"
#include "Logger.h"
#include "TrackedTask.h"

class WebSocket {
  using ResponseData = String;
  using Handler = std::function< ResponseData(AsyncWebSocketClient*, String) >;

  AsyncWebSocket                                  m_ws;
  AsyncWebServer                                  m_server;
  std::unordered_map< char, Handler >             m_handlers;
  std::unordered_map< AwsEventType, TrackedTask > m_tasks;

public:
  WebSocket(uint16_t port, std::string_view uri);
  ~WebSocket();

  WebSocket(const WebSocket&)            = delete;
  WebSocket(WebSocket&&)                 = delete;
  WebSocket& operator=(const WebSocket&) = delete;
  WebSocket& operator=(WebSocket&&)      = delete;

  void begin();
  void end();
  void sendReady();
  void sendNotReady();
  static void sendLostControl(AsyncWebSocketClient* client);

  template< typename Fn >
  void attachListener(char startingLetter, Fn&& handler) {
    m_handlers.emplace(startingLetter, std::forward< Fn >(handler));
  }

private:
  void        onConnect(AsyncWebSocketClient* client);
  void        onDisconnect(AsyncWebSocketClient* client);
  void        onData(AsyncWebSocketClient* client, const String& data);
  static void onError(AsyncWebSocketClient* client, const String& msg);
  static void onPing(AsyncWebSocketClient* client);

  void onEvent(
    AsyncWebSocket*       server,
    AsyncWebSocketClient* client,
    AwsEventType          type,
    void*                 arg,
    uint8_t*              data,
    size_t                len);
};

#endif // MIKROSKOP_WEBSOCKET_H
