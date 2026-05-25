#pragma once

#include <Arduino.h>
#include <Servo.h>

#include "MessageProtocol.h"

namespace SmartParking {

// Drives a servo-controlled gate and auto-closes after an IR sensor clears.
// Non-blocking — call update() from loop().
class Gate {
 public:
  Gate(GateId id, uint8_t irPin, uint8_t servoPin, uint16_t autoCloseMs);

  void begin();

  // Returns true when the gate's open/closed state flips during this call.
  bool update();

  // Manual command (from a WebSocket client).
  void setOpen(bool open);

  bool   isOpen() const { return open_; }
  GateId id() const     { return id_; }

 private:
  void writeServo();

  const GateId   id_;
  const uint8_t  irPin_;
  const uint8_t  servoPin_;
  const uint16_t autoCloseMs_;

  Servo    servo_;
  bool     open_              = false;
  bool     vehicleSeen_       = false;
  uint32_t vehicleClearedAt_  = 0;
};

}  // namespace SmartParking
