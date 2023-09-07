#include "Global.h"
#include <SPIFFS.h>
#include "Logger.h"

namespace Global {

void setup() {
  Serial.begin(115200);

  Logger::info("Starting up...");
  Logger::info("Mounting SPIFFS");
  if (!SPIFFS.begin())
  {
    Logger::critical("Failed to mount SPIFFS");
    ESP.restart();
  }
  Logger::info("SPIFFS mounted");
}

} // namespace Global