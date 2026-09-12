# Circuit and Pinout Reference

## Controller and Motor Driver

The project uses an Arduino Uno R3 as the controller and an L293D H-bridge driver to control two DC motors.

### Motor-control mapping

| Arduino | L293D | Function |
|---|---|---|
| D3 | Input 1 | Motor 1 control |
| D5 | Input 2 | Motor 1 control |
| D4 | Input 3 | Motor 2 control |
| D2 | Input 4 | Motor 2 control |

### Motor logic used by the program

- Forward: both motors commanded forward.
- Reverse: both motors commanded backward.
- Left turn: left motor reverse, right motor forward.
- Right turn: left motor forward, right motor reverse.
- Stop: all four motor-control inputs LOW.

## HC-SR04

| HC-SR04 | Connection |
|---|---|
| VCC | Arduino +5V rail |
| TRIG | Arduino D13 |
| ECHO | Arduino D12 |
| GND | Common GND rail |

## Servo Scanner

| Servo | Connection |
|---|---|
| Signal | Arduino D8 |
| Power | +5V rail |
| Ground | Common GND rail |

The program uses 90 degrees as the center position, 160 degrees for the left scan, and 20 degrees for the right scan.

## 16x2 LCD

The LCD is connected in 4-bit parallel mode.

| LCD signal | LCD pin | Connection |
|---|---:|---|
| VSS / GND | 1 | GND rail |
| VCC / VDD | 2 | +5V rail |
| V0 | 3 | Potentiometer wiper |
| RS | 4 | Arduino D6 |
| RW | 5 | GND rail |
| E | 6 | Arduino D7 |
| D0 | 7 | Not connected |
| D1 | 8 | Not connected |
| D2 | 9 | Not connected |
| D3 | 10 | Not connected |
| DB4 / D4 | 11 | Arduino D9 |
| DB5 / D5 | 12 | Arduino D10 |
| DB6 / D6 | 13 | Arduino D11 |
| DB7 / D7 | 14 | Arduino A0 |
| LED+ / A | 15 | +5V through 220 Ohm resistor |
| LED- / K | 16 | GND rail |

## LCD Contrast Potentiometer

- One outer pin -> +5V.
- Middle pin / wiper -> LCD V0.
- Other outer pin -> GND.

## Common Ground

Arduino ground, L293D ground, HC-SR04 ground, servo ground, and LCD ground connections use the common ground rail in the simulation.

## Power Concept

The Tinkercad prototype uses the Arduino's simulated 5V and common ground rails for the low-voltage electronics shown in the simulation. A physical build would require an appropriate independent motor power arrangement and a common ground between the controller and motor driver.
