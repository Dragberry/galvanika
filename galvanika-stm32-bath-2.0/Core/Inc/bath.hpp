#ifndef INC_BATH_HPP_
#define INC_BATH_HPP_

#include <stdint.h>
#include <stdio.h>
#include <flash.hpp>
#include <ring_buffer.hpp>
#include <displays/display.hpp>

static const uint8_t DURATION_MODES_SIZE = 4;
static constexpr uint16_t DURATION_MODES[DURATION_MODES_SIZE][2] = {
    {1, 20}, {10, 200}, {100, 2000}, {1000, 20000}
};

enum Mode {
  CURRENT, PROGRRESS, RESET_REQUEST
};

struct ADC {
  uint32_t duration;
  uint32_t direct;
  uint32_t reverse;
};

struct Sensor {
  volatile uint32_t *port;
  uint16_t pin;

  inline
  bool check() {
    return *port & pin;
  }
};

struct Control {
  volatile uint32_t *port;
  uint16_t pin;

  inline
  void set() {
    *port |= pin;
  }

  inline
  void clear() {
    *port &= ~pin;
  }
};

struct Data {
  uint32_t time_ms = 0;
  uint32_t measures = 0;
  double average_current = 0;
  double weight = 0;
  const uint64_t end = 0;
};

class Bath {
public:
  static const float SPEED;
private:
  const char name;

  RingBuffer<uint32_t, 32> current_direct_raw_values;
  float current_direct;
  RingBuffer<uint32_t, 32> current_reverse_raw_values;
  float current_reverse;

  uint8_t duration_mode;

  uint16_t scale;
  uint8_t direct_duration;
  uint8_t direct_progress;
  uint32_t reverse_duration_raw;
  uint8_t reverse_duration;
  uint8_t reverse_progress;


  char current_string[21];
  char mode_string[8];
  char time_string[21];
  char stats_string[21];
  char disabled_string[12];

  uint16_t control_pressed_time;

  Mode mode;

public:
  uint8_t y_position;
  ADC adc;
  Control pwm;
  Sensor branch;
  Sensor control;
  Sensor power;

private:
  float (* adjust_current)(float);
  float (* adjust_reverse)(float);
  STM32::Flash<Data> storage;
  Data data;
  uint8_t answer_timer;

public:
  Bath(
      const char name,
      uint8_t y_position,
      ADC adc,
      Control pwm,
      Sensor branch,
      Sensor control,
      Sensor power,
      uint32_t storage_address,
      float (* adjust_current)(float),
      float (* adjust_reverse)(float)
      ) :
        name(name),
        y_position(y_position),
        adc(adc),
        pwm(pwm),
        branch(branch),
        control(control),
        power(power),
        adjust_current(adjust_current),
        adjust_reverse(adjust_reverse),
        storage(STM32::Flash<Data>(storage_address, 1)) {
    this->current_direct = 0.0;
    this->current_reverse = 0.0;
    this->scale = 1;
    this->direct_duration = 20;
    this->direct_progress = 0;
    this->reverse_duration = 0;
    this->reverse_progress = 0;
    this->duration_mode = 0;
    this->control_pressed_time = 0;
    this->mode = Mode::CURRENT;
    this->answer_timer = 0;
    snprintf(disabled_string, 21, "%c: Disabled", name);
  }

  void build() {
    reverse_duration = (uint8_t) (reverse_duration_raw * 11 / 4096);
    current_direct = adjust_current(((float) current_direct_raw_values.average()) * 0.000264485);
    current_reverse = -adjust_reverse(((float) current_reverse_raw_values.average()) * 0.000264485);
    if (power.check()) {
      switch (mode) {
      case Mode::CURRENT: {
        snprintf(current_string, 19, "%c: %+1.3f/%1.3f", name, current_direct, current_reverse);
        uint16_t current_mode_duration = DURATION_MODES[duration_mode][1];
        uint16_t current_mode_duration_normalized = current_mode_duration  < 1000 ? current_mode_duration : current_mode_duration / 1000;
        const char* duration_mode_unit = current_mode_duration < 1000 ? "ms" : "s";
        snprintf(mode_string, 7, "%3d %-2s", current_mode_duration_normalized, duration_mode_unit);
        break;
      }
      case Mode::PROGRRESS: {
        uint32_t time_seconds = sniprintf_uptime();
        data.weight = data.average_current * time_seconds * Bath::SPEED;
        sniprintf(stats_string, 20, "%1.3fA / %2.3fg", data.average_current, data.weight);
        break;
      }
      case Mode::RESET_REQUEST: {
        sniprintf_uptime();
        sniprintf(stats_string, 20, "Confirm Reset: %d", answer_timer);
        break;
      }
      default:
        break;
      }
    }
  }

