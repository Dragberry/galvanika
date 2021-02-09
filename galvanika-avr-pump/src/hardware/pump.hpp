#ifndef PUMP_HPP_
#define PUMP_HPP_

#include <avr/eeprom.h>
#include "segment7x4.hpp"

enum Mode
{
    MANUAL = 0, AUTO = 0xA
};

enum ManualMode
{
    OFF = 0, ON = 1
};

enum AutoMode
{
    IN_PROGRESS = 0, SETTINGS = 1,
};

enum AutoSettingsMode
{
    SET_DURATION = 0xd, SET_VALUE = 0xb,
};


class AutoPumpState
{
private:
    static const uint8_t EEMEM STORED_AUTO_DURATION;
    static const uint8_t EEMEM STORED_AUTO_VALUE;
public:
    uint8_t duration = 12;
    uint8_t value = 127;
    uint16_t duration_progress = 0;

    enum AutoPumpStatus
    {
        DISABLED, READY, START, PUMP, STOP, FILTER
    } state = DISABLED;

    void ready()
    {
        state = READY;
    }

    void load()
    {
        duration = eeprom_read_byte(&STORED_AUTO_DURATION);
        value = eeprom_read_byte(&STORED_AUTO_VALUE);
        ready();
    }

    void save_value()
    {
        eeprom_update_byte((uint8_t*) &STORED_AUTO_VALUE, value);
    }

    void save_duration()
    {
        eeprom_update_byte((uint8_t*) &STORED_AUTO_DURATION, duration);
    }

    void activate_sensors()
    {
        sbi(PORTB, PB0);
        sbi(PORTD, PD5);
    }

    void deactivate_sensors()
    {
        cbi(PORTB, PB0);
        cbi(PORTD, PD5);
    }


    AutoPumpStatus check()
    {
        bool top_sensor = check_bit(PIND, PIN6);
        bool bottom_sensor = check_bit(PIND, PIN7);
        switch (state)
        {
        case DISABLED:
            break;
        case READY:
            if (!top_sensor)
            {
                state = START;
                duration_progress = duration;
            }
            else
            {
                state = FILTER;
                duration_progress = 0;
            }
            break;
        case START:
            state = PUMP;
            duration_progress = duration;
            break;
        case PUMP:
            if (top_sensor || (bottom_sensor && duration_progress == 0))
            {
                state = STOP;
            }
            else if (!bottom_sensor && duration_progress == 0)
            {
                state = DISABLED;
            }
            break;
        case STOP:
            state = FILTER;
            duration_progress = 0;
            break;
        case FILTER:
            if (!bottom_sensor)
            {
                state = START;
            }
            break;
        default:
            break;
        }
        return state;
    }

    void do_progress()
    {
        switch (state)
        {
        case FILTER:
            duration_progress++;
            break;
        case PUMP:
            if (duration_progress > 0)
            {
                duration_progress--;
            }
            break;
        default:
            break;
        }
    }

    template <typename ValueSetter>
    void display_value(ValueSetter&& set_value)
    {
        switch (state)
        {
        case DISABLED:
            set_value(0x0FF, 16);
            break;
        default:
            set_value(duration_progress, 10);
            break;
        }
    }
};

class Pump
{
private:
    uint16_t time_ms = 0;
    Segment8x7 display;
    Mode mode = Mode::MANUAL;
    ManualMode manual_mode = ManualMode::OFF;
    AutoMode auto_mode = IN_PROGRESS;
    AutoSettingsMode auto_settings_mode = SET_DURATION;

    AutoPumpState auto_pump_state;

public:
    void init()
    {
        // Sensor bottom output
        sbi(DDRB, PB0);
        cbi(PORTB, PB0);
        // Sensor bottom input
        cbi(DDRD, PD7);
        cbi(PORTD, PD7);
        // Sensor top output
        sbi(DDRD, PD5);
        cbi(PORTD, PD5);
        // Sensor top input
        cbi(DDRD, PD6);
        cbi(PORTD, PD6);

        sbi(DDRB, PB1);
        sbi(PORTB, PB1);

        cbi(TCCR1B, WGM13);
        sbi(TCCR1B, WGM12);
        cbi(TCCR1A, WGM11);
        sbi(TCCR1A, WGM10);

        display.init();
        set_manual_off_mode();
    }

    void start()
    {
        sbi(TCCR1B, CS12);
        cbi(TCCR1B, CS11);
        sbi(TCCR1B, CS10);

        sbi(TCCR1A, COM1A1);
        cbi(TCCR1A, COM1A0);

        outb(OCR1AL, auto_pump_state.value);
    }

    void stop()
    {
        cbi(TCCR1B, CS12);
        cbi(TCCR1B, CS11);
        cbi(TCCR1B, CS10);

        cbi(TCCR1A, COM1A1);
        cbi(TCCR1A, COM1A0);

        outb(OCR1AL, 0xFF);
    }

