#include "PWMControllerRPi5.h"
#include <iostream>
#include <gpiod.h>
#include <unistd.h>

PWMControllerRPi5::PWMControllerRPi5() : chip_(nullptr) {
}

PWMControllerRPi5::~PWMControllerRPi5() {
    shutdownGpiod();
}

bool PWMControllerRPi5::initGpiod() {
    // Open GPIO chip (usually /dev/gpiochip0 on RPi)
    chip_ = gpiod_chip_open("/dev/gpiochip0");
    if (!chip_) {
        std::cerr << "Failed to open GPIO chip" << std::endl;
        return false;
    }
    std::cout << "PWMControllerRPi5: GPIO chip opened successfully" << std::endl;
    return true;
}

void PWMControllerRPi5::shutdownGpiod() {
    // Release all lines
    for (auto& [pin, line] : lines_) {
        if (line) {
            gpiod_line_release(line);
        }
    }
    lines_.clear();
    
    // Close chip
    if (chip_) {
        gpiod_chip_close(chip_);
        chip_ = nullptr;
    }
    std::cout << "PWMControllerRPi5: GPIO chip closed" << std::endl;
}

bool PWMControllerRPi5::initPin(uint32_t pin, uint32_t frequency) {
    if (!chip_) {
        if (!initGpiod()) {
            return false;
        }
    }

    // Request line from chip
    struct gpiod_line* line = gpiod_chip_get_line(chip_, pin);
    if (!line) {
        std::cerr << "Failed to get GPIO line " << pin << std::endl;
        return false;
    }
    
    // Request line for output
    if (gpiod_line_request_output(line, "gimbal", 0) < 0) {
        std::cerr << "Failed to request GPIO line " << pin << " as output" << std::endl;
        return false;
    }
    
    lines_[pin] = line;
    std::cout << "PWMControllerRPi5: Initialized pin " << pin 
              << " with frequency " << frequency << " Hz" << std::endl;
    return true;
}

bool PWMControllerRPi5::setPulseWidth(uint32_t pin, uint32_t pulse_width_us, uint32_t period_us) {
    auto it = lines_.find(pin);
    if (it == lines_.end()) {
        std::cerr << "Pin " << pin << " not initialized" << std::endl;
        return false;
    }
    
    struct gpiod_line* line = it->second;
    
    // Calculate duty cycle (0-1000000, microsecond precision)
    uint32_t duty_cycle = (pulse_width_us * 1000000) / period_us;
    
    // Simulate PWM by toggling GPIO rapidly based on duty cycle
    // For actual hardware PWM, you would need to use a different approach
    // such as ioctl calls or a userspace PWM daemon
    
    // For now, set HIGH if duty cycle > 50%, LOW otherwise
    int value = (duty_cycle > 500000) ? 1 : 0;
    if (gpiod_line_set_value(line, value) < 0) {
        std::cerr << "Failed to set GPIO line " << pin << " value" << std::endl;
        return false;
    }
    
    std::cout << "PWMControllerRPi5: Set pin " << pin << " pulse width: " 
              << pulse_width_us << " µs (duty: " << (duty_cycle / 10000) << "%)" << std::endl;
    return true;
}

bool PWMControllerRPi5::shutdownPin(uint32_t pin) {
    auto it = lines_.find(pin);
    if (it == lines_.end()) {
        return false;
    }
    
    struct gpiod_line* line = it->second;
    gpiod_line_set_value(line, 0);
    gpiod_line_release(line);
    lines_.erase(it);
    
    std::cout << "PWMControllerRPi5: Shutdown pin " << pin << std::endl;
    return true;
}
