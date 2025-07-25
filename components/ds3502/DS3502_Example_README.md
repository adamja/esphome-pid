# DS3502 Digital Potentiometer ESPHome Component Example

This example demonstrates how to use the DS3502 I2C digital potentiometer as an output device in ESPHome.

## Quick Start

1. **Copy secrets file**: `cp ds3502_secrets.example.yaml secrets.yaml`
2. **Update secrets.yaml** with your WiFi credentials and settings
3. **Flash firmware**: `esphome run ds3502_example.yaml`
4. **Access device**: Navigate to `http://ds3502-example.local`

## Configuration

### Secrets File Setup

The example uses a `secrets.yaml` file for sensitive information. Copy the example file and update with your values:

```bash
cp ds3502_secrets.example.yaml secrets.yaml
```

Required secrets:
- `wifi_ssid` & `wifi_password`: Your WiFi network credentials
- `api_password` & `ota_password`: ESPHome access passwords  
- `mqtt_broker`, `mqtt_port`, `mqtt_username`, `mqtt_password`: MQTT broker settings

## Hardware Requirements

- ESP32 development board
- Adafruit DS3502 I2C Digital 10K Potentiometer Breakout (or compatible)
- Jumper wires

**References:**
- [Adafruit DS3502 Arduino Library](https://github.com/adafruit/Adafruit_DS3502) - Official Arduino library and documentation
- [Adafruit DS3502 Product Page](https://www.adafruit.com/product/4286) - Hardware specifications and purchase info

## Wiring

Connect the DS3502 to your ESP32:

```
ESP32 Pin    | DS3502 Pin | Description
-------------|------------|-------------
GPIO 21      | SDA        | I2C Data Line (default ESP32 SDA)
GPIO 22      | SCL        | I2C Clock Line (default ESP32 SCL)
3.3V         | VCC        | Power Supply
GND          | GND        | Ground
```

## Features

This example provides:

### 🎛️ **Output Control**
- **Number Slider**: Control output level from 0-100% (slider mode)
- **Quick Set Buttons**: Instantly set to 0%, 25%, 50%, 75%, or 100%
- **Inverted Output**: Output is inverted by default (0% input = 100% wiper, 100% input = 0% wiper)

### 📊 **Monitoring**
- **Wiper Position Sensor**: Read back actual potentiometer position (updates every 5s)
- **Status Text Sensor**: Shows "Connected - Ready" or "Failed - Check I2C connection"
- **Debug Logging**: Detailed I2C communication logs

### 🌐 **User Interfaces**
- **Web Server**: Built-in web interface on port 80
- **Home Assistant**: Full API integration via both MQTT and native API
- **MQTT Discovery**: Automatic device discovery in Home Assistant
- **OTA Updates**: Over-the-air firmware updates

## Usage

1. **Setup**: Copy `ds3502_secrets.example.yaml` to `secrets.yaml` and update with your credentials
2. **Flash**: Upload to your ESP32 using `esphome run ds3502_example.yaml`
3. **Access**: 
   - Web interface: `http://ds3502-example.local`
   - Home Assistant: Auto-discovered via MQTT and native API
4. **Control**: Use the number slider (0-100%) or quick-set buttons to adjust output level
5. **Monitor**: Watch the wiper position sensor (updates every 5s) and status text sensor for feedback

**Note**: The output is inverted by default, so 0% input results in 100% wiper position (maximum resistance) and 100% input results in 0% wiper position (minimum resistance).

## Configuration Details

### I2C Configuration
```yaml
i2c:
  sda: 21          # ESP32 default SDA pin
  scl: 22          # ESP32 default SCL pin  
  scan: true       # Enable I2C device scanning
```

### DS3502 Component
```yaml
ds3502:
  id: my_ds3502
  address: 0x28    # Default DS3502 I2C address
```

### Output Platform (with inversion)
```yaml
output:
  - platform: ds3502
    id: ds3502_output
    ds3502_id: my_ds3502
    inverted: true   # Inverts output: 0% input = 100% wiper, 100% input = 0% wiper
```

### Number Component (Slider Control)
```yaml
number:
  - platform: template
    name: "DS3502 Output Level"
    id: output_level
    min_value: 0
    max_value: 100
    step: 1
    mode: slider     # Displays as a slider in UI
    unit_of_measurement: "%"
```

## API Usage

You can control the DS3502 programmatically:

### Set Output Level
```cpp
// Set to 50%
id(ds3502_output)->set_level(0.5);
```

### Read Wiper Position
```cpp
// Get current wiper value (0-127)
uint8_t wiper = id(my_ds3502)->get_wiper_value();

// Convert to percentage
float percentage = (wiper / 127.0f) * 100.0f;
```

### Set Default Wiper Value (Non-volatile)
```cpp
// Set default power-on position to 50%
id(my_ds3502)->set_default_wiper_value(64);
```

## Troubleshooting

### Device Not Found
- Check wiring connections
- Verify I2C address (common addresses: 0x28, 0x2C, 0x2D, 0x2E)
- Enable I2C scanning: `scan: true`

### Communication Errors
- Check pull-up resistors on SDA/SCL if using long wires
- Verify 3.3V power supply
- Monitor logs for detailed error messages

### Expected Log Output
```
[I][ds3502:xxx]: Setting up DS3502...
[I][ds3502:xxx]: DS3502 setup complete
[I][i2c:xxx]: Found I2C device at address 0x28
[D][ds3502_sensor:xxx]: Wiper value: 63 (49.6%)
[D][ds3502.output:xxx]: Inverting output: 0.500 -> 0.500
[D][ds3502.output:xxx]: Setting DS3502 wiper to: 63 (50.0%)
[D][text_sensor:xxx]: 'DS3502 Status': Sending state 'Connected - Ready'
```

**Note on Precision**: The DS3502 has 128 discrete positions (0-127), so it cannot achieve exactly 50%. When you set 50%, it uses wiper position 63, which reads back as 49.6% due to the discrete nature of the hardware. This is normal and expected behavior.

## Integration Examples

### PID Controller Output
```yaml
# Use DS3502 as PID controller output
pid_controller:
  output_id: ds3502_output
  # ... other PID config

# Note: With inverted output, PID may need tuning adjustment
```

### Home Assistant Automation
```yaml
# Home Assistant automation example
automation:
  - alias: "Set DS3502 based on sensor"
    trigger:
      platform: numeric_state
      entity_id: sensor.temperature
      above: 25
    action:
      service: number.set_value
      target:
        entity_id: number.ds3502_output_level
      data:
        value: 75  # This will result in 25% wiper due to inversion
```

## Component Files

The DS3502 component consists of:
- `components/ds3502/__init__.py` - Component definition
- `components/ds3502/ds3502.h` - Header file
- `components/ds3502/ds3502.cpp` - Implementation
- `components/ds3502/output.h` - Output platform header
- `components/ds3502/output.cpp` - Output platform implementation  
- `components/ds3502/output.py` - Output platform configuration

## References

- [Adafruit DS3502 Arduino Library](https://github.com/adafruit/Adafruit_DS3502) - Official Arduino library with examples and documentation
- [DS3502 Datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/DS3502.pdf) - Complete technical specifications
- [ESPHome Custom Components](https://esphome.io/custom/custom_component.html) - Guide for creating custom ESPHome components
