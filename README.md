# 🚗 Navix — Autonomous Robotic Vehicle

Navix is an autonomous robotic vehicle powered by the **Raspberry Pi Pico (RP2040)**. Designed for efficiency and modularity, Navix navigates independently using an ultrasonic radar system mounted on a servo motor, performing 180° environment scans to detect obstacles and make real-time navigation decisions.

## 🚀 Key Features

* **Dual-Core Architecture:** Fully leverages the RP2040's dual-core design.
    * **Core 1:** Deterministic sensing and servo positioning.
    * **Core 0:** Navigation logic and motor control.
    * **Synchronization:** Inter-core communication via FIFO/Mutexes to ensure lock-free, synchronized data transfer.
* **Modular Hardware:** Designed for easy upgrades, supporting future sensors and actuators.
* **Safety First:** Implementation of contingency logic for obstacle trapping (U-turn detection, retreat/reset cycles).
* **Optimized Power:** Separate power domains for logic (5V) and motors (7.4V) with voltage monitoring.

## 🛠 Hardware Stack

| Component | Purpose |
| :--- | :--- |
| **Raspberry Pi Pico** | Central Brain (RP2040) |
| **TB6612FNG** | Motor Driver |
| **HC-SR04** | Ultrasonic Radar |
| **SG90** | Servo for Radar Scanning |
| **NEO-6M** | GPS Module for Navigation |
| **Li-ion (7.4V)** | Motor Power |
| **Power Bank (5V)** | Logic Power (RPi Pico) |

*Note: The system includes custom voltage dividers to ensure safe 3.3V logic levels for the Radar ECHO and Battery monitoring pins.*

## 💻 Software Architecture

The firmware utilizes a robust architecture designed for low latency and high efficiency:

* **State Machine:** Implements a state-based control loop to handle movement and obstacle avoidance.
* **Interrupts (ISR):** Non-blocking radar polling and hardware-timer-based servo control.
* **Efficiency:** Uses `__wfi` (Wait For Interrupt) to minimize power consumption when the system is idle.

## 📈 Project Status (50% Complete)

- [x] Hardware: Motor, Radar, and Servo unit tests.
- [x] Hardware: Servo-Radar integration.
- [x] Basic navigation logic (obstacle avoidance).
- [ ] **In Progress:** GPS coordinate integration.
- [ ] **In Progress:** Navigation logic refinement.

### Out of Scope (Future Improvements)
* Computer Vision (Camera modules).
* Artificial Intelligence / Machine Learning.
* Voice recognition commands.
