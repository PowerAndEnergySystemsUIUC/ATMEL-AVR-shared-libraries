/*
 * riegel_sdcard_demo.c
 *
 * Created: 2/2/2012 9:03:49 PM
 *  Author: Josiah McClurg
 */ 

#ifdef RIEGEL_SDCARD_DEMO

#include <avr/interrupt.h>

#define UART_CONFIGURE_FOR_STDIO
#define UART_BAUD_RATE 19200
#define F_CPU 8000000L
#include "../fleury_uart/fleury_uart.h"

#include "sd_raw_config.h"
#include "sd_raw.h"

int main(void)
{
	int i = sd_raw_init();
	char buffer[512];
	char texts[3][11] = {"Hello Word",
	                  "Whatup, G?",
					  "xxxxxxxxxx"};

	uart_config_default_stdio();
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
	
	sei();
    printf("\n\nSD init returned %d.\n",i);
	
	memcpy(buffer,texts[0],11);
	i = sd_write_block(0,buffer);
	printf("SD write 0 returned %d.\n",i);
	
	memcpy(buffer,texts[1],11);
	i = sd_write_block(1,buffer);
	printf("SD write 1 returned %d.\n",i);
	
	memcpy(buffer,texts[2],11);
	
	i = sd_read_block(0,buffer);
	printf("SD read 0 returned %d. Read ",i);
	printf("<%s>\n",buffer);
	
	i = sd_read_block(1,buffer);
	printf("SD read 1 returned %d. Read ",i);
	printf("<%s>\n",buffer);
	
	while(1)
	{
		
	}
	return 0;
}

#endif