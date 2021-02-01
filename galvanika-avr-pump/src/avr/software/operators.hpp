#ifndef OPERATORS_HPP_
#define OPERATORS_HPP_

#include <stdio.h>
#include <stdlib.h>

#define INT16_MAX 0x7fff
#define INT16_MIN (-INT16_MAX - 1)

#define 	outb(addr, data)  	addr = (data)
#define 	inb(addr)   		(addr)
#define 	outw(addr, data)  	addr = (data)
#define 	inw(addr)   		(addr)
#define 	BV(bit)   			(1<<(bit))
#define     check_bit(addr, bit)(addr & BV(bit))
#define 	cbi(reg, bit)   	reg &= ~(BV(bit))
#define 	sbi(reg, bit)   	reg |= (BV(bit))

void * operator new(size_t size);

void operator delete(void * ptr);

void * operator new[](size_t size);

void operator delete[](void * ptr);

#endif
