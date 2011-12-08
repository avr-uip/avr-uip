//Project specific configurations
#ifndef __GLOBAL_CONF_H__
#define __GLOBAL_CONF_H__

//#define CONSOLE_SPEED_9600 80
//#define CONSOLE_SPEED_9600 85 


//Define frequency
#define F_CPU 16000000UL
//Mac adress definition for enc28j60
#define ETHADDR0		0x00
#define ETHADDR1		0xbd
#define ETHADDR2		0x3b
#define ETHADDR3		0x33
#define ETHADDR4		0x05
#define ETHADDR5		0x71
//Mac adress definition for uip
#define UIP_ETHADDR0    ETHADDR0
#define UIP_ETHADDR1    ETHADDR1
#define UIP_ETHADDR2    ETHADDR2
#define UIP_ETHADDR3    ETHADDR3
#define UIP_ETHADDR4    ETHADDR4
#define UIP_ETHADDR5    ETHADDR5

// ENC28J60 SPI port
#define ENC28J60_SPI_PORT               PORTB
#define ENC28J60_SPI_DDR                DDRB


#if defined(__AVR_ATmega644__)||defined(__AVR_ATmega644P__)||defined(__AVR_ATmega1284P__)
  #define ENC28J60_SPI_SCK        PORTB7
  #define ENC28J60_SPI_MOSI       PORTB5
  #define ENC28J60_SPI_MISO       PORTB6
  #define ENC28J60_SPI_SS         PORTB4
  #define ENC28J60_CONTROL_CS      PORTB4
#else
  #error "NIC SPI PORT PINS NOT DEFINED"
#endif

// ENC28J60 control port
#define ENC28J60_CONTROL_PORT   PORTB
#define ENC28J60_CONTROL_DDR    DDRB


//Include uip.h gives all the uip configurations in uip-conf.h
#include "uip.h"


#define led_conf()      DDRC |= (1<<DDC0)
#define led_low()       PORTC |= (1<<PORTC0)
#define led_high()      PORTC &= ~(1<<PORTC0)
#define led_blink()     PORTC ^= (1<<PORTC0)


#endif /*__GLOBAL_CONF_H__*/
