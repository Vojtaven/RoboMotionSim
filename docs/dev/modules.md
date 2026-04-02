# Module Reference

## Core (`src/core/`)

Shared data types used across all modules.

### Key Types

**`RobotConfig`** (`RobotConfig.hpp`)
- `RobotDriveType` enum class: `DIFFERENTIAL`, `OMNI_WHEEL`, `MECANUM`, `CUSTOM`
- `Wheel` struct: diameter, position (x, y), wheel angle, roller angle
- `Motor` struct: pin mapping, hardware max speed limit, steps per rotation
- `DriveAxle_t`: pairs a Wheel with a Motor
- `RobotConfig` class: drive type + vector of drive axles, with `validateConfig()` for constraint checking

**`RobotState`** (`RobotState.hpp`)
- Robot position, angles, velocities (local and global frames)
- Per-wheel speeds, roller speeds, distances, powered state
- Direction vectors for visualization points
- `fromWheelSpeeds` flag: determines whether physics derives robot velocity from wheel speeds or vice versa
- All angles stored internally in radians; displayed in degrees

**`AppConfig`** (`AppConfig.hpp`)
- Window positions and sizes
- Render settings (scale, grid, trail, vectors)
- Input settings (type, key bindings, controller mapping, IPC/serial config)
- Settings wrapped in `ConfigSection<T>` for reactive change notification

**`ConfigSection<T>`** (`ConfigSection.hpp`)
- Generic reactive wrapper for configuration data
- Publish-subscribe pattern: `scopedSubscribe()` returns a `ScopedSubscription` (RAII guard)
- Copy semantics copy only data, not callbacks
- `getMutable()` provides direct access without triggering notifications (used for deserialization and UI editing)

**`MathUtils`** (`MathUtils.hpp`)
- `Vec2<T>` template: 2D vector with arithmetic operators, `length()`, `rotated()`, `rotatedInverse()`
- Type aliases: `Vec2f`, `Vec2d`, `Vec2i`
- `DegreesToRadians()` / `RadiansToDegrees()` conversions

**`RobotStatLogger`** (`RobotStatLogger.hpp`)
- CSV data logger for simulation state
- Wall-clock timestamping
- Per-frame snapshots of robot and wheel data

---

## PhysicsEngine (`src/physicsEngine/`)

Handles all kinematics calculations.

### Key Methods

| Method | Description |
|--------|-------------|
| `update(dt, state, config)` | Main update — runs the full kinematics pipeline |
| `limitMovement(state, config)` | Constrains velocities by drive type (e.g., no strafing for differential) |
| `limitMotorSpeeds(state, config)` | Scales all wheels proportionally so no motor exceeds its hardware step rate limit |
| `calculateLocalVelocityFromWheelSpeed(state, config)` | Inverse kinematics: wheel speeds → robot velocity via Jacobian + Gaussian elimination |
| `toWheelSpeed(state, config)` | Forward kinematics: robot velocity → wheel speeds |
| `toGlobalFrame(state)` | Rotates local velocity to global frame using chassis + front angle |
| `updatePosition(dt, state)` | Integrates velocities to update position |
| `updateDirectionVectors(state)` | Calculates velocity vectors at visualization points |

### Kinematics Pipeline

The update runs a two-pass pipeline depending on the input source:

```
limitMovement()              ← Apply drive-type constraints

If input provides wheel speeds (fromWheelSpeeds = true):
    limitMotorSpeeds()       ← Clamp to hardware limits (if enabled)
    calculateLocalVelocity() ← Inverse kinematics

If input provides robot velocity (fromWheelSpeeds = false):
    toWheelSpeed()           ← Forward kinematics
    If motor limiting enabled:
        limitMotorSpeeds()   ← Clamp to hardware limits
        calculateLocalVelocity() ← Back-calculate achievable velocity

toWheelSpeed()               ← Final pass for consistency
toGlobalFrame()              ← Rotate to world coordinates
updatePosition()             ← Integrate position from velocities
updateDirectionVectors()     ← Compute visualization vectors
```

### Inverse Kinematics Algorithm

For each wheel, a constraint row is added to a Jacobian matrix:

```
v_wheel_i = [cos(α_i), sin(α_i), r_i] · [vx, vy, ω]
```

