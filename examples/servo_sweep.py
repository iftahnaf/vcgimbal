#!/usr/bin/env python3
import argparse
import sys
import time

try:
    import pigpio
except ImportError:
    print("pigpio Python module not found. Install pigpio v79 (source) or 'sudo apt install python3-pigpio' if available.")
    sys.exit(1)


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

    # Try default, then IPv4 and IPv6 loopback
    candidates = [
        (None, None),
        ("127.0.0.1", 8888),
        ("::1", 8888),
    ]

    pi = None
    for host, port in candidates:
        try:
            pi = pigpio.pi() if host is None else pigpio.pi(host, port)
            if pi and pi.connected:
                break
        except Exception:
            pass

    if not pi or not pi.connected:
        print("pigpio daemon not reachable. Start or restart it explicitly:")
        print("  sudo pkill pigpiod || true")
        print("  sudo pigpiod -s10 -t0")
        sys.exit(1)

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
