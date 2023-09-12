#ifndef MIKROSKOP_MOTORS_H
#define MIKROSKOP_MOTORS_H

#include <AsyncWebSocket.h>
#include <Stepper.h>
#include <atomic>
#include <mutex>
#include <optional>
#include <tuple>
#include "Camera.h"
#include "Global.h"
#include "TrackedTask.h"
#include "WebSocket.h"

class Motors {
  enum class Direction {
    X,
    Y,
    Z
  };

  using Extremes = std::pair<
    std::atomic< std::optional< int_fast16_t > >,
    std::atomic< std::optional< int_fast16_t > > >;

  static constexpr auto       SPEED_MANUAL_MS = 30;
  static constexpr auto       SPEED_AUTO_RPM  = 120;
  static constexpr auto       STEP_PER_REV    = 200;
  static constexpr std::tuple X_PINS{ 32, 33, 25, 26 };
  static constexpr std::tuple Y_PINS{ 27, 14, 12, 13 };
  static constexpr std::tuple Z_PINS{ 21, 19, 18, 5 };
  static constexpr auto       READY_COUNT = 6;
  static constexpr auto DEFAULT_SHUTTER_SPEED = GLOBAL::TIME::MS::TIME_1_125;

  Stepper                                      m_xStepper; // left/right
  Stepper                                      m_yStepper; // upper/lower
  Stepper                                      m_zStepper; // up/down
  WebSocket*                                   m_webSocket;
  Camera*                                      m_camera;
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
  std::atomic< uint_fast8_t >                  m_readyCount;
  std::atomic< bool >                          m_ready;
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
  std::atomic< bool >                          m_stepSet;
  std::atomic< bool >                          m_nextStepLeft;

public:
  Motors(WebSocket* webSocket, Camera* camera);
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

  void                 moveTo(int_fast16_t target, Direction direction);
  bool                 nextStep();
  bool nextHeight();
  void resetHeight();
  void moveTask();
  [[nodiscard]] String makeInfoString() const;
  [[nodiscard]] bool   verifyManualClient(AsyncWebSocketClient* client) const;
  void                 checkReady();
  void                 checkNotReady();
  void resetValue(std::atomic< std::optional< int_fast16_t > >& value);
  void resetValues(
    std::atomic< std::optional< int_fast16_t > >& value1,
    std::atomic< std::optional< int_fast16_t > >& value2);
  void resetStep();
  void setValue(
    std::atomic< std::optional< int_fast16_t > >& ref, int_fast16_t newValue);
  void setValues(
    std::atomic< std::optional< int_fast16_t > >& ref1,
    std::atomic< std::optional< int_fast16_t > >& ref2,
    int_fast16_t                                  newValue1,
    int_fast16_t                                  newValue2);
  void setStep(int_fast16_t newValue);

  void startHandler(AsyncWebSocketClient* client, const String& msg);
  void stopHandler(AsyncWebSocketClient* client, const String& msg);
  void calStartHandler(AsyncWebSocketClient* client, const String& msg);
  void calULHandler(AsyncWebSocketClient* client, const String& msg);
  void calLRHandler(AsyncWebSocketClient* client, const String& msg);
  void calUpHandler(AsyncWebSocketClient* client, const String& msg);
  void calDownHandler(AsyncWebSocketClient* client, const String& msg);
  void calStepHandler(AsyncWebSocketClient* client, const String& msg);
  void calResetULHandler(AsyncWebSocketClient* client, const String& msg);
  void calResetLRHandler(AsyncWebSocketClient* client, const String& msg);
  void calResetUpHandler(AsyncWebSocketClient* client, const String& msg);
  void calResetDownHandler(AsyncWebSocketClient* client, const String& msg);
  void calResetStepHandler(AsyncWebSocketClient* client, const String& msg);
  void mStartLeftHandler(AsyncWebSocketClient* client, const String& msg);
  void mStartRightHandler(AsyncWebSocketClient* client, const String& msg);
  void mStartUpperHandler(AsyncWebSocketClient* client, const String& msg);
  void mStartLowerHandler(AsyncWebSocketClient* client, const String& msg);
  void mStartUpHandler(AsyncWebSocketClient* client, const String& msg);
  void mStartDownHandler(AsyncWebSocketClient* client, const String& msg);
  void mStopLeftHandler(AsyncWebSocketClient* client, const String& msg);
  void mStopRightHandler(AsyncWebSocketClient* client, const String& msg);
  void mStopUpperHandler(AsyncWebSocketClient* client, const String& msg);
  void mStopLowerHandler(AsyncWebSocketClient* client, const String& msg);
  void mStopUpHandler(AsyncWebSocketClient* client, const String& msg);
  void mStopDownHandler(AsyncWebSocketClient* client, const String& msg);
  void mConfirmHandler(AsyncWebSocketClient* client, const String& msg);
  void infoHandler(AsyncWebSocketClient* client, const String& msg);
  void timeHandler(AsyncWebSocketClient* client, const String& msg);
  void disconnectHandler(AsyncWebSocketClient* client, const String& msg);
  void connectHandler(AsyncWebSocketClient* client, const String& msg);
};

#endif // MIKROSKOP_MOTORS_H
