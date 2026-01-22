#!/usr/bin/env python3
import argparse
import sys
import time
import subprocess
import platform

try:
    import pigpio
except ImportError:
    print("pigpio Python module not found. Install pigpio v79 (source) or 'sudo apt install python3-pigpio' if available.")
    sys.exit(1)


def ensure_pigpiod():
    pi = pigpio.pi()
    if pi is not None and pi.connected:
        return pi

    # Try starting pigpiod
    arch = platform.machine()
    t_flag = "-t0" if arch == "aarch64" else "-t1"
    # 10us sample rate is fine for servos
    cmd = ["sudo", "pigpiod", "-s10", t_flag]
    try:
        subprocess.run(cmd, check=True)
        time.sleep(0.5)
    except Exception as e:
        print(f"Failed to start pigpiod: {e}")

    pi = pigpio.pi()
    if not pi.connected:
        print("Unable to connect to pigpio daemon. Ensure pigpio is installed and pigpiod can run.")
        sys.exit(1)
    return pi


def center(pi, pan, tilt):
    pi.set_servo_pulsewidth(pan, 1500)
    pi.set_servo_pulsewidth(tilt, 1500)


def sweep(pi, pan, tilt, delay=0.02):
    for pw in range(1000, 2001, 25):
        pi.set_servo_pulsewidth(pan, pw)
        pi.set_servo_pulsewidth(tilt, 2000 - (pw - 1000))
        time.sleep(delay)
    for pw in range(2000, 999, -25):
        pi.set_servo_pulsewidth(pan, pw)
        pi.set_servo_pulsewidth(tilt, 2000 - (pw - 1000))
        time.sleep(delay)


def main():
    parser = argparse.ArgumentParser(description="Simple servo sweep using pigpio")
    parser.add_argument("--pan", type=int, default=17, help="GPIO for pan servo (default: 17)")
    parser.add_argument("--tilt", type=int, default=27, help="GPIO for tilt servo (default: 27)")
    args = parser.parse_args()

    pi = ensure_pigpiod()
    print(f"Connected to pigpio. Sweeping PAN={args.pan}, TILT={args.tilt}")

    try:
        center(pi, args.pan, args.tilt)
        time.sleep(0.5)
        print("Sweeping... Press Ctrl+C to stop.")
        while True:
            sweep(pi, args.pan, args.tilt)
    except KeyboardInterrupt:
        print("\nStopping. Centering and turning off servos...")
        center(pi, args.pan, args.tilt)
        time.sleep(0.5)
    finally:
        # Turn off servo pulses
        pi.set_servo_pulsewidth(args.pan, 0)
        pi.set_servo_pulsewidth(args.tilt, 0)
        pi.stop()


if __name__ == "__main__":
    main()
