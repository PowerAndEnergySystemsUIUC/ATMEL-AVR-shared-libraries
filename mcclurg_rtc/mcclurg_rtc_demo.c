/*
 * rtc_demo.c
 *
 * Created: 1/31/2012 3:37:37 PM
 *  Author: Josiah
 */ 
#ifdef MCCLURG_RTC_DEMO

#include <avr/interrupt.h>

#define F_CPU 8000000L

#define SCL_CLOCK  100000L
#include "../fleury_i2c/raw_i2c.h"

#define UART_BAUD_RATE 19200
#define UART_CONFIGURE_FOR_STDIO
#include "../fleury_uart/fleury_uart.h"

#include "rtc.h"

int main(void)
{
	i2c_init(I2C_CLOCK_SELECT(SCL_CLOCK,F_CPU));
	initRTC(0,0);
	
	uart_config_default_stdio();
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
	
	sei();
	
	Date date;
	Timestamp ts;
   date.year   = 1988;
   date.month  = 12;
   date.day    = 13;
   date.hours   = 6;
   date.minutes = 13;
   date.seconds = 57;
        
	printf("\n\nSetting clock to %d:%d:%d on %d/%d/%d\n",date.hours,date.minutes,date.seconds,date.month,date.day,date.year);
	rtc_set_date(&date);   
	rtc_get_date(&date);
	printf("The time is now %d:%d:%d on %d/%d/%d\n",date.hours,date.minutes,date.seconds,date.month,date.day,date.year);
	rtc_setCounting(1);
	
	printf("Kindly type a key to view the updated time.\n");
	while(1)
	{
		fgetc(stdin);
		rtc_get_date(&date);
		printf("The time is now %d:%d:%d on %d/%d/%d",date.hours,date.minutes,date.seconds,date.month,date.day,date.year);
		rtc_date_to_timestamp(&date,&ts);
		printf(" (%ld)\n",ts);
	}
	
	return 0;
}

#endif