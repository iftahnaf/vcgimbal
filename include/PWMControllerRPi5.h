#ifndef PWM_CONTROLLER_RPI5_H
#define PWM_CONTROLLER_RPI5_H

#include "PWMController.h"
#include <cstdint>
#include <map>
#include <memory>

struct gpiod_chip;
struct gpiod_line;

/**
 * @class PWMControllerRPi5
 * @brief PWM controller for Raspberry Pi 5 using libgpiod
 * 
 * Implements PWM control via libgpiod library for RPi5/RPi4 systems.
 */
class PWMControllerRPi5 : public PWMController {
public:
    PWMControllerRPi5();
    ~PWMControllerRPi5() override;

    bool initPin(uint32_t pin, uint32_t frequency) override;
    bool setPulseWidth(uint32_t pin, uint32_t pulse_width_us, uint32_t period_us) override;
    bool shutdownPin(uint32_t pin) override;
    const char* getPlatformName() const override { return "Raspberry Pi 5 (libgpiod)"; }

private:
    struct gpiod_chip* chip_;
    std::map<uint32_t, struct gpiod_line*> lines_;
    
    /**
     * @brief Initialize libgpiod and open GPIO chip
     * @return true if successful
     */
    bool initGpiod();
    
    /**
     * @brief Shutdown libgpiod
     */
    void shutdownGpiod();
};

#endif // PWM_CONTROLLER_RPI5_H
