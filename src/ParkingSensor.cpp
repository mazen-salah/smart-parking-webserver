#include "ParkingSensor.h"

namespace SmartParking {

ParkingSensor::ParkingSensor(uint8_t pin, uint8_t id, uint16_t debounceMs)
    : pin_(pin), id_(id), debounceMs_(debounceMs) {}

void ParkingSensor::begin() {
  pinMode(pin_, INPUT);
  lastRawHigh_  = digitalRead(pin_) == HIGH;
  occupied_     = !lastRawHigh_;
  lastChangeAt_ = millis();
  if (occupied_) occupiedSince_ = lastChangeAt_;
}

bool ParkingSensor::update() {
  const bool     rawHigh = digitalRead(pin_) == HIGH;
  const uint32_t now     = millis();

  if (rawHigh != lastRawHigh_) {
    lastRawHigh_  = rawHigh;
    lastChangeAt_ = now;
    return false;
  }

  // Stable reading — apply if it differs from current logical state.
  const bool wouldBeOccupied = !rawHigh;
  if (wouldBeOccupied == occupied_) return false;
  if (now - lastChangeAt_ < debounceMs_) return false;

  occupied_      = wouldBeOccupied;
  occupiedSince_ = occupied_ ? now : 0;
  return true;
}

uint32_t ParkingSensor::occupiedSinceMs() const {
  if (!occupied_ || occupiedSince_ == 0) return 0;
  return millis() - occupiedSince_;
}

}  // namespace SmartParking
