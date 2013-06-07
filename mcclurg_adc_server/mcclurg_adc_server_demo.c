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

// Both must be a power of 2
#define SCHEDULE_LEN	4
#define BUFFER_LEN		(SCHEDULE_LEN*2)

// Mask indicating which ADCs to use.
#define ADC_MASK		0b00001111

// Allocate the space necessary for the ADC buffer.
uint8_t bufferr[BUFFER_LEN];
uint8_t schedule[SCHEDULE_LEN];
RingBuffer buffer;

int main(void)
{
	// Enable the pullup on RESET pin.
	#if defined(__AVR_AT90PWM316__)
	PORTE = (1<<PINE0);
	#elif defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
	PORTC = (1<<PINC6);
	#else
	#error Target not supported.
	#endif
	
	// populate the buffer with known values
	for(int i = 0; i < BUFFER_LEN; i++){
		bufferr[i] = 3;
	}
	buffer.buffer = bufferr;
	buffer.size = BUFFER_LEN;
	buffer.mask = BUFFER_LEN-1;
	buffer.head = 0;
	buffer.tail = 0;
	
	// Populate the ADC read schedule
	schedule[0] = 0;
	schedule[1] = 1;
	schedule[2] = 2;
	schedule[3] = 3;
	adc_encode_muxSchedule(schedule, SCHEDULE_LEN, ADC_MASK);
	
	// Setup all of port C as inputs, except for pin C5
	DDRC = (1<<PINC5);
	PORTC &= ~(1<<PINC5);
	
	DDRD = (1<<PIND5) | (1<<PIND6);
	PORTD = (1<<PIND6);
	
	adc_init(ADC_MODE_AUTO, &buffer, schedule, SCHEDULE_LEN);
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