
/*
 * Copyright (c) 2006-2011 by Roland Riegel <feedback@roland-riegel.de>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <avr/pgmspace.h>

/* some mcus have multiple uarts */
#ifdef UDR1
#define UBRRH UBRR1H
#define UBRRL UBRR1L
#define UDR UDR1

#define UCSRA UCSR1A
#define UDRE UDRE1
#define RXC RXC1

#define UCSRB UCSR1B
#define RXEN RXEN1
#define TXEN TXEN1
#define RXCIE RXCIE1

#define UCSRC UCSR1C
#define URSEL 
#define UCSZ0 UCSZ10
#define UCSZ1 UCSZ11
#define UCSRC_SELECT 0
#else
#define UCSRC_SELECT (1 << URSEL)
#endif

#ifndef USART_RXC_vect
#if defined(UART0_RX_vect)
#define USART_RXC_vect UART0_RX_vect
#elif defined(UART_RX_vect)
#define USART_RXC_vect UART_RX_vect
#elif defined(USART0_RX_vect)
#define USART_RXC_vect USART0_RX_vect
#elif defined(USART_RX_vect)
#define USART_RXC_vect USART_RX_vect
#elif defined(USART0_RXC_vect)
#define USART_RXC_vect USART0_RXC_vect
#elif defined(USART1_RX_vect)
#define USART_RXC_vect USART1_RX_vect
#elif defined(USART_RXC_vect)
#define USART_RXC_vect USART_RXC_vect
#else
#error "Uart receive complete interrupt not defined!"
#endif
#endif

#ifndef F_CPU
#define F_CPU 16000000
#endif
//#define BAUD 19200UL
#define BAUD 9600UL
#define UBRRVAL (F_CPU/(BAUD*16)-1)
#define USE_SLEEP 0


#ifdef __cplusplus
extern "C"
{
#endif

void uart_init(void);

void uart_putc(uint8_t c);

void uart_putc_hex(uint8_t b);
void uart_putw_hex(uint16_t w);
void uart_putdw_hex(uint32_t dw);

void uart_putw_dec(uint16_t w);
void uart_putdw_dec(uint32_t dw);

void uart_puts(const char* str);
void uart_puts_p(PGM_P str);

uint8_t uart_getc(void);
uint8_t uart_read_line(char* buffer, uint8_t buffer_length);

void uart_puts_int8(uint8_t number);
void uart_puts_int16(uint16_t number);
void uart_puts_int32(uint32_t number);

#ifdef __cplusplus
}
#endif

#endif

