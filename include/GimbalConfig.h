#ifndef GIMBAL_CONFIG_H
#define GIMBAL_CONFIG_H

/**
 * @file GimbalConfig.h
 * @brief Configuration constants for Gimbal controller
 * 
 * Modify these constants to adapt to different hardware or requirements
 */

// =============================================================================
// GPIO PIN CONFIGURATION
// =============================================================================

/// GPIO pin for pan servo motor
#define GIMBAL_PAN_PIN 17

/// GPIO pin for tilt servo motor
#define GIMBAL_TILT_PIN 27

// =============================================================================
// PWM CONFIGURATION (MG90S Servo Specifications)
// =============================================================================

/// PWM frequency in Hz (standard for servo: 50 Hz = 20ms period)
#define GIMBAL_PWM_FREQUENCY 50

/// Minimum pulse width in microseconds (servo at -90 degrees)
#define GIMBAL_MIN_PULSE_WIDTH 1000

/// Mid pulse width in microseconds (servo at 0 degrees/center)
#define GIMBAL_MID_PULSE_WIDTH 1500

/// Maximum pulse width in microseconds (servo at +90 degrees)
#define GIMBAL_MAX_PULSE_WIDTH 2000

// =============================================================================
// ANGLE CONFIGURATION
// =============================================================================

/// Maximum pan angle in degrees (left/right range: ±90)
#define GIMBAL_MAX_PAN_ANGLE 90.0f

/// Minimum pan angle in degrees
#define GIMBAL_MIN_PAN_ANGLE -90.0f

/// Maximum tilt angle in degrees (up/down range: ±90)
#define GIMBAL_MAX_TILT_ANGLE 90.0f

/// Minimum tilt angle in degrees
#define GIMBAL_MIN_TILT_ANGLE -90.0f

// =============================================================================
// OPTIONAL FEATURES (Define to enable)
// =============================================================================

/// Enable hardware PWM control (requires pigpio)
// #define GIMBAL_ENABLE_PIGPIO

/// Enable debug logging
// #define GIMBAL_DEBUG_LOGGING

/// Enable angle smoothing/interpolation
// #define GIMBAL_ENABLE_SMOOTHING

// =============================================================================
// SERVO-SPECIFIC CALIBRATION
// =============================================================================

/**
 * Different servo models may have slightly different pulse-to-angle mappings.
 * If your servo doesn't center at 1500µs or span exactly 1000-2000µs,
 * adjust these constants based on your servo's specifications.
 * 
 * For example, some digital servos:
 * - MG90S: 1000-2000 µs (standard)
 * - SG90:  1000-2000 µs (standard)
 * - HS-422: 900-2100 µs (wider range)
 * - MG996R: 1000-2000 µs (standard)
 */

// Define GIMBAL_CALIBRATION_SERVO to select preset values
// Available options: MG90S (default), SG90, HS422, MG996R
#define GIMBAL_CALIBRATION_SERVO MG90S

/**
 * If your servo behavior is off, you can manually set calibration points:
 * - At -90°: what pulse width centers the servo?
 * - At 0°: what pulse width is the true center?
 * - At +90°: what pulse width is the other extreme?
 */

// Example for custom servo calibration (uncomment to use):
// #define GIMBAL_MIN_PULSE_WIDTH_CALIBRATED 950
// #define GIMBAL_MID_PULSE_WIDTH_CALIBRATED 1500
// #define GIMBAL_MAX_PULSE_WIDTH_CALIBRATED 2050

// =============================================================================
// PERFORMANCE TUNING
// =============================================================================

/// Angle change threshold to consider as movement (degrees)
#define GIMBAL_ANGLE_DEADZONE 0.5f

/// Maximum angle change per command (degrees, 0 = unlimited)
#define GIMBAL_MAX_ANGLE_VELOCITY 0.0f

/// Smoothing factor for angle interpolation (0.0 to 1.0, 0 = disabled)
#define GIMBAL_SMOOTHING_FACTOR 0.0f

#endif // GIMBAL_CONFIG_H
