/*
 * AVR-LIBS - SPI
 * MCU: Atmega328p 
 * Created: 08.02.2019
 * Last version: 18.09.2020
 * (c) madcatdev https://github.com/madcatdev/avr-libs

 * DESCR:
 * Hardware SPI in master mode

 * NOTES:
 * �������� ��� �� �����, ���� ����� ������ � ����� ������ (���� slave ������, �� 0xFF)
 * ������� ����� (hardware), ���������� �� ���������� slave. �������� ��� �������� ������ �����������
 * SPDR - ������� �����, TX � RX. ���� ����� � ����, �� TX, ���� ������, �� RX

 * TODO: 
 - �������� ���� ����� �������� ������ exch_array. ����� ����� ������ ������� ��� ��������, �������� �� � NULL, 0 (�������� ����������)
	��� ��� ������� ��� ���� ����� ����������
 
 */ 


#ifndef SPI_H_
#define SPI_H_

	#include <avr/io.h>
	#include "../misc/inttypes.h"
	#define AVRLIBS_SPI0 //m328p
	
	#ifdef AVRLIBS_SPI0
		// ������� ��� ��������� �������. � �������� ������ �� ������������, �.�. ���������� SPI
		#define SPI_PORTX PORTB
		#define SPI_DDRX DDRB
		#define SPI_MOSI 3
		#define SPI_MISO 4
		#define SPI_SCK 5

		// ������
		#define WAITFOR_SPIF while( !(SPSR & (1<<SPIF) ) ); // �������� ��������� �������� �����
	#endif


	class spi_class {
		public:
		// Methods
		void init_master(u8 prescaler);
		void stop_master(void);
		void choose(volatile u8 *_portx, volatile u8 *_ddrx, u8 _pinx);	
		void select(void);
		void deselect(void);
		u8 exch_u8(u8 txd);
		u8 exch_u8_sel(u8 txd);
		void exch_array(u8 *txd, u8 txdsize, u8 *rxd, u8 rxdsize);
		void recv_array(u8 *rxd, u8 rxdsize);
		void send_array(u8 *txd, u8 txdsize);
	
		private:
		// Vars
		volatile u8 *portx, *ddrx;  // ��� ������ SS/CS 
		u8 pinx;
	};

	#ifdef AVRLIBS_SPI0
		extern spi_class spi0;
	#endif

#endif /* SPI_H_ */