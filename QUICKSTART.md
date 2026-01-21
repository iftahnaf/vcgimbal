# Quick Start Guide

## 1. Prerequisites

```bash
# Update system packages
sudo apt-get update
sudo apt-get install -y build-essential cmake

# Optional: Install pigpio for real GPIO control
sudo apt-get install -y pigpio
sudo systemctl enable pigpiod
sudo systemctl start pigpiod
```

## 2. Build the Project

```bash
cd ~/dev/gimbal

# Create build directory
mkdir -p build
cd build

# Generate and compile
cmake ..
make

# Test the build
./bin/gimbal_example
```

## 3. Connect Hardware

### Wiring Diagram

```
Raspberry Pi 5                          Servo Motors
┌─────────────────┐                    
│                 │                    
│  GPIO 17 ──────→│─── Pan Servo Signal (Yellow/Orange)
│  GPIO 27 ──────→│─── Tilt Servo Signal (Yellow/Orange)
│  5V      ──────→├─── Servo Power (Red)
│  GND     ──────→├─── Servo Ground (Black/Brown)
│                 │
└─────────────────┘
```

**Pin Configuration (can be customized):**
- Pan Servo: GPIO 17
- Tilt Servo: GPIO 27

## 4. Enable Hardware Control

Once hardware is connected:

### Step 1: Enable pigpio in CMakeLists.txt

Edit `CMakeLists.txt` and uncomment the pigpio section:

```cmake
# Uncomment lines 47-52 to enable pigpio:
find_package(pigpio QUIET)
if(pigpio_FOUND)
    message(STATUS "pigpio found, enabling GPIO control")
    target_link_libraries(gimbal_lib ${pigpio_LIBRARIES})
    target_include_directories(gimbal_lib PRIVATE ${pigpio_INCLUDE_DIRS})
    target_compile_definitions(gimbal_lib PRIVATE USE_PIGPIO)
else()
    message(STATUS "pigpio not found - GPIO control will be simulated")
endif()
```

### Step 2: Rebuild

```bash
cd ~/dev/gimbal/build
cmake ..
make
```

### Step 3: Run with Hardware

```bash
# Must run as root for GPIO access
sudo ./bin/gimbal_example
```

## 5. Basic Usage

Create a simple test file `test_gimbal.cpp`:

```cpp
#include "Gimbal.h"

int main() {
    Gimbal gimbal(17, 27);  // GPIO 17 for pan, GPIO 27 for tilt
    
    if (!gimbal.init()) {
        return 1;
    }
    
    // Pan left 45 degrees, tilt up 30 degrees
    gimbal.setTipAngle(-45.0f, 30.0f);
    
    // Get current position
    printf("Pan: %.1f°, Tilt: %.1f°\n", 
           gimbal.getPanAngle(), 
           gimbal.getTiltAngle());
    
    gimbal.shutdown();
    return 0;
}
```

## 6. Common Operations

### Center the gimbal
```cpp
gimbal.setTipAngle(0.0f, 0.0f);
```

### Pan left (45°)
```cpp
gimbal.setTipAngle(-45.0f, 0.0f);
```

### Pan right (45°)
```cpp
gimbal.setTipAngle(45.0f, 0.0f);
```

### Tilt up (45°)
```cpp
gimbal.setTipAngle(0.0f, 45.0f);
```

### Tilt down (45°)
```cpp
gimbal.setTipAngle(0.0f, -45.0f);
```

### Diagonal movement
```cpp
gimbal.setTipAngle(-30.0f, 30.0f);  // Pan left, tilt up
```

## 7. Troubleshooting

### "Gimbal not initialized" error
- Make sure to call `gimbal.init()` before `setTipAngle()`

### "Invalid angle values" error
- Angles must be between -90 and +90 degrees
- Pan: -90 (left) to +90 (right)
- Tilt: -90 (down) to +90 (up)

### Servo not responding (hardware mode)
1. Check pigpio daemon: `sudo systemctl status pigpiod`
2. Verify GPIO pin connections
3. Check power supply voltage (5V)
4. Ensure servo signal wires are connected correctly

### Build errors
- Ensure C++17 support: `g++ --version` (should be 5.0+)
- Check CMake version: `cmake --version` (3.10+)

## 8. Next Steps

- Implement camera auto-tracking
- Add trajectory planning algorithms
- Create a ROS interface
- Add unit tests
- Implement speed/acceleration control

See [README.md](README.md) for full documentation.
