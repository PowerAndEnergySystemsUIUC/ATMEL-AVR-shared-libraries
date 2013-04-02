/*
 * riegel_uart_demo.c
 *
 * Created: 2/2/2012 9:04:31 PM
 *  Author: Josiah McClurg
 */ 

#ifdef FLEURY_UART_DEMO
#include "fleury_uart.h"
#include <avr/interrupt.h>
#define UART_BAUD_RATE 19200
#define F_CPU 8000000

#define UART_DEMO_STR_BUFFER_LEN 10
#define UART_DEMO_STR_BUFFER_LEN_STR "10"
int main(void)
{
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
	uart_config_default_stdio();
	
	sei();
	char c;
	int i;
	char str[UART_DEMO_STR_BUFFER_LEN];
	uart_putc('@');
	uart_puts("\n\nHello Raw UART! Test getc: ");
	do 
	{
		i = uart_getc();
	} while (i == UART_NO_DATA || i == UART_BUFFER_OVERFLOW || i == UART_FRAME_ERROR);
	uart_putc(i);
	
	uart_putc('\n');
	
	printf("Hello %s! Test fgetc and fputc now: ", "printf");
	
	i = fgetc(stdin);
	fputc(i,stdout);
	fputc('\n',stdout);
	
	printf("Great. Now test scanf (%d-byte buffer): ", UART_DEMO_STR_BUFFER_LEN);
	i = scanf("%"UART_DEMO_STR_BUFFER_LEN_STR"s",str);
	printf("\nScanf returned %d. Read \"%s\"\nYou may now type whatever you want.\n",i,str);
	
	while(1)
	{
		i = fgetc(stdin);
		if(i == '\r')
			fputc('\n',stdout);
		else
			fputc(i,stdout);
	}
	return 0;
}

#endif