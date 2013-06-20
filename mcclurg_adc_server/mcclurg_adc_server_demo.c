/*
 * mcclurg_adc_server.c
 *
 * Created: 6/4/2013 11:23:57 AM
 *  Author: jcmcclu2
 */ 

#ifdef DEBUG
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <uart.h>
#include <ringBuffer.h>

#include "adc.h"

#define UART_BAUD_RATE 115200L

#define DEBUG_PIN0_ON()	PORTB |= (1 << PINB0)
#define DEBUG_PIN1_ON()	PORTB |= (1 << PINB1)
#define DEBUG_PIN0_OFF()	PORTB &= ~(1 << PINB0)
#define DEBUG_PIN1_OFF()	PORTB &= ~(1 << PINB0)
#define SETUP_DEBUG_PINS()	DDRB |= (1 << PINB0) | (1 << PINB1)

// Both must be a power of 2
#define SCHEDULE_LEN	4

// The tx buffer must be at least 2 times the schedule length
// because of the following sequence:
// 1. Timer ISR executed, and ADC sequence triggered.
// 2. ADC goes through schedule, buffering results
// 3. Timer ISR executed, and ADC sequence triggered. Begins emptying of buffer.
// 4. ADC goes through schedule, and must be guaranteed enough space.
#define BUFFER_LEN		(SCHEDULE_LEN*2)
#define RXBUFFER_LEN    BUFFER_LEN
#define TXBUFFER_LEN    2*BUFFER_LEN

// Prescale by 8.
#define start_timer0()	TCNT0 = 250; TCCR0B = (3<<CS00)
#define stop_timer0()	TCCR0B = 0

// Allocate the space necessary for the ADC buffer.
uint8_t txbuffer_storage[TXBUFFER_LEN];
uint8_t rxbuffer_storage[RXBUFFER_LEN];
uint8_t adcbuffer_storage[BUFFER_LEN];

uint8_t adc_mux_schedule[SCHEDULE_LEN];

RingBuffer adcbuffer;
RingBuffer rxbuffer;
RingBuffer txbuffer;

#ifdef TIMER0_COMP_A_vect
ISR(TIMER0_COMP_A_vect)
#else
ISR(TIMER0_COMPA_vect)
#endif
{
	uint8_t i;
	DEBUG_PIN0_ON();
	sei();
	adc_trigger();
	if(ringBuffer_canRead(&adcbuffer)){
		for(i = 0; i < SCHEDULE_LEN; i++){
			uart_put_hex8(ringBuffer_read(&adcbuffer));
			//uart_putc(' ');
		}
		uart_putc('\n');
	}
	DEBUG_PIN0_OFF();
}

void timer0_init(void){
	// Set timer to reset on compare match
	TCCR0A = (2<<WGM00);
	
	// Start counter at 0
	TCNT0 = 0;
	OCR0A = 250;
	
	TIMSK0 = (1<<OCIE0A);
}

int main(void)
{
	int i;
	int count = 0;
	
	DDRB = 0;
	DDRC = 0;
	DDRE = 0;
	
	// Enable the pullup on RESET pin.
	#if defined(__AVR_AT90PWM316__)
	PORTE = (1<<PINE0);
	PORTE &= ~(1<<PINE0);
	#elif defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
	PORTC = (1<<PINC6);
	PORTC &= ~(1<<PINC6);
	#else
	#warning "Didn't enable reset pin pullup"
	#endif
	
	// Setup all of port C as inputs, except for pin C5
	SETUP_DEBUG_PINS();
	
	// Set up the ADC single conversion pin.
	DDRC  &= ~(1<<PINC6);
	PORTC &= ~(1<<PINC6);
	
	// Disable all the ADC digital inputs.
	DIDR1 = 0xFF;
	DIDR0 = 0xFF & ~(1<<ADC1D);
	
	// populate the buffer with known values
	ringBuffer_initialize(&txbuffer,txbuffer_storage,TXBUFFER_LEN);
	ringBuffer_initialize(&adcbuffer,adcbuffer_storage,BUFFER_LEN);
	ringBuffer_initialize(&rxbuffer,rxbuffer_storage,RXBUFFER_LEN);
	
	// Populate the ADC read schedule with the mux indices that you want.
	adc_mux_schedule[0] = 2;
	adc_mux_schedule[1] = 10;
	adc_mux_schedule[2] = 6;
	adc_mux_schedule[3] = 3;
	
	
	uart_init( UART_BAUD_SELECT_DOUBLE_SPEED(UART_BAUD_RATE,F_CPU), &rxbuffer, &txbuffer);
	//uart_config_default_stdio();
	// Translate those indices and the mask specifying which MUXes to turn on,
	// to an encoded mux schedule that the ADC understands.
	adc_encode_muxSchedule(adc_mux_schedule, SCHEDULE_LEN);
	
	
	adc_init(ADC_MODE_MANUAL, &adcbuffer, adc_mux_schedule, SCHEDULE_LEN);
	timer0_init();
	sei();
	uart_puts_P("\f\r\n\r\nADC and UART Demo. Commands:\n"
				"b      Begin 4 channel read bursts at CLK/16K\n"
				"e      End reading.\n"
				"s      Single conversion.\n");
	
	DEBUG_PIN1_OFF();
	while(1)
	{
		do
		{
			i = uart_getc();
		} while (i == UART_NO_DATA);
		DEBUG_PIN1_ON();
		if(i == 'b'){
			uart_puts_P("Begin.\n");
			start_timer0();
		}
		else if(i == 'e'){
			stop_timer0();
			_delay_us(100);
			uart_puts_P("End.\n");
		}
		else if(i == 's'){
			uart_puts_P("ADC");
			uart_put_hex8(count);
			uart_puts_P(": ");
			i = adc_single_conversion(count);
			uart_put_hex8((i >> 8) & 0xFF);
			uart_put_hex8(i & 0xFF);
			uart_putc('\n');
			count = (count + 1) & 0xF;
			if(count == 11)
			{
				count = 13;
			}
		}
		else{
			uart_puts_P("Invalid command: ");
			uart_putc(i);
			uart_putc('\n');
		}
		DEBUG_PIN1_OFF();
	}
	
	return 0;
}

#endif