/*
 * AVR-LIBS - inttypes
 * MCU: Any
 * Created: 10.02.2019
 * Last version: 04.09.2020
 * (c) madcatdev https://github.com/madcatdev/avr-libs

 * DESCR:
 * ��� ��������� ���� ������ ��� ��������� ����� (������� ���� - ���������� ��������� ������ - 
 * ����� �� ������ ��� ��� uint32_t ��� �� ��� ����� unsigned long) � ��� �� ��������� ���������.
 * ���� ���� ������������ ����� ������������ avr-libs
 */ 


#ifndef INTTYPES_H_
#define INTTYPES_H_

	#include <stdint.h> // ��� �������� uint/int_xx
	
	// ������������� ����
	typedef uint8_t u8;
	typedef int8_t i8;
	typedef uint16_t u16;
	typedef int16_t i16;
	typedef uint32_t u32;
	typedef int32_t i32;
	typedef uint64_t u64;
	typedef int64_t i64;
	
	// ���������
	#define NL 0x0A // New line \n
	#define CR 0x0D // Carriage return \r

#endif /* INTTYPES_H_ */