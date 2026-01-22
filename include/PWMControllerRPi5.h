#ifndef PWM_CONTROLLER_RPI5_H
#define PWM_CONTROLLER_RPI5_H

#include "PWMController.h"
#include <cstdint>
#include <set>
#include <map>

/**
 * @class PWMControllerRPi5
 * @brief PWM controller for Raspberry Pi 5 using lgpio
 * 
 * Generates PWM for servos via lgpio's tx_pwm at 50 Hz.
 */
class PWMControllerRPi5 : public PWMController {
public:
    PWMControllerRPi5();
    ~PWMControllerRPi5() override;

    bool initPin(uint32_t pin, uint32_t frequency) override;
    bool setPulseWidth(uint32_t pin, uint32_t pulse_width_us, uint32_t period_us) override;
    bool shutdownPin(uint32_t pin) override;
    const char* getPlatformName() const override { return "Raspberry Pi 5 (lgpio)"; }

private:
    int chip_;
    std::set<uint32_t> claimed_pins_;
    std::map<uint32_t, uint32_t> pin_frequency_;

    bool initLgpio();
    void shutdownLgpio();
};

#endif // PWM_CONTROLLER_RPI5_H
