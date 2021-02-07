#include <avr/io.h>
#include <avr/interrupt.h>
#include "avr/hardware/spi.hpp"
#include "avr/hardware/uart.hpp"
#include "hardware/pump.hpp"
#include "hardware/segment7x4.hpp"

static volatile uint16_t time_ms = 0;

Pump pump;

void init_buttons()
{
    // State change interruption
    cbi(DDRD, PD2);
    sbi(PORTD, PD2);
    cbi(MCUCR, ISC01);
    sbi(MCUCR, ISC00);
    sbi(GICR, INT0);

    cbi(DDRD, PD3);
    sbi(PORTD, PD3);
    cbi(MCUCR, ISC11);
    sbi(MCUCR, ISC10);
    sbi(GICR, INT1);

}

void init_adc()
{
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

void init_sensors()
{
    // 1 ms
    cbi(TCCR2, CS22);
    sbi(TCCR2, CS21);
    sbi(TCCR2, CS20);
    sbi(TCCR2, WGM21);
    cbi(TCCR2, WGM20);
    outb(OCR2, 255);
    sbi(TIMSK, OCIE2);
}

int main()
{
    pump.init();
    init_buttons();
    init_adc();
    init_sensors();

    sei();

    while (true)
        ;
}

volatile uint8_t mode = 0;

ISR(ADC_vect)
{
    pump.on_adc((uint8_t) (ADC >> 8));
}

class ButtonState
{
private:
    volatile uint8_t *port;
    uint8_t pin;
    bool pressed;
    uint16_t time;

public:
    ButtonState(volatile uint8_t *port, uint8_t pin) :
            port(port), pin(pin)
    {
        pressed = false;
        time = 0;
    }

    void increment_time()
    {
        time++;
    }

    template <typename OnClick, typename OnLongClick>
    void on_state_changed(OnClick&& on_click, OnLongClick&& on_long_click)
    {
        pressed = !(*port & (1 << pin));
        if (pressed)
        {
            time = 0;
        }
        else
        {
            if (time >= 1000)
            {
                on_long_click();
            }
            else
            {
                on_click();
            }
        }
    }

};

ButtonState button_0 = ButtonState(&PIND, PD2);
ButtonState button_1 = ButtonState(&PIND, PD3);

ISR(TIMER2_COMP_vect)
{
    pump.increment_time();
    button_0.increment_time();
    button_1.increment_time();
    if (++time_ms == 50000)
    {
        time_ms = 0;
    }
}

ISR(TIMER0_OVF_vect)
{
    pump.show();
}

ISR(INT0_vect)
{
    button_0.on_state_changed(
            [&]() -> void
            {
                pump.on_button_0_click();
            },
            [&]() -> void
            {
                pump.on_button_0_long_click();
            }
    );
}

ISR(INT1_vect)
{
    button_1.on_state_changed(
            [&]() -> void
            {
                pump.on_button_1_click();
            },
            [&]() -> void
            {
                pump.on_button_1_long_click();
            }
    );
}

