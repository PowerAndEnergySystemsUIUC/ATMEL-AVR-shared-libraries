/*
    Created on: Mar. 30, 2010
	Under project: RFM22
	Author: Jim Lindblom
	RFM22 Transmit Example
	
	Hardware: Arduino Pro (ATmega328) running at 8Mhz, 3.3V
	Wiring:
	RFM22 BOB	|	Arduino Pro
	----------------------------
		GND		|	GND
		3.3V	|	3.3V
		GPIO0	|	NC
		GPIO1	|	NC
		GPIO2	|	NC
		RXANT	|	PD6 (D6)
		TXANT	|	PD5 (D5)
		IRQ		|	PD4 (D4)
		CSN		|	PB2 (D10)
		SCK		|	PB5 (D13)
		SDI		|	PB3 (D11)
		SDO		|	PB4 (D12)
		
	Antenna is a 17cm piece of wire sticking straight up into the air.
*/

//======================//
//======================//
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "demo_txtest.h"
#include "radio_dev_rfm22b.h"

void txtest_send_current_packet(void);
void txtest_fill_packet_from_uart(void);

//======================//
//Global Variables
unsigned char tx_buf[17] = "0123456789:;<=>?";

// Initialize the RFM22 for transmitting
void txtest_configure_radio(void)
{
	rfm22b_write(INTEN2, 0x00);	// Disable all interrupts
	rfm22b_write(OMFC1, 0x01);		// Set READY mode
	rfm22b_write(0x09, 0x7F);		// Cap = 12.5pF
	rfm22b_write(0x0A, 0x05);		// Clk output is 2MHz
	
	rfm22b_write(0x0B, 0xF4);		// GPIO0 is for RX data output
	rfm22b_write(0x0C, 0xEF);		// GPIO1 is TX/RX data CLK output
	rfm22b_write(0x0D, 0x00);		// GPIO2 for MCLK output
	rfm22b_write(0x0E, 0x00);		// GPIO port use default value
	
	rfm22b_write(0x0F, 0x70);		// NO ADC used
	rfm22b_write(0x10, 0x00);		// no ADC used
	rfm22b_write(0x12, 0x00);		// No temp sensor used
	rfm22b_write(0x13, 0x00);		// no temp sensor used
	
	rfm22b_write(0x70, 0x20);		// No manchester code, no data whiting, data rate < 30Kbps
	
	rfm22b_write(0x1C, 0x1D);		// IF filter bandwidth
	rfm22b_write(0x1D, 0x40);		// AFC Loop
	//write(0x1E, 0x0A);	// AFC timing
	
	rfm22b_write(0x20, 0xA1);		// clock recovery
	rfm22b_write(0x21, 0x20);		// clock recovery
	rfm22b_write(0x22, 0x4E);		// clock recovery
	rfm22b_write(0x23, 0xA5);		// clock recovery
	rfm22b_write(0x24, 0x00);		// clock recovery timing
	rfm22b_write(0x25, 0x0A);		// clock recovery timing
	
	//write(0x2A, 0x18);
	rfm22b_write(0x2C, 0x00);
	rfm22b_write(0x2D, 0x00);
	rfm22b_write(0x2E, 0x00);
	
	rfm22b_write(0x6E, 0x27);		// TX data rate 1
	rfm22b_write(0x6F, 0x52);		// TX data rate 0
	
	rfm22b_write(0x30, 0x8C);		// Data access control
	
	rfm22b_write(0x32, 0xFF);		// Header control
	
	rfm22b_write(0x33, 0x42);		// Header 3, 2, 1, 0 used for head length, fixed packet length, synchronize word length 3, 2,
	
	rfm22b_write(0x34, 64);		// 64 nibble = 32 byte preamble
	rfm22b_write(0x35, 0x20);		// 0x35 need to detect 20bit preamble
	rfm22b_write(0x36, 0x2D);		// synchronize word
	rfm22b_write(0x37, 0xD4);
	rfm22b_write(0x38, 0x00);
	rfm22b_write(0x39, 0x00);
	rfm22b_write(0x3A, 's');		// set tx header 3
	rfm22b_write(0x3B, 'o');		// set tx header 2
	rfm22b_write(0x3C, 'n');		// set tx header 1
	rfm22b_write(0x3D, 'g');		// set tx header 0
	rfm22b_write(0x3E, 17);		// set packet length to 17 bytes
	
	rfm22b_write(0x3F, 's');		// set rx header
	rfm22b_write(0x40, 'o');
	rfm22b_write(0x41, 'n');
	rfm22b_write(0x42, 'g');
	rfm22b_write(0x43, 0xFF);		// check all bits
	rfm22b_write(0x44, 0xFF);		// Check all bits
	rfm22b_write(0x45, 0xFF);		// check all bits
	rfm22b_write(0x46, 0xFF);		// Check all bits
	
	rfm22b_write(0x56, 0x01);
	
	rfm22b_write(0x6D, 0x07);		// Tx power to max
	
	rfm22b_write(0x79, 0x00);		// no frequency hopping
	rfm22b_write(0x7A, 0x00);		// no frequency hopping
	
	rfm22b_write(0x71, 0x22);		// GFSK, fd[8]=0, no invert for TX/RX data, FIFO mode, txclk-->gpio
	
	rfm22b_write(0x72, 0x48);		// Frequency deviation setting to 45K=72*625
	
	rfm22b_write(0x73, 0x00);		// No frequency offset
	rfm22b_write(0x74, 0x00);		// No frequency offset
	
	rfm22b_write(0x75, 0x53);		// frequency set to 434MHz
	rfm22b_write(0x76, 0x64);		// frequency set to 434MHz
	rfm22b_write(0x77, 0x00);		// frequency set to 434Mhz
	
	rfm22b_write(0x5A, 0x7F);
	rfm22b_write(0x59, 0x40);
	rfm22b_write(0x58, 0x80);
	
	rfm22b_write(0x6A, 0x0B);
	rfm22b_write(0x68, 0x04);
	rfm22b_write(0x1F, 0x03);
}

