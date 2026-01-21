# Platform Support

## Targets
- Raspberry Pi 5 (Linux, optional pigpio for hardware PWM)
- Raspberry Pi Pico (RP2040, pico-sdk firmware)

## Architecture
```
User Code (Gimbal)
       ↓
PWMController (abstract)
       ↓
┌──────────────────┬──────────────────┐
│                  │                  │
v                  v                  v
PWMControllerRPi5  PWMControllerPico  Custom…
```

## Build Commands
- RPi5: `bash scripts/build.sh RPi5`
- Pico: `bash scripts/build.sh PICO`

## Outputs
- RPi5: build/bin/gimbal_example (release: gimbal_rpi5.bin, gimbal_rpi5.elf)
- Pico: build/bin/gimbal_example.uf2, .bin, .elf (release: gimbal_pico.uf2, gimbal_pico.bin, gimbal_pico.elf)

## Defaults
- Frequency: 50 Hz (20 ms)
- Pulse width: 1000–2000 µs
- Angles: ±90°
- Pins: Pan GPIO 17, Tilt GPIO 27

## Flashing Pico
1) Put board in BOOTSEL mode
2) Copy build/bin/gimbal_example.uf2 to the RPI-RP2 drive

## Adding a Platform
Implement a `PWMController` subclass, add it to CMake, and select it via build option similar to existing RPi5/Pico backends.
