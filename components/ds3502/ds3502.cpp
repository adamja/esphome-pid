#include "ds3502.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ds3502 {

static const char *const TAG = "ds3502";

void DS3502Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up DS3502...");
  
  // Test basic I2C communication by trying to read current wiper position
  uint8_t test_value = 0;
  if (!this->read_byte(DS3502_WIPER, &test_value)) {
    ESP_LOGE(TAG, "Failed to communicate with DS3502 at address 0x%02X", this->address_);
    this->communication_failed_ = true;
    this->mark_failed();
    return;
  }
  
  this->communication_failed_ = false;
  ESP_LOGCONFIG(TAG, "DS3502 setup complete");
}

void DS3502Component::dump_config() {
  ESP_LOGCONFIG(TAG, "DS3502:");
  LOG_I2C_DEVICE(this);
  
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with DS3502 failed!");
  }
}

bool DS3502Component::set_wiper_value(uint8_t value) {
  // Clamp value to 7-bit range (0-127)
  value = value & 0x7F;
  
  if (!this->write_byte(DS3502_WIPER, value)) {
    ESP_LOGE(TAG, "Failed to set wiper value");
    this->communication_failed_ = true;
    return false;
  }
  
  // Clear failed state if communication succeeds
  if (this->communication_failed_) {
    ESP_LOGI(TAG, "DS3502 communication restored");
    this->communication_failed_ = false;
  }
  
  ESP_LOGD(TAG, "Set wiper value to: %d", value);
  return true;
}

uint8_t DS3502Component::get_wiper_value() {
  uint8_t value = 0;
  
  if (!this->read_byte(DS3502_WIPER, &value)) {
    ESP_LOGE(TAG, "Failed to read wiper value");
    this->communication_failed_ = true;
    return 0;
  }
  
  // Clear failed state if communication succeeds
  if (this->communication_failed_) {
    ESP_LOGI(TAG, "DS3502 communication restored");
    this->communication_failed_ = false;
  }
  
  ESP_LOGD(TAG, "Read wiper value: %d", value & 0x7F);
  return value & 0x7F;  // Mask to 7-bit value
}

bool DS3502Component::set_default_wiper_value(uint8_t value) {
  // Clamp value to 7-bit range (0-127)
  value = value & 0x7F;
  
  if (!this->write_byte(DS3502_NVMEM, value)) {
    ESP_LOGE(TAG, "Failed to set default wiper value");
    return false;
  }
  
  ESP_LOGD(TAG, "Set default wiper value to: %d", value);
  return true;
}

}  // namespace ds3502
}  // namespace esphome
