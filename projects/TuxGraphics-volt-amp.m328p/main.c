#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>

#include "global-conf.h"
#include "timer.h"

#include "uip_arp.h"
#include "network.h"
#include "enc28j60.h"
#include "apps-conf.h"
#include <string.h>

#include "net_conf.h"

//#include "usart.h"
//#include "uart.h"

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

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



//EEPROM parameters (TCP/IP parameters)


int main(void)
{
    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

	network_init();


	CLKPR = (1<<CLKPCE);	//Change prescaler
	CLKPR = (1<<CLKPS0);	//Use prescaler 2
	enc28j60Write(ECOCON, 1 & 0x7);	//Get a 25MHz signal from enc28j60

	int i;
	uip_ipaddr_t ipaddr;
	struct timer periodic_timer, arp_timer;

	clock_init();

	timer_set(&periodic_timer, CLOCK_SECOND);
	timer_set(&arp_timer, CLOCK_SECOND * 10);

	uip_init();
    // must be done or sometimes arp doesn't work
    uip_arp_init();
	
	// load the network configs from eeprom
	net_conf_init();

    // start up the webserver
    //httpd_init();
	//simple_httpd_init();
	telnetd_init();

//	usart_init(CONSOLE_SPEED_9600);
//	usart_redirect_stdout();
//
//	printf("\nThis is a test... ");

/*
	USART_init(CONSOLE_SPEED_9600);
	USART_transmit('A');
	USART_transmit('B');
	USART_transmit('C');
	USART_transmit('D');

	sendString("Hello out there\n\r");
*/
led_high();

	while(1){
		uip_len = network_read();


		if(uip_len > 0) {
			if(BUF->type == htons(UIP_ETHTYPE_IP)){
				uip_arp_ipin(); // arp seems to have issues w/o this
				uip_input();
				if(uip_len > 0) {
					uip_arp_out();
					network_send();
				}
			}else if(BUF->type == htons(UIP_ETHTYPE_ARP)){
				uip_arp_arpin(); // this is correct
				if(uip_len > 0){
					network_send();
				}
			}

		}else if(timer_expired(&periodic_timer)) {
			timer_reset(&periodic_timer);


			for(i = 0; i < UIP_CONNS; i++) {
				uip_periodic(i);
				if(uip_len > 0) {
					uip_arp_out();
					network_send();
				}
			}

			#if UIP_UDP
			for(i = 0; i < UIP_UDP_CONNS; i++) {
				uip_udp_periodic(i);
				if(uip_len > 0) {
					uip_arp_out();
					network_send();
				}
			}
			#endif /* UIP_UDP */

			if(timer_expired(&arp_timer)) {
				timer_reset(&arp_timer);
				uip_arp_timer();
			}
		}
/// 		while(network_sending()) {asm("nop");}; //wait untill packet is sent away
	}
	return 0;
}

#if UIP_CONF_LOGGING == 1
void uip_log(char *m)
{
//	sendString(m);
	printf("%s", m);
	//TODO: Get debug information out here somehow, does anybody know a smart way to do that?
}
#endif

/*---------------------------------------------------------------------------*/


