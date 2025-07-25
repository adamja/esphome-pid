#pragma once

#include "esphome/core/component.h"
#include "esphome/components/output/float_output.h"
#include "ds3502.h"

namespace esphome {
namespace ds3502 {

class DS3502Output : public output::FloatOutput, public Component {
 public:
  void set_parent(DS3502Component *parent) { parent_ = parent; }
  void set_inverted(bool inverted) { inverted_ = inverted; }
  void dump_config() override;

 protected:
  void write_state(float state) override;
  DS3502Component *parent_;
  bool inverted_{false};
};

}  // namespace ds3502
}  // namespace esphome
