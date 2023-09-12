#include <Arduino.h>
#include "Camera.h"
#include "Global.h"
#include "HTTPServer.h"
#include "Motors.h"
#include "WebSocket.h"

void setup() {
  GLOBAL::setup(); // Serial, SPIFFS, WiFi
}

void loop() {
  using namespace GLOBAL::SETTINGS;

  static WebSocket        wsServer(WS_PORT, WS_URI);
  static Camera           camera;
  static const Motors     motors(&wsServer, &camera);
  static const HTTPServer httpServer(HTTP_PORT);
}
