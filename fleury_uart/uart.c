/*************************************************************************
Title:	 Interrupt UART library with receive/transmit circular buffers
Author:	Peter Fleury <pfleury@gmx.ch>	http://jump.to/fleury
File:	  $Id: uart.c,v 1.9 2012/11/10 12:59:31 peter Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: any AVR with built-in UART, 
License:  GNU General Public License 
			 
DESCRIPTION:
	 An interrupt is generated when the UART has finished transmitting or
	 receiving a byte. The interrupt handling routines use circular buffers
	 for buffering received and transmitted data.
	 
	 The UART_RxBuf->size and UART_TxBuf->size variables define
	 the buffer size in bytes. Note that these variables must be a 
	 power of 2.
	
	Modified June 2013 by Josiah McClurg:
		1. Provide externally-allocated transmit and receive structs. For
			maximum flexibility, user must externally set all the properties
		  of each RingBuffer. Typical values are below.
			  size:	recommended to be power of 2  (length of buffer)
			  mask:	recommended to be size-1		(mask used to allow buffer
													to wrap back to beginning: 
													index = (head + 1) & mask)
			  buffer: must be unsigned char array[size]
			  head:	recommended to be 0
			  tail:	recommended to be 0
		2. Provide uart_flush_tx() macro to allow direct external manipulation
			of the tx buffer. This involved moving the other macro definitions
		  to uart.h
	 
USAGE:
	 Refere to the header file uart.h for a description of the routines. 
	 See also example test_uart.c.

NOTES:
	 Based on Atmel Application Note AVR306
						  
LICENSE:
	 Copyright (C) 2006 Peter Fleury

	 This program is free software; you can redistribute it and/or modify
	 it under the terms of the GNU General Public License as published by
	 the Free Software Foundation; either version 2 of the License, or
	 any later version.

	 This program is distributed in the hope that it will be useful,
	 but WITHOUT ANY WARRANTY; without even the implied warranty of
	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	 GNU General Public License for more details.
								
*************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "uart.h"

/*
 *  module global variables
 */
/* size of RX/TX buffers */
static volatile RingBuffer* UART_RxBuf;
static volatile RingBuffer* UART_TxBuf;
static volatile unsigned char  UART_LastRxError;

#if defined( ATMEGA_USART1 )
static volatile RingBuffer* UART1_RxBuf;
static volatile RingBuffer* UART1_TxBuf;
static volatile unsigned char  UART1_LastRxError;
#endif

ISR (UART0_RECEIVE_INTERRUPT)	
/*************************************************************************
Function: UART Receive Complete interrupt
Purpose:  called when the UART has received a character
**************************************************************************/
{
	 unsigned char tmphead;
	 unsigned char data;
	 unsigned char usr;
	 unsigned char lastRxError;
 
 
	 /* read UART status register and UART data register */ 
	 usr  = UART0_STATUS;
	 data = UART0_DATA;
	 
	 /* */
#if defined( AT90_UART )
	 lastRxError = (usr & (_BV(FE)|_BV(DOR)) );
#elif defined( ATMEGA_USART )
	 lastRxError = (usr & (_BV(FE)|_BV(DOR)) );
#elif defined( ATMEGA_USART0 )
	 lastRxError = (usr & (_BV(FE0)|_BV(DOR0)) );
#elif defined ( ATMEGA_UART )
	 lastRxError = (usr & (_BV(FE)|_BV(DOR)) );
#endif
		  
	 /* calculate buffer index */ 
	 tmphead = ( UART_RxBuf->head + 1) & UART_RxBuf->mask;
	 
	 if ( tmphead == UART_RxBuf->tail ) {
		  /* error: receive buffer overflow */
		  lastRxError = UART_BUFFER_OVERFLOW >> 8;
	 }else{
		  /* store new index */
		  UART_RxBuf->head = tmphead;
		  /* store received data in buffer */
		  UART_RxBuf->buffer[tmphead] = data;
	 }
	 UART_LastRxError |= lastRxError;	
}


ISR (UART0_TRANSMIT_INTERRUPT)
/*************************************************************************
Function: UART Data Register Empty interrupt
Purpose:  called when the UART is ready to transmit the next byte
**************************************************************************/
{
	 unsigned char tmptail;
	 
	 if ( UART_TxBuf->head != UART_TxBuf->tail) {
		  /* calculate and store new buffer index */
		  tmptail = (UART_TxBuf->tail + 1) & UART_TxBuf->mask;
		  UART_TxBuf->tail = tmptail;
		  /* get one byte from buffer and write it to UART */
		  UART0_DATA = UART_TxBuf->buffer[tmptail];  /* start transmission */
	 }else{
		  /* tx buffer empty, disable UDRE interrupt */
		  UART0_CONTROL &= ~_BV(UART0_UDRIE);
	 }
}


