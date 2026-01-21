#include "Gimbal.h"
#include <iostream>
#include <thread>
#include <chrono>

/**
 * @brief Example usage of Gimbal controller
 * 
 * This example demonstrates:
 * - Initializing the gimbal
 * - Setting various pan/tilt angles
 * - Sweeping the camera view
 * - Proper cleanup
 */

void delay_ms(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

int main() {
    std::cout << "=== Gimbal Control Example ===" << std::endl;
    std::cout << "RPi5 2D Gimbal with MG90S Servo Motors" << std::endl << std::endl;

    // GPIO pin configuration
    // Adjust these based on your Raspberry Pi wiring
    const uint32_t PAN_PIN = 17;    // GPIO 17 for pan servo
    const uint32_t TILT_PIN = 27;   // GPIO 27 for tilt servo

    // Create gimbal controller instance
    Gimbal gimbal(PAN_PIN, TILT_PIN);

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
