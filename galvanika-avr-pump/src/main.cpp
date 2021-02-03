#include <avr/io.h>
#include <avr/interrupt.h>
#include "avr/hardware/spi.hpp"
#include "avr/hardware/uart.hpp"
#include "hardware/segment7x4.hpp"

static volatile uint16_t time_ms = 0;

Segment8x7 display;

void init_buttons() {
    // State change interruption
   cbi(DDRD, PD2);
   sbi(PORTD, PD2);
   sbi(MCUCR, ISC01);
   cbi(MCUCR, ISC00);
   sbi(GICR, INT0);

   cbi(DDRD, PD3);
   sbi(PORTD, PD3);
   sbi(MCUCR, ISC11);
   cbi(MCUCR, ISC10);
   sbi(GICR, INT1);

}

void init_adc() {
    // channel 0
    cbi(ADMUX, MUX3);
    cbi(ADMUX, MUX2);
    cbi(ADMUX, MUX1);
    cbi(ADMUX, MUX0);

    cbi(ADMUX, REFS1);
    sbi(ADMUX, REFS0);
    sbi(ADMUX, ADLAR);

    sbi(ADCSRA, ADFR);
    sbi(ADCSRA, ADEN);
    sbi(ADCSRA, ADIE);

    sbi(ADCSRA, ADPS2);
    sbi(ADCSRA, ADPS1);
    sbi(ADCSRA, ADPS0);

    sbi(ADCSRA, ADSC);
}

void init_pwm() {
    sbi(DDRB, PB1);

    sbi(TCCR1B, CS12);
    cbi(TCCR1B, CS11);
    sbi(TCCR1B, CS10);

    // Fast PWM 8-bit
    cbi(TCCR1B, WGM13);
    sbi(TCCR1B, WGM12);
    cbi(TCCR1A, WGM11);
    sbi(TCCR1A, WGM10);

    sbi(TCCR1A, COM1A1);
    cbi(TCCR1A, COM1A0);

    outb(OCR1AL, 127);
}

void init_sensors() {
    cbi(DDRD, PD5);
    cbi(DDRD, PD7);
    sbi(DDRD, PD6);
    sbi(PORTD, PD6);

    // 1 ms
    cbi(TCCR2, CS22);
    sbi(TCCR2, CS21);
    sbi(TCCR2, CS20);
    sbi(TCCR2, WGM21);
    cbi(TCCR2, WGM20);
    outb(OCR2, 255);
    sbi(TIMSK, OCIE2);
}

void start_pump() {
    sbi(TCCR1B, CS12);
    cbi(TCCR1B, CS11);
    sbi(TCCR1B, CS10);

    sbi(TCCR1A, COM1A1);
    cbi(TCCR1A, COM1A0);
}

void stop_pump() {
    cbi(TCCR1B, CS12);
    cbi(TCCR1B, CS11);
    cbi(TCCR1B, CS10);

    cbi(TCCR1A, COM1A1);
    cbi(TCCR1A, COM1A0);

    sbi(PORTB, PB1);
}

int main() {
    display.init();
    init_buttons();
    init_pwm();
    init_adc();
    init_sensors();

    start_pump();
    sei();

    while(true);
}

volatile uint8_t mode = 0;

ISR(ADC_vect) {
    uint8_t value = (uint8_t)(ADC >> 8);
    outb(OCR1AL, value);
    display.set_mode(mode);
    display.set_value(value);
}


ISR(TIMER2_COMP_vect) {
    time_ms++;
//    if (time_ms % 250 == 0) {
//        if (check_bit(PIND, PD5) && check_bit(PIND, PD5)) {
//            stop_pump();
//        } else {
//            start_pump();
//        }
//    }
    if (time_ms == 50000)
    {
        time_ms = 0;
    }
}

ISR(TIMER0_OVF_vect)
{
    display.show();
}

ISR(INT0_vect)
{
    stop_pump();
}

ISR(INT1_vect)
{
    mode++;
    if (mode == 16)
    {
        mode = 0;
    }
    display.set_mode(mode);
}

