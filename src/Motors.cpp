#include "Motors.h"
#include <atomic>
#include "Logger.h"

Motors::Motors(WebSocket* webSocket):
  m_webSocket(webSocket),
  m_manualClientID(std::nullopt),
  m_xStepper(
    STEP_PER_REV,
    std::get< 0 >(X_PINS),
    std::get< 1 >(X_PINS),
    std::get< 2 >(X_PINS),
    std::get< 3 >(X_PINS)),
  m_yStepper(
    STEP_PER_REV,
    std::get< 0 >(Y_PINS),
    std::get< 1 >(Y_PINS),
    std::get< 2 >(Y_PINS),
    std::get< 3 >(Y_PINS)),
  m_zStepper(
    STEP_PER_REV,
    std::get< 0 >(Z_PINS),
    std::get< 1 >(Z_PINS),
    std::get< 2 >(Z_PINS),
    std::get< 3 >(Z_PINS)),
  m_xPos(0),
  m_yPos(0),
  m_zPos(0),
  m_xExtremes(std::nullopt, std::nullopt),
  m_yExtremes(std::nullopt, std::nullopt),
  m_zExtremes(std::nullopt, std::nullopt),
  m_step(std::nullopt),
  m_time(GLOBAL::Time()),
  m_shouldStop(false),
  m_calibratingUL(false),
  m_calibratingLR(false),
  m_calibratingUp(false),
  m_calibratingDown(false),
  m_calibratingStepStart(false),
  m_calibratingStepEnd(false),
  m_manualMovingLeft(false),
  m_manualMovingRight(false),
  m_manualMovingUpper(false),
  m_manualMovingLower(false),
  m_manualMovingUp(false),
  m_manualMovingDown(false) {
  m_xStepper.setSpeed(SPEED_AUTO_RPM);
  m_yStepper.setSpeed(SPEED_AUTO_RPM);
  m_zStepper.setSpeed(SPEED_AUTO_RPM);

  using namespace GLOBAL::MSG_TYPE;
  using namespace GLOBAL::HANDLER;

  m_webSocket->attachListener(
    START, [this](AsyncWebSocketClient* client, String msg) {
      return startHandler(client, msg);
    });
  m_webSocket->attachListener(
    STOP, [this](AsyncWebSocketClient* client, String msg) {
      return stopHandler(client, msg);
    });
  m_webSocket->attachListener(
    CAL_START, [this](AsyncWebSocketClient* client, String msg) {
      return calStartHandler(client, msg);
    });
  m_webSocket->attachListener(
    CAL_UL, [this](AsyncWebSocketClient* client, String msg) {
      return calULHandler(client, msg);
    });
  m_webSocket->attachListener(
    CAL_LR, [this](AsyncWebSocketClient* client, String msg) {
      return calLRHandler(client, msg);
    });
  m_webSocket->attachListener(
    CAL_UP, [this](AsyncWebSocketClient* client, String msg) {
      return calUpHandler(client, msg);
    });
  m_webSocket->attachListener(
    CAL_DOWN, [this](AsyncWebSocketClient* client, String msg) {
      return calDownHandler(client, msg);
    });
  m_webSocket->attachListener(
    CAL_STEP, [this](AsyncWebSocketClient* client, String msg) {
      return calStepHandler(client, msg);
    });
  m_webSocket->attachListener(
    CAL_RESET_UL, [this](AsyncWebSocketClient* client, String msg) {
      return calResetULHandler(client, msg);
    });
  m_webSocket->attachListener(
    CAL_RESET_LR, [this](AsyncWebSocketClient* client, String msg) {
      return calResetLRHandler(client, msg);
    });
  m_webSocket->attachListener(
    CAL_RESET_U, [this](AsyncWebSocketClient* client, String msg) {
      return calResetUpHandler(client, msg);
    });
  m_webSocket->attachListener(
    CAL_RESET_D, [this](AsyncWebSocketClient* client, String msg) {
      return calResetDownHandler(client, msg);
    });
  m_webSocket->attachListener(
    CAL_RESET_STEP, [this](AsyncWebSocketClient* client, String msg) {
      return calResetStepHandler(client, msg);
    });
  m_webSocket->attachListener(
    MANUAL_START_LEFT, [this](AsyncWebSocketClient* client, String msg) {
      return mStartLeftHandler(client, msg);
    });
  m_webSocket->attachListener(
    MANUAL_START_RIGHT, [this](AsyncWebSocketClient* client, String msg) {
      return mStartRightHandler(client, msg);
    });
  m_webSocket->attachListener(
    MANUAL_START_UPPER, [this](AsyncWebSocketClient* client, String msg) {
      return mStartUpperHandler(client, msg);
    });
  m_webSocket->attachListener(
    MANUAL_START_LOWER, [this](AsyncWebSocketClient* client, String msg) {
      return mStartLowerHandler(client, msg);
    });
  m_webSocket->attachListener(
    MANUAL_START_UP, [this](AsyncWebSocketClient* client, String msg) {
      return mStartUpHandler(client, msg);
    });
  m_webSocket->attachListener(
    MANUAL_START_DOWN, [this](AsyncWebSocketClient* client, String msg) {
      return mStartDownHandler(client, msg);
    });
  m_webSocket->attachListener(
    MANUAL_STOP_LEFT, [this](AsyncWebSocketClient* client, String msg) {
      return mStopLeftHandler(client, msg);
    });
  m_webSocket->attachListener(
    MANUAL_STOP_RIGHT, [this](AsyncWebSocketClient* client, String msg) {
      return mStopRightHandler(client, msg);
    });
  m_webSocket->attachListener(
    MANUAL_STOP_UPPER, [this](AsyncWebSocketClient* client, String msg) {
      return mStopUpperHandler(client, msg);
    });
  m_webSocket->attachListener(
    MANUAL_STOP_LOWER, [this](AsyncWebSocketClient* client, String msg) {
      return mStopLowerHandler(client, msg);
    });
  m_webSocket->attachListener(
    MANUAL_STOP_UP, [this](AsyncWebSocketClient* client, String msg) {
      return mStopUpHandler(client, msg);
    });
  m_webSocket->attachListener(
    MANUAL_STOP_DOWN, [this](AsyncWebSocketClient* client, String msg) {
      return mStopDownHandler(client, msg);
    });
  m_webSocket->attachListener(
    MANUAL_CONFIRM, [this](AsyncWebSocketClient* client, String msg) {
      return mConfirmHandler(client, msg);
    });
  m_webSocket->attachListener(
    INFO, [this](AsyncWebSocketClient* client, String msg) {
      return infoHandler(client, msg);
    });
  m_webSocket->attachListener(
    TIME, [this](AsyncWebSocketClient* client, String msg) {
      return timeHandler(client, msg);
    });
  m_webSocket->attachListener(
    DISCONNECT, [this](AsyncWebSocketClient* client, String msg) {
      return disconnectHandler(client, msg);
    });
}

