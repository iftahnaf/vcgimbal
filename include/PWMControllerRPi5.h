#ifndef PWM_CONTROLLER_RPI5_H
#define PWM_CONTROLLER_RPI5_H

#include "PWMController.h"
#include <cstdint>
#include <set>

/**
 * @class PWMControllerRPi5
 * @brief PWM controller for Raspberry Pi 5 using sysfs GPIO
 * 
 * Implements GPIO control via sysfs interface for RPi5/RPi4 systems.
 */
class PWMControllerRPi5 : public PWMController {
public:
    PWMControllerRPi5();
    ~PWMControllerRPi5() override;

    bool initPin(uint32_t pin, uint32_t frequency) override;
    bool setPulseWidth(uint32_t pin, uint32_t pulse_width_us, uint32_t period_us) override;
    bool shutdownPin(uint32_t pin) override;
    const char* getPlatformName() const override { return "Raspberry Pi 5 (sysfs GPIO)"; }

private:
    std::set<uint32_t> exported_pins_;
    
    bool exportPin(uint32_t pin);
    bool unexportPin(uint32_t pin);
    bool writeGpio(uint32_t pin, int value);
};

#endif // PWM_CONTROLLER_RPI5_H
