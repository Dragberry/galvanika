#include <avr/interrupt.h>
#include "segment8x7.hpp"

const uint8_t Segment8x7::DIGITS[10] = {
        0b11111100,
        0b01100000,
        0b11011010,
        0b11110010,
        0b01100110,
        0b10110110,
        0b10111110,
        0b11100000,
        0b11111110,
        0b11110110
};

uint8_t Segment8x7::data[BUFFER_SIZE] =  { 1, 2, 3, 4, 5, 6, 7, 8 };

volatile uint8_t Segment8x7::display_index = 0;

//
//ISR(TIMER0_COMP_vect)
//{
//    Segment8x7::display_segment();
//}



