#include "adc.h"
#include <util/delay.h>

uint8_t adc_mode;

RingBuffer* adc_buffer;

// Sets the sampling schedule for the different ADC channels.
uint8_t* adc_mux_schedule;
uint8_t  adc_schedule_len;
uint8_t  adc_schedule_mask;

uint8_t _findbit(uint8_t needle, uint8_t haystack){
	 ++needle;
	uint8_t pos = 0;
	uint8_t counter = 0;
	while(pos < 8 && counter < needle){
		if(haystack & (1<<pos)){
			++counter;
			if(counter != needle){
				++pos;
			}
		}
		else{
			++pos;
		}
	}
	return pos;
}

// This guy handles storing the ADC result when it is done
ISR (ADC_vect)
{
	unsigned char tmphead;
	
	#ifdef DEBUG
	C5ON();
	#endif

	tmphead = ( adc_buffer->head + 1) & adc_buffer->mask;
	
	if(tmphead != adc_buffer->tail){
		// This is the result from the current scheduled MUX
		adc_buffer->buffer[tmphead] = adc_read();
		adc_buffer->head = tmphead;
	
		// The MUX schedule 
		tmphead = tmphead & adc_schedule_mask;

		if(!tmphead && adc_mode != ADC_MODE_AUTO){
			ADCSRA &= ~(1<<ADATE);
		}
		ADMUX = adc_mux_schedule[tmphead];
	}
	
	#ifdef DEBUG
	C5OFF();
	#endif
}

void adc_trigger(void){
	adc_disable();
	// Set the MUX for the first conversion.
	ADMUX  = (1 << ADLAR) | (adc_mux_schedule[(adc_buffer->head + adc_schedule_len -1) & adc_schedule_mask] << MUX0);
	
	cli();
	// Start the conversion
	ADCSRA = (1 << ADATE) | (1<<ADEN) | (1<<ADIE) | (ADC_PRESCALER << ADPS0) | (1<<ADSC);
	
	// delay loop 2 has four CPU cycles per iteration, so wait four CPU cycles past the first ADC cycle.
	// we must do this to ensure that the MUX really gets set.
	_delay_loop_2(((1<<ADC_PRESCALER)/4) + 1);
	
	// Set the MUX for the next conversion.
	ADMUX  = (1 << ADLAR) | (adc_mux_schedule[(adc_buffer->head) & adc_schedule_mask] << MUX0);
	sei();
}

void adc_enable(){
	#ifdef ADHSM
	ADCSRB = (1<<ADHSM);
	#else
	ADCSRB = 0;
	#endif
	
	if(adc_mode == ADC_MODE_TIMER0){
		ADCSRB |= (3 << ADTS0);
	}
	else if(adc_mode == ADC_MODE_TIMER1){
		ADCSRB |= (5 << ADTS0);
	}
	// else if(adc_mode == ADC_MODE_MANUAL){}
	// else if(adc_mode == ADC_MODE_AUTO){}
		
	// Enable the ADC. We use the auto conversion mode to make the
	// muxed measurements as close as possible together.
	ADCSRA = (1 << ADATE) | (1<<ADEN) | (1<<ADIE) | (ADC_PRESCALER << ADPS0);
}

void adc_encode_muxSchedule(uint8_t* scheduleIndices, uint8_t scheduleLen, uint8_t mask){
	uint8_t scheduleMask = scheduleLen-1;
	uint8_t i;
	uint8_t j;
	uint8_t tmp;
	if(scheduleLen & scheduleMask){
		// must be a power of two
		return;
	}
	
	// Pre-populate the left-adjust bit into the mux schedule, so you don't have to do it in the ISR
	for(i = 0; i < scheduleLen; ++i){
		scheduleIndices[i] = (1 << ADLAR) | (_findbit(scheduleIndices[i], mask) << MUX0);
	}
	
	// Shift the mux schedule left by 1.
	i=0;
	for(i=0; i < scheduleLen-1; ++i){
		// i is source index
		// j is destination index
		j = (i + scheduleLen - 1) & scheduleMask;
		
		// swap contents mux elements i and j
		tmp = scheduleIndices[j];
		scheduleIndices[j] = scheduleIndices[i];
		scheduleIndices[i] = tmp;
	}
}

/*
 The sequence of ADC measurement goes:
 1. Set MUX=0
 2. Trigger ADC
 3. Wait 1 ADC cycle
 4. Set MUX=1
 5. 0: Read ADC (MUX=0),	auto-trigger ADC
		Set MUX=2
	1: Read ADC (MUX=1),	auto-trigger ADC
		Set MUX=3
  n-1: Read ADC (MUX=n-1), auto-trigger ADC
		Set MUX=n+2

 Alternately, you can left-shift the MUX schedule first, and then replace with
 1. Set MUX-2=n-2
 2. Trigger ADC
 3. Wait 1 ADC cycle
 4. Set MUX-2=n-1
 5. 0: Read ADC (MUX-2=n-2), auto-trigger ADC
		Set MUX-2=0
	1: Read ADC (MUX-2=n-1), auto-trigger ADC
		Set MUX-2=1
	2: Read ADC (MUX-2=0),	auto-trigger ADC
		Set MUX-2=2
  n-1: Read ADC (MUX-2=n-3), auto-trigger ADC
		Set MUX-2=n
 */
void adc_init(	uint8_t mode,
				RingBuffer* buffer,
				uint8_t* muxSchedule,
				uint8_t	scheduleLen)
{
	uint8_t mask = 0;
	uint8_t i;
	adc_disable();
	
	adc_mux_schedule = muxSchedule;
	adc_schedule_len = scheduleLen;
	adc_schedule_mask = scheduleLen-1;
	
	if ( adc_buffer->size & adc_buffer->mask || adc_schedule_len & adc_schedule_mask){
		// RX buffer size is not a power of 2
		return;
	}
	
	for (i = 0; i < scheduleLen; ++i){
		mask |= 1 << ((muxSchedule[i] & ~(1 << ADLAR)) >> MUX0);
	}
	
	// Store the memory locations.
	adc_mode = mode;
	adc_buffer = buffer;
	
	DIDR0 = mask & 0x1F;  // Because ADC6 and ADC7 do not have digital input buffers, you don't have to disable them.
	adc_enable();
}