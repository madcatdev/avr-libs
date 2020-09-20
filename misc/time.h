/*
 * AVR-LIBS - Time
 * MCU: Any
 * Created: 14.01.2019
 * Last version: 14.09.2020
 * (c) madcatdev https://github.com/madcatdev/avr-libs

 * DESCR:
 * Bunch of functions for operations with time

 * TODO:
 */ 


#ifndef TIME_H_
#define TIME_H_

	#include "inttypes.h"
	#include "string.h"
	
	// Simple timekeeping structure (non-BCD)
	struct time_struct	{ 
		u8 sec;
		u8 min;
		u8 hour;
		u8 wday; // Day of Week (DOW), 1-7
		u8 day;
		u8 month;
		u8 year;
		u8 validUTC; // 
	};

	void UnixToTime(u32 timeInput, time_struct &tm);
	u32 TimeToUnix(const time_struct &tm);
	void TimeToStr(char *str, time_struct &t);
	u8 DecToBcd(u8 dec);
	u8 BcdToDec(u8 bcd);


#endif /* TIME_H_ */