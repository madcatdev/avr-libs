/*
 * AVR-LIBS - SPI
 * MCU: Atmega328p 
 * Created: 08.02.2019
 * Last version: 18.09.2020
 * (c) madcatdev https://github.com/madcatdev/avr-libs

 * DESCR:
 * Hardware SPI in master mode

 * NOTES:
 * Таймауты тут не нужны, байт будет принят в любом случае (если slave молчит, то 0xFF)
 * Бодрейт общий (hardware), независимо от количества slave. Задается при создании класса прескалером
 * SPDR - двойной буфер, TX и RX. Если пишем в него, то TX, если читаем, то RX

 * TODO: 
 - возможно есть смысл оставить только exch_array. Когда нужно только принять или передать, вызывать ее с NULL, 0 (придется переделать)
	Все три функции при этом можно объединить
 
 */ 


#ifndef SPI_H_
#define SPI_H_

	#include <avr/io.h>
	#include "../misc/inttypes.h"
	#define AVRLIBS_SPI0 //m328p
	
	#ifdef AVRLIBS_SPI0
		// Дефайны для настройки выводов. В передаче данных не используется, т.к. аппаратный SPI
		#define SPI_PORTX PORTB
		#define SPI_DDRX DDRB
		#define SPI_MOSI 3
		#define SPI_MISO 4
		#define SPI_SCK 5

		// Прочее
		#define WAITFOR_SPIF while( !(SPSR & (1<<SPIF) ) ); // Ожидание окончания передачи байта
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
		volatile u8 *portx, *ddrx;  // Для вывода SS/CS 
		u8 pinx;
	};

	#ifdef AVRLIBS_SPI0
		extern spi_class spi0;
	#endif

#endif /* SPI_H_ */