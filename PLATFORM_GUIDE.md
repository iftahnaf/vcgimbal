# Platform Support Guide

This gimbal controller supports multiple platforms:
- **Raspberry Pi 5** - Full Linux with pigpio PWM control
- **Raspberry Pi Pico** - Embedded RP2040 microcontroller with pico-sdk

## Platform Architecture

The project uses a platform abstraction layer:

```
User Code (Gimbal)
       ↓
PWMController (abstract interface)
       ↓
┌──────────────────┬──────────────────┐
│                  │                  │
v                  v                  v
PWMControllerRPi5  PWMControllerPico  Custom...
(pigpio)           (pico-sdk)
```

This design allows adding support for other platforms easily.

## Building for Different Platforms

### Build for Raspberry Pi 5 (default)

```bash
bash scripts/build.sh RPi5
# Or simply:
bash scripts/build.sh
```

**Requirements:**
```bash
sudo apt-get install build-essential cmake g++
# Optional (for real GPIO control):
sudo apt-get install pigpio
```

**Output:**
- Binary: `build/bin/gimbal_example`
- Library: `build/lib/libgimbal.a`

**Run:**
```bash
./build/bin/gimbal_example
```

### Build for Raspberry Pi Pico

```bash
bash scripts/build.sh PICO
```

**Requirements:**
```bash
# ARM toolchain
sudo apt-get install gcc-arm-none-eabi

# CMake
sudo apt-get install cmake

# pico-sdk (automatically cloned as submodule)
# If not present, run:
git submodule update --init --recursive
```

**Output:**
- Firmware: `build/bin/gimbal_example.uf2`
- Binary: `build/bin/gimbal_example` (for simulation/testing)

**Flashing to Pico:**

Option 1 - Using provided script:
```bash
# Connect Pico in bootloader mode (hold BOOTSEL while plugging in)
bash scripts/flash.sh
```

Option 2 - Manual:
```bash
# 1. Connect Pico in bootloader mode
# 2. Find the RPI-RP2 mount point
lsblk

# 3. Copy firmware
cp build/bin/gimbal_example.uf2 /media/[username]/RPI-RP2/
```

## Platform-Specific Details

### Raspberry Pi 5

**PWM Control:**
- Uses `pigpio` library for GPIO/PWM access
- Requires daemon: `pigpiod` running
- Frequency: 50 Hz (20ms period)
- Pulse width: 1000-2000 microseconds

**GPIO Pins (default):**
- Pan servo: GPIO 17
- Tilt servo: GPIO 27

**Hardware Setup:**
- 5V power supply for servos (separate from RPi)
- Servo signal → GPIO pin (3.3V logic)
- GND connection required

**Implementation:**
- [include/PWMControllerRPi5.h](../include/PWMControllerRPi5.h)
- [src/PWMControllerRPi5.cpp](../src/PWMControllerRPi5.cpp)

### Raspberry Pi Pico

**PWM Control:**
- Uses `pico-sdk` hardware PWM peripheral
- 16-bit PWM resolution (0-65535)
- Automatic clock divider calculation
- No daemon needed - runs standalone

**GPIO Pins (default):**
- Pan servo: GPIO 17
- Tilt servo: GPIO 27

**Hardware Setup:**
- 5V power supply for servos (separate from Pico)
- Servo signal → GPIO pin (3.3V logic)
- GND connection required
- USB power for Pico (or separate 5V)

**Features:**
- USB serial output for debugging
- Can operate without connected computer
- Power consumption: ~50mA (servos separate)

**Implementation:**
- [include/PWMControllerPico.h](../include/PWMControllerPico.h)
- [src/PWMControllerPico.cpp](../src/PWMControllerPico.cpp)

## Adding a New Platform

To add support for another platform (e.g., Arduino, STM32):

### 1. Create PWM Controller Header

