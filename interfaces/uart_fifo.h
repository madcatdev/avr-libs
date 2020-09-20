/*
 * AVR-LIBS - UART with FIFO
 * MCU: Atmega328p
 * Created: 03.02.2019
 * Last version: 10.09.2020
 * (c) madcatdev https://github.com/madcatdev/avr-libs
 
 * DESCR:
 * UART with FIFO buffer and interrupts
 
 * NOTES: 
 
 * TODO:
 1. Убрать привязку к МК. Можно через #ifdef madcatdev_m328p ...#endif указывать нужные регистры при компиляции
 Или же объявлять регистры при явном указании интерфейса (UARTn)
 */ 

#ifndef UART_FIFO_H_
#define UART_FIFO_H_
	// Standart
	#include <stdint.h>
	#include <avr/interrupt.h> 
	#include <avr/io.h>
	#include <stdlib.h> // itoa/ultoa
	
	// madcatdev AVR Libraries
	#include "../misc/inttypes.h"
	#include "../misc/string.h"

	#define FIFO_SIZE 128
	#define AVRLIBS_UART0 //m328p

	struct fifo_struct	{
		char b[FIFO_SIZE]; // Сам буфер
		volatile u8 head;
		volatile u8 tail;
		volatile u8 count;
		volatile bool ovf;
		volatile u8 ln_count;
		};
	
	class uart_fifo_class {
		public:
		// Параметры
		fifo_struct txb;
		fifo_struct rxb;
	
		// Service
		void init(u32 clock, u32 baud);
		void stop(void);
		//void tx(void);
		//void rx(void);
		void flush_all (void);
		
		// Write (Transmit)
		void write_pu8 (u8 &b);
		void write_u8 (u8 b);
		void write(const char *b);
		void writeln(const char *b);
		void write_hex (u8 b);
		void write_hexarray (u8 *b, u8 len);
		void write_hexstr (const char *b, u8 len);
	
		// Read (Receive)
		u8 read_u8 (void);
		bool read(char *b, u8 len);
		bool readln(char *b, u8 len);
	
		private:
		void flush_buf (fifo_struct &f);
	};

	// Внеклассовы функции, особенность работы с прерываниями
	void uart_fifo_tx(uart_fifo_class &uart);
	void uart_fifo_rx(uart_fifo_class &uart);
	
	#ifdef AVRLIBS_UART0
		extern uart_fifo_class uart0;
	#endif
#endif /* UART_FIFO_H_ */