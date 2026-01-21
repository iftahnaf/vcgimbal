# RPi5/Pico 2D Gimbal Controller

[![Release](https://img.shields.io/github/v/release/iftahnaf/vcgimbal?label=release&color=0e8a16)](https://github.com/iftahnaf/vcgimbal/releases)
[![CI](https://img.shields.io/github/actions/workflow/status/iftahnaf/vcgimbal/ci.yml?branch=main&label=CI)](https://github.com/iftahnaf/vcgimbal/actions/workflows/ci.yml)
[![Platforms](https://img.shields.io/badge/platforms-RPi5%20%7C%20Pico-blue)](docs/platforms.md)

A C++ library for controlling a pan-tilt camera gimbal with MG90S servos on **Raspberry Pi 5 (Linux)** and **Raspberry Pi Pico (RP2040)**. The core API is platform-independent; platform-specific PWM drivers are selected at build time.

## Features
- Platform abstraction (`PWMController`) with RPi5 (pigpio) and Pico (pico-sdk) backends
- Gimbal API: init, set angles, query state, shutdown
- Servo-safe defaults for MG90S (50 Hz, 1000-2000 µs, ±90°)
- Automated CI and semantic-release publishing
- Release artifacts per platform: RPi5 `.bin`/`.elf`, Pico `.uf2`/`.bin`/`.elf`

## Building the Project

### Prerequisites (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake git
# Pico toolchain (Pico builds only)
sudo apt-get install -y gcc-arm-none-eabi
# Optional (RPi5 hardware PWM)
sudo apt-get install -y pigpio
```

### Quick Builds
```bash
# Raspberry Pi 5 (Linux host build)
bash scripts/build.sh RPi5

# Raspberry Pi Pico (generates .uf2/.bin/.elf)
bash scripts/build.sh PICO
```

Outputs:
- RPi5: build/bin/gimbal_example (release assets: gimbal_rpi5.bin, gimbal_rpi5.elf)
- Pico: build/bin/gimbal_example.uf2, .bin, .elf (release assets: gimbal_pico.uf2, gimbal_pico.bin, gimbal_pico.elf)

### Optional: Start pigpio (RPi5 hardware PWM)
```bash
sudo systemctl enable pigpiod
sudo systemctl start pigpiod
```

### Cross-Compilation / CI
CI builds on Ubuntu using the bundled pico-sdk submodule and ARM toolchain. See .github/workflows/ci.yml for reference.

More guides: [docs/quickstart.md](docs/quickstart.md) and [docs/platforms.md](docs/platforms.md).

## Usage

### Basic Example
```cpp
#include "Gimbal.h"
#include "PWMControllerRPi5.h"   // For Raspberry Pi 5 builds
#include "PWMControllerPico.h"   // For Pico builds (PICO_BUILD defined)

int main() {
  const uint32_t PAN_PIN = 17;
  const uint32_t TILT_PIN = 27;

#ifdef PICO_BUILD
  auto pwm = std::make_shared<PWMControllerPico>();
#else
  auto pwm = std::make_shared<PWMControllerRPi5>();
#endif

  Gimbal gimbal(pwm, PAN_PIN, TILT_PIN);

  if (!gimbal.init()) {
    return 1;
  }

  gimbal.setTipAngle(30.0f, -15.0f);  // Pan 30°, Tilt -15°

  float pan = gimbal.getPanAngle();
  float tilt = gimbal.getTiltAngle();

  gimbal.shutdown();
  return 0;
}
```

### Running the Example
```bash
cd build
./bin/gimbal_example           # RPi5 host run (simulation unless pigpio enabled)

# For Pico: flash build/bin/gimbal_example.uf2 to the board (see scripts/flash.sh)
```

## CI and Releases
- CI: GitHub Actions matrix builds for RPi5 and PICO (see .github/workflows/ci.yml).
- Releases: Automated by semantic-release on pushes to main (see .github/workflows/release.yml).
- Conventional commits: Use feat:, fix:, chore:, etc. to drive semantic versioning.
- Release assets: gimbal_rpi5.bin, gimbal_rpi5.elf, gimbal_pico.uf2, gimbal_pico.bin, gimbal_pico.elf

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
Sets up GPIO, PWM, and centers gimbal. Returns true on success.

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

Returns true on success.

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
- Class architecture and API
- Angle validation and conversion
- Logging and example code
- PWM signals logged but not actually applied to GPIO

### TODO - Hardware Integration
- Implement actual PWM control using pigpio library
- Add hardware error handling
- Implement servo calibration
- Add temperature and voltage monitoring

### TODO - Advanced Features
- Trajectory planning algorithms
- PID control for smooth motion
- Camera auto-tracking
- Speed/acceleration control
- Boundary checking and safety limits

## Configuration

### MG90S Servo Specifications
Located in include/Gimbal.h:

```cpp
static constexpr uint32_t PWM_FREQUENCY = 50;      // 50 Hz
```
