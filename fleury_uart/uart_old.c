
/*
 * Copyright (c) 2006-2011 by Roland Riegel <feedback@roland-riegel.de>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sfr_defs.h>
#include <avr/sleep.h>

#include "uart.h"

void uart_init(void)
{
    /* set baud rate */
    UBRRH = UBRRVAL >> 8;
    UBRRL = UBRRVAL & 0xff;
    /* set frame format: 8 bit, no parity, 1 bit */
    UCSRC = UCSRC_SELECT | (1 << UCSZ1) | (1 << UCSZ0);
    /* enable serial receiver and transmitter */
#if !USE_SLEEP
    UCSRB = (1 << RXEN) | (1 << TXEN);
#else
    UCSRB = (1 << RXEN) | (1 << TXEN) | (1 << RXCIE);
#endif
}

void uart_putc(uint8_t c)
{
    if(c == '\n')
        uart_putc('\r');

    /* wait until transmit buffer is empty */
    while(!(UCSRA & (1 << UDRE)));

    /* send next byte */
    UDR = c;
}

void uart_putc_hex(uint8_t b)
{
    /* upper nibble */
    if((b >> 4) < 0x0a)
        uart_putc((b >> 4) + '0');
    else
        uart_putc((b >> 4) - 0x0a + 'a');

    /* lower nibble */
    if((b & 0x0f) < 0x0a)
        uart_putc((b & 0x0f) + '0');
    else
        uart_putc((b & 0x0f) - 0x0a + 'a');
}

void uart_putw_hex(uint16_t w)
{
    uart_putc_hex((uint8_t) (w >> 8));
    uart_putc_hex((uint8_t) (w & 0xff));
}

void uart_putdw_hex(uint32_t dw)
{
    uart_putw_hex((uint16_t) (dw >> 16));
    uart_putw_hex((uint16_t) (dw & 0xffff));
}

void uart_putw_dec(uint16_t w)
{
    uint16_t num = 10000;
    uint8_t started = 0;

    while(num > 0)
    {
        uint8_t b = w / num;
        if(b > 0 || started || num == 1)
        {
            uart_putc('0' + b);
            started = 1;
        }
        w -= b * num;

        num /= 10;
    }
}

void uart_putdw_dec(uint32_t dw)
{
    uint32_t num = 1000000000;
    uint8_t started = 0;

    while(num > 0)
    {
        uint8_t b = dw / num;
        if(b > 0 || started || num == 1)
        {
            uart_putc('0' + b);
            started = 1;
        }
        dw -= b * num;

        num /= 10;
    }
}

void uart_puts(const char* str)
{
    while(*str)
        uart_putc(*str++);
}

void uart_puts_p(PGM_P str)
{
    while(1)
    {
        uint8_t b = pgm_read_byte_near(str++);
        if(!b)
            break;

        uart_putc(b);
    }
}

uint8_t uart_getc(void)
{
    /* wait until receive buffer is full */
#if USE_SLEEP
    uint8_t sreg = SREG;
    sei();

    while(!(UCSRA & (1 << RXC)))
        sleep_mode();z

    SREG = sreg;
#else
    while(!(UCSRA & (1 << RXC)));
#endif

    uint8_t b = UDR;
    if(b == '\r')
        b = '\n';

    return b;
}

uint8_t uart_read_line(char* buffer, uint8_t buffer_length)
{
    uint8_t read_length = 0;
    while(read_length < buffer_length - 1)
    {
        uint8_t c = uart_getc();

        if(c == 0x08 || c == 0x7f)
        {
            if(read_length < 1)
                continue;

            --read_length;
            buffer[read_length] = '\0';

            uart_putc(0x08);
            uart_putc(' ');
            uart_putc(0x08);

            continue;
        }

        uart_putc(c);

        if(c == '\n')
        {
            buffer[read_length] = '\0';
            break;
        }
        else
        {
            buffer[read_length] = c;
            ++read_length;
        }
    }

    return read_length;
}

void uart_puts_int8(uint8_t number)
{
   int i;
   unsigned char c;
   for(i = 1;i >= 0; i--)
   {
	   c = ((number >> (4*i)) & 0x0F) + '0';
	   if(c > '9')
	      c += ('A' - '9' - 1);
       uart_putc(c);
   }
}

void uart_puts_int16(uint16_t number)
{
   int i;
   unsigned char c;
   for(i = 3;i >= 0; i--)
   {
	   c = ((number >> (4*i)) & 0x0F) + '0';
	   if(c > '9')
	      c += ('A' - '9' - 1);
       uart_putc(c);
   }
}

void uart_puts_int32(uint32_t number)
{
   int i;
   unsigned char c;
   for(i = 7; i >= 0; i--)
   {
	   c = ((number >> (4*i)) & 0x0F) + '0';
	   if(c > '9')
	      c += ('A' - '9' - 1);
       uart_putc(c);
   }
}

EMPTY_INTERRUPT(USART_RXC_vect)