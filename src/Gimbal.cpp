#include "Gimbal.h"
#include <algorithm>
#include <cmath>
#include <iostream>

Gimbal::Gimbal(PWMController* pwm_controller, uint32_t pan_pin, uint32_t tilt_pin)
    : pwm_controller_(pwm_controller),
      pan_pin_(pan_pin),
      tilt_pin_(tilt_pin),
      current_pan_angle_(0.0f),
      current_tilt_angle_(0.0f),
      initialized_(false) {
}

Gimbal::Gimbal(std::shared_ptr<PWMController> pwm_controller, uint32_t pan_pin, uint32_t tilt_pin)
    : pwm_controller_(pwm_controller),
      pan_pin_(pan_pin),
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

    if (!pwm_controller_) {
        std::cerr << "PWM controller not set" << std::endl;
        return false;
    }

    std::cout << "Initializing gimbal on pins: pan=" << pan_pin_ 
              << ", tilt=" << tilt_pin_ 
              << " (Platform: " << pwm_controller_->getPlatformName() << ")" << std::endl;

    // Initialize PWM on both pins
    if (!pwm_controller_->initPin(pan_pin_, PWM_FREQUENCY)) {
        std::cerr << "Failed to initialize pan servo PWM" << std::endl;
        return false;
    }

    if (!pwm_controller_->initPin(tilt_pin_, PWM_FREQUENCY)) {
        std::cerr << "Failed to initialize tilt servo PWM" << std::endl;
        return false;
    }

    // Center gimbal
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

    if (pwm_controller_) {
        pwm_controller_->shutdownPin(pan_pin_);
        pwm_controller_->shutdownPin(tilt_pin_);
    }

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
    if (!pwm_controller_) {
        return false;
    }

    // Period = 1000000 microseconds / 50 Hz = 20000 microseconds
    uint32_t period_us = 1000000 / PWM_FREQUENCY;
    
    return pwm_controller_->setPulseWidth(pin, pulse_width, period_us);
}
