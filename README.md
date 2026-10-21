# Dora-707-Software

## DORA 707 is a device that autonomously follows a specific target. It issues commands to its motors based on incoming data in the form of (x, y, z) coordinates.

### Çelikkubbe Air Defense System - ESP32 Core (v0.1)

Core embedded software for the "Steel Dome" (Çelikkubbe) Air Defense Systems competition. Designed for the ESP32, this system handles real-time target tracking, precise motor control, and autonomous firing logic.

### Key Features
* **2-Axis Kinematics:** High-acceleration NEMA 17 stepper control via TB6600 drivers for rapid Pan/Tilt movements.
* **Dynamic PID Control:** Smooth target locking with distance-based dynamic tolerance and built-in relay auto-tuning.
* **Autonomous Firing:** MOSFET-driven AEG airsoft trigger mechanism with adjustable burst and semi-auto pulse configurations.
* **Tactical Safety:** Programmable no-fire zones (friendly fire prevention), watchdog timers, and strict ARM/DISARM states.
* **Serial API:** Fully integrated with the custom Web HUD for manual joystick override, system configuration, and live telemetry.
