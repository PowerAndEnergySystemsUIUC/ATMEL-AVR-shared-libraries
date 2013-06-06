#include "adc.h"
#include <util/delay.h>

uint8_t adc_mode;
uint8_t adc_mask;

// Tells where the buffer memory location starts and ends.
uint8_t* adc_buffer;				// uint8_t adc_buffer[adc_buffer_num][adc_buffer_size]
uint8_t* adc_buffer_max;			// end of the adc buffer

// Sets the sampling schedule for the different ADC channels.
uint8_t* adc_schedule;				// uint8_t adc_schedule[adc_schedule_len] only contains elements < adc_buffer_num
uint8_t* adc_mux_schedule;
uint8_t  adc_schedule_len;         // length of adc_schedule
uint8_t  adc_schedule_index;		// current element of adc_schedule

// Circular buffer
uint8_t* adc_buffer_start;			// uint8_t* adc_buffer_start[adc_buffer_num];
uint8_t* adc_buffer_end;			// uint8_t* adc_buffer_end[adc_buffer_num];
uint8_t  adc_buffer_current_len;	// uint8_t  adc_buffer_current_len[adc_buffer_num];

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
   // The conversion for the next scheduled MUX has already started
   
   // This is the result from the current scheduled MUX
   *adc_buffer_end = adc_read();
   //*adc_buffer_end = adc_buffer_current_len;
   
   // If this is the end of the buffer, wrap around. Otherwise, increment.
   if(adc_buffer_end == adc_buffer_max){
	   adc_buffer_end = adc_buffer;
   }
   else{
	   ++adc_buffer_end;
   }
   ++adc_buffer_current_len;
   
   // Loop through the ADC schedule
   ++adc_schedule_index;
   if(adc_schedule_index == adc_schedule_len){
		adc_schedule_index = 0;
   }
   
   // Since the next conversion has already started, we have to disable the auto-conversion
   // ahead of time.
   if(adc_schedule_index == adc_schedule_len - 1){
	   if(adc_mode != ADC_MODE_AUTO){
		   ADCSRA &= ~(1<<ADATE);
	   }
	   ADMUX = adc_mux_schedule[0];
	   #ifdef DEBUG
	   C5ON();
	   #endif
   }
   else{
	   ADMUX = adc_mux_schedule[adc_schedule_index + 1];
	   #ifdef DEBUG
	   C5OFF();
	   #endif
   }
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
	ADMUX  = adc_mux_schedule[1];
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

void adc_init(	uint8_t  mode,
				uint8_t* buffer,
				uint8_t  bufferLen,
				uint8_t  mask,
				uint8_t* schedule,
				uint8_t* muxSchedule,
				uint8_t  scheduleLen)
{
	adc_disable();
	
	adc_mask = mask;
	adc_schedule = schedule;
	adc_mux_schedule = muxSchedule;
	adc_schedule_len = scheduleLen;
	adc_schedule_index = 0;
	for(uint8_t i = 0; i < adc_schedule_len; i++){
		adc_mux_schedule[i] = (1 << ADLAR) | (_findbit(adc_schedule[i],adc_mask) << MUX0);
	}
	
	// Store the memory locations.
	adc_mode = mode;
	adc_buffer = buffer;
	adc_buffer_max = buffer + bufferLen - 1;

	adc_buffer_start = buffer;
	adc_buffer_end = buffer;
	adc_buffer_current_len = 0;
	
	DIDR0 = adc_mask & 0x1F;  // Because ADC6 and ADC7 do not have digital input buffers, you don't have to disable them.
	
	adc_enable();
}