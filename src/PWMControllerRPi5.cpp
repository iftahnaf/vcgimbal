#include "PWMControllerRPi5.h"
#include <iostream>
#include <lgpio.h>

PWMControllerRPi5::PWMControllerRPi5() : chip_(-1) {}

PWMControllerRPi5::~PWMControllerRPi5() {
    shutdownLgpio();
}

bool PWMControllerRPi5::initLgpio() {
    chip_ = lgGpiochipOpen(0);
    if (chip_ < 0) {
        std::cerr << "Failed to open gpiochip0" << std::endl;
        return false;
    }
    std::cout << "PWMControllerRPi5: gpiochip0 opened" << std::endl;
    return true;
}

void PWMControllerRPi5::shutdownLgpio() {
    for (auto pin : claimed_pins_) {
        // Stop PWM and free pin
        lgTxPwm(chip_, pin, 50.0, 0.0);
        lgGpioFree(chip_, pin);
    }
    claimed_pins_.clear();
    pin_frequency_.clear();
    if (chip_ >= 0) {
        lgGpiochipClose(chip_);
        chip_ = -1;
    }
    std::cout << "PWMControllerRPi5: gpiochip0 closed" << std::endl;
}

bool PWMControllerRPi5::initPin(uint32_t pin, uint32_t frequency) {
    if (chip_ < 0) {
        if (!initLgpio()) {
            return false;
        }
    }
    if (claimed_pins_.count(pin)) {
        pin_frequency_[pin] = frequency;
        return true;
    }
    if (lgGpioClaimOutput(chip_, pin, 0) < 0) {
        std::cerr << "Failed to claim GPIO " << pin << " as output" << std::endl;
        return false;
    }
    claimed_pins_.insert(pin);
    pin_frequency_[pin] = frequency;
    std::cout << "PWMControllerRPi5: Initialized pin " << pin << " at " << frequency << " Hz" << std::endl;
    return true;
}

bool PWMControllerRPi5::setPulseWidth(uint32_t pin, uint32_t pulse_width_us, uint32_t period_us) {
    if (chip_ < 0 || !claimed_pins_.count(pin)) {
        std::cerr << "Pin " << pin << " not initialized" << std::endl;
        return false;
    }
    auto itf = pin_frequency_.find(pin);
    if (itf == pin_frequency_.end()) {
        std::cerr << "Frequency not set for pin " << pin << std::endl;
        return false;
    }
    double duty = (static_cast<double>(pulse_width_us) / static_cast<double>(period_us)) * 100.0;
    if (lgTxPwm(chip_, pin, static_cast<double>(itf->second), duty) < 0) {
        std::cerr << "Failed to set PWM on pin " << pin << std::endl;
        return false;
    }
    return true;
}

bool PWMControllerRPi5::shutdownPin(uint32_t pin) {
    if (!claimed_pins_.count(pin)) {
        return false;
    }
    lgTxPwm(chip_, pin, 50.0, 0.0);
    lgGpioFree(chip_, pin);
    claimed_pins_.erase(pin);
    pin_frequency_.erase(pin);
    std::cout << "PWMControllerRPi5: Shutdown pin " << pin << std::endl;
    return true;
}
