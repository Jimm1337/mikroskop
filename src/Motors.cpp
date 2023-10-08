#include "Motors.h"
#include "Logger.h"
#include <cstdlib>

Motors::Motors(WebSocket *webSocket, Camera *camera)
    : m_webSocket(webSocket), m_camera(camera), m_manualClientID(std::nullopt),
      m_xStepper(STEP_PER_REV, std::get<0>(X_PINS), std::get<1>(X_PINS),
                 std::get<2>(X_PINS), std::get<3>(X_PINS)),
      m_yStepper(STEP_PER_REV, std::get<0>(Y_PINS), std::get<1>(Y_PINS),
                 std::get<2>(Y_PINS), std::get<3>(Y_PINS)),
      m_zStepper(STEP_PER_REV, std::get<0>(Z_PINS), std::get<1>(Z_PINS),
                 std::get<2>(Z_PINS), std::get<3>(Z_PINS)),
      m_xPos(0), m_yPos(0), m_zPos(0), m_xExtremes(std::nullopt, std::nullopt),
      m_yExtremes(std::nullopt, std::nullopt),
      m_zExtremes(std::nullopt, std::nullopt), m_step(std::nullopt),
      m_readyCount(0), m_ready(false), m_shouldStop(true),
      m_calibratingUL(false), m_calibratingLR(false), m_calibratingUp(false),
      m_calibratingDown(false), m_calibratingStepStart(false),
      m_calibratingStepEnd(false), m_manualMovingLeft(false),
      m_manualMovingRight(false), m_manualMovingUpper(false),
      m_manualMovingLower(false), m_manualMovingUp(false),
      m_manualMovingDown(false), m_stepSet(false), m_nextStepLeft(false),
      m_time(GLOBAL::TIME::CHAR::TIME_1_125), m_captureFilm(false),
      m_movementSmooth(false) {
  m_xStepper.setSpeed(SPEED_AUTO_RPM);
  m_yStepper.setSpeed(SPEED_AUTO_RPM);
  m_zStepper.setSpeed(SPEED_AUTO_RPM);

  using namespace GLOBAL::MSG_TYPE;
  using namespace GLOBAL::HANDLER;

  m_webSocket->attachListener(START,
                              [this](AsyncWebSocketClient *client, String msg) {
                                startHandler(client, msg);
                              });
  m_webSocket->attachListener(STOP,
                              [this](AsyncWebSocketClient *client, String msg) {
                                stopHandler(client, msg);
                              });
  m_webSocket->attachListener(CAL_START,
                              [this](AsyncWebSocketClient *client, String msg) {
                                calStartHandler(client, msg);
                              });
  m_webSocket->attachListener(CAL_UL,
                              [this](AsyncWebSocketClient *client, String msg) {
                                calULHandler(client, msg);
                              });
  m_webSocket->attachListener(CAL_LR,
                              [this](AsyncWebSocketClient *client, String msg) {
                                calLRHandler(client, msg);
                              });
  m_webSocket->attachListener(CAL_UP,
                              [this](AsyncWebSocketClient *client, String msg) {
                                calUpHandler(client, msg);
                              });
  m_webSocket->attachListener(CAL_DOWN,
                              [this](AsyncWebSocketClient *client, String msg) {
                                calDownHandler(client, msg);
                              });
  m_webSocket->attachListener(CAL_STEP,
                              [this](AsyncWebSocketClient *client, String msg) {
                                calStepHandler(client, msg);
                              });
  m_webSocket->attachListener(CAL_RESET_UL,
                              [this](AsyncWebSocketClient *client, String msg) {
                                calResetULHandler(client, msg);
                              });
  m_webSocket->attachListener(CAL_RESET_LR,
                              [this](AsyncWebSocketClient *client, String msg) {
                                calResetLRHandler(client, msg);
                              });
  m_webSocket->attachListener(CAL_RESET_U,
                              [this](AsyncWebSocketClient *client, String msg) {
                                calResetUpHandler(client, msg);
                              });
  m_webSocket->attachListener(CAL_RESET_D,
                              [this](AsyncWebSocketClient *client, String msg) {
                                calResetDownHandler(client, msg);
                              });
  m_webSocket->attachListener(CAL_RESET_STEP,
                              [this](AsyncWebSocketClient *client, String msg) {
                                calResetStepHandler(client, msg);
                              });
  m_webSocket->attachListener(MANUAL_START_LEFT,
                              [this](AsyncWebSocketClient *client, String msg) {
                                mStartLeftHandler(client, msg);
                              });
  m_webSocket->attachListener(MANUAL_START_RIGHT,
                              [this](AsyncWebSocketClient *client, String msg) {
                                mStartRightHandler(client, msg);
                              });
  m_webSocket->attachListener(MANUAL_START_UPPER,
                              [this](AsyncWebSocketClient *client, String msg) {
                                mStartUpperHandler(client, msg);
                              });
  m_webSocket->attachListener(MANUAL_START_LOWER,
                              [this](AsyncWebSocketClient *client, String msg) {
                                mStartLowerHandler(client, msg);
                              });
  m_webSocket->attachListener(MANUAL_START_UP,
                              [this](AsyncWebSocketClient *client, String msg) {
                                mStartUpHandler(client, msg);
                              });
  m_webSocket->attachListener(MANUAL_START_DOWN,
                              [this](AsyncWebSocketClient *client, String msg) {
                                mStartDownHandler(client, msg);
                              });
  m_webSocket->attachListener(MANUAL_STOP_LEFT,
                              [this](AsyncWebSocketClient *client, String msg) {
                                mStopLeftHandler(client, msg);
                              });
  m_webSocket->attachListener(MANUAL_STOP_RIGHT,
                              [this](AsyncWebSocketClient *client, String msg) {
                                mStopRightHandler(client, msg);
                              });
  m_webSocket->attachListener(MANUAL_STOP_UPPER,
                              [this](AsyncWebSocketClient *client, String msg) {
                                mStopUpperHandler(client, msg);
                              });
  m_webSocket->attachListener(MANUAL_STOP_LOWER,
                              [this](AsyncWebSocketClient *client, String msg) {
                                mStopLowerHandler(client, msg);
                              });
  m_webSocket->attachListener(MANUAL_STOP_UP,
                              [this](AsyncWebSocketClient *client, String msg) {
                                mStopUpHandler(client, msg);
                              });
  m_webSocket->attachListener(MANUAL_STOP_DOWN,
                              [this](AsyncWebSocketClient *client, String msg) {
                                mStopDownHandler(client, msg);
                              });
  m_webSocket->attachListener(MANUAL_CONFIRM,
                              [this](AsyncWebSocketClient *client, String msg) {
                                mConfirmHandler(client, msg);
                              });
  m_webSocket->attachListener(INFO,
                              [this](AsyncWebSocketClient *client, String msg) {
                                infoHandler(client, msg);
                              });
  m_webSocket->attachListener(TIME,
                              [this](AsyncWebSocketClient *client, String msg) {
                                timeHandler(client, msg);
                              });
  m_webSocket->attachListener(MOVEMENT,
                              [this](AsyncWebSocketClient *client, String msg) {
                                movementModeHandler(client, msg);
                              });
  m_webSocket->attachListener(CAPTURE,
                              [this](AsyncWebSocketClient *client, String msg) {
                                captureModeHandler(client, msg);
                              });
  m_webSocket->attachListener(DISCONNECT,
                              [this](AsyncWebSocketClient *client, String msg) {
                                disconnectHandler(client, msg);
                              });
  m_webSocket->attachListener(CONNECT,
                              [this](AsyncWebSocketClient *client, String msg) {
                                connectHandler(client, msg);
                              });

  m_camera->setShutterSpeed(DEFAULT_SHUTTER_SPEED);
}

