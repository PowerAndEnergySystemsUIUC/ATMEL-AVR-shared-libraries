#include "adc.h"
#include <util/delay.h>

uint8_t _adc_mode;

RingBuffer* _adc_buffer;

// Sets the sampling schedule for the different ADC channels.
uint8_t* _adc_mux_schedule;
uint8_t  _adc_schedule_mask;
volatile uint8_t _adc_busy;


/*uint8_t _findbit(uint8_t needle, uint8_t haystack){
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
}*/

// This guy handles storing the ADC result when it is done
ISR (ADC_vect)
{
	unsigned char tmphead;
	
	#ifdef DEBUG
	ADC_OUT_ON();
	#endif

    // Equivalent to head = (head+1) mod adc_buffer_len
	tmphead = ( _adc_buffer->head + 1) & _adc_buffer->mask;

	if(tmphead == _adc_buffer->tail){
		// Overwrite the old contents of the buffer.
		_adc_buffer->tail = (tmphead + 1) & _adc_buffer->mask;
	}
	
	// This is the result from the current scheduled MUX
	#ifdef DUMMY_ADC_READINGS
	uint8_t tmp = ((_adc_mux_schedule[(tmphead + _adc_schedule_mask - 1) & _adc_schedule_mask] & ~((1 << ADLAR) | (ADC_REFERENCE << REFS0))) >> MUX0);
	if(tmp < 10){
		_adc_buffer->buffer[tmphead] = '0'+tmp;
	}
	else{
		_adc_buffer->buffer[tmphead] = ('A'-10)+tmp;
	}
	//_adc_buffer->buffer[tmphead] = tmp;
	#else
	_adc_buffer->buffer[tmphead] = adc_read();
	#endif
	_adc_buffer->head = tmphead;
	
	// The MUX schedule 
	tmphead = tmphead & _adc_schedule_mask;

    if(_adc_mode == ADC_MODE_MANUAL){
		_adc_busy = 1;
		// Stop auto-converting on the next-to-last element of the
		// schedule, since the last element of the schedule is
		// already running.
		if(tmphead == _adc_schedule_mask){
			ADCSRA &= ~(1<<ADATE);
		}
		// Re-enable auto-converting on the last element of the schedule,
		// so that the next ADC trigger will trigger the schedule to
		// run again.
		else if(tmphead == 0){
			ADCSRA |= (1<<ADATE);
			_adc_busy = 0;
		}
	}
	ADMUX = _adc_mux_schedule[tmphead];
	
	#ifdef DEBUG
	ADC_OUT_OFF();
	#endif
}

void adc_trigger(void){
	_adc_busy = 1;
	// Set the MUX for the first conversion.
	// We have rotated the schedule to the left by 1, so this is the first element of the schedule.
	// remember, that adc_schedule_mask is nothing more than adc_schedule_len - 1
	ADMUX  = _adc_mux_schedule[(_adc_buffer->head + _adc_schedule_mask) & _adc_schedule_mask];
	
	// Start the conversion
	ADCSRA = (1 << ADATE) | (1<<ADEN) | (1<<ADIE) | (ADC_PRESCALER << ADPS0) | (1<<ADSC) | (1<<ADIF);

	// delay loop 2 has four CPU cycles per iteration, so wait four CPU cycles past the first ADC cycle.
	// we must do this to ensure that the MUX really gets set.
	_delay_loop_2(((1<<ADC_PRESCALER)/4) + 1);
	
	// Set the MUX for the next conversion.
	// We have rotated the schedule to the left by 1, so this is the second element of the schedule.
	ADMUX  = _adc_mux_schedule[(_adc_buffer->head) & _adc_schedule_mask];
}

void adc_disable(void){
	_adc_busy = 0;
	ADCSRA = (1<<ADIF);
}

uint8_t adc_is_busy(void){
	return _adc_busy;
}

void adc_enable(){
	#ifdef ADHSM
	ADCSRB = (1<<ADHSM) | ((_adc_mode & 0xF) << ADTS0);
	#else
	ADCSRB = (_adc_mode & 0xF) << ADTS0;
	#endif
		
	// Enable the ADC. We use the auto conversion mode to make the
	// muxed measurements as close as possible together.
	ADCSRA = (1 << ADATE) | (1<<ADEN) | (1<<ADIE) | (ADC_PRESCALER << ADPS0) | (1<<ADIF);
}

void adc_encode_muxSchedule(uint8_t* scheduleIndices, uint8_t scheduleLen){
	uint8_t scheduleMask = scheduleLen-1;
	uint8_t i;
	uint8_t j;
	uint8_t tmp;
	/*if(scheduleLen & scheduleMask){
		// must be a power of two
		return;
	}*/
	
	// Pre-populate the left-adjust bit into the mux schedule, so you don't have to do it in the ISR
	for(i = 0; i < scheduleLen; ++i){
		scheduleIndices[i] = (1 << ADLAR) | (ADC_REFERENCE << REFS0) | (scheduleIndices[i] << MUX0);
	}
	
	// Shift the mux schedule left by 1. This is done so that the ISR code can be as fast as possible.
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

void adc_reset_schedule(void){
	_adc_buffer->head = 0;
	_adc_buffer->tail = 0;
}

uint16_t adc_single_conversion(uint8_t muxVal){
	uint16_t ret;
	ADCSRA = 0;
	ADMUX = (1<<ADLAR) | (ADC_REFERENCE << REFS0) | ((muxVal & 0xF) << MUX0);
	ADCSRA = (1<<ADEN) | (ADC_PRESCALER << ADPS0) | (1<<ADSC) | (1<<ADIF);
	
	while(ADCSRA & (1<<ADSC)){
		// Wait until the ADC is populated.
	}
	ret = ADC;
	
	ADCSRA = (1 << ADATE) | (1<<ADEN) | (1<<ADIE) | (ADC_PRESCALER << ADPS0) | (1<<ADIF);
	
	return ret;
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
	#ifdef DEBUG
	ADC_OUT_DDR |= 1<<ADC_OUT_PIN;
	#endif
	
	uint16_t mask = 0;
	uint8_t i;
	adc_disable();
	
	// Store the memory locations.
	_adc_mode = mode;
	_adc_buffer = buffer;
	_adc_mux_schedule = muxSchedule;
	_adc_schedule_mask = scheduleLen-1;
	
	/*if ( adc_buffer->size & adc_buffer->mask || scheduleLen & adc_schedule_mask){
		// RX buffer size is not a power of 2
		return;
	}*/
	
	for (i = 0; i < scheduleLen; ++i){
		mask |= 1 << ((muxSchedule[i] & ~((1 << ADLAR) | (ADC_REFERENCE << REFS0))) >> MUX0);
	}
	
	//DIDR0 = mask & 0x1F;  // Because ADC6 and ADC7 do not have digital input buffers, you don't have to disable them.
	DIDR0 |= mask & 0xFF;
	DIDR1 |= (mask >> 8) & 0xFF;
	adc_enable();
}