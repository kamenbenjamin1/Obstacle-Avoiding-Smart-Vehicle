# Obstacle-Avoiding 4-Motor Robot (Servo + Ultrasonic) - Arduino Sketch

This repository contains an Arduino sketch for a simple obstacle-avoiding robot that uses:

- **4 DC motors** (driven in pairs with PWM speed control)
- **1 servo motor** to “look” left/center/right by rotating the ultrasonic sensor
- **1 HC-SR04-style ultrasonic sensor** for distance measurement
- A small **state machine** (`GO`, `BACKUP`, `TURN`) to avoid obstacles smoothly


## What the robot does

- Continuously measures the **minimum** distance in front of the robot by scanning:
  - right (servo ~40°)
  - center (servo ~90°) twice (averaged)
  - left (servo ~140°)
- If the path is clear, it drives forward with **speed depending on distance**.
- If an obstacle is too close:
  - it **stops immediately**
  - **backs up**
  - then **nudges a turn** (left or right) based on which side looks clearer
- Uses **turn direction memory** (`lastTurnDir`) to reduce left/right oscillation when distances tie.


## Hardware required

- Arduino (Uno/Nano/etc.)
- 4x DC motors + motor driver(s)
  - **Important:** this code assumes each motor has two direction pins and the motors are grouped by PWM channels.
- 1x Servo (e.g., SG90)
- 1x Ultrasonic sensor (HC-SR04 or compatible)
- External motor power supply (recommended)
- Common ground between Arduino and motor supply


## Pin mapping (as used in the code)

### Servo
- `servo_pin = 11`

### Motors 1 & 2 (group 1)
- `motor1_pin1 = 8`
- `motor1_pin2 = 9`
- `motor2_pin1 = 10`
- `motor2_pin2 = 12`
- `motor1_2_PWM = 6`

### Motors 3 & 4 (group 2)
- `motor3_pin1 = 3`
- `motor3_pin2 = 2`
- `motor4_pin1 = 4`
- `motor4_pin2 = 5`
- `motor3_4_PWM = 6`  ⚠️ (same PWM pin as motor1_2_PWM)

### Ultrasonic
- `trigPin = 13`
- `echoPin = 7`

## Important notes / likely issues to fix

### 1) Both motor PWM channels use the same pin, just to save Arduino I/O assignment 
The sketch sets:
- `motor1_2_PWM = 6`
- `motor3_4_PWM = 6`

That means both motor groups share the **same PWM signal** (and pin 6 is configured twice).  
If you intended **independent control**, assign a different PWM pin to `motor3_4_PWM` (e.g., 5 on Uno is already used as direction pin, so pick something like 3/9/10/11 depending on your board usage).

### 2) `senseSurface()` is to estimate the plane normal as seen by the sensor --> may not be needed 

### 3) Servo angles depend on your mounting
The code uses:
- right: ~40°
- center: ~90°
- left: ~140°

You may need to tune these if your servo orientation is flipped or your sensor mount differs.

### 4) Ultrasonic “no echo” behavior
If no echo is received, `senseDistance()` returns `999.0` as “far away”.  
This is intentional to avoid blocking.


## Behavior parameters (tunable)
const float STOP_DIST  = 20.0;  // cm -> emergency stop + backup
const float CLEAR_DIST = 40.0;  // cm -> considered “safe to go”
int s_delay = 150;              // servo settle delay (ms)
