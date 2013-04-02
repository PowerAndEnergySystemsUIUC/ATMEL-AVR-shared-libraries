/*
 * mcclurg_radio.h
 *
 * Created: 2/9/2012 5:22:44 AM
 *  Author: Josiah
 */ 


#ifndef MCCLURG_RADIO_H_
#define MCCLURG_RADIO_H_

#ifdef RADIO_DEV_RFM22B
#include "radio_dev_rfm22b.h"
#else
#error "No device specified"
#endif

void radio_init(void);
uint8_t radio_tx(void* packet, uint16_t size);
uint8_t radio_rx(void* packet, uint16_t size);


#endif /* MCCLURG_RADIO_H_ */