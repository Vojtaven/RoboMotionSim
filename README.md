# RoboMotion Simulator

RoboMotion Simulator is a real-time 2D robot motion simulator written in C++. It was developed as part of a bachelor thesis at Charles University, building upon the Move Library. The simulator lets you design wheeled robots, control them through multiple input methods, and observe their kinematics in real time.

## What It Does

- **Simulate** differential, omni-wheel, mecanum, and custom drive robots with accurate forward and inverse kinematics
- **Control** robots via keyboard, gamepad, ZeroMQ network protocol (IPC), or serial port
- **Visualize** robot motion with a coordinate grid, path trail, and per-wheel velocity vectors
- **Design** robots interactively with the built-in Robot Designer — adjust wheels, motors, and drive type with a live preview
- **Log** simulation data to CSV for analysis in spreadsheets or Python/pandas
- **Integrate** with external programs through the IPC protocol — send movement commands and receive telemetry over ZeroMQ

## Quick Start

### Pre-built Releases

Download a ready-to-run executable from [GitHub Releases](https://github.com/Vojtaven/RoboMotionSim/releases/).

### Building from Source

Requires a **C++23** compiler (MSVC 2022+ or GCC 13+), **CMake 3.24+**, and **Git**. All other dependencies are fetched automatically.

```sh
git clone https://github.com/Vojtaven/RoboMotionSim.git
cd RoboMotionSim
cmake -S . -B build
cmake --build build --config Release
```

On **Linux** (tested on Fedora 43), you'll need a few system packages first — see the [installation guide](docs/user/installation.md#additional-packages-needed-on-linux) for details.

Run the simulator:
```sh
# Windows
./build/Release/RoboMotionSim.exe

# Linux
./build/RoboMotionSim
```

On first launch, default configuration files are created automatically in `%APPDATA%\RoboMotionSim\` (Windows) or `~/.config/RoboMotionSim/` (Linux).

## Documentation

Everything you need to know is covered in the guides below.

| Guide | Description |
|-------|-------------|
| [Installation](docs/user/installation.md) | System requirements, build instructions, Linux packages |
| [Getting Started](docs/user/getting-started.md) | First simulation session walkthrough |
| [Robot Configuration](docs/user/robot-configuration.md) | Drive types, `.ini` config format, Robot Designer |
| [Input Methods](docs/user/input-methods.md) | Keyboard, gamepad, IPC (ZeroMQ), and serial control |
| [Visualization Settings](docs/user/visualization-settings.md) | Grid, trail, vectors, and display options |
| [Data Logging](docs/user/data-logging.md) | CSV export and analysis |
| [Architecture](docs/dev/architecture.md) | Module overview and main loop |
| [IPC Protocol](docs/dev/ipc-protocol.md) | ZeroMQ command/telemetry protocol reference |
| [Physics Engine](docs/dev/physics-engine.md) | Kinematics math and algorithms |
| [Extending](docs/dev/extending.md) | How to add drive types, input sources, and UI windows |

## Tech Stack

| Library | Purpose |
|---------|---------|
| [SFML 3](https://www.sfml-dev.org/) | Graphics, windowing, input |
| [Dear ImGui](https://github.com/ocornut/imgui) | Immediate-mode UI |
| [ZeroMQ](https://zeromq.org/) | Network IPC |
| [Asio](https://think-async.com/Asio/) | Serial port I/O |
| [nlohmann/json](https://github.com/nlohmann/json) | Configuration serialization |
| [ExprTk](https://www.partow.net/programming/exprtk/) | Math expressions in config files |
| [NFD](https://github.com/btzy/nativefiledialog-extended) | Native file picker dialogs |


## Issues

If you find a bug or performance problem, please open a new issue on [GitHub](https://github.com/Vojtaven/RoboMotionSim/issues). Include your system specs and describe the issue in detail. If a similar issue already exists, add your information there instead of opening a new one.