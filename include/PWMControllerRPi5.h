#ifndef PWM_CONTROLLER_RPI5_H
#define PWM_CONTROLLER_RPI5_H

#include "PWMController.h"
#include <cstdint>
#include <map>

struct gpiod_chip;
struct gpiod_line_request;

/**
 * @class PWMControllerRPi5
 * @brief PWM controller for Raspberry Pi 5 using libgpiod v2
 * 
 * Implements GPIO control via libgpiod v2 API for RPi5/RPi4 systems.
 */
class PWMControllerRPi5 : public PWMController {
public:
    PWMControllerRPi5();
    ~PWMControllerRPi5() override;

    bool initPin(uint32_t pin, uint32_t frequency) override;
    bool setPulseWidth(uint32_t pin, uint32_t pulse_width_us, uint32_t period_us) override;
    bool shutdownPin(uint32_t pin) override;
    const char* getPlatformName() const override { return "Raspberry Pi 5 (libgpiod v2)"; }

private:
    struct gpiod_chip* chip_;
    std::map<uint32_t, struct gpiod_line_request*> requests_;

    bool initGpiod();
    void shutdownGpiod();
};

#endif // PWM_CONTROLLER_RPI5_H
