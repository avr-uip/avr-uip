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
//#include "adc.h"


#define BUF ((struct uip_eth_hdr *)&uip_buf[0])
struct timer dhcp_timer;


int main(void)
{
    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

led_conf();


        CLKPR=(1<<CLKPCE); // change enable
        CLKPR=0; // "no pre-scaler"
        _delay_loop_1(0); // 60us

/*
	CLKPR = (1<<CLKPCE);	//Change prescaler
	CLKPR = 0;	//Use prescaler 2
	//CLKPR = (1<<CLKPS0);	//Use prescaler 2
	_delay_loop_1(0);
	enc28j60Write(ECOCON, 2 & 0x7);	//Get a 12.5MHz signal from enc28j60
	_delay_loop_1(0);
*/
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

    network_init_mac(net_conf_get_mac());

    if (net_conf_is_dhcpc())
    {
        // setup the dhcp renew timer the make the first request
        timer_set(&dhcp_timer, CLOCK_SECOND * 60);
// orig        timer_set(&dhcp_timer, CLOCK_SECOND * 600);
        dhcpc_init(net_conf_get_mac(), 6);
        dhcpc_request();
    }

    // start telnet server
    telnetd_init();
    httpd_init();
/*
	adc_init();
*/

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
        } else if (net_conf_is_dhcpc() && timer_expired(&dhcp_timer)) {
            // for now turn off the led when we start the dhcp process
            led_low();
            dhcpc_renew();
            timer_reset(&dhcp_timer);
        }
/*else
{
	_delay_loop_2(50000);
led_blink();
	_delay_loop_2(50000);
	_delay_loop_2(50000);
	_delay_loop_2(50000);
}*/
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


/*---------------------------------------------------------------------------*/


void dhcpc_configured(const struct dhcpc_state *s)
{
    // byte swap the network info
    net_conf_set_ip_ipaddr(s->ipaddr);

    net_conf_set_nm_ipaddr(s->netmask);

    net_conf_set_gw_ipaddr(s->default_router);

    net_conf_init();

//  code to use dhcp server lease time removed due to uint16_t overflow
//  issues with calculating the time.  Just use 5 minutes instead.  
//    timer_set(&dhcp_timer, 5 * 60 * CLOCK_SECOND);
    timer_reset(&dhcp_timer);

#ifdef DHCP_DEBUG
    // for now turn on the led when we get an ip
    led_high();
#endif
}

/*---------------------------------------------------------------------------*/
