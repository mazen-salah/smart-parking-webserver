#include <Servo.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>

const char *ssid = "Smart Parking";
const char *pass = "m123456789";

WebSocketsServer webSocket = WebSocketsServer(81);

const int IR_PARKING_1_PIN = 4;
const int IR_PARKING_2_PIN = 0;
const int IR_PARKING_3_PIN = 2;
const int IR_PARKING_4_PIN = 14;
const int IR_INPUT_GATE_PIN = 5;
const int IR_EXIT_GATE_PIN = 12;
const int INPUT_GATE_PIN = 16; 
const int EXIT_GATE_PIN = 13;

bool isParking1Occupied = false;
bool isParking2Occupied = false;
bool isParking3Occupied = false;
bool isParking4Occupied = false;
bool isInputGateOpen = false;
bool isExitGateOpen = false;

Servo inputGateServo;
Servo exitGateServo;




void setup() {
  Serial.begin(9600);
  pinMode(IR_INPUT_GATE_PIN, INPUT);
  pinMode(IR_EXIT_GATE_PIN, INPUT);
  pinMode(IR_PARKING_1_PIN, INPUT);
  pinMode(IR_PARKING_2_PIN, INPUT);
  pinMode(IR_PARKING_3_PIN, INPUT);
  pinMode(IR_PARKING_4_PIN, INPUT);
  inputGateServo.attach(INPUT_GATE_PIN);
  inputGateServo.write(180);
  exitGateServo.attach(EXIT_GATE_PIN);
  exitGateServo.write(180);
  IPAddress apIP(192, 168, 0, 1);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, pass);  

  webSocket.begin();  //websocket Begin
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
  handleGate(IR_INPUT_GATE_PIN, INPUT_GATE_PIN, &isInputGateOpen, "G:Entry:");
  handleGate(IR_EXIT_GATE_PIN, EXIT_GATE_PIN, &isExitGateOpen, "G:Exit:");
  handleParkingSensor(IR_PARKING_1_PIN, &isParking1Occupied, "P:1:");
  handleParkingSensor(IR_PARKING_2_PIN, &isParking2Occupied, "P:2:");
  handleParkingSensor(IR_PARKING_3_PIN, &isParking3Occupied, "P:3:");
  handleParkingSensor(IR_PARKING_4_PIN, &isParking4Occupied, "P:4:");
}

void handleGate(int irSensorPin, int gatePin, bool *isOpen, const char *prefix) {
  int irSensorVal = digitalRead(irSensorPin);

  if (!irSensorVal && !(*isOpen)) {
    String message = String(prefix) + "1";
    openGate(gatePin);
    Serial.println(message);
    webSocket.sendTXT(0, message);
    *isOpen = true;
  } else if (irSensorVal && (*isOpen)) {
    String message = String(prefix) + "0";
    closeGate(gatePin);
    Serial.println(message);
    webSocket.sendTXT(0, message);
    *isOpen = false;
  }
}

void handleParkingSensor(int irSensorPin, bool *isOccupied, const char *prefix) {
  int irSensorVal = digitalRead(irSensorPin);

  if (!irSensorVal && !(*isOccupied)) {
    *isOccupied = true;
    String message = String(prefix) + "1";
    Serial.println(message);
    webSocket.sendTXT(0, message);
  } else if (irSensorVal && (*isOccupied)) {
    *isOccupied = false;
    String message = String(prefix) + "0";
    Serial.println(message);
    webSocket.sendTXT(0, message);
  }
}

void openGate(int gatePin) {
  if (gatePin == INPUT_GATE_PIN) {
    inputGateServo.write(0);
  }
  else if (gatePin == EXIT_GATE_PIN) {
    exitGateServo.write(0);
  }
}

void closeGate(int gatePin) {
  if (gatePin == INPUT_GATE_PIN) {
    delay(2000);
    inputGateServo.write(180);
  }
  else if (gatePin == EXIT_GATE_PIN) {
    delay(2000);
    exitGateServo.write(180);
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  String cmd = "";
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connection from ", num);
        Serial.println(ip.toString());
        webSocket.sendTXT(num, "connected");
      }
      break;
    case WStype_TEXT:
      cmd = "";
      for (int i = 0; i < length; i++) {
        cmd = cmd + (char)payload[i];
      }  
      Serial.println(cmd);
      if (cmd == "G:Entry:1") {
        inputGateServo.write(0);
        webSocket.sendTXT(0,"G:Entry:1");
      } else if (cmd == "G:Entry:0") {
        inputGateServo.write(180);
        webSocket.sendTXT(0,"G:Entry:0");
      } 
      else if (cmd == "G:Exit:1") {
        exitGateServo.write(0);
      webSocket.sendTXT(0,"G:Exit:1");
      } else if (cmd == "G:Exit:0") {
        exitGateServo.write(180);
      webSocket.sendTXT(0,"G:Exit:0");
      }

      break;
    case WStype_BIN:
      Serial.printf("[%u] Bin length: %u\n", num, length);
      break;
  }
}