////////////////////////////////////////////////////////////////////////////////
// MANUAL MOVEMENT /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Motors::executeManualLeft() {
  while (m_manualMovingLeft.load()) {
    m_xStepper.step(-1);
    m_xPos--;
    delay(SPEED_MANUAL_MS);
  }
}

void Motors::executeManualRight() {
  while (m_manualMovingRight.load()) {
    m_xStepper.step(1);
    m_xPos++;
    delay(SPEED_MANUAL_MS);
  }
}

void Motors::executeManualUpper() {
  while (m_manualMovingUpper.load()) {
    m_yStepper.step(-1);
    m_yPos--;
    delay(SPEED_MANUAL_MS);
  }
}

void Motors::executeManualLower() {
  while (m_manualMovingLower.load()) {
    m_yStepper.step(1);
    m_yPos++;
    delay(SPEED_MANUAL_MS);
  }
}

void Motors::executeManualDown() {
  while (m_manualMovingDown.load()) {
    m_zStepper.step(-1);
    m_zPos--;
    delay(SPEED_MANUAL_MS);
  }
}

void Motors::executeManualUp() {
  while (m_manualMovingUp.load()) {
    m_zStepper.step(1);
    m_zPos++;
    delay(SPEED_MANUAL_MS);
  }
}

////////////////////////////////////////////////////////////////////////////////
// AUTOMATIC MOVEMENT //////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Motors::moveTo(int_fast16_t target, Direction direction) {
  auto steps = target;

  switch (direction) {
  case Direction::X:
    steps -= m_xPos;
    m_xStepper.step(steps);
    m_xPos = target;
    break;
  case Direction::Y:
    steps -= m_yPos;
    m_yStepper.step(steps);
    m_yPos = target;
    break;
  case Direction::Z:
    steps -= m_zPos;
    m_zStepper.step(steps);
    m_zPos = target;
    break;
  }
}

