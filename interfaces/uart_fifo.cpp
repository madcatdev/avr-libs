/*
 * AVR-LIBS - UART with FIFO
 * (c) madcatdev https://github.com/madcatdev/avr-libs
 */ 
#include "uart_fifo.h"

void uart_fifo_class::init(u32 clock, u32 baud)	{
	// Настройка скорости
	u16 _ubrr = clock/16/baud-1;
	UBRR0H = (u8)(_ubrr>>8);
	UBRR0L = (u8)_ubrr;
	// Включаем приемник и передатчик
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	// Настройка формата байта: 8data, 2stop. УТОЧНИТЬ!!
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
	// Включаем прерывание по приему байта (для FIFO)
	UCSR0B |= (1<<RXCIE0); // Receive Complete Interrupt 
}

void uart_fifo_class::stop(void) {
	// Остановка (отключение) UART, отключение прерывания
	UCSR0B &= ~(1<<RXEN0);
	UCSR0B &= ~(1<<TXEN0);
	UCSR0B &= ~(1<<RXCIE0); // Прерывание
}

void uart_fifo_class::flush_all (void) {
	// Работа в основном потоке c буфером (USER <--> FIFO)
	// Очистка всех приемных буферов
	flush_buf(txb);
	flush_buf(rxb);
}

void uart_fifo_class::flush_buf (fifo_struct &f) {
	// Очистка буфера
	f.head = 0;
	f.tail = 0;
	f.count = 0;
	f.ln_count = 0;
	f.ovf = false;
}

void uart_fifo_class::write_pu8 (u8 &b) {
	// Кладем 1 байт в FIFO и включаем прерывание по очистке приемного буфера
	if (txb.count < FIFO_SIZE) { // Если есть место
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
	// Кладем 1 байт в FIFO и включаем прерывание по очистке приемного буфера
	// Передача байта напрямую, без ссылки
	if (txb.count < FIFO_SIZE) { // Если есть место
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
	// Кладем в FIFO_TX строку
	// Передаем, пока не встретим \0
	while(*b)
		write_u8(*b++);
}

void uart_fifo_class::writeln(const char *b){
	// Кладем в FIFO_TX строку, с \n (0x0A) в конце
	// Передаем, пока не встретим \0
	while(*b)
		write_u8(*b++);
	write_u8(NL); // Line feed, перевод строки, \n
}

u8 uart_fifo_class::read_u8 (void) {
	// Получаем 1 байт из приемного FIFO
	u8 x = 0;
	
	cli();
	
	if (rxb.count > 0){
		x = rxb.b[rxb.head];
		rxb.count--;
		rxb.head++;
		if (rxb.head == FIFO_SIZE) 
			rxb.head = 0;	
		if (x == NL) 
			rxb.ln_count--; // Уменьшаем счетчик строк
	}
	
	sei();
	
	return x;
}

bool uart_fifo_class::read(char *b, u8 len) {
	// Получаем строку до конца FIFO, но не длиннее чем len
	u8 x = 0; 
		//len--; // Поскольку строка завершается символом 
	while (rxb.count && len) { // Принимаем до конца FIFO, либо до конца буфера len
		b[x] = read_u8(); // Кладем символ в строку
		len--;
		if (len) x++; // Смещаемся по строке, если позволяет длина
		// Если не проверять, получаем странный баг со временем мк (возможно из-за записи 0x00 хуй пойми куда)
	}
	b[x] = 0; // Терминируем
	
	if (x > 0) 
		return true; 
	else 
		return false;	

}

bool uart_fifo_class::readln(char *b, u8 len) {
	// Получаем строку до символа 0x0A, но не длиннее чем len
	
	if (rxb.ln_count || rxb.ovf) { // Если есть строки (символы LN) либо переполнение
		u8 x = 0; 
		//len--; // Поскольку строка завершается символом 
		while ((x != NL) && rxb.count && len) { // Принимаем до символа LN либо, либо до конца FIFO, либо до конца буфера len
			x = read_u8();
			// Если у нас символ \r (0x0D), вместо него будет \0
			if (x == CR) 
				x = 0;
			//*b++ = x;
			*b = x; // Кладем символ в строку
			len--;
			if (len) // Смещаемся по строке, если позволяет длина
				b++; 
			// Если не проверять, получаем странный баг со временем мк (мб из-за записи 0x00 хуй пойми куда)
		}
		*--b = 0; // Обрезаем, чтобы не лез мусор (ставим 0 вместо 0x0A
		return true;
	} else 
		return false; // Нету строк
	
}

void uart_fifo_class::write_hex (u8 b) {
	// Берем байт, конвертим его в HEX и кладем в фифо (2 байта)
	char str[3]; // F+F+\0
	u8_to_hex(b, str);
	if (str[1] == 0) { // Если меньше 1 символа, ставим 0 перед ним
		str[1] = str[0];
		str[0] = '0';
	}
	write_u8(str[0]);
	write_u8(str[1]);
}

void uart_fifo_class::write_hexarray (u8 *b, u8 len) {
	// Выводим массив байт как hex-строку
	for (u8 x = 0; x < len; x++)
		write_hex(b[x]);
}

void uart_fifo_class::write_hexstr (const char *b, u8 len) {
	// Отправка строки как HEX-дампа
	for (u8 x = 0; x < len; x++)
		write_hex(b[x]);
}

void uart_fifo_tx(uart_fifo_class *uart) {
	// Работа по прерываниям (FIFO <-> UART)
	// Отправка 1 байта из FIFO в UART.
	// ISR - вызывается только из прерывания!
	if (uart->txb.count > 0){
		UDR0 = uart->txb.b[uart->txb.head];
		uart->txb.count--;
		uart->txb.head++;
		if (uart->txb.head == FIFO_SIZE) 
			uart->txb.head = 0;
	} else
		UCSR0B &= ~(1<<UDRIE0); // Отключаем это прерывание, если FIFO пустое
}

void uart_fifo_rx(uart_fifo_class *uart) {
	// Получение 1 байта из UART и запись в FIFO
	// ISR - вызывается только из прерывания!
	if (uart->rxb.count < FIFO_SIZE) { // Если есть место
		uart->rxb.b[uart->rxb.tail] = UDR0;
		if (uart->rxb.b[uart->rxb.tail] == 0x0A) // Новая строка
			uart->rxb.ln_count++; 
		uart->rxb.count++;
		uart->rxb.tail++;
		if (uart->rxb.tail == FIFO_SIZE) 
			uart->rxb.tail = 0;
		uart->rxb.ovf = false;
	} else { // Если места нет, выставляем флаг OVF
		uart->rxb.ovf = true;
	}
}

// Настройка прерываний (и регистров) по нужному интерфейсу
#ifdef AVRLIBS_UART0
	uart_fifo_class uart0;
	// По очистке приемного буфера UDR кладем туда байт из FIFO
	ISR(USART_UDRE_vect) {		
		uart_fifo_tx(&uart0); }
	// По приему байта кладем его в FIFO
	ISR(USART_RX_vect) {		
		uart_fifo_rx(&uart0); }
#endif