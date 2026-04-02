# Build System

## Overview

The project uses CMake (3.24+) with automatic dependency fetching via `FetchContent`. The build produces a single statically-linked executable.

## Project Structure

```
CMakeLists.txt              Root build configuration
cmake/
├── dependencies.cmake      Dependency declarations
├── module_helpers.cmake     Helper functions for module setup
└── dependencies/            Per-library fetch configurations
src/
├── app/CMakeLists.txt
├── core/CMakeLists.txt
├── physicsEngine/CMakeLists.txt
├── configManager/CMakeLists.txt
├── inputManager/CMakeLists.txt
└── visualization/CMakeLists.txt
```

## Build Commands

### Standard Build

```sh
cmake -S . -B build
cmake --build build --config Release
```

### Debug Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

### Specifying a Generator

```sh
# Visual Studio
cmake -S . -B build -G "Visual Studio 17 2022"

# Ninja
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
```

## Dependencies

All dependencies are fetched automatically via CMake `FetchContent`:

| Library            | Version | Purpose                       |
| ------------------ | ------- | ----------------------------- |
| SFML               | 3.0.2   | Graphics, windowing, input    |
| ImGui              | v1.91.9 | Immediate-mode UI             |
| ImGui-SFML         | v3.0    | ImGui backend for SFML        |
| Asio               | 1.36.0  | Serial port I/O               |
| libzmq + cppzmq    | v4.11.0 | Network IPC (ZeroMQ)          |
| nlohmann/json      | v3.12.0 | JSON configuration parsing    |
| Native File Dialog | v1.3.0  | OS-native file picker dialogs |
| ExprTk             | bundled | Math expression evaluation    |

## Compiler Settings

- **C++ Standard**: C++23
- **Warnings**: `-Wall -Wextra` (GCC/Clang), `/W4` (MSVC)
- **Runtime**: Static linking (`/MT` on MSVC, static libstdc++ on GCC)

## Platform-Specific Notes

### Windows
- Subsystem set to `WINDOWS` (no console window in Release)
- Links `ws2_32` and `wsock32` for networking
- Resource file `assets/app.rc` embeds the application icon
- Visual Studio: sets `RoboMotionSim` as the startup project

### Linux
- Links `pthread` for threading

## Adding a New Module

1. Create the directory `src/mymodule/` with `include/` and `src/` subdirectories.
2. Add a `CMakeLists.txt` using the helper from `module_helpers.cmake`.
3. Link against `core` and any needed dependencies.
4. Add the module to the root `CMakeLists.txt` via `add_subdirectory`.
5. Link it into the `app` target.
