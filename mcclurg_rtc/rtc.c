#include "rtc.h"

#include <math.h>

#define RTC_SECONDS_PER_DAY 86400L

uint8_t _rtc_year_offset;
uint8_t _rtc_address;

// set stop counting bit 1 = start, 0 = stop
void rtc_setCounting(uint8_t count)
{
	_rtc_setCounting_dev(count,_rtc_address);
}

uint8_t rtc_bcd_to_binary(uint8_t bcd)
{
	return ((((bcd >> 4) & 0x0F)*10) + (bcd & 0x0F));
}

uint8_t rtc_binary_to_bcd(uint8_t binary)
{
	return ((((binary/10)<<4) & 0xF0) | ((binary % 10) & 0x0F));
}

void rtc_get_date( Date* tm )
{
	_rtc_get_date_dev(tm,_rtc_year_offset,_rtc_address);
}

void rtc_set_date(Date* tm)
{
	rtc_setCounting(0);
	_rtc_set_date_dev(tm,_rtc_year_offset,_rtc_address);
}

void rtc_get_time(Timestamp* ts)
{
	_rtc_get_time_dev(ts,_rtc_year_offset,_rtc_address);
}

void rtc_set_time(Timestamp* ts)
{
	rtc_setCounting(0);
	_rtc_set_time_dev(ts,_rtc_year_offset,_rtc_address);
}

///////////////////////////////////////////////////////////////
// Initalize RTC to some time value and start the clock
void initRTC(uint8_t address, uint8_t year_offset)
{
	_rtc_address = address;
	_rtc_year_offset = year_offset;
	
	_rtc_dev_init(address,year_offset);
}

void rtc_timestamp_to_date( Timestamp* ts, Date* date )
{
	// ts max is 0xFFFFFFFF, so the maximum of days is only 0xC22F
	uint16_t days    = (uint16_t)((*ts)/RTC_SECONDS_PER_DAY);
    //									    years: 1970 1971 1972 1973
	// How many 4-year segments do you have (going 365, 365, 366, 365 days)
	date->year    = days/(365*4 + 1);
	date->year    = 1970 + (date->year)*4;

	// How much is left over when you divide days into 4-year segments
		days    = days%(365*4 + 1);
	if(days >= 365 && days < 2*365)
	{
		days -= 365;
		date->year += 1;
	}
	else if(days >= 2*365 && days < 3*365 + 1)
	{
		days -= 2*365;
		date->year += 2;
	}
	else
	{
		days -= (3*365 + 1);
		date->year += 3;
	}

	uint32_t seconds = (*ts)%RTC_SECONDS_PER_DAY;
	date->hours   = (uint8_t)(seconds/3600);
		seconds = seconds%3600;
	date->minutes = (uint8_t)(seconds/60);
	date->seconds = (uint8_t)(seconds%60);

	for(date->month=1; date->month <= 12; (date->month)++)
	{
		// 30 days has September, April, June, and November
		if((date->month == 4 || date->month == 6 || date->month == 9 || date->month == 11) && days >= 30)
			days -= 30;

		// Except for February, which has 28 days
		// (the 29 days on leap year already accounted for above)
		else if(date->month == 2 && days >= 28)
			days -= 28;

		// All the rest have 31
		else if(days >= 31)
			days -= 31;

		else
			break;
	}
	days++;
	date->day = days;
}

void rtc_date_to_timestamp( Date* date, Timestamp* ts )
{
	uint16_t year = date->year - 1970;
	uint8_t leaps = year/4;
	if((year % 4) == 2)
		leaps++;
		
	*ts = RTC_SECONDS_PER_DAY*365*year;
	*ts += RTC_SECONDS_PER_DAY*leaps;
	
	uint8_t i;
	for(i=1; i < (date->month); i++)
	{
		// 30 days has September, April, June, and November
		if(i == 4 || i == 6 || i == 9 || i == 11)
			*ts += RTC_SECONDS_PER_DAY*30;

		// Except for February, which has 28 days
		// (the 29 days on leap year already accounted for above)
		else if(i == 2)
			*ts += RTC_SECONDS_PER_DAY*28;

		// All the rest have 31
		else
			*ts += RTC_SECONDS_PER_DAY*31;
	}
	*ts += RTC_SECONDS_PER_DAY*(date->day - 1);
	*ts += 3600*(date->hours);
	*ts += 60*(date->minutes);
	*ts += (date->seconds);
}