#include <avr/io.h>
#include "hardware/segment8x7.hpp"
#include <avr/interrupt.h>
#include "hardware/display.hpp"

volatile enum AdcChannel
{
    DUTY_CYCLE, DIRECT, REVERSE, IDLE
} adc_channel = DUTY_CYCLE;

Display display;

volatile Mode mode = Mode::M_DIRECT;
volatile uint16_t scale = 100;
static volatile uint16_t time_ms = 0;
static volatile uint16_t time = 0;
static volatile uint8_t reverse = 0;
static volatile uint8_t reverse_progress = 0;
static const uint8_t direct = 20;
static volatile uint8_t direct_progress = 0;

void adc_direct()
{
    adc_channel = DIRECT;
    // channel 4
    cbi(ADMUX, MUX3);
    sbi(ADMUX, MUX2);
    cbi(ADMUX, MUX1);
    cbi(ADMUX, MUX0);

    sbi(ADMUX, REFS1);
    sbi(ADMUX, REFS0);

    sbi(ADCSRA, ADSC);
}

void adc_reverse()
{
    adc_channel = REVERSE;
    // channel 5
    cbi(ADMUX, MUX3);
    sbi(ADMUX, MUX2);
    cbi(ADMUX, MUX1);
    sbi(ADMUX, MUX0);

    sbi(ADMUX, REFS1);
    sbi(ADMUX, REFS0);

    sbi(ADCSRA, ADSC);
}

void adc_duty_cycle()
{
    adc_channel = DUTY_CYCLE;
    // channel 0
    cbi(ADMUX, MUX3);
    cbi(ADMUX, MUX2);
    cbi(ADMUX, MUX1);
    cbi(ADMUX, MUX0);

    sbi(ADMUX, REFS1);
    sbi(ADMUX, REFS0);

    sbi(ADCSRA, ADSC);
}

void init()
{
    sbi(DDRB, PB1);

    // Indicator's timer setup
    sbi(TCCR0, CS02);
    cbi(TCCR0, CS01);
    cbi(TCCR0, CS00);
    sbi(TIMSK, TOIE0);

    // PWM timer setup 1ms
    sbi(TCCR2, CS22);
    cbi(TCCR2, CS21);
    cbi(TCCR2, CS20);
    sbi(TCCR2, WGM21);
    cbi(TCCR2, WGM20);
    outb(OCR2, 255);
    sbi(TIMSK, OCIE2);

    // State change interruption
    cbi(DDRD, PD2);
    sbi(PORTD, PD2);
    sbi(MCUCR, ISC01);
    cbi(MCUCR, ISC00);
    sbi(GICR, INT0);

    cbi(DDRD, PD1);
    sbi(PORTD, PD1);

    // ADC
    cbi(ADMUX, REFS1);
    sbi(ADMUX, REFS0);
    cbi(ADMUX, ADLAR);

    sbi(ADCSRA, ADEN);
    sbi(ADCSRA, ADIE);

    sbi(ADCSRA, ADPS2);
    sbi(ADCSRA, ADPS1);
    sbi(ADCSRA, ADPS0);

    adc_duty_cycle();

    display.set_mode(mode);
    sbi(PORTB, PB1);
}


int main()
{
    SPI::init();
    init();
    sei();


    display.init();
    while(true);
}

ISR(ADC_vect)
{
    switch (adc_channel)
    {
    case DUTY_CYCLE:
        reverse = ADC / 100;
        display.set_scale(reverse);
        if (check_bit(PORTB, PB1))
        {
            adc_direct();
        }
        else
        {
            adc_reverse();
        }
        break;
    case DIRECT:
//        display.set_raw_value(ADC, false);
        display.set_value(ADC, false);
        adc_duty_cycle();
        break;
    case REVERSE:
//        display.set_raw_value(ADC, true);
        display.set_value(ADC, true);
        adc_duty_cycle();
        break;
    default:
        adc_duty_cycle();
        break;
    }
}

ISR(INT0_vect)
{
    switch(mode)
    {
    case M_1:
        mode = M_10;
        scale = 10;
        break;
    case M_10:
        mode = M_100;
        scale = 100;
        break;
    case M_100:
        mode = M_1000;
        scale = 1000;
        break;
    case M_1000:
        mode = M_DIRECT;
        scale = 1;
        sbi(PORTB, PB1);
        adc_channel = IDLE;
        break;
    case M_DIRECT:
        mode = M_REVERSE;
        scale = 1;
        cbi(PORTB, PB1);
        adc_channel = IDLE;
        break;
    case M_REVERSE:
        mode = M_1;
        scale = 1;
        break;
    default:
        break;
    }
    display.set_mode(mode);
}


ISR(TIMER0_OVF_vect)
{
    display.show();
}

ISR(TIMER2_COMP_vect)
{
    time_ms++;
    if (time_ms % scale == 0)
    {
        if (direct_progress == direct)
        {
            if (reverse != 0 && reverse_progress == 0)
            {
                // change direction to reverse
                if (mode != Mode::M_DIRECT && mode != Mode::M_REVERSE)
                {
                    cbi(PORTB, PB1);
                    adc_channel = IDLE;
                }
            }
            reverse_progress++;
            if (reverse_progress > reverse)
            {
                reverse_progress = 0;
                direct_progress = 0;
                // change directions to direct
                if (mode != Mode::M_DIRECT && mode != Mode::M_REVERSE)
                {
                    sbi(PORTB, PB1);
                    adc_channel = IDLE;
                }
            }
        }
        else
        {
            direct_progress++;
        }
        time++;
    }
    if (time_ms == 50000)
    {
        time_ms = 0;
        time = 0;
    }

}

