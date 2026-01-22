# Quick Start

## Prerequisites (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake git

# RPi5 PWM control via lgpio userspace driver
sudo apt-get install -y liblgpio-dev

# Pico builds only
sudo apt-get install -y gcc-arm-none-eabi
```

## Hardware Wiring (Raspberry Pi 5)
```
Pan Servo Signal  → GPIO 17 (Physical Pin 11)
Tilt Servo Signal → GPIO 27 (Physical Pin 13)
Servo 5V Power    → 5V Rail (Physical Pin 2 or 4)
Servo Ground      → GND (Physical Pin 6, 9, 14, 20, 25, 30, 34, 39)
```

## Build
```bash
# RPi5
bash scripts/build.sh RPi5

# Pico (.uf2/.bin/.elf)
bash scripts/build.sh PICO
```

## Run / Flash
```bash
# RPi5 (requires GPIO access - use sudo or gpio group membership)
sudo ./build/bin/gimbal_example

# Or add user to gpio group (requires re-login)
sudo usermod -a -G gpio $(whoami)
./build/bin/gimbal_example

# Pico: copy build/bin/gimbal_example.uf2 to RPI-RP2 drive
```

## Quick Servo Test
```bash
# Python lgpio sweep
python3 examples/servo_lgpio_sweep.py
```

## Basic C++ Example
```cpp
#include "Gimbal.h"
#include "PWMControllerRPi5.h"

int main() {
  auto pwm = std::make_shared<PWMControllerRPi5>();
  Gimbal gimbal(pwm, 17, 27);
  
  if (gimbal.init()) {
    gimbal.setTipAngle(30.0f, -15.0f);   // Pan 30°, Tilt -15°
    gimbal.shutdown();
  }
  return 0;
}
```
