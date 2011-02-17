#ifndef __APPS_CONF_H__
#define __APPS_CONF_H__

//Here we include the header file for the application(s) we use in our project.

//#include "smtp.h"
//#include "hello-world.h"
//#include "simple-httpd.h"
//#include "telnetd.h"

/////////////////////////
// webserver options
// - uncomment to turn on POST support in the web server
//#define HTTP_POST_SUPPORT
// - the max amount of data which may be posted (bytes)
#define MAX_POST_DATA 80
#include "webserver.h"
/////////////////////////

#include "dhcpc.h"

//#include "resolv.h"
//#include "webclient.h"

#endif /*__APPS_CONF_H__*/
