/************************************************************************
 STDIO wrapper and extensions written by Josiah McClurg, spring 2013ish.
 I'm not all too concerned about Copyrights, but attribution would make
 me happy.
 ************************************************************************/
#include "uart.h"
#include "fleury_uart.h"

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