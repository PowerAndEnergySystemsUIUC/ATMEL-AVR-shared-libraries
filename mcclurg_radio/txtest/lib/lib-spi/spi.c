/*
 * spi.c
 *
 * Created: 10/20/2011 12:10:03 PM
 *  Author: Josiah
 */ 

#include <avr/io.h>
#include "spi.h"

char spi_read(void)
{
	SPDR = 0x55;
	while((SPSR&0x80) == 0x00)
		;

	return SPDR;
}

void spi_write(char data)
{
	SPDR = data;
	while((SPSR&0x80) == 0x00);
}

void spi_init(void)
{
	// enable SPI
	// make SPI master
	// SCLK idle low
	// sample data on rising edge
	// Fosc/4 is SPI frequency = 2MHz
	//SPCR |= 0b01010000;	// Fosc/4
	SPCR |= 0b01010011;		// Fosc/128
}