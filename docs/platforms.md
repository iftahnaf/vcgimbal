# Platform Support

## Targets
- **Raspberry Pi 5** (Linux, userspace PWM via lgpio library)
- **Raspberry Pi Pico** (RP2040, hardware PWM via pico-sdk)

## Architecture
```
User Code (Gimbal)
       ↓
PWMController (abstract interface)
       ↓
┌──────────────────┬──────────────────┐
│                  │                  │
v                  v                  v
PWMControllerRPi5  PWMControllerPico  Custom…
(lgpio)            (pico-sdk)
```

## Raspberry Pi 5 (RPi5)

### Backend: lgpio (Userspace PWM)
- **Driver**: lgpio (Light GPIO library)
- **Install**: `sudo apt install -y liblgpio-dev`
- **No daemon required**: Runs directly in user space
- **GPIO Pins**: GPIO 17 (pan), GPIO 27 (tilt)
- **PWM Control**: Direct hardware PWM via `lgTxPwm()` at 50 Hz
- **Permissions**: Requires GPIO group or sudo
- **Pulse Width**: 1000–2000 µs for ±90° servo range

### Build & Run
```bash
# Build
bash scripts/build.sh RPi5

# Run
sudo ./build/bin/gimbal_example
# OR (after adding user to gpio group):
./build/bin/gimbal_example
```

## Raspberry Pi Pico (RP2040)

### Backend: pico-sdk (Hardware PWM)
- **SDK**: Raspberry Pi Pico SDK (bundled as submodule)
- **PWM**: Direct hardware PWM controllers
- **Firmware**: UF2, ELF, BIN formats

### Build & Flash
```bash
# Build
bash scripts/build.sh PICO

# Flash (copy to RPI-RP2 drive or use picotool)
cp build/bin/gimbal_example.uf2 /media/$(whoami)/RPI-RP2/
# OR
bash scripts/flash.sh
```

## Servo Specifications (All Platforms)
- **Frequency**: 50 Hz (20 ms period)
- **Pulse Width**: 1000–2000 µs
  - 1000 µs = -90° (full left/down)
  - 1500 µs = 0° (center)
  - 2000 µs = +90° (full right/up)
- **Angle Range**: ±90° (configurable in GimbalConfig.h)
- Pins: Pan GPIO 17, Tilt GPIO 27

## Flashing Pico
1) Put board in BOOTSEL mode
2) Copy build/bin/gimbal_example.uf2 to the RPI-RP2 drive

## Adding a Platform
Implement a `PWMController` subclass, add it to CMake, and select it via build option similar to existing RPi5/Pico backends.
