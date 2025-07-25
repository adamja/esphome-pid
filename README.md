# ESP-PID Controller

A custom ESPHome component for implementing PID control on ESP32 devices.

## Configuration Parameters

These options provide fine-grained control of the PID behavior and integration within your ESPHome project.

| Parameter                     | Description                                                                                   |
|-------------------------------|-----------------------------------------------------------------------------------------------|
| **Setpoint**                  | Desired target value (e.g., target temperature).                                              |
| **P / I / D**                 | PID gains: Proportional, Integral, Derivative values for tuning response.                     |
| **Direction**                 | `Direct` or `Reverse` – determines whether increasing input increases or decreases output.    |
| **Mode**                      | `Manual` or `Automatic` – allows external override of PID logic.                              |
| **Proportional On**           | `P_ON_E` (Proportional on Error) or `P_ON_M` (Proportional on Measurement).                   |
| **Output Manual Value**       | Manual override value when in Manual mode.                                                    |
| **Input Invert**              | If true, inverts the input signal.                                                            |
| **Output Invert**             | If true, inverts the output signal.                                                           |
| **Input Scale Min / Max**     | Minimum and maximum values used to scale the raw input reading.                               |
| **Output Scale Min / Max**    | Range to scale PID output (e.g., 0–1 for PWM, 0–100 for percent).                             |
| **Output Threshold**          | Value above/below which a binary output is triggered (e.g., relay turns on/off).              |
| **Input Low Alarm Setpoint**  | Triggers a callback when input falls below threshold.                                         |
| **Input High Alarm Setpoint** | Triggers a callback when input rises above threshold.                                         |
| **Sampling Mode**             | `Event-driven` or `Time-based` sampling for PID calculation updates.                          |


## Overview

This project provides a flexible Proportional-Integral-Derivative (PID) controller component for ESPHome, suitable for temperature, speed, and other feedback control applications. It is designed for easy integration, real-time tuning, and compatibility with a variety of sensors and actuators.

## Features

- Real-time PID computation with adjustable parameters
- Support for both temperature and other sensor inputs
- Configurable setpoint and threshold values
- Binary output state based on PID output threshold
- Integration with ESPHome's sensor and binary sensor components

## Hardware Requirements

- ESP32 development board
- DHT sensor (or other compatible temperature sensor)
- Optional: Display for real-time monitoring
- Optional: Relay or other actuator for control output

## Installation

1. Copy the `components/pid_controller` directory into your ESPHome project.
2. Reference the component in your YAML configuration.
3. Configure your sensors, setpoints, and PID parameters as needed.


## PID Modes: P_ON_E vs P_ON_M
You'll most often choose between "Proportional-on-Error" (P_ON_E) and "Proportional-on-Measurement" (P_ON_M) based on how sensitive your loop is to sudden setpoint changes or measurement noise.

### 1. Proportional-on-Error (P_ON_E)

**Behavior:**
- P-term acts on the difference: `error = Setpoint - Input`
- Gives a "classic" PID response: when you step the setpoint, you immediately get a full P kick

**When to use:**
- Simple, clean processes (e.g., low-noise temperature loops, liquid-level control) where you want the controller to react strongly to every setpoint change
- Systems with infrequent setpoint moves—you care more about chasing a new target cleanly than about smoothing noise
- Tuning by hand, since it behaves like the textbook PID: increasing Kₚ yields a more aggressive chase of the setpoint

### 2. Proportional-on-Measurement (P_ON_M)

**Behavior:**
- P-term is effectively applied against the change in measurement: `-Kₚ × Δ(Input)`
- Leads to less "bump" on setpoint steps, and combines with the integral term in a slightly different ("parallel") wiring

**When to use:**
- Noisy measurements (e.g., pressure, flow, some electrical signals) where you want to avoid big jumps in the controller output every time your sensor ticks
- Tight derivative action—since the P term is on the same signal as the D term (measurement), you avoid putting two opposing forces on the loop
- Processes that can tolerate a gentler setpoint approach in order to reduce overshoot or actuator wear (e.g., precision motion control)

## Quick Reference Table

| Feature / Concern | P_ON_E | P_ON_M |
|---|---|---|
| Reaction to setpoint steps | Immediate, strong P kick | Softer, no extra bump |
| Noise sensitivity | Higher (P acts on raw error) | Lower (P acts on measurement change) |
| Ease of tuning | Straightforward textbook behavior | May require a bit more care |
| When setpoints change often | Good | Might feel sluggish |
| When smoothness is critical | Can overshoot/oscillate | Generally smoother |

## Application Examples

**Temperature control in an oven:** Mostly P_ON_E, since the temperature sensor is relatively slow/clean and you want a fast approach to your target.

**Speed control on a noisy tachometer:** Try P_ON_M to reduce the jitter you'd otherwise inject into your motor drive every time the sensor "ticks."

**Precise position control (e.g., CNC axes):** P_ON_M often leads to smoother starts and stops, especially with high-gain D terms.

## General Guidelines

In most Arduino PID applications the default P_ON_E is fine, but if you notice excessive "kick" or oscillation whenever you change your setpoint—or if your sensor is noisy—you can switch to P_ON_M and retune your Kₚ, Kᵢ, Kd accordingly.

## Tuning Tips

1. **Start with conservative values:** Begin with small Kₚ, zero Kᵢ, and zero Kd
2. **Increase Kₚ first:** Gradually increase until you get a reasonable response
3. **Add integral action:** Increase Kᵢ to eliminate steady-state error
4. **Fine-tune with derivative:** Add small amounts of Kd to reduce overshoot

## Project Structure

```
esp-pid/
├── base_esp_sensor.yaml         # Base sensor config
├── base_esp.yaml                # Base ESP config
├── base_group.yaml              # Base group config
├── base_heartbeat.yaml          # Heartbeat, time sync, MQTT status
├── config_input.yaml            # Input config
├── config_output.yaml           # Output config
├── config_pid.yaml              # PID config
├── esp-pid-1.yaml               # Example device config
├── secrets.yaml                 # Secrets (WiFi, MQTT, etc.)
├── components/
│   ├── ds3502/                  # DS3502 digital potentiometer support
│   │   ├── ds3502.cpp, .h, .py  # DS3502 implementation
│   │   └── output.cpp, .h       # Output helpers
│   └── pid_controller/          # PID controller component
│       ├── __init__.py
│       ├── pid_controller.cpp, .h
│       └── PID_v1.cpp, .h       # PID logic
└── documents/                   # Reference images, datasheets (not tracked in git)
```
