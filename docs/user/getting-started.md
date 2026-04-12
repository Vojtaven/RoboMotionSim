# Getting Started

This guide walks you through your first simulation session with RoboMotion Simulator.

## Overview

RoboMotion Simulator lets you simulate wheeled robots with different drive types — differential, omni-wheel, and mecanum — and control them through keyboard, gamepad, network (ZMQ), or serial input.

## First Launch

On first launch, the simulator creates default configuration files and loads a default robot. The main window shows the robot on a coordinate grid.

## Loading a Robot Configuration

1. Open the menu by clicking the **≡** button in the top-right corner.
2. Select **Robot Designer**.
3. In the Robot Designer window, click **Load from** and select a `.ini` configuration file.
4. Adjust settings if needed, then click **Apply to Simulation**.

You can also edit the robot directly in the Robot Designer — change drive type, add/remove axles, adjust wheel positions, motor parameters, etc.

## Controlling the Robot

By default, the simulator uses **keyboard input**. The default key bindings are:

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

> **Note**: Differential drive robots cannot strafe (A/D have no effect) and have no independent front rotation (J/K have no effect).

To change the input method or key bindings, open **Input Settings** from the menu.

## Understanding the Visualization

The main window displays:

- **Robot shape**: A convex hull body with wheels drawn at their configured positions.
- **Grid**: A coordinate grid with labeled axes (in millimeters). The grid auto-scales as you zoom.
- **Trail**: A path history showing where the robot has been (toggle in Render Settings).
- **Wheel vectors**: Arrows on each wheel showing forward speed, roller speed, and combined direction.
- **Front vector**: An arrow indicating the robot's forward direction.

### Keyboard Shortcuts

| Key | Action              |
|-----|---------------------|
| F7  | Toggle timestamp overlay |
| F8  | Toggle FPS overlay  |
| ESC | Close menu          |

## Viewing Robot Statistics

Open **Robot Stats** from the menu to see real-time data:

- Angular velocities in deg/s
- Local and global velocities with visual bars in mm/s
- Position coordinates in mm
- Chassis and front angles in degrees
- Per-wheel speeds with color-coded progress bars
- Data logging controls (CSV export) with folder picker

## Saving and Exporting

- **Save robot config**: In Robot Designer, click **Save As...** to export the robot configuration to an `.ini` file.
- **Load robot config**: In Robot Designer, click **Load from** to import from a file.
- **Log data**: In the Robot Stats window, click **Start Logging** to record simulation data to a CSV file.

## Next Steps

- [Robot Configuration Guide](robot-configuration.md) — Learn about drive types and configuration files.
- [Input Methods](input-methods.md) — Set up gamepad, network, or serial control.
- [Visualization Settings](visualization-settings.md) — Customize the display.
- [Data Logging](data-logging.md) — Record and analyze simulation data.
- [ImGui Interface Tips](imgui-interface.md) — Tricks for sliders, rebinding, windows, and more.
