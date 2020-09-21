/*
 * AVR-LIBS - I2C_Master
 * MCU: Atmega328p
 * Created: 10.01.2019
 * Last version: 14.09.2020
 * (c) madcatdev https://github.com/madcatdev/avr-libs

 * DESCR:
 * Hardware I2C (TWI) implementation in master mode with non-blocking timeouts
 
 * TODO:
 * autobauding � ����������� �� F_CPU
 */ 

#ifndef I2C_MASTER_H
#define I2C_MASTER_H
	
	#define AVRLIBS_I2C0
	// ����� ��� ������ � ������ �� 7-������� ������ I2C (����� ADDR_READ � ADDR_WRITE �� ������� ����)
	// ��������, ����� I2C Slave = 0x68 (1101000), ����� _READ = D1 (11010001), � _WRITE = D0 (11010000)
	#define I2C_READ 0x01
	#define I2C_WRITE 0x00
	// ������� �������� ������ �� Slave
	#define I2C_TM_MAX 0x0FFF
	
	#include "../misc/inttypes.h"
	#include <stdint.h>

	class i2c_class{
		public:
		void init(void);
		void disable(void);
		u8 start(u8 address);
		u8 write(u8 data);
		u8 read_ack(void);
		u8 read_nack(void);
		bool transmit(u8 addr, u8* data, u16 len);
		bool receive(u8 addr, u8* data, u16 len);
		bool write_reg(u8 addr, u8 reg, u8* data, u16 len);
		bool read_reg(u8 addr, u8 reg, u8* data, u16 len);
		void stop(void);
		private: 
		u16 tm; //Timeout varivale
	};

	// ���������� ���������� �� ��������� �����
	#ifdef AVRLIBS_I2C0
		extern i2c_class i2c0;
	#endif
#endif // I2C_MASTER_H
