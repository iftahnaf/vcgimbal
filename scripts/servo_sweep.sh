#!/usr/bin/env bash
set -euo pipefail

# Simple servo sweep using pigpio's pigs CLI
# Usage: sudo bash scripts/servo_sweep.sh [PAN_GPIO] [TILT_GPIO]
# Defaults: PAN=17, TILT=27

PAN=${1:-17}
TILT=${2:-27}

if ! command -v pigs >/dev/null 2>&1; then
  echo "Error: pigs not found. Install pigpio (v79) and ensure it's in PATH."
  echo "Install from source:"
  echo "  wget https://github.com/joan2937/pigpio/archive/refs/tags/v79.tar.gz"
  echo "  tar zxf v79.tar.gz && cd pigpio-79 && make && sudo make install && sudo ldconfig"
  exit 1
fi

# Ensure pigpiod is running
if ! pgrep -x pigpiod >/dev/null 2>&1; then
  echo "Starting pigpiod..."
  if command -v systemctl >/dev/null 2>&1; then
    sudo systemctl start pigpiod || sudo pigpiod
  else
    sudo pigpiod
  fi
  sleep 0.5
fi

center() {
  pigs s "$PAN" 1500
  pigs s "$TILT" 1500
}

sweep_once() {
  # Sweep 1000 → 2000 µs for PAN and TILT
  for pw in $(seq 1000 50 2000); do
    pigs s "$PAN" "$pw"
    pigs s "$TILT" "$((2000 - (pw - 1000)))"
    sleep 0.02
  done
  # Sweep back
  for pw in $(seq 2000 50 1000); do
    pigs s "$PAN" "$pw"
    pigs s "$TILT" "$((2000 - (pw - 1000)))"
    sleep 0.02
  done
}

echo "Centering servos (PAN=$PAN, TILT=$TILT)..."
center
sleep 0.5

echo "Sweeping servos... Press Ctrl+C to stop."
trap 'echo; echo "Centering and exiting"; center; exit 0' INT TERM

while true; do
  sweep_once
done
