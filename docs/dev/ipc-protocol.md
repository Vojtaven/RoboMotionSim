# IPC Protocol Reference

## Overview

The IPC subsystem enables external programs to control the simulator over a network using ZeroMQ (ZMQ) sockets. This is the primary integration point for connecting custom controllers, algorithms, or hardware abstraction layers.

## Socket Architecture

| Socket          | ZMQ Type | Direction        | Purpose                                       |
|-----------------|----------|------------------|-----------------------------------------------|
| Command/Response| ROUTER   | Bidirectional    | Receive commands from client, send ACKs/responses back |
| Telemetry       | PUB      | Outbound (Sim →) | Stream odometry data (`conflate=1`, keeps only latest) |

Both sockets bind to configurable addresses and ports (set in app configuration). Only one client can be connected at a time.

## Message Format

All messages use a packed binary header:

```c
struct MsgHeader {
    uint32_t id;            // sequential message ID (starting at 1)
    uint16_t payload_size;  // bytes following header
    MsgType  type;          // uint8_t envelope type
};
```

## Message Types

| Type (hex)           | Direction    | Description                            |
|----------------------|-------------|----------------------------------------|
| `HANDSHAKE` (0x01)       | Client → Sim | Initiate connection                   |
| `HANDSHAKE_ACK` (0x02)   | Sim → Client | Acknowledge connection                |
| `HEARTBEAT` (0x03)       | Client → Sim | Keep connection alive                 |
| `HEARTBEAT_ACK` (0x04)   | Sim → Client | Respond to heartbeat                  |
| `MOTOR_COUNT` (0x05)     | Sim → Client | Inform client of robot's wheel count  |
| `TELEMETRY` (0x06)       | Sim → Client | Broadcast odometry data               |
| `COMMAND` (0x07)         | Client → Sim | Send a movement command               |
| `CMD_ACK` (0x08)         | Sim → Client | Command received and queued           |
| `CMD_ERROR` (0x09)       | Sim → Client | Command rejected (invalid params)     |
| `CMD_COMPLETE` (0x0A)    | Sim → Client | Command execution finished            |
| `CLEAR_COMMAND_QUEUE` (0x0B) | Client → Sim | Flush all pending commands        |
| `DISCONNECT_ACK` (0xFE)  | Sim → Client | Confirm disconnect                    |
| `DISCONNECT` (0xFF)      | Client → Sim | Request graceful disconnect           |

## Connection Lifecycle

```
Client                          Simulator
  │                                 │
  │──── HANDSHAKE ────────────────►│
  │◄─── HANDSHAKE_ACK ────────────│
  │◄─── MOTOR_COUNT ──────────────│  (sim tells client how many wheels)
  │                                 │
  │──── COMMAND ──────────────────►│
  │◄─── CMD_ACK ──────────────────│
  │◄─── CMD_COMPLETE ─────────────│  (when time/distance/angle target reached)
  │                                 │
  │◄─── TELEMETRY (continuous) ───│  (on PUB socket)
  │                                 │
  │──── HEARTBEAT ────────────────►│
  │◄─── HEARTBEAT_ACK ────────────│
  │                                 │
  │──── DISCONNECT ───────────────►│
  │◄─── DISCONNECT_ACK ───────────│
```

If no heartbeat is received within the configured timeout (default 5 seconds), the simulator considers the connection lost and resets.

## Command Types

Commands are sent as `COMMAND` messages. The payload begins with a `uint16_t` CommandType, followed by command-specific parameters.

### Priority Commands (skip the queue)

| Command             | Type   | Parameters           | Description                          |
|---------------------|--------|----------------------|--------------------------------------|
| `STOP`              | 0x04   | (none)               | Cancel current command, stop all motors immediately |
| `STOP_MOTOR`        | 0x08   | `uint16_t motor_id`  | Stop a specific motor in the active wrapper |

### Raw Movement (direct velocity control)

