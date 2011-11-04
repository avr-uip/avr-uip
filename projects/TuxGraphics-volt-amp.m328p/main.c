#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>

#include "global-conf.h"
#include "timer.h"
#include <util/delay.h>




#include "uip_arp.h"
#include "network.h"
#include "enc28j60.h"
#include "apps-conf.h"
#include <string.h>

#include "net_conf.h"

#include "pt.h"

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])


static struct pt blink_thread;
static struct timer blink_timer;
/* === PROTOTHREADS  ===*/
/* ---  Blink protothread ---*/
static 
PT_THREAD(blink(void))
{
	PT_BEGIN(&blink_thread);

		led_high();
		//printf("led ON\n");
		timer_set(&blink_timer, CLOCK_CONF_SECOND);
		PT_WAIT_UNTIL(&blink_thread, 
				timer_expired(&blink_timer));

		led_low();
		//printf("Led OFF\n");
		timer_set(&blink_timer, CLOCK_CONF_SECOND);
		PT_WAIT_UNTIL(&blink_thread,
				timer_expired(&blink_timer));

	PT_END(&blink_thread);
}



int main(void)
{
    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

	network_init();


	CLKPR = (1<<CLKPCE);	//Change prescaler
	CLKPR = 0;	//Use prescaler 2
	//CLKPR = (1<<CLKPS0);	//Use prescaler 2
	_delay_loop_1(0);
	enc28j60Write(ECOCON, 2 & 0x7);	//Get a 12.5MHz signal from enc28j60
	_delay_loop_1(0);

	int i;
	struct timer periodic_timer, arp_timer;

	clock_init();

	timer_set(&periodic_timer, CLOCK_SECOND);
	timer_set(&arp_timer, CLOCK_SECOND * 10);

	uip_init();
    // must be done or sometimes arp doesn't work
    uip_arp_init();
	
	// load the network configs from eeprom
	net_conf_init();

    // start telnet server
	telnetd_init();

	// set up the udp data stream
	// configure the target system ip and port
	uip_ipaddr_t ipaddr;
	uip_ipaddr(&ipaddr, 192,168,2,99);
	udpds_conf(ipaddr, 33, 8);

led_conf();

	while(1){
//		blink();
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

int udpds_set_payload(void)
{
  led_blink();
  memcpy((char *)uip_appdata, "hello out there", sizeof("hello out there"));
  return (sizeof("hello out there")); 
}


