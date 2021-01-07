#ifndef DISPLAY_HPP_
#define DISPLAY_HPP_

#include <stdint.h>
#include "../avr/hardware/spi.hpp"

enum Mode
{
    M_1 = 0, M_10 = 1, M_100 = 2, M_1000 = 3, M_DIRECT = 4, M_REVERSE = 5
};

class Display
{
private:
    static const uint8_t DIGITS[10];

    static const uint8_t HALF_SECOND = 128;

    static const uint8_t MINUS = 0b11101111;

    static const uint8_t EMPTY = 0b11111111;

    static const uint8_t DOT_MASK = 0b11111011;

    uint16_t time = 0;

    Mode mode = M_100;
    bool current_mode = false;

    uint8_t data1 = 0;
    uint8_t data2 = 0;
    uint8_t data3 = 0;
    uint8_t data4 = 0;

    uint8_t direct_values[4];
    uint8_t reverse_values[4];
    uint8_t *current_values = direct_values;
    uint8_t current_digit = 0;

    bool direct_only = false;

public:
    Display()
    {
        set_value(0, true);
        set_value(0, false);
    }

    void init()
    {
        SPI::init();
    }

    void show()
    {
        set_digit(current_values[current_digit]);
        select_digit(current_digit);
        current_digit++;
        if (current_digit == 4)
        {
            current_digit = 0;
        }
        for (uint8_t i = 0; i < 4; i++)
        {
            SPI::reset();
            SPI::send_byte(data1);
            SPI::send_byte(data2);
            SPI::send_byte(data3);
            SPI::send_byte(data4);
            SPI::latch();

        }
        SPI::reset();
        time++;
        if (time % HALF_SECOND == 0)
        {
            if (mode == Mode::M_DIRECT)
            {
                if (current_mode)
                {
                    data1 = (data1 & 0b11000000);
                    current_mode = false;
                }
                else
                {
                    data1 = (data1 & 0b11000000) | 0b00000001;
                    current_mode = true;
                }
                current_values = direct_values;
            }
            else if (mode == Mode::M_REVERSE)
            {
                if (current_mode)
                {
                    data1 = (data1 & 0b11000000);
                    current_mode = false;
                }
                else
                {
                    data1 = (data1 & 0b11000000) | 0b00000010;
                    current_mode = true;
                }
                current_values = reverse_values;
            }
            else if (direct_only)
            {
                current_values = direct_values;
            }
            else
            {
                if (time % (HALF_SECOND * 16) == 0)
                {
                    if (current_values == direct_values)
                    {
                        current_values = reverse_values;
                    }
                    else
                    {
                        current_values = direct_values;
                    }
                }
            }
        }
    }

    void set_mode(Mode value)
    {
        mode = value;
        uint8_t real_value = 0;
        switch (mode)
        {
        case M_1:
            real_value = 0;
            break;
        case M_10:
            real_value = 2;
            break;
        case M_100:
            real_value = 1;
            break;
        case M_1000:
            real_value = 3;
            break;
        case M_DIRECT:
            real_value = 1;
            break;
        case M_REVERSE:
            real_value = 2;
            break;
        default:
            real_value = 2;
            break;
        }
        data1 = (data1 & 0b11000000) | real_value;
    }

    void set_scale_value(uint16_t value)
    {
        data1 = (data1 & 0b00000011) | ((value & 0b00000011) << 6);
        data2 = value >> 2;
    }

    void set_scale(uint8_t scale)
    {
        direct_only = (scale == 0);
        static uint16_t mask = 0b1111111111;
        uint16_t value = mask >> (10 - scale);
        set_scale_value(value);
    }

    void select_digit(uint8_t digit)
    {
        uint8_t real_idx = digit;
        if (digit == 1)
        {
            real_idx = 3;
        }
        else if (digit == 3)
        {
            real_idx = 1;
        }
        // 0 - 0
        // 1 - 3
        // 2 - 2
        // 3 - 1
        data4 = 0b11110000 ^ (1 << (4 + real_idx));
    }

    void set_digit(uint8_t digit)
    {
        data3 = digit;
    }

    void set_value(uint16_t value, bool reverse)
    {
        if (value == 0)
        {
            value = 0;
        }
        if (value < 10)
        {
            value += 11;
        }
        else if (value < 15)
        {
            value += 10;
        }
        else if (value < 20)
        {
            value += 9;
        }
        else if (value < 30)
        {
            value += 8;
        }
        else if (value < 35)
        {
            value += 7;
        }
        else if (value < 45)
        {
            value += 6;
        }
        else if (value < 50)
        {
           value += 5;
        }
        else if (value < 55)
        {
           value += 4;
        }
        else if (value < 60)
        {
           value += 3;
        }
        else if (value < 73)
        {
           value += 2;
        }
        else if (value < 76)
        {
           value += 1;
        }

//        value = value * 25 / 10;
        if (reverse)
        {
            value = value * 2;
            set_value(reverse_values, value, MINUS);
        }
        else
        {
            value = value * 5 / 2;
            set_value(direct_values, value, EMPTY);
        }
    }

    void set_value(uint8_t *destination, uint16_t value, uint8_t sign)
    {
        value /= 10;
        destination[0] = DIGITS[value % 10];
        value /= 10;
        destination[1] = DIGITS[value % 10];
        value /= 10;
        destination[2] = DIGITS[value % 10] & DOT_MASK;
        destination[3] = sign;
    }

    void set_raw_value(uint16_t value, bool reverse)
    {
        if (reverse)
        {
            set_raw_value(reverse_values, value);
        }
        else
        {
            set_raw_value(direct_values, value);
        }
    }

    void set_raw_value(uint8_t *destination, uint16_t value)
    {

        destination[0] = DIGITS[value % 10];
        value /= 10;
        destination[1] = DIGITS[value % 10];
        value /= 10;
        destination[2] = DIGITS[value % 10];
        value /= 10;
        destination[3] = DIGITS[value % 10];
    }

};


#endif
