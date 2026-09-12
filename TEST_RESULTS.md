# Simulation Test Results

## Test Environment

- Simulation platform: Tinkercad Circuits
- Controller: Arduino Uno R3
- Motor driver: L293D
- Distance sensor: HC-SR04
- Servo scanner: positional micro servo
- Display: 16x2 LCD
- Serial Monitor: 9600 baud

## Locked Parameters

| Parameter | Value |
|---|---:|
| Obstacle threshold | 25 cm |
| Minimum escape distance | 15 cm |
| Servo left position | 160 degrees |
| Servo center position | 90 degrees |
| Servo right position | 20 degrees |
| Reverse duration | 500 ms |
| Turn duration | 600 ms |
| Sensor samples | 3 |

## Validation Cases

### 1. Clear Path

**Condition:** Front distance greater than 25 cm.

**Expected:** Robot continues forward.

**Observed:** Serial Monitor reported `Status: CLEAR | Action: FORWARD` and the LCD displayed the measured distance with `CLEAR - FORWARD`.

**Result:** PASSED

### 2. Obstacle Detection

**Condition:** Front distance at or below 25 cm.

**Expected:** Robot stops and reverses before scanning.

**Observed:** Serial Monitor reported obstacle detection followed by STOP and REVERSE. The LCD displayed `OBSTACLE DET` and then the reverse state.

**Result:** PASSED

### 3. Left Path Selection

**Condition:** Left scan distance greater than right scan distance.

**Expected:** Robot selects and turns left.

**Observed:** Serial Monitor reported `LEFT path is clearer` followed by `TURN LEFT`. The LCD displayed the corresponding path and turning state.

**Result:** PASSED

### 4. Right Path Selection

**Condition:** Right scan distance greater than left scan distance.

**Expected:** Robot selects and turns right.

**Observed:** Serial Monitor reported `RIGHT path is clearer` followed by `TURN RIGHT`. The LCD displayed the corresponding path and turning state.

**Result:** PASSED

### 5. Approximately Equal Paths

**Condition:** Left and right scan distances are equal in the simulation.

**Expected:** Use the programmed default right-turn path.

**Observed:** Serial Monitor reported `Both paths are equal` followed by `DEFAULT -> TURN RIGHT`.

**Result:** PASSED

### 6. Both Paths Blocked

**Condition:** Left and right distances both below 15 cm.

**Expected:** Do not select either side. Reverse and rescan.

**Observed:** Serial Monitor reported `BOTH PATHS BLOCKED` and `REVERSE + RESCAN`. The LCD displayed `BOTH BLOCKED / REVERSE+RESCAN`.

**Result:** PASSED

### 7. Invalid Ultrasonic Reading

**Condition:** No valid ultrasonic echo is received.

**Expected:** Stop and wait for a valid reading instead of treating the path as clear.

**Observed:** Serial Monitor reported `Invalid ultrasonic reading` and `STOP - Waiting for valid reading`. The LCD displayed `SENSOR ERROR / ROBOT STOPPED`.

**Result:** PASSED

### 8. LCD Integration

**Condition:** Robot changes between sensing, obstacle, reverse, scanning, turning and recovery states.

**Expected:** LCD updates to the corresponding concise state without changing the navigation logic.

**Observed:** LCD correctly displayed distance, clear/forward, obstacle, reverse, scanning, path, turning and blocked-path states during the integrated simulation.

**Result:** PASSED

## Overall Result

The integrated simulation successfully demonstrated continuous ultrasonic sensing, autonomous obstacle detection, stopping, reversing, servo-assisted left/right scanning, alternative-path selection, motor direction control, blocked-path recovery, invalid-reading safety handling, LCD feedback, and Serial Monitor diagnostics.

## Simulation Limitation

Tinkercad does not physically model the complete robot chassis moving through a 3D environment with the ultrasonic sensor mechanically rotating with the chassis. Consequently, repeated obstacle/recovery cycles can occur when a virtual target remains in the sensor's simulated field after a motor turn. This is a limitation of the simulation environment and is documented rather than hidden.
