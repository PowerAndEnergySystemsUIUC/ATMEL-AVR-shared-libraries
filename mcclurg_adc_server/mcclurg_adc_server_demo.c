/*
 * mcclurg_adc_server.c
 *
 * Created: 6/4/2013 11:23:57 AM
 *  Author: jcmcclu2
 */ 

#ifdef DEBUG
#define F_CPU 8000000L
#include <avr/io.h>
#include <stdlib.h>
#include "adc.h"

#define NUM_ADCS      4
#define SCHEDULE_LEN  6
#define BUFFER_LEN    7
#define ADC_MASK      0xF

// Allocate the space necessary for the ADC buffers.
uint8_t  buffer[NUM_ADCS*BUFFER_LEN];
uint8_t* bufferStartArray[NUM_ADCS];
uint8_t* bufferEndArray[NUM_ADCS];
uint8_t  bufferCurrentLenArray[NUM_ADCS];
uint8_t  schedule[SCHEDULE_LEN];
uint8_t  muxSchedule[SCHEDULE_LEN];

int main(void)
{
	// populate the buffer with known values
	for(int i = 0; i < NUM_ADCS*BUFFER_LEN; i++){
		buffer[i] = 3;
	}
	// Enable the pullup on RESET pin.
	PORTC = (1<<PINC6);
	
	// Populate the ADC read schedule
	schedule[0] = 0;
	schedule[1] = 1;
	schedule[2] = 2;
	schedule[3] = 3;
	schedule[4] = 2;
	schedule[5] = 1;
	
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
				SCHEDULE_LEN,
				(uint8_t**) bufferStartArray,
				(uint8_t**) bufferEndArray,
				bufferCurrentLenArray);
	adc_trigger();
	
	// LED on.
	PORTD = (1<<PIND5);
	
	while(1){}
	PORTC &= ~(1<<PINC5);
	PORTD = 0;
    return 0;
}

#endif