  uint32_t sniprintf_uptime() {
    uint32_t time_seconds = data.time_ms / 1000;
    uint32_t time_minutes = time_seconds / 60;
    uint32_t time_hours = time_minutes / 60;

    uint32_t seconds = time_seconds % 60;
    uint32_t minutes = time_minutes % 60;
    uint32_t hours = time_hours % 60;
    sniprintf(time_string, 20, "%c: %03d%:%02d:%02d", name, hours, minutes, seconds);

    return time_seconds;
  }

  void change_scale() {
    duration_mode++;
    if (duration_mode == DURATION_MODES_SIZE) {
      duration_mode = 0;
    }
    scale = DURATION_MODES[duration_mode][0];
  }

  void step_up() {
    if (power.check()) {
      data.time_ms++;
      double instant = current_direct + (current_reverse * ((float) reverse_duration / (float) (direct_duration + reverse_duration)));
      double total_current = (data.average_current * data.measures) + instant;
      data.measures++;
      data.average_current = total_current / data.measures;
      if (data.time_ms % 60000 == 0) {
        save();
      }
      if (mode == Mode::RESET_REQUEST) {
        if (data.time_ms % 1000 == 0) {
          answer_timer--;
          if (answer_timer == 0) {
           mode = Mode::PROGRRESS;
          }
        }
      }
    }
    if (!control.check()) {
      control_pressed_time++;
    }
    if (data.time_ms % scale == 0) {
      if (direct_progress == direct_duration) {
        if (reverse_duration > 0) {
          if (reverse_progress == 0) {
            pwm.set();
          }
          reverse_progress++;
          if (reverse_progress > reverse_duration) {
            reverse_progress = 0;
            direct_progress = 0;
            pwm.clear();
          }
        }
      } else {
        direct_progress++;
      }
    }
  }

  void set_reverse_duration(uint32_t value) {
    this->reverse_duration_raw = value;
  }

  void add_current_direct(uint32_t value) {
    this->current_direct_raw_values.add(value);
  }

  void add_current_reverse(uint32_t value) {
    this->current_reverse_raw_values.add(value);
  }

  void pressed() {
    this->control_pressed_time = 0;
  }

  void unpressed() {
    if (power.check()) {
      if (control_pressed_time > 1000) {
        on_long_click();
      } else {
        on_short_click();
      }
    }
  }

  void on_short_click() {
    switch (mode) {
    case Mode::CURRENT:
      change_scale();
      break;
    case Mode::PROGRRESS:
      mode = Mode::RESET_REQUEST;
      answer_timer = 5;
      break;
    case Mode::RESET_REQUEST:
      reset();
      save();
      mode = Mode::PROGRRESS;
      break;
    default:
      break;
    }
  }

  void on_long_click() {
    switch (mode) {
    case Mode::CURRENT:
      mode = Mode::PROGRRESS;
      break;
    case Mode::PROGRRESS:
    case Mode::RESET_REQUEST:
      save();
      mode = Mode::CURRENT;
      break;
    default:
      break;
    }
  }

  void show(Display& display) {
    display.fill_line(y_position, false);
    display.fill_line(y_position + 1, false);
    if (power.check()) {
      switch (mode) {
      case Mode::CURRENT: {
        const uint8_t bar_length = display.get_width() - 40;
        const uint8_t bar_item = bar_length / 10;
        display.set_cursor(0, y_position);
        display.put_string(current_string);
        display.set_cursor(0, y_position + 1);
        display.put_bar(10, reverse_duration, bar_item);
        display.set_cursor(bar_length, y_position + 1);
        display.put_string(mode_string);
        break;
      }
      case Mode::PROGRRESS:
      case Mode::RESET_REQUEST:
        display.set_cursor(0, y_position);
        display.put_string(time_string);
        display.set_cursor(0, y_position + 1);
        display.put_string(stats_string);
        break;
      default:
        break;
      }
    } else {
      display.set_cursor(0, y_position);
      display.put_string(disabled_string);
    }
  }

  void reset() {
    data.average_current = 0;
    data.measures = 0;
    data.time_ms = 0;
    data.weight = 0;
  }

  void load(bool erase = false) {
    if (erase) {
      storage.erase();
      storage.write(&data);
    } else {
      storage.read(&data);
    }
  }

  void save() {
    storage.write(&data);
  }
};

#endif
