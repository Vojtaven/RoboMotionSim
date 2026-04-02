# Installation Guide

## System Requirements

- **Operating System**: Windows 10+ or Linux (Tested on Fedora 43)
- **Compiler**: A C++23-compatible compiler:
  - MSVC 2022+ (Windows)
  - GCC 13+ (Linux)
- **[CMake](https://cmake.org/)**: Version 3.24 or higher
- **[Git](https://git-scm.com/)**: For cloning the repository

All other dependencies are fetched automatically during the build process except some packages on linux see [section](#additional-packages-needed-on-linux).

## Dowloanding executable

On **[Github](https://github.com/Vojtaven/RoboMotionSim)** are pre-compiled realeases ready to run. See [Releases](https://github.com/Vojtaven/RoboMotionSim/releases/).

## Building from Source

### 1. Clone the Repository

```sh
git clone <repository-url>
cd RobotMotionSim
```

### 2. Configure and Build

```sh
cmake -S . -B build
cmake --build build --config Release
```

The build produces a single statically-linked executable with no external DLL dependencies.

### 3. Run the Simulator

On Windows:
```sh
./build/Release/RoboMotionSim.exe
```

On Linux:
```sh
./build/RoboMotionSim
```

## First Launch

On first launch, the simulator creates a configuration directory:

- **Windows**: `%APPDATA%\RoboMotionSim\`
- **Linux**: `$XDG_CONFIG_HOME/RoboMotionSim/` or `~/.config/RoboMotionSim/`



## Additional packages needed on linux
On fedora 43 you will need to install these packages:
- UDev
- Freetype
- GTK (NFD)
- X11 extensions

Complete command to install all you need:
```sh
sudo dnf install \
  gcc gcc-c++ make cmake pkg-config \
  libX11-devel libXrandr-devel libXcursor-devel libXi-devel \
  systemd-devel freetype-devel mesa-libGL-devel \
  gtk3-devel

```

**What the packages do:**
| Packages                                                             | Purpose                                              |
| -------------------------------------------------------------------- | ---------------------------------------------------- |
| `gcc`, `gcc-c++`, `make`, `cmake`, `pkg-config`                      | Compile and build the project                        |
| `libX11-devel`, `libXrandr-devel`, `libXcursor-devel`, `libXi-devel` | Windowing, input, cursor, and display handling (X11) |
| `systemd-devel`                                                      | Device detection via libudev (e.g. gamepads)         |
| `mesa-libGL-devel`                                                   | Hardware-accelerated graphics (OpenGL)               |
| `freetype-devel`                                                     | Font and text rendering                              |
| `gtk3-devel`                                                         | Native file dialogs (used by Native File Dialog)     |