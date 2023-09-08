#include "Motors.h"
#include "Logger.h"

Motors::Motors(WebSocket* webSocket):
  m_xPos(0),
  m_yPos(0),
  m_zPos(0),
  m_moving(false),
  m_stop(false),
  m_webSocket(webSocket),
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
    std::get< 3 >(Z_PINS)) {
  m_webSocket->attachListener(
    Global::CHAR_MOVE, [this](AsyncWebSocketClient* client, String msg) {
      moveHandler(client, msg);
    });
  m_webSocket->attachListener(
    Global::CHAR_STOP, [this](AsyncWebSocketClient* client, String msg) {
      stopHandler(client, msg);
    });
  m_webSocket->attachListener(
    Global::CHAR_INFO, [this](AsyncWebSocketClient* client, String msg) {
      infoHandler(client, msg);
    });
}

Motors::~Motors() {
  m_moving = false;
}

Global::PosTupleXYZ Motors::getPos() const noexcept {
  return { m_xPos, m_yPos, m_zPos };
}

void Motors::moveHandler(AsyncWebSocketClient* client, const String& msg) {
  // Stop any current movement (from other threads)
  if (m_moving) { m_stop = true; }

  // wait for other threads to clear stop flag
  while (m_stop) { delay(1); }

  const auto xIndex = msg.indexOf('x');
  const auto yIndex = msg.indexOf('y');
  const auto zIndex = msg.indexOf('z');

  const auto speed = msg.substring(1, xIndex).toInt();

  if (speed <= 0)
  {
    Logger::warn("Speed must be greater than 0"); // Handle this in the future
    return;
  }

  // Parse message s()x()-()y()-()z()-() <- FORMAT
  const auto xString = msg.substring(xIndex + 1, yIndex);
  const auto yString = msg.substring(yIndex + 1, zIndex);
  const auto zString = msg.substring(zIndex + 1);
  const auto xBegin  = xString.substring(0, xString.indexOf('-')).toInt();
  const auto xEnd    = xString.substring(xString.indexOf('-') + 1).toInt();
  const auto yBegin  = yString.substring(0, yString.indexOf('-')).toInt();
  const auto yEnd    = yString.substring(yString.indexOf('-') + 1).toInt();
  const auto zBegin  = zString.substring(0, zString.indexOf('-')).toInt();
  const auto zEnd    = zString.substring(zString.indexOf('-') + 1).toInt();

  // Calculate steps
  const auto xSteps = xEnd - xBegin;
  const auto ySteps = yEnd - yBegin;
  const auto zSteps = zEnd - zBegin;
  const auto xReset = xBegin - m_xPos;
  const auto yReset = yBegin - m_yPos;
  const auto zReset = zBegin - m_zPos;

  Logger::debug(
    String("Move: ") + "x: " + xBegin + " -> " + xEnd + " (" + xSteps + "), " +
    "y: " + yBegin + " -> " + yEnd + " (" + ySteps + "), " + "z: " + zBegin +
    " -> " + zEnd + " (" + zSteps + "), " + "speed: " + speed);

  Logger::debug(
    "Current pos: " + String(m_xPos) + ", " + String(m_yPos) + ", " +
    String(m_zPos));

  // set moving flag
  m_moving = true;

  // Reset to beginning
  if (!m_stop) { step(xReset, yReset, zReset, RESET_SPEED); }
  if (!m_stop) { Logger::debug("Finished Reset"); }

  // Wait after reset (if there was a reset)
  if (!m_stop)
  {
    if (xReset != 0 || yReset != 0 || zReset != 0)
    { delay(RESET_MOVE_DELAY_MS); }
  }
  if (!m_stop) { Logger::debug("Finished Delay"); }

  // Move to end
  if (!m_stop) { step(xSteps, ySteps, zSteps, speed); }
  if (!m_stop) { Logger::debug("Finished Move"); }

  // reset flags
  m_moving = false;
  m_stop   = false;
}

void Motors::stopHandler(
  AsyncWebSocketClient* /*unused*/, const String& /*unused*/) {
  if (m_moving) { m_stop = true; }

  Logger::debug("Stop");
  Logger::debug(
    "Current pos: " + String(m_xPos) + ", " + String(m_yPos) + ", " +
    String(m_zPos));
}

void Motors::infoHandler(
  AsyncWebSocketClient* /*unused*/, const String& /*unused*/) {
  m_webSocket->sendPos(getPos());
}

void Motors::step(
  int_fast16_t xSteps,
  int_fast16_t ySteps,
  int_fast16_t zSteps,
  int_fast16_t delayMS) {
  while (!m_stop && (xSteps != 0 || ySteps != 0 || zSteps != 0))
  {
    Logger::debug(
      "Motor loop: " + String(xSteps) + ", " + String(ySteps) + ", " +
      String(zSteps));

    if (xSteps > 0)
    {
      m_xStepper.step(1);
      --xSteps;
      ++m_xPos;
    } else if (xSteps < 0) {
      m_xStepper.step(-1);
      ++xSteps;
      --m_xPos;
    }

    if (ySteps > 0)
    {
      m_yStepper.step(1);
      --ySteps;
      ++m_yPos;
    } else if (ySteps < 0) {
      m_yStepper.step(-1);
      ++ySteps;
      --m_yPos;
    }

    if (zSteps > 0)
    {
      m_zStepper.step(1);
      --zSteps;
      ++m_zPos;
    } else if (zSteps < 0) {
      m_zStepper.step(-1);
      ++zSteps;
      --m_zPos;
    }

    delay(delayMS);
  }
}
