#ifndef INC_DISPLAYS_SSD1306_DISPLAY_INTERFACE_HPP_
#define INC_DISPLAYS_SSD1306_DISPLAY_INTERFACE_HPP_

#include <displays/display_interface.hpp>

class I2C {
public:
  virtual void send(uint8_t address, uint8_t *data, uint8_t length) = 0;
};

class SSD1306DisplayInterface : public DisplayInterface {
private:
  I2C *const i2c;
  const uint8_t address;
  uint8_t command_block[2] = {0x00, 0x00};
  uint8_t data_block[2] = {0x40, 0x00};

public:
  SSD1306DisplayInterface(I2C *const i2c, const uint8_t address = 0x3C, const uint8_t height = 64) :
      DisplayInterface(128, height),
      i2c(i2c), address(address) {
  }

public:
  void init() {
    write_command(0xAE); //display off
    write_command(0x20); //Set Memory Addressing Mode
    write_command(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    write_command(0xB0); //Set Page Start Address for Page Addressing Mode,0-7
    write_command(0xC8); //Set COM Output Scan Direction
    write_command(0x00); //---set low column address
    write_command(0x10); //---set high column address
    write_command(0x40); //--set start line address
    write_command(0x81); //--set contrast control register
    write_command(0xFF);
    write_command(0xA1); //--set segment re-map 0 to 127
    write_command(0xA6); //--set normal display
    write_command(0xA8); //--set multiplex ratio(1 to 64)
    write_command(0x3F); //
    write_command(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    write_command(0xD3); //-set display offset
    write_command(0x00); //-not offset
    write_command(0xD5); //--set display clock divide ratio/oscillator frequency
    write_command(0xF0); //--set divide ratio
    write_command(0xD9); //--set pre-charge period
    write_command(0x22); //
    write_command(0xDA); //--set com pins hardware configuration
    write_command(0x12);
    write_command(0xDB); //--set vcomh
    write_command(0x20); //0x20,0.77xVcc
    write_command(0x8D); //--set DC-DC enable
    write_command(0x14); //
    write_command(0xAF); //--turn on SSD1306 panel
  }

  void write_data(uint8_t data) {
    data_block[1] = data;
    i2c->send(address, data_block, 2);
  }


  void write_command(uint8_t command) {
    command_block[1] = command;
    i2c->send(address, command_block, 2);
  }

  void set_cursor(uint8_t x, uint8_t y) {
    const uint8_t real_x = x + 2;
    write_command(0xB0 | y);
    write_command(0x10 | ((real_x >> 4) & 0x0F));
    write_command(0x00 | (real_x & 0x0F));
  }
};

#endif
