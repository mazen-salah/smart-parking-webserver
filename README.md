## ESP8266 Smart Parking Websocket Server

This project implements a Smart Parking System using an ESP8266 microcontroller, infrared sensors, and servos. It allows monitoring parking spaces, controlling entry and exit gates, and communicating status updates via WebSocket.

### Hardware Requirements
- ESP8266 microcontroller
- Infrared sensors
- Servos
- Power supply
- Gates (for entry and exit)

### Software Dependencies
- `Servo.h`: Arduino Servo library for controlling servos.
- `ESP8266WiFi.h`: ESP8266WiFi library for connecting to WiFi networks.
- `WebSocketsServer.h`: WebSocketsServer library for WebSocket communication.

### Setup
1. Connect the hardware components as per the provided schematic.
2. Upload the provided code to the ESP8266 microcontroller.
3. Ensure the infrared sensors are appropriately positioned to detect vehicle presence.
4. Adjust the gate servo positions if necessary for proper opening and closing.

### Configuration
- Set the WiFi network credentials in the code:
  ```cpp
  const char *ssid = "Smart Parking";
  const char *pass = "m123456789";
  ```
- Ensure correct pin assignments for IR sensors, servos, and gate control pins.

### Operation
- The system monitors parking spaces and gate entrances/exits continuously.
- WebSocket communication allows real-time updates on parking space occupancy and gate status.
- The system automatically opens gates upon vehicle detection and closes them after a delay.

### Flutter Application
- A companion Flutter application is available for monitoring and controlling the Smart Parking System.
- Repository link: [Smart Parking App](https://github.com/mazen-salah/Smart-Parking-App)

### Demo Video
- See the Smart Parking System in action: [Demo Video](https://www.youtube.com/watch?v=odxMqw8v5l0)

## Contributing

Contributions are welcome! If you have any suggestions, improvements, or bug fixes, feel free to open an issue or submit a pull request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
