#include "Gate.h"

#include "Config.h"

namespace SmartParking {

Gate::Gate(GateId id, uint8_t irPin, uint8_t servoPin, uint16_t autoCloseMs)
    : id_(id), irPin_(irPin), servoPin_(servoPin), autoCloseMs_(autoCloseMs) {}

void Gate::begin() {
  pinMode(irPin_, INPUT);
  servo_.attach(servoPin_);
  open_ = false;
  writeServo();
}

bool Gate::update() {
  const bool     vehiclePresent = digitalRead(irPin_) == LOW;
  const uint32_t now            = millis();

  if (vehiclePresent) {
    vehicleSeen_      = true;
    vehicleClearedAt_ = 0;
    if (!open_) {
      open_ = true;
      writeServo();
      return true;
    }
    return false;
  }

  // Vehicle no longer detected — schedule auto-close.
  if (open_ && vehicleSeen_) {
    if (vehicleClearedAt_ == 0) {
      vehicleClearedAt_ = now;
      return false;
    }
    if (now - vehicleClearedAt_ >= autoCloseMs_) {
      open_             = false;
      vehicleSeen_      = false;
      vehicleClearedAt_ = 0;
      writeServo();
      return true;
    }
  }
  return false;
}

void Gate::setOpen(bool open) {
  if (open == open_) return;
  open_             = open;
  vehicleSeen_      = false;
  vehicleClearedAt_ = open ? 0 : millis();
  writeServo();
}

void Gate::writeServo() {
  servo_.write(open_ ? SERVO_ANGLE_OPEN : SERVO_ANGLE_CLOSED);
}

}  // namespace SmartParking
