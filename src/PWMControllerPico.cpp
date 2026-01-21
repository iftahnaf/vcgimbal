#include "PWMControllerPico.h"
#include <iostream>
#include <cmath>

// Platform-specific includes - only compile on Pico
#ifdef PICO_BUILD
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#endif

PWMControllerPico::PWMControllerPico() : initialized_(false) {
}

PWMControllerPico::~PWMControllerPico() {
    // Cleanup handled by pico-sdk
}

bool PWMControllerPico::initPin(uint32_t pin, uint32_t frequency) {
#ifdef PICO_BUILD
    // Initialize GPIO as PWM
    gpio_set_function(pin, GPIO_FUNC_PWM);
    
    // Get PWM slice for this pin
    uint slice_num = pwm_gpio_to_slice_num(pin);
    
    // Calculate clock divider for desired frequency
    // Formula: PWM frequency = clock_speed / (wrap * divider)
    // Default clock speed is 125 MHz
    uint32_t clock_speed = clock_get_hz(clk_sys);
    float divider = (float)clock_speed / (frequency * 65536.0f);  // 65536 is max wrap value
    
    if (divider < 1.0f) {
        divider = 1.0f;
    }
    
    pwm_set_clkdiv(slice_num, divider);
    pwm_set_wrap(slice_num, 65535);  // Full range for 16-bit resolution
    pwm_set_enabled(slice_num, true);
    
    std::cout << "PWMControllerPico: Initialized pin " << pin 
              << " with frequency " << frequency << " Hz" << std::endl;
#else
    // Simulation mode
    std::cout << "PWMControllerPico: Initialized pin " << pin 
              << " with frequency " << frequency << " Hz (simulation)" << std::endl;
#endif
    
    return true;
}

bool PWMControllerPico::setPulseWidth(uint32_t pin, uint32_t pulse_width_us, uint32_t period_us) {
    uint16_t pwm_level = calculatePWMLevel(pulse_width_us, period_us);

#ifdef PICO_BUILD
    // Get PWM channel for this pin
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint channel = pwm_gpio_to_channel(pin);
    
    // Set PWM level
    pwm_set_chan_level(slice_num, channel, pwm_level);
    
    std::cout << "PWMControllerPico: Set pin " << pin << " level: " 
              << pwm_level << "/65535 (pulse: " << pulse_width_us << " µs)" << std::endl;
#else
    // Simulation mode
    std::cout << "PWMControllerPico: Set pin " << pin << " pulse width: " 
              << pulse_width_us << " µs (level: " << pwm_level << "/65535) (simulation)" << std::endl;
#endif
    
    return true;
}

bool PWMControllerPico::shutdownPin(uint32_t pin) {
#ifdef PICO_BUILD
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint channel = pwm_gpio_to_channel(pin);
    
    pwm_set_chan_level(slice_num, channel, 0);
    pwm_set_enabled(slice_num, false);
    
    std::cout << "PWMControllerPico: Shutdown pin " << pin << std::endl;
#else
    // Simulation mode
    std::cout << "PWMControllerPico: Shutdown pin " << pin << " (simulation)" << std::endl;
#endif
    
    return true;
}

uint16_t PWMControllerPico::calculatePWMLevel(uint32_t pulse_width_us, uint32_t period_us) const {
    // Convert pulse width to PWM level (0-65535)
    // PWM level = (pulse_width / period) * 65535
    if (period_us == 0) {
        return 0;
    }
    
    float ratio = (float)pulse_width_us / (float)period_us;
    return static_cast<uint16_t>(ratio * 65535.0f);
}
