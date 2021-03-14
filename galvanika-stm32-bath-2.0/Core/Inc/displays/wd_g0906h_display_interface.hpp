#ifndef INC_DISPLAYS_WD_G0906H_DISPLAY_INTERFACE_HPP_
#define INC_DISPLAYS_WD_G0906H_DISPLAY_INTERFACE_HPP_

#include <displays/display_interface.hpp>

template <typename Port, typename Pin>
class WD_G0906DisplayInterface : public DisplayInterface {
private:
  Output<Port, Pin> CS;
  Output<Port, Pin> RST;
  Output<Port, Pin> SDA;
  Output<Port, Pin> SCK;

public:
  WD_G0906DisplayInterface<Port, Pin>(
      Output<Port, Pin> CS,
      Output<Port, Pin> RST,
      Output<Port, Pin> SDA,
      Output<Port, Pin> SCK
      ) :
      DisplayInterface(96, 68),
      CS(CS), RST(RST), SDA(SDA), SCK(SCK) {
  }

  void init() {
    RST.clear();
    delay(24000);
    write_command(0xE2); // Internal reset
    write_command(0xEB); // Thermal comp. on
    write_command(0x2F); // Supply mode
    write_command(0xA1); // Horisontal reverse: Reverse - 0xA9, Normal - 0xA1
    //write_command(0xC8); // Vertical reverse (comment if no need)
    write_command(0xA4); // Clear screen
    write_command(0xA6); // Positive - A7, Negative - A6
    write_command(0x90); // Contrast 0x90...0x9F
    write_command(0xAF); // Enable LCD
    RST.set();
  }

  void set_cursor(uint8_t x, uint8_t y) {
    write_command(0xB0 | (y & 0x0F));
    write_command(0x10 | (x >> 0x04));
    write_command(0x0F & x);
  }

  void write_command(uint8_t command) {
    register uint32_t i;
    CS.clear();
    cbi(PORT, SDA);
    sbi(PORT, SCK);
    for (register uint8_t i = 0; i < 8; i++) {
      SCK.clear();
      if (command & 0x80) {
        SDA.set();
      } else {
        SDA.clear();
      }
      SCK.set();
      command <<= 1;
    }
    SCK.clear();
    CS.set();
  }

  void write_data(uint8_t data) {
    CS.clear();
    SDA.set();
    SCK.set();
    for (register uint8_t i = 0; i < 8; i++) {
      SCK.clear();
      if (data & 0x80) {
        SDA.set();
      } else {
        SDA.clear();
      }
      SCK.set();
      data <<= 1;
    }
  }

};
#endif
