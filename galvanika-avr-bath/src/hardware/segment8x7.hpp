#ifndef SEGMENT8X7_HPP_
#define SEGMENT8X7_HPP_

#include <stdint.h>
#include <stdlib.h>
#include "../avr/hardware/spi.hpp"
#include "../avr/software/operators.hpp"

class Segment8x7
{
private:
    static const uint8_t BUFFER_SIZE = 8;

    static const uint8_t DIGITS[10];

    static uint8_t data[BUFFER_SIZE];

    static volatile uint8_t display_index;

public:
    static void init()
    {
//        SPI::init();
//        cbi(TCCR0, WGM00);
//        sbi(TCCR0, WGM01);
//
//        sbi(TCCR0, CS02);
//        cbi(TCCR0, CS01);
//        sbi(TCCR0, CS00);
//
//        OCR0 = 19;
//        sbi(TIMSK, OCIE0);
    }

    static void set_value(uint16_t value)
    {
        value = (value * 195) / 10;
        int8_t idx = 3;
        while (idx >= 0)
        {
            data[4 + idx] = value % 10;
            value /= 10;
//            uint16_t temp = value;
//            value = 0;
//            while (temp > 10)
//            {
//                temp -= 10;
//                value++;
//            }
            idx--;
        }
    }

    static void display_segment()
    {
        SPI::reset();
        SPI::send_byte(1 << display_index);
        SPI::send_byte(DIGITS[data[display_index]]);
        SPI::latch();
        display_index++;
        if (display_index == BUFFER_SIZE)
        {
            display_index = 0;
        }
    }

};


#endif
