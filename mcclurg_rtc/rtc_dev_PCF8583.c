/*
 * rtc_dev_PCF8583.c
 *
 * Created: 2/1/2012 10:44:19 AM
 *  Author: Josiah
 */ 

#ifdef RTC_CHIP_PCF8583

#include "rtc.h"
#include "../fleury_i2c/raw_i2c.h"
#include "rtc_dev_PCF8583.h"

#define RTC_CONFIG_COMMAND 0

void _rtc_setCounting_dev(uint8_t count, uint8_t addr)
{
   i2c_start_wait(addr+RTC_DEV_WRITE_ADDRESS); // Addresses the RTC
   i2c_write(0x00); // Set memory address 0x00
   i2c_write(RTC_CONFIG_COMMAND | (((count == 0) & 0x1)<<7));
   i2c_stop();
}

void _rtc_set_time_dev(Timestamp* ts, uint8_t year_offset, uint8_t addr)
{
	Date d;
	rtc_timestamp_to_date(ts,&d);
	_rtc_set_date_dev(&d,year_offset,addr);
}

void _rtc_get_time_dev(Timestamp* ts, uint8_t year_offset, uint8_t addr)
{
	Date d;
	_rtc_get_date_dev(&d,year_offset,addr);
	rtc_date_to_timestamp(&d,ts);
}

void _rtc_set_date_dev(Date* tm, uint8_t year_offset, uint8_t addr)
{
	i2c_start_wait(addr+RTC_DEV_WRITE_ADDRESS); // Addresses the RTC
    i2c_write(0x01); // Set address 0x00

    i2c_write(0); // centiseconds
    i2c_write(rtc_binary_to_bcd(tm->seconds));
    i2c_write(rtc_binary_to_bcd(tm->minutes));
    i2c_write(rtc_binary_to_bcd(tm->hours & 0x3F));
	i2c_write(rtc_binary_to_bcd((tm->day & 0x3F)) | rtc_binary_to_bcd(((((tm->year - 1970 - year_offset) % RTC_MAX_YEARS) & 0x3) << 6)));
	i2c_write(rtc_binary_to_bcd(tm->month));
	i2c_stop();
}

void _rtc_get_date_dev(Date* tm, uint8_t year_offset, uint8_t addr)
{
   uint8_t c;
   i2c_start_wait(addr+RTC_DEV_WRITE_ADDRESS);  // set device address and write mode
   i2c_write(0x02);                            // write memory address = 0x02
   i2c_rep_start(addr+RTC_DEV_READ_ADDRESS);    // set device address and read mode
   
   c = i2c_readAck();                     // 0x02
   tm->seconds = rtc_bcd_to_binary(c);
   
   c = i2c_readAck();                     // 0x03
   tm->minutes = rtc_bcd_to_binary(c);
	  
   // Throw away AMPM information
   i2c_readAck();                         // 0x04
   
   c = i2c_readAck();                     // 0x05
   tm->day = rtc_bcd_to_binary(c & 0x3F);
   tm->year =  ((rtc_bcd_to_binary(c) >> 6) & 0x3) + 1970 + year_offset;
         
   c = i2c_readNak();                     // 0x06: don't acknowledge the last byte
   
   // Throw away day of week information
   //tm->dayOfWeek = ((c >> 5) & 0x7);
   tm->month = (c & 0x1F);
      
   i2c_stop();					          // stop i2c bus
}

void _rtc_dev_init( uint8_t address, uint8_t year_offset )
{
	i2c_start_wait(address+RTC_DEV_WRITE_ADDRESS); // Addresses the RTC
	i2c_write(0x00); // Set memory address 0x00
	i2c_write(RTC_CONFIG_COMMAND);
	i2c_stop();
}

#endif