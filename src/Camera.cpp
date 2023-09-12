#include "Camera.h"
#include <cmath>

void Camera::setShutterSpeed(GLOBAL::TIME::Period shutterSpeed) {
  m_shutterSpeed = shutterSpeed;
}

void Camera::takePictureAndWait() {
  // TODO TAKE PIC AND WAIT FOR PIC TAKEN RESP FROM CAMERA
  // placeholder
  delay(ceil(m_shutterSpeed));

  delay(MS_WAIT_MARGIN);
}