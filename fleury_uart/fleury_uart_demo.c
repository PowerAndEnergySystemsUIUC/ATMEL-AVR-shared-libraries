/*
 * riegel_uart_demo.c
 *
 * Created: 2/2/2012 9:04:31 PM
 *  Author: Josiah McClurg
 */ 

#ifdef FLEURY_UART_DEMO
#include "fleury_uart.h"
#include <avr/interrupt.h>
#define UART_BAUD_RATE 9600

#ifndef F_CPU
#define F_CPU 8000000
#endif

#define UART_DEMO_STR_BUFFER_LEN 10
#define UART_DEMO_STR_BUFFER_LEN_STR "10"
int main(void)
{
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
	#ifdef UART_CONFIGURE_FOR_STDIO
	uart_config_default_stdio();
	#endif
	
	sei();
	
	char c;
	int i;
	char str[UART_DEMO_STR_BUFFER_LEN];
	uart_putc('\n');
	uart_putc('\n');
	uart_putc('H');
	uart_puts("ello Raw UART!\n");
	#ifdef UART_CONFIGURE_FOR_STDIO
	fputc('H',stdout);
	printf("ello %s!\n", "printf");
	#endif
	
	uart_puts("Type something for getc: ");
	while ( !(UCSRA & (1<<RXC)) );
	uart_putc(UDR);
	do 
	{
		i = uart_getc();
	} while (i == UART_NO_DATA);
	uart_putc(i);
	
	uart_putc('\n');
	
	#ifdef UART_CONFIGURE_FOR_STDIO
	printf("Now for fgetc: ");
	i = fgetc(stdin);
	fputc(i,stdout);
	fputc('\n',stdout);
	
	printf("Great. Now test scanf (%d-byte buffer): ", UART_DEMO_STR_BUFFER_LEN);
	i = scanf("%"UART_DEMO_STR_BUFFER_LEN_STR"s",str);
	printf("\nScanf returned %d. Read \"%s\"\nYou may now type whatever you want.\n",i,str);
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