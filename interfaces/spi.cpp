/*
 * AVR-LIBS - SPI
 * (c) madcatdev https://github.com/madcatdev/avr-libs
 */

#include "spi.h"

void spi_class::init_master(u8 prescaler) {
	// Инициализация аппаратного SPI 
	// TODO если работать на разных частотах кварца, то придется сделать autobaud
	//Настройка IO,	все на OUT и HIGH, кроме MISO (оно с подтяжкой)
	SPI_DDRX |= _BV(SPI_MOSI) | _BV(SPI_SCK); // Direction
	SPI_PORTX |= _BV(SPI_MOSI) | _BV(SPI_SCK) | _BV(SPI_MISO); // Level
	
	// SPCR: Включим SPI, Старший бит вперед, Мастер, Режим_0, 
	SPCR = (1<<SPE) | (0<<DORD) | (1<<MSTR) | (0<<CPOL) | (0<<CPHA);

	// Atmega328.pdf, page 168 -Table 19-5. Выставляем биты регистра SPCR согласно выбранному прескалеру
	// Для 8 МГц Fspi 125 Кгц, P = 64, Для 16 МГц Fspi 125 Кгц, P = 128
	switch (prescaler){

		// SPI2X SPR1 SPR0
		case 2 :   // 100
			SPCR |= _BV(SPI2X);
			break;
		case 4 :   // 000
			break;
		case 8 :   // 101
			SPCR |= _BV(SPI2X) | _BV(SPR0);
			break;
		case 16 :  // 001
			SPCR |= _BV(SPR0);
			break;
		case 32 :   // 110
			SPCR |= _BV(SPI2X) | _BV(SPR1);
			break;		
		case 64 :   // 010 или 111
			SPCR |= _BV(SPI2X) | _BV(SPR1) | _BV(SPR0);
			break;
		case 128 :   // 011
			SPCR |= _BV(SPR1) | _BV(SPR0);
			break;
		default :  // 128
			SPCR |= _BV(SPR1) | _BV(SPR0);
	} // end switch

}

void spi_class::stop_master(void){
	// Отключение периф.устройства SPIx
	SPCR = 0;
	
	// Отключение SPI линий (перевод в hi-z)
	SPI_DDRX &= ~( _BV(SPI_MOSI) | _BV(SPI_SCK) ); // Direction
	SPI_PORTX &= ~( _BV(SPI_MOSI) | _BV(SPI_SCK) | _BV(SPI_MISO) ); // Level
}

void spi_class::choose(volatile u8 *_portx, volatile u8 *_ddrx, u8 _pinx) {
	// Настройка порта ввода-вывода (Chip/Slave Select) на выход
	// Записываем в приватные поля класса
	portx = _portx;
	ddrx = _ddrx;
	pinx = _pinx;

	// Установка регистров
	*ddrx |= _BV(pinx); // На выход
	*portx |= _BV(pinx); // На высокий уровень
}

void spi_class::select(void) {
	*portx &= ~_BV(pinx); // SS/CS в низкий уровень
}

void spi_class::deselect(void) {
	*portx |= _BV(pinx); // SS/CS в высокий уровень
}

u8 spi_class::exch_u8(u8 txd){
	// Передаем 1 байт, принимаем 1 байт, NO CHIP SELECT
	SPDR = txd;  // Кладем байт в буфер передачи, запуская ее
	WAITFOR_SPIF;  // Ожидаем окончания передачи (в это же время состоится прием)
	return SPDR; // Читаем байт из буфера приема
}

u8 spi_class::exch_u8_sel(u8 txd){
	// Передаем 1 байт, принимаем 1 байт, CHIP SELECT
	select();
	SPDR = txd;
	WAITFOR_SPIF;
	return SPDR;
	deselect();
}

void spi_class::exch_array(u8 *txd, u8 txdsize, u8 *rxd, u8 rxdsize){
	/*
	Отправка массива одной размерности и одновременный прием массива другой размерности (можно принять больше, чем было отправлено)
	В таком случае при выходе за пределы массива txd будет передаваться 0xFF. Выйти за пределы массива rxd невозможно.
	Функция заточена под UBX, который передает данные непосредственно при poll-запросе, при этом poll может сокращенным.
	*/
	u8 x; // Текущий байт для отправки
	u8 i = 0; // Индекс чтения из массива передачи
	u8 j = 0; // Индекс записи в массив приема
	
	select();
	
	// Побайтово передаем наш запрос (не выходя за пределы массива txd) и тут же принимаем ответ
	while (i < rxdsize) {
		// Определяемся с тем, что передавать
		if (i > (txdsize-1) ) 
			x = 0xFF; // Массив передачи кончился, заполняем FF
		else 
			x = txd[i];
		
		SPDR = x; // Передача
		WAITFOR_SPIF;
		rxd[j] = SPDR; // Чтение

		j++;
		i++;
	}
		
	deselect();
	/*
	// Пример использования (передаем 8 байт, принимаем 37):
	uint8_t txd[8] = {0xB5, 0x62, 0x01, 0x02, 0x00, 0x00, 0x03, 0x0A};  // Запрос
	uint8_t rxd[37]; // Приемный буфер, ожидаем фикс.размер 
	exch_array(txd,8,rxd,37);
	*/
}

void spi_class::send_array(u8 *txd, u8 txdsize){
	// Отправка массива байт размера NUM, приходящие данные игнорируем
	select();
	while(txdsize--){
		SPDR = *txd++;
		WAITFOR_SPIF;
	}
	deselect();

	/*
	// Пример использования:
	uint8_t buf[3] = {12, 43, 98};
	send_array(buf, 3);
	*/
}

void spi_class::recv_array(u8 *rxd, u8 rxdsize){
	// Прием массива размером rxdsize (поскольку принять что-то, ничего не отправляя мы не можем, шлем 0xFF)
	u8 i = 0; // Индекс записи в массив приема
	select();

	while (i < rxdsize) {		
		SPDR = 0xFF; 
		WAITFOR_SPIF;
		rxd[i] = SPDR; 
		i++;
	}
		
	deselect();
}

#ifdef AVRLIBS_SPI0
	spi_class spi0;
#endif