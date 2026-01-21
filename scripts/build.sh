#!/bin/bash
# build.sh - Build gimbal for different platforms

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Get script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

# Platform selection
PLATFORM="${1:-RPi5}"
BUILD_DIR="${PROJECT_DIR}/build"

echo -e "${YELLOW}=== Gimbal Build Script ===${NC}"
echo "Platform: $PLATFORM"
echo "Build directory: $BUILD_DIR"
echo ""

# Validate platform
if [[ "$PLATFORM" != "RPi5" && "$PLATFORM" != "PICO" ]]; then
    echo -e "${RED}Error: Invalid platform '$PLATFORM'${NC}"
    echo "Usage: $0 [RPi5|PICO]"
    exit 1
fi

# Check for pico-sdk if building for Pico
if [[ "$PLATFORM" == "PICO" ]]; then
    if [[ ! -d "${PROJECT_DIR}/lib/pico-sdk" ]]; then
        echo -e "${RED}Error: pico-sdk not found at ${PROJECT_DIR}/lib/pico-sdk${NC}"
        echo "Initialize with: git submodule update --init --recursive"
        exit 1
    fi
    
    # Check for ARM toolchain
    if ! command -v arm-none-eabi-gcc &> /dev/null; then
        echo -e "${YELLOW}Warning: ARM toolchain not found${NC}"
        echo "Install with: sudo apt-get install -y gcc-arm-none-eabi"
    fi
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure CMake
echo -e "${YELLOW}Configuring CMake for $PLATFORM...${NC}"
if [[ "$PLATFORM" == "PICO" ]]; then
    cmake .. -DPLATFORM=PICO -DCMAKE_BUILD_TYPE=Release
else
    cmake .. -DPLATFORM=RPi5 -DCMAKE_BUILD_TYPE=Release
fi

# Build
echo -e "${YELLOW}Building...${NC}"
make -j$(nproc)

echo ""
echo -e "${GREEN}Build complete!${NC}"
echo ""

# Output information
if [[ "$PLATFORM" == "PICO" ]]; then
    echo "Pico firmware:"
    echo "  Location: $BUILD_DIR/bin/gimbal_example.uf2"
    echo "  To flash:"
    echo "    1. Connect Pico to USB with BOOTSEL held"
    echo "    2. Run: cp $BUILD_DIR/bin/gimbal_example.uf2 /media/[username]/RPI-RP2/"
    echo "    Or use: bash $SCRIPT_DIR/flash.sh"
else
    echo "RPi5 executable:"
    echo "  Location: $BUILD_DIR/bin/gimbal_example"
    echo "  To run: $BUILD_DIR/bin/gimbal_example"
fi

echo ""
echo -e "${GREEN}Done!${NC}"
