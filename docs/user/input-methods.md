# Input Methods

RoboMotion Simulator supports four input methods. You can switch between them in the **Input Settings** window (accessible from the main menu).

## Keyboard

The default input method. Keys map directly to robot movement directions.

**Default bindings:**

| Action              | Key |
|---------------------|-----|
| Move forward        | W   |
| Move backward       | S   |
| Strafe left         | A   |
| Strafe right        | D   |
| Rotate chassis left | Q   |
| Rotate chassis right| E   |
| Rotate front left   | J   |
| Rotate front right  | K   |

Key bindings can be customized in the Input Settings window by clicking a binding button and pressing the desired key. Press Escape to cancel.

**Speed parameters:**
- **Max speed** (mm/s): Maximum linear velocity (configurable slider, 1–2000).
- **Max rotation speed** (deg/s): Maximum angular velocity (configurable slider, 1–360).
- **Register input without focus**: When enabled, keyboard input is read even when the simulator window is not focused.

## Controller (Gamepad/Joystick)

Connect a gamepad or joystick for analog control.

**Configuration:**
- **Controller ID**: Select which connected controller to use (auto-refreshes every second).
- **Axis/Button mapping**: Each movement direction can be mapped to either a joystick axis or a pair of buttons.
- **Deadzone**: Set the minimum stick deflection threshold (percentage) to ignore noise.
- **Invert**: Flip the axis direction if needed.

To rebind: click a binding button, then move the desired axis or press the desired button. Press Escape to cancel.

## IPC (Network via ZeroMQ)

Control the robot remotely over a network using the ZMQ protocol. This is the primary method for integrating external controllers or programs.

**Configuration:**
- **Address**: Network address to bind to (default: `tcp://127.0.0.1`).
- **Command port**: Port for receiving commands and sending responses (default: 5555).
- **Telemetry port**: Port for broadcasting odometry data (default: 5556).
- **Heartbeat timeout**: Time in seconds to wait without a heartbeat before considering the connection lost (default: 5.0s).

**Protocol overview:**

1. Client sends `HANDSHAKE` → Simulator responds with `HANDSHAKE_ACK` + `MOTOR_COUNT`.
2. Client sends movement commands (`MOVE_AT_SPEED_MOTORS`, `MOVE_BY_DISTANCE`, etc.).
3. Simulator acknowledges with `CMD_ACK` and notifies with `CMD_COMPLETE` when done.
4. Simulator streams `TELEMETRY` (odometry) on the telemetry port.
5. Client sends periodic `HEARTBEAT` to keep the connection alive.

**Available command types:**
- `MOVE_AT_SPEED_MOTORS` — Set individual motor speeds directly.
- `MOVE_BY_DISTANCE` / `MOVE_BY_TIME` / `MOVE_AT_SPEED` — High-level movement with pivot point support.
- `MOVE_BY_ANGLE` — Rotate by a specified angle (radians).
- `START_MOTOR` / `RUN_MOTOR_FOR_TIME` / `RUN_MOTOR_FOR_DISTANCE` — Single motor control.
- `STOP` / `STOP_MOTOR` — Immediate stop (bypasses command queue).
- `CLEAR_COMMAND_QUEUE` — Flush pending commands.

See the [IPC Protocol Reference](../dev/ipc-protocol.md) for full technical details.

## Serial

Communicate with hardware over a serial port (e.g., Arduino, microcontroller).

**Configuration:**
- **Port name**: Serial port (e.g., `COM3` on Windows, `/dev/ttyUSB0` on Linux).
- **Baud rate**: Communication speed (selectable: 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600).

**Binary protocol:**

The serial input expects a simple binary frame: `[uint8_t motorCount][float × motorCount]`

- 1 byte: number of motors (1–64)
- N × 4 bytes: wheel speeds as IEEE 754 floats (mm/s)

A background thread reads continuously from the serial port. The main thread picks up the latest data each frame. Serial errors are displayed in the main window.

## Common Settings

Keyboard and Controller input share these settings:
- **Max speed** and **Max rotation speed** scale the input to real velocities.
- **Limit Motor Speed**: When enabled, the physics engine scales all wheel speeds proportionally so no motor exceeds its hardware step rate limit.
- **Register input without focus**: Accept input even when the window is not focused.
