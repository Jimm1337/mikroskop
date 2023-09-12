#ifndef MIKROSKOP_GLOBAL_H
#define MIKROSKOP_GLOBAL_H

#include <cstdint>
#include <tuple>

namespace GLOBAL {

namespace SETTINGS {
inline constexpr auto SSID      = "Mikroskop";
inline constexpr auto PASS      = "pn_mikroskop";
inline constexpr auto WS_PORT   = 1337;
inline constexpr auto WS_URI    = "/";
inline constexpr auto HTTP_PORT = 80;
} // namespace SETTINGS

void setup();

namespace MSG_TYPE {
enum CHAR : char {
  START              = 'S',
  STOP               = '#',
  CAL_START          = 'C',
  CAL_UL             = 'L',
  CAL_LR             = 'R',
  CAL_UP             = 'U',
  CAL_DOWN           = 'D',
  CAL_STEP           = 'T',
  CAL_RESET_UL       = 'l',
  CAL_RESET_LR       = 'r',
  CAL_RESET_U        = 'u',
  CAL_RESET_D        = 'd',
  CAL_RESET_STEP     = 't',
  READY              = 'Y', // SEND ONLY
  NOT_READY          = 'N', // SEND ONLY
  MANUAL_START_LEFT  = 'M',
  MANUAL_START_RIGHT = 'm',
  MANUAL_START_UPPER = 'X',
  MANUAL_START_LOWER = 'x',
  MANUAL_START_UP    = 'W',
  MANUAL_START_DOWN  = 'w',
  MANUAL_STOP_LEFT   = 'O',
  MANUAL_STOP_RIGHT  = 'o',
  MANUAL_STOP_UPPER  = 'Z',
  MANUAL_STOP_LOWER  = 'z',
  MANUAL_STOP_UP     = 'Q',
  MANUAL_STOP_DOWN   = 'q',
  MANUAL_CONFIRM     = 'K',
  TIME               = 'y',
  LOST_CONTROL       = 'L', // SEND ONLY
  INFO               = 'i', // SEND ONLY
};
} // namespace MSG_TYPE

namespace MSG_DATA {
enum CHAR : char {
  DATA_TIME_1_4000 = '1',
  DATA_TIME_1_2000 = '2',
  DATA_TIME_1_1000 = '3',
  DATA_TIME_1_500  = '4',
  DATA_TIME_1_250  = '5',
  DATA_TIME_1_125  = '6',
  DATA_TIME_1_60   = '7',
  DATA_TIME_1_30   = '8',
  DATA_TIME_1_15   = '9',
  DATA_TIME_1_8    = 'a',
  DATA_TIME_1_4    = 'b',
  DATA_TIME_1_2    = 'c',
  DATA_TIME_1      = 'd',
  DATA_TIME_2      = 'e',
  DATA_TIME_5      = 'f',
  DATA_TIME_10     = 'g',
};
} // namespace MSG_DATA

namespace HANDLER {
enum CHAR : char {
  CONNECT    = ',', // Not applicable
  DISCONNECT = '.', // Not applicable
};
} // namespace HANDLER

namespace TIME {
using Period = double;

enum CHAR : char {
  TIME_1_4000 = '0',
  TIME_1_2000 = '1',
  TIME_1_1000 = '2',
  TIME_1_500  = '3',
  TIME_1_250  = '4',
  TIME_1_125  = '5',
  TIME_1_60   = '6',
  TIME_1_30   = '7',
  TIME_1_15   = '8',
  TIME_1_8    = '9',
  TIME_1_4    = 'a',
  TIME_1_2    = 'b',
  TIME_1      = 'c',
  TIME_2      = 'd',
  TIME_5      = 'e',
  TIME_10     = 'f',
  EMPTY       = ' ',
};

namespace MS {
inline constexpr Period TIME_1_4000 = .25;
inline constexpr Period TIME_1_2000 = .5;
inline constexpr Period TIME_1_1000 = 1.;
inline constexpr Period TIME_1_500  = 2.;
inline constexpr Period TIME_1_250  = 4.;
inline constexpr Period TIME_1_125  = 8.;
inline constexpr Period TIME_1_60   = 16.67;
inline constexpr Period TIME_1_30   = 33.33;
inline constexpr Period TIME_1_15   = 66.67;
inline constexpr Period TIME_1_8    = 125.;
inline constexpr Period TIME_1_4    = 250.;
inline constexpr Period TIME_1_2    = 500.;
inline constexpr Period TIME_1      = 1000.;
inline constexpr Period TIME_2      = 2000.;
inline constexpr Period TIME_5      = 5000.;
inline constexpr Period TIME_10     = 10000.;
} // namespace MS
} // namespace TIME

struct Time {
  TIME::Period asMs;
  TIME::CHAR   asChar;

