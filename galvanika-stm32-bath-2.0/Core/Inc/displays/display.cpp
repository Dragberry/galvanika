#include <displays/display.hpp>
#include <displays/font5x8.hpp>

Display::Display(DisplayInterface *const interface) :
    interface(interface) {
}

void Display::set_cursor(uint8_t x, uint8_t y) {
  interface->set_cursor(x, y);
}

void Display::clear() {
  for (uint8_t i = 0; i < interface->rows; i++) {
    interface->set_cursor(0, i);
    for (uint8_t j = 0; j < interface->width; j++) {
      interface->write_data(0x00);
    }
  }
}

void Display::fill() {
  for (uint8_t i = 0; i < interface->rows; i++) {
    interface->set_cursor(0, i);
    for (uint8_t j = 0; j < interface->width; j++) {
      interface->write_data(0xff);
    }
  }
}

void Display::fill_line(uint8_t y, bool fill) {
  interface->set_cursor(0, y);
  for (uint8_t j = 0; j < interface->width; j++) {
    interface->write_data(fill ? 0xff : 0x00);
  }
}

void Display::put_char(const char c) {
  uint32_t i;
  for (i = 0; i < 5; i++) {
    interface->write_data(FONT5x8[(c * 5) + i]);
  }
  interface->write_data(0x00);
}

void Display::put_string(const char *str) {
  char c;
  while ((c = (*str++))) {
    put_char(c);
  }
}

void Display::put_bar(const uint8_t size, const uint8_t filled, const uint8_t section_width) {
  uint8_t total_width = size * section_width;
  uint8_t total_filled_width = filled * section_width;
  interface->write_data(0x7F);
  for (uint8_t i = 0; i < total_width; i++) {
    if (i < total_filled_width) {
      interface->write_data((i + 1) % section_width ? 0x7F : 0x41);
    } else {
      interface->write_data(0x41);
    }
  }
  interface->write_data(0x7F);
}
