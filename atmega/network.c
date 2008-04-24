#include "enc28j60.h"
#include <avr/io.h>
#include <util/delay.h>
#include "uip.h"
unsigned int network_read(void){
	uint16_t len;
	len=enc28j60PacketReceive(UIP_BUFSIZE, (uint8_t *)uip_buf);
	return len;
}

void network_send(void){
	if(uip_len <= UIP_LLH_LEN + 40){
		enc28j60PacketSend(uip_len, (uint8_t *)uip_buf, 0, 0);
	}else{
		enc28j60PacketSend(54, (uint8_t *)uip_buf , uip_len - UIP_LLH_LEN - 40, (uint8_t*)uip_appdata);
	}
}

void network_init(void){
	CLKPR = (1<<CLKPCE);
	CLKPR = 0;
	_delay_ms(1);

	/* enable PD2/INT0, as input */
	DDRD&= ~(1<<DDD2);
	
	/*initialize enc28j60*/
	enc28j60Init();
	enc28j60clkout(2);// change clkout from 6.25MHz to 12.5MHz
	_delay_ms(10);

	// Magjack leds configuration, see enc28j60 datasheet, page 11
	// LEDB=yellow LEDA=green
	//
	// 0x476 is PHLCON LEDA=links status, LEDB=receive/transmit
	// enc28j60PhyWrite(PHLCON,0b0000 0100 0111 01 10);
	enc28j60PhyWrite(PHLCON,0x476);
	_delay_ms(20);
}

