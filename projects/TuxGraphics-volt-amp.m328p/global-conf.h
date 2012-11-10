//Project specific configurations
#ifndef __GLOBAL_CONF_H__
#define __GLOBAL_CONF_H__

//#define CONSOLE_SPEED_9600 80
#define CONSOLE_SPEED_9600 85 


//Define frequency
#define F_CPU 12500000UL
//Mac adress definition for enc28j60
#define ETHADDR0		0x00
#define ETHADDR1		0xbd
#define ETHADDR2		0x3b
#define ETHADDR3		0x33
#define ETHADDR4		0x05
#define ETHADDR5		0x71
//Mac adress definition for uip
#if 0  // using net_conf so remove here
#define UIP_ETHADDR0    ETHADDR0
#define UIP_ETHADDR1    ETHADDR1
#define UIP_ETHADDR2    ETHADDR2
#define UIP_ETHADDR3    ETHADDR3
#define UIP_ETHADDR4    ETHADDR4
#define UIP_ETHADDR5    ETHADDR5
#endif


// ENC28J60 SPI port
#define ENC28J60_SPI_PORT		PORTB
#define ENC28J60_SPI_DDR		DDRB

// pre ATMega328p 
#ifdef PB5
#define ENC28J60_SPI_SCK		PB5
#define ENC28J60_SPI_MOSI		PB3
#define ENC28J60_SPI_MISO		PB4
#define ENC28J60_SPI_SS			PB2
#define ENC28J60_CONTROL_CS		PB2
#else
#define ENC28J60_SPI_SCK        PORTB5
#define ENC28J60_SPI_MOSI       PORTB3
#define ENC28J60_SPI_MISO       PORTB4
#define ENC28J60_SPI_SS         PORTB2
#define ENC28J60_CONTROL_CS      PORTB2
#endif

// ENC28J60 control port
#define ENC28J60_CONTROL_PORT	PORTB
#define ENC28J60_CONTROL_DDR	DDRB


//Include uip.h gives all the uip configurations in uip-conf.h
#include "uip.h"


#ifdef PORTB1
//Led on tuxgraphics board
#define led_conf()      DDRB |= (1<<DDB1)
#define led_low()       PORTB |= (1<<PORTB1)
#define led_high()      PORTB &= ~(1<<PORTB1)
#define led_blink()     PORTB ^= (1<<PORTB1)
#else
//Led on tuxgraphics board
#define led_conf()      DDRB |= (1<<DDB1)
#define led_low()       PORTB |= (1<<PB1)
#define led_high()      PORTB &= ~(1<<PB1)
#define led_blink()     PORTB ^= (1<<PB1)
#endif


#endif /*__GLOBAL_CONF_H__*/
