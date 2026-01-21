# Quick Start

## Prerequisites (Ubuntu/Debian)
```
sudo apt-get update
sudo apt-get install -y build-essential cmake git
# Pico builds only
sudo apt-get install -y gcc-arm-none-eabi
# Optional: pigpio for RPi5 hardware PWM
sudo apt-get install -y pigpio
```

## Build
```
# RPi5
bash scripts/build.sh RPi5

# Pico (.uf2/.bin/.elf)
bash scripts/build.sh PICO
```

## Run / Flash
- RPi5: `./build/bin/gimbal_example` (sudo if pigpio hardware PWM enabled)
- Pico: copy `build/bin/gimbal_example.uf2` to the `RPI-RP2` drive

## Wiring (defaults)
- Pan servo signal → GPIO 17
- Tilt servo signal → GPIO 27
- Power 5V, common ground

## Basic Test
```cpp
Gimbal gimbal(17, 27);
if (gimbal.init()) {
  gimbal.setTipAngle(30.0f, -15.0f);
  gimbal.shutdown();
}
```
