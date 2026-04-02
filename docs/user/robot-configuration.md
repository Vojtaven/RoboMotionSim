# Robot Configuration Guide

## Configuration File Format

Robot configurations are stored in `.ini` files. The simulator supports mathematical expressions in configuration values (using the ExprTk expression engine), which makes it easy to specify angles and positions using trigonometric functions.

## Basic Structure

```ini
DRIVE_TYPE = DIFFERENTIAL
WHEELS = 2

WHEEL_0 = 52, 82.5, 0, 0, 0
MOTOR_0 = 0, 1, 1000, 200

WHEEL_1 = 52, -82.5, 0, 180, 0
MOTOR_1 = 2, 3, 1000, 200
```

> **Note**: Wheels and motors are 0-indexed (`WHEEL_0`, `MOTOR_0`, etc.).

## Drive Types

### Differential (`DIFFERENTIAL`)

A two-wheeled (or more) drive where the robot can move forward/backward and rotate, but cannot strafe sideways. The front angle is locked to 0 (no independent front rotation).

- **Minimum wheels**: 2
- **Typical configuration**: Two wheels on opposite sides of the robot center.

### Omni-wheel (`OMNI_WHEEL`)

Uses wheels with perpendicular rollers, allowing movement in any direction.

- **Minimum wheels**: 3 (must not be collinear)
- **Roller angle**: 90° (set automatically unless using CUSTOM)
- **Typical configuration**: Three wheels evenly spaced at 120° intervals.

### Mecanum (`MECANUM`)

Uses wheels with angled rollers (typically ±45°) for omnidirectional movement.

- **Exact wheels**: 4
- **Roller angle**: 45° (set automatically unless using CUSTOM)

### Custom (`CUSTOM`)

Bypasses drive-type validation and allows setting roller angles per wheel. Use this for non-standard configurations. **This configuration doesn;t support any validation**

## Wheel Parameters

Each wheel is defined on a single line:

```
WHEEL_N = diameter, x_position, y_position, wheel_angle, roller_angle
```

| Parameter      | Unit    | Description                                             |
|----------------|---------|---------------------------------------------------------|
| `diameter`     | mm      | Wheel diameter                                          |
| `x_position`   | mm      | X offset from robot center (forward axis)               |
| `y_position`   | mm      | Y offset from robot center (lateral axis)               |
| `wheel_angle`  | degrees | Direction the wheel drives in (0° = forward)            |
| `roller_angle` | degrees | Roller angle relative to wheel axis (0 for standard wheels, 90 for omni, ±45 for mecanum) |

## Motor Parameters

Each motor is defined on a single line:

```
MOTOR_N = pin1, pin2, maxSpeed, stepsPerRotation
```

| Parameter          | Unit      | Description                                |
|--------------------|-----------|--------------------------------------------|
| `pin1`             | —         | First motor control pin                    |
| `pin2`             | —         | Second motor control pin                   |
| `maxSpeed`         | steps/sec | Hardware maximum speed limit               |
| `stepsPerRotation` | steps     | Steps for one full wheel rotation (including gearbox) |

## Using Expressions

You can use mathematical expressions with standard functions:

```ini
WHEEL_1 = 47, 165 * sin(2 * pi / 3), 165 * cos(2 * pi / 3), 240, 90
```

Supported functions include `sin`, `cos`, `tan`, `sqrt`, `abs`, and constants like `pi` and `e`.

## Validation Rules

The simulator validates configurations when applied to the simulation:

- Wheel count must match the `WHEELS` value.
- All diameters, max speeds, and steps per rotation must be positive.
- No two wheels can have the same position (within 0.01 mm).
- For 3+ wheels with non-differential drive: wheels must not all be collinear.
- Drive-type-specific wheel count rules must be satisfied (see above).
- `CUSTOM` drive type skips all validation.
- Invalid configurations are rejected with an error message in the Robot Designer.

## Robot Designer

The Robot Designer window provides an interactive GUI for editing robot configurations:

- Change drive type, add/remove axles
- Edit wheel positions, diameters, and angles
- Edit motor parameters
- Live preview of the robot shape
- Save to / load from `.ini` files
- Apply directly to the running simulation