////////////////////////////////////////////////////////////////////////////////
// MANUAL MOVEMENT /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Motors::executeManualLeft() {
  while (m_manualMovingLeft.load())
  {
    m_xStepper.step(-1);
    m_xPos--;
    delay(SPEED_MANUAL_MS);
  }
}

void Motors::executeManualRight() {
  while (m_manualMovingRight.load())
  {
    m_xStepper.step(1);
    m_xPos++;
    delay(SPEED_MANUAL_MS);
  }
}

void Motors::executeManualUpper() {
  while (m_manualMovingUpper.load())
  {
    m_yStepper.step(-1);
    m_yPos--;
    delay(SPEED_MANUAL_MS);
  }
}

void Motors::executeManualLower() {
  while (m_manualMovingLower.load())
  {
    m_yStepper.step(1);
    m_yPos++;
    delay(SPEED_MANUAL_MS);
  }
}

void Motors::executeManualDown() {
  while (m_manualMovingDown.load())
  {
    m_zStepper.step(-1);
    m_zPos--;
    delay(SPEED_MANUAL_MS);
  }
}

void Motors::executeManualUp() {
  while (m_manualMovingUp.load())
  {
    m_zStepper.step(1);
    m_zPos++;
    delay(SPEED_MANUAL_MS);
  }
}

////////////////////////////////////////////////////////////////////////////////
// AUTOMATIC MOVEMENT //////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Motors::moveTo(
  int_fast16_t xTarget, int_fast16_t yTarget, int_fast16_t zTarget) {
}

