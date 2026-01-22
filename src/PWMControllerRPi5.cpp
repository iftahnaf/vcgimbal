#include "PWMControllerRPi5.h"
#include <iostream>
#include <fstream>
#include <string>

PWMControllerRPi5::PWMControllerRPi5() {
}

PWMControllerRPi5::~PWMControllerRPi5() {
    // Unexport all pins
    for (uint32_t pin : exported_pins_) {
        unexportPin(pin);
    }
}

bool PWMControllerRPi5::exportPin(uint32_t pin) {
    if (exported_pins_.count(pin)) {
        return true;  // Already exported
    }

    std::ofstream export_file("/sys/class/gpio/export");
    if (!export_file.is_open()) {
        std::cerr << "Failed to open GPIO export" << std::endl;
        return false;
    }
    
    export_file << pin << std::endl;
    export_file.close();
    
    exported_pins_.insert(pin);
    std::cout << "PWMControllerRPi5: Exported GPIO pin " << pin << std::endl;
    return true;
}

bool PWMControllerRPi5::unexportPin(uint32_t pin) {
    if (!exported_pins_.count(pin)) {
        return true;
    }

    std::ofstream unexport_file("/sys/class/gpio/unexport");
    if (!unexport_file.is_open()) {
        return false;
    }
    
    unexport_file << pin << std::endl;
    unexport_file.close();
    
    exported_pins_.erase(pin);
    std::cout << "PWMControllerRPi5: Unexported GPIO pin " << pin << std::endl;
    return true;
}

bool PWMControllerRPi5::writeGpio(uint32_t pin, int value) {
    std::string path = "/sys/class/gpio/gpio" + std::to_string(pin) + "/value";
    std::ofstream value_file(path);
    if (!value_file.is_open()) {
        std::cerr << "Failed to write GPIO value for pin " << pin << std::endl;
        return false;
    }
    
    value_file << (value ? 1 : 0) << std::endl;
    value_file.close();
    return true;
}

bool PWMControllerRPi5::initPin(uint32_t pin, uint32_t frequency) {
    if (!exportPin(pin)) {
        return false;
    }

    // Set direction to output
    std::string direction_path = "/sys/class/gpio/gpio" + std::to_string(pin) + "/direction";
    std::ofstream direction_file(direction_path);
    if (!direction_file.is_open()) {
        std::cerr << "Failed to set GPIO direction for pin " << pin << std::endl;
        return false;
    }
    
    direction_file << "out" << std::endl;
    direction_file.close();
    
    std::cout << "PWMControllerRPi5: Initialized pin " << pin 
              << " with frequency " << frequency << " Hz" << std::endl;
    return true;
}

bool PWMControllerRPi5::setPulseWidth(uint32_t pin, uint32_t pulse_width_us, uint32_t period_us) {
    // Calculate duty cycle
    uint32_t duty_cycle = (pulse_width_us * 1000000) / period_us;
    
    // Set HIGH if duty cycle > 50%, LOW otherwise
    // For actual hardware PWM, you'd need to use PWM sysfs or device tree overlays
    int value = (duty_cycle > 500000) ? 1 : 0;
    
    if (!writeGpio(pin, value)) {
        return false;
    }
    
    std::cout << "PWMControllerRPi5: Set pin " << pin << " pulse width: " 
              << pulse_width_us << " µs (duty: " << (duty_cycle / 10000) << "%)" << std::endl;
    return true;
}

bool PWMControllerRPi5::shutdownPin(uint32_t pin) {
    writeGpio(pin, 0);
    return unexportPin(pin);
}
