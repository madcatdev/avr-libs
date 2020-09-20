/*
 * AVR-LIBS - Strings
 * MCU: Any
 * Created: 02.08.2019
 * Last version: 08.09.2020
 * (c) madcatdev https://github.com/madcatdev/avr-libs

 * DESCR:
 * Custom text operations

 * TODO:

 */ 

#ifndef STRING_H_
#define STRING_H_

	#include "inttypes.h"
	#include <stdlib.h> // utoa
	#include <string.h> // strlen, memcpy
	
	// Лингвистические операции
	u8 str_pos(const char *str, const char *substr, u8 start);
	bool str_compare(const char *str1, const char *str2);
	void str_copy(const char *str_from, char *str_to, u8 start, u8 len);
	void str_add(char *str, const char *substr);
	u8 str_len(const char *str);
	
	// Конвертирование
	u8 ascii_to_val(const char a);
	char val_to_ascii(u8 x);
	u8 hex_to_u8(const char *str);
	void u8_to_hex(const u8 x, char *str);
	void u8_to_str_z(char *str, u8 x);
	void n32_to_strfloat(char *str, u32 num, u8 left_len, u8 right_len, bool is_signed);

#endif /* STRING_H_ */