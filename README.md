# Smart Parking — ESP8266 firmware

PlatformIO firmware for an ESP8266-based smart parking installation. Exposes a JSON-over-WebSocket API for monitoring four parking bays and controlling entry/exit gates. Pairs with the companion [Smart Parking App](https://github.com/mazen-salah/Smart-Parking-App).

## Architecture

```
src/
├── main.cpp              # setup / loop, WebSocket event routing
├── Config.h              # Wi-Fi, pin map, timing constants
├── Gate.h / Gate.cpp     # Servo gate with non-blocking auto-close
├── ParkingSensor.h/.cpp  # IR occupancy detector with debounce
└── MessageProtocol.h/.cpp# JSON encoders + client-command decoder
docs/
└── PROTOCOL.md           # Wire-format reference
```

Each subsystem is a small class with a single responsibility. `main.cpp` only wires them together — no business logic.

## Hardware

| Component        | NodeMCU pin | GPIO |
|------------------|-------------|------|
| Parking 1 IR     | D2          | 4    |
| Parking 2 IR     | D3          | 0    |
| Parking 3 IR     | D4          | 2    |
| Parking 4 IR     | D5          | 14   |
| Entry gate IR    | D1          | 5    |
| Exit gate IR     | D6          | 12   |
| Entry gate servo | D0          | 16   |
| Exit gate servo  | D7          | 13   |

Pins are defined in [`src/Config.h`](src/Config.h); change them there.

## Build & flash

This project uses [PlatformIO](https://platformio.org). Install the PlatformIO CLI or the VS Code extension, then:

```bash
# Build
pio run

# Flash over USB
pio run -t upload

# Open the serial monitor (115200 baud)
pio device monitor
```

PlatformIO will auto-install the three required libraries the first time you build:

- `links2004/WebSockets` (^2.4.1)
- `bblanchon/ArduinoJson` (^7.0.4)
- `arduino-libraries/Servo` (^1.2.2)

## Wi-Fi

The firmware starts a soft access point:

| Setting | Default |
|---------|---------|
| SSID    | `Smart Parking` |
| Password| `m123456789` |
| IP      | `192.168.0.1` |
| WS port | `81` |

Override the defaults by editing `AP_SSID`, `AP_PASSWORD`, `AP_IP` in `src/Config.h`.

## Protocol

See [`docs/PROTOCOL.md`](docs/PROTOCOL.md) for the full JSON message contract used by both this firmware and the companion app.

## Demo

[![Demo Video](https://img.youtube.com/vi/odxMqw8v5l0/0.jpg)](https://www.youtube.com/watch?v=odxMqw8v5l0)

## License

MIT — see [LICENSE](LICENSE).
