#include "uip.h"
#include "uiplib.h"
#include <string.h>
#include "port_app_mapper.h"
#include "global-conf.h"

//#include "usart.h"

#ifndef WEBSERVER_APP_CALL_MAP
#define WEBSERVER_APP_CALL_MAP
#endif

#ifndef WEBCLIENT_APP_CALL_MAP
#define WEBCLIENT_APP_CALL_MAP
#endif

#ifndef TELNET_APP_CALL_MAP
#define TELNET_APP_CALL_MAP
#endif

#ifndef SIMPLE_HTTPD_APP_CALL_MAP
#define SIMPLE_HTTPD_APP_CALL_MAP
#endif

#ifndef DHCPC_APP_CALL_MAP
#define DHCPC_APP_CALL_MAP
#endif

#ifndef RESOLV_APP_CALL_MAP
#define RESOLV_APP_CALL_MAP
#endif

u8_t myflags;
struct pt port_thread;

struct port_appcall_map tcp_port_app_map[] = {
		WEBSERVER_APP_CALL_MAP
		WEBCLIENT_APP_CALL_MAP
		TELNET_APP_CALL_MAP
		SIMPLE_HTTPD_APP_CALL_MAP
		{NULL, 0, 0},
	};

struct port_appcall_map udp_port_app_map[] = {
		DHCPC_APP_CALL_MAP
		RESOLV_APP_CALL_MAP
		{NULL, 0, 0},
	};

// define a basic common type for udp and tcp connections so I can look at the ports
struct uip_base_conn {
  uip_ipaddr_t ripaddr;   /**< The IP address of the remote host. */

  u16_t lport;        /**< The local TCP port, in network byte order. */
  u16_t rport; 
};

struct uip_base_conn *base_conn;


// this function maps the current packet against the mappings
//   pointed to by the param
void uip_port_app_mapper(struct port_appcall_map* cur_map)
{
char str[60];
	// save a copy of the current flags as they will change which each appcall
	myflags = uip_flags;
sendString("\r\n----------------------\r\n");
sendString("\r\nUIP  CONN INFO \r\n");
sprintf(str, "UIP - R:%d - L:%d\r\n", (unsigned short)HTONS(uip_conn->rport), (unsigned short)HTONS(uip_conn->lport));
sendString(str);
if (base_conn != NULL)
{
sprintf(str, "UIP base - R:%d - L:%d\r\n", HTONS(base_conn->rport), HTONS(base_conn->lport));
sendString(str);
}
/*
	// hack for now.. only dhcp or one udp app will work.
	if ((cur_map == udp_port_app_map) && 
		(cur_map->an_appcall != NULL))
	{
//		sendString("\n\rcalling UDP app\n\r");
		cur_map->an_appcall();

		return;
	}
*/
	// yes this will walk the netire list which is 2 items at the moment
	while (cur_map->an_appcall != NULL)
	{
/*		// if timeout case, we need to call all apps
		if ((myflags == UIP_TIMEDOUT) || (myflags == UIP_POLL) )
		{
sendString("app call timeout case\r\n");
			// always restore the flags we last had
			// each time we make the timeout call
			uip_flags = myflags;
			cur_map->an_appcall();
		}
		else
		{
*/sendString("\r\napp call else case\r\n");
sprintf(str, "R:%d - L:%d\r\n", cur_map->rport, cur_map->lport);
sendString(str);
			// Now match the app to the packet.
			// local AND/OR remote ports match
			// firs check remote port matches and local may or may not
			// then check local port matches
			// can't do it in one statement due to the l & r ports could both be zero and match all apps.

			if (((base_conn->rport == HTONS(cur_map->rport)) &&
				 ((cur_map->lport == 0) || (base_conn->lport == HTONS(cur_map->lport)))) ||
				((base_conn->lport == HTONS(cur_map->lport)) &&
				 ((cur_map->rport == 0) || (base_conn->rport == HTONS(cur_map->rport)))))
			{
sendString("\r\nCON MATCH \r\n");
sprintf(str, "UIP MATCH - R:%d - L:%d\r\n", HTONS(base_conn->rport), HTONS(base_conn->lport) );
sendString(str);
sendString("UIP  CONN INFO \r\n");
				cur_map->an_appcall();
base_conn = NULL;
				break;
			}
//		}
		cur_map++;
	}
}

// wrapper around the uip_port_app_mapper
// pass it the list of apps to match against
void uip_udp_appcall_port_app_mapper()
{
sendString("\r\nudp app call\r\n");
    base_conn = (struct uip_base_conn *) uip_udp_conn;
	uip_port_app_mapper(udp_port_app_map);
}

void uip_appcall_port_app_mapper()
{
    base_conn = (struct uip_base_conn *) uip_conn;
	uip_port_app_mapper(tcp_port_app_map);
}

//    printf("GW : %u %u %u\r\n",ntohs(uip_conn->lport), ntohs(uip_conn->rport),uip_ipaddr1(uip_conn->ripaddr));
/*
    if (uip_conn->lport == HTONS(80))
    {
      httpd_appcall();
    }
    else if (uip_conn->rport == HTONS(80) || uip_conn->rport == HTONS(26600))
    {
      webclient_app();
   } else if (uip_conn->lport == HTONS(1000))
   {
     // not implemented lcd_appcall();
   } else {
       printf("Unknown packet ... \r\n");
       printf("rport : %u\r\n",ntohs(uip_conn->rport));
       printf("lport : %u\r\n",ntohs(uip_conn->lport));
       printf("LEN : %u\r\n",uip_conn->len);
       printf("IP : %u.%u.%u.%u\r\n",uip_ipaddr1(uip_conn->ripaddr),uip_ipaddr2(uip_conn->ripaddr),uip_ipaddr3(uip_conn->ripaddr),uip_ipaddr4(uip_conn->ripaddr));
   }
   */


