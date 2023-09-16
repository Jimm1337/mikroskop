#include "Global.h"
#include <SPIFFS.h>
#include "Logger.h"
#include <WiFi.h>

namespace GLOBAL {

void setup() {
  using namespace SETTINGS;

  Serial.begin(115200);

  Logger::info("Starting up...");
  Logger::info("Mounting SPIFFS");
  if (!SPIFFS.begin())
  {
    Logger::critical("Failed to mount SPIFFS");
    ESP.restart();
  }
  Logger::info("SPIFFS mounted");

  WiFi.softAP(SSID, PASS);
  Logger::info("AP running");
  Logger::info("ESP IP: " + WiFi.softAPIP().toString());
}

} // namespace GLOBAL