Where `α_i` is the wheel's effective angle and `r_i` is its distance from the rotation center. The overdetermined system is solved via Gaussian elimination (3×3 matrix from the normal equations J^T·J·x = J^T·b).

---

## ConfigManager (`src/configManager/`)

Handles loading, saving, and validating configuration files.

### Components

- **ConfigManager**: Orchestrates load/save operations, manages config directory paths
- **ImportHelper**: Parses `.ini` robot configs (with ExprTk expression evaluation) and `.json` app configs
- **ExportHelper**: Serializes configs back to `.ini` and `.json` formats
- **ConfigJsonSerialization**: nlohmann/json serialization for AppConfig types (uses `NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT` macros)

### Configuration Directory

| Platform | Path |
|----------|------|
| Windows  | `%APPDATA%\RoboMotionSim\` |
| Linux    | `$XDG_CONFIG_HOME/RoboMotionSim/` or `~/.config/RoboMotionSim/` |

Default configs are embedded as binary data and written to disk on first launch. On version mismatch, the app config is reset to defaults.

### Config Version

Current version: `0.7.0` (defined as `CONFIG_VERSION` in `AppConfig.hpp`). Mismatched versions trigger a reset to defaults.

---

## InputManager (`src/inputManager/`)

Abstracts multiple input sources behind a common interface.

### Input Sources

| Class | Description | Output |
|-------|-------------|--------|
| `KeyboardInput` | SFML key polling | Local velocity (vx, vy, ω, front ω) |
| `JoystickInput` | Gamepad axes/buttons | Local velocity (vx, vy, ω, front ω) |
| `IPCInput` | ZMQ network protocol | Command queue → velocities or wheel speeds |
| `SerialInput` | Asio serial port (background thread) | Wheel speeds directly |

All `update()` methods return `std::optional<std::string>` — an error message if something went wrong (e.g. controller disconnected, serial error), or `std::nullopt` on success.

### IPC Protocol

Uses two ZMQ sockets:
- **ROUTER** socket for receiving commands and sending responses/ACKs back to the connected client
- **PUB** socket for broadcasting telemetry (with `conflate=1` to keep only the latest message)

Message flow: `HANDSHAKE` → `HANDSHAKE_ACK` + `MOTOR_COUNT` → `COMMAND` → `CMD_ACK`/`CMD_COMPLETE` → `TELEMETRY` (continuous)

Only one client can be connected at a time. A heartbeat timeout mechanism detects lost connections.

### Serial Protocol

Binary protocol over serial port: `[uint8_t motorCount][float × motorCount]`. A background thread reads continuously and the main thread picks up the latest data each frame.

---

## Visualization (`src/visualization/`)

Renders the UI and simulation scene using SFML and ImGui.

### Components

**VisualizationEngine** — Thin wrapper that owns the `MainWindow`.

**Windows:**
- `MainWindow` — Primary viewport with hamburger menu, FPS overlay (F8), timestamp overlay (F7)
- `RobotDesignerWindow` — Interactive wheel/motor editor with live preview
- `RobotStatWindow` — Real-time statistics, velocity bars, and CSV logging controls
- `RenderSettingsWindow` — Visual display options (grid, trail, vectors, colors)
- `InputSettingsWindow` — Input method configuration with live key/button/axis binding
- `SettingsWindowBase` — Base class for all settings windows (shared open/close/draw/event logic, ImGui lifecycle)

**Rendering:**
- `RenderEngine` — Manages world and UI views, draws the robot, grid, trail, and vectors
- `Grid` — Coordinate grid with auto-scaling (1-2-5 snap sequence) and text labels
- `RobotTrail` — Path history renderer using circle shapes in a deque
- `Text` / `TextLabel` — World-to-screen mapped text labels with background and anchor points

**Shapes:**
- `RobotShape` — Composite shape: convex hull body, wheels, front vector, per-wheel velocity vectors, direction vectors
- `CompositeShape` — Base class for multi-part drawable assemblies with hierarchical transforms
- `WheelVectors` — Three arrows per wheel: forward speed, roller speed, combined direction (law of cosines)
- `Arrow` / `PointVector` — Vector visualization primitives (negative length flips via scale)