void Motors::nextStep() {
}

String Motors::makeInfoString() const {
  String info("       ");

  // See JS function processInfo(data) For explanation

  m_mutex.lock();
  info[0] = m_time.asChar;
  m_mutex.unlock();

  info[1] = m_moveTask.isRunning() ? '1' : '0';

  info[2] = m_xExtremes.first.load() ? 'D' : m_calibratingUL.load() ? '1' : '0';

  info[3] = m_xExtremes.second.load() ? 'D' :
            m_calibratingLR.load()    ? '1' :
                                        '0';

  info[4] = m_zExtremes.second.load() ? 'D' :
            m_calibratingUp.load()    ? '1' :
                                        '0';

  info[5] = m_zExtremes.first.load() ? 'D' :
            m_calibratingDown.load() ? '1' :
                                       '0';

  info[6] = m_step.load().has_value()     ? 'D' :
            m_calibratingStepStart.load() ? '1' :
            m_calibratingStepEnd.load()   ? '2' :
                                            '0';

  return info;
}

////////////////////////////////////////////////////////////////////////////////
// Handlers ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

String Motors::startHandler(AsyncWebSocketClient* client, const String& msg) {
  Logger::debug("Start request received");

  // TODO: Implement

  return {};
}

String Motors::stopHandler(AsyncWebSocketClient* client, const String& msg) {
  Logger::debug("Stop request received");

  // TODO: Implement

  return {};
}

String Motors::calStartHandler(
  AsyncWebSocketClient* client, const String& /*msg*/) {
  Logger::debug("Calibration start request received");

  if (m_moveTask.isRunning())
  {
    Logger::warn("Already moving");
    return {};
  }

  m_manualMovingLeft  = false;
  m_manualMovingRight = false;
  m_manualMovingUpper = false;
  m_manualMovingLower = false;
  m_manualMovingUp    = false;
  m_manualMovingDown  = false;

  if (m_manualClientID.load().has_value())
  { m_webSocket->sendLostControl(client); }

  m_manualLeftTask.safeJoin();
  m_manualRightTask.safeJoin();
  m_manualUpperTask.safeJoin();
  m_manualLowerTask.safeJoin();
  m_manualUpTask.safeJoin();
  m_manualDownTask.safeJoin();

  m_manualClientID = client->id();

  m_xExtremes = std::make_pair(std::nullopt, std::nullopt);
  m_yExtremes = std::make_pair(std::nullopt, std::nullopt);
  m_zExtremes = std::make_pair(std::nullopt, std::nullopt);
  m_step      = std::nullopt;

  return String(static_cast< char >(GLOBAL::MSG_TYPE::CAL_START));
}

String Motors::calULHandler(
  AsyncWebSocketClient* client, const String& /*msg*/) {
  Logger::debug("Calibration UL request received");

  if (m_manualClientID.load() != client->id())
  {
    Logger::warn("Client ID mismatch");
    return {};
  }

  m_calibratingUp        = false;
  m_calibratingDown      = false;
  m_calibratingLR        = false;
  m_calibratingStepEnd   = false;
  m_calibratingStepStart = false;

  m_calibratingUL = true;

  return String(static_cast< char >(GLOBAL::MSG_TYPE::CAL_UL));
}

String Motors::calLRHandler(
  AsyncWebSocketClient* client, const String& /*msg*/) {
  Logger::debug("Calibration LR request received");

  if (m_manualClientID.load() != client->id())
  {
    Logger::warn("Client ID mismatch");
    return {};
  }

  m_calibratingUp        = false;
  m_calibratingDown      = false;
  m_calibratingUL        = false;
  m_calibratingStepEnd   = false;
  m_calibratingStepStart = false;

  m_calibratingLR = true;

  return String(static_cast< char >(GLOBAL::MSG_TYPE::CAL_LR));
}

String Motors::calUpHandler(
  AsyncWebSocketClient* client, const String& /*msg*/) {
  Logger::debug("Calibration Up request received");

  if (m_manualClientID.load() != client->id())
  {
    Logger::warn("Client ID mismatch");
    return {};
  }

  m_calibratingDown      = false;
  m_calibratingUL        = false;
  m_calibratingLR        = false;
  m_calibratingStepEnd   = false;
  m_calibratingStepStart = false;

  m_calibratingUp = true;

  return String(static_cast< char >(GLOBAL::MSG_TYPE::CAL_UP));
}

