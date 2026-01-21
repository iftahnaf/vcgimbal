#include "Gimbal.h"
#include <algorithm>
#include <cmath>
#include <iostream>

Gimbal::Gimbal(uint32_t pan_pin, uint32_t tilt_pin)
    : pan_pin_(pan_pin),
      tilt_pin_(tilt_pin),
      current_pan_angle_(0.0f),
      current_tilt_angle_(0.0f),
      initialized_(false) {
}

Gimbal::~Gimbal() {
    if (initialized_) {
        shutdown();
    }
}

bool Gimbal::init() {
    if (initialized_) {
        std::cout << "Gimbal already initialized" << std::endl;
        return true;
    }

    // TODO: Initialize pigpio library
    // TODO: Set up GPIO pins as PWM outputs
    // TODO: Center gimbal at 0,0 degrees

    std::cout << "Initializing gimbal on pins: pan=" << pan_pin_ 
              << ", tilt=" << tilt_pin_ << std::endl;

    // Initialize to center position
    if (!setPWM(pan_pin_, MID_PULSE_WIDTH)) {
        std::cerr << "Failed to initialize pan servo" << std::endl;
        return false;
    }

    if (!setPWM(tilt_pin_, MID_PULSE_WIDTH)) {
        std::cerr << "Failed to initialize tilt servo" << std::endl;
        return false;
    }

    current_pan_angle_ = 0.0f;
    current_tilt_angle_ = 0.0f;
    initialized_ = true;

    std::cout << "Gimbal initialized successfully" << std::endl;
    return true;
}

void Gimbal::shutdown() {
    if (!initialized_) {
        return;
    }

    // TODO: Stop PWM on both pins
    // TODO: Cleanup pigpio resources

    std::cout << "Shutting down gimbal" << std::endl;
    initialized_ = false;
}

bool Gimbal::setTipAngle(float pan_angle, float tilt_angle) {
    if (!initialized_) {
        std::cerr << "Gimbal not initialized" << std::endl;
        return false;
    }

    // Validate angles
    if (!isValidAngle(pan_angle) || !isValidAngle(tilt_angle)) {
        std::cerr << "Invalid angle values. Pan: " << pan_angle 
                  << ", Tilt: " << tilt_angle 
                  << " (valid range: " << MIN_ANGLE << " to " << MAX_ANGLE << ")" << std::endl;
        return false;
    }

    // Convert angles to PWM pulse widths
    uint32_t pan_pulse = angleToPulseWidth(pan_angle);
    uint32_t tilt_pulse = angleToPulseWidth(tilt_angle);

    // Apply PWM signals to servos
    if (!setPWM(pan_pin_, pan_pulse)) {
        std::cerr << "Failed to set pan servo" << std::endl;
        return false;
    }

    if (!setPWM(tilt_pin_, tilt_pulse)) {
        std::cerr << "Failed to set tilt servo" << std::endl;
        return false;
    }

    current_pan_angle_ = pan_angle;
    current_tilt_angle_ = tilt_angle;

    std::cout << "Gimbal angles set - Pan: " << pan_angle 
              << "°, Tilt: " << tilt_angle << "°" << std::endl;

    return true;
}

float Gimbal::getPanAngle() const {
    return current_pan_angle_;
}

float Gimbal::getTiltAngle() const {
    return current_tilt_angle_;
}

bool Gimbal::isInitialized() const {
    return initialized_;
}

uint32_t Gimbal::angleToPulseWidth(float angle) const {
    // Clamp angle to valid range
    angle = std::clamp(angle, MIN_ANGLE, MAX_ANGLE);

    // Map angle [-90, 90] to pulse width [1000, 2000] microseconds
    // Formula: pulse = MID_PULSE_WIDTH + (angle / MAX_ANGLE) * (MAX_PULSE_WIDTH - MID_PULSE_WIDTH)
    uint32_t pulse_width = static_cast<uint32_t>(
        MID_PULSE_WIDTH + (angle / MAX_ANGLE) * (MAX_PULSE_WIDTH - MID_PULSE_WIDTH)
    );

    return pulse_width;
}

bool Gimbal::isValidAngle(float angle) const {
    return angle >= MIN_ANGLE && angle <= MAX_ANGLE;
}

bool Gimbal::setPWM(uint32_t pin, uint32_t pulse_width) {
    // TODO: Implement actual PWM control using pigpio library
    // For now, just log the action
    std::cout << "Setting PWM on pin " << pin << ": " 
              << pulse_width << " µs" << std::endl;
    return true;
}
