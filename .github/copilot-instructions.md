# RPi5 Gimbal Controller - Development Instructions

This is a C++ project for controlling a 2D pan-tilt gimbal with MG90S servo motors on Raspberry Pi 5.

## Architecture

The project provides a basic, modular architecture:

- **Gimbal Class** (`include/Gimbal.h`): Core controller for gimbal operations
  - `init()`: Initialize GPIO and PWM
  - `setTipAngle(pan, tilt)`: Set pan/tilt angles (-90 to +90 degrees)
  - `getPanAngle()`, `getTiltAngle()`: Query current angles
  - `shutdown()`: Cleanup resources

- **Implementation** (`src/Gimbal.cpp`): Angle-to-PWM conversion and GPIO abstraction
  - Currently in simulation mode (logs commands)
  - Designed for easy integration with pigpio for real hardware control

- **Example** (`examples/example_basic.cpp`): Usage demonstration with sweep patterns

## Build & Run

```bash
# Build
mkdir -p build && cd build
cmake ..
make

# Run example (simulation mode)
./bin/gimbal_example

# For hardware testing (requires pigpio installed):
# 1. Uncomment pigpio section in CMakeLists.txt
# 2. Rebuild
# 3. Run with: sudo ./bin/gimbal_example
```

## Hardware Wiring

- Pan Servo Signal → GPIO 17 (PWM)
- Tilt Servo Signal → GPIO 27 (PWM)
- Both servos: 5V power, GND ground

## Next Steps for Enhancement

1. **Enable Hardware Control**: Uncomment pigpio integration in CMakeLists.txt
2. **Add Tracking Algorithms**: Extend Gimbal class with motion planning
3. **Implement Safety**: Add boundary checking and speed limits
4. **Add Camera Integration**: Connect to RPi camera module
5. **Testing**: Create unit tests for angle conversion and limits

## Key Files

- `include/Gimbal.h` - Class definition
- `src/Gimbal.cpp` - Implementation
- `examples/example_basic.cpp` - Usage example
- `CMakeLists.txt` - Build configuration
- `README.md` - Full documentation
