#ifndef INC_DISPLAY_HPP_
#define INC_DISPLAY_HPP_

#include <stdint.h>
#include <displays/display_interface.hpp>

class Display {
protected:
  DisplayInterface *const interface;

public:
  Display(DisplayInterface *const interface);

  void init(void (*init_io)(void)) {
    init_io();
    interface->init();
    clear();
  }

  void set_cursor(uint8_t x, uint8_t y);

  inline
  uint8_t get_width() const {
    return interface->width;
  }

  inline
  uint8_t get_height() const {
    return interface->height;
  }

  void clear();

  void fill();

  void fill_line(uint8_t y, bool fill);

  void put_char(const char c);

  void put_string(const char *str);

  void put_bar(const uint8_t size, const uint8_t filled, const uint8_t section_width);

};

#endif