    void show()
    {
        display.show();
    }

    void increment_time()
    {
        time_ms++;
        switch (mode)
        {
        case Mode::MANUAL:
            switch (manual_mode)
            {
            case ManualMode::ON:
                if (time_ms % 256 == 0)
                {
                    display.blink();
                }
                break;
            default:
                display.set_enabled(true);
                break;
            }
            break;
        case Mode::AUTO:
            switch (auto_mode)
            {
            case AutoMode::SETTINGS:
                if (time_ms % 256 == 0)
                {
                    display.blink();
                }
                break;
            case AutoMode::IN_PROGRESS:
                switch (time_ms % 256)
                {
                case 0:
                    auto_pump_state.activate_sensors();
                    break;
                case 2:
                    switch(auto_pump_state.check())
                    {
                    case AutoPumpState::STOP:
                    case AutoPumpState::DISABLED:
                        stop();
                        break;
                    case AutoPumpState::START:
                        start();
                        break;
                    default:
                        break;
                    }
                    auto_pump_state.deactivate_sensors();
                    break;
                }

                if (time_ms % 1024 == 0)
                {
                    auto_pump_state.do_progress();
                    auto_pump_state.display_value(
                            [&](uint16_t value, uint8_t base) -> void { display.set_value(value, base); }
                    );
                }
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }

    }

    void set_auto_mode()
    {
        auto_pump_state.load();
        mode = AUTO;
        auto_mode = AutoMode::IN_PROGRESS;
        display.set_enabled(true);
        display.set_mode(mode);
        display.set_value(auto_pump_state.duration_progress);
    }

    void set_manual_off_mode()
    {
        stop();
        mode = Mode::MANUAL;
        manual_mode = ManualMode::OFF;
        display.set_enabled(true);
        display.set_mode(mode);
        display.set_value(0x0ff, 16);
    }

    void set_manual_on_mode()
    {
        mode = Mode::MANUAL;
        manual_mode = ManualMode::ON;
        display.set_enabled(true);
        display.set_mode(mode);
        display.set_value(0);
        start();
    }

    void on_button_0_click()
    {
        switch (mode)
        {
        case Mode::AUTO:
            switch (auto_mode)
            {
            case AutoMode::IN_PROGRESS:
                auto_pump_state.ready();
                break;
            case AutoMode::SETTINGS:
                switch (auto_settings_mode)
                {
                case AutoSettingsMode::SET_DURATION:
                    auto_settings_mode = AutoSettingsMode::SET_VALUE;
                    display.set_mode(auto_settings_mode);
                    break;
                case AutoSettingsMode::SET_VALUE:
                    auto_settings_mode = AutoSettingsMode::SET_DURATION;
                    display.set_mode(auto_settings_mode);
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
            break;
        case Mode::MANUAL:
            switch(manual_mode)
            {
            case ManualMode::OFF:
                set_manual_on_mode();
                break;
            case ManualMode::ON:
                set_manual_off_mode();
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

    void on_button_0_long_click()
    {
        switch (mode)
        {
        case Mode::AUTO:
            switch (auto_mode)
            {
            case AutoMode::SETTINGS:
                switch (auto_settings_mode)
                {
                case AutoSettingsMode::SET_VALUE:
                   auto_pump_state.save_value();
                   break;
                case AutoSettingsMode::SET_DURATION:
                    auto_pump_state.save_duration();
                   break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

    void on_button_1_click()
    {
        switch (mode)
        {
        case MANUAL:
            set_auto_mode();
            break;
        case AUTO:
            if (auto_mode == AutoMode::IN_PROGRESS)
            {
                set_manual_off_mode();
            }
            break;
        default:
            break;
        }
    }

    void on_button_1_long_click()
    {
        switch (mode)
        {
        case AUTO:
            switch (auto_mode)
            {
            case AutoMode::IN_PROGRESS:
                stop();
                auto_mode = AutoMode::SETTINGS;
                auto_settings_mode = AutoSettingsMode::SET_DURATION;
                display.set_mode(auto_settings_mode);
                break;
            case AutoMode::SETTINGS:
                set_auto_mode();
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

    void on_adc(uint8_t value)
    {
        switch (mode)
        {
        case Mode::MANUAL:
            switch (manual_mode)
            {
            case ManualMode::ON:
                outb(OCR1AL, value);
                display.set_value(value);
                break;
            default:
                display.set_value(0x0ff, 16);
                break;
            }
            break;
        case Mode::AUTO:
            switch (auto_mode)
            {
            case AutoMode::SETTINGS:
                switch (auto_settings_mode)
                {
                case AutoSettingsMode::SET_DURATION:
                   auto_pump_state.duration = (0xff - value) / 4;
                   display.set_value(auto_pump_state.duration);
                   break;
               case AutoSettingsMode::SET_VALUE:
                   auto_pump_state.value = value;
                   display.set_value(auto_pump_state.value);
                   break;
                }
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

};

#endif
