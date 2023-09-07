#include "Logger.h"

namespace Logger {

void info(String msg) {
  Serial.print("[i] ");
  Serial.println(msg);
}

void debug(String msg) {
#ifdef LOG_DEBUG
  Serial.print("[DEBUG] ");
  Serial.println(msg);
#endif
}

void warn(String msg) {
  Serial.print("[W] ");
  Serial.println(msg);
}

void error(String msg) {
  Serial.print("[!] ");
  Serial.println(msg);
}

void critical(String msg) {
  Serial.print("[!!!] ");
  Serial.println(msg);
}

} // namespace Logger