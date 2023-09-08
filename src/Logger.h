#ifndef MIKROSKOP_LOGGER_H
#define MIKROSKOP_LOGGER_H

#include <Arduino.h>

#define LOG_DEBUG

namespace Logger {

  void info(String msg);
  void debug(String msg);
  void warn(String msg);
  void error(String msg);
  void critical(String msg);

}

#endif // MIKROSKOP_LOGGER_H
