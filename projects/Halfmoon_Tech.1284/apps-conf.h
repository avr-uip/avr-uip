#ifndef __APPS_CONF_H__
#define __APPS_CONF_H__

//Here we include the header file for the application(s) we use in our project.

#define PORT_APP_MAPPER 1

//#include "smtp.h"
//#include "hello-world.h"
//#include "simple-httpd.h"
#define NETWORK_CMD 1
#include "telnetd.h"
//#include "udpds.h"
/////////////////////////
// webserver options
// - uncomment to turn on POST support in the web server
//#define HTTP_POST_SUPPORT
// - uncomment to turn on param support for http GET requests in the web server
#define HTTP_GET_PARAM_SUPPORT
// - the max amount of data which may be posted or param in get (bytes)
#define MAX_PARAM_DATA 100
#define HTTPD_MAX_CONNECTIONS 4
//#include "webserver.h"
#include "dhcpc.h"
#include "resolv.h"
#include "webclient.h"

//#define HTTP_POST_SUPPORT

#if defined PORT_APP_MAPPER
	#include "port_app_mapper.h"
#endif

#endif /*__APPS_CONF_H__*/
