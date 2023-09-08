#include "Logger.h"

namespace Logger {

void info(String msg) {
  Serial.print("[i] ");
  Serial.println(msg);
}

#ifdef LOG_DEBUG
void debug(String msg) {

  Serial.print("[DEBUG] ");
  Serial.println(msg);
}
#else
void debug(String msg) {}
#endif

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