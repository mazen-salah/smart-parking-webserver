# WebSocket protocol

The ESP8266 firmware exposes a WebSocket server on `ws://192.168.0.1:81` (default access-point IP). All frames are UTF-8 JSON objects with a required `type` field.

## Server → client

### `hello`
Sent immediately after a client connects, before the initial snapshot.
```json
{ "type": "hello", "status": "connected" }
```

### `snapshot`
Full state of every parking space and gate. Sent on connect and in response to a `refresh` command.
```json
{
  "type": "snapshot",
  "spaces": [
    { "id": 1, "occupied": false, "since_ms": 0 },
    { "id": 2, "occupied": true,  "since_ms": 41230 },
    { "id": 3, "occupied": false, "since_ms": 0 },
    { "id": 4, "occupied": false, "since_ms": 0 }
  ],
  "gates": { "entry": false, "exit": false }
}
```

### `parking`
Emitted when a single space changes state.
```json
{ "type": "parking", "id": 2, "occupied": true, "since_ms": 0 }
```

### `gate`
Emitted whenever a gate opens or closes (automatically or via command).
```json
{ "type": "gate", "gate": "entry", "open": true }
```

`gate` is one of `"entry"` or `"exit"`.

## Client → server

### `gate` — open or close a gate
```json
{ "type": "gate", "gate": "exit", "open": true }
```
The firmware echoes the resulting state back to all clients as a server-side `gate` event.

### `refresh` — request a snapshot
```json
{ "type": "refresh" }
```
The firmware replies with a `snapshot` frame to the requesting client only.

## Notes

- `since_ms` is the duration the space has been occupied, in milliseconds, since the most recent occupancy event. It is `0` when the space is vacant. The client is responsible for rendering it (e.g. as `H:MM:SS`).
- Unknown or malformed frames are ignored by the firmware and logged over serial.
- Auto-close: the firmware closes a gate `GATE_AUTOCLOSE_MS` (default 2000 ms) after its IR beam clears. This is non-blocking; the loop continues serving WebSocket frames in the meantime.
