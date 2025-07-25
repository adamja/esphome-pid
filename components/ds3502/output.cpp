#include "output.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ds3502 {

static const char *const TAG = "ds3502.output";

void DS3502Output::write_state(float state) {
  // Apply inversion if enabled
  if (this->inverted_) {
    state = 1.0f - state;
    ESP_LOGD(TAG, "Inverting output: %.3f -> %.3f", 1.0f - state, state);
  }
  
  // Convert 0.0-1.0 range to 0-127 wiper range
  uint8_t wiper_value = static_cast<uint8_t>(state * 127.0f);
  
  ESP_LOGD(TAG, "Setting DS3502 wiper to: %d (%.1f%%)", wiper_value, state * 100.0f);
  
  if (!this->parent_->set_wiper_value(wiper_value)) {
    ESP_LOGE(TAG, "Failed to set DS3502 wiper value");
  }
}

void DS3502Output::dump_config() {
  ESP_LOGCONFIG(TAG, "DS3502 Output:");
  ESP_LOGCONFIG(TAG, "  Inverted: %s", YESNO(this->inverted_));
}

}  // namespace ds3502
}  // namespace esphome
