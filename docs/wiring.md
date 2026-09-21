# Hardware Wiring & Pinout Guide

## Component Overview

- **Controller**: ESP32 Development Board
- **Motor Driver**: L298N Dual H-Bridge Driver
- **Sensors**: 2x Photoelectric Speed Encoder Modules (Slot Optical Sensors)
- **Motors**: 2x DC Gear Motors (Left & Right Wheels)

---

## ESP32 to L298N Motor Driver Pinout

| ESP32 GPIO | L298N Terminal | Function | Description |
|---|---|---|---|
| GPIO 5 | ENA | Left Motor Speed | PWM (LEDC Channel 0, 2 kHz, 8-bit) |
| GPIO 18 | IN1 | Left Motor Direction 1 | Motor Left Control Signal 1 |
| GPIO 19 | IN2 | Left Motor Direction 2 | Motor Left Control Signal 2 |
| GPIO 21 | ENB | Right Motor Speed | PWM (LEDC Channel 1, 2 kHz, 8-bit) |
| GPIO 22 | IN3 | Right Motor Direction 1 | Motor Right Control Signal 1 |
| GPIO 23 | IN4 | Right Motor Direction 2 | Motor Right Control Signal 2 |
| GND | GND | Ground Reference | Common ground with L298N and battery |

---

## Optical Wheel Encoders Pinout

| ESP32 GPIO | Sensor Module | Function | Configuration |
|---|---|---|---|
| GPIO 34 | Left Encoder Out | Left Wheel Pulse Count | Input Pull-up, Hardware ISR (`leftISR`, RISING edge) |
| GPIO 35 | Right Encoder Out | Right Wheel Pulse Count | Input Pull-up, Hardware ISR (`rightISR`, RISING edge) |
| 3.3V / 5V | VCC | Sensor Power | Module power supply |
| GND | GND | Sensor Ground | Common ground |

---

## Power Connections

- **L298N 12V Terminal**: Connected to Battery Pack Positive terminal.
- **L298N GND Terminal**: Connected to Battery Pack Negative terminal and ESP32 GND.
- **ESP32 VIN / 5V Pin**: Powered from L298N onboard 5V regulator output or dedicated power supply.
