#include "timer.h"

#include <avr/io.h>

#include <avr/interrupt.h>

static volatile uint32_t sys_ticks;
static volatile uint8_t sub_ticks;

ISR(TIM0_OVF_vect)
{
    sub_ticks++;
    if (sub_ticks == 36) {
        sys_ticks += 2;
        sub_ticks = 0;
    }
}

void init_timer(void)
{
   // Set OC0B pin to output
   PORTB |= ~_BV(PB1);    // pull it high before switching to output
   DDRB |= _BV(PB1);

   // Clear OC0B on Compare Match, set OC0A at TOP (inverted mode)
   // Inverted mode is used in order to remove narrow spikes in OFF mode.
   // Fast PWM mode
   TCCR0A = _BV(COM0B1) | _BV(WGM00) | _BV(WGM01);

   OCR0A = 0xFF;     // initial state OFF (inverted)

   // no prescaling
   TCCR0B = _BV(CS00);

   sys_ticks = 0;
   sub_ticks = 0;

   TIMSK0 = _BV(TOIE0);
}

void set_pwm(uint8_t duty)
{
    if (duty == 0) {
        TCCR0A &= ~_BV(COM0B1);  // disconnect OC0B
        PORTB &= ~_BV(PB0);
    } else {
        TCCR0A |= _BV(COM0B1);  // connect OC0B
        OCR0B = duty;
    }
}

uint32_t get_ticks(void)
{
   uint32_t mseconds;
   uint8_t sreg = SREG;

   cli();

   mseconds = sys_ticks;

   SREG = sreg;

   return mseconds;
}

void sleep_ms(uint32_t mseconds)
{
   uint32_t startTime = get_ticks();

   while ( (startTime + mseconds) > get_ticks() );
}

