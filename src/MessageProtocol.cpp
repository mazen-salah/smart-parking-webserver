#include "MessageProtocol.h"

namespace SmartParking {

const char* gateIdToString(GateId id) {
  return id == GateId::Entry ? "entry" : "exit";
}

bool gateIdFromString(const char* s, GateId& out) {
  if (!s) return false;
  if (strcasecmp(s, "entry") == 0) { out = GateId::Entry; return true; }
  if (strcasecmp(s, "exit")  == 0) { out = GateId::Exit;  return true; }
  return false;
}

size_t encodeHello(char* buf, size_t bufSize) {
  JsonDocument doc;
  doc["type"]   = "hello";
  doc["status"] = "connected";
  return serializeJson(doc, buf, bufSize);
}

size_t encodeParkingEvent(char* buf, size_t bufSize, uint8_t spaceId, bool occupied, uint32_t occupiedSinceMs) {
  JsonDocument doc;
  doc["type"]     = "parking";
  doc["id"]       = spaceId;
  doc["occupied"] = occupied;
  doc["since_ms"] = occupiedSinceMs;
  return serializeJson(doc, buf, bufSize);
}

size_t encodeGateEvent(char* buf, size_t bufSize, GateId gate, bool open) {
  JsonDocument doc;
  doc["type"] = "gate";
  doc["gate"] = gateIdToString(gate);
  doc["open"] = open;
  return serializeJson(doc, buf, bufSize);
}

size_t encodeSnapshot(char* buf, size_t bufSize,
                      const bool* spaceOccupied,
                      const uint32_t* spaceSinceMs,
                      uint8_t spaceCount,
                      bool entryOpen,
                      bool exitOpen) {
  JsonDocument doc;
  doc["type"] = "snapshot";

  JsonArray spaces = doc["spaces"].to<JsonArray>();
  for (uint8_t i = 0; i < spaceCount; i++) {
    JsonObject space = spaces.add<JsonObject>();
    space["id"]       = i + 1;
    space["occupied"] = spaceOccupied[i];
    space["since_ms"] = spaceSinceMs[i];
  }

  JsonObject gates = doc["gates"].to<JsonObject>();
  gates["entry"] = entryOpen;
  gates["exit"]  = exitOpen;

  return serializeJson(doc, buf, bufSize);
}

bool decodeClientCommand(const char* payload, size_t length, ClientCommand& out) {
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, payload, length);
  if (err) return false;

  const char* type = doc["type"] | (const char*)nullptr;
  if (!type) return false;

  if (strcasecmp(type, "gate") == 0) {
    const char* gate = doc["gate"] | (const char*)nullptr;
    if (!gateIdFromString(gate, out.gate)) return false;
    if (!doc["open"].is<bool>()) return false;
    out.kind = ClientCommand::Kind::GateSet;
    out.gateOpen = doc["open"].as<bool>();
    return true;
  }

  if (strcasecmp(type, "refresh") == 0) {
    out.kind = ClientCommand::Kind::Refresh;
    return true;
  }

  return false;
}

}  // namespace SmartParking