bool Motors::nextStep() {
  // -> -> -> v
  // v <- <- <-
  // -> -> -> v
  // v <- <- <-
  // -> -> -> |

  Direction direction{};
  int steps{};

  if (m_xPos == m_xExtremes.second.load() && !m_nextStepLeft) {
    direction = Direction::Y;
    steps = min(*m_step.load(), *m_yExtremes.second.load() - m_yPos);
    m_nextStepLeft = true;
  } else if (m_xPos == m_xExtremes.first.load() && m_nextStepLeft) {
    direction = Direction::Y;
    steps = min(*m_step.load(), *m_yExtremes.second.load() - m_yPos);
    m_nextStepLeft = false;
  } else {
    direction = Direction::X;
    if (!m_nextStepLeft) {
      steps = min(*m_step.load(), *m_xExtremes.second.load() - m_xPos);
    } else {
      steps = max(-*m_step.load(), *m_xExtremes.first.load() - m_xPos);
    }
  }

  if (steps == 0) {
    Logger::info("End of pattern reached");
    return false;
  }

  switch (direction) {
  case Direction::X:
    moveTo(m_xPos + steps, direction);
    break;
  case Direction::Y:
    moveTo(m_yPos + steps, direction);
    break;
  default:
    break;
  }

  return true;
}

bool Motors::nextHeight() {
  if (m_zPos == *m_zExtremes.second.load()) {
    return false;
  }
  moveTo(m_zPos + 1, Direction::Z);
  return true;
}

void Motors::resetHeight() { moveTo(*m_zExtremes.first.load(), Direction::Z); }

void Motors::moveTask() {
  // Goto Upper Left Lowest Height
  moveTo(*m_xExtremes.first.load(), Direction::X);
  moveTo(*m_yExtremes.first.load(), Direction::Y);
  moveTo(*m_zExtremes.first.load(), Direction::Z);
  m_nextStepLeft = false;

  // Execute pattern
  while (!m_shouldStop.load()) {
    do {
      m_camera->takePictureAndWait();
    } while (nextHeight());

    resetHeight();

    if (!nextStep()) {
      break;
    }
  }

  m_shouldStop = true;
}

////////////////////////////////////////////////////////////////////////////////
// Helpers /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

