#ifndef PWM_CONTROLLER_RPI5_H
#define PWM_CONTROLLER_RPI5_H

#include "PWMController.h"
#include <cstdint>

/**
 * @class PWMControllerRPi5
 * @brief PWM controller for Raspberry Pi 5 using pigpio
 * 
 * Implements PWM control via pigpio library for RPi5/RPi4 systems.
 */
class PWMControllerRPi5 : public PWMController {
public:
    PWMControllerRPi5();
    ~PWMControllerRPi5() override;

    bool initPin(uint32_t pin, uint32_t frequency) override;
    bool setPulseWidth(uint32_t pin, uint32_t pulse_width_us, uint32_t period_us) override;
    bool shutdownPin(uint32_t pin) override;
    const char* getPlatformName() const override { return "Raspberry Pi 5 (pigpio)"; }

private:
    bool initialized_;
    
    /**
     * @brief Initialize pigpio library
     * @return true if successful
     */
    bool initPigpio();
    
    /**
     * @brief Shutdown pigpio library
     */
    void shutdownPigpio();
};

#endif // PWM_CONTROLLER_RPI5_H
