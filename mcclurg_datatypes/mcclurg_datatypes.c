/*
 * mcclurg_datatypes.c
 *
 * Created: 6/19/2013 11:20:50 AM
 *  Author: jcmcclu2
 */ 

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "ringBuffer.h"
#include "ascii.h"

#define UART_BAUD_RATE 115200L

#ifndef F_CPU
#define F_CPU 16000000L
#endif

#define BUFLEN 32

unsigned char txbuff[BUFLEN];
unsigned char rxbuff[BUFLEN];
unsigned char buff[8];

RingBuffer buf;
RingBuffer txbuf;
RingBuffer rxbuf;

#include <avr/io.h>
#include <uart.h>

int main(void)
{
	int i;
	ringBuffer_initialize(&txbuf,txbuff,BUFLEN);
	ringBuffer_initialize(&rxbuf,rxbuff,BUFLEN);
	ringBuffer_initialize(&buf,buff,8);

	uart_init( UART_BAUD_SELECT_DOUBLE_SPEED(UART_BAUD_RATE,F_CPU), &rxbuf, &txbuf);
	uart_config_default_stdio();
	
	sei();
	printf_P(PSTR("\f\r\n\r\nWelcome to the datatype test.\n"));
	
	if(ringBuffer_canRead(&buf)){
		printf_P(PSTR("Ringbuffer is not empty.\n"));
	}
	else{
		printf_P(PSTR("Ringbuffer is empty.\n"));
	}
	
	if(ringBuffer_canWrite(&buf)){
		printf_P(PSTR("Ringbuffer can be written to.\n"));
	}
	else{
		printf_P(PSTR("Ringbuffer cannot be written to.\n"));
	}
	
	printf_P(PSTR("Writing 0-9 to a size 8 ring buffer.\n"));
	for(i = 0; i < 10; ++i){
		ringBuffer_write(&buf,i);
	}
	
	if(ringBuffer_canRead(&buf)){
		printf_P(PSTR("Ringbuffer is not empty.\n"));
	}
	else{
		printf_P(PSTR("Ringbuffer is empty.\n"));
	}
	if(ringBuffer_canWrite(&buf)){
		printf_P(PSTR("Ringbuffer can be written to.\n"));
	}
	else{
		printf_P(PSTR("Ringbuffer cannot be written to.\n"));
	}
	
	printf_P(PSTR("Current contents of ringbuffer are:\n"));
	for(i = 0; i < 7; ++i){
		printf_P(PSTR("%d: %d\n"),i,ringBuffer_read(&buf));
	}
	
	i = 0;
    while(1)
    {
		fgetc(stdin);
		fputc(i,stdout);
		printf("%d\n",i);
        ++i;
    }
}