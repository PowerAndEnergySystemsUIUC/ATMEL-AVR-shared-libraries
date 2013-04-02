
#include <stdlib.h>
#include <stdint.h>

#if defined(RTC_CHIP_PCF8583)
#include "rtc_dev_PCF8583.h"
#elif defined(RTC_CHIP_PCF8563)
#include "rtc_dev_PCF8563.h"
#else
#error "No chip defined!"
#endif

typedef uint32_t Timestamp;
typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t day;
	uint8_t month;
	uint16_t year;
} Date;

void initRTC(uint8_t address, uint8_t year_offset);
void rtc_setCounting(uint8_t count);
void rtc_get_date(Date* tm);
void rtc_set_date(Date* tm);
void rtc_get_time(Timestamp* ts);
void rtc_set_time(Timestamp* ts);

void rtc_timestamp_to_date(Timestamp* ts, Date* date);
void rtc_date_to_timestamp(Date* date, Timestamp* ts);
uint8_t rtc_bcd_to_binary(uint8_t bcd);
uint8_t rtc_binary_to_bcd(uint8_t binary);

void _rtc_setCounting_dev(uint8_t count, uint8_t addr);
void _rtc_set_time_dev(Timestamp* ts, uint8_t year_offset, uint8_t addr);
void _rtc_get_time_dev(Timestamp* ts, uint8_t year_offset, uint8_t addr);
void _rtc_set_date_dev(Date* tm, uint8_t year_offset, uint8_t addr);
void _rtc_get_date_dev(Date* tm, uint8_t year_offset, uint8_t addr);
void _rtc_dev_init( uint8_t address, uint8_t year_offset );