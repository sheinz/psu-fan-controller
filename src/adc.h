#ifndef __ADC_H__
#define __ADC_H__

#include <inttypes.h>


// ADC input is PB4 which is ADC2 (MUX[1:0] = 10)
// VCC is used as analog reference
#define INIT_ADC()             \
   {                          \
      ADMUX |=  _BV(ADLAR) | _BV(MUX1) | _BV(REFS0);                        \
      DIDR0 |= _BV(ADC1D);                                                 \
      ADCSRA = _BV(ADATE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0) | _BV(ADEN) | _BV(ADSC);       \
   }

#define ADC_OFF()     ADCSRA &= ~_BV(ADEN)
#define ADC_ON()      ADCSRA |= _BV(ADEN) | _BV(ADSC)


#define GET_ADC_VALUE()   ADCH      // use macros instead


#endif // __ADC_H__
