#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>

#include "global-conf.h"
#include "timer.h"
#include <util/delay.h>

#include <avr/interrupt.h>



#include "uip_arp.h"
#include "network.h"
#include "enc28j60.h"
#include "apps-conf.h"
#include <string.h>

#include "net_conf.h"
//#include "adc.h"

//#include "uart.h"

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])
struct timer dhcp_timer;

int sec4=0;
char foo[100];
ISR(USART0_RX_vect){
        sec4=4;
        char tt;
        tt = UDR;
        if (tt == '\r'){
//                process_command(foo);
                sec4 = 0;
                memset(foo, 0, sizeof(foo));
        }
        else{
                foo[sec4] = tt;
                sec4++;
        }
        return;
}

// wd code from stoker bot
ISR(WDT_vect) {
    sendString("Watchdog caused reset\r\n");
 //   timedSaveEeprom();
    while(true); //Next WDT timeout will reset chip
}

int main(void)
{

    // start watch dog setup
    sei();
    uint8_t resetSource = MCUSR;
    MCUSR = 0;
    wdt_reset();
    wdt_disable();

    wdt_enable(WDTO_4S);
    WDTCSR |= (1 << WDIE);  //enable watchdog interrupt
    wdt_reset();
    cli();
    // end  watch dog setup

led_conf();
led_low();

    CLKPR=(1<<CLKPCE); // change enable
    CLKPR=0; // "no pre-scaler"
    _delay_loop_1(0); // 60us

    int i;
    struct timer periodic_timer, arp_timer;
struct timer dns_test;

    clock_init();

    timer_set(&periodic_timer, CLOCK_SECOND * 3);
    timer_set(&arp_timer, CLOCK_SECOND * 10);

timer_set(&dns_test, CLOCK_SECOND * 600);

    USART_Init(95);
    sendString("\E[H\E[J");
    sendString("Booting Biomass Ethernet\r\n");

/*
// debug why there was a reset
    if(resetSource & (1<<WDRF))
    {
        sendString("Mega was reset by watchdog...\r\n");
    }

    if(resetSource & (1<<BORF))
    {
        sendString("Mega was reset by brownout...\r\n");
    }

    if(resetSource & (1<<EXTRF))
    {
        sendString("Mega was reset by external...\r\n");
    }

    if(resetSource & (1<<PORF))
    {
        sendString("Mega was reset by power on...\r\n");
    }
*/
    uip_init();

    // must be done or sometimes arp doesn't work
    uip_arp_init();
    
    // load the network configs from eeprom
    net_conf_init();

    network_init_mac(net_conf_get_mac());

    if (net_conf_is_dhcpc())
    {
        // setup the dhcp renew timer and make the first request
        //timer_set(&dhcp_timer, CLOCK_SECOND * 3276); // 10 minutes till renew
        timer_set(&dhcp_timer, CLOCK_SECOND * 300); 
        dhcpc_init(net_conf_get_mac(), 6);
        dhcpc_request();
    }

    // start hosted services
    telnetd_init();
    httpd_init();
    resolv_init();

    // hard code resolver configuration for now
    uip_ipaddr_t ipaddr;
    uip_ipaddr(ipaddr, 192,168,2,1);
    resolv_conf(ipaddr);


/*
    adc_init();
*/

    while(1)
    {
        // reset the watch dog timer every time we loop around
        wdt_reset();

    if (timer_expired(&dns_test))
    {
//led_high();
sendString("\r\nDNS TEST:\r\n");
        resolv_lookup("www.yahoo.com");
        timer_reset(&dns_test);
    }

        uip_len = network_read();
        if(uip_len > 0)
        {
//sendString("\r\nnetwork data read");
            if(BUF->type == htons(UIP_ETHTYPE_IP))
            {
                //uip_arp_ipin(); // arp seems to have issues w/o this
                uip_input();
                if(uip_len > 0)
                {
                    uip_arp_out();
                    network_send();
                }
            }
            else if(BUF->type == htons(UIP_ETHTYPE_ARP))
            {
                uip_arp_arpin(); // this is correct
                if(uip_len > 0)
                {
                    network_send();
                }
            }
        }
        else if(timer_expired(&periodic_timer))
        {
            timer_reset(&periodic_timer);

            for(i = 0; i < UIP_CONNS; i++)
            {
                uip_periodic(i);
                if(uip_len > 0)
                {
                    uip_arp_out();
                    network_send();
                }
            }

            #if UIP_UDP
            for(i = 0; i < UIP_UDP_CONNS; i++)
            {
                uip_udp_periodic(i);
                if(uip_len > 0)
                {
                    uip_arp_out();
                    network_send();
                }
            }
            #endif /* UIP_UDP */

            if(timer_expired(&arp_timer))
            {
                timer_reset(&arp_timer);
                uip_arp_timer();
            }
        }
        else if (net_conf_is_dhcpc() && timer_expired(&dhcp_timer))
        {
            timer_reset(&dhcp_timer);
            dhcpc_renew();
        }
    }

    return 0;
}

#if UIP_CONF_LOGGING == 1
void uip_log(char *m)
{
//    sendString(m);
    printf("%s", m);
    //TODO: Get debug information out here somehow, does anybody know a smart way to do that?
}
#endif

/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/


void dhcpc_configured(const struct dhcpc_state *s)
{
    net_conf_set_ip_ipaddr(s->ipaddr);

    net_conf_set_nm_ipaddr(s->netmask);

    net_conf_set_gw_ipaddr(s->default_router);

    net_conf_uip_set();

//  code to use dhcp server lease time removed due to uint16_t overflow
//  issues with calculating the time.   
//  just do the reset in the main loop.
// WARNING, if you reset a the dhcp timer here and in main you will have
// issues where the timer keeps showing as expired :-\

#ifdef DHCP_DEBUG
    // for now turn on the led when we get an ip
    //led_high();
#endif

char ip[60];
  sprintf(ip, "\r\nDHCP ADDR %d.%d.%d.%d", 
                 htons(s->ipaddr[0]) >> 8,
                 htons(s->ipaddr[0]) & 0xff,
                 htons(s->ipaddr[1]) >> 8,
                 htons(s->ipaddr[1]) & 0xff);
  sendString(ip);
  sendString("\r\n");

}

/*---------------------------------------------------------------------------*/

/*void
smtp_done(unsigned char code)
{
	//printf("SMTP done with code %d\n", code);
}
void
webclient_closed(void)
{
	//printf("Webclient: connection closed\n");
}
void
webclient_aborted(void)
{
	//printf("Webclient: connection aborted\n");
}
void
webclient_timedout(void)
{
	//printf("Webclient: connection timed out\n");
}
void
webclient_connected(void)
{
	//printf("Webclient: connected, waiting for data...\n");
}
void
webclient_datahandler(char *data, u16_t len)
{
	//printf("Webclient: got %d bytes of data.\n", len);
}*/
/*---------------------------------------------------------------------------*/
