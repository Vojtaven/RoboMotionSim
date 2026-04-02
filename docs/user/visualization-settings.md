# Visualization Settings

The **Render Settings** window lets you customize the simulation display. Open it from the main menu (≡ button).

## General Settings

- **Scale factor**: Zoom level for the visualization (0.1–5.0). Higher values zoom in.
- **Lock view on robot**: When enabled, the camera follows the robot as it moves.
- **Frame rate limit**: Cap the rendering frame rate (0 = unlimited, up to 480).

## Grid

- **Show grid**: Toggle grid visibility.
- **Auto grid spacing**: Automatically adjust grid spacing based on zoom level using a 1-2-5 snap sequence for clean values.
- **Grid spacing** (mm): Manual distance between grid lines (only when auto spacing is off).
- **Grid subdivision count**: Number of minor grid lines between major lines (0–10).

## Colors

A color picker lets you customize:
- **Grid color**: Color of the major grid lines.
- **SubGrid color**: Color of the minor subdivision lines.
- **Trail color**: Color (with alpha) of the path trail points.

## Trail

- **Show trail**: Toggle the path history display.
- **Point size**: Radius of each trail point.
- **Spawn interval**: Time in seconds between recording trail points.
- **Max length**: Maximum number of trail points stored. Older points are discarded.
- **Reset Trail**: Clear all trail points.

## Vectors

Three separate toggles for wheel overlays:
- **Show Forward Vectors**: Arrows showing each wheel's drive speed.
- **Show Roller Vectors**: Arrows showing each wheel's roller speed (relevant for omni/mecanum).
- **Show Wheel Direction Vectors**: Arrows showing the combined velocity direction at each wheel (forward + roller combined via law of cosines).

When a wheel is not actively powered (e.g., individual motor commands), its vectors turn gray.

## Robot Display

The robot is rendered with:
- A convex hull body shape computed from wheel positions.
- Wheels drawn as rectangles at their configured positions and angles.
- A front direction vector showing the robot's heading.
- Optional velocity vectors overlaid on each wheel.
- Direction vectors at arbitrary points on the body (if configured).
