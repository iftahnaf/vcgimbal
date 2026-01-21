#!/bin/bash
# flash.sh - Flash Pico firmware

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="${PROJECT_DIR}/build"
FIRMWARE="${BUILD_DIR}/bin/gimbal_example.uf2"

echo -e "${YELLOW}=== Gimbal Pico Flasher ===${NC}"
echo ""

# Check firmware exists
if [[ ! -f "$FIRMWARE" ]]; then
    echo -e "${RED}Error: Firmware not found at $FIRMWARE${NC}"
    echo "Build first with: bash $SCRIPT_DIR/build.sh PICO"
    exit 1
fi

echo "Firmware: $FIRMWARE"
echo ""

# Wait for Pico in bootloader mode
echo -e "${YELLOW}Waiting for Pico in bootloader mode...${NC}"
echo "Instructions:"
echo "  1. Disconnect Pico from USB"
echo "  2. Hold BOOTSEL button"
echo "  3. Connect Pico to USB (while holding BOOTSEL)"
echo "  4. Release BOOTSEL"
echo ""

# Find Pico mount point
PICO_MOUNT=""
for i in {1..30}; do
    if [[ -d "/media/$USER/RPI-RP2" ]]; then
        PICO_MOUNT="/media/$USER/RPI-RP2"
        break
    fi
    
    sleep 1
    echo -n "."
done

echo ""

if [[ -z "$PICO_MOUNT" ]]; then
    echo -e "${RED}Error: Pico not found${NC}"
    echo ""
    echo "Troubleshooting:"
    echo "  - Check USB connection"
    echo "  - Hold BOOTSEL while connecting"
    echo "  - Try: lsblk (to find mount point)"
    echo "  - Manual copy: cp $FIRMWARE /path/to/RPI-RP2/"
    exit 1
fi

echo -e "${GREEN}Found Pico at $PICO_MOUNT${NC}"
echo "Flashing..."

cp "$FIRMWARE" "$PICO_MOUNT/"

# Wait for unmount (flashing completes)
for i in {1..10}; do
    if ! mountpoint -q "$PICO_MOUNT" 2>/dev/null; then
        break
    fi
    sleep 1
done

echo -e "${GREEN}Flash complete!${NC}"
echo ""
echo "Pico is now running gimbal_example"
echo "Connect via serial (115200 baud) to see output"
