# RPi5 2D Gimbal Controller

A C++ library for controlling a pan-tilt camera gimbal on Raspberry Pi 5 using two MG90S servo motors.

## Overview

This project provides a basic, extensible architecture for controlling a 2D gimbal (pan-tilt) system with a camera mounted on the tip. The library offers a clean, simple API for controlling servo angles and is designed to be easily wrapped with more sophisticated algorithms later.

### Features

- **Simple API**: Easy-to-use class for gimbal initialization and angle control
- **Modular Design**: Clean separation between servo control and high-level gimbal logic
- **Raspberry Pi 5 Optimized**: Built for RPi5 GPIO and PWM capabilities
- **MG90S Servo Support**: Pre-configured for MG90S servo specifications
  - Operating voltage: 3-7V
  - Range: ±90° for each axis
  - Pulse width: 1000-2000 microseconds
- **Extensible**: Base class designed for future algorithm implementations

## Project Structure

```
gimbal/
├── include/
│   └── Gimbal.h           # Gimbal controller class definition
├── src/
│   └── Gimbal.cpp         # Gimbal controller implementation
├── examples/
│   └── example_basic.cpp  # Basic usage example
├── CMakeLists.txt         # Build configuration
├── README.md              # This file
└── .github/
    └── copilot-instructions.md
```

## Hardware Requirements

### Components
- **Raspberry Pi 5** (or similar with GPIO and PWM support)
- **2x MG90S Servo Motors** (pan and tilt)
- **Camera Module** (any RPi-compatible camera)
- **Power Supply** (appropriate for servos, typically 5V/2A+)
- **Connecting Wires** and **Servo Brackets**

### Wiring

```
MG90S Pan Servo:
  - Signal (Yellow/Orange) → GPIO 17 (PWM)
  - Power (Red)            → 5V
  - Ground (Brown/Black)   → GND

MG90S Tilt Servo:
  - Signal (Yellow/Orange) → GPIO 27 (PWM)
  - Power (Red)            → 5V
  - Ground (Brown/Black)   → GND

Camera:
  - Connect to RPi CSI/DSI connector (ribbon cable)
```

**Note:** Adjust GPIO pin numbers in code based on your specific wiring. Default pins are 17 and 27.

## Building the Project

### Prerequisites

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake git
```

### Optional: Install pigpio for Real GPIO Control

```bash
sudo apt-get install -y pigpio
# Start pigpio daemon
sudo systemctl enable pigpiod
sudo systemctl start pigpiod
```

### Build Steps

```bash
# Create build directory
mkdir build
cd build

# Generate build files
cmake ..

# Compile
make

# Executables will be in build/bin/
# Libraries will be in build/lib/
```

### Cross-Compilation from Linux x86_64

```bash
mkdir build-rpi5
cd build-rpi5

# Use appropriate cross-compiler toolchain
cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain-rpi5.cmake

make
```

## Usage

### Basic Example

```cpp
#include "Gimbal.h"

