/*
 * AVR-LIBS - I2C_Master
 * (c) madcatdev https://github.com/madcatdev/avr-libs
*/

#include <avr/io.h>
#include <util/twi.h>
#include "i2c_master.h"

void i2c_class::init(void){
	// Инициализация на нужную частоту
	TWSR &= ~3; // Прескалер = 1
	TWCR |= _BV(TWIE); // Enable TWI (22.9.2 TWCR – TWI Control Register)
	
	// Настройка частоты I2C, 22.5.2 Bit Rate Generator Unit
	TWBR = 72; // 100 КГц на 16 МГц
	//TWBR = 32; // 100 КГц на 8 МГц
	//TWBR = 12; // 100 КГц на 4 МГц	
}

void i2c_class::disable(void){
	TWCR = 0;
}

u8 i2c_class::start(u8 addr){	
	// Передача условия START и адреса slave
	// Вернет 0 в случае успеха
	TWCR = 0;	
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN) ; // Передаем START condition 
	tm = 0;
	while( (!(TWCR & _BV(TWINT))) & (tm<I2C_TM_MAX) ) 
		tm++; // Ждем окончания передачи по таймауту
	if((TWSR & 0xF8) != TW_START)
		 return 1; // Если START не передалось, вернем 1
	
	TWDR = addr;		
	TWCR = _BV(TWINT) | _BV(TWEN); // Передаем адрес
	tm = 0;
	while( (!(TWCR & _BV(TWINT))) & (tm<I2C_TM_MAX) ) 
		tm++; // Ждем окончания передачи адреса
		
	u8 twst = TW_STATUS & 0xF8;
	if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) ) 
		return 2; // Отсутствие ACK?
	
	return 0;
}

u8 i2c_class::write(u8 data){
	// Передача байта для slave
	TWDR = data;
	TWCR = _BV(TWINT) | _BV(TWEN); // Передаем

	tm = 0;
	while( (!(TWCR & _BV(TWINT))) & (tm<I2C_TM_MAX) ) 
		tm++; // Ждем окончания передачи	
	if( (TWSR & 0xF8) != TW_MT_DATA_ACK )
		return 1;  // При отсутствии ACK вернем 1
	
	return 0;
}

u8 i2c_class::read_ack(void){
	// Receiving DATA+ACK from slave
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA); 
	tm = 0;
	while( (!(TWCR & _BV(TWINT))) & (tm<I2C_TM_MAX) ) 
		tm++;
	return TWDR;
}

u8 i2c_class::read_nack(void){
	// Receiving DATA from Slave (without ACK)
	TWCR = _BV(TWINT) | _BV(TWEN);
	tm = 0;
	while( (!(TWCR & _BV(TWINT))) & (tm<I2C_TM_MAX) ) 
		tm++;
	return TWDR;
}

bool i2c_class::transmit(u8 addr, u8* data, u16 len){
	// Передаем массив данных определенной длины
	// Пытаемся подключиться на адрес с маской _WRITE
	if (start((addr << 1) | I2C_WRITE)) // Адрес 7-битный, бит записи #0, поэтому сдвигаем и делаем ИЛИ
		return false;
	// Передаем массив данных
	for (u16 i = 0; i < len; i++){
		if (write(data[i])) { // Если передача неуспешна, выходим
			stop();
			return false;
		}	
	}
	stop();
	return true;
}

bool i2c_class::receive(u8 addr, u8* data, u16 len){
	// Принимаем массив данных определенной длины
	if (start((addr << 1) | I2C_READ)) 
		return false;
	
	for (u16 i = 0; i < (len-1); i++) {
		data[i] = read_ack();
	}
	data[(len-1)] = read_nack();
	
	stop();
	return true;
}

bool i2c_class::write_reg(u8 addr, u8 reg, u8* data, u16 len){
	// Запись массива данных в регистр по адресу reg
	if (start((addr << 1) | I2C_WRITE)) // Передаем адрес с битом на запись
		return false;
	write(reg); // Передаем адрес регистра

	for (u16 i = 0; i < len; i++) {
		if (write(data[i])) { // В случае неудачной записи байта выходим
			stop();
			return false;
		}	
	}
	stop();
	return true;
}

bool i2c_class::read_reg(u8 addr, u8 reg, u8* data, u16 len){
	// Чтение массива данных из регистра по адресу reg
	// Сперва адрес с битом чтения и регистром
	if (start((addr << 1) | I2C_WRITE)) 
		return false;
	write(reg);
	stop();
	
	// Затем опять адрес чтения и принимаем массив
	if (start((addr << 1) | I2C_READ)) 
		return false;
	for (u16 i = 0; i < (len-1); i++) 
		data[i] = read_ack();
	data[(len-1)] = read_nack(); // Последний байт без ack

	stop();
	return true;
}

void i2c_class::stop(void){
	// transmit STOP condition
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO);
}

// Создание класса здесь (для единообразности с uart)
#ifdef AVRLIBS_I2C0
	i2c_class i2c0;
#endif