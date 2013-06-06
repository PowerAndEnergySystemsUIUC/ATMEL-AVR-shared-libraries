/*
 * riegel_uart_demo.c
 *
 * Created: 2/2/2012 9:04:31 PM
 *  Author: Josiah McClurg
 */ 

#ifdef FLEURY_UART_DEMO
#include "fleury_uart.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define UART_BAUD_RATE 115200L

#ifndef F_CPU
#define F_CPU 16000000L
#endif

#define UART_DEMO_STR_BUFFER_LEN 10
#define UART_DEMO_STR_BUFFER_LEN_STR "10"
#define BUFLEN 32

unsigned char txbuff[BUFLEN];
unsigned char rxbuff[BUFLEN];
RingBuffer txbuf;
RingBuffer rxbuf;

int main(void)
{
	int i;
	txbuf.buffer = txbuff;
	txbuf.size   = BUFLEN;
	txbuf.mask   = BUFLEN-1;
	txbuf.head   = 0;
	txbuf.tail   = 0;
	
	rxbuf.buffer = rxbuff;
	rxbuf.size   = BUFLEN;
	rxbuf.mask   = BUFLEN-1;
	rxbuf.head   = 0;
	rxbuf.tail   = 0;
	
	uart_init( UART_BAUD_SELECT_DOUBLE_SPEED(UART_BAUD_RATE,F_CPU), &rxbuf, &txbuf);
	
	#ifdef UART_CONFIGURE_FOR_STDIO
	uart_config_default_stdio();
	#endif
	
	sei();
	
	uart_puts_P("You won 32 dollars!\n");
	
	uart_wait_tx_empty();
	for(i = 0; i < BUFLEN; ++i){
		txbuff[i] = '$';
	}
	txbuf.head   = BUFLEN-1;
	txbuf.tail   = 0;
	uart_flush_tx();
	uart_putc('\n');

	char str[UART_DEMO_STR_BUFFER_LEN];
	uart_putc('\n');
	uart_putc('\n');
	uart_putc('H');
	uart_puts_P("ello Raw UART!\n");
	#ifdef UART_CONFIGURE_FOR_STDIO
	fputc('H',stdout);
	printf_P(PSTR("ello %s!\n"), "printf");
	#endif
	
	uart_puts_P("Type something for getc: ");
	//while ( !(UCSRA & (1<<RXC)) );
	//uart_putc(UDR);
	do 
	{
		i = uart_getc();
	} while (i == UART_NO_DATA);
	uart_putc(i);
	
	uart_putc('\n');
	
	#ifdef UART_CONFIGURE_FOR_STDIO
	printf_P(PSTR("Now for fgetc: "));
	i = fgetc(stdin);
	fputc(i,stdout);
	fputc('\n',stdout);
	
	printf_P(PSTR("Great. Now test scanf (%d-byte buffer): "), UART_DEMO_STR_BUFFER_LEN);
	i = scanf("%"UART_DEMO_STR_BUFFER_LEN_STR"s",str);
	printf_P(PSTR("\nScanf returned %d. Read \"%s\"\nYou may now type whatever you want.\n"),i,str);
	#endif
	
	while(1)
	{
		do
		{
			i = uart_getc();
		} while (i == UART_NO_DATA);
		uart_putc(i);
	}
	
	return 0;
}

#endif