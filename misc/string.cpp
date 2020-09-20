/*
 * AVR-LIBS - Strings
 * (c) madcatdev https://github.com/madcatdev/avr-libs
 */ 

#include "string.h"

u8 str_pos(const char *str, const char *substr, u8 start) {
	// ����� ��������� � ������ (������������), ���������� ������� ��������� (������� �� ����� start). ���������
	// ����� ������ �� ����� 254 ��������, ���� �� ������� - ������ 255 (0xFF)
	// purecodecpp.com/archives/2579
	u8 i = 0; // ������ ������
	u8 k = 0; // ������ ���������

	while (str[i] != 0) { // ���������� str
		k = 0;
		// ���� �� ��������� ��� ������ � ��������� ������� �������
		while ( (str[i+k] != 0)  && (substr[k] != 0)  && (str[i+k] == substr[k])) {
			// ���� ������ ������ � ��� ��������� ������ � ���������, ������� � �����������
			// ������ ����� ������� � ������ str, �� ������ ���� ����� �� ������� start
			if ( (substr[k+1] == 0) && (i >= start)) 
				return i; 
			k++;
		}
		i++;
	}

	return 0xFF; // �� �����
}

bool str_compare(const char *str1, const char *str2) {
	// ��������� ����� �� �������� \0
	while (*str1 && *str2) {
		if  (*str1 != *str2)  // ���� ������� �� ���������
			return false;
		*str1++;
		*str2++;
	}
	if (*str1 != *str2)  // ���� ���� �� ����� �� ���������
		return false; 
	return true; // ���� ����� ����, ������ ������ ����������
}

void str_copy(const char *str_from, char *str_to, u8 start, u8 len) {
	// ����������� ����� ������ str_from � ������ str_to, c ����� start (���� �� 0) �� ����� len
	// �� ����������� ����� ������ str_to � ����� str_from ��� ������� ��������� start,  ��������� �������!
	// ������ str_from ����� �������� ������� �� ����������
	u8 x = 0;	
	while ( (str_from[start+x] != 0) && len) {
		len--;
		str_to[x] = str_from[start+x];
		x++;
	}	
	str_to[x] = 0; // �����������
}

void str_add(char *str, const char *substr) {
	// ����������� ����� ������ str_from � ����� ������ str_to, c ����� start (���� �� 0) �� ����� len
	// �� ����������� ����� ������ str_to � ����� str_from ��� ������� ��������� start, ��������� �������!
	// ������ str_from ����� �������� ������� �� ����������
	u8 a = 0; // ������ ������
	u8 b = 0; // ������ ���������
		
	while (str[a]) a++; // ������� ������ �� ����� ������	
	while (substr[b]) { // �������� �� ����� ���������
		str[a+b] = substr[b];
		b++;
	}	
	str[a+b] = 0; // �����������
}

u8 str_len(const char *str) {
	// ������� ����� ������ (�� ������� /0)
	u8 len = 0;
	while (str[len]) len++; 
	return len;
}

u8 ascii_to_val(const char a) {
	// ��������� ��������� �������� ������ �� ������� ASCII 
	// �������: celitel.info/klad/tabsim.htm
	
	if ( (a >= 0x30) && (a <= 0x39) )  // 0x30-39 (����� 0-9)
		return a - 0x30;
	
	if ( (a >= 0x41) && (a <= 0x46) )  // 0x41-46 (����� A-F
		return a - 0x37; // -0x41 + 10 - ��������� ����� 10-16
		
	if ( (a >= 0x61) && (a <= 0x66) )  // 0x61-66 (����� a-f)
		return a - 0x57; // -0x61 + 10 - ��������� ����� 10-16	
	
	return 0; // ���� ��� ����������
}

char val_to_ascii(u8 x) {
	// ��������� ������� �� ��� ��������� ��������
	if (x < 10) 
		return 0x30 + x; // 0x30-39 (����� 0-9)
	else
		return 0x41 + (x - 10); // 0x41-46 (����� A-F)
}

