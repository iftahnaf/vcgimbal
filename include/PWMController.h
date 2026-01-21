#ifndef PWM_CONTROLLER_H
#define PWM_CONTROLLER_H

#include <cstdint>

/**
 * @class PWMController
 * @brief Platform-agnostic PWM controller interface
 * 
 * Abstract interface for PWM control across different platforms:
 * - Raspberry Pi 5 (using pigpio)
 * - Raspberry Pi Pico (using pico-sdk)
 */
class PWMController {
public:
    virtual ~PWMController() = default;

    /**
     * @brief Initialize PWM on a GPIO pin
     * @param pin GPIO pin number
     * @param frequency PWM frequency in Hz
     * @return true if successful
     */
    virtual bool initPin(uint32_t pin, uint32_t frequency) = 0;

    /**
     * @brief Set PWM duty cycle via pulse width
     * @param pin GPIO pin number
     * @param pulse_width_us Pulse width in microseconds
     * @param period_us Period in microseconds (1000000 / frequency)
     * @return true if successful
     */
    virtual bool setPulseWidth(uint32_t pin, uint32_t pulse_width_us, uint32_t period_us) = 0;

    /**
     * @brief Shutdown PWM on a pin
     * @param pin GPIO pin number
     * @return true if successful
     */
    virtual bool shutdownPin(uint32_t pin) = 0;

    /**
     * @brief Get platform name for logging
     * @return Platform identifier string
     */
    virtual const char* getPlatformName() const = 0;
};

#endif // PWM_CONTROLLER_H
