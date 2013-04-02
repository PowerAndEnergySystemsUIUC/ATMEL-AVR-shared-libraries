/*
 * rfm22b.c
 *
 * Created: 10/20/2011 12:14:51 PM
 *  Author: Josiah
 */ 

#include "../lib-spi/spi.h"
#include "rfm22b.h"

void rfm22b_write(uint8_t address, char data)
{
	//write any data byte to any single address
	//adds a 0 to the MSB of the address byte (WRITE mode)
	
	address |= 0x80;

	cbi(PORTB,CS);
	_rfm22b_delay_ms(1);
	spi_write(address);
	_rfm22b_delay_ms(1);
	spi_write(data);
	_rfm22b_delay_ms(1);
	sbi(PORTB,CS);
}

char rfm22b_read(uint8_t address)
{
	//returns the contents of any 1 byte register from any address
	//sets the MSB for every address byte (READ mode)

	char byte;
	
	address &= 0x7F;

	cbi(PORTB,CS);
	spi_write(address);
	byte = spi_read();
	sbi(PORTB,CS);

	return byte;
}


void rfm22b_init (void)
{
	spi_init();
	
    //1 = output, 0 = input
    DDRB = 0b11101111; //MISO input
    DDRC = 0b11111111; //All outputs
    DDRD = 0b11101110; //PORTD (RX on PD0), PD4 input
	
	TCCR2B = (1<<CS21);
	
	cbi(PORTD, TXANT);
	cbi(PORTD, RXANT);
}


//General short delays
void _rfm22b_delay_ms(uint16_t x)
{
    for (; x > 0 ; x--)
        _rfm22b_delay_us(1000);
}

//General short delays
void _rfm22b_delay_us(uint16_t x)
{
    while(x > 256)
    {
        TIFR2 = (1<<TOV2); //Clear any interrupt flags on Timer2
        TCNT2 = 0; //256 - 125 = 131 : Preload timer 2 for x clicks. Should be 1us per click
        while( (TIFR2 & (1<<TOV2)) == 0);

        x -= 256;
    }

    TIFR2 = (1<<TOV2); //Clear any interrupt flags on Timer2
    TCNT2= 256 - x; //256 - 125 = 131 : Preload timer 2 for x clicks. Should be 1us per click
    while( (TIFR2 & (1<<TOV2)) == 0);
}