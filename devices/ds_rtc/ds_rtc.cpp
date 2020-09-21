/*
 * AVR-LIBS - DS_RTC
 * (c) madcatdev https://github.com/madcatdev/avr-libs
 */ 
#include "ds_rtc.h"

ds_rtc_class::ds_rtc_class (i2c_class &i2c_device) {
	// ����������� ������ � ����������� ���������� I2C
	i2c = &i2c_device;
}

bool ds_rtc_class::init_ds3121(void){
	// ������������� RTC (����� �� ����� �������� �� �������, ��� ������ ��������� ���������� ������ ��� - ����� �� ����������)
	u8 data[2] = {0b00100000,0b00001000}; // ������ ��������� �����������, ��� ����� 1 ��, ��� ����� 32 ���
	return i2c->write_reg(RTC_ADDR, 0x0E, data, 2); // Control Register (0Eh)           
}

bool ds_rtc_class::init_ds1307(void){
	// ������������� RTC
	// ������ ������� �������� ������ �� �������� 00h
	u8 sec_bcd = 0;
	if (!i2c->read_reg(RTC_ADDR, 0x00, &sec_bcd, 1))
		return false;
	// ��������� ��� CH � 0 � ������� ������� � ����������
	sec_bcd = sec_bcd & 0x7F; 
	return i2c->write_reg(RTC_ADDR, 0x00, &sec_bcd, 1); 
}

bool ds_rtc_class::read_time(time_struct &t){
	// �������� ��� DS3231 � DS1307
	u8 data[7];
	if (!i2c->read_reg(RTC_ADDR, 0x00, data, 7))
		return false;  
	
	t.sec = BcdToDec(data[0]);
	t.min = BcdToDec(data[1]);      
	t.hour = BcdToDec(data[2]);     
	t.wday = BcdToDec(data[3]);     
	t.day = BcdToDec(data[4]);     
	t.month = BcdToDec(data[5]);    
	t.year = BcdToDec(data[6]); 
	return true;              
}
	
bool ds_rtc_class::write_time(time_struct &t){
	// ��������� �������
	// �������� ��� DS3231 � DS1307
	u8 data[7] = {DecToBcd(t.sec), DecToBcd(t.min), DecToBcd(t.hour), DecToBcd(t.wday),
		DecToBcd(t.day), DecToBcd(t.month), DecToBcd(t.year)};
	return i2c->write_reg(RTC_ADDR, 0x00, data, 7);
}

bool ds_rtc_class::read_temp(i16 &temp){
	/*
	DS3231 specific, DS1307 has no temp sensor
	������ �����������, 10-bit (0.25 C), ����� � ����� ����� ������� , MAX/MIN = +-128.75 �
	Temp registers (0x11, 0x12) structure:
	01111111 01000000	= 128.25 C
	SDDDDDDD FF------	S - sign, D - whole degrees value (MSB), F - float degrees value (LSB)									    
	*/
	u8 data[2]; // TEMP_MSB, TEMP_LSB
	
	if (i2c->read_reg(RTC_ADDR, 0x11, data, 2)) {
		temp = i8(data[0]) * 100; // ����������� ����� �����
		data[1] = (data[1] >> 6) * 25; // ����������� ������� ����� 
		if (temp > 0) // � ����������� �� ����� �������� ��� ���������� ������� �����
			temp += data[1];
		else
			temp -= data[1];
		return true;
	} else
		return false;
}