String Motors::makeInfoString() const {
  String info("         ");

  // See JS function processInfo(data) For explanation

  m_mutex.lock();
  info[0] = m_time.asChar;
  m_mutex.unlock();

  info[1] = m_moveTask.isRunning() ? '1' : '0'; // todo: not working correctly!

  info[2] = m_xExtremes.first.load() ? 'D' : m_calibratingUL.load() ? '1' : '0';

  info[3] = m_xExtremes.second.load() ? 'D'
            : m_calibratingLR.load()  ? '1'
                                      : '0';

  info[4] = m_zExtremes.second.load() ? 'D'
            : m_calibratingUp.load()  ? '1'
                                      : '0';

  info[5] = m_zExtremes.first.load()   ? 'D'
            : m_calibratingDown.load() ? '1'
                                       : '0';

  info[6] = m_stepSet.load()                ? 'D'
            : m_calibratingStepStart.load() ? '1'
            : m_calibratingStepEnd.load()   ? '2'
                                            : '0';

  info[7] = m_movementSmooth ? GLOBAL::MSG_DATA_MOVEMENT::DATA_MOVEMENT_SMOOTH
                             : GLOBAL::MSG_DATA_MOVEMENT::DATA_MOVEMENT_SNAP;

  info[8] = m_captureFilm ? GLOBAL::MSG_DATA_CAPTURE::DATA_CAPTURE_FILM
                          : GLOBAL::MSG_DATA_CAPTURE::DATA_CAPTURE_PHOTO;

  return String(static_cast<char>(GLOBAL::MSG_TYPE::INFO)) + info;
}

bool Motors::verifyManualClient(AsyncWebSocketClient *client) const {
  if (m_manualClientID.load() != client->id()) {
    Logger::warn("Client ID mismatch");
    return false;
  }

  return true;
}

void Motors::checkReady() {
  Logger::debug(String("Ready counter: ") + m_readyCount);

  if (m_readyCount == READY_COUNT) {
    m_ready = true;
    m_webSocket->send(String(static_cast<char>(GLOBAL::MSG_TYPE::READY)));
  }
}

void Motors::checkNotReady() {
  Logger::debug(String("Ready counter: ") + m_readyCount);

  if (m_ready.load()) {
    m_ready = false;
    m_webSocket->send(String(static_cast<char>(GLOBAL::MSG_TYPE::NOT_READY)));
  }
}

void Motors::resetValue(std::atomic<std::optional<int_fast16_t>> &value) {
  if (value.load().has_value()) {
    value = std::nullopt;
    --m_readyCount;
  }
}

void Motors::resetValues(std::atomic<std::optional<int_fast16_t>> &value1,
                         std::atomic<std::optional<int_fast16_t>> &value2) {
  if (value1.load().has_value() && value2.load().has_value()) {
    value1 = std::nullopt;
    value2 = std::nullopt;
    --m_readyCount;
  }
}

void Motors::resetStep() {
  m_step = std::nullopt;

  if (m_stepSet.load()) {
    m_stepSet = false;
    --m_readyCount;
  }
}

void Motors::setValue(std::atomic<std::optional<int_fast16_t>> &ref,
                      int_fast16_t newValue) {
  if (!ref.load().has_value()) {
    ++m_readyCount;
  }
  ref = newValue;
}

void Motors::setValues(std::atomic<std::optional<int_fast16_t>> &ref1,
                       std::atomic<std::optional<int_fast16_t>> &ref2,
                       int_fast16_t newValue1, int_fast16_t newValue2) {
  if (!ref1.load().has_value() && !ref2.load().has_value()) {
    ++m_readyCount;
  }
  ref1 = newValue1;
  ref2 = newValue2;
}

