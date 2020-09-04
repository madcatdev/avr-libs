# avr-libs
madcatdev AVR Libraries

This is a bunch of libraries for AVR MCUs (currently Atmega328p).
It was created to simpify my work with bare-metal MCU. 

Currently implemented:
- EEPROM with 16-bit addresses.
- I2C in master mode.
- UART with FIFO.
- SPI in master mode.
- RTC (based on async timer with 32.768 crystal).
- Some text processing functions.

Plans:
- 1-Wire (interrupt-based).
