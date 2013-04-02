/*
 * blinky.c
 *
 * Created: 10/20/2011 10:32:14 AM
 *  Author: Josiah
 */ 

#define F_CPU 8000000L
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

// Compile with the following:
// -DUART_AUTOMATIC_CARRIAGE_RETURN=1
// -DUART_CONFIGURE_FOR_STDIO=1
#define UART_BAUD_RATE 9600L
#include "../fleury_uart/fleury_uart.h"
#include "lib/lib-rfm22b/rfm22b.h"
#include "txtest.h"

#define LED1_INIT() DDRD |= (1<<7)
#define LED1_ON() PORTD &= ~(1<<7)
#define LED1_OFF() PORTD |= (1<<7)

#define LED2_INIT() DDRB |= (1<<0)
#define LED2_ON() PORTB &= ~(1<<0)
#define LED2_OFF() PORTB |= (1<<0)


int main(void)
{
	char temp;

	LED1_INIT();
	LED2_INIT();
	uart_config_default_stdio();
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
	sei();
	LED1_ON();
	LED2_ON();
	printf("\n\n********RFM22 Communication Test********\n");	
	LED1_ON();
	LED2_OFF();
	
	rfm22b_init();
	txtest_configure_radio();

	//====================//
	//Communications Test
	
	temp = rfm22b_read(DTYPE);
	temp = rfm22b_read(DVERS);
	temp = rfm22b_read(INTEN1);
	temp = rfm22b_read(INTEN2);
	temp = rfm22b_read(OMFC1);
	temp = rfm22b_read(OMFC2);
	
	LED1_OFF();
	LED2_ON();
	
	printf("*****************************************\n\n");
	txtest_send_current_packet();	// Send test packet	'0123456789:;<=>?"
	
	// This example allows you to enter a 16-byte packet to send
	printf("Entering TX Mode...Give me a 16-byte packet\n\n");
	
	LED1_OFF();
	LED2_OFF();
	txtest_fill_packet_from_uart();
	
	while(1)
	{
		txtest_send_current_packet();
		_delay_ms(1000);
	}
	return 0;
}