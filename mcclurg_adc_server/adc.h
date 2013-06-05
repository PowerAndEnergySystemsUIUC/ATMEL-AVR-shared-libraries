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
#define toggleC5() PORTC ^= (1 << PINC5)
#endif

#define adc_disable()	ADCSRA = (1<<ADIF)
#define adc_read()		ADC

//#define ADC_PRESCALER   2 // Divide by 4
#define ADC_PRESCALER   4 // Divide by 8

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
				uint8_t   scheduleLen,
				uint8_t** bufferStartArray,
				uint8_t** bufferEndArray,
				uint8_t*  bufferCurrentLenArray);

void adc_trigger(void);

#endif