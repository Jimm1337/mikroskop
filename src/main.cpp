#include <Arduino.h>
#include <WiFi.h>
#include "Global.h"
#include "Logger.h"
#include "HTTPServer.h"
#include "WebSocket.h"

static constexpr auto     SSID      = "Mikroskop";
static constexpr auto     PASS      = "pn_mikroskop";
static constexpr uint16_t HTTP_PORT = 80;
static constexpr uint16_t WS_PORT   = 1337;

void setup() {
  Global::setup();

  WiFi.softAP(SSID, PASS);
  Logger::info("AP running");
  Logger::info("ESP IP: " + WiFi.softAPIP().toString());
}

void loop() {
  static const WebSocket  wsServer(WS_PORT, "/");
  static const HTTPServer httpServer(HTTP_PORT);
}