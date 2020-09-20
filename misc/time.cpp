/*
 * AVR-LIBS - Time
 * (c) madcatdev https://github.com/madcatdev/avr-libs
 */ 

#include "time.h"

void TimeToStr(char *str, time_struct &t) {
	// ����� ������� � ������� ��-��-�� ��:��:��
	// ����� ����� ������ - 12 ���� + \0
	char b[4]; // ����� (255\0)
	
	// ������������ � �������� �����������
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
		
	*str++ = '\0'; // ������� ������ ������� ��������
}

u8 BcdToDec(u8 bcd) {
	// �������������� �� BCD (�������-���������� ������ Dallas RTC) � ������������ DEC
	return (((bcd & 0xF0) >> 4)*10) + (bcd & 0x0F);
}

u8 DecToBcd(u8 dec) {
	// �������������� �� DEC � BCD
	return ( (dec / 10) << 4) + (dec % 10);
}