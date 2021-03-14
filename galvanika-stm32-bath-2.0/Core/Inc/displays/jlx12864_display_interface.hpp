#ifndef INC_GMG12864_DISPLAY_INTERFACE_HPP_
#define INC_GMG12864_DISPLAY_INTERFACE_HPP_

#include <displays/display_interface.hpp>

template <typename Port, typename Pin>
class JLX12864DisplayInterface : public DisplayInterface {
private:
  Output<Port, Pin> CS;
  Output<Port, Pin> RST;
  Output<Port, Pin> RS;
  Output<Port, Pin> SDA;
  Output<Port, Pin> SCK;

public:
  JLX12864DisplayInterface<Port, Pin>(
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
    RST.set();
    write_command(0xE2);          // Reset
    write_command(0x28 | 0x04);   // turn on voltage converter (VC=1, VR=0, VF=0)
    write_command(0x28 | 0x06);   // turn on voltage regulator (VC=1, VR=1, VF=0)
    write_command(0x28 | 0x07);   // turn on voltage follower  (VC=1, VR=1, VF=1)
    write_command(0x20 | 0x04);   // set lcd operating voltage (regulator resistor, ref voltage resistor)
    write_command(0x81);          // Contrast level
    write_command(0x20);          // 0...63, 32 - default

    write_command(0xA0);          // ADC select (0xA0/vA1)
    write_command(0xA2);          // Bias (0xA2=9/0xA3=7)
    write_command(0xA6);          // Display (0xA6=NORMAL/0xA7=REVERSE)
    write_command(0xC8);          // COM output scan direction (0xC0=NORMAL/0xC8=REVERSE)
    write_command(0x40 | 0x00);   // Initial display line
    write_command(0xAF);          // Display (0xAF=ON/0xAE=OFF)
    write_command(0xA4);          // Display all points (0xA4=NO/0xA5=YES)
    CS.set();
  }

  void set_cursor(uint8_t x, uint8_t y) {
    CS.clear();
    write_command(0xb0 + y);
    write_command(((x >> 4) & 0x0f) + 0x10);
    write_command(x & 0x0f);
  }

  void write_command(uint8_t command) {
    CS.clear();
    RS.clear();
    for (uint8_t i = 0; i < 8; i++) {
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
    CS.clear();
    RS.set();
    for (uint8_t i = 0; i < 8; i++) {
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

//  void write_data(uint8_t data) {
//    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
////    GPIOB->ODR &= ~(1 << CS.pin);
//    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
////    GPIOB->ODR |= (1 << RS.pin);
//    for (uint8_t i = 0; i < 8; i++) {
//      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
////      GPIOB->ODR &= ~(1 << SCK.pin);
//      if (data & 0x80) {
//        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
////        GPIOB->ODR |= (1 << SDA.pin);
//      } else {
//        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
////        GPIOB->ODR &= ~(1 << SDA.pin);
//      }
//      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
////      GPIOB->ODR |= (1 << SCK.pin);
//      data <<= 1;
//    }
};

#endif
