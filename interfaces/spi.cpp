/*
 * AVR-LIBS - SPI
 * (c) madcatdev https://github.com/madcatdev/avr-libs
 */

#include "spi.h"

void spi_class::init_master(u8 prescaler) {
	// ������������� ����������� SPI 
	// TODO ���� �������� �� ������ �������� ������, �� �������� ������� autobaud
	//��������� IO,	��� �� OUT � HIGH, ����� MISO (��� � ���������)
	SPI_DDRX |= _BV(SPI_MOSI) | _BV(SPI_SCK); // Direction
	SPI_PORTX |= _BV(SPI_MOSI) | _BV(SPI_SCK) | _BV(SPI_MISO); // Level
	
	// SPCR: ������� SPI, ������� ��� ������, ������, �����_0, 
	SPCR = (1<<SPE) | (0<<DORD) | (1<<MSTR) | (0<<CPOL) | (0<<CPHA);

	// Atmega328.pdf, page 168 -Table 19-5. ���������� ���� �������� SPCR �������� ���������� ����������
	// ��� 8 ��� Fspi 125 ���, P = 64, ��� 16 ��� Fspi 125 ���, P = 128
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
		case 64 :   // 010 ��� 111
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
	// ���������� �����.���������� SPIx
	SPCR = 0;
	
	// ���������� SPI ����� (������� � hi-z)
	SPI_DDRX &= ~( _BV(SPI_MOSI) | _BV(SPI_SCK) ); // Direction
	SPI_PORTX &= ~( _BV(SPI_MOSI) | _BV(SPI_SCK) | _BV(SPI_MISO) ); // Level
}

void spi_class::choose(volatile u8 *_portx, volatile u8 *_ddrx, u8 _pinx) {
	// ��������� ����� �����-������ (Chip/Slave Select) �� �����
	// ���������� � ��������� ���� ������
	portx = _portx;
	ddrx = _ddrx;
	pinx = _pinx;

	// ��������� ���������
	*ddrx |= _BV(pinx); // �� �����
	*portx |= _BV(pinx); // �� ������� �������
}

void spi_class::select(void) {
	*portx &= ~_BV(pinx); // SS/CS � ������ �������
}

void spi_class::deselect(void) {
	*portx |= _BV(pinx); // SS/CS � ������� �������
}

u8 spi_class::exch_u8(u8 txd){
	// �������� 1 ����, ��������� 1 ����, NO CHIP SELECT
	SPDR = txd;  // ������ ���� � ����� ��������, �������� ��
	WAITFOR_SPIF;  // ������� ��������� �������� (� ��� �� ����� ��������� �����)
	return SPDR; // ������ ���� �� ������ ������
}

u8 spi_class::exch_u8_sel(u8 txd){
	// �������� 1 ����, ��������� 1 ����, CHIP SELECT
	select();
	SPDR = txd;
	WAITFOR_SPIF;
	return SPDR;
	deselect();
}

void spi_class::exch_array(u8 *txd, u8 txdsize, u8 *rxd, u8 rxdsize){
	/*
	�������� ������� ����� ����������� � ������������� ����� ������� ������ ����������� (����� ������� ������, ��� ���� ����������)
	� ����� ������ ��� ������ �� ������� ������� txd ����� ������������ 0xFF. ����� �� ������� ������� rxd ����������.
	������� �������� ��� UBX, ������� �������� ������ ��������������� ��� poll-�������, ��� ���� poll ����� �����������.
	*/
	u8 x; // ������� ���� ��� ��������
	u8 i = 0; // ������ ������ �� ������� ��������
	u8 j = 0; // ������ ������ � ������ ������
	
	select();
	
	// ��������� �������� ��� ������ (�� ������ �� ������� ������� txd) � ��� �� ��������� �����
	while (i < rxdsize) {
		// ������������ � ���, ��� ����������
		if (i > (txdsize-1) ) 
			x = 0xFF; // ������ �������� ��������, ��������� FF
		else 
			x = txd[i];
		
		SPDR = x; // ��������
		WAITFOR_SPIF;
		rxd[j] = SPDR; // ������

		j++;
		i++;
	}
		
	deselect();
	/*
	// ������ ������������� (�������� 8 ����, ��������� 37):
	uint8_t txd[8] = {0xB5, 0x62, 0x01, 0x02, 0x00, 0x00, 0x03, 0x0A};  // ������
	uint8_t rxd[37]; // �������� �����, ������� ����.������ 
	exch_array(txd,8,rxd,37);
	*/
}

void spi_class::send_array(u8 *txd, u8 txdsize){
	// �������� ������� ���� ������� NUM, ���������� ������ ����������
	select();
	while(txdsize--){
		SPDR = *txd++;
		WAITFOR_SPIF;
	}
	deselect();

	/*
	// ������ �������������:
	uint8_t buf[3] = {12, 43, 98};
	send_array(buf, 3);
	*/
}

void spi_class::recv_array(u8 *rxd, u8 rxdsize){
	// ����� ������� �������� rxdsize (��������� ������� ���-��, ������ �� ��������� �� �� �����, ���� 0xFF)
	u8 i = 0; // ������ ������ � ������ ������
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