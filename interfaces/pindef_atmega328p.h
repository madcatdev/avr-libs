/*
 * AVR-LIBS - Pin definitions
 * MCU: Atmega328p
 * Created: 09.01.2019
 * Last version: 14.09.2020
 * (c) madcatdev https://github.com/madcatdev/avr-libs

 * DESCR:
 * Set of macro for easy pin operations (short version)

 * TODO:

*/

// PORT B
// PB6, PB7 - XTAL, PB5 - LED
#define OUT_PB(x) DDRB |= _BV(x);
#define IN_PB(x) DDRB &= ~_BV(x);
#define HI_PB(x) PORTB |= _BV(x);
#define LO_PB(x)  PORTB &= ~_BV(x);
#define INV_PB(x) PORTB ^= _BV(x);
#define RD_PB(x) ((PINB & _BV(x)) >> x)

// PORT C
// PC6 - RESET, PC7 - Missing
#define OUT_PC(x) DDRC |= _BV(x);
#define IN_PC(x) DDRC &= ~_BV(x);
#define HI_PC(x) PORTC |= _BV(x);
#define LO_PC(x)  PORTC &= ~_BV(x);
#define INV_PC(x) PORTC ^= _BV(x);
#define RD_PC(x) ((PINC & _BV(x)) >> x)

// PORT D
// PD0 - RXD, PD1 - TXD
#define OUT_PD(x) DDRD |= _BV(x);
#define IN_PD(x) DDRD &= ~_BV(x);
#define HI_PD(x) PORTD |= _BV(x);
#define LO_PD(x)  PORTD &= ~_BV(x);
#define INV_PD(x) PORTD ^= _BV(x);
#define RD_PD(x) ((PIND & _BV(x)) >> x)