u8 hex_to_u8(const char *str) {
	/*
	�������������� �� HEX ������ (���� F0) � ����, /0 �� ���������
	������ HEX ������ ���, ��� ����� (8 ���) ����������� � ��� �������� �� 4 ����, ������ ������ ������������� ������ (F0 -> 1111 0000, 0F -> 0000 1111)
	����� ������, ���������� � �������� ASCII, �������� ��� �������� ��������, ����� ������ � ��������� � ����
	
	TODO - ����� ������� dec_to_u8 (��������� �����)
	*/
	
	u8 high = ascii_to_val(str[0]);
	u8 low = ascii_to_val(str[1]);
	return (high << 4) | low;
}

void u8_to_hex(const u8 x, char *str){
	// �������������� �� ����� � HEX ������
	// ����� �� ��� ������, ������� �� HEX �������� � ��������� � ������, �����������

	str[0] = val_to_ascii(x >> 4);
	str[1] = val_to_ascii(x & 0x0F);
	str[2] = 0;
}

void u8_to_str_z(char *str, u8 x) {
	// ������� Uint8 � ������ �� ���� ��������, �� ����� �������� �����
	// ��������, "5" -> "05", "75" -> "75"
	// ����� � 4 ������� �������!
	
	utoa(x, str, 10); // ���������� � ������
	if (str[1] == '\0') { // ���� � ��� 1 ���������� ������, ��� ����� ������ \0. ��� ������� ��� ������
		str[1] = str[0]; // �������� �� 1 ������
		str[0] = '0'; // ���������� '0' ASCII
	}

}

void n32_to_strfloat(char *str, u32 num, u8 left_len, u8 right_len, bool is_signed) {
	/* 
    �������������� uint32/int32 ���� (-)12345 � ������ ����(-)12.345 
	���� ����� �������������, �� ���� '-' �� ����������� (�������, ���� �� ���� - ����� ����� ����� �����, 
	���� ������ �� ������� ���� � ������� ����� �����.)
	str - �������� �����, num - ���� �����, left_len - ���. ���-�� ������ �� �������
	(����������� ����������� ������ ���� ����� ������), right_len - ���-�� ������ ����� �������
	(�������������), is_signed - ���� true, �� ����� ���������������� ��� int32_t
	*/
	char b[16]; // �����. ����.��� uint32 11 + 1����� + 1���� + 1 \0
	u8 x = 0; // ��������� ������� �������� ������
	u8 y = 0; // ��������� ������� ������ ����������
	
	// ������������ � ������ � ������� �����
	if (is_signed)
		ltoa(num,b,10); // Signed long to array
	else
		ultoa(num,b,10); // Unsigned long to array
	u8 len = strlen(b);
	u8 desired_len = left_len + right_len; // �������� ����� �����
	u8 len_true = len; // ����� ��� ����� �����
	if (b[0] == '-') 
		len_true--;
		
	// ���������� ���-�� ���������� �����
	i8 irr_zeroes_count = 0; // ���-�� ���������� ����� �� �����.
	if (len_true < desired_len)  // ���� ����������� ����� ������ ��������
		irr_zeroes_count = desired_len - len_true;    

    // ���������� ������� �����
	i8 point_pos = 0;
	point_pos = len_true + irr_zeroes_count - right_len;

	// ���������
	// ������ �����, ���� ����
	if (b[0] == '-'){
		str[y++] = b[x++];
		point_pos++; // ������� ������� �����
	}

	// ����� ���������� ���� � �����, ���� ����
	while (irr_zeroes_count > 0) {
		str[y++] = '0';
		irr_zeroes_count--;
		if (y == point_pos)
			str[y++] = '.';
	}

	// ����� ���� ����� � �����, ���� ����
	while (x < len) {
		str[y++] = b[x++];
		if (y == point_pos)
			str[y++] = '.';
	}

	str[y] = 0; // �����������
}
