#include <Arduino.h>
#include "Global.h"
#include "HTTPServer.h"
#include "Motors.h"
#include "WebSocket.h"

void setup() {
  GLOBAL::setup(); // Serial, SPIFFS, WiFi
}

void loop() {
  using namespace GLOBAL::SETTINGS;

  static WebSocket        wsServer(WS_PORT, "/");
  static const Motors     motors(&wsServer);
  static const HTTPServer httpServer(HTTP_PORT);
}
