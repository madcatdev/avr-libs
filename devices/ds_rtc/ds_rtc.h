/*
 * AVR-LIBS - DS_RTC
 * MCU: Any
 * Created: 14.01.2019
 * Last version: 15.09.2020
 * (c) madcatdev https://github.com/madcatdev/avr-libs

 * DESCR:
 * Library for DS1307/DS3231 RTC 
 * Макс. частота I2C - 400 КГц
 * Не реализовано - будильники и прочие доп. функции, кроме температуры
 
 * TODO:
 * 
 *
 */ 


#ifndef DS_RTC_H_
#define DS_RTC_H_


	#include "../../interfaces/i2c_master.h"
	#include "../../misc/inttypes.h"
	#include "../../misc/time.h"
	#include <stdint.h>
	#include <stdlib.h>

	//#define RTC_DS1307
	#define RTC_DS3231

	// Адреса одинаковые для DS3231 и DS1307
	#ifdef RTC_DS1307
		#define RTC_ADDR 0x68
	#endif

	#ifdef RTC_DS3231
		#define RTC_ADDR 0x68
	#endif

	#ifndef RTC_ADDR
		#error "RTC type is not defined"
	#endif
	
	class ds_rtc_class{
		public:
		ds_rtc_class (i2c_class &i2c_device);
		bool init_ds3121(void);
		bool init_ds1307(void);
		void stop_ds1307(void);
		bool read_time(time_struct &t);
		bool read_temp(i16 &temp);
		bool write_time(time_struct &t);
		private:
		i2c_class *i2c;
	};

#endif /* DS3231_H_ */