String Motors::calDownHandler(
  AsyncWebSocketClient* client, const String& /*msg*/) {
  Logger::debug("Calibration Down request received");

  if (m_manualClientID.load() != client->id())
  {
    Logger::warn("Client ID mismatch");
    return {};
  }

  m_calibratingUp        = false;
  m_calibratingUL        = false;
  m_calibratingLR        = false;
  m_calibratingStepEnd   = false;
  m_calibratingStepStart = false;

  m_calibratingDown = true;

  return String(static_cast< char >(GLOBAL::MSG_TYPE::CAL_DOWN));
}

String Motors::calStepHandler(
  AsyncWebSocketClient* client, const String& /*msg*/) {
  Logger::debug("Calibration Step request received");

  if (m_manualClientID.load() != client->id())
  {
    Logger::warn("Client ID mismatch");
    return {};
  }

  m_calibratingUp   = false;
  m_calibratingDown = false;
  m_calibratingUL   = false;
  m_calibratingLR   = false;

  m_calibratingStepStart = true;

  return String(static_cast< char >(GLOBAL::MSG_TYPE::CAL_STEP));
}

String Motors::calResetULHandler(
  AsyncWebSocketClient* client, const String& /*msg*/) {
  Logger::debug("Calibration Reset UL request received");

  if (m_manualClientID.load() != client->id())
  {
    Logger::warn("Client ID mismatch");
    return {};
  }

  m_xExtremes.first = std::nullopt;
  m_yExtremes.first = std::nullopt;

  m_calibratingUL = false;

  return String(static_cast< char >(GLOBAL::MSG_TYPE::CAL_RESET_UL));
}

String Motors::calResetLRHandler(
  AsyncWebSocketClient* client, const String& /*msg*/) {
  Logger::debug("Calibration Reset LR request received");

  if (m_manualClientID.load() != client->id())
  {
    Logger::warn("Client ID mismatch");
    return {};
  }

  m_xExtremes.second = std::nullopt;
  m_yExtremes.second = std::nullopt;

  m_calibratingLR = false;

  return String(static_cast< char >(GLOBAL::MSG_TYPE::CAL_RESET_LR));
}

String Motors::calResetUpHandler(
  AsyncWebSocketClient* client, const String& /*msg*/) {
  Logger::debug("Calibration Reset Up request received");

  if (m_manualClientID.load() != client->id())
  {
    Logger::warn("Client ID mismatch");
    return {};
  }

  m_zExtremes.first = std::nullopt;

  m_calibratingUp = false;

  return String(static_cast< char >(GLOBAL::MSG_TYPE::CAL_RESET_U));
}

String Motors::calResetDownHandler(
  AsyncWebSocketClient* client, const String& /*msg*/) {
  Logger::debug("Calibration Reset Down request received");

  if (m_manualClientID.load() != client->id())
  {
    Logger::warn("Client ID mismatch");
    return {};
  }

  m_zExtremes.second = std::nullopt;

  m_calibratingDown = false;

  return String(static_cast< char >(GLOBAL::MSG_TYPE::CAL_RESET_D));
}

String Motors::calResetStepHandler(
  AsyncWebSocketClient* client, const String& /*msg*/) {
  Logger::debug("Calibration Reset Step request received");

  if (m_manualClientID.load() != client->id())
  {
    Logger::warn("Client ID mismatch");
    return {};
  }

  m_step = std::nullopt;

  m_calibratingStepStart = false;
  m_calibratingStepEnd   = false;

  return String(static_cast< char >(GLOBAL::MSG_TYPE::CAL_RESET_STEP));
}

String Motors::mStartLeftHandler(
  AsyncWebSocketClient* client, const String& /*msg*/) {
  Logger::debug("Manual Start Left request received");

  if (client->id() != m_manualClientID.load())
  {
    Logger::warn("Client ID mismatch");
    return {};
  }

  m_manualMovingLeft = true;
  m_manualLeftTask   = TrackedTask(false, [this]() { executeManualLeft(); });

  return String(static_cast< char >(GLOBAL::MSG_TYPE::MANUAL_START_LEFT));
}

