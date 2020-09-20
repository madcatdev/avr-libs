/*
 * AVR-LIBS - Strings
 * (c) madcatdev https://github.com/madcatdev/avr-libs
 */ 

#include "string.h"

u8 str_pos(const char *str, const char *substr, u8 start) {
	// Поиск подстроки в строке (посимвольный), возвращаем позицию подстроки (начиная со знака start). Побайтово
	// Длина строки не более 254 символов, если не найдено - вернет 255 (0xFF)
	// purecodecpp.com/archives/2579
	u8 i = 0; // Индекс строки
	u8 k = 0; // Индекс подстроки

	while (str[i] != 0) { // Перебираем str
		k = 0;
		// Пока не кончились обе строки и совпадают текущие символы
		while ( (str[i+k] != 0)  && (substr[k] != 0)  && (str[i+k] == substr[k])) {
			// Если символ совпал и это последний символ в подстроке, выходим с результатом
			// Вернем номер символа в строке str, но только если дошли до символа start
			if ( (substr[k+1] == 0) && (i >= start)) 
				return i; 
			k++;
		}
		i++;
	}

	return 0xFF; // Не нашли
}

bool str_compare(const char *str1, const char *str2) {
	// Сравнение строк до символов \0
	while (*str1 && *str2) {
		if  (*str1 != *str2)  // если символы не совпадают
			return false;
		*str1++;
		*str2++;
	}
	if (*str1 != *str2)  // Если одна из строк не кончилась
		return false; 
	return true; // Если дошли сюда, значит строки одинаковые
}

void str_copy(const char *str_from, char *str_to, u8 start, u8 len) {
	// Копирование куска строки str_from в строку str_to, c байта start (счет от 0) на длину len
	// Не учитывается длина буфера str_to и длина str_from при задании параметра start,  проверять вручную!
	// Строка str_from далее нулевого символа не копируется
	u8 x = 0;	
	while ( (str_from[start+x] != 0) && len) {
		len--;
		str_to[x] = str_from[start+x];
		x++;
	}	
	str_to[x] = 0; // Терминируем
}

void str_add(char *str, const char *substr) {
	// Копирование куска строки str_from в конец строки str_to, c байта start (счет от 0) на длину len
	// Не учитывается длина буфера str_to и длина str_from при задании параметра start, проверять вручную!
	// Строка str_from далее нулевого символа не копируется
	u8 a = 0; // Индекс строки
	u8 b = 0; // Индекс подстроки
		
	while (str[a]) a++; // Сдвинем индекс на длину строки	
	while (substr[b]) { // Копируем до конца подстроки
		str[a+b] = substr[b];
		b++;
	}	
	str[a+b] = 0; // Терминируем
}

u8 str_len(const char *str) {
	// Подсчет длины строки (до символа /0)
	u8 len = 0;
	while (str[len]) len++; 
	return len;
}

u8 ascii_to_val(const char a) {
	// Получение числового значения ниббла по таблице ASCII 
	// Таблица: celitel.info/klad/tabsim.htm
	
	if ( (a >= 0x30) && (a <= 0x39) )  // 0x30-39 (числа 0-9)
		return a - 0x30;
	
	if ( (a >= 0x41) && (a <= 0x46) )  // 0x41-46 (буквы A-F
		return a - 0x37; // -0x41 + 10 - поскольку числа 10-16
		
	if ( (a >= 0x61) && (a <= 0x66) )  // 0x61-66 (буквы a-f)
		return a - 0x57; // -0x61 + 10 - поскольку числа 10-16	
	
	return 0; // Если нет совпадения
}

char val_to_ascii(u8 x) {
	// Получение символа по его числовому значению
	if (x < 10) 
		return 0x30 + x; // 0x30-39 (числа 0-9)
	else
		return 0x41 + (x - 10); // 0x41-46 (буквы A-F)
}

u8 hex_to_u8(const char *str) {
	/*
	Преобразование из HEX строки (вида F0) в байт, /0 не требуется
	Формат HEX удобен тем, что октет (8 бит) представлен в нем группами по 4 бита, каждой группе соответствует символ (F0 -> 1111 0000, 0F -> 0000 1111)
	Берем символ, сравниваем с таблицей ASCII, получаем его числовое значение, затем второй и склеиваем в байт
	
	TODO - можно сделать dec_to_u8 (посложнее будет)
	*/
	
	u8 high = ascii_to_val(str[0]);
	u8 low = ascii_to_val(str[1]);
	return (high << 4) | low;
}

void u8_to_hex(const u8 x, char *str){
	// Преобразование из байта в HEX строку
	// Делим на два ниббла, находим их HEX значение и склеиваем в строку, терминируем

	str[0] = val_to_ascii(x >> 4);
	str[1] = val_to_ascii(x & 0x0F);
	str[2] = 0;
}

void u8_to_str_z(char *str, u8 x) {
	// Перевод Uint8 в строку из двух символов, не теряя значащих нулей
	// Например, "5" -> "05", "75" -> "75"
	// Буфер в 4 символа минимум!
	
	utoa(x, str, 10); // Конвертнем в строку
	if (str[1] == '\0') { // Если у нас 1 десятичный разряд, тут будет символ \0. Это порежет нам строку
		str[1] = str[0]; // Сдвигаем на 1 разряд
		str[0] = '0'; // Дописываем '0' ASCII
	}

}

void n32_to_strfloat(char *str, u32 num, u8 left_len, u8 right_len, bool is_signed) {
	/* 
    Преобразование uint32/int32 вида (-)12345 в строку вида(-)12.345 
	Если число отрицательное, то знак '-' не учитывается (неважно, есть ли знак - числа будут одной длины, 
	счет знаков до запятой идет с правого конца числа.)
	str - выходной буфер, num - само число, left_len - мин. кол-во знаков до запятой
	(заполняются незначащими нулями если число меньше), right_len - кол-во знаков после запятой
	(фиксированное), is_signed - если true, то число интерпретируется как int32_t
	*/
	char b[16]; // Буфер. Макс.для uint32 11 + 1точка + 1знак + 1 \0
	u8 x = 0; // Указатель позиции исходной строки
	u8 y = 0; // Указатель позиции строки назначения
	
	// Конвертируем в строку и считаем длину
	if (is_signed)
		ltoa(num,b,10); // Signed long to array
	else
		ultoa(num,b,10); // Unsigned long to array
	u8 len = strlen(b);
	u8 desired_len = left_len + right_len; // Желаемая длина числа
	u8 len_true = len; // Длина без знака минус
	if (b[0] == '-') 
		len_true--;
		
	// Вычисление кол-ва незначащих нулей
	i8 irr_zeroes_count = 0; // Кол-во незначащих нулей до числа.
	if (len_true < desired_len)  // Если фактическая длина меньше желаемой
		irr_zeroes_count = desired_len - len_true;    

    // Вычисление позиции точки
	i8 point_pos = 0;
	point_pos = len_true + irr_zeroes_count - right_len;

	// Заполнени
	// Ставим минус, если надо
	if (b[0] == '-'){
		str[y++] = b[x++];
		point_pos++; // Смещаем позицию точки
	}

	// Затем незначащие нули и точка, если надо
	while (irr_zeroes_count > 0) {
		str[y++] = '0';
		irr_zeroes_count--;
		if (y == point_pos)
			str[y++] = '.';
	}

	// Затем сами цифры и точка, если надо
	while (x < len) {
		str[y++] = b[x++];
		if (y == point_pos)
			str[y++] = '.';
	}

	str[y] = 0; // Терминируем
}
