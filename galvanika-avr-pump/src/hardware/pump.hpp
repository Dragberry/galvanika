#ifndef PUMP_HPP_
#define PUMP_HPP_

#include "segment7x4.hpp"

enum Mode
{
    OFF = 0, AUTO = 0xA, PUMP = 0xb,
};

enum AutoMode
{
    IN_PROGRESS = 0, SETTINGS = 1,
};

enum AutoSettingsMode
{
    SET_DURATION = 0xd, SET_VALUE = 0xb,
};


class Pump
{
private:
    uint16_t time_ms = 0;
    Segment8x7 display;
    Mode mode = OFF;

    AutoMode auto_mode = IN_PROGRESS;
    AutoSettingsMode auto_settings_mode = SET_DURATION;
    uint8_t auto_duration = 0;
    uint8_t auto_duration_progress = 0;
    uint8_t auto_value = 0xFF;


public:
    void init()
    {
        sbi(DDRB, PB1);
        sbi(PORTB, PB1);

        cbi(TCCR1B, WGM13);
        sbi(TCCR1B, WGM12);
        cbi(TCCR1A, WGM11);
        sbi(TCCR1A, WGM10);

        display.init();
        set_off_mode();
    }

    void start()
    {
        sbi(TCCR1B, CS12);
        cbi(TCCR1B, CS11);
        sbi(TCCR1B, CS10);

        sbi(TCCR1A, COM1A1);
        cbi(TCCR1A, COM1A0);

        outb(OCR1AL, auto_value);
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
        if (mode == Mode::AUTO)
        {
            time_ms++;
            switch (auto_mode)
            {
            case AutoMode::SETTINGS:
                if (time_ms % 256 == 0)
                {
                    display.blink();
                }
                break;
            case AutoMode::IN_PROGRESS:
                if (time_ms % 1024 == 0)
                {
                    if (auto_duration_progress > 0)
                    {
                        auto_duration_progress--;
                        display.set_value(auto_duration_progress);
                    }
                    else
                    {
                        stop();
                    }
                }
                break;
            default:
                break;
            }
        }

    }

    void set_off_mode()
    {
        stop();
        mode = OFF;
        display.set_enabled(true);
        display.set_mode(mode);
        display.set_value(0x0ff, 16);
    }

    void set_auto_mode()
    {
        mode = AUTO;
        auto_mode = AutoMode::IN_PROGRESS;
        display.set_enabled(true);
        display.set_mode(mode);
        auto_duration_progress = auto_duration;
        display.set_value(auto_duration_progress);
        start();
    }

    void set_pump_mode()
    {
        mode = PUMP;
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
                   break;
                case AutoSettingsMode::SET_DURATION:
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
        case OFF:
            set_auto_mode();
            break;
        case AUTO:
            if (auto_mode == AutoMode::IN_PROGRESS)
            {
                set_pump_mode();
            }
            break;
        case PUMP:
            set_off_mode();
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
        case Mode::PUMP:
            outb(OCR1AL, value);
            display.set_value(value);
            break;
        case Mode::AUTO:
            switch (auto_mode)
            {
            case AutoMode::SETTINGS:
                switch (auto_settings_mode)
                {
                case AutoSettingsMode::SET_DURATION:
                   auto_duration = (0xff - value) / 8;
                   display.set_value(auto_duration);
                   break;
               case AutoSettingsMode::SET_VALUE:
                   auto_value = value;
                   display.set_value(auto_value);
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
