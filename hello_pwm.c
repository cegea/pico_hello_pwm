#include "pico/stdlib.h"
#include "hardware/pwm.h"

typedef struct 
{
    uint8_t gpio;
    uint32_t slice;
    uint32_t channel;
    uint32_t frequency;
    uint16_t dutyCycle;
} pwm_gpio_t;

uint32_t pwm_set_freq_duty(uint slice_num, uint chan, float frequency, int duty_cycle)
{
    if (frequency == 0) return 0;  // Div by 0 control

    float clock = 125.0e6f;  // Clock base (125 MHz)
    float divider = clock / (4096.f * frequency);

    if (divider < 1.f) divider = 1.f;
    else if (divider > 256.f) divider = 256.f;

    uint32_t wrap = (uint32_t)((clock / divider) / frequency) - 1;
    if (wrap > 65535) wrap = 65535;  // Limit values

    float level = (float)wrap * ((float)duty_cycle / 100.f);

    pwm_set_clkdiv(slice_num, divider);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_chan_level(slice_num, chan, (uint16_t)level);

    return wrap;
}

int main() 
{
    pwm_gpio_t blue  = { .gpio = 20, .frequency = 500, .dutyCycle = 100};
    pwm_gpio_t green = { .gpio = 19, .frequency = 500, .dutyCycle = 0 };
    pwm_gpio_t red   = { .gpio = 18, .frequency = 500, .dutyCycle = 0 };

    gpio_set_function(blue.gpio, GPIO_FUNC_PWM);
    gpio_set_function(red.gpio, GPIO_FUNC_PWM);
    gpio_set_function(green.gpio, GPIO_FUNC_PWM);

    blue.slice = pwm_gpio_to_slice_num(blue.gpio);
    green.slice = pwm_gpio_to_slice_num(green.gpio);
    red.slice = pwm_gpio_to_slice_num(red.gpio);

    blue.channel = pwm_gpio_to_channel(blue.gpio);
    green.channel = pwm_gpio_to_channel(green.gpio);
    red.channel = pwm_gpio_to_channel(red.gpio);

    pwm_set_freq_duty(blue.slice, blue.channel, blue.frequency, blue.dutyCycle);
    pwm_set_freq_duty(green.slice, green.channel, green.frequency, green.dutyCycle);
    pwm_set_freq_duty(red.slice, red.channel, red.frequency, red.dutyCycle);

    pwm_set_output_polarity(blue.slice, true, true);
    pwm_set_output_polarity(green.slice, true, true);
    pwm_set_output_polarity(red.slice, true, true);

    pwm_set_enabled(blue.slice, true);
    pwm_set_enabled(green.slice, true);
    pwm_set_enabled(red.slice, true);

    int step = 2;
    int delay_ms = 10; // Delay between steps

    while (1) 
    {
        for (int duty = 0; duty <= 100; duty += step)
        {
            pwm_set_freq_duty(blue.slice, blue.channel, blue.frequency, duty);
            sleep_ms(delay_ms);
        }
        for (int duty = 100; duty >= 0; duty -= step)
        {
            pwm_set_freq_duty(blue.slice, blue.channel, blue.frequency, duty);
            sleep_ms(delay_ms);
        }

        for (int duty = 0; duty <= 100; duty += step)
        {
            pwm_set_freq_duty(green.slice, green.channel, green.frequency, duty);
            sleep_ms(delay_ms);
        }
        for (int duty = 100; duty >= 0; duty -= step)
        {
            pwm_set_freq_duty(green.slice, green.channel, green.frequency, duty);
            sleep_ms(delay_ms);
        }

        for (int duty = 0; duty <= 100; duty += step)
        {
            pwm_set_freq_duty(red.slice, red.channel, red.frequency, duty);
            sleep_ms(delay_ms);
        }
        for (int duty = 100; duty >= 0; duty -= step)
        {
            pwm_set_freq_duty(red.slice, red.channel, red.frequency, duty);
            sleep_ms(delay_ms);
        }
    }

    return 0;
}
