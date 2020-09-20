/*
 * AVR-LIBS - UART with FIFO
 * (c) madcatdev https://github.com/madcatdev/avr-libs
 */ 
#include "uart_fifo.h"

void uart_fifo_class::init(u32 clock, u32 baud)	{
	// ��������� ��������
	u16 _ubrr = clock/16/baud-1;
	UBRR0H = (u8)(_ubrr>>8);
	UBRR0L = (u8)_ubrr;
	// �������� �������� � ����������
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	// ��������� ������� �����: 8data, 2stop. ��������!!
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
	// �������� ���������� �� ������ ����� (��� FIFO)
	UCSR0B |= (1<<RXCIE0); // Receive Complete Interrupt 
}

void uart_fifo_class::stop(void) {
	// ��������� (����������) UART, ���������� ����������
	UCSR0B &= ~(1<<RXEN0);
	UCSR0B &= ~(1<<TXEN0);
	UCSR0B &= ~(1<<RXCIE0); // ����������
}

void uart_fifo_class::flush_all (void) {
	// ������ � �������� ������ c ������� (USER <--> FIFO)
	// ������� ���� �������� �������
	flush_buf(txb);
	flush_buf(rxb);
}

void uart_fifo_class::flush_buf (fifo_struct &f) {
	// ������� ������
	f.head = 0;
	f.tail = 0;
	f.count = 0;
	f.ln_count = 0;
	f.ovf = false;
}

void uart_fifo_class::write_pu8 (u8 &b) {
	// ������ 1 ���� � FIFO � �������� ���������� �� ������� ��������� ������
	if (txb.count < FIFO_SIZE) { // ���� ���� �����
		cli();
		txb.b[txb.tail] = b;
		txb.count++;
		txb.tail++;
		if (txb.tail == FIFO_SIZE) 
			txb.tail = 0; 
		sei();
	}
	
	UCSR0B |= (1<<UDRIE0); // Data Register Empty Interrupt Enable
}

void uart_fifo_class::write_u8 (u8 b) {
	// ������ 1 ���� � FIFO � �������� ���������� �� ������� ��������� ������
	// �������� ����� ��������, ��� ������
	if (txb.count < FIFO_SIZE) { // ���� ���� �����
		cli();
		txb.b[txb.tail] = b;
		txb.count++;
		txb.tail++;
		if (txb.tail == FIFO_SIZE) 
			txb.tail = 0;
		sei();
	}
	
	UCSR0B |= (1<<UDRIE0); // Data Register Empty Interrupt Enable
}

void uart_fifo_class::write(const char *b){
	// ������ � FIFO_TX ������
	// ��������, ���� �� �������� \0
	while(*b)
		write_u8(*b++);
}

void uart_fifo_class::writeln(const char *b){
	// ������ � FIFO_TX ������, � \n (0x0A) � �����
	// ��������, ���� �� �������� \0
	while(*b)
		write_u8(*b++);
	write_u8(NL); // Line feed, ������� ������, \n
}

u8 uart_fifo_class::read_u8 (void) {
	// �������� 1 ���� �� ��������� FIFO
	u8 x = 0;
	
	cli();
	
	if (rxb.count > 0){
		x = rxb.b[rxb.head];
		rxb.count--;
		rxb.head++;
		if (rxb.head == FIFO_SIZE) 
			rxb.head = 0;	
		if (x == NL) 
			rxb.ln_count--; // ��������� ������� �����
	}
	
	sei();
	
	return x;
}

bool uart_fifo_class::read(char *b, u8 len) {
	// �������� ������ �� ����� FIFO, �� �� ������� ��� len
	u8 x = 0; 
		//len--; // ��������� ������ ����������� �������� 
	while (rxb.count && len) { // ��������� �� ����� FIFO, ���� �� ����� ������ len
		b[x] = read_u8(); // ������ ������ � ������
		len--;
		if (len) x++; // ��������� �� ������, ���� ��������� �����
		// ���� �� ���������, �������� �������� ��� �� �������� �� (�������� ��-�� ������ 0x00 ��� ����� ����)
	}
	b[x] = 0; // �����������
	
	if (x > 0) 
		return true; 
	else 
		return false;	

}

