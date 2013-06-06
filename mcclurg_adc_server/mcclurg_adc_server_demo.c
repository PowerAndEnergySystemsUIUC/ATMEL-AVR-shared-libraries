/*
 * mcclurg_adc_server.c
 *
 * Created: 6/4/2013 11:23:57 AM
 *  Author: jcmcclu2
 */ 

#ifdef DEBUG
#include <avr/io.h>
#include <stdlib.h>
#include "adc.h"

#define NUM_ADCS      4
#define SCHEDULE_LEN  4
#define BUFFER_LEN    (SCHEDULE_LEN*NUM_ADCS)
#define ADC_MASK      0b00001111

// Allocate the space necessary for the ADC buffer.
uint8_t  buffer[BUFFER_LEN];
uint8_t  schedule[SCHEDULE_LEN];
uint8_t  muxSchedule[SCHEDULE_LEN];

int main(void)
{
	// Enable the pullup on RESET pin.
	#if defined(__AVR_AT90PWM316__)
	PORTE = (1<<PINE0);
	#elif defined(__AVR_ATmega328__)
	PORTC = (1<<PINC6);
	#else
	#error Target not supported.
	#endif
	
	// populate the buffer with known values
	for(int i = 0; i < BUFFER_LEN; i++){
		buffer[i] = 3;
	}
	
	// Populate the ADC read schedule
	schedule[0] = 0;
	schedule[1] = 1;
	schedule[2] = 2;
	schedule[3] = 3;
	
	// Setup all of port C as inputs, except for pin C5
	DDRC = (1<<PINC5);
	PORTC &= ~(1<<PINC5);
	
	DDRD = (1<<PIND5) | (1<<PIND6);
	PORTD = (1<<PIND6);
	
    adc_init(	ADC_MODE_AUTO,
				buffer,
				BUFFER_LEN,
				ADC_MASK,
				schedule,
				muxSchedule,
				SCHEDULE_LEN);
	sei();
	adc_trigger();
	
	// LED on.
	PORTD = (1<<PIND5);
	
	while(1){}
	PORTC |= (1<<PINC5);
	PORTD  = 0;
    return 0;
}

#endif