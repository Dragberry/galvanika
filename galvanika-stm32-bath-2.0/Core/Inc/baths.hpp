#ifndef INC_BATHS_HPP_
#define INC_BATHS_HPP_

#include <bath.hpp>

template <uint8_t Size>
class Baths {
public:
  Bath *baths;
  uint32_t adc_channel;

public:
  Baths<Size>(Bath baths[3]) : baths(baths) { }

  uint32_t get_adc_channel() const {
    return adc_channel;
  }

  template <typename Handler>
  void iterate(Handler&& handle_bath) {
    for (register uint32_t i = 0; i < Size; i++) {
      handle_bath(baths[i]);
    }
  }

  void step_up() {
    iterate([](Bath& bath) -> void {
      bath.step_up();
    });
  }

  void change_button_state(uint16_t pin) {
    iterate([pin](Bath& bath) -> void {
      if (bath.control.pin == pin) {
        if (bath.control.check()) {
          bath.unpressed();
        } else {
          bath.pressed();
        }
      }
    });
  }

  void apply_adc(uint32_t value) {
    register uint32_t i = 0;
    while (i < Size) {
      Bath* bath = &baths[i];
      if (adc_channel == bath->adc.direct) {
        bath->add_current_direct(value);
        adc_channel = get_next_channel(i);
        break;
      } else if (adc_channel == bath->adc.reverse) {
        bath->add_current_reverse(value);
        adc_channel = get_next_channel(i);
        break;
      } else if (adc_channel == bath->adc.duration) {
        bath->set_reverse_duration(value);
        if (bath->power.check()) {
          if (bath->branch.check()) {
            adc_channel = bath->adc.reverse;
          } else {
            adc_channel = bath->adc.direct;
          }
        } else {
          adc_channel = get_next_channel(i);
        }
        break;
      }
      i++;
    }
  }

private:
  uint32_t get_next_channel(uint8_t i) {
     i++;
     if (i == Size) {
       i = 0;
     }
     return baths[i].adc.duration;
   }
};

#endif
