#include "WebSocket.h"

WebSocket::WebSocket(uint16_t port, std::string_view uri):
  m_ws(uri.data()), m_server(port) {
  m_ws.onEvent(
    [this](
      AsyncWebSocket*       server,
      AsyncWebSocketClient* client,
      AwsEventType          type,
      void*                 arg,
      uint8_t*              data,
      size_t len) { this->onEvent(server, client, type, arg, data, len); });
  m_ws.enable(true);
  m_server.addHandler(&m_ws);
  begin();
}

WebSocket::~WebSocket() {
  end();
}

void WebSocket::begin() {
  m_server.begin();
}

void WebSocket::end() {
  m_ws.closeAll();
  m_server.end();
}

void WebSocket::onEvent(
  AsyncWebSocket*       server,
  AsyncWebSocketClient* client,
  AwsEventType          type,
  void*                 arg,
  uint8_t*              data,
  size_t                len) {
  switch (type)
  {
  case WS_EVT_CONNECT:
    onConnect(client);
    break;
  case WS_EVT_DISCONNECT:
    onDisconnect(client);
    break;
  case WS_EVT_DATA:
    onData(client, data, len);
    break;
  case WS_EVT_ERROR:
    onError(client);
    break;
  case WS_EVT_PONG:
    onPing(client);
    break;
  default:
    Logger::warn("Unknown WebSocket event type");
    break;
  }
}

void WebSocket::onConnect(AsyncWebSocketClient* client) {
  Logger::info(String("(") + client->id() + ") WS Connected!");
}

void WebSocket::onDisconnect(AsyncWebSocketClient* client) {
  Logger::info(String("(") + client->id() + ") WS Disconnected!");
}

void WebSocket::onData(
  AsyncWebSocketClient* client, uint8_t* data, size_t len) {
  Logger::debug(
    String("(") + client->id() +
    ") WS Data: " + String(reinterpret_cast< char* >(data), len));
}

void WebSocket::onError(AsyncWebSocketClient* client) {
  Logger::error(String("(") + client->id() + ") WS Error!");
}

void WebSocket::onPing(AsyncWebSocketClient* client) {
  Logger::debug(String("(") + client->id() + ") WS Ping!");
}
