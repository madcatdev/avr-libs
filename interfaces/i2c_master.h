/*
 * AVR-LIBS - I2C_Master
 * MCU: Atmega328p
 * Created: 10.01.2019
 * Last version: 14.09.2020
 * (c) madcatdev https://github.com/madcatdev/avr-libs

 * DESCR:
 * Hardware I2C (TWI) implementation in master mode with non-blocking timeouts
 
 * TODO:
 * autobauding в зависимости от F_CPU
 */ 

#ifndef I2C_MASTER_H
#define I2C_MASTER_H
	
	#define AVRLIBS_I2C0
	// Маска для чтения и записи по 7-битному адресу I2C (когда ADDR_READ и ADDR_WRITE не указаны явно)
	// Допустим, адрес I2C Slave = 0x68 (1101000), тогда _READ = D1 (11010001), а _WRITE = D0 (11010000)
	#define I2C_READ 0x01
	#define I2C_WRITE 0x00
	// Таймаут ожидания данных от Slave
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

	// Глобальная переменная во вложенном файле
	#ifdef AVRLIBS_I2C0
		extern i2c_class i2c0;
	#endif
#endif // I2C_MASTER_H