String Motors::mStartRightHandler(
  AsyncWebSocketClient* client, const String& /*msg*/) {
  Logger::debug("Manual Start Right request received");

  if (client->id() != m_manualClientID.load())
  {
    Logger::warn("Client ID mismatch");
    return {};
  }

  m_manualMovingRight = true;
  m_manualRightTask   = TrackedTask(false, [this]() { executeManualRight(); });

  return String(static_cast< char >(GLOBAL::MSG_TYPE::MANUAL_START_RIGHT));
}

String Motors::mStartUpperHandler(
  AsyncWebSocketClient* client, const String& /*msg*/) {
  Logger::debug("Manual Start Upper request received");

  if (client->id() != m_manualClientID.load())
  {
    Logger::warn("Client ID mismatch");
    return {};
  }

  m_manualMovingUpper = true;
  m_manualUpperTask   = TrackedTask(false, [this]() { executeManualUpper(); });

  return String(static_cast< char >(GLOBAL::MSG_TYPE::MANUAL_START_UPPER));
}

String Motors::mStartLowerHandler(
  AsyncWebSocketClient* client, const String& /*msg*/) {
  Logger::debug("Manual Start Lower request received");

  if (client->id() != m_manualClientID.load())
  {
    Logger::warn("Client ID mismatch");
    return {};
  }

  m_manualMovingLower = true;
  m_manualLowerTask   = TrackedTask(false, [this]() { executeManualLower(); });

  return String(static_cast< char >(GLOBAL::MSG_TYPE::MANUAL_START_LOWER));
}

String Motors::mStartUpHandler(
  AsyncWebSocketClient* client, const String& /*msg*/) {
  Logger::debug("Manual Start Up request received");

  if (client->id() != m_manualClientID.load())
  {
    Logger::warn("Client ID mismatch");
    return {};
  }

  m_manualMovingUp = true;
  m_manualUpTask   = TrackedTask(false, [this]() { executeManualUp(); });

  return String(static_cast< char >(GLOBAL::MSG_TYPE::MANUAL_START_UP));
}

String Motors::mStartDownHandler(
  AsyncWebSocketClient* client, const String& /*msg*/) {
  Logger::debug("Manual Start Down request received");

  if (client->id() != m_manualClientID.load())
  {
    Logger::warn("Client ID mismatch");
    return {};
  }

  m_manualMovingDown = true;
  m_manualDownTask   = TrackedTask(false, [this]() { executeManualDown(); });

  return String(static_cast< char >(GLOBAL::MSG_TYPE::MANUAL_START_DOWN));
}

String Motors::mStopLeftHandler(
  AsyncWebSocketClient* client, const String& /*msg*/) {
  Logger::debug("Manual Stop Left request received");

  if (!m_manualClientID.load().has_value())
  {
    Logger::warn("No manual client connected");
    return {};
  } else if (client->id() != m_manualClientID.load().value()) {
    Logger::warn("Manual client ID mismatch");
    return {};
  }

  m_manualMovingLeft = false;
  m_manualLeftTask.safeJoin();

  return String(static_cast< char >(GLOBAL::MSG_TYPE::MANUAL_STOP_LEFT));
}

String Motors::mStopRightHandler(
  AsyncWebSocketClient* client, const String& /*msg*/) {
  Logger::debug("Manual Stop Right request received");

  if (!m_manualClientID.load().has_value())
  {
    Logger::warn("No manual client connected");
    return {};
  } else if (client->id() != m_manualClientID.load().value()) {
    Logger::warn("Manual client ID mismatch");
    return {};
  }

  m_manualMovingRight = false;
  m_manualRightTask.safeJoin();

  return String(static_cast< char >(GLOBAL::MSG_TYPE::MANUAL_STOP_RIGHT));
}

String Motors::mStopUpperHandler(
  AsyncWebSocketClient* client, const String& /*msg*/) {
  Logger::debug("Manual Stop Upper request received");

  if (!m_manualClientID.load().has_value())
  {
    Logger::warn("No manual client connected");
    return {};
  } else if (client->id() != m_manualClientID.load().value()) {
    Logger::warn("Manual client ID mismatch");
    return {};
  }

  m_manualMovingUpper = false;
  m_manualUpperTask.safeJoin();

  return String(static_cast< char >(GLOBAL::MSG_TYPE::MANUAL_STOP_UPPER));
}