/*************************************************************************
Function: uart_init()
Purpose:  initialize UART and set baudrate
Input:	 baudrate using macro UART_BAUD_SELECT()
Returns:  none
Note:	  Size of the circular receive buffer, must be power of 2
**************************************************************************/
void uart_init(unsigned int baudrate, RingBuffer* rx_buffer, RingBuffer* tx_buffer)
{
	UART_RxBuf = rx_buffer;
	UART_TxBuf = tx_buffer;
		
	if ( UART_RxBuf->size & UART_RxBuf->mask ){
		// RX buffer size is not a power of 2
		return;
	}
	if ( UART_TxBuf->size & UART_TxBuf->mask ){
		// TX buffer size is not a power of 2
		return;
	}
	 
#if defined( AT90_UART )
	 /* set baud rate */
	 UBRR = (unsigned char)baudrate; 

	 /* enable UART receiver and transmmitter and receive complete interrupt */
	 UART0_CONTROL = _BV(RXCIE)|_BV(RXEN)|_BV(TXEN);

#elif defined (ATMEGA_USART)
	 /* Set baud rate */
	 if ( baudrate & 0x8000 )
	 {
	 	 UART0_STATUS = (1<<U2X);  //Enable 2x speed 
	 	 baudrate &= ~0x8000;
	 }
	 UBRRH = (unsigned char)(baudrate>>8);
	 UBRRL = (unsigned char) baudrate;
	
	 /* Enable USART receiver and transmitter and receive complete interrupt */
	 UART0_CONTROL = _BV(RXCIE)|(1<<RXEN)|(1<<TXEN);
	 
	 /* Set frame format: asynchronous, 8data, no parity, 1stop bit */
	 #ifdef URSEL
	 UCSRC = (1<<URSEL)|(3<<UCSZ0);
	 #else
	 UCSRC = (3<<UCSZ0);
	 #endif 
	 
#elif defined (ATMEGA_USART0 )
	 /* Set baud rate */
	 if ( baudrate & 0x8000 ) 
	 {
			UART0_STATUS = (1<<U2X0);  //Enable 2x speed 
			baudrate &= ~0x8000;
		}
	 UBRR0H = (unsigned char)(baudrate>>8);
	 UBRR0L = (unsigned char) baudrate;

	 /* Enable USART receiver and transmitter and receive complete interrupt */
	 UART0_CONTROL = _BV(RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	 
	 /* Set frame format: asynchronous, 8data, no parity, 1stop bit */
	 #ifdef URSEL0
	 UCSR0C = (1<<URSEL0)|(3<<UCSZ00);
	 #else
	 UCSR0C = (3<<UCSZ00);
	 #endif 

#elif defined ( ATMEGA_UART )
	 /* set baud rate */
	 if ( baudrate & 0x8000 ) 
	 {
	 	UART0_STATUS = (1<<U2X);  //Enable 2x speed 
	 	baudrate &= ~0x8000;
	 }
	 UBRRHI = (unsigned char)(baudrate>>8);
	 UBRR	= (unsigned char) baudrate;

	 /* Enable UART receiver and transmitter and receive complete interrupt */
	 UART0_CONTROL = _BV(RXCIE)|(1<<RXEN)|(1<<TXEN);

#endif

}/* uart_init */


/*************************************************************************
Function: uart_getc()
Purpose:  return byte from ringbuffer  
Returns:  lower byte:  received byte from ringbuffer
			 higher byte: last receive error
**************************************************************************/
unsigned int uart_getc(void)
{	 
	 unsigned char tmptail;
	 unsigned char data;


	 if ( UART_RxBuf->head == UART_RxBuf->tail ) {
		  return UART_NO_DATA;	/* no data available */
	 }
	 
	 /* calculate /store buffer index */
	 tmptail = (UART_RxBuf->tail + 1) & UART_RxBuf->mask;
	 UART_RxBuf->tail = tmptail; 
	 
	 /* get data from receive buffer */
	 data = UART_RxBuf->buffer[tmptail];
	 
	 data = (UART_LastRxError << 8) + data;
	 UART_LastRxError = 0;
	 return data;

}/* uart_getc */

void uart_wait_tx_empty(void){
	while(UART_TxBuf->head != UART_TxBuf->tail){
		; // wait for the whole buffer to be emptied.
	}
}

void uart_wait_tx_free(void){
	unsigned char tmphead;

	
	tmphead  = (UART_TxBuf->head + 1) & UART_TxBuf->mask;
	
	while ( tmphead == UART_TxBuf->tail ){
		;/* wait for free space in buffer */
	}
}

/*************************************************************************
Function: uart_putc()
Purpose:  write byte to ringbuffer for transmitting via UART
Input:	 byte to be transmitted
Returns:  none			 
**************************************************************************/
void uart_putc(unsigned char data)
{
	 unsigned char tmphead;

	 
	 tmphead  = (UART_TxBuf->head + 1) & UART_TxBuf->mask;
	 
	 while ( tmphead == UART_TxBuf->tail ){
		  ;/* wait for free space in buffer */
	 }
	 
	 UART_TxBuf->buffer[tmphead] = data;
	 UART_TxBuf->head = tmphead;

	 /* enable UDRE interrupt */
	 UART0_CONTROL	 |= _BV(UART0_UDRIE);

}/* uart_putc */


/*************************************************************************
Function: uart_puts()
Purpose:  transmit string to UART
Input:	 string to be transmitted
Returns:  none			 
**************************************************************************/
void uart_puts(const char *s )
{
	 while (*s) 
		uart_putc(*s++);

}/* uart_puts */


/*************************************************************************
Function: uart_puts_p()
Purpose:  transmit string from program memory to UART
Input:	 program memory string to be transmitted
Returns:  none
**************************************************************************/
void uart_puts_p(const char *progmem_s )
{
	 register char c;
	 
	 while ( (c = pgm_read_byte(progmem_s++)) ) 
		uart_putc(c);

}/* uart_puts_p */


/*
 * these functions are only for ATmegas with two USART
 */
#if defined( ATMEGA_USART1 )

ISR(UART1_RECEIVE_INTERRUPT)
/*************************************************************************
Function: UART1 Receive Complete interrupt
Purpose:  called when the UART1 has received a character
**************************************************************************/
{
	 unsigned char tmphead;
	 unsigned char data;
	 unsigned char usr;
	 unsigned char lastRxError;
 
 
	 /* read UART status register and UART data register */ 
	 usr  = UART1_STATUS;
	 data = UART1_DATA;
	 
	 /* */
	 lastRxError = (usr & (_BV(FE1)|_BV(DOR1)) );
		  
	 /* calculate buffer index */ 
	 tmphead = ( UART1_RxBuf->head + 1) & UART_RxBuf->mask;
	 
	 if ( tmphead == UART1_RxBuf->tail ) {
		  /* error: receive buffer overflow */
		  lastRxError = UART_BUFFER_OVERFLOW >> 8;
	 }else{
		  /* store new index */
		  UART1_RxBuf->head = tmphead;
		  /* store received data in buffer */
		  UART1_RxBuf->buffer[tmphead] = data;
	 }
	 UART1_LastRxError |= lastRxError;	
}


ISR(UART1_TRANSMIT_INTERRUPT)
/*************************************************************************
Function: UART1 Data Register Empty interrupt
Purpose:  called when the UART1 is ready to transmit the next byte
**************************************************************************/
{
	 unsigned char tmptail;

	 
	 if ( UART1_TxBuf->head != UART1_TxBuf->tail) {
		  /* calculate and store new buffer index */
		  tmptail = (UART1_TxBuf->tail + 1) & UART_TxBuf->mask;
		  UART1_TxBuf->tail = tmptail;
		  /* get one byte from buffer and write it to UART */
		  UART1_DATA = UART1_TxBuf->buffer[tmptail];  /* start transmission */
	 }else{
		  /* tx buffer empty, disable UDRE interrupt */
		  UART1_CONTROL &= ~_BV(UART1_UDRIE);
	 }
}


/*************************************************************************
Function: uart1_init()
Purpose:  initialize UART1 and set baudrate
Input:	 baudrate using macro UART_BAUD_SELECT()
Returns:  none
Note:	  buffer sizes must be powers of two.
**************************************************************************/
void uart1_init(unsigned int baudrate, RingBuffer* rx_buffer, RingBuffer* tx_buffer)
{
	UART1_RxBuf = rx_buffer;
	UART1_TxBuf = tx_buffer;
	
	/* test if the size of the circular buffers fits into SRAM */
	if ( UART1_RxBuf->size & UART1_RxBuf->mask ){
		// RX buffer size is not a power of 2
		return;
	}
	if ( UART1_TxBuf->size & UART1_TxBuf->mask ){
		// TX buffer size is not a power of 2
		return;
	}
	 

	 /* Set baud rate */
	 if ( baudrate & 0x8000 ) 
	 {
	 	UART1_STATUS = (1<<U2X1);  //Enable 2x speed 
		baudrate &= ~0x8000;
	 }
	 UBRR1H = (unsigned char)(baudrate>>8);
	 UBRR1L = (unsigned char) baudrate;

	 /* Enable USART receiver and transmitter and receive complete interrupt */
	 UART1_CONTROL = _BV(RXCIE1)|(1<<RXEN1)|(1<<TXEN1);
	 
	 /* Set frame format: asynchronous, 8data, no parity, 1stop bit */	
	 #ifdef URSEL1
	 UCSR1C = (1<<URSEL1)|(3<<UCSZ10);
	 #else
	 UCSR1C = (3<<UCSZ10);
	 #endif 
}/* uart_init */


/*************************************************************************
Function: uart1_getc()
Purpose:  return byte from ringbuffer  
Returns:  lower byte:  received byte from ringbuffer
			 higher byte: last receive error
**************************************************************************/
unsigned int uart1_getc(void)
{	 
	 unsigned char tmptail;
	 unsigned char data;


	 if ( UART1_RxBuf->head == UART1_RxBuf->tail ) {
		  return UART_NO_DATA;	/* no data available */
	 }
	 
	 /* calculate /store buffer index */
	 tmptail = (UART1_RxBuf->tail + 1) & UART_RxBuf->mask;
	 UART1_RxBuf->tail = tmptail; 
	 
	 /* get data from receive buffer */
	 data = UART1_RxBuf->buffer[tmptail];
	 
	 data = (UART1_LastRxError << 8) + data;
	 UART_LastRxError = 0;
	 return data;

}/* uart1_getc */


/*************************************************************************
Function: uart1_putc()
Purpose:  write byte to ringbuffer for transmitting via UART
Input:	 byte to be transmitted
Returns:  none			 
**************************************************************************/
void uart1_putc(unsigned char data)
{
	 unsigned char tmphead;

	 
	 tmphead  = (UART1_TxBuf->head + 1) & UART_TxBuf->mask;
	 
	 while ( tmphead == UART1_TxBuf->tail ){
		  ;/* wait for free space in buffer */
	 }
	 
	 UART1_TxBuf->buffer[tmphead] = data;
	 UART1_TxBuf->head = tmphead;

	 /* enable UDRE interrupt */
	 UART1_CONTROL	 |= _BV(UART1_UDRIE);

}/* uart1_putc */


/*************************************************************************
Function: uart1_puts()
Purpose:  transmit string to UART1
Input:	 string to be transmitted
Returns:  none			 
**************************************************************************/
void uart1_puts(const char *s )
{
	 while (*s) 
		uart1_putc(*s++);

}/* uart1_puts */


/*************************************************************************
Function: uart1_puts_p()
Purpose:  transmit string from program memory to UART1
Input:	 program memory string to be transmitted
Returns:  none
**************************************************************************/
void uart1_puts_p(const char *progmem_s )
{
	 register char c;
	 
	 while ( (c = pgm_read_byte(progmem_s++)) ) 
		uart1_putc(c);

}/* uart1_puts_p */


#endif

#ifdef UART_CONFIGURE_FOR_STDIO
#include <stdio.h>
static int uart_stdio_putc_wrapper(char c, FILE *stream);
static int uart_stdio_getc_wrapper(FILE *stream);

static FILE _uart_stdio_stream = FDEV_SETUP_STREAM(uart_stdio_putc_wrapper, uart_stdio_getc_wrapper, _FDEV_SETUP_RW);
#endif

void uart_put_bin8(char data)
{
	int i;
	for(i=0;i<8;i++)
	{
		if(data & (1<<(8-i-1)))
		uart_putc('1');
		else
		uart_putc('0');
	}
}

void uart_put_bin(void* data, int numBytes)
{
	int i;
	for(i=0;i<numBytes;i++)
	{
		uart_put_bin8(*((char*)(data + numBytes - i - 1)));
		uart_putc(' ');
	}
}

void uart_put_hex8( char data )
{
	char c = '0';
	c += ((data >> 4) & 0x0F);
	if(c > '9')
	c = 'A' + (c - '9' - 1);
	uart_putc(c);
	
	c = '0';
	c += (data & 0x0F);
	if(c > '9')
	c = 'A' + (c - '9' - 1);
	uart_putc(c);
}

void uart_put_hex( void* data, int numBytes )
{
	int i;
	for(i=0;i<numBytes;i++)
	{
		uart_put_hex8(*((char*)(data + numBytes - i - 1)));
		uart_putc(' ');
	}
}

#ifdef UART_CONFIGURE_FOR_STDIO
static int uart_stdio_putc_wrapper(char c, FILE *stream)
{
	uart_putc(c);
	return 0;
}

static int uart_stdio_getc_wrapper(FILE *stream)
{
	int i;
	do
	{
		i = uart_getc();
	} while(i == UART_NO_DATA || i == UART_BUFFER_OVERFLOW || i == UART_FRAME_ERROR);
	return i;
}

FILE* uart_get_output_stream()
{
	return &_uart_stdio_stream;
}

FILE* uart_get_input_stream()
{
	return &_uart_stdio_stream;
}

void uart_config_default_stdio( void )
{
	stdin = &_uart_stdio_stream;
	stdout = &_uart_stdio_stream;
	stderr = &_uart_stdio_stream;
}
#endif