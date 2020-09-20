/*
 * AVR-LIBS - I2C_Master
 * (c) madcatdev https://github.com/madcatdev/avr-libs
*/

#include <avr/io.h>
#include <util/twi.h>
#include "i2c_master.h"

void i2c_class::init(void){
	// ������������� �� ������ �������
	TWSR &= ~3; // ��������� = 1
	TWCR |= _BV(TWIE); // Enable TWI (22.9.2 TWCR � TWI Control Register)
	
	// ��������� ������� I2C, 22.5.2 Bit Rate Generator Unit
	TWBR = 72; // 100 ��� �� 16 ���
	//TWBR = 32; // 100 ��� �� 8 ���
	//TWBR = 12; // 100 ��� �� 4 ���	
}

void i2c_class::disable(void){
	TWCR = 0;
}

u8 i2c_class::start(u8 addr){	
	// �������� ������� START � ������ slave
	// ������ 0 � ������ ������
	TWCR = 0;	
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN) ; // �������� START condition 
	tm = 0;
	while( (!(TWCR & _BV(TWINT))) & (tm<I2C_TM_MAX) ) 
		tm++; // ���� ��������� �������� �� ��������
	if((TWSR & 0xF8) != TW_START)
		 return 1; // ���� START �� ����������, ������ 1
	
	TWDR = addr;		
	TWCR = _BV(TWINT) | _BV(TWEN); // �������� �����
	tm = 0;
	while( (!(TWCR & _BV(TWINT))) & (tm<I2C_TM_MAX) ) 
		tm++; // ���� ��������� �������� ������
		
	u8 twst = TW_STATUS & 0xF8;
	if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) ) 
		return 2; // ���������� ACK?
	
	return 0;
}

u8 i2c_class::write(u8 data){
	// �������� ����� ��� slave
	TWDR = data;
	TWCR = _BV(TWINT) | _BV(TWEN); // ��������

	tm = 0;
	while( (!(TWCR & _BV(TWINT))) & (tm<I2C_TM_MAX) ) 
		tm++; // ���� ��������� ��������	
	if( (TWSR & 0xF8) != TW_MT_DATA_ACK )
		return 1;  // ��� ���������� ACK ������ 1
	
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
	// �������� ������ ������ ������������ �����
	// �������� ������������ �� ����� � ������ _WRITE
	if (start((addr << 1) | I2C_WRITE)) // ����� 7-������, ��� ������ #0, ������� �������� � ������ ���
		return false;
	// �������� ������ ������
	for (u16 i = 0; i < len; i++){
		if (write(data[i])) { // ���� �������� ���������, �������
			stop();
			return false;
		}	
	}
	stop();
	return true;
}

bool i2c_class::receive(u8 addr, u8* data, u16 len){
	// ��������� ������ ������ ������������ �����
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
	// ������ ������� ������ � ������� �� ������ reg
	if (start((addr << 1) | I2C_WRITE)) // �������� ����� � ����� �� ������
		return false;
	write(reg); // �������� ����� ��������

	for (u16 i = 0; i < len; i++) {
		if (write(data[i])) { // � ������ ��������� ������ ����� �������
			stop();
			return false;
		}	
	}
	stop();
	return true;
}

bool i2c_class::read_reg(u8 addr, u8 reg, u8* data, u16 len){
	// ������ ������� ������ �� �������� �� ������ reg
	// ������ ����� � ����� ������ � ���������
	if (start((addr << 1) | I2C_WRITE)) 
		return false;
	write(reg);
	stop();
	
	// ����� ����� ����� ������ � ��������� ������
	if (start((addr << 1) | I2C_READ)) 
		return false;
	for (u16 i = 0; i < (len-1); i++) 
		data[i] = read_ack();
	data[(len-1)] = read_nack(); // ��������� ���� ��� ack

	stop();
	return true;
}

void i2c_class::stop(void){
	// transmit STOP condition
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO);
}

// �������� ������ ����� (��� ��������������� � uart)
#ifdef AVRLIBS_I2C0
	i2c_class i2c0;
#endif