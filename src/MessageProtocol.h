#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

namespace SmartParking {

enum class GateId : uint8_t { Entry, Exit };

const char* gateIdToString(GateId id);
bool        gateIdFromString(const char* s, GateId& out);

// Encoders — write JSON into the provided buffer; return length written.
size_t encodeHello(char* buf, size_t bufSize);
size_t encodeParkingEvent(char* buf, size_t bufSize, uint8_t spaceId, bool occupied, uint32_t occupiedSinceMs);
size_t encodeGateEvent(char* buf, size_t bufSize, GateId gate, bool open);
size_t encodeSnapshot(char* buf, size_t bufSize,
                      const bool* spaceOccupied,
                      const uint32_t* spaceSinceMs,
                      uint8_t spaceCount,
                      bool entryOpen,
                      bool exitOpen);

// Decoder — parses an incoming client command.
struct ClientCommand {
  enum class Kind { Unknown, GateSet, Refresh };
  Kind   kind = Kind::Unknown;
  GateId gate = GateId::Entry;
  bool   gateOpen = false;
};

bool decodeClientCommand(const char* payload, size_t length, ClientCommand& out);

}  // namespace SmartParking
