#ifndef UART_HPP_
#define UART_HPP_

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

namespace UART {
    void init(uint16_t baud_rate);

    void send(uint8_t byte);

    void send_string(char* string);
}

#endif
