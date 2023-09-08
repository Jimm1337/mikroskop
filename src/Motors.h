#ifndef MIKROSKOP_MOTORS_H
#define MIKROSKOP_MOTORS_H

#include <AsyncWebSocket.h>
#include <Stepper.h>
#include <atomic>
#include <tuple>
#include "Global.h"
#include "WebSocket.h"

class Motors {
  static constexpr auto       RESET_SPEED         = 50;
  static constexpr auto       STEP_PER_REV        = 200;
  static constexpr auto       RESET_MOVE_DELAY_MS = 1000;
  static constexpr std::tuple X_PINS{ 32, 33, 25, 26 };
  static constexpr std::tuple Y_PINS{ 27, 14, 12, 13 };
  static constexpr std::tuple Z_PINS{ 21, 19, 18, 5 };

  std::atomic< int_fast16_t > m_xPos;
  std::atomic< int_fast16_t > m_yPos;
  std::atomic< int_fast16_t > m_zPos;
  std::atomic< bool >         m_moving;
  std::atomic< bool >         m_stop;
  Stepper                     m_xStepper;
  Stepper                     m_yStepper;
  Stepper                     m_zStepper;
  WebSocket*                  m_webSocket;

public:
  explicit Motors(WebSocket* webSocket);
  ~Motors();

  Motors(const Motors&)            = delete;
  Motors(Motors&&)                 = delete;
  Motors& operator=(const Motors&) = delete;
  Motors& operator=(Motors&&)      = delete;

  [[nodiscard]] Global::PosTupleXYZ getPos() const noexcept;

private:
  void moveHandler(AsyncWebSocketClient* client, const String& msg);
  void stopHandler(AsyncWebSocketClient*, const String&);
  void infoHandler(AsyncWebSocketClient*, const String&);

  void step(
    int_fast16_t xSteps,
    int_fast16_t ySteps,
    int_fast16_t zSteps,
    int_fast16_t delayMS); // MS delay between steps
};

#endif // MIKROSKOP_MOTORS_H
