# Architecture Overview

## High-Level Design

RoboMotion Simulator follows a modular architecture with five core subsystems orchestrated by a central `AppEngine`. Each subsystem is implemented as an independent CMake module under `src/`.

```
┌─────────────────────────────────────────────┐
│                  AppEngine                   │
│          (Main loop & orchestration)         │
├──────────┬──────────┬───────────┬───────────┤
│ Physics  │ Visuali- │  Input    │  Config   │
│ Engine   │ zation   │  Manager  │  Manager  │
│          │ Engine   │           │           │
├──────────┴──────────┴───────────┴───────────┤
│               Core (shared types)            │
│   RobotState · RobotConfig · AppConfig      │
│   MathUtils · ConfigSection · StatLogger    │
└─────────────────────────────────────────────┘
```

## Module Dependency Graph

```
app ──► core
app ──► physicsEngine ──► core
app ──► visualization ──► core
app ──► inputManager ──► core
app ──► configManager ──► core
```

All modules depend on `core` for shared data types. No module depends on another peer module — they communicate through the shared `RobotState` and `RobotConfig` objects managed by `AppEngine`.

## Reactive Configuration

Settings are wrapped in `ConfigSection<T>`, which provides change notification via a publish-subscribe pattern. When a setting is modified (e.g. in the UI), all subscribers are notified automatically. `ScopedSubscription` provides RAII-based lifetime management for callbacks.

## Main Loop

The simulation runs a game loop in `AppEngine::run()` with physics sub-stepping:

```
while (window is open):
    frameDelta = time since last frame
    targetFrameTime = 1 / frameRateLimit

    // Physics runs in a tight loop until enough time has passed for one frame
    do:
        physicsDelta = time since last physics tick
        inputManager.update(robotState, hasFocus)
        physicsEngine.update(physicsDelta, robotState, config)
        inputManager.checkForInputCompletion(robotState, physicsDelta)
        physicsTimeSpent += physicsDelta
    while (physicsTimeSpent < targetFrameTime)

    vizEngine.update(frameDelta, robotState, wallTime)
    vizEngine.draw()
```

Each subsystem reads from and writes to `RobotState`:
- **InputManager** writes velocities or wheel speeds based on user input. Also checks if time/distance-based commands have completed.
- **PhysicsEngine** reads velocities, applies kinematics, writes updated positions and wheel data.
- **VisualizationEngine** reads the full state to render the scene.

## Source Tree

```
src/
├── app/                    Entry point and AppEngine
├── core/                   Shared data structures and utilities
├── physicsEngine/          Kinematics calculations
├── configManager/          Configuration file I/O
├── inputManager/           Input source abstraction
├── visualization/          SFML + ImGui rendering
├── 3rdPartyLibs/           Bundled third-party code (ExprTk)
└── assets/                 Fonts, icons, resource files
```

Each module follows the convention:
```
module/
├── include/    Public headers
├── src/        Implementation files
└── CMakeLists.txt
```
