#include "adc.h"
#include <util/delay.h>

uint8_t adc_mode;
uint8_t adc_mask;

// Tells where the buffer memory location starts and ends.
uint8_t*  adc_buffer;				// uint8_t adc_buffer[adc_buffer_num][adc_buffer_size]
uint8_t   adc_buffer_num;			// number from 1 to 9
uint8_t   adc_buffer_size;			// length of adc_buffer

// Sets the sampling schedule for the different ADC channels.
uint8_t*  adc_schedule;				// uint8_t adc_schedule[adc_schedule_len] only contains elements < adc_buffer_num
uint8_t*  adc_mux_schedule;
uint8_t   adc_schedule_len;         // length of adc_schedule
uint8_t   adc_schedule_index;		// current element of adc_schedule

// Circular buffer
uint8_t** adc_buffer_start;			// uint8_t* adc_buffer_start[adc_buffer_num];
uint8_t** adc_buffer_end;			// uint8_t* adc_buffer_end[adc_buffer_num];
uint8_t*  adc_buffer_current_len;	// uint8_t  adc_buffer_current_len[adc_buffer_num];

uint8_t _findbit(uint8_t needle, uint8_t haystack){
	uint8_t pos = 0;
	uint8_t counter = 0;
	while(pos < 8 && counter < needle){
		if(haystack & (1<<pos)){
			counter++;
			if(counter != needle){
				pos++;
			}
		}
		else{
			pos++;
		}
	}
	return pos;
}

// This guy handles storing the ADC result when it is done
ISR (ADC_vect)
{
	toggleC5();
   // The conversion for the next scheduled MUX has already started
   
   // This is the result from the current scheduled MUX
   //*(adc_buffer_end[adc_schedule_index]) = adc_read();
   *(adc_buffer_end[adc_schedule_index]) = 1;
   
   // If this is the end of the buffer, wrap around. Otherwise, increment.
   if(adc_buffer_end[adc_schedule_index] == adc_buffer + adc_buffer_size*(adc_schedule_index + 1) - 1){
	   adc_buffer_end[adc_schedule_index] = adc_buffer + adc_buffer_size*adc_schedule_index;
   }
   else{
	   adc_buffer_end[adc_schedule_index]++;
   }
   adc_buffer_current_len[adc_schedule_index]++;
   
   // Loop through the ADC schedule
   adc_schedule_index++;
   if(adc_schedule_index == adc_schedule_len){
		adc_schedule_index = 0;
   }
   
   // Since the next conversion has already started, we have to disable the auto-conversion
   // ahead of time.
   else if((adc_mode != ADC_MODE_AUTO) && (adc_schedule_index == adc_schedule_len - 1)){
		ADCSRA &= ~(1<<ADATE);
   }
   
   // Because the next scheduled MUX has already started, we must set the MUX to the next one after that.
   if(adc_schedule_index + 1 == adc_schedule_len){
	   ADMUX = (1 << ADLAR) | adc_mux_schedule[0] << MUX0;
   }
   else{
	   ADMUX = (1 << ADLAR) | adc_mux_schedule[adc_schedule_index + 1] << MUX0;
   }
   toggleC5();
}

void adc_trigger(void){
	adc_disable();
	adc_schedule_index = 0;
	// Set the MUX for the first conversion.
	ADMUX  = (1 << ADLAR) | (adc_mux_schedule[0] << MUX0);
	
	cli();
	// Start the conversion
	ADCSRA = (1 << ADATE) | (1<<ADEN) | (1<<ADIE) | (ADC_PRESCALER << ADPS0) | (1<<ADSC);
	
	// delay loop 2 has four CPU cycles per iteration, so wait four CPU cycles past the first ADC cycle.
	// we must do this to ensure that the MUX really gets set.
	_delay_loop_2(((1<<ADC_PRESCALER)/4) + 1);
	
	// Set the MUX for the next conversion.
	ADMUX  = (1 << ADLAR) | (adc_mux_schedule[1] << MUX0);
	sei();
}

void adc_enable(){
	ADCSRB = 0;
	
	if(adc_mode == ADC_MODE_TIMER0){
		ADCSRB = (3 << ADTS0);
	}
	else if(adc_mode == ADC_MODE_TIMER1){
		ADCSRB = (5 << ADTS0);
	}
	// else if(adc_mode == ADC_MODE_MANUAL){}
	// else if(adc_mode == ADC_MODE_AUTO){}
		
	// Enable the ADC. We use the auto conversion mode to make the
	// muxed measurements as close as possible together.
	ADCSRA = (1 << ADATE) | (1<<ADEN) | (1<<ADIE) | (ADC_PRESCALER << ADPS0);
}

void adc_init(	uint8_t mode,
				uint8_t*  buffer,
				uint8_t   bufferLen,
				uint8_t   mask,
				uint8_t*  schedule,
				uint8_t*  muxSchedule,
				uint8_t   scheduleLen,
				uint8_t** bufferStartArray,
				uint8_t** bufferEndArray,
				uint8_t*  bufferCurrentLenArray)
{
	adc_disable();
	
	adc_mask = mask;
	for(uint8_t i = 0; i < 8; i++){
		if(adc_mask & (1 << i)){
			adc_buffer_num++;
		}
	}
	
	adc_schedule = schedule;
	adc_mux_schedule = muxSchedule;
	adc_schedule_len = scheduleLen;
	adc_schedule_index = 0;
	for(uint8_t i = 0; i < adc_schedule_len; i++){
		adc_mux_schedule[i] = _findbit(adc_schedule[i],adc_mask);
	}
	
	// Store the memory locations.
	adc_mode = mode;
	adc_buffer = buffer;
	adc_buffer_size = bufferLen;

	adc_buffer_start = bufferStartArray;
	adc_buffer_end = bufferEndArray;
	adc_buffer_current_len = bufferCurrentLenArray;
	for(uint8_t i = 0; i < adc_buffer_num; i++){
		adc_buffer_start[i] = adc_buffer + adc_buffer_size*i;
		adc_buffer_end[i] = adc_buffer + adc_buffer_size*i;
		adc_buffer_current_len[i] = 0;
	}
	
	DIDR0 = adc_mask & 0x1F;  // Because ADC6 and ADC7 do not have digital input buffers, you don't have to disable them.
	
	adc_enable();
}