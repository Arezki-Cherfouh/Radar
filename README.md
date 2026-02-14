# Arduino Radar System with Processing Visualizer

A complete radar simulation system that uses an ultrasonic sensor mounted on a servo to detect objects and display them on a radar-style GUI.

## Features

- **Servo Scanning**: 180° automatic sweep with ultrasonic distance measurement
- **Manual Override**: Buttons to force servo direction (Left/Right)
- **Proximity Alarm**: Buzzer activates when objects <20cm detected
- **Real-time Visualization**: Processing sketch displays radar sweep with detected objects
- **Serial Communication**: Arduino sends angle/distance data to Processing

## Hardware Required

- Arduino (Uno/Nano/Mega)
- HC-SR04 Ultrasonic Sensor
- Servo motor (continuous rotation modified or standard)
- Passive buzzer
- 2x Push buttons (with pull-up resistors)
- Jumper wires
- Power supply (if needed)

## Wiring Diagram

```

Ultrasonic HC-SR04:
VCC → 5V
GND → GND
Trig → Pin 7
Echo → Pin 6

Servo:
Signal → Pin 9
VCC → 5V
GND → GND

Buzzer:
Positive → Pin 8
Negative → GND

Buttons (INPUT_PULLUP):
Left → Pin 2
Right → Pin 3
Other side → GND

```

## How It Works

### Arduino Side

1. Servo continuously sweeps from 0° to 180°
2. At each position, ultrasonic sensor measures distance
3. Data sent via Serial: `angle,distance.` format
4. Buzzer triggers if object <20cm
5. Buttons override direction temporarily

### Processing Side

1. Reads serial data from Arduino
2. Draws radar grid (arcs and angle lines)
3. Plots detected objects in real-time
4. Displays current angle and distance
5. Red sweep line shows servo position

## Installation

### Arduino

1. Install `Servo` library (built-in)
2. Upload `arduino_radar.ino` to your board

### Processing

1. Install Processing from [processing.org](https://processing.org)
2. Open `processing_radar.pde`
3. Change `COM3` to your Arduino port (e.g., `/dev/ttyUSB0` on Linux, `COM5` on Windows)
4. Run the sketch

## Customization

- **Alarm Distance**: Change `alarmDistance` variable (default 20cm)
- **Scan Speed**: Adjust `delay(30)` in loop
- **Visual Colors**: Modify RGB values in Processing draw functions
- **Servo Range**: Edit angle limits (0-180 default)

## Troubleshooting

| Issue               | Solution                                 |
| ------------------- | ---------------------------------------- |
| No serial data      | Check port name in Processing            |
| Erratic distances   | Ensure stable power to servo             |
| Buzzer always on    | Increase `alarmDistance` or check sensor |
| Servo jitter        | Add capacitor across 5V/GND              |
| No objects detected | Verify echo/trig pins and timing         |

## Applications

- Security systems
- Obstacle detection for robots
- Educational tool for radar principles
- Interactive art installations
- Room mapping experiments

## License

MIT - Free for learning and modification

```

```
