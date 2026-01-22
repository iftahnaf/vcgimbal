#!/usr/bin/env python3
import argparse
import sys
import time

try:
    import lgpio
except ImportError:
    print("lgpio not found. Install with: sudo apt install -y python3-lgpio liblgpio-dev")
    sys.exit(1)


def duty_from_us(us: int) -> float:
    # 50 Hz -> 20,000 µs period; duty% = pulse_us / 20000 * 100
    return (us / 20000.0) * 100.0


def center(pi: int, pan: int, tilt: int):
    lgpio.tx_pwm(pi, pan, 50.0, duty_from_us(1500))
    lgpio.tx_pwm(pi, tilt, 50.0, duty_from_us(1500))


def sweep(pi: int, pan: int, tilt: int, delay: float = 0.02):
    for us in range(1000, 2001, 25):
        lgpio.tx_pwm(pi, pan, 50.0, duty_from_us(us))
        lgpio.tx_pwm(pi, tilt, 50.0, duty_from_us(2000 - (us - 1000)))
        time.sleep(delay)
    for us in range(2000, 999, -25):
        lgpio.tx_pwm(pi, pan, 50.0, duty_from_us(us))
        lgpio.tx_pwm(pi, tilt, 50.0, duty_from_us(2000 - (us - 1000)))
        time.sleep(delay)


def main():
    parser = argparse.ArgumentParser(description="Simple servo sweep using lgpio PWM")
    parser.add_argument("--pan", type=int, default=17, help="GPIO for pan servo (default: 17)")
    parser.add_argument("--tilt", type=int, default=27, help="GPIO for tilt servo (default: 27)")
    args = parser.parse_args()

    # Open gpiochip0 and claim outputs
    pi = lgpio.gpiochip_open(0)
    try:
        lgpio.gpio_claim_output(pi, args.pan)
        lgpio.gpio_claim_output(pi, args.tilt)

        print(f"Centering servos PAN={args.pan}, TILT={args.tilt}")
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
        # Stop PWM by setting 0% duty
        lgpio.tx_pwm(pi, args.pan, 50.0, 0.0)
        lgpio.tx_pwm(pi, args.tilt, 50.0, 0.0)
        # Free lines and close chip
        try:
            lgpio.gpio_free(pi, args.pan)
            lgpio.gpio_free(pi, args.tilt)
        finally:
            lgpio.gpiochip_close(pi)


if __name__ == "__main__":
    main()