String Motors::mStopLowerHandler(
  AsyncWebSocketClient* client, const String& /*msg*/) {
  Logger::debug("Manual Stop Lower request received");

  if (!m_manualClientID.load().has_value())
  {
    Logger::warn("No manual client connected");
    return {};
  } else if (client->id() != m_manualClientID.load().value()) {
    Logger::warn("Manual client ID mismatch");
    return {};
  }

  m_manualMovingLower = false;
  m_manualLowerTask.safeJoin();

  return String(static_cast< char >(GLOBAL::MSG_TYPE::MANUAL_STOP_LOWER));
}

String Motors::mStopUpHandler(
  AsyncWebSocketClient* client, const String& /*msg*/) {
  Logger::debug("Manual Stop Up request received");

  if (!m_manualClientID.load().has_value())
  {
    Logger::warn("No manual client connected");
    return {};
  } else if (client->id() != m_manualClientID.load().value()) {
    Logger::warn("Manual client ID mismatch");
    return {};
  }

  m_manualMovingUp = false;
  m_manualUpTask.safeJoin();

  return String(static_cast< char >(GLOBAL::MSG_TYPE::MANUAL_STOP_UP));
}

String Motors::mStopDownHandler(
  AsyncWebSocketClient* client, const String& /*msg*/) {
  Logger::debug("Manual Stop Down request received");

  if (!m_manualClientID.load().has_value())
  {
    Logger::warn("No manual client connected");
    return {};
  } else if (client->id() != m_manualClientID.load().value()) {
    Logger::warn("Manual client ID mismatch");
    return {};
  }

  m_manualMovingDown = false;
  m_manualDownTask.safeJoin();

  return String(static_cast< char >(GLOBAL::MSG_TYPE::MANUAL_STOP_DOWN));
}

String Motors::mConfirmHandler(
  AsyncWebSocketClient* client, const String& /*msg*/) {
  Logger::debug("Manual Confirm request received");

  if (!m_manualClientID.load().has_value())
  {
    Logger::warn("No manual client connected");
    return {};
  } else if (client->id() != m_manualClientID.load().value()) {
    Logger::warn("Manual client ID mismatch");
    return {};
  }

  if (m_calibratingUL.load())
  {
    m_xExtremes.first = m_xPos;
    m_yExtremes.first = m_yPos;
    m_calibratingUL   = false;
  } else if (m_calibratingLR.load()) {
    m_xExtremes.second = m_xPos;
    m_yExtremes.second = m_yPos;
    m_calibratingLR    = false;
  } else if (m_calibratingUp.load()) {
    m_zExtremes.second = m_zPos;
    m_calibratingUp    = false;
  } else if (m_calibratingDown.load()) {
    m_zExtremes.first = m_zPos;
    m_calibratingDown = false;
  } else if (m_calibratingStepStart.load()) {
    m_step                 = m_xPos;
    m_calibratingStepStart = false;
  } else if (m_calibratingStepEnd.load()) {
    m_step               = m_xPos - m_step.load().value();
    m_calibratingStepEnd = false;
  } else {
    Logger::warn("No manual action");
    return {};
  }

  return String(static_cast< char >(GLOBAL::MSG_TYPE::MANUAL_CONFIRM));
}

String Motors::infoHandler(
  AsyncWebSocketClient* /*client*/, const String& /*msg*/) {
  Logger::debug("Info request received");

  return makeInfoString();
}

String Motors::timeHandler(
  AsyncWebSocketClient* /*client*/, const String& msg) {
  Logger::debug("Time request received");

  m_time = GLOBAL::Time(static_cast< GLOBAL::TIME::CHAR >(msg[0]));

  return msg.substring(1);
}

String Motors::disconnectHandler(
  AsyncWebSocketClient* client, const String& msg) {
  Logger::debug("Disconnect request received");

  if (client->id() == m_manualClientID.load())
  {
    m_manualMovingLeft  = false;
    m_manualMovingRight = false;
    m_manualMovingUpper = false;
    m_manualMovingLower = false;
    m_manualMovingUp    = false;
    m_manualMovingDown  = false;
    m_manualClientID    = std::nullopt;
  }

  return {};
}