| Command               | Type   | Parameters                                           | Description                     |
|------------------------|--------|------------------------------------------------------|---------------------------------|
| `MOVE_BY_DISTANCE_RAW` | 0x01  | `float distance_mm, x_speed, y_speed, rotation_speed, front_rotation_speed` | Move until distance reached |
| `MOVE_BY_TIME_RAW`     | 0x02  | `float time_s, x_speed, y_speed, rotation_speed, front_rotation_speed` | Move for a duration |
| `MOVE_AT_SPEED_RAW`    | 0x03  | `float x_speed, y_speed, rotation_speed, front_rotation_speed` | Move continuously |
| `MOVE_BY_ANGLE_RAW`    | 0x0E  | `float angle_rad, x_speed, y_speed, rotation_speed, front_rotation_speed` | Move until angle reached |

### High-Level Movement (with pivot point)

These commands accept a rotation center offset and a `rotate_chassis` flag. The simulator internally converts them to raw velocities using a pivot-point transformation.

| Command             | Type   | Parameters                                                             |
|---------------------|--------|------------------------------------------------------------------------|
| `MOVE_BY_DISTANCE`  | 0x0B   | `float distance_mm, x_speed, y_speed, rotation_speed, center_x_mm, center_y_mm, bool rotate_chassis` |
| `MOVE_BY_TIME`      | 0x0A   | `float time_s, x_speed, y_speed, rotation_speed, center_x_mm, center_y_mm, bool rotate_chassis` |
| `MOVE_AT_SPEED`     | 0x0C   | `float x_speed, y_speed, rotation_speed, center_x_mm, center_y_mm, bool rotate_chassis` |
| `MOVE_BY_ANGLE`     | 0x0D   | `float x_speed, y_speed, angle_rad, rotation_speed, center_x_mm, center_y_mm, bool rotate_chassis` |

When `rotate_chassis` is false, the front direction counter-rotates to stay pointing the same way while the base rotates.

### Single Motor Control

| Command                | Type   | Parameters                                | Description                |
|------------------------|--------|-------------------------------------------|----------------------------|
| `START_MOTOR`          | 0x09   | `uint16_t motor_id, float speed`          | Run a motor continuously   |
| `RUN_MOTOR_FOR_TIME`   | 0x06   | `uint16_t motor_id, float speed, float time_s` | Run for a duration    |
| `RUN_MOTOR_FOR_DISTANCE`| 0x07  | `uint16_t motor_id, float speed, float distance_mm` | Run until distance |

Consecutive single-motor commands are automatically merged into a `MotorCommandWrapper` so they execute in parallel.

### Multi-Motor Control

| Command               | Type   | Parameters                                  | Description                 |
|------------------------|--------|---------------------------------------------|-----------------------------|
| `MOVE_AT_SPEED_MOTORS` | 0x05  | `uint16_t motor_count, float[motor_count] speeds` | Set all motor speeds directly |

### Queue Management

| Command               | Type   | Description                       |
|------------------------|--------|-----------------------------------|
| `CLEAR_COMMAND_QUEUE`  | 0x0B   | Discard all queued commands       |

## Telemetry Data

The simulator broadcasts telemetry on the PUB socket every frame. The telemetry payload uses packed structs:

```c
struct TelemetryOdometry {
    uint64_t timestamp_ms;
    float distanceTraveledX;        // mm
    float distanceTraveledY;        // mm
    float localVelocityX;           // mm/s
    float localVelocityY;           // mm/s
    float frontAngle;               // rad
    float chassisAngle;             // rad
    float chassisAngularVelocity;   // rad/s
    uint8_t wheelCount;             // number of TelemetryWheelState following
};

struct TelemetryWheelState {
    float speed;            // mm/s
    float distanceTraveled; // mm
};
```

## Configuration

IPC settings are stored in the app configuration JSON:

```json
"ipcMapping": {
    "address": "tcp://127.0.0.1",
    "command_port": 5555,
    "telemetry_port": 5556,
    "heartbeatTimeout": 5.0
}
```

These can also be changed at runtime in the Input Settings window. Changing settings triggers socket rebinding.
