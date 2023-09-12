#include "WebSocket.h"
#include <thread>

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
  AsyncWebSocket* /*server*/,
  AsyncWebSocketClient* client,
  AwsEventType          type,
  void* /*arg*/,
  uint8_t* data,
  size_t   len) {
  const String msg = String(data, len);

  switch (type)
  {
  case WS_EVT_CONNECT:
    m_tasks[WS_EVT_CONNECT] =
      TrackedTask(false, [this, client]() { onConnect(client); });
    break;
  case WS_EVT_DISCONNECT:
    m_tasks[WS_EVT_DISCONNECT] =
      TrackedTask(false, [this, client]() { onDisconnect(client); });
    break;
  case WS_EVT_DATA:
    m_tasks[WS_EVT_DATA] =
      TrackedTask(true, [this, client, msg]() { onData(client, msg); });
    break;
  case WS_EVT_ERROR:
    m_tasks[WS_EVT_ERROR] =
      TrackedTask(true, [client, msg]() { onError(client, msg); });
    break;
  case WS_EVT_PONG:
    m_tasks[WS_EVT_PONG] = TrackedTask(true, [client]() { onPing(client); });
    break;
  default:
    Logger::warn(String("Unknown WebSocket event: ") + type);
    break;
  }
}

void WebSocket::onConnect(AsyncWebSocketClient* client) {
  Logger::info(String("(") + client->id() + ") WS Connected!");

  if (m_tasks[WS_EVT_DISCONNECT].isRunning())
  { m_tasks[WS_EVT_DISCONNECT].safeJoin(); } // Finish disconnect first

  if (m_handlers.find(GLOBAL::HANDLER::CONNECT) != m_handlers.end())
  {
    m_handlers[GLOBAL::HANDLER::CONNECT](client, "");
  } else {
    Logger::warn("No handler for Connect");
  }

  if (m_handlers.find(GLOBAL::HANDLER::INFO) != m_handlers.end())
  {
    const auto dataResponse = m_handlers[GLOBAL::HANDLER::INFO](client, "");
    client->text(dataResponse);
  } else {
    Logger::warn("No handler for Info");
  }
}

void WebSocket::onDisconnect(AsyncWebSocketClient* client) {
  Logger::info(String("(") + client->id() + ") WS Disconnected!");

  if (m_handlers.find(GLOBAL::HANDLER::DISCONNECT) != m_handlers.end())
  {
    m_handlers[GLOBAL::HANDLER::DISCONNECT](client, "");
  } else {
    Logger::warn("No handler for Disconnect");
  }
}

void WebSocket::onData(AsyncWebSocketClient* client, const String& data) {
  if (data.isEmpty())
  {
    Logger::warn("Empty WebSocket message");
    return;
  }

  Logger::info(String("(") + client->id() + ") WS Data: " + data);

  if (m_handlers.find(data[0]) != m_handlers.end())
  {
    const auto dataResponse = m_handlers[data[0]](client, data);
    if (!dataResponse.isEmpty()) { m_ws.textAll(dataResponse); }
  } else {
    Logger::warn(String("No handler for: ") + data[0]);
  }
}

void WebSocket::onError(AsyncWebSocketClient* client, const String& msg) {
  Logger::error(String("(") + client->id() + ") WS Error!");
  Logger::error(msg);
}

void WebSocket::onPing(AsyncWebSocketClient* client) {
  Logger::debug(String("(") + client->id() + ") WS Ping!");
}

void WebSocket::sendReady() {
  m_ws.textAll(String(static_cast< char >(GLOBAL::MSG_TYPE::READY)));
}

void WebSocket::sendNotReady() {
  m_ws.textAll(String(static_cast< char >(GLOBAL::MSG_TYPE::NOT_READY)));
}

void WebSocket::sendLostControl(AsyncWebSocketClient* client) {
  client->text(String(static_cast< char >(GLOBAL::MSG_TYPE::LOST_CONTROL)));
}