bool uart_fifo_class::readln(char *b, u8 len) {
	// �������� ������ �� ������� 0x0A, �� �� ������� ��� len
	
	if (rxb.ln_count || rxb.ovf) { // ���� ���� ������ (������� LN) ���� ������������
		u8 x = 0; 
		//len--; // ��������� ������ ����������� �������� 
		while ((x != NL) && rxb.count && len) { // ��������� �� ������� LN ����, ���� �� ����� FIFO, ���� �� ����� ������ len
			x = read_u8();
			// ���� � ��� ������ \r (0x0D), ������ ���� ����� \0
			if (x == CR) 
				x = 0;
			//*b++ = x;
			*b = x; // ������ ������ � ������
			len--;
			if (len) // ��������� �� ������, ���� ��������� �����
				b++; 
			// ���� �� ���������, �������� �������� ��� �� �������� �� (�� ��-�� ������ 0x00 ��� ����� ����)
		}
		*--b = 0; // ��������, ����� �� ��� ����� (������ 0 ������ 0x0A
		return true;
	} else 
		return false; // ���� �����
	
}

void uart_fifo_class::write_hex (u8 b) {
	// ����� ����, ��������� ��� � HEX � ������ � ���� (2 �����)
	char str[3]; // F+F+\0
	u8_to_hex(b, str);
	if (str[1] == 0) { // ���� ������ 1 �������, ������ 0 ����� ���
		str[1] = str[0];
		str[0] = '0';
	}
	write_u8(str[0]);
	write_u8(str[1]);
}

void uart_fifo_class::write_hexarray (u8 *b, u8 len) {
	// ������� ������ ���� ��� hex-������
	for (u8 x = 0; x < len; x++)
		write_hex(b[x]);
}

void uart_fifo_class::write_hexstr (const char *b, u8 len) {
	// �������� ������ ��� HEX-�����
	for (u8 x = 0; x < len; x++)
		write_hex(b[x]);
}

void uart_fifo_tx(uart_fifo_class *uart) {
	// ������ �� ����������� (FIFO <-> UART)
	// �������� 1 ����� �� FIFO � UART.
	// ISR - ���������� ������ �� ����������!
	if (uart->txb.count > 0){
		UDR0 = uart->txb.b[uart->txb.head];
		uart->txb.count--;
		uart->txb.head++;
		if (uart->txb.head == FIFO_SIZE) 
			uart->txb.head = 0;
	} else
		UCSR0B &= ~(1<<UDRIE0); // ��������� ��� ����������, ���� FIFO ������
}

void uart_fifo_rx(uart_fifo_class *uart) {
	// ��������� 1 ����� �� UART � ������ � FIFO
	// ISR - ���������� ������ �� ����������!
	if (uart->rxb.count < FIFO_SIZE) { // ���� ���� �����
		uart->rxb.b[uart->rxb.tail] = UDR0;
		if (uart->rxb.b[uart->rxb.tail] == 0x0A) // ����� ������
			uart->rxb.ln_count++; 
		uart->rxb.count++;
		uart->rxb.tail++;
		if (uart->rxb.tail == FIFO_SIZE) 
			uart->rxb.tail = 0;
		uart->rxb.ovf = false;
	} else { // ���� ����� ���, ���������� ���� OVF
		uart->rxb.ovf = true;
	}
}

// ��������� ���������� (� ���������) �� ������� ����������
#ifdef AVRLIBS_UART0
	uart_fifo_class uart0;
	// �� ������� ��������� ������ UDR ������ ���� ���� �� FIFO
	ISR(USART_UDRE_vect) {		
		uart_fifo_tx(&uart0); }
	// �� ������ ����� ������ ��� � FIFO
	ISR(USART_RX_vect) {		
		uart_fifo_rx(&uart0); }
#endif