# avr-libs
madcatdev AVR Libraries

This is a bunch of libraries for AVR MCUs (currently Atmega168/328(p)).
It was created to simpify my work with bare-metal MCU. 

Currently implemented:
- EEPROM with 16-bit addresses.
- I2C/TWI in master mode.
- UART with FIFO.
- SPI in master mode.
- Some text processing and time conversion functions.

Plans:
- 1-Wire (interrupt-based).
- Power management or sleep modes.
- RTC (based on async timer with 32.768 crystal).
- Support for MCUs with multiple SPI/UART/I2C interfaces.
