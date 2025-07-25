#pragma once
#include "esphome.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/number/number.h"
#include "esphome/components/select/select.h"
#include "PID_v1.h"

namespace esphome {
namespace pid_controller {

using sensor::Sensor;
using binary_sensor::BinarySensor;
using number::Number;
using select::Select;

class PIDController : public Component {
 public:
  PIDController() : pid(&current_input_, &pid_output_, &setpoint_, 1.0, 0.0, 0.0, DIRECT),
                    sample_time_(100), update_scheduled_(false) {
    pid.SetMode(AUTOMATIC);
    pid.SetOutputLimits(0, 255);
    pid.SetSampleTime(100);
  }

  void set_input_sensor(Sensor *sensor) { input_sensor_ = sensor; }
  void set_setpoint_number(Number *number) { setpoint_number_ = number; }
  void set_kp_number(Number *number) { kp_number_ = number; }
  void set_ki_number(Number *number) { ki_number_ = number; }
  void set_kd_number(Number *number) { kd_number_ = number; }
  void set_output_sensor(Sensor *sensor) { output_sensor_ = sensor; }
  void set_state_sensor(BinarySensor *sensor) { state_sensor_ = sensor; }
  void set_manual_output_number(Number *number) { manual_output_number_ = number; }
  void set_sample_time_number(Number *number) { sample_time_number_ = number; }
  void set_output_min_number(Number *number) { output_min_number_ = number; }
  void set_output_max_number(Number *number) { output_max_number_ = number; }
  void set_proportional_on_select(Select *select) { proportional_on_select_ = select; }
  void set_controller_direction_select(Select *select) { controller_direction_select_ = select; }
  void set_controller_mode_select(Select *select) { controller_mode_select_ = select; }
  void set_sampling_mode_select(Select *select) { sampling_mode_select_ = select; }
  void set_threshold(float threshold) { threshold_ = threshold; }
  void set_threshold_number(Number *number) { threshold_number_ = number; }

  void setup() override {
    // initialize values
    if (input_sensor_ && setpoint_number_) {
      current_input_  = input_sensor_->state;
      setpoint_       = setpoint_number_->state;
      pid_output_     = 0.0;
    }
    
    // Set initial output limits if sensors are available
    if (output_min_number_ && output_max_number_) {
      pid.SetOutputLimits(output_min_number_->state, output_max_number_->state);
    }
    
    // Set initial sample time if sensor is available
    if (sample_time_number_) {
      sample_time_ = (uint32_t)sample_time_number_->state;
      pid.SetSampleTime((int)sample_time_);
    }

    // Set up sensor callbacks for immediate parameter response
    if (input_sensor_) {
      input_sensor_->add_on_state_callback([this](float state) {
        current_input_ = state;
        // Check if we're in event-driven mode and trigger computation
        if (sampling_mode_select_ && sampling_mode_select_->state == "Event-driven") {
          schedule_event_driven_compute();
        }
      });
    }
    
    if (setpoint_number_) {
      setpoint_number_->add_on_state_callback([this](float state) {
        setpoint_ = state;
        // Setpoint changes don't need immediate parameter update
      });
    }
    
    // Set up callbacks for parameter changes that need immediate updates
    if (kp_number_) {
      kp_number_->add_on_state_callback([this](float state) {
        schedule_update();
      });
    }
    
    if (ki_number_) {
      ki_number_->add_on_state_callback([this](float state) {
        schedule_update();
      });
    }
    
    if (kd_number_) {
      kd_number_->add_on_state_callback([this](float state) {
        schedule_update();
      });
    }
    
    if (sample_time_number_) {
      sample_time_number_->add_on_state_callback([this](float state) {
        schedule_update();
      });
    }
    
    if (output_min_number_) {
      output_min_number_->add_on_state_callback([this](float state) {
        schedule_update();
      });
    }
    
    if (output_max_number_) {
      output_max_number_->add_on_state_callback([this](float state) {
        schedule_update();
      });
    }
    
    if (controller_mode_select_) {
      controller_mode_select_->add_on_state_callback([this](const std::string &state, size_t index) {
        schedule_update();
      });
    }
    
    if (controller_direction_select_) {
      controller_direction_select_->add_on_state_callback([this](const std::string &state, size_t index) {
        schedule_update();
      });
    }
    
    if (proportional_on_select_) {
      proportional_on_select_->add_on_state_callback([this](const std::string &state, size_t index) {
        schedule_update();
      });
    }
    
    if (sampling_mode_select_) {
      sampling_mode_select_->add_on_state_callback([this](const std::string &state, size_t index) {
        schedule_sampling_mode_change();
      });
    }
    
    // Start the PID computation timer (will be managed based on sampling mode)
    start_timer();
  }

 private:
  void schedule_update() {
    if (!update_scheduled_) {
      update_scheduled_ = true;
      // Schedule immediate parameter update (next loop cycle)
      defer([this]() { this->update_now(); });
    }
  }
  
  void update_now() {
    update_scheduled_ = false;
    
    // Check if all required sensors are available
    if (!input_sensor_ || !setpoint_number_ || !kp_number_ || !ki_number_ || 
        !kd_number_ || !output_sensor_ || !state_sensor_) {
      return;
    }
    
    // Update sample time and reschedule if changed
    if (sample_time_number_) {
      uint32_t new_sample_time = (uint32_t)sample_time_number_->state;
      if (new_sample_time >= 10 && new_sample_time != sample_time_) {  // Minimum 10ms
        sample_time_ = new_sample_time;
        pid.SetSampleTime((int)sample_time_);
        ESP_LOGD("pid_controller", "Updated sample time to %u ms", sample_time_);
        // Timer will automatically use new interval on next reschedule
      }
    }
    
    // Update output limits
    if (output_min_number_ && output_max_number_) {
      double min_val = output_min_number_->state;
      double max_val = output_max_number_->state;
      if (max_val > min_val) {  // Validate limits
        pid.SetOutputLimits(min_val, max_val);
      }
    }

    // Update controller direction
    if (controller_direction_select_) {
      std::string direction = controller_direction_select_->state;
      int new_direction = (direction == "REVERSE") ? REVERSE : DIRECT;
      pid.SetControllerDirection(new_direction);
    }

    // Update PID tunings including proportional mode
    double kp = kp_number_->state;
    double ki = ki_number_->state;
    double kd = kd_number_->state;
    
    // Determine proportional mode
    int proportional_mode = P_ON_E;  // Default to Proportional-on-Error
    if (proportional_on_select_) {
      std::string prop_mode = proportional_on_select_->state;
      if (prop_mode == "Proportional-on-Measurement") {
        proportional_mode = P_ON_M;
      }
    }
    
    // Set tunings with proportional mode
    pid.SetTunings(kp, ki, kd, proportional_mode);

    // Update controller mode
    if (controller_mode_select_) {
      std::string mode = controller_mode_select_->state;
      if (mode == "AUTOMATIC") {
        if (pid.GetMode() != AUTOMATIC) {
          pid.SetMode(AUTOMATIC);
        }
      } else if (mode == "MANUAL") {
        if (pid.GetMode() != MANUAL) {
          pid.SetMode(MANUAL);
        }
      }
    }
  }
  
  void compute_and_reschedule() {
    // Read current sensor values
    if (input_sensor_) {
      current_input_ = input_sensor_->state;
    }
    if (setpoint_number_) {
      setpoint_ = setpoint_number_->state;
    }
    
    // Perform PID computation
    perform_pid_computation();
    
    // Reschedule next computation only if in time-based mode
    if (sampling_mode_select_ && sampling_mode_select_->state == "Time-based") {
      uint32_t interval = sample_time_;
      set_timeout("pid_compute", interval, [this]() {
        this->compute_and_reschedule();
      });
    }
  }
  
  void perform_pid_computation() {
    // Handle PID computation based on mode
    if (controller_mode_select_) {
      std::string mode = controller_mode_select_->state;
      if (mode == "AUTOMATIC" && pid.GetMode() == AUTOMATIC) {
        // Compute PID output
        pid.Compute();
      } else if (mode == "MANUAL" && manual_output_number_) {
        // Use manual output value
        pid_output_ = manual_output_number_->state;
      }
    } else {
      // Default to automatic mode
      if (pid.GetMode() == AUTOMATIC) {
        pid.Compute();
      }
    }

    // Publish float output
    if (output_sensor_) {
      output_sensor_->publish_state(pid_output_);
    }

    // Publish boolean state based on threshold
    if (state_sensor_) {
      float current_threshold = threshold_;
      if (threshold_number_) {
        current_threshold = threshold_number_->state;
      }
      bool on = pid_output_ > current_threshold;
      state_sensor_->publish_state(on);
    }
  }

 private:
  void start_timer() {
    // Only start timer if in time-based mode
    if (!sampling_mode_select_ || sampling_mode_select_->state == "Time-based") {
      set_timeout("pid_compute", sample_time_, [this]() {
        this->compute_and_reschedule();
      });
    }
  }
  
  void schedule_sampling_mode_change() {
    if (!sampling_mode_change_scheduled_) {
      sampling_mode_change_scheduled_ = true;
      defer([this]() { this->handle_sampling_mode_change(); });
    }
  }
  
  void handle_sampling_mode_change() {
    sampling_mode_change_scheduled_ = false;
    
    if (!sampling_mode_select_) return;
    
    std::string mode = sampling_mode_select_->state;
    ESP_LOGD("pid_controller", "Sampling mode changed to: %s", mode.c_str());
    
    if (mode == "Time-based") {
      // Cancel any pending timer and restart in time-based mode
      cancel_timeout("pid_compute");
      set_timeout("pid_compute", sample_time_, [this]() {
        this->compute_and_reschedule();
      });
    } else if (mode == "Event-driven") {
      // Cancel the timer - computation will be event-driven
      cancel_timeout("pid_compute");
    }
  }
  
  void schedule_event_driven_compute() {
    if (!event_compute_scheduled_) {
      event_compute_scheduled_ = true;
      defer([this]() { this->compute_now(); });
    }
  }
  
  void compute_now() {
    event_compute_scheduled_ = false;
    
    // Read current sensor values
    if (input_sensor_) {
      current_input_ = input_sensor_->state;
    }
    if (setpoint_number_) {
      setpoint_ = setpoint_number_->state;
    }
    
    // Perform PID computation
    perform_pid_computation();
  }
  double setpoint_ = 0.0, current_input_ = 0.0, pid_output_ = 0.0;
  PID pid;

  // sensors for inputs & tunings
  Sensor *input_sensor_ = nullptr;
  Number *setpoint_number_ = nullptr;
  Number *kp_number_ = nullptr;
  Number *ki_number_ = nullptr;
  Number *kd_number_ = nullptr;
  
  // optional configuration sensors
  Number *manual_output_number_ = nullptr;
  Number *sample_time_number_ = nullptr;
  Number *output_min_number_ = nullptr;
  Number *output_max_number_ = nullptr;
  Number *threshold_number_ = nullptr;
  
  // select controls
  Select *proportional_on_select_ = nullptr;
  Select *controller_direction_select_ = nullptr;
  Select *controller_mode_select_ = nullptr;
  Select *sampling_mode_select_ = nullptr;

  // our outputs
  Sensor *output_sensor_ = nullptr;
  BinarySensor *state_sensor_ = nullptr;

  // threshold for ON/OFF
  float threshold_ = 0.0;
  
  // Hybrid approach variables
  bool update_scheduled_ = false;
  bool sampling_mode_change_scheduled_ = false;
  bool event_compute_scheduled_ = false;
  uint32_t sample_time_ = 100;
};

}  // namespace pid_controller
}  // namespace esphome