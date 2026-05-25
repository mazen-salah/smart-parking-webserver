#pragma once

#include <Arduino.h>

namespace SmartParking {

// IR-based occupancy detector with debouncing.
// Emits a state change only after the new reading is stable for `debounceMs`.
class ParkingSensor {
 public:
  ParkingSensor(uint8_t pin, uint8_t id, uint16_t debounceMs);

  void begin();

  // Returns true and updates `changed` when occupancy state flips.
  bool update();

  bool     occupied() const { return occupied_; }
  uint8_t  id() const       { return id_; }
  // Milliseconds since the space became occupied, or 0 if vacant.
  uint32_t occupiedSinceMs() const;

 private:
  const uint8_t  pin_;
  const uint8_t  id_;
  const uint16_t debounceMs_;

  bool     occupied_       = false;
  bool     lastRawHigh_    = true;
  uint32_t lastChangeAt_   = 0;
  uint32_t occupiedSince_  = 0;
};

}  // namespace SmartParking
