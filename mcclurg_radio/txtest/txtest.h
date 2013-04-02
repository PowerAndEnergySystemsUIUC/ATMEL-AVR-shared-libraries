/*
 * txtest.h
 *
 * Created: 10/20/2011 12:20:42 PM
 *  Author: Josiah
 */ 


#ifndef TXTEST_H_
#define TXTEST_H_

void txtest_send_current_packet(void);
void txtest_fill_packet_from_uart(void);
void txtest_configure_radio(void);
void txtest_checkINT(void);
#endif /* TXTEST_H_ */