/*
 * AVR-LIBS - eeprom_16
 * MCU: Atmega328p (1024 B EEPROM)
 * Created: 11.02.2019
 * Last version: 04.09.2020
 * (c) madcatdev https://github.com/madcatdev/avr-libs

 * DESCR:
 * Реализация работы с 16-битными (10-битными в m328p) адресами. Стандартная либа от avr поддерживает только 8-битные

 * NOTES:
 * 1. Запись в EEPROM не может производиться одновременно с записью во FLASH. Данная библиотека это не проверяет.

 * TODO:
 *
 */ 


#ifndef EEPROM_16_H_
#define EEPROM_16_H_

	#include "../misc/inttypes.h"
	#include <avr/io.h>
	#include <avr/interrupt.h>
		
	#define EEPE_WAIT while(EECR & (1 << EEPE)); // Ожидание установки бита EEPROM Write Enable

	u8 eeprom_read_u8(u16 addr);
	void eeprom_write_u8(u16 addr, u8 data);
	void eeprom_update_u8(u16 addr, u8 data);

	void eeprom_read_arr(u16 addr, u8 len, u8 *data);
	void eeprom_update_arr(u16 addr, u8 len, const u8 *data);

	void eeprom_read_str(u16 addr, u8 len, char *str);
	void eeprom_update_str(u16 addr, u8 len, const char *str);

	void eeprom_erase(u16 addr, u16 len);

#endif /* EEPROM_16_H_ */