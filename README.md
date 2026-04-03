# 🚗 Navix — Autonomous Robotic Vehicle

Navix is an autonomous robotic vehicle powered by the Raspberry Pi Pico, 
built around the high-performance RP2040 dual-core microcontroller. 
The platform navigates independently using an ultrasonic radar system 
mounted on a servo motor, performing 180° environment scans to detect 
obstacles and make real-time navigation decisions.

The RP2040 dual-core architecture is fully leveraged: Core 1 handles 
all radar sensing and servo positioning with deterministic timing, 
while Core 0 runs the navigation logic and motor control. Both cores 
communicate through a FIFO protocol, ensuring synchronized and 
lock-free data transfer.

Navix is designed as a modular and expandable platform. While the 
current version relies on ultrasonic sensing, the architecture is 
built to support future upgrades such as computer vision, TinyML 
gesture recognition, and natural language command modules.