```cpp
// include/PWMControllerMyPlatform.h
#ifndef PWM_CONTROLLER_MYPLATFORM_H
#define PWM_CONTROLLER_MYPLATFORM_H

#include "PWMController.h"

class PWMControllerMyPlatform : public PWMController {
public:
    PWMControllerMyPlatform();
    ~PWMControllerMyPlatform() override;
    
    bool initPin(uint32_t pin, uint32_t frequency) override;
    bool setPulseWidth(uint32_t pin, uint32_t pulse_width_us, uint32_t period_us) override;
    bool shutdownPin(uint32_t pin) override;
    const char* getPlatformName() const override { return "My Platform"; }
};

#endif
```

### 2. Create Implementation

```cpp
// src/PWMControllerMyPlatform.cpp
#include "PWMControllerMyPlatform.h"
#include <your_platform_lib.h>

// Implement methods...
```

### 3. Update CMakeLists.txt

Add to CMakeLists.txt:
```cmake
option(PLATFORM_MYPLATFORM "Build for MyPlatform" OFF)

if(PLATFORM_MYPLATFORM)
    list(APPEND GIMBAL_COMMON_SOURCES src/PWMControllerMyPlatform.cpp)
    # Link any required libraries
endif()
```

### 4. Update Example

```cpp
#ifdef PLATFORM_MYPLATFORM
#include "PWMControllerMyPlatform.h"
// ...
pwm_controller = std::make_shared<PWMControllerMyPlatform>();
#endif
```

## Troubleshooting

### RPi5 Build Fails

**"pigpio not found"**
- Normal - pigpio is optional for simulation mode
- To enable real GPIO: `sudo apt-get install pigpio`

**GPIO not responding**
- Start pigpio daemon: `sudo systemctl start pigpiod`
- Verify daemon: `sudo systemctl status pigpiod`

### Pico Build Fails

**"pico-sdk not found"**
```bash
git submodule update --init --recursive
```

**"ARM toolchain not found"**
```bash
sudo apt-get install gcc-arm-none-eabi
```

**Pico not detected during flashing**
- Hold BOOTSEL button while connecting USB
- Check: `lsblk` (should show RPI-RP2)
- Try manual: `cp build/bin/gimbal_example.uf2 /path/to/RPI-RP2/`

### Cross-compilation Issues

For building Pico binaries on non-ARM systems:

```bash
# Specify cross-compiler
cmake .. -DPLATFORM=PICO \
  -DCMAKE_TOOLCHAIN_FILE=lib/pico-sdk/cmake/preload/toolchain/generic_arm_gcc.cmake

make
```

## Performance Comparison

| Aspect | RPi5 | Pico |
|--------|------|------|
| **PWM Resolution** | Software-based | 16-bit hardware |
| **Frequency Accuracy** | OS-dependent | Precise hardware |
| **Latency** | ~10-50ms | <1ms |
| **Power Usage** | ~0.5W (idle) | ~50mA (GPIO only) |
| **Development** | Full Linux | Embedded C/C++ |
| **Cost** | $40-60 | $4-8 |
| **Use Case** | Always-on system | Embedded, battery-powered |

## Platform-Specific Build Options

### CMake Variables

```bash
# Set platform
cmake -DPLATFORM=RPi5    # (default)
cmake -DPLATFORM=PICO

# Set build type
cmake -DCMAKE_BUILD_TYPE=Release  # (default)
cmake -DCMAKE_BUILD_TYPE=Debug

# For Pico, specify pico-sdk path if not in lib/
cmake -DPICO_SDK_PATH=/path/to/pico-sdk
```

## Serial Output

### RPi5
Run example directly:
```bash
./build/bin/gimbal_example
```

### Pico
Connect USB and open serial terminal:
```bash
# Linux
miniterm.py /dev/ttyACM0 115200

# Or use screen
screen /dev/ttyACM0 115200
```

## Next Steps

1. **Choose your platform** - RPi5 for Linux development, Pico for embedded
2. **Build for your target** - Use provided build scripts
3. **Wire up servos** - Follow hardware guide in README.md
4. **Test with example** - Run gimbal_example on your platform
5. **Integrate into your project** - Link libgimbal.a library
