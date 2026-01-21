cmake_minimum_required(VERSION 3.10)
project(gimbal VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform detection
if(NOT CMAKE_SYSTEM_PROCESSOR)
    set(CMAKE_SYSTEM_PROCESSOR ${CMAKE_HOST_SYSTEM_PROCESSOR})
endif()

message(STATUS "System Processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "System Name: ${CMAKE_SYSTEM_NAME}")

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

# Release build optimizations
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Debug build flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files for gimbal library
set(GIMBAL_SOURCES
    src/Gimbal.cpp
)

# Create gimbal library
add_library(gimbal_lib STATIC ${GIMBAL_SOURCES})

# Set library output directory
set_target_properties(gimbal_lib PROPERTIES
    OUTPUT_NAME gimbal
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# Example executable
add_executable(gimbal_example examples/example_basic.cpp)
target_link_libraries(gimbal_example gimbal_lib)

# Set executable output directory
set_target_properties(gimbal_example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Optional: Find and link pigpio library if available
# Uncomment this section once you're ready to use actual GPIO control
# find_package(pigpio QUIET)
# if(pigpio_FOUND)
#     message(STATUS "pigpio found, enabling GPIO control")
#     target_link_libraries(gimbal_lib ${pigpio_LIBRARIES})
#     target_include_directories(gimbal_lib PRIVATE ${pigpio_INCLUDE_DIRS})
#     target_compile_definitions(gimbal_lib PRIVATE USE_PIGPIO)
# else()
#     message(STATUS "pigpio not found - GPIO control will be simulated")
# endif()

# Print build summary
message(STATUS "")
message(STATUS "=== Build Configuration ===")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "")
message(STATUS "Targets:")
message(STATUS "  - gimbal_lib (static library)")
message(STATUS "  - gimbal_example (executable)")
message(STATUS "")
message(STATUS "Output directories:")
message(STATUS "  - Libraries: ${CMAKE_BINARY_DIR}/lib")
message(STATUS "  - Executables: ${CMAKE_BINARY_DIR}/bin")
message(STATUS "")
