#ifndef _ADC_ENGINE_H_
#define _ADC_ENGINE_H_

// NOTE:
// This library assumes that you only care about 8 bit accuracy, and
// wish for maximum ADC conversion speed, and that the ADC sampling
// channels are not used for any other purpose during the sampling.
//
// If the buffer is not emptied, the library overwrites it in a round-
// robin fashion, so that it always contains the latest samples.

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdlib.h>
#include <math.h>
#include "ringBuffer.h"

#ifdef DEBUG
#ifndef ADC_OUT_PORT
#define ADC_OUT_PORT PORTB
#define ADC_OUT_PIN  PINB0
#define ADC_OUT_DDR  DDRB
#endif

#ifndef ADC_OUT_PIN
#error "you have to specify ADC_OUT_PIN"
#endif

#ifndef ADC_OUT_DDR
#error "you have to specify ADC_OUT_DDR"
#endif

#define ADC_OUT_ON()	ADC_OUT_PORT |=  (1 << ADC_OUT_PIN)
#define ADC_OUT_OFF()	ADC_OUT_PORT &= ~(1 << ADC_OUT_PIN)
#endif
#define adc_read()		ADCH

// 1. ADC takes 13.5*ADC_PRESCALER cycles to finish, with 2*ADC_PRESCALER cycles of setup time between runs
// 2. User code of the ISR is 56 cycles, not including setup and teardown time, but ADC runs during the ISR.
//
// You can go down to a prescaler of 2 (divide by 4), but the resulting measurements will not be as fast as
// calculated above, due to the fact that the ADC will auto-trigger again before the ISR is complete. Use a
// smaller prescaler at your own risk.
#define ADC_PRESCALER	3 // Divide by 8


#define ADC_MODE_AUTO	0x10 // Autoconversion mode: Loop through the schedule continuously at the maximum rate.
#define ADC_MODE_MANUAL 0x20 // Each time ADC is triggered, loop through the schedule once at the maximum rate.

void adc_enable(void);
void adc_disable(void);
uint8_t adc_is_busy(void);

void adc_encode_muxSchedule(uint8_t* scheduleIndices, uint8_t scheduleLen);

// Mode is either ADC_MODE_AUTO, ADC_MODE_MANUAL, or a mask specifying the autotrigger source selection bits.
void adc_init(	uint8_t mode,
				RingBuffer* buffer,
				uint8_t* muxSchedule,
				uint8_t	scheduleLen);

void adc_trigger(void);
void adc_reset_schedule(void);


#endif
