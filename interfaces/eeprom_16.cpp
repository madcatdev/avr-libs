/*
 * AVR-LIBS - eeprom_16
 * (c) madcatdev https://github.com/madcatdev/avr-libs
 */ 

#include "eeprom_16.h"

void eeprom_write_u8(u16 addr, u8 data){
	// Запись 1 байта в EEPROM. Адрес 8 бит, прерывания не отключаются!
	EEPE_WAIT;
	EEAR = addr; 
	EEDR = data; // Кладем байт в буфер
	cli();
	EECR |= 1 << EEMPE; // Разрешение записи
	EECR |= 1 << EEPE; //Запуск записи. Не более 4-х тактов после предыдущей команды!!!
	sei();
}

u8 eeprom_read_u8(u16 addr){
	// Чтение 1 байта из EEPROM. Адрес 8 бит, прерывания не отключаются!
	EEPE_WAIT;
	EEAR = addr;
	EECR |= (1 << EERE); // Запуск чтения
	return EEDR;
}

void eeprom_update_u8(u16 addr,  u8 data){
	// Обновление значения в ячейке (запись только если есть отличия)
	if (eeprom_read_u8(addr) != data)
		eeprom_write_u8(addr, data);
}

void eeprom_read_arr(u16 addr, u8 len, u8 *data) {
	// Чтение массмва из EEPROM в буфер data, на длину len 
	while (len--) 
		*data++ = eeprom_read_u8(addr++); // Читаем посимвольно
}

void eeprom_update_arr(u16 addr, u8 len, const u8 *data){
	u8 x;
	while (len--) {
		x = *data++;
		if (eeprom_read_u8(addr) != x) // Если байт не совпал, пишем его
			eeprom_write_u8(addr, x);
		addr++; // Смещаемся на следующую ячейку
	}
}

void eeprom_read_str(u16 addr, u8 len, char *str) {
	// Чтение строки из EEPROM в буфер str, на длину len (
	// ВНИМАНИЕ! Длина str должна быть len+1, иначе будет запись за границы массива
	while (len--) 
		*str++ = eeprom_read_u8(addr++); // Читаем посимвольно
	*str = 0; // Терминируем
}

void eeprom_update_str(u16 addr, u8 len, const char *str) {
	// Запись строки из буфера str в EEPROM на длину len
	// \0 не пишем
	
	// Пока не дошли до 0-го символа
	while (len-- && *str) 
		eeprom_update_u8(addr++, *str++);
	
	// Если строка кончилась, а длина еще осталась
	while (len--)
		eeprom_update_u8(addr++, 0); // Забиваем нулями	
}

void eeprom_erase(u16 addr, u16 len){
	// Затираем данные единицами (0xFF) - состояние EEPROM по умолчанию (после стирания через Chip Erase будет так же)
	while (len--) 
		eeprom_update_u8(addr++, 0xFF);
}