void Motors::setStep(int_fast16_t newValue) {
  if (m_step.load().has_value()) {
    m_step = abs(newValue - *m_step.load());
    m_stepSet = true;
    ++m_readyCount;
  } else {
    m_step = newValue;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Handlers ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Motors::startHandler(AsyncWebSocketClient *client, const String &msg) {
  Logger::debug("Start request received");

  m_shouldStop = false;

  m_webSocket->send(String(static_cast<char>(GLOBAL::MSG_TYPE::START)));

  m_moveTask = TrackedTask(false, [this]() {
    Logger::info("Move task started");
    moveTask();
    Logger::info("Move task ended");
    m_webSocket->send(String(static_cast<char>(GLOBAL::MSG_TYPE::STOP)));
  });
}

void Motors::stopHandler(AsyncWebSocketClient *client, const String &msg) {
  Logger::debug("Stop request received");

  m_shouldStop = true;
  m_moveTask.safeJoin();
}

void Motors::calStartHandler(AsyncWebSocketClient *client,
                             const String & /*msg*/) {
  Logger::debug("Calibration start request received");

  if (m_moveTask.isRunning()) {
    Logger::warn("Already moving");
    return;
  }

  m_manualMovingLeft = false;
  m_manualMovingRight = false;
  m_manualMovingUpper = false;
  m_manualMovingLower = false;
  m_manualMovingUp = false;
  m_manualMovingDown = false;

  if (m_manualClientID.load().has_value()) {
    WebSocket::sendLostControl(client);
  }
  m_manualClientID = client->id();

  resetValues(m_xExtremes.first, m_yExtremes.first);
  resetValues(m_xExtremes.second, m_yExtremes.second);
  resetValue(m_zExtremes.first);
  resetValue(m_zExtremes.second);
  resetStep();

  m_manualLeftTask.safeJoin();
  m_manualRightTask.safeJoin();
  m_manualUpperTask.safeJoin();
  m_manualLowerTask.safeJoin();
  m_manualUpTask.safeJoin();
  m_manualDownTask.safeJoin();

  checkNotReady();

  m_webSocket->send(String(static_cast<char>(GLOBAL::MSG_TYPE::CAL_START)));
}

void Motors::calULHandler(AsyncWebSocketClient *client,
                          const String & /*msg*/) {
  Logger::debug("Calibration UL request received");

  if (!verifyManualClient(client)) {
    return;
  }

  resetValues(m_xExtremes.first, m_yExtremes.first);

  m_calibratingUp = false;
  m_calibratingDown = false;
  m_calibratingLR = false;
  m_calibratingStepEnd = false;
  m_calibratingStepStart = false;

  m_calibratingUL = true;

  checkNotReady();

  m_webSocket->send(String(static_cast<char>(GLOBAL::MSG_TYPE::CAL_UL)));
}

void Motors::calLRHandler(AsyncWebSocketClient *client,
                          const String & /*msg*/) {
  Logger::debug("Calibration LR request received");

  if (!verifyManualClient(client)) {
    return;
  }

  resetValues(m_xExtremes.second, m_yExtremes.second);

  m_calibratingUp = false;
  m_calibratingDown = false;
  m_calibratingUL = false;
  m_calibratingStepEnd = false;
  m_calibratingStepStart = false;

  m_calibratingLR = true;

  checkNotReady();

  m_webSocket->send(String(static_cast<char>(GLOBAL::MSG_TYPE::CAL_LR)));
}

void Motors::calUpHandler(AsyncWebSocketClient *client,
                          const String & /*msg*/) {
  Logger::debug("Calibration Up request received");

  if (!verifyManualClient(client)) {
    return;
  }

  resetValue(m_zExtremes.second);

  m_calibratingDown = false;
  m_calibratingUL = false;
  m_calibratingLR = false;
  m_calibratingStepEnd = false;
  m_calibratingStepStart = false;

  m_calibratingUp = true;

  checkNotReady();

  m_webSocket->send(String(static_cast<char>(GLOBAL::MSG_TYPE::CAL_UP)));
}

void Motors::calDownHandler(AsyncWebSocketClient *client,
                            const String & /*msg*/) {
  Logger::debug("Calibration Down request received");

  if (!verifyManualClient(client)) {
    return;
  }

  resetValue(m_zExtremes.first);

  m_calibratingUp = false;
  m_calibratingUL = false;
  m_calibratingLR = false;
  m_calibratingStepEnd = false;
  m_calibratingStepStart = false;

  m_calibratingDown = true;

  checkNotReady();

  m_webSocket->send(String(static_cast<char>(GLOBAL::MSG_TYPE::CAL_DOWN)));
}

void Motors::calStepHandler(AsyncWebSocketClient *client,
                            const String & /*msg*/) {
  Logger::debug("Calibration Step request received");

  if (!verifyManualClient(client)) {
    return;
  }

  resetStep();

  m_calibratingUp = false;
  m_calibratingDown = false;
  m_calibratingUL = false;
  m_calibratingLR = false;
  m_calibratingStepEnd = false;

  m_calibratingStepStart = true;

  checkNotReady();

  m_webSocket->send(String(static_cast<char>(GLOBAL::MSG_TYPE::CAL_STEP)));
}

void Motors::calResetULHandler(AsyncWebSocketClient *client,
                               const String & /*msg*/) {
  Logger::debug("Calibration Reset UL request received");

  if (!verifyManualClient(client)) {
    return;
  }

  resetValues(m_xExtremes.first, m_yExtremes.first);

  m_calibratingUL = false;

  checkNotReady();

  m_webSocket->send(String(static_cast<char>(GLOBAL::MSG_TYPE::CAL_RESET_UL)));
}

void Motors::calResetLRHandler(AsyncWebSocketClient *client,
                               const String & /*msg*/) {
  Logger::debug("Calibration Reset LR request received");

  if (!verifyManualClient(client)) {
    return;
  }

  resetValues(m_xExtremes.second, m_yExtremes.second);

  m_calibratingLR = false;

  checkNotReady();

  m_webSocket->send(String(static_cast<char>(GLOBAL::MSG_TYPE::CAL_RESET_LR)));
}

void Motors::calResetUpHandler(AsyncWebSocketClient *client,
                               const String & /*msg*/) {
  Logger::debug("Calibration Reset Up request received");

  if (!verifyManualClient(client)) {
    return;
  }

  resetValue(m_zExtremes.second);

  m_calibratingUp = false;

  checkNotReady();

  m_webSocket->send(String(static_cast<char>(GLOBAL::MSG_TYPE::CAL_RESET_U)));
}

void Motors::calResetDownHandler(AsyncWebSocketClient *client,
                                 const String & /*msg*/) {
  Logger::debug("Calibration Reset Down request received");

  if (!verifyManualClient(client)) {
    return;
  }

  resetValue(m_zExtremes.first);

  m_calibratingDown = false;

  checkNotReady();

  m_webSocket->send(String(static_cast<char>(GLOBAL::MSG_TYPE::CAL_RESET_D)));
}

void Motors::calResetStepHandler(AsyncWebSocketClient *client,
                                 const String & /*msg*/) {
  Logger::debug("Calibration Reset Step request received");

  if (!verifyManualClient(client)) {
    return;
  }

  resetStep();

  m_calibratingStepStart = false;
  m_calibratingStepEnd = false;

  checkNotReady();

  m_webSocket->send(
      String(static_cast<char>(GLOBAL::MSG_TYPE::CAL_RESET_STEP)));
}

void Motors::mStartLeftHandler(AsyncWebSocketClient *client,
                               const String & /*msg*/) {
  Logger::debug("Manual Start Left request received");

  if (!verifyManualClient(client)) {
    return;
  }

  m_manualMovingLeft = true;
  m_manualLeftTask = TrackedTask(false, [this]() { executeManualLeft(); });

  m_webSocket->send(
      String(static_cast<char>(GLOBAL::MSG_TYPE::MANUAL_START_LEFT)));
}

void Motors::mStartRightHandler(AsyncWebSocketClient *client,
                                const String & /*msg*/) {
  Logger::debug("Manual Start Right request received");

  if (!verifyManualClient(client)) {
    return;
  }

  m_manualMovingRight = true;
  m_manualRightTask = TrackedTask(false, [this]() { executeManualRight(); });

  m_webSocket->send(
      String(static_cast<char>(GLOBAL::MSG_TYPE::MANUAL_START_RIGHT)));
}

void Motors::mStartUpperHandler(AsyncWebSocketClient *client,
                                const String & /*msg*/) {
  Logger::debug("Manual Start Upper request received");

  if (!verifyManualClient(client)) {
    return;
  }

  m_manualMovingUpper = true;
  m_manualUpperTask = TrackedTask(false, [this]() { executeManualUpper(); });

  m_webSocket->send(
      String(static_cast<char>(GLOBAL::MSG_TYPE::MANUAL_START_UPPER)));
}

void Motors::mStartLowerHandler(AsyncWebSocketClient *client,
                                const String & /*msg*/) {
  Logger::debug("Manual Start Lower request received");

  if (!verifyManualClient(client)) {
    return;
  }

  m_manualMovingLower = true;
  m_manualLowerTask = TrackedTask(false, [this]() { executeManualLower(); });

  m_webSocket->send(
      String(static_cast<char>(GLOBAL::MSG_TYPE::MANUAL_START_LOWER)));
}

void Motors::mStartUpHandler(AsyncWebSocketClient *client,
                             const String & /*msg*/) {
  Logger::debug("Manual Start Up request received");

  if (!verifyManualClient(client)) {
    return;
  }

  m_manualMovingUp = true;
  m_manualUpTask = TrackedTask(false, [this]() { executeManualUp(); });

  m_webSocket->send(
      String(static_cast<char>(GLOBAL::MSG_TYPE::MANUAL_START_UP)));
}

void Motors::mStartDownHandler(AsyncWebSocketClient *client,
                               const String & /*msg*/) {
  Logger::debug("Manual Start Down request received");

  if (!verifyManualClient(client)) {
    return;
  }

  m_manualMovingDown = true;
  m_manualDownTask = TrackedTask(false, [this]() { executeManualDown(); });

  m_webSocket->send(
      String(static_cast<char>(GLOBAL::MSG_TYPE::MANUAL_START_DOWN)));
}

void Motors::mStopLeftHandler(AsyncWebSocketClient *client,
                              const String & /*msg*/) {
  Logger::debug("Manual Stop Left request received");

  if (!verifyManualClient(client)) {
    return;
  }

  m_manualMovingLeft = false;
  m_manualLeftTask.safeJoin();

  Logger::debug("Manual Stop Left request received");
  Logger::debug(String("X: ") + m_xPos);

  m_webSocket->send(
      String(static_cast<char>(GLOBAL::MSG_TYPE::MANUAL_STOP_LEFT)));
}

void Motors::mStopRightHandler(AsyncWebSocketClient *client,
                               const String & /*msg*/) {
  Logger::debug("Manual Stop Right request received");

  if (!verifyManualClient(client)) {
    return;
  }

  m_manualMovingRight = false;
  m_manualRightTask.safeJoin();

  Logger::debug("Manual Stop Right request received");
  Logger::debug(String("X: ") + m_xPos);

  m_webSocket->send(
      String(static_cast<char>(GLOBAL::MSG_TYPE::MANUAL_STOP_RIGHT)));
}

void Motors::mStopUpperHandler(AsyncWebSocketClient *client,
                               const String & /*msg*/) {
  Logger::debug("Manual Stop Upper request received");

  if (!verifyManualClient(client)) {
    return;
  }

  m_manualMovingUpper = false;
  m_manualUpperTask.safeJoin();

  Logger::debug("Manual Stop Upper request received");
  Logger::debug(String("Y: ") + m_yPos);

  m_webSocket->send(
      String(static_cast<char>(GLOBAL::MSG_TYPE::MANUAL_STOP_UPPER)));
}

void Motors::mStopLowerHandler(AsyncWebSocketClient *client,
                               const String & /*msg*/) {
  Logger::debug("Manual Stop Lower request received");

  if (!verifyManualClient(client)) {
    return;
  }

  m_manualMovingLower = false;
  m_manualLowerTask.safeJoin();

  Logger::debug("Manual Stop Lower request received");
  Logger::debug(String("Y: ") + m_yPos);

  m_webSocket->send(
      String(static_cast<char>(GLOBAL::MSG_TYPE::MANUAL_STOP_LOWER)));
}

void Motors::mStopUpHandler(AsyncWebSocketClient *client,
                            const String & /*msg*/) {
  Logger::debug("Manual Stop Up request received");

  if (!verifyManualClient(client)) {
    return;
  }

  m_manualMovingUp = false;
  m_manualUpTask.safeJoin();

  Logger::debug("Manual Stop Up request received");
  Logger::debug(String("Z: ") + m_zPos);

  m_webSocket->send(
      String(static_cast<char>(GLOBAL::MSG_TYPE::MANUAL_STOP_UP)));
}

void Motors::mStopDownHandler(AsyncWebSocketClient *client,
                              const String & /*msg*/) {
  Logger::debug("Manual Stop Down request received");

  if (!verifyManualClient(client)) {
    return;
  }

  m_manualMovingDown = false;
  m_manualDownTask.safeJoin();

  Logger::debug("Manual Stop Down request received");
  Logger::debug(String("Z: ") + m_zPos);

  m_webSocket->send(
      String(static_cast<char>(GLOBAL::MSG_TYPE::MANUAL_STOP_DOWN)));
}

void Motors::mConfirmHandler(AsyncWebSocketClient *client,
                             const String & /*msg*/) {
  Logger::debug("Manual Confirm request received");

  if (!verifyManualClient(client)) {
    return;
  }

  if (m_calibratingUL.load()) {
    setValues(m_xExtremes.first, m_yExtremes.first, m_xPos, m_yPos);
    m_calibratingUL = false;
    Logger::debug(String("Upper Left: X = ") + *m_xExtremes.first.load() +
                  ", Y = " + *m_yExtremes.first.load());
  } else if (m_calibratingLR.load()) {
    setValues(m_xExtremes.second, m_yExtremes.second, m_xPos, m_yPos);
    m_calibratingLR = false;
    Logger::debug(String("Lower Right: X = ") + *m_xExtremes.second.load() +
                  ", Y = " + *m_yExtremes.second.load());
  } else if (m_calibratingUp.load()) {
    setValue(m_zExtremes.second, m_zPos);
    m_calibratingUp = false;
    Logger::debug(String("Up: Z = ") + *m_zExtremes.second.load());
  } else if (m_calibratingDown.load()) {
    setValue(m_zExtremes.first, m_zPos);
    m_calibratingDown = false;
    Logger::debug(String("Down: Z = ") + *m_zExtremes.first.load());
  } else if (m_calibratingStepStart.load()) {
    setStep(m_xPos);
    m_calibratingStepStart = false;
    m_calibratingStepEnd = true;
    Logger::debug(String("Step start: X = ") + m_xPos.load());
  } else if (m_calibratingStepEnd.load()) {
    setStep(m_xPos);
    m_calibratingStepEnd = false;
    m_stepSet = true;
    Logger::debug(String("Step end: X = ") + m_xPos.load());
    Logger::debug(String("Step Distance: ") + *m_step.load());
  } else {
    Logger::warn("No manual action");
    return;
  }

  checkReady();

  m_webSocket->send(
      String(static_cast<char>(GLOBAL::MSG_TYPE::MANUAL_CONFIRM)));
}

void Motors::infoHandler(AsyncWebSocketClient * /*client*/,
                         const String & /*msg*/) {
  Logger::debug("Info request received");

  m_webSocket->send(makeInfoString());
}

void Motors::timeHandler(AsyncWebSocketClient * /*client*/, const String &msg) {
  Logger::debug("Time request received");

  m_time = GLOBAL::Time(static_cast<GLOBAL::TIME::CHAR>(msg[1]));
  m_camera->setShutterSpeed(m_time.asMs);

  m_webSocket->send(msg);
}

void Motors::movementModeHandler(AsyncWebSocketClient *client,
                                 const String &msg) {
  Logger::debug("Move mode received");

  //todo: make movement happen in auto mode

  using namespace GLOBAL::MSG_DATA_MOVEMENT;

  switch (msg[1]) {
  case DATA_MOVEMENT_SNAP:
    m_movementSmooth = false;
    break;
  case DATA_MOVEMENT_SMOOTH:
    m_movementSmooth = true;
    break;
  default:
    Logger::warn(String("Unknown move type: ") + msg[1]);
    break;
  }

  m_webSocket->send(msg);
}

void Motors::captureModeHandler(AsyncWebSocketClient *client, const String &msg) {
  Logger::debug("Capture mode received");

  //todo: make movement happen in auto mode

  using namespace GLOBAL::MSG_DATA_CAPTURE;

  switch (msg[1]) {
  case DATA_CAPTURE_PHOTO:
    m_captureFilm = false;
    break;
  case DATA_CAPTURE_FILM:
    m_captureFilm = true;
    break;
  default:
    Logger::warn(String("Unknown capture type: ") + msg[1]);
    break;
  }

  m_webSocket->send(msg);
}

void Motors::disconnectHandler(AsyncWebSocketClient *client,
                               const String & /*msg*/) {
  Logger::debug("Disconnect request received");

  if (client->id() == m_manualClientID.load()) {
    m_manualMovingLeft = false;
    m_manualMovingRight = false;
    m_manualMovingUpper = false;
    m_manualMovingLower = false;
    m_manualMovingUp = false;
    m_manualMovingDown = false;
    m_manualClientID = std::nullopt;
  }
}

void Motors::connectHandler(AsyncWebSocketClient * /*client*/,
                            const String & /*msg*/) {
  Logger::debug("Connect request received");

  if (m_ready) {
    m_webSocket->send(String(static_cast<char>(GLOBAL::MSG_TYPE::READY)));
  }
}
