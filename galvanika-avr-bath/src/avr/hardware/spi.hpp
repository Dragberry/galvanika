#ifndef SPI_HPP_
#define SPI_HPP_

#include "../software/operators.hpp"
#include <avr/io.h>
#include <stdint.h>

#define SPI_DDR DDRB
#define SPI_PORT PORTB
#define SPI_SCK PB5
#define SPI_MOSI PB3
#define SPI_SS PB2

namespace SPI
{
	void init();

	void send_byte(const uint8_t byte);

	void send_single_byte(const uint8_t byte);

	void reset();

	void latch();

	void stop();
}

#endif
