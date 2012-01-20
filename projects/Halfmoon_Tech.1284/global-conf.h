//Project specific configurations
#ifndef __GLOBAL_CONF_H__
#define __GLOBAL_CONF_H__

//#define CONSOLE_SPEED_9600 80
//#define CONSOLE_SPEED_9600 85 


//Define frequency
#define F_CPU 14745600UL


//Include uip.h gives all the uip configurations in uip-conf.h
#include "uip.h"


#define led_conf()      DDRC |= (1<<DDC0)
#define led_low()       PORTC |= (1<<PORTC0)
#define led_high()      PORTC &= ~(1<<PORTC0)
#define led_blink()     PORTC ^= (1<<PORTC0)


#endif /*__GLOBAL_CONF_H__*/
