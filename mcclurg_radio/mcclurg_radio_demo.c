/*
 * mcclurg_radio.c
 *
 * Created: 2/9/2012 5:20:18 AM
 *  Author: Josiah
 */ 

#ifdef MCCLURG_RADIO_DEMO
#include <avr/io.h>
#include <avr/interrupt.h>

#define UART_CONFIGURE_FOR_STDIO
#define UART_BAUD_RATE 9600
#define F_CPU 8000000L
#include "../fleury_uart/fleury_uart.h"

#include "../mcclurg_spi/spi.h"
#include "demo_txtest.h"
#include "mcclurg_radio.h"

int main(void)
{
	uart_config_default_stdio();
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
	
	spi_init();
	radio_init();
	sei();
	
	printf("\nTX Test. Type in a packet:\n");
	
	txtest_configure_radio();

	//====================//
	//Communications Test
	char temp;
	temp = rfm22b_read(DTYPE);
	temp = rfm22b_read(DVERS);
	temp = rfm22b_read(INTEN1);
	temp = rfm22b_read(INTEN2);
	temp = rfm22b_read(OMFC1);
	temp = rfm22b_read(OMFC2);

	
    while(1)
    {
		
        txtest_fill_packet_from_uart();
		txtest_send_current_packet();
		printf("Transmit done. Type another:\n\n");
    }
}
#endif