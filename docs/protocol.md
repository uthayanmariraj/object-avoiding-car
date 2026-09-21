# Bluetooth Communication Protocol Specification

## Connection Handshake

1. **Bluetooth Name**: `noobbot`
2. **Baud Rate (Serial Monitor)**: 115200 bps
3. **Handshake Sequence**:
   - When a Bluetooth client connects, the robot transmits `REQ\n` to indicate readiness for a command.
   - Upon completing execution of any received command, the robot responds with `REQ\n` to request the next command.
   - If the client disconnects, motor PWM channels are immediately set to 0 as a safety measure.

---

## Command Formats

All incoming commands must end with a newline character (`\n`).

### 1. Move Command
- **Format**: `CMD:MOVE:<distance_cm>`
- **Example**: `CMD:MOVE:50.0`
- **Behavior**:
  - Moves the robot forward by `<distance_cm>` centimeters using optical encoder feedback.
  - Standard move speed: PWM `185`.
  - **Special Case (`60.0` cm)**: If `<distance_cm>` equals `60.0`, the robot executes a pre-configured multi-segment sequence:
    1. Move forward 25 cm
    2. Turn 20° left
    3. Move forward 30 cm
    4. Turn 20° left

### 2. Turn Command
- **Format**: `CMD:TURN:<direction>:<degrees>`
- **Example**: `CMD:TURN:L:45.0` or `CMD:TURN:R:90.0`
- **Parameters**:
  - `<direction>`: `L` for Left, `R` (or any other character) for Right.
  - `<degrees>`: Target turn angle in degrees.
- **Behavior**:
  - Rotates the robot in place until the target optical encoder pulse count is met (`pulses = degrees * 0.295`).
