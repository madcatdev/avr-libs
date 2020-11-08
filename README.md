This is a bunch of C++ libraries for AVR (currently atmega/attiny).
It was created to simpify my work with bare-metal MCU. 

Supported MCUs:
- Atmega168/328(p)

Implemented interfaces:
- EEPROM with 16-bit addressation.
- I2C/TWI in master mode.
- UART with FIFO buffer.
- SPI in master mode.
- Some text processing and time conversion functions.

Supported devices:
- DS1307/DS3231 (I2C-based RTC)

Plans:
- 1-Wire in master and slave mode.
- Power management with sleep modes.
- RTC (based on async timer with 32.768 crystal).
- Support for MCUs with multiple SPI/UART/I2C interfaces.
- XMega support.
