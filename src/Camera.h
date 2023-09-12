#ifndef MIKROSKOP_CAMERA_H
#define MIKROSKOP_CAMERA_H

#include <Arduino.h>
#include "Global.h"

// TODO: ADD CAMERA WHEN AVAILABLE

class Camera {
  static constexpr auto MS_WAIT_MARGIN = 3;

  GLOBAL::TIME::Period m_shutterSpeed;

public:
  void setShutterSpeed(GLOBAL::TIME::Period shutterSpeed);
  void takePictureAndWait();
};

#endif // MIKROSKOP_CAMERA_H
