#include "adc.h"
#include "../data_acquisition.h"
VoltageRecord* adc_buffer;

unsigned int adc_buffer_row_num;
unsigned char adc_buffer_col_num;
unsigned int adc_temp_counter;
//unsigned int counter = 0;
//unsigned int counter2 = 0;
unsigned int temp;

// This should never happen.
ISR (TIMER3_OVF_vect)
{
	adc_buffer->samplePeriod++;
}

// This guy handles timing the ADC conversions
ISR (TIMER1_OVF_vect)
{
   adc_startADCTimer();
}

// This guy handles storing the ADC result when it is done
ISR (ADC_vect)
{
   unsigned int a = adc_readADC();
   unsigned int b;
   if(adc_buffer_col_num == 0 && adc_buffer_row_num == 0)
   {
	   period_startTimer();
	   b = 0;
   }
   else
   {
	   b = period_readTimer();
   }
   
   // Discard the first sample because it took too long and
   // will mess up the timing calculations.
   if(adc_buffer_col_num == 0xFF)
	   adc_buffer_col_num = 0;
   else
   {
	   //period_stopTimer();
	   //adc_buffer->samplePeriod += period_readTimer();
	   //period_startTimer();
	   //temp = adc_readTempTimer();
	   //if((adc_buffer_col_num == 0) && (adc_buffer_row_num == 0))
	   //{
	   //   adc_startTempTimer();
	   //}		  
		  
	   data_access_pack_voltageRow(
		  adc_buffer_col_num,
		  a,
		  adc_buffer->voltageSamples[adc_buffer_row_num]);
   
	   //counter2++;
	   adc_buffer_col_num++;
	   if(adc_buffer_col_num == VOLTAGE_SAMPLES_PER_RECORD_ROW)
	   {
		   adc_buffer_col_num = 0;
		   adc_buffer_row_num++;
	   
		   // If we have finished, stop the conversion process.
		   if(adc_buffer_row_num == VOLTAGE_SAMPLES_PER_RECORD_COL)
		   {
			   adc_stopADCTimer();
			   period_stopTimer();
			   adc_buffer->samplePeriod *= 0xFFFF;
			   adc_buffer->samplePeriod += b;
			   sei();
		   }
	   }
   }
}

/*!
 * Starts the conversion of a voltage sample, and waits until the
 * conversion is done.  This function is not thread safe.
 *
 * \param[in] buffer The buffer to put the voltage sample in.
 */
void data_acquisition_get_voltage_sample(VoltageRecord* buffer)
{
	while(adc_isRunning());
	
	data_acquisition_get_time(&(buffer->timeRecorded));
	
	// This signals the interrupt that it is the first conversion.
	// Since the first conversion takes longer than the others, we
	// discard it.
	adc_buffer_col_num = 0xFF;
	adc_buffer_row_num = 0;
	adc_buffer = buffer;
	adc_buffer->samplePeriod = 0;
	//counter = 0;
	//counter2 = 0;
	adc_triggerADCConversion();
	
	while(adc_isRunning());
}

void adc_init(void)
{
   // #################
   // # Set up timers #
   // #################

   // Set timers to normal mode, ensure that timers are stopped, and disable output compare
   TCCR1A = 0;
   TCCR1B = 0;
   TCCR1C = 0;

   // Disable asynchronous mode
   ASSR = 0;

   // Clear all interrupt flags.
   TIFR1  = 0xFF;

   // Enable timer interrupts
   TIMSK1 = 0x1;
   
   
   TCCR3A = 0;
   TCCR3B = 0;
   TCCR3C = 0;
   TIMSK3 = 0x1;
   TIFR3  = 0xFF;
   
   
   // ##############
   // # Set up ADC #
   // ##############

   // Disable digital input on ADC3
   DIDR0 = (1<<3);

   // Set the reference voltage to ARef and the input voltage
   // to ADC3, no left adjust
   ADMUX = 0x03;
   
   // Enable high speed mode, set trigger source to timer 1 overflow
   ADCSRB = (1 << 7) | (0x6);

   // Turn on the ADC, enable auto-triggering, enable ADC
   // interrupts, and set prescaler to 8
   ADCSRA = (1 << ADEN) | (1<<ADATE) | (1<<ADIF) | (1 << ADIE) | (0x4);
}