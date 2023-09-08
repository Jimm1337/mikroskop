#include "WebSocket.h"
#include <thread>

WebSocket::WebSocket(uint16_t port, std::string_view uri):
  m_ws(uri.data()),
  m_server(port),
  m_handlers() {
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

void WebSocket::sendPos(Global::PosTupleXYZ pos) {
  const auto [x, y, z] = pos;
  m_ws.textAll(
    String(Global::CHAR_INFO) + String(x) + ',' + String(y) + ',' + String(z));
}

void WebSocket::onEvent(
  AsyncWebSocket*       server,
  AsyncWebSocketClient* client,
  AwsEventType          type,
  void*                 arg,
  uint8_t*              data,
  size_t                len) {
  const String msg = String(data, len);

  switch (type)
  {
  case WS_EVT_CONNECT:
    std::thread([this, client] { this->onConnect(client); }).detach();
    break;
  case WS_EVT_DISCONNECT:
    std::thread([this, client] { this->onDisconnect(client); }).detach();
    break;
  case WS_EVT_DATA:
    std::thread([this, client, msg] { this->onData(client, msg); }).detach();
    break;
  case WS_EVT_ERROR:
    std::thread([this, client, msg] { this->onError(client, msg); }).detach();
    break;
  case WS_EVT_PONG:
    std::thread([this, client] { this->onPing(client); }).detach();
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

void WebSocket::onData(AsyncWebSocketClient* client, const String& data) {
  if (data.isEmpty())
  {
    Logger::warn("Empty WebSocket message");
    return;
  }

  if (data[0] != Global::CHAR_INFO)
  { Logger::info(String("(") + client->id() + ") WS Data: " + data); }

  switch (data[0])
  {
  case Global::CHAR_STOP:
    if (m_handlers.find(Global::CHAR_STOP) == m_handlers.end())
    {
      Logger::warn("No handler for stop");
      break;
    }
    m_handlers[Global::CHAR_STOP](client, data);
    break;
  case Global::CHAR_MOVE:
    if (m_handlers.find(Global::CHAR_MOVE) == m_handlers.end())
    {
      Logger::warn("No handler for move");
      break;
    }
    m_handlers[Global::CHAR_MOVE](client, data);
    break;
  case Global::CHAR_INFO:
    if (m_handlers.find(Global::CHAR_INFO) == m_handlers.end())
    {
      Logger::warn("No handler for info");
      break;
    }
    m_handlers[Global::CHAR_INFO](client, data);
    break;
  default:
    Logger::warn("Unknown WebSocket message");
    break;
  }
}

void WebSocket::onError(AsyncWebSocketClient* client, const String& msg) {
  Logger::error(String("(") + client->id() + ") WS Error!");
  Logger::error(msg);
}

void WebSocket::onPing(AsyncWebSocketClient* client) {
  Logger::debug(String("(") + client->id() + ") WS Ping!");
}
