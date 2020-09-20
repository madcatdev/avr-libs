/*
 * AVR-LIBS - eeprom_16
 * (c) madcatdev https://github.com/madcatdev/avr-libs
 */ 

#include "eeprom_16.h"

void eeprom_write_u8(u16 addr, u8 data){
	// ������ 1 ����� � EEPROM. ����� 8 ���, ���������� �� �����������!
	EEPE_WAIT;
	EEAR = addr; 
	EEDR = data; // ������ ���� � �����
	cli();
	EECR |= 1 << EEMPE; // ���������� ������
	EECR |= 1 << EEPE; //������ ������. �� ����� 4-� ������ ����� ���������� �������!!!
	sei();
}

u8 eeprom_read_u8(u16 addr){
	// ������ 1 ����� �� EEPROM. ����� 8 ���, ���������� �� �����������!
	EEPE_WAIT;
	EEAR = addr;
	EECR |= (1 << EERE); // ������ ������
	return EEDR;
}

void eeprom_update_u8(u16 addr,  u8 data){
	// ���������� �������� � ������ (������ ������ ���� ���� �������)
	if (eeprom_read_u8(addr) != data)
		eeprom_write_u8(addr, data);
}

void eeprom_read_arr(u16 addr, u8 len, u8 *data) {
	// ������ ������� �� EEPROM � ����� data, �� ����� len 
	while (len--) 
		*data++ = eeprom_read_u8(addr++); // ������ �����������
}

void eeprom_update_arr(u16 addr, u8 len, const u8 *data){
	u8 x;
	while (len--) {
		x = *data++;
		if (eeprom_read_u8(addr) != x) // ���� ���� �� ������, ����� ���
			eeprom_write_u8(addr, x);
		addr++; // ��������� �� ��������� ������
	}
}

void eeprom_read_str(u16 addr, u8 len, char *str) {
	// ������ ������ �� EEPROM � ����� str, �� ����� len (
	// ��������! ����� str ������ ���� len+1, ����� ����� ������ �� ������� �������
	while (len--) 
		*str++ = eeprom_read_u8(addr++); // ������ �����������
	*str = 0; // �����������
}

void eeprom_update_str(u16 addr, u8 len, const char *str) {
	// ������ ������ �� ������ str � EEPROM �� ����� len
	// \0 �� �����
	
	// ���� �� ����� �� 0-�� �������
	while (len-- && *str) 
		eeprom_update_u8(addr++, *str++);
	
	// ���� ������ ���������, � ����� ��� ��������
	while (len--)
		eeprom_update_u8(addr++, 0); // �������� ������	
}

void eeprom_erase(u16 addr, u16 len){
	// �������� ������ ��������� (0xFF) - ��������� EEPROM �� ��������� (����� �������� ����� Chip Erase ����� ��� ��)
	while (len--) 
		eeprom_update_u8(addr++, 0xFF);
}