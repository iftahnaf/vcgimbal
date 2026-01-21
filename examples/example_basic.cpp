#include "Gimbal.h"
#include "PWMControllerRPi5.h"
#include "PWMControllerPico.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <memory>

/**
 * @brief Example usage of Gimbal controller on different platforms
 * 
 * This example demonstrates:
 * - Platform selection (RPi5 or Pico)
 * - Initializing the gimbal with appropriate PWM controller
 * - Setting various pan/tilt angles
 * - Sweeping the camera view
 * - Proper cleanup
 */

void delay_ms(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

int main() {
    std::cout << "=== Gimbal Control Example ===" << std::endl;
    
    // GPIO pin configuration
    // Adjust these based on your Raspberry Pi or Pico wiring
    const uint32_t PAN_PIN = 17;    // GPIO 17 for pan servo
    const uint32_t TILT_PIN = 27;   // GPIO 27 for tilt servo

    // Create platform-specific PWM controller
    std::shared_ptr<PWMController> pwm_controller;

#ifdef PICO_BUILD
    std::cout << "Running on Raspberry Pi Pico" << std::endl;
    pwm_controller = std::make_shared<PWMControllerPico>();
#else
    std::cout << "Running on Raspberry Pi 5" << std::endl;
    pwm_controller = std::make_shared<PWMControllerRPi5>();
#endif

    // Create gimbal controller instance
    Gimbal gimbal(pwm_controller, PAN_PIN, TILT_PIN);

    // Initialize gimbal
    if (!gimbal.init()) {
        std::cerr << "Failed to initialize gimbal" << std::endl;
        return 1;
    }

    std::cout << "\nGimbal initialized. Starting control sequence..." << std::endl;

    // Example 1: Center the gimbal
    std::cout << "\n--- Center Position ---" << std::endl;
    gimbal.setTipAngle(0.0f, 0.0f);
    delay_ms(1000);

    // Example 2: Pan left
    std::cout << "\n--- Pan Left ---" << std::endl;
    gimbal.setTipAngle(-45.0f, 0.0f);
    delay_ms(1000);

    // Example 3: Pan right
    std::cout << "\n--- Pan Right ---" << std::endl;
    gimbal.setTipAngle(45.0f, 0.0f);
    delay_ms(1000);

    // Example 4: Tilt up
    std::cout << "\n--- Tilt Up ---" << std::endl;
    gimbal.setTipAngle(0.0f, 45.0f);
    delay_ms(1000);

    // Example 5: Tilt down
    std::cout << "\n--- Tilt Down ---" << std::endl;
    gimbal.setTipAngle(0.0f, -45.0f);
    delay_ms(1000);

    // Example 6: Diagonal movement
    std::cout << "\n--- Diagonal Movement ---" << std::endl;
    gimbal.setTipAngle(-30.0f, 30.0f);
    delay_ms(1000);

    // Example 7: Sweep pattern
    std::cout << "\n--- Horizontal Sweep ---" << std::endl;
    for (int angle = -90; angle <= 90; angle += 15) {
        gimbal.setTipAngle(static_cast<float>(angle), 0.0f);
        delay_ms(500);
    }

    // Return to center
    std::cout << "\n--- Return to Center ---" << std::endl;
    gimbal.setTipAngle(0.0f, 0.0f);
    delay_ms(500);

    // Print final state
    std::cout << "\n--- Final Gimbal State ---" << std::endl;
    std::cout << "Pan Angle: " << gimbal.getPanAngle() << "°" << std::endl;
    std::cout << "Tilt Angle: " << gimbal.getTiltAngle() << "°" << std::endl;
    std::cout << "Initialized: " << (gimbal.isInitialized() ? "Yes" : "No") << std::endl;

    std::cout << "\nShutting down gimbal..." << std::endl;
    gimbal.shutdown();

    std::cout << "Example completed successfully!" << std::endl;

    return 0;
}
