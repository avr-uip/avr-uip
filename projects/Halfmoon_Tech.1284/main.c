#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#include "global-conf.h"
#include "timer.h"
#include <util/delay.h>

//#include "a2d.h"
#include "uart-t.h"



#include "uip_arp.h"
#include "network.h"
#include "enc28j60.h"
#include "apps-conf.h"
#include <string.h>

#include "net_conf.h"

#define NUM_TEMP_SENSORS 8
#define NUM_TEMP_READINGS 16


// this may need to be a 32 bit number instead
uint16_t temp_sensors[NUM_TEMP_SENSORS];



/*
void read_sensors(void){
	extern int sensor0, sensor1, sensor2, sensor3, sensor4, sensor5, sensor6, sensor7;

	int ii, iii, sample,temp;
	for(iii=0;iii<8;iii++){
		sample=0;
		for(ii=0;ii<16;ii++){
			//sample += a2dConvert10bit(iii);
		}
		temp = (sample / 16);
		//temp = convert_adc2far(temp);
		if(iii==0){sensor0 = temp;}
		else if(iii==1){sensor1 = temp;}
		else if(iii==2){sensor2 = temp;}
		else if(iii==3){sensor3 = temp;}
		else if(iii==4){sensor4 = temp;}
		else if(iii==5){sensor5 = temp;}
		else if(iii==6){sensor6 = temp;}
		else if(iii==7){sensor7 = temp;}
	}

int convert_adc2far(int adc_data){
    float ttt;
    ttt = (adc_data * .0048828125 * 100);
    ttt = ttt * 1.8;
   return (ttt + 32);
}

void read_sensors(void)
{
    uint8_t sensor_index, read_number;
    uint16_t sample;
    for(sensor_index = 0; sensor_index < NUM_TEMP_SENSORS; sensor_index++)
    {
        sample=0;
        for(read_number = 0; read_number < NUM_TEMP_READINGS; read_number++)
        {
            //sample += a2dConvert10bit(sensor_number);
        }
        sample = (sample / NUM_TEMP_READINGS);
        sample = convert_adc2far(sample);
        temp_sensors[sensor_index] = sample;
    }
}
*/

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])
struct timer dhcp_timer;

int bob = 986;


//uint8_t EEMEM ee_tmp[4]={tt_timber0,tt_timber1,tt_timber2,tt_timber3};

//temp_data[0] = 0x11;
//temp_data[1] = 0x22;
//temp_data[2] = 0x33;
//temp_data[3] = 0x44;

int main(void)
{
    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();


//led_conf();

    CLKPR=(1<<CLKPCE); // change enable
    CLKPR=0; // "no pre-scaler"
    _delay_loop_1(0); // 60us

    int i;
    struct timer periodic_timer, arp_timer, sensor_timer;

    clock_init();

    timer_set(&periodic_timer, CLOCK_SECOND);
    timer_set(&arp_timer, CLOCK_SECOND * 10);
    timer_set(&sensor_timer, CLOCK_SECOND * 5);

    uip_init();

    // must be done or sometimes arp doesn't work
    uip_arp_init();
    
    // load the network configs from eeprom
    net_conf_init();

    network_init_mac(net_conf_get_mac());

    // init temp array
    for( i = 0; i < NUM_TEMP_SENSORS; i++)
    {
        temp_sensors[i] = 0;
    }

    if (net_conf_is_dhcpc())
    {
        // setup the dhcp renew timer the make the first request
        timer_set(&dhcp_timer, CLOCK_SECOND * 600); // 10 minutes till renew
        dhcpc_init(net_conf_get_mac(), 6);
        dhcpc_request();
    }

	uip_ipaddr_t ipaddr;
    resolv_init();
    uip_ipaddr(ipaddr, 8, 8, 8, 8);
    resolv_conf(ipaddr);
    webclient_init();

	// start hosted services
    telnetd_init();
    httpd_init();
	//a2dInit();
	USART_Init(95);
	sendString("\E[H\E[J");
	sendString("Booting Biomass Ethernet\r\n");
	
	//eeprom_update_byte  (&ee_tmp, net_conf_enable_dhcp);
	//eeprom_read_block ((void *)temp_data, (const void *)&ee_tmp,4);

	while(1)
    {
        
        if(timer_expired(&sensor_timer))
        {
            timer_reset(&sensor_timer);
			//read_sensors();
            webclient_get_P(PSTR("http://pyroperformances.com/"), 80, PSTR("index.html"));
        }

		uip_len = network_read();
        if(uip_len > 0)
        {
            if(BUF->type == htons(UIP_ETHTYPE_IP))
            {
                uip_arp_ipin(); // arp seems to have issues w/o this
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
