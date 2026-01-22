#include "PWMControllerRPi5.h"
#include <iostream>
#include <gpiod.h>

PWMControllerRPi5::PWMControllerRPi5() : chip_(nullptr) {}

PWMControllerRPi5::~PWMControllerRPi5() {
    shutdownGpiod();
}

bool PWMControllerRPi5::initGpiod() {
    chip_ = gpiod_chip_open("/dev/gpiochip0");
    if (!chip_) {
        std::cerr << "Failed to open GPIO chip" << std::endl;
        return false;
    }
    std::cout << "PWMControllerRPi5: GPIO chip opened successfully" << std::endl;
    return true;
}

void PWMControllerRPi5::shutdownGpiod() {
    for (auto &entry : requests_) {
        if (entry.second) {
            gpiod_line_request_release(entry.second);
        }
    }
    requests_.clear();

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

    // Build request and line configs per libgpiod v2 API
    gpiod_request_config *req_cfg = gpiod_request_config_new();
    if (!req_cfg) {
        std::cerr << "Failed to allocate request config" << std::endl;
        return false;
    }
    gpiod_request_config_set_consumer(req_cfg, "gimbal");

    gpiod_line_settings *settings = gpiod_line_settings_new();
    if (!settings) {
        std::cerr << "Failed to allocate line settings" << std::endl;
        gpiod_request_config_free(req_cfg);
        return false;
    }
    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
    gpiod_line_settings_set_output_value(settings, GPIOD_LINE_VALUE_INACTIVE);

    gpiod_line_config *line_cfg = gpiod_line_config_new();
    if (!line_cfg) {
        std::cerr << "Failed to allocate line config" << std::endl;
        gpiod_line_settings_free(settings);
        gpiod_request_config_free(req_cfg);
        return false;
    }

    unsigned int offsets[] = { pin };
    if (gpiod_line_config_add_line_settings(line_cfg, offsets, 1, settings) < 0) {
        std::cerr << "Failed to add line settings for pin " << pin << std::endl;
        gpiod_line_config_free(line_cfg);
        gpiod_line_settings_free(settings);
        gpiod_request_config_free(req_cfg);
        return false;
    }

    gpiod_line_settings_free(settings);

    gpiod_line_request *request = gpiod_chip_request_lines(chip_, req_cfg, line_cfg);
    gpiod_request_config_free(req_cfg);
    gpiod_line_config_free(line_cfg);

    if (!request) {
        std::cerr << "Failed to request GPIO line " << pin << std::endl;
        return false;
    }

    requests_[pin] = request;
    std::cout << "PWMControllerRPi5: Initialized pin " << pin
              << " with frequency " << frequency << " Hz" << std::endl;
    return true;
}

bool PWMControllerRPi5::setPulseWidth(uint32_t pin, uint32_t pulse_width_us, uint32_t period_us) {
    auto it = requests_.find(pin);
    if (it == requests_.end()) {
        std::cerr << "Pin " << pin << " not initialized" << std::endl;
        return false;
    }

    uint32_t duty_cycle = (pulse_width_us * 1000000) / period_us;
    gpiod_line_value value = (duty_cycle > 500000) ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE;

    if (gpiod_line_request_set_value(it->second, 0, value) < 0) {
        std::cerr << "Failed to set GPIO line " << pin << " value" << std::endl;
        return false;
    }

    std::cout << "PWMControllerRPi5: Set pin " << pin << " pulse width: "
              << pulse_width_us << " µs (duty: " << (duty_cycle / 10000) << "%)" << std::endl;
    return true;
}

bool PWMControllerRPi5::shutdownPin(uint32_t pin) {
    auto it = requests_.find(pin);
    if (it == requests_.end()) {
        return false;
    }
    gpiod_line_request_set_value(it->second, 0, GPIOD_LINE_VALUE_INACTIVE);
    gpiod_line_request_release(it->second);
    requests_.erase(it);
    std::cout << "PWMControllerRPi5: Shutdown pin " << pin << std::endl;
    return true;
}
