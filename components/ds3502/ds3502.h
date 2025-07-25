#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace ds3502 {

class DS3502Component : public Component, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  bool set_wiper_value(uint8_t value);
  uint8_t get_wiper_value();
  bool set_default_wiper_value(uint8_t value);
  bool is_communication_failed() { return this->communication_failed_; }

 protected:
  static const uint8_t DS3502_WIPER = 0x00;
  static const uint8_t DS3502_CONTROL = 0x02;
  static const uint8_t DS3502_NVMEM = 0x01;
  
  bool communication_failed_ = false;
};

}  // namespace ds3502
}  // namespace esphome
