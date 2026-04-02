# Physics Engine — Technical Reference

## Overview

The `PhysicsEngine` class implements wheel kinematics for multiple drive types. It supports forward kinematics (velocity → wheel speeds) and inverse kinematics (wheel speeds → velocity), with optional motor speed limiting.

## Coordinate Frames

- **Local frame**: Robot-centric. X = forward, Y = lateral (right). Origin at the robot's geometric center (centroid of wheel positions).
- **Chassis frame**: The local frame rotated by the front angle offset. Wheel positions and angles are defined in this frame.
- **Global frame**: World coordinates. The local frame is rotated by `chassisAngle + frontAngle`.

## Update Pipeline

The `update()` method runs a two-pass pipeline. The path depends on whether the input source provides wheel speeds directly (`fromWheelSpeeds = true`, used by IPC motor commands and serial input) or robot velocity (`fromWheelSpeeds = false`, used by keyboard and joystick):

```
limitMovement()              ← Apply drive-type constraints

If fromWheelSpeeds:
    limitMotorSpeeds()       ← Scale wheels to stay within hardware limits
    calculateLocalVelocity() ← Inverse kinematics (wheel speeds → vx, vy, ω)
Else:
    toWheelSpeed()           ← Forward kinematics (vx, vy, ω → wheel speeds)
    If motor limiting enabled:
        limitMotorSpeeds()   ← Scale wheels to stay within limits
        calculateLocalVelocity() ← Back-calculate the actual achievable velocity

toWheelSpeed()               ← Final pass to ensure wheel speeds are consistent
toGlobalFrame()              ← Rotate to world coordinates
updatePosition()             ← Integrate position from velocities
updateDirectionVectors()     ← Compute visualization velocity vectors
```

## Forward Kinematics: `toWheelSpeed()`

Converts the robot's desired velocity `(vx, vy, ω)` to individual wheel speeds.

For each wheel at position `(wx, wy)` with wheel angle `θ_w` and roller angle `θ_r`:

1. Rotate local velocity into chassis frame (accounting for front angle offset):
   ```
   chassisVel = localVelocity.rotated(frontAngle)
   ```

2. Compute the velocity at the wheel contact point (body velocity + rotational component):
   ```
   v_local_x = chassisVx - ω · wy
   v_local_y = chassisVy + ω · wx
   ```

3. Decompose into longitudinal (drive) and transverse (roller) components:
   ```
   v_long = v_local_x · cos(θ_w) + v_local_y · sin(θ_w)
   v_tran = -v_local_x · sin(θ_w) + v_local_y · cos(θ_w)
   ```

4. Compute wheel and roller speeds:
   - If `|cos(θ_r)| < 0.001` (roller perpendicular to wheel — pure omni wheel):
     ```
     wheel_speed = v_long
     roller_speed = v_tran
     ```
   - Otherwise:
     ```
     wheel_speed = v_long + v_tran · tan(θ_r)
     roller_speed = v_tran / cos(θ_r)
     ```
   - For differential drive, roller speed is forced to 0.

## Inverse Kinematics: `calculateLocalVelocityFromWheelSpeed()`

Solves for robot velocity given measured wheel speeds using least-squares.

### Algorithm

1. Build a Jacobian matrix `J` where each row corresponds to a wheel constraint:
   ```
   J_i · [vx, vy, ω]^T = v_wheel_i
   ```

2. Form the normal equations: `A = J^T · J`, `b = J^T · v_wheels`

3. Solve the 3×3 system `A · x = b` using Gauss-Jordan elimination with partial pivoting.

4. Result: `x = [vx, vy, ω]` in the chassis frame, then rotated back to local frame via `rotatedInverse(frontAngle)`.

This handles overdetermined systems (more wheels than DOF) by finding the least-squares solution.

## Drive-Type Constraints: `limitMovement()`

| Drive Type   | Constraint                                                     |
|-------------|----------------------------------------------------------------|
| Differential | `localVelocity.y = 0`, `frontAngle = 0`, `frontAngularVelocity = 0` |
| Omni-wheel   | No constraints (full 3-DOF movement)                          |
| Mecanum      | No constraints (full 3-DOF movement)                          |
| Custom       | No constraints                                                |

## Motor Speed Limiting: `limitMotorSpeeds()`

When enabled (via the "Limit Motor Speed" checkbox in Input Settings), this scales all wheel speeds proportionally so that no motor exceeds its hardware step rate limit:

1. For each wheel, convert `speed` (mm/s) to steps/s using `stepsPerRotation / (π · diameter)`.
2. Find the worst-case ratio: `maxRatio = max(speedInSteps / hardwareMaxSpeedLimit)`.
3. If `maxRatio > 1`, divide all wheel speeds (and roller speeds) by `maxRatio`.

This preserves the robot's trajectory direction while reducing overall speed.

## Position Integration: `updatePosition()`

Simple Euler integration:
```
position += globalVelocity · dt
chassisAngle += angularVelocity · dt
frontAngle += frontAngularVelocity · dt
distanceTraveled += localVelocity · dt
```

Per-wheel distance accumulators are also updated.

## Direction Vectors: `updateDirectionVectors()`

For visualization, computes the velocity vector at arbitrary points on the robot body. Each point's velocity is the body velocity (in chassis frame) plus the rotational velocity at that point's offset from the center:
```
v_point = chassisVelocity + ω × offset
```
