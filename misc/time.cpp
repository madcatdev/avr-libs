/*
 * AVR-LIBS - Time
 * (c) madcatdev https://github.com/madcatdev/avr-libs
 */ 

#include "time.h"

void TimeToStr(char *str, time_struct &t) {
	// Вывод времени в формате ГГ-ММ-ДД ЧЧ:ММ:СС
	// Общая длина строки - 12 байт + \0
	char b[4]; // Буфер (255\0)
	
	// Конвертируем и копируем посимвольно
	u8_to_str_z(b, t.year);
	*str++ = b[0];
	*str++ = b[1]; 
	*str++ = '-';
	u8_to_str_z(b, t.month);
	*str++ = b[0]; 
	*str++ = b[1]; 
	*str++ = '-';		
	u8_to_str_z(b, t.day);
	*str++ = b[0];
	*str++ = b[1];
	*str++ = ' ';
	u8_to_str_z(b, t.hour);
	*str++ = b[0];
	*str++ = b[1];
	*str++ = ':';
	u8_to_str_z(b, t.min);
	*str++ = b[0];
	*str++ = b[1];
	*str++ = ':';	
	u8_to_str_z(b, t.sec);
	*str++ = b[0];
	*str++ = b[1];
		
	*str++ = '\0'; // Кончаем строку нулевым символом
}

u8 BcdToDec(u8 bcd) {
	// Преобразование из BCD (бинарно-десятчиный формат Dallas RTC) в человеческий DEC
	return (((bcd & 0xF0) >> 4)*10) + (bcd & 0x0F);
}

u8 DecToBcd(u8 dec) {
	// Преобразование из DEC в BCD
	return ( (dec / 10) << 4) + (dec % 10);
}