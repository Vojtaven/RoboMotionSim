# RoboMotion Simulator

RoboMotion Simulator is a C++ application for simulating and visualizing robot motion, developed as part of a bachelor thesis at Charles University. The simulator builds upon the Move Library and aims to provide a flexible, extensible platform for robotics research, algorithm development, and educational purposes.

## Features
- Modular architecture for easy extension and integration
- Real-time visualization of robot motion and environments
- Support for custom robot models and motion planning algorithms
- Cross-platform build with CMake

## Building the Project
This project uses CMake for configuration and building. A typical build process looks like this:

```sh
cmake -S . -B build
cmake --build build --config Release
```

You can then run the simulator from the `build` directory.

## Documentation
- **User documentation**: See `docs/user/` for guides on installation, usage, and simulation setup.
- **Developer documentation**: See `docs/dev/` for architecture, contributing, and advanced customization.

For more complex usage, troubleshooting, or development details, please refer to the appropriate documentation section above.