int main() {
    // Create gimbal controller (GPIO pins 17 and 27)
    Gimbal gimbal(17, 27);
    
    // Initialize
    if (!gimbal.init()) {
        return 1;
    }
    
    // Set pan and tilt angles (-90 to +90 degrees)
    gimbal.setTipAngle(30.0f, -15.0f);  // Pan 30°, Tilt -15°
    
    // Get current angles
    float pan = gimbal.getPanAngle();
    float tilt = gimbal.getTiltAngle();
    
    // Shutdown
    gimbal.shutdown();
    
    return 0;
}
```

### Running the Example

```bash
cd build
./bin/gimbal_example
```

## API Reference

### Gimbal Class

#### Constructor
```cpp
Gimbal(uint32_t pan_pin, uint32_t tilt_pin);
```
- `pan_pin`: GPIO pin number for pan servo
- `tilt_pin`: GPIO pin number for tilt servo

#### Initialization
```cpp
bool init();
```
Sets up GPIO, PWM, and centers gimbal. Returns `true` on success.

#### Shutdown
```cpp
void shutdown();
```
Stops PWM signals and releases resources.

#### Angle Control
```cpp
bool setTipAngle(float pan_angle, float tilt_angle);
```
Sets gimbal angles in degrees. Valid range: -90 to +90.
- Pan angle: negative = left, positive = right
- Tilt angle: negative = down, positive = up

Returns `true` on success.

#### Get Current Angles
```cpp
float getPanAngle() const;
float getTiltAngle() const;
```

#### Status
```cpp
bool isInitialized() const;
```

## Implementation Status

### Current (Simulation Mode)
- ✅ Class architecture and API
- ✅ Angle validation and conversion
- ✅ Logging and example code
- ⚠️ PWM signals logged but not actually applied to GPIO

### TODO - Hardware Integration
- [ ] Implement actual PWM control using pigpio library
- [ ] Add hardware error handling
- [ ] Implement servo calibration
- [ ] Add temperature and voltage monitoring

### TODO - Advanced Features
- [ ] Trajectory planning algorithms
- [ ] PID control for smooth motion
- [ ] Camera auto-tracking
- [ ] Speed/acceleration control
- [ ] Boundary checking and safety limits

## Configuration

### MG90S Servo Specifications
Located in `include/Gimbal.h`:

```cpp
static constexpr uint32_t PWM_FREQUENCY = 50;      // 50 Hz
static constexpr uint32_t MIN_PULSE_WIDTH = 1000;  // -90°
static constexpr uint32_t MID_PULSE_WIDTH = 1500;  // 0°
static constexpr uint32_t MAX_PULSE_WIDTH = 2000;  // +90°
```

To adjust for different servos or requirements, modify these constants.

### GPIO Pins
Modify in `examples/example_basic.cpp`:
```cpp
const uint32_t PAN_PIN = 17;    // Change as needed
const uint32_t TILT_PIN = 27;   // Change as needed
```

## Testing

### Simulation Mode
Test the logic without connected hardware:
```bash
./bin/gimbal_example
```

### Hardware Testing (with pigpio)
1. Connect servos and camera to RPi5
2. Uncomment pigpio integration in `CMakeLists.txt`
3. Rebuild: `cmake .. && make`
4. Run with appropriate permissions: `sudo ./bin/gimbal_example`

## Troubleshooting

### Servo Not Responding
- Verify GPIO pins are correct
- Check power supply voltage (should be 5V)
- Ensure pigpio daemon is running: `sudo systemctl status pigpiod`
- Check servo signal connections (yellow/orange wire)

### Build Errors
- Ensure CMake >= 3.10: `cmake --version`
- Check C++ compiler supports C++17: `g++ --version`
- For cross-compilation, verify toolchain file exists

### Angle Not Changing
- In simulation mode, angles are logged but PWM not applied
- Enable pigpio integration for actual hardware control

## Future Enhancements

This basic architecture is designed to be extended with:

1. **Trajectory Planning**: Smooth paths between positions
2. **PID Control**: Precise positioning and damping
3. **Computer Vision**: Auto-tracking based on camera feed
4. **Motion Algorithms**: Panorama, timelapse, orbit patterns
5. **Multi-axis Support**: Extensible to 3+ axes
6. **ROS Integration**: Robot Operating System compatibility
7. **Network Control**: Remote gimbal via network interface

## Contributing

This is a starting framework. Extend it with:
- Your tracking algorithms
- Custom motion patterns
- Hardware optimizations
- Testing utilities

## License

This project is provided as-is for educational and personal use.

## References

- [Raspberry Pi GPIO Documentation](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html)
- [MG90S Servo Specification](https://www.towerpro.com.tw/product/mg90s-digital-servo/)
- [pigpio Library](http://abyz.me.uk/rpi/pigpio/)
- [CMake Documentation](https://cmake.org/documentation/)

## Support

For issues or questions:
1. Check the troubleshooting section
2. Review example code
3. Enable debug logging in source code
4. Verify hardware connections
