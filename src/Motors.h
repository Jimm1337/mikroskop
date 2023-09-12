#ifndef MIKROSKOP_MOTORS_H
#define MIKROSKOP_MOTORS_H

#include <AsyncWebSocket.h>
#include <Stepper.h>
#include <atomic>
#include <mutex>
#include <optional>
#include <tuple>
#include "Global.h"
#include "TrackedTask.h"
#include "WebSocket.h"

class Motors {
  using Extremes = std::pair<
    std::atomic< std::optional< int_fast16_t > >,
    std::atomic< std::optional< int_fast16_t > > >;

  static constexpr auto       SPEED_MANUAL_MS = 30;
  static constexpr auto       SPEED_AUTO_RPM  = 120;
  static constexpr auto       STEP_PER_REV    = 200;
  static constexpr std::tuple X_PINS{ 32, 33, 25, 26 };
  static constexpr std::tuple Y_PINS{ 27, 14, 12, 13 };
  static constexpr std::tuple Z_PINS{ 21, 19, 18, 5 };

  Stepper                                      m_xStepper; // left/right
  Stepper                                      m_yStepper; // upper/lower
  Stepper                                      m_zStepper; // up/down
  WebSocket*                                   m_webSocket;
  std::atomic< std::optional< uint32_t > >     m_manualClientID;
  TrackedTask                                  m_moveTask;
  TrackedTask                                  m_manualLeftTask;
  TrackedTask                                  m_manualRightTask;
  TrackedTask                                  m_manualUpperTask;
  TrackedTask                                  m_manualLowerTask;
  TrackedTask                                  m_manualUpTask;
  TrackedTask                                  m_manualDownTask;
  std::atomic< int_fast16_t >                  m_xPos;
  std::atomic< int_fast16_t >                  m_yPos;
  std::atomic< int_fast16_t >                  m_zPos;
  Extremes                                     m_xExtremes; // left/right
  Extremes                                     m_yExtremes; // upper/lower
  Extremes                                     m_zExtremes; // up/down
  std::atomic< std::optional< int_fast16_t > > m_step;
  GLOBAL::Time                                 m_time;
  mutable std::mutex                           m_mutex;
  std::atomic< bool >                          m_shouldStop;
  std::atomic< bool >                          m_calibratingUL;
  std::atomic< bool >                          m_calibratingLR;
  std::atomic< bool >                          m_calibratingUp;
  std::atomic< bool >                          m_calibratingDown;
  std::atomic< bool >                          m_calibratingStepStart;
  std::atomic< bool >                          m_calibratingStepEnd;
  std::atomic< bool >                          m_manualMovingLeft;
  std::atomic< bool >                          m_manualMovingRight;
  std::atomic< bool >                          m_manualMovingUpper;
  std::atomic< bool >                          m_manualMovingLower;
  std::atomic< bool >                          m_manualMovingUp;
  std::atomic< bool >                          m_manualMovingDown;

public:
  explicit Motors(WebSocket* webSocket);
  ~Motors() = default;

  Motors(const Motors&)            = delete;
  Motors(Motors&&)                 = delete;
  Motors& operator=(const Motors&) = delete;
  Motors& operator=(Motors&&)      = delete;

private:
  void executeManualLeft();
  void executeManualRight();
  void executeManualUpper();
  void executeManualLower();
  void executeManualUp();
  void executeManualDown();

  void moveTo(int_fast16_t xTarget, int_fast16_t yTarget, int_fast16_t zTarget);
  void nextStep();
  [[nodiscard]] String makeInfoString() const;

  String startHandler(AsyncWebSocketClient* client, const String& msg);
  String stopHandler(AsyncWebSocketClient* client, const String& msg);
  String calStartHandler(AsyncWebSocketClient* client, const String& msg);
  String calULHandler(AsyncWebSocketClient* client, const String& msg);
  String calLRHandler(AsyncWebSocketClient* client, const String& msg);
  String calUpHandler(AsyncWebSocketClient* client, const String& msg);
  String calDownHandler(AsyncWebSocketClient* client, const String& msg);
  String calStepHandler(AsyncWebSocketClient* client, const String& msg);
  String calResetULHandler(AsyncWebSocketClient* client, const String& msg);
  String calResetLRHandler(AsyncWebSocketClient* client, const String& msg);
  String calResetUpHandler(AsyncWebSocketClient* client, const String& msg);
  String calResetDownHandler(AsyncWebSocketClient* client, const String& msg);
  String calResetStepHandler(AsyncWebSocketClient* client, const String& msg);
  String mStartLeftHandler(AsyncWebSocketClient* client, const String& msg);
  String mStartRightHandler(AsyncWebSocketClient* client, const String& msg);
  String mStartUpperHandler(AsyncWebSocketClient* client, const String& msg);
  String mStartLowerHandler(AsyncWebSocketClient* client, const String& msg);
  String mStartUpHandler(AsyncWebSocketClient* client, const String& msg);
  String mStartDownHandler(AsyncWebSocketClient* client, const String& msg);
  String mStopLeftHandler(AsyncWebSocketClient* client, const String& msg);
  String mStopRightHandler(AsyncWebSocketClient* client, const String& msg);
  String mStopUpperHandler(AsyncWebSocketClient* client, const String& msg);
  String mStopLowerHandler(AsyncWebSocketClient* client, const String& msg);
  String mStopUpHandler(AsyncWebSocketClient* client, const String& msg);
  String mStopDownHandler(AsyncWebSocketClient* client, const String& msg);
  String mConfirmHandler(AsyncWebSocketClient* client, const String& msg);
  String infoHandler(AsyncWebSocketClient* client, const String& msg);
  String timeHandler(AsyncWebSocketClient* client, const String& msg);
  String disconnectHandler(AsyncWebSocketClient* client, const String& msg);
};

#endif // MIKROSKOP_MOTORS_H
