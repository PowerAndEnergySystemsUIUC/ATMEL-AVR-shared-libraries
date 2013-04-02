/*
 * rfm22b.h
 *
 * Created: 10/20/2011 12:14:33 PM
 *  Author: Josiah
 */ 


#ifndef RFM22B_H_
#define RFM22B_H_

#include <stdint.h>
#include <avr/io.h>

#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t)~(1 << mask))
#define CS 2 //pin for chip select
#define NIRQ 4
#define TXANT 5	// PD5
#define RXANT 6	// PD6

#define DTYPE 0x00
#define DVERS 0x01
#define DSTATUS 0x02

#define INTEN1 0x05
#define INTEN2 0x06
#define OMFC1 0x07
#define OMFC2 0x08
#define CRYSLOADCAP 0x09
#define UCOUTCLK 0x0A

#define TEMPCAL 0x12
#define TEMPOFF 0x13

void rfm22b_init (void);
char rfm22b_read(uint8_t address);
void rfm22b_write(uint8_t address, char data);

void _rfm22b_delay_ms(uint16_t x);
void _rfm22b_delay_us(uint16_t x);

#endif /* RFM22B_H_ */