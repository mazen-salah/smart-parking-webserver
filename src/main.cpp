#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>

#include "Config.h"
#include "Gate.h"
#include "MessageProtocol.h"
#include "ParkingSensor.h"

using namespace SmartParking;

namespace {

constexpr size_t MESSAGE_BUFFER_SIZE = 384;

WebSocketsServer webSocket(WEBSOCKET_PORT);

ParkingSensor sensors[PARKING_SPACE_COUNT] = {
    {PIN_PARKING_1_IR, 1, SENSOR_DEBOUNCE_MS},
    {PIN_PARKING_2_IR, 2, SENSOR_DEBOUNCE_MS},
    {PIN_PARKING_3_IR, 3, SENSOR_DEBOUNCE_MS},
    {PIN_PARKING_4_IR, 4, SENSOR_DEBOUNCE_MS},
};

Gate entryGate(GateId::Entry, PIN_GATE_ENTRY_IR, PIN_GATE_ENTRY_SERVO, GATE_AUTOCLOSE_MS);
Gate exitGate (GateId::Exit,  PIN_GATE_EXIT_IR,  PIN_GATE_EXIT_SERVO,  GATE_AUTOCLOSE_MS);

void broadcast(const char* json, size_t len) {
  webSocket.broadcastTXT(json, len);
}

void sendSnapshot(uint8_t clientNum) {
  bool     occ[PARKING_SPACE_COUNT];
  uint32_t since[PARKING_SPACE_COUNT];
  for (uint8_t i = 0; i < PARKING_SPACE_COUNT; i++) {
    occ[i]   = sensors[i].occupied();
    since[i] = sensors[i].occupiedSinceMs();
  }
  char   buf[MESSAGE_BUFFER_SIZE];
  size_t len = encodeSnapshot(buf, sizeof(buf), occ, since, PARKING_SPACE_COUNT,
                              entryGate.isOpen(), exitGate.isOpen());
  webSocket.sendTXT(clientNum, buf, len);
}

void publishParking(uint8_t index) {
  char   buf[MESSAGE_BUFFER_SIZE];
  size_t len = encodeParkingEvent(buf, sizeof(buf),
                                  sensors[index].id(),
                                  sensors[index].occupied(),
                                  sensors[index].occupiedSinceMs());
  broadcast(buf, len);
}

void publishGate(const Gate& gate) {
  char   buf[MESSAGE_BUFFER_SIZE];
  size_t len = encodeGateEvent(buf, sizeof(buf), gate.id(), gate.isOpen());
  broadcast(buf, len);
}

void handleWebSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[ws %u] disconnected\n", num);
      break;

    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[ws %u] connected from %s\n", num, ip.toString().c_str());
      char   buf[MESSAGE_BUFFER_SIZE];
      size_t len = encodeHello(buf, sizeof(buf));
      webSocket.sendTXT(num, buf, len);
      sendSnapshot(num);
      break;
    }

    case WStype_TEXT: {
      ClientCommand cmd;
      if (!decodeClientCommand(reinterpret_cast<const char*>(payload), length, cmd)) {
        Serial.printf("[ws %u] unparseable: %.*s\n", num, (int)length, payload);
        return;
      }
      switch (cmd.kind) {
        case ClientCommand::Kind::GateSet: {
          Gate& target = cmd.gate == GateId::Entry ? entryGate : exitGate;
          target.setOpen(cmd.gateOpen);
          publishGate(target);
          break;
        }
        case ClientCommand::Kind::Refresh:
          sendSnapshot(num);
          break;
        default:
          break;
      }
      break;
    }

    default:
      break;
  }
}

void startAccessPoint() {
  IPAddress ip(AP_IP[0], AP_IP[1], AP_IP[2], AP_IP[3]);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(ip, ip, IPAddress(255, 255, 255, 0));
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  Serial.print(F("[wifi] AP up at "));
  Serial.println(WiFi.softAPIP());
}

}  // namespace

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(50);
  Serial.println();
  Serial.println(F("[boot] Smart Parking firmware"));

  for (auto& s : sensors) s.begin();
  entryGate.begin();
  exitGate.begin();

  startAccessPoint();

  webSocket.begin();
  webSocket.onEvent(handleWebSocketEvent);
  Serial.printf("[ws] listening on :%u\n", WEBSOCKET_PORT);
}

void loop() {
  webSocket.loop();

  for (uint8_t i = 0; i < PARKING_SPACE_COUNT; i++) {
    if (sensors[i].update()) publishParking(i);
  }
  if (entryGate.update()) publishGate(entryGate);
  if (exitGate.update())  publishGate(exitGate);
}