void txtest_send_current_packet(void)
{
	unsigned char i;
    printf("Transmitting: %s\n", tx_buf);
	rfm22b_write(0x07, 0x01);	// To ready mode
	cbi(PORTD, RXANT);
	sbi(PORTD, TXANT);
	_rfm22b_delay_ms(50);
	
	rfm22b_write(0x08, 0x03);	// FIFO reset
	rfm22b_write(0x08, 0x00);	// Clear FIFO
	
	rfm22b_write(0x34, 64);	// preamble = 64nibble
	rfm22b_write(0x3E, 17);	// packet length = 17bytes
	for (i=0; i<17; i++)
	{
		rfm22b_write(0x7F, tx_buf[i]);	// send payload to the FIFO
	}

	rfm22b_write(0x05, 0x04);	// enable packet sent interrupt
	i = rfm22b_read(0x03);		// Read Interrupt status1 register
	i = rfm22b_read(0x04);
	
	rfm22b_write(0x07, 9);	// Start TX
	
	while ((PIND & (1<<NIRQ)) != 0)
		; 	// need to check interrupt here
	
	rfm22b_write(0x07, 0x01);	// to ready mode
	
	cbi(PORTD, RXANT);	// disable all interrupts
	cbi(PORTD, TXANT);
}

void txtest_fill_packet_from_uart(void)
{
	unsigned char i, chksum;
	
	for(i=0; i<16; i++)
	{
		chksum = fgetc(stdin);
		if(chksum == '\r' || chksum == '\n')
		break;
		
		tx_buf[i] = chksum;
		fputc(tx_buf[i],stdout);
	}
	for(;i<16;i++)
	{
		tx_buf[i] = 0;
	}
	
	chksum = 0;
	for(i=0; i<16; i++)
		chksum += tx_buf[i];
	
	tx_buf[16] = chksum;
}

void txtest_checkINT(void)
{
	if ((PIND & (1<<NIRQ)) == 0)
		printf("INT == 0\n");
	else
		printf("INT == 1\n");
}