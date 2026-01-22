cmake_minimum_required(VERSION 3.10)

# Platform selection - can be overridden with: cmake -DPLATFORM=PICO
set(PLATFORM "RPi5" CACHE STRING "Target platform: RPi5 or PICO")

# Initialize project based on platform
if(PLATFORM STREQUAL "PICO")
    # Pico-specific setup (must be done before project())
    set(PICO_SDK_PATH "${CMAKE_SOURCE_DIR}/lib/pico-sdk" CACHE PATH "Path to Pico SDK")
    include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
    
    project(gimbal VERSION 1.0.0 LANGUAGES CXX C)
    
    # Initialize pico SDK
    pico_sdk_init()
    
    message(STATUS "Building for Raspberry Pi Pico")
else()
    project(gimbal VERSION 1.0.0 LANGUAGES CXX)
    message(STATUS "Building for Raspberry Pi 5")
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files - common to all platforms
set(GIMBAL_COMMON_SOURCES
    src/Gimbal.cpp
)

# Platform-specific PWM controller
if(PLATFORM STREQUAL "PICO")
    list(APPEND GIMBAL_COMMON_SOURCES src/PWMControllerPico.cpp)
    add_compile_definitions(PICO_BUILD=1)
else()
    list(APPEND GIMBAL_COMMON_SOURCES src/PWMControllerRPi5.cpp)
endif()

# Create gimbal library
add_library(gimbal_lib STATIC ${GIMBAL_COMMON_SOURCES})

# Platform-specific library linking
if(PLATFORM STREQUAL "PICO")
    target_link_libraries(gimbal_lib
        pico_stdlib
        hardware_pwm
        hardware_clocks
    )
    target_compile_definitions(gimbal_lib PUBLIC PICO_BUILD=1)
else()
    # RPi5 uses sysfs GPIO - no special dependencies needed
    message(STATUS "Building for Raspberry Pi 5 with sysfs GPIO control")
endif()

# Set library output directory
set_target_properties(gimbal_lib PROPERTIES
    OUTPUT_NAME gimbal
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# Example executable
add_executable(gimbal_example examples/example_basic.cpp)
target_link_libraries(gimbal_example gimbal_lib)

if(PLATFORM STREQUAL "PICO")
    # Enable USB serial output for Pico
    pico_enable_stdio_usb(gimbal_example 1)
    pico_enable_stdio_uart(gimbal_example 0)
    
    # Create .uf2 and .bin outputs for flashing
    pico_add_extra_outputs(gimbal_example)
endif()

# Set executable output directory
set_target_properties(gimbal_example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Print build summary
message(STATUS "")
message(STATUS "=== Build Configuration ===")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Platform: ${PLATFORM}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "")
message(STATUS "Targets:")
message(STATUS "  - gimbal_lib (static library)")
message(STATUS "  - gimbal_example (executable)")
message(STATUS "")
message(STATUS "Output directories:")
message(STATUS "  - Libraries: ${CMAKE_BINARY_DIR}/lib")
message(STATUS "  - Executables: ${CMAKE_BINARY_DIR}/bin")
message(STATUS "")

if(PLATFORM STREQUAL "PICO")
    message(STATUS "Pico notes:")
    message(STATUS "  - Firmware: ${CMAKE_BINARY_DIR}/bin/gimbal_example.uf2 (and .bin)")
    message(STATUS "  - For details, run: ./scripts/flash.sh")
    message(STATUS "")
endif()
