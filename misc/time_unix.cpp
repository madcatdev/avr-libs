/*
 * AVR-LIBS - Unix Time
 * (c) Paul Stoffregen https://github.com/PaulStoffregen/Time
 * (c) madcatdev https://github.com/madcatdev/avr-libs
 

 * NOTES:
 * This code based on https://github.com/PaulStoffregen/Time/blob/master/Time.cpp
 * The main difference - year contains 2 chars instead of 4. So, the earliest possible date - 2000.01.01,
 * which will be interpreted as 00.01.01

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.
This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 */ 

#include "time.h"

// Time library defines (for Unix Time)
#define LEAP_YEAR(Y)     ( ((1970+(Y))>0) && !((1970+(Y))%4) && ( ((1970+(Y))%100) || !((1970+(Y))%400) ) ) // leap year calulator expects year argument as years offset from 1970
static  const u8 monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0
#define SECS_PER_MIN  ((u32)(60UL))
#define SECS_PER_HOUR ((u32)(3600UL))
#define SECS_PER_DAY  ((u32)(SECS_PER_HOUR * 24UL))
#define DAYS_PER_WEEK ((u32)(7UL))
#define SECS_PER_WEEK ((u32)(SECS_PER_DAY * DAYS_PER_WEEK))
#define SECS_PER_YEAR ((u32)(SECS_PER_DAY * 365UL)) // TODO: ought to handle leap years

u32 TimeToUnix(const time_struct &tm){
	// Преобразование даты в формат Unix Time
	// Based on https://github.com/PaulStoffregen/Time
	// This version cannot operate with date before year 2000, but we dont need it
	i16 i;
	u32 seconds;
	u8 year_2000 = tm.year + 30; // Shift from 1970 на 2000

	// seconds from 1970 till 1 jan 00:00:00 of the given year
	seconds= year_2000 * (SECS_PER_DAY * 365);
	for (i = 0; i < year_2000; i++) {
		if (LEAP_YEAR(i))
		seconds +=  SECS_PER_DAY;   // add extra days for leap years
	}
	
	// add days for this year, months start from 1
	for (i = 1; i < tm.month; i++) {
		if ( (i == 2) && LEAP_YEAR(year_2000))
		seconds += SECS_PER_DAY * 29;
		else
		seconds += SECS_PER_DAY * monthDays[i-1];  //monthDay array starts from 0
	}
	seconds+= (tm.day-1) * SECS_PER_DAY;
	seconds+= tm.hour * SECS_PER_HOUR;
	seconds+= tm.min * SECS_PER_MIN;
	seconds+= tm.sec;
	return (u32)seconds;
}

void UnixToTime(u32 timeInput, time_struct &tm){
	// Конвертация Unix Time в структуру с датой
	// Based on https://github.com/PaulStoffregen/Time
	// Работает, не требует зависимостей, учитывае високосный год. Однако жрет много тактов, рекомендую юзать как можно реже!
	u8 year;
	u8 month, monthLength;
	u32 time;
	u32 days;

	time = (u32)timeInput;
	tm.sec = time % 60;
	time /= 60; // now it is minutes
	tm.min = time % 60;
	time /= 60; // now it is hours
	tm.hour = time % 24;
	time /= 24; // now it is days
	
	year = 0;
	days = 0;
	while((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time)
	year++;
	
	tm.year = year - 30; // Offset from 1970 - 30 = Offset from 2000. Пример: 2012 = 12
	days -= LEAP_YEAR(year) ? 366 : 365;
	time -= days; // now it is days in this year, starting at 0
	
	days=0;
	month=0;
	monthLength=0;
	for (month=0; month<12; month++) {
		if (month==1) { // february
			if (LEAP_YEAR(year)) {
				monthLength=29;
				} else {
				monthLength=28;
			}
			} else {
			monthLength = monthDays[month];
		}
		
		if (time >= monthLength) {
			time -= monthLength;
			} else {
			break;
		}
	}
	tm.month = month + 1;  // jan is month 1
	tm.day = time + 1;     // day of month
}