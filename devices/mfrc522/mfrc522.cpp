/*
 * AVR-LIBS - MFRC522
 * (c) madcatdev https://github.com/madcatdev/avr-libs
 */ 

#include "mfrc522.h"

mfrc522_class::mfrc522_class (spi_class &spi_device, volatile u8 *_portx, volatile  u8 *_ddrx, u8 _pinx) {
	// Конструктор класса с присвоением устройства I2C, порта и пина вывода SS/CS
	spi = &spi_device;
	portx = _portx;
	ddrx = _ddrx;
	pinx = _pinx;

	// Первичное назначение пина SS в высокий уровень и на выход
	spi->choose(portx, ddrx, pinx); // Передаем без сслыки, ибо там уже указатель
}

u8 mfrc522_class::read(u8 reg){
	// Чтение регистра
	u8 data;
	spi->choose(portx, ddrx, pinx);
	spi->select();
	spi->exch_u8(((reg<<1) & 0x7E) | 0x80); // Отсылаем адрес регистра
	data = spi->exch_u8(0x00); // Читаем байт
	spi->deselect();
	
	return data;
}

void mfrc522_class::write(u8 reg, u8 data){
	// Запись байта в регистр
	spi->choose(portx, ddrx, pinx);
	spi->select();
	spi->exch_u8((reg << 1) & 0x7E); // Адрес регистра со странным смещением (см.даташит)
	spi->exch_u8(data);
	spi->deselect();
}

bool mfrc522_class::init(void){
	// Инициализация
	u8 x;
	reset();
	
	write(TModeReg, 0x8D);
	write(TPrescalerReg, 0x3E);
	write(TReloadReg_1, 30);
	write(TReloadReg_2, 0);
	write(TxASKReg, 0x40);
	write(ModeReg, 0x3D);
	
	// Проверка регистра по маске (зачем?)
	x = read(TxControlReg);
	if(!(x & 0x03))
		write(TxControlReg, x | 0x03);
	return true; // ?
}

void mfrc522_class::reset(void){
	write(CommandReg, SoftReset_CMD);
}

u8 mfrc522_class::request(u8 req_mode, u8 * tag_type){
	u8  status;
	u32 backBits;//The received data bits

	write(BitFramingReg, 0x07);//TxLastBists = BitFramingReg[2..0]	???
	
	tag_type[0] = req_mode;
	status = to_card(Transceive_CMD, tag_type, 1, tag_type, &backBits);

	if ((status != CARD_FOUND) || (backBits != 0x10))
		status = ERROR;

	return status;
}

u8 mfrc522_class::to_card(u8 cmd, u8 *send_data, u8 send_data_len, u8 *back_data, u32 *back_data_len){
	u8 status = ERROR;
	u8 irqEn = 0x00;
	u8 waitIRq = 0x00;
	u8 lastBits;
	u8 n;
	u8 tmp;
	u32 i;

	switch (cmd)	{
		case MFAuthent_CMD:	{	//Certification cards close	
			irqEn = 0x12;
			waitIRq = 0x10;
			break;
		}
		case Transceive_CMD: {	//Transmit FIFO data
			irqEn = 0x77;
			waitIRq = 0x30;
			break;
		}
		default:
		break;
	}
	
	//mfrc522_write(ComIEnReg, irqEn|0x80);	//Interrupt request
	n = read(ComIrqReg);
	write(ComIrqReg,n&(~0x80));//clear all interrupt bits
	n = read(FIFOLevelReg);
	write(FIFOLevelReg,n|0x80);//flush FIFO data
	
	write(CommandReg, Idle_CMD);	//NO action; Cancel the current cmd???

	//Writing data to the FIFO
	for (i=0; i<send_data_len; i++)
		write(FIFODataReg, send_data[i]);

	//Execute the cmd
	write(CommandReg, cmd);
	if (cmd == Transceive_CMD)	{
		n=read(BitFramingReg);
		write(BitFramingReg,n|0x80);
	}
	
	//Waiting to receive data to complete
	i = 2000;	//i according to the clock frequency adjustment, the operator M1 card maximum waiting time 25ms???
	do	{
		//CommIrqReg[7..0]
		//Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
		n = read(ComIrqReg);
		i--;
	} while ((i!=0) && !(n&0x01) && !(n & waitIRq));

	tmp=read(BitFramingReg);
	write(BitFramingReg,tmp&(~0x80));
	
	if (i != 0)	{
		if(!(read(ErrorReg) & 0x1B)){	//BufferOvfl Collerr CRCErr ProtecolErr
		
			status = CARD_FOUND;
			if (n & irqEn & 0x01)
				status = CARD_NOT_FOUND;			//??
	
			if (cmd == Transceive_CMD) {
				n = read(FIFOLevelReg);
				lastBits = read(ControlReg) & 0x07;
				if (lastBits)
					*back_data_len = (n-1)*8 + lastBits;
				else
					*back_data_len = n*8;

				if (n == 0)
					n = 1;
				if (n > MAX_LEN)
					n = MAX_LEN;
				
				//Reading the received data in FIFO
				for (i=0; i<n; i++)
					back_data[i] = read(FIFODataReg);
			}
		} else
			status = ERROR;		
	}
	
	//SetBitMask(ControlReg,0x80);           //timer stops
	//mfrc522_write(cmdReg, PCD_IDLE);

	return status;
}

u8 mfrc522_class::get_card_serial(u8 * uid) {
	// Получение UID карты. Возвращает 1/2/3 (см.дефайны)
	u8 status;
	u8 i;
	u8 serNumCheck=0;
	u32 unLen;
	
	write(BitFramingReg, 0x00);		//TxLastBists = BitFramingReg[2..0]
	
	uid[0] = PICC_ANTICOLL;
	uid[1] = 0x20;
	status = to_card(Transceive_CMD, uid, 2, uid, &unLen);

	if (status == CARD_FOUND) {
		//Check card serial number
		for (i=0; i<4; i++)
			serNumCheck ^= uid[i];

		if (serNumCheck != uid[i])
			status = ERROR;
	}
	return status;
}