#ifndef _ADC_ENGINE_H_
#define _ADC_ENGINE_H_

// NOTE:
// This library assumes that you only care about 8 bit accuracy, and
// wish for maximum ADC conversion speed, and that the ADC sampling
// channels are not used for any other purpose during the sampling.

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdlib.h>
#include <math.h>

#ifdef DEBUG
#define C5ON() PORTC |= (1 << PINC5)
#define C5OFF() PORTC &= ~(1 << PINC5)
#endif

#define adc_disable()	ADCSRA = (1<<ADIF)
#define adc_read()		ADCH

// Can't really go much faster unless you write the ADC ISR code in assembly:
// 1. Current execution time of the ISR is about 80 cycles.
// 2. Since the ADC executes every 13.5 cycles, the current period comes to about 100 cycles.
// 3. The next smallest period is about 50 cycles, which is simply not enough to do anything in the ISR.
//#define ADC_PRESCALER   3 // Divide by 8.
#define ADC_PRESCALER   3

#define ADC_MODE_AUTO   1
#define ADC_MODE_TIMER0 2
#define ADC_MODE_TIMER1 3
#define ADC_MODE_MANUAL 4

void adc_enable(void);

void adc_init(	uint8_t mode,
				uint8_t*  buffer,
				uint8_t   bufferLen,
				uint8_t   mask,
				uint8_t*  schedule,
				uint8_t*  muxSchedule,
				uint8_t   scheduleLen);

void adc_trigger(void);

#endif