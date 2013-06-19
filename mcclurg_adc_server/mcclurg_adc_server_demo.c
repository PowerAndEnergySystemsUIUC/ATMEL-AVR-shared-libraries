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

#define D5ON()	PORTD |= (1 << PIND5)
#define D6ON()	PORTD |= (1 << PIND6)
#define D5OFF()	PORTD &= ~(1 << PIND5)
#define D6OFF()	PORTD &= ~(1 << PIND6)

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

// Prescale by 8.
#define start_timer0()	TCNT0 = 250; TCCR0B = (3<<CS00)
#define stop_timer0()	TCCR0B = 0

// Allocate the space necessary for the ADC buffer.
uint8_t txbuffer_storage[BUFFER_LEN];

uint8_t rxbuffer_storage[RXBUFFER_LEN];
uint8_t adc_mux_schedule[SCHEDULE_LEN];

RingBuffer rxbuffer;
RingBuffer txbuffer;

#ifdef TIMER0_COMP_A_vect
ISR(TIMER0_COMP_A_vect)
#else
ISR(TIMER0_COMPA_vect)
#endif
{
	D5ON();
	sei();
	adc_trigger();
	uart_flush_tx();
	uart_wait_tx_empty();
	D5OFF();
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
	
	// Enable the pullup on RESET pin.
	#if defined(__AVR_AT90PWM316__)
	PORTE = (1<<PINE0);
	#elif defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
	PORTC = (1<<PINC6);
	#else
	#warning "Didn't enable reset pin pullup"
	#endif
	
	// Setup all of port C as inputs, except for pin C5
	DDRC = (1<<PINC5);
	PORTC &= ~(1<<PINC5);
	
	DDRD |= (1<<PIND5) | (1<<PIND6);
	
	// populate the buffer with known values
	for(int i = 0; i < BUFFER_LEN; i++){
		txbuffer_storage[i] = '0';
	}
	ringBuffer_initialize(&txbuffer,txbuffer_storage,BUFFER_LEN);
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
	
	
	adc_init(ADC_MODE_MANUAL, &txbuffer, adc_mux_schedule, SCHEDULE_LEN);
	timer0_init();
	sei();
	uart_puts_P("\f\r\n\r\nADC and UART Demo. Commands:\nb      Begin 4 channel read bursts at CLK/16K\ne      End reading.\n");
	uart_wait_tx_empty();
	
	D6OFF();
	while(1)
	{
		do
		{
			i = uart_getc();
		} while (i == UART_NO_DATA);
		D6ON();
		if(i == 'b'){
			uart_flush_tx();
			uart_wait_tx_empty();
			uart_puts_P("Begin.\n");
			uart_wait_tx_empty();
			D6OFF();
			start_timer0();
		}
		else if(i == 'e'){
			stop_timer0();
			D6OFF();
			_delay_us(100);
			uart_flush_tx();
			uart_wait_tx_empty();
			uart_puts_P("End.\n");
			uart_wait_tx_empty();
		}
		else{
			uart_flush_tx();
			uart_wait_tx_empty();
			uart_puts_P("Invalid command: ");
			uart_wait_tx_empty();
			uart_putc(i);
			uart_putc('\n');
			D6OFF();
		}
	}
	
	return 0;
}

#endif