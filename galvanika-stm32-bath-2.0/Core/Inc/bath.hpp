#ifndef INC_BATH_HPP_
#define INC_BATH_HPP_

#include "stdint.h"
#include "stdio.h"

static const uint8_t MODES_SIZE = 4;
static constexpr uint16_t MODES[MODES_SIZE][2] = {
    {1, 20}, {10, 200}, {100, 2000}, {1000, 20000}
};

class Bath {
private:
  const char name;

public:
  const uint32_t adc_channel_reverse_duration;
  const uint32_t adc_channel_current_direct;
  const uint32_t adc_channel_current_reverse;
  volatile uint32_t *pwm_port;
  const uint32_t pwm_pin;
  const uint32_t mode_pin;
  const uint32_t power_pin;
  const uint32_t branch_pin;
  const uint8_t y_position;

  bool enabled;

private:
  uint32_t current_direct_raw;
  float current_direct;
  uint32_t current_reverse_raw;
  float current_reverse;

  uint8_t mode;

  uint16_t time_ms;
  uint16_t scale;
  uint8_t direct_duration;
  uint8_t direct_progress;
  uint32_t reverse_duration_raw;
  uint8_t reverse_duration;
  uint8_t reverse_progress;

  char current_string[20];
  char mode_string[8];

  float (* adjust_current)(float);
  float (* adjust_reverse)(float);

public:
  Bath(
      char name,
      uint8_t y_position,
      uint32_t adc_duration,
      uint32_t adc_direct,
      uint32_t adc_reverse,
      volatile uint32_t *pwm_port,
      uint32_t pwm_pin,
      uint32_t mode_pin,
      uint32_t power_pin,
      uint32_t branch_pin,
      float (* adjust_current)(float),
      float (* adjust_reverse)(float)
      ) :
  name(name),
  adc_channel_reverse_duration(adc_duration),
  adc_channel_current_direct(adc_direct),
  adc_channel_current_reverse(adc_reverse),
  pwm_port(pwm_port),
  pwm_pin(pwm_pin),
  mode_pin(mode_pin),
  power_pin(power_pin),
  branch_pin(branch_pin),
  y_position(y_position),
  adjust_current(adjust_current),
  adjust_reverse(adjust_reverse) {
    this->current_direct_raw = 0;
    this->current_direct = 0.0;
    this->current_reverse_raw = 0;
    this->current_reverse = 0.0;
    this->time_ms = 0;
    this->scale = 1;
    this->direct_duration = 20;
    this->direct_progress = 0;
    this->reverse_duration = 0;
    this->reverse_progress = 0;
    this->mode = 0;
  }

  template <typename PinChecker>
  bool check_power(PinChecker&& check_pin) {
    enabled = check_pin(power_pin);
    return enabled;
  }

  template <typename PinChecker>
  bool check_branch(PinChecker&& check_pin) {
    return check_pin(branch_pin);
  }

  void build() {
    reverse_duration = (uint8_t) (reverse_duration_raw * 11 / 4096);
    current_direct = adjust_current(((float) current_direct_raw) * 0.000264485);
    current_reverse = -adjust_reverse(((float) current_reverse_raw) * 0.000264485);
    if (enabled) {
      snprintf(current_string, 19, "%c: %+1.3f/%1.3f", name, current_direct, current_reverse);
      uint16_t mode_duration = MODES[mode][1];
      uint16_t mode_duration_normalized = mode_duration  < 1000 ? mode_duration : mode_duration / 1000;
      const char* mode_duration_unit = mode_duration < 1000 ? "ms" : "s";
      snprintf(mode_string, 7, "%3d %-2s", mode_duration_normalized, mode_duration_unit);
    } else {
      snprintf(current_string, 19, "%c:Disabled      ", name);
      snprintf(mode_string, 7, "      ");
    }
  }

  void change_scale() {
    mode++;
    if (mode == MODES_SIZE) {
      mode = 0;
    }
    scale = MODES[mode][0];
  }

  template <typename EnableReverse, typename DisableReverse>
  void step_up(EnableReverse&& enable_reverse, DisableReverse&& disable_reverse) {
    time_ms++;
    if (time_ms % scale == 0) {
      if (direct_progress == direct_duration) {
        if (reverse_duration > 0) {
          if (reverse_progress == 0) {
            enable_reverse(pwm_port, pwm_pin);
          }
          reverse_progress++;
          if (reverse_progress > reverse_duration) {
            reverse_progress = 0;
            direct_progress = 0;
            disable_reverse(pwm_port, pwm_pin);
          }
        }
      } else {
        direct_progress++;
      }
    }
    if (time_ms == 50000) {
      time_ms = 0;
    }
  }

  uint8_t get_reverse_duration() const {
    return reverse_duration;
  }

  void set_reverse_duration(uint32_t value) {
    this->reverse_duration_raw = value;
  }

  void set_current_direct(uint32_t value) {
    current_direct_raw = value;
  }

  void set_current_reverse(uint32_t value) {
    current_reverse_raw = value;
  }

  char* get_currents_string() {
    return current_string;
  }

  char* get_mode_string() {
     return mode_string;
   }
};

#endif
