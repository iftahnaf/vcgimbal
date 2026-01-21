#ifndef GIMBAL_H
#define GIMBAL_H

#include "PWMController.h"
#include <cstdint>
#include <memory>

/**
 * @class Gimbal
 * @brief Basic 2D gimbal controller for MG90S servo motors
 * 
 * Controls a pan-tilt gimbal with camera mounted on the tip.
 * Uses two MG90S servo motors for horizontal (pan) and vertical (tilt) movement.
 * Communicates with servos via PWM signals on GPIO pins.
 * 
 * Platform-independent: works with any PWMController implementation
 * (RPi5 with pigpio, Pico with pico-sdk, etc.)
 */
class Gimbal {
public:
    /**
     * @brief Constructor for Gimbal controller
     * @param pwm_controller Platform-specific PWM controller (must be initialized)
     * @param pan_pin GPIO pin number for pan servo motor
     * @param tilt_pin GPIO pin number for tilt servo motor
     */
    Gimbal(PWMController* pwm_controller, uint32_t pan_pin, uint32_t tilt_pin);
    
    /**
     * @brief Constructor with shared pointer (preferred for modern C++)
     * @param pwm_controller Shared pointer to PWM controller
     * @param pan_pin GPIO pin number for pan servo motor
     * @param tilt_pin GPIO pin number for tilt servo motor
     */
    Gimbal(std::shared_ptr<PWMController> pwm_controller, uint32_t pan_pin, uint32_t tilt_pin);

    /**
     * @brief Destructor - cleanup PWM and GPIO resources
     */
    ~Gimbal();

    /**
     * @brief Initialize the gimbal controller
     * Sets up GPIO pins, PWM, and centers the gimbal at (0, 0) degrees
     * @return true if initialization successful, false otherwise
     */
    bool init();

    /**
     * @brief Shut down the gimbal controller
     * Stops PWM signals and releases resources
     */
    void shutdown();

    /**
     * @brief Set the gimbal tip angles
     * @param pan_angle Pan angle in degrees (-90 to 90, where 0 is center)
     * @param tilt_angle Tilt angle in degrees (-90 to 90, where 0 is center)
     * @return true if angles are valid and set successfully, false otherwise
     */
    bool setTipAngle(float pan_angle, float tilt_angle);

    /**
     * @brief Get the current pan angle
     * @return Current pan angle in degrees
     */
    float getPanAngle() const;

    /**
     * @brief Get the current tilt angle
     * @return Current tilt angle in degrees
     */
    float getTiltAngle() const;

    /**
     * @brief Check if gimbal is initialized
     * @return true if initialized, false otherwise
     */
    bool isInitialized() const;

private:
    // PWM controller (platform-specific implementation)
    std::shared_ptr<PWMController> pwm_controller_;
    
    // GPIO and PWM configuration
    uint32_t pan_pin_;
    uint32_t tilt_pin_;
    
    // Servo angle tracking
    float current_pan_angle_;
    float current_tilt_angle_;
    
    // Initialization state
    bool initialized_;

    // PWM constants for MG90S servo motor
    // MG90S specifications:
    // - Operating voltage: 3-7V
    // - Pulse width: 1000-2000 microseconds
    // - 1000 µs = -90°, 1500 µs = 0°, 2000 µs = +90°
    static constexpr uint32_t PWM_FREQUENCY = 50;      // 50 Hz (20ms period)
    static constexpr uint32_t MIN_PULSE_WIDTH = 1000;  // 1000 µs for -90°
    static constexpr uint32_t MID_PULSE_WIDTH = 1500;  // 1500 µs for 0°
    static constexpr uint32_t MAX_PULSE_WIDTH = 2000;  // 2000 µs for +90°
    static constexpr float MAX_ANGLE = 90.0f;          // ±90 degrees
    static constexpr float MIN_ANGLE = -90.0f;

    /**
     * @brief Convert angle to PWM pulse width in microseconds
     * @param angle Angle in degrees (-90 to 90)
     * @return Pulse width in microseconds (1000-2000)
     */
    uint32_t angleToPulseWidth(float angle) const;

    /**
     * @brief Validate angle is within acceptable range
     * @param angle Angle in degrees
     * @return true if angle is valid, false otherwise
     */
    bool isValidAngle(float angle) const;

    /**
     * @brief Apply PWM signal to servo motor
     * @param pin GPIO pin number
     * @param pulse_width Pulse width in microseconds
     * @return true if successful, false otherwise
     */
    bool setPWM(uint32_t pin, uint32_t pulse_width);
};

#endif // GIMBAL_H
