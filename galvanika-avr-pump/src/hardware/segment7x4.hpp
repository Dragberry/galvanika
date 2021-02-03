#ifndef SEGMENT7X4_HPP_
#define SEGMENT7X4_HPP_

#include <stdint.h>
#include <stdlib.h>
#include "../avr/hardware/spi.hpp"
#include "../avr/software/operators.hpp"

class Segment8x7
{
private:
    static const uint8_t BUFFER_SIZE = 4;

    static const uint8_t DIGITS[16];

    static const uint8_t DOT_MASK = 0b11111011;

    uint8_t data[BUFFER_SIZE];

    uint8_t current_digit;

    uint8_t data1 = 0;
    uint8_t data2 = 0;

public:
    void init()
    {
        SPI::init();
        cbi(TCCR0, CS02);
        sbi(TCCR0, CS01);
        sbi(TCCR0, CS00);
        sbi(TIMSK, TOIE0);

    }

    void show()
    {
        set_digit(data[current_digit]);
        select_digit(current_digit);
        current_digit++;
        if (current_digit == 4)
        {
            current_digit = 0;
        }
        SPI::reset();
        SPI::send_byte(data1);
        SPI::send_byte(data2);
        SPI::latch();
    }

    void select_digit(uint8_t digit)
    {
        data2 = 0b11110000 ^ (1 << (4 + digit));
    }

    void set_digit(uint8_t digit)
    {
        data1 = digit;
    }

    void set_mode(uint8_t mode)
    {
        data[3] = DIGITS[mode % 16] & DOT_MASK;
    }

    void set_value(uint8_t value)
    {
        data[0] = DIGITS[value % 10];
        value /= 10;
        data[1] = DIGITS[value % 10];
        value /= 10;
        data[2] = DIGITS[value % 10];
    }

};

#endif