  explicit Time(TIME::CHAR asChar): asChar(asChar) {
    switch (asChar)
    {
    case TIME::CHAR::TIME_1_4000:
      asMs = TIME::MS::TIME_1_4000;
      break;
    case TIME::CHAR::TIME_1_2000:
      asMs = TIME::MS::TIME_1_2000;
      break;
    case TIME::CHAR::TIME_1_1000:
      asMs = TIME::MS::TIME_1_1000;
      break;
    case TIME::CHAR::TIME_1_500:
      asMs = TIME::MS::TIME_1_500;
      break;
    case TIME::CHAR::TIME_1_250:
      asMs = TIME::MS::TIME_1_250;
      break;
    case TIME::CHAR::TIME_1_125:
      asMs = TIME::MS::TIME_1_125;
      break;
    case TIME::CHAR::TIME_1_60:
      asMs = TIME::MS::TIME_1_60;
      break;
    case TIME::CHAR::TIME_1_30:
      asMs = TIME::MS::TIME_1_30;
      break;
    case TIME::CHAR::TIME_1_15:
      asMs = TIME::MS::TIME_1_15;
      break;
    case TIME::CHAR::TIME_1_8:
      asMs = TIME::MS::TIME_1_8;
      break;
    case TIME::CHAR::TIME_1_4:
      asMs = TIME::MS::TIME_1_4;
      break;
    case TIME::CHAR::TIME_1_2:
      asMs = TIME::MS::TIME_1_2;
      break;
    case TIME::CHAR::TIME_1:
      asMs = TIME::MS::TIME_1;
      break;
    case TIME::CHAR::TIME_2:
      asMs = TIME::MS::TIME_2;
      break;
    case TIME::CHAR::TIME_5:
      asMs = TIME::MS::TIME_5;
      break;
    case TIME::CHAR::TIME_10:
      asMs = TIME::MS::TIME_10;
      break;
    case TIME::EMPTY:
      asMs = 0;
      break;
    }
  }

  explicit Time(TIME::Period asMs): asMs(asMs) {
    if (asMs == TIME::MS::TIME_1_4000)
    {
      asChar = TIME::CHAR::TIME_1_4000;
    } else if (asMs == TIME::MS::TIME_1_2000) {
      asChar = TIME::CHAR::TIME_1_2000;
    } else if (asMs == TIME::MS::TIME_1_1000) {
      asChar = TIME::CHAR::TIME_1_1000;
    } else if (asMs == TIME::MS::TIME_1_500) {
      asChar = TIME::CHAR::TIME_1_500;
    } else if (asMs == TIME::MS::TIME_1_250) {
      asChar = TIME::CHAR::TIME_1_250;
    } else if (asMs == TIME::MS::TIME_1_125) {
      asChar = TIME::CHAR::TIME_1_125;
    } else if (asMs == TIME::MS::TIME_1_60) {
      asChar = TIME::CHAR::TIME_1_60;
    } else if (asMs == TIME::MS::TIME_1_30) {
      asChar = TIME::CHAR::TIME_1_30;
    } else if (asMs == TIME::MS::TIME_1_15) {
      asChar = TIME::CHAR::TIME_1_15;
    } else if (asMs == TIME::MS::TIME_1_8) {
      asChar = TIME::CHAR::TIME_1_8;
    } else if (asMs == TIME::MS::TIME_1_4) {
      asChar = TIME::CHAR::TIME_1_4;
    } else if (asMs == TIME::MS::TIME_1_2) {
      asChar = TIME::CHAR::TIME_1_2;
    } else if (asMs == TIME::MS::TIME_1) {
      asChar = TIME::CHAR::TIME_1;
    } else if (asMs == TIME::MS::TIME_2) {
      asChar = TIME::CHAR::TIME_2;
    } else if (asMs == TIME::MS::TIME_5) {
      asChar = TIME::CHAR::TIME_5;
    } else if (asMs == TIME::MS::TIME_10) {
      asChar = TIME::CHAR::TIME_10;
    } else {
      asChar = TIME::CHAR::EMPTY;
    }
  }
};

} // namespace GLOBAL

#endif // MIKROSKOP_GLOBAL_H
