#include "Camera.h"
#include "Logger.h"
#include <cmath>

void Camera::setShutterSpeed(GLOBAL::TIME::Period shutterSpeed) {
  m_shutterSpeed = shutterSpeed;
}

void Camera::takePictureAndWait() {
  Logger::debug("Shoot pic");

  // TODO TAKE PIC AND WAIT FOR PIC TAKEN RESP FROM CAMERA

  // placeholder
  delay(ceil(m_shutterSpeed * 3));

  Logger::debug("Shoot pic done");
}