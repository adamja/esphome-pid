# ESP-PID Controller

A custom ESPHome component for implementing PID control on ESP32 devices.

## Overview

This project provides a PID (Proportional-Integral-Derivative) controller component for ESPHome that can be used for various control applications like temperature control, motor speed control, and other feedback systems.

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

1. Copy the `components/pid_controller` directory to your ESPHome project
2. Include the component in your YAML configuration
3. Configure sensors and parameters as needed

## Configuration Example

```yaml
sensor:
  - platform: dht
    pin: GPIO4
    temperature:
      name: "Temperature Input"
      id: temp_input

pid_controller:
  id: my_pid
  input_sensor: temp_input
  setpoint_sensor: setpoint
  kp_sensor: pid_kp_sensor
  ki_sensor: pid_ki_sensor
  kd_sensor: pid_kd_sensor
  output_sensor: pid_output
  state_sensor: pid_state
  threshold: 0.5
  update_interval: 2s
```

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

## Files Structure

```
components/
├── pid_controller/
│   ├── __init__.py          # ESPHome component configuration
│   ├── pid_controller.h     # C++ header file
│   └── pid_controller.cpp   # C++ implementation
config.yaml                  # Main ESPHome configuration
esp-pid-1.yaml              # Device-specific configuration
```

## Dependencies

- ESPHome 2025.6.2 or later
- Arduino PID Library (automatically downloaded from GitHub)
- ESP32 Arduino framework

## License

This project is open source. Please check individual component licenses for specific terms.