#pragma once

#include <Arduino.h>

namespace SmartParking {

// ---------- Wi-Fi access point ----------
constexpr const char* AP_SSID     = "Smart Parking";
constexpr const char* AP_PASSWORD = "m123456789";
constexpr uint8_t     AP_IP[4]    = {192, 168, 0, 1};

// ---------- Network ----------
constexpr uint16_t WEBSOCKET_PORT = 81;
constexpr uint32_t SERIAL_BAUD    = 115200;

// ---------- GPIO pin map (NodeMCU labels in comments) ----------
constexpr uint8_t PIN_PARKING_1_IR = 4;   // D2
constexpr uint8_t PIN_PARKING_2_IR = 0;   // D3
constexpr uint8_t PIN_PARKING_3_IR = 2;   // D4
constexpr uint8_t PIN_PARKING_4_IR = 14;  // D5

constexpr uint8_t PIN_GATE_ENTRY_IR    = 5;   // D1
constexpr uint8_t PIN_GATE_EXIT_IR     = 12;  // D6
constexpr uint8_t PIN_GATE_ENTRY_SERVO = 16;  // D0
constexpr uint8_t PIN_GATE_EXIT_SERVO  = 13;  // D7

// ---------- Servo angles ----------
constexpr uint8_t SERVO_ANGLE_OPEN   = 0;
constexpr uint8_t SERVO_ANGLE_CLOSED = 180;

// ---------- Timing (milliseconds) ----------
constexpr uint32_t GATE_AUTOCLOSE_MS = 2000;  // hold open after IR clears
constexpr uint32_t SENSOR_DEBOUNCE_MS = 50;

// ---------- Number of parking spaces ----------
constexpr uint8_t PARKING_SPACE_COUNT = 4;

}  // namespace SmartParking
