#include <inttypes.h>

#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdbool.h>

#include "timer.h"
#include "adc.h"

#define TEMP_TO_ADC_VAL(x)   ((x) * 2.33)
#define TEMP_30_C   (70)   // 30 * (256/110)
#define TEMP_35_C   (81)
#define TEMP_40_C   (93)
#define TEMP_45_C   (104)
#define TEMP_50_C   (116)
#define TEMP_60_C   (140)
#define TEMP_70_C   (163)
#define TEMP_80_C   (186)

#define TEMP_HISTEREZIS   (3)

typedef struct {
    uint8_t temp;
    uint8_t fan_speed;
} mode_t;

static mode_t modes[] = {
    {TEMP_40_C, 190u},
    {TEMP_45_C, 210u},
    {TEMP_50_C, 230u},
    {TEMP_70_C, 255u},
    {0, 0}
};

// -1 -- off
static int8_t current_mode = -1;


static void set_mode(int8_t mode_index)
{
    if (mode_index > -1) {
        set_pwm(modes[mode_index].fan_speed);
    } else {
        set_pwm(0);
    }

    current_mode = mode_index;
}

static void process(uint8_t curr_temp)
{
    int8_t highest_mode = -1;
    for (uint8_t i = 0; modes[i].temp != 0; i++) {
        if (curr_temp >= modes[i].temp) {
            highest_mode = i;
        }
    }

    if (highest_mode < current_mode) {
        if (modes[current_mode].temp - curr_temp < TEMP_HISTEREZIS) {
            return;   // do not switch to lower mode if difference is too small
        }
    }
    if (highest_mode != current_mode) {
        set_mode(highest_mode);
    }
}

#define VALUE_BUF_SIZE 8
static uint8_t value_buf[VALUE_BUF_SIZE] = {0};

static uint8_t get_average_adc()
{
    for (uint8_t i = 0; i < VALUE_BUF_SIZE - 1; i++) {
        value_buf[i+1] = value_buf[i];
    }
    value_buf[0] = GET_ADC_VALUE();
    uint16_t sum = 0;
    for (uint8_t i = 0; i < VALUE_BUF_SIZE; i++) {
        sum += value_buf[i];
    }
    return (uint8_t)(sum / VALUE_BUF_SIZE);
}

int main(void)
{
    uint8_t adc_val;
    wdt_enable(WDTO_2S);

    INIT_ADC();

    init_timer();

    MCUCR |= _BV(PUD);  // disable pull-up for all inputs (ADC)

    sei();

    sleep_ms(10);
    set_pwm(0xFF);  // testing fan on start up
    sleep_ms(1000);

    set_pwm(0);    // initial state fan is off

    while (1) {
        adc_val = get_average_adc();
        process(adc_val);

        sleep_ms(200);
        wdt_reset();
    }

    return 0;
}

