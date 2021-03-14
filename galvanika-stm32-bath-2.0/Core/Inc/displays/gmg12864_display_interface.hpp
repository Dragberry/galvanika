#ifndef INC_JLX12864_DISPLAY_INTERFACE_HPP_
#define INC_JLX12864_DISPLAY_INTERFACE_HPP_

#include <displays/display_interface.hpp>

template <typename Port, typename Pin>
class GMG12864DisplayInterface : public DisplayInterface {
private:
  Output<Port, Pin> CS;
  Output<Port, Pin> RST;
  Output<Port, Pin> RS;
  Output<Port, Pin> SDA;
  Output<Port, Pin> SCK;

public:
  GMG12864DisplayInterface<Port, Pin>(
      Output<Port, Pin> CS,
      Output<Port, Pin> RST,
      Output<Port, Pin> RS,
      Output<Port, Pin> SDA,
      Output<Port, Pin> SCK
      ) :
      DisplayInterface(128, 64),
      CS(CS), RST(RST), RS(RS), SDA(SDA), SCK(SCK) {
  }

public:
  void init() {
    CS.clear();
    RST.clear();
    delay(50000);
    RST.set();
    delay(50000);

    write_command(0xE2);          // Internal reset
    write_command(0x28 | 0x04);   // turn on voltage converter (VC=1, VR=0, VF=0)
    write_command(0x28 | 0x06);   // turn on voltage regulator (VC=1, VR=1, VF=0)
    write_command(0x28 | 0x07);   // turn on voltage follower  (VC=1, VR=1, VF=1)
    write_command(0x20 | 0x04);   // set lcd operating voltage (regulator resistor, ref voltage resistor)
    write_command(0x81);          // Contrast level
    write_command(0x20);          // 0...63, 32 - default
    write_command(0xA0);          // ADC select (0xA0/vA1)
    write_command(0xA3);          // Bias (0xA2=9/0xA3=7)
    write_command(0xA6);          // Display (0xA6=NORMAL/0xA7=REVERSE)
    write_command(0xC8);          // COM output scan direction (0xC0=NORMAL/0xC8=REVERSE)
    write_command(0x40  | 0x00);  // Initial display line
    write_command(0xAF);          // Display (0xAF=ON/0xAE=OFF)
    write_command(0xA4);          // Display all points (0xA4=NO/0xA5=YES)
    delay(50000);
  }

  void set_cursor(uint8_t x, uint8_t y) {
    write_command(0xb0 | y);
    write_command(0x10 | ((x >> 4) & 0x0F));
    write_command(0x00 | (x & 0x0F));
  }

  void write_command(uint8_t command) {
    RS.clear();
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
  }

  void write_data(uint8_t data) {
    RS.set();
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
