# UTRA2024

# Arduino Robot Control System

This Arduino project is a robot control system that utilizes infrared (IR) distance sensors for line following and an ultrasonic sensor for obstacle avoidance. The robot is designed to navigate through a predefined path, make course corrections, and avoid obstacles.

## Table of Contents

- [Overview](#overview)
- [Hardware Requirements](#hardware-requirements)
- [Dependencies](#dependencies)
- [Setup](#setup)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgments](#acknowledgments)

## Overview

The project is composed of two main sections:
1. **Line Following:** The robot uses two IR distance sensors to follow a line on the ground. The left and right sensors monitor the position of the line relative to the robot and make course corrections as needed.

2. **Obstacle Avoidance:** An ultrasonic sensor is used to detect obstacles in the robot's path. When an obstacle is detected, the robot will make decisions to navigate around it.

## Hardware Requirements

- Arduino board
- Infrared (IR) distance sensors (e.g., KY-033 TCRT5000)
- Ultrasonic sensor (e.g., HC-SR04)
- Motor driver module (e.g., L298N)
- DC motors
- Chassis for the robot
- Wheels and other mechanical components

## Dependencies

The project relies on the following external sources and libraries:

1. [Running DC Motor With Arduino Using L298N Motor Driver](https://www.instructables.com/Running-DC-Motor-With-Arduino-Using-L298N-Motor-Dr/) - Motor control with L298N.
2. [Arduino Tutorial: IR Distance Line Tracing with KY-033 TCRT5000](https://mschoeffler.com/2017/11/27/arduino-tutorial-ir-distance-line-tracing-line-tracking-sensor-mh-sensor-series-ky-033-tcrt5000/) - IR distance sensor tutorial.
3. [Simple Arduino and HC-SR04 Example](https://www.instructables.com/Simple-Arduino-and-HC-SR04-Example/) - HC-SR04 ultrasonic sensor example.

## Setup

1. Connect the hardware components according to the provided pin assignments and wiring details in the code.
2. Upload the Arduino code to your Arduino board using the Arduino IDE.

## Usage

- Power on the robot and observe its behavior as it follows lines and avoids obstacles.
- Adjust parameters such as speed, sensor thresholds, and delays based on your robot's characteristics and the environment.

## Contributing

Feel free to contribute to the project by creating issues or submitting pull requests. Any improvements or additional features are welcome!

## License

This project is licensed under the [MIT License](LICENSE).

## Acknowledgments

- Thanks to the authors of the external tutorials and examples used in this project.

---

Feel free to customize the README further based on your specific project details and requirements.
