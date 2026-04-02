# RoboMotion Simulator — Documentation

## User Guide

Start here if you want to use the simulator.

| Guide | Description |
|-------|-------------|
| [Installation](user/installation.md) | System requirements, building from source, Linux packages |
| [Getting Started](user/getting-started.md) | Your first simulation session — launching, controlling, and viewing the robot |
| [Robot Configuration](user/robot-configuration.md) | Drive types, `.ini` config format, Robot Designer |
| [Input Methods](user/input-methods.md) | Keyboard, gamepad, IPC (ZeroMQ), and serial control |
| [Visualization Settings](user/visualization-settings.md) | Grid, trail, vectors, colors, and display options |
| [Data Logging](user/data-logging.md) | Recording simulation data to CSV |

## Developer Reference

Start here if you want to understand or modify the codebase.

| Guide | Description |
|-------|-------------|
| [Architecture](dev/architecture.md) | High-level design, module dependencies, main loop, reactive config system |
| [Modules](dev/modules.md) | Detailed reference for every module — Core, Physics, Config, Input, Visualization |
| [Physics Engine](dev/physics-engine.md) | Forward/inverse kinematics, coordinate frames, motor limiting |
| [IPC Protocol](dev/ipc-protocol.md) | ZeroMQ socket architecture, message types, command reference, telemetry format |
| [Build System](dev/building.md) | CMake setup, dependencies, compiler settings, platform notes |
| [Extending](dev/extending.md) | How to add new drive types, input sources, UI windows, and visualization elements |

## Quick Links

- **I want to run the simulator** → [Installation](user/installation.md) then [Getting Started](user/getting-started.md)
- **I want to control the robot from my code** → [IPC Protocol](dev/ipc-protocol.md) and [Input Methods](user/input-methods.md#ipc-network-via-zeromq)
- **I want to connect hardware over serial** → [Input Methods — Serial](user/input-methods.md#serial)
- **I want to design a custom robot** → [Robot Configuration](user/robot-configuration.md)
- **I want to add a feature to the simulator** → [Architecture](dev/architecture.md) then [Extending](dev/extending.md)
