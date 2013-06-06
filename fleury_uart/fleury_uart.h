#ifndef FUART_H
#define FUART_H
/************************************************************************
 STDIO wrapper and extensions written by Josiah McClurg, spring 2013ish.
 I'm not all too concerned about Copyrights, but attribution would make
 me happy.
 ************************************************************************/
#include <avr/io.h>
#include "uart.h"

#ifdef UART_CONFIGURE_FOR_STDIO
#include <stdio.h>

void uart_config_default_stdio(void);
FILE* uart_get_output_stream();
FILE* uart_get_input_stream();

#endif

void uart_put_bin8(char data);
void uart_put_hex8(char data);
void uart_put_bin(void* data, int numBytes);
void uart_put_hex(void* data, int numBytes);

#endif // UART_H 

