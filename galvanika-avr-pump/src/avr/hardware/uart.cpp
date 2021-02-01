#include "../software/operators.hpp"
#include "uart.hpp"

void UART::init(uint16_t baud_rate) {
    sbi(UCSRA, U2X);

    sbi(UCSRB, RXEN);
    sbi(UCSRB, TXEN);
    // 1 stop bit
    cbi(UCSRC, USBS);
    // 8 bit
    sbi(UCSRC, UCSZ0);
    sbi(UCSRC, UCSZ1);

    outb(UBRRH, baud_rate >> 8);
    outb(UBRRL, baud_rate);
}

void UART::send(uint8_t byte) {
    UDR = byte;
}

void send_string(char* string) {
    uint8_t index = 0;
    while (string[index] != '\0') {
        UART::send(string[index++]);
    }
}
