#ifndef PWM_CONTROLLER_PICO_H
#define PWM_CONTROLLER_PICO_H

#include "PWMController.h"
#include <cstdint>

/**
 * @class PWMControllerPico
 * @brief PWM controller for Raspberry Pi Pico using pico-sdk
 * 
 * Implements PWM control via pico-sdk for RP2040 microcontroller.
 */
class PWMControllerPico : public PWMController {
public:
    PWMControllerPico();
    ~PWMControllerPico() override;

    bool initPin(uint32_t pin, uint32_t frequency) override;
    bool setPulseWidth(uint32_t pin, uint32_t pulse_width_us, uint32_t period_us) override;
    bool shutdownPin(uint32_t pin) override;
    const char* getPlatformName() const override { return "Raspberry Pi Pico (pico-sdk)"; }

private:
    bool initialized_;
    
    /**
     * @brief Calculate PWM level from pulse width
     * @param pulse_width_us Pulse width in microseconds
     * @param period_us Period in microseconds
     * @return PWM level (0-65535)
     */
    uint16_t calculatePWMLevel(uint32_t pulse_width_us, uint32_t period_us) const;
};

#endif // PWM_CONTROLLER_PICO_H
