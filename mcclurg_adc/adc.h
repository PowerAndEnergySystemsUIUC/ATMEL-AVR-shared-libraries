#ifndef _ADC_ENGINE_H_
#define _ADC_ENGINE_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <math.h>
#include "../../data_access/data_types.h"

//! The number of cycles between each ADC sample = 16 MHz / (7.2 kHz)
#define ADC_TIMER_SEGMENT 0x08AE

//! The number of cycles required by the call to the interrupt routine itself
#define ADC_CORRECTION_FACTOR 0x3B

#define ADC_STATE_STOPPED 0
#define ADC_STATE_RUNNING 1

#define adc_isRunning()     (TCCR1B != 0)

#define adc_startADCTimer() TCCR1B = 0; \
							TCNT1 = (0xFFFF - (ADC_TIMER_SEGMENT - ADC_CORRECTION_FACTOR)); \
							TCCR1B = 0x1

#define adc_triggerADCConversion() TCCR1B = 0; \
                                   TCNT1 = 0xFFFF; \
                                   TCCR1B = 0x1;

#define period_startTimer()  TCCR3B = 0; \
                             TCNT3  = 0; \
                             TCCR3B = 0x1

#define period_stopTimer() TCCR3B = 0
#define period_readTimer() TCNT3

#define adc_stopADCTimer() TCCR1B = 0
#define adc_readADC() ADC

void adc_init(void);
void adc_startSampling(VoltageRecord* buffer);

#endif