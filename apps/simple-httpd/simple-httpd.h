#ifndef __SIMPLE_HTTPD_H__
#define __SIMPLE_HTTPD_H__
#include "psock.h"

typedef struct simple_httpd_state {
	struct psock p;
} uip_tcp_appstate_t;

void simple_httpd_appcall(void);
#ifndef UIP_APPCALL
#define UIP_APPCALL simple_httpd_appcall
#endif /* UIP_APPCALL */

void simple_httpd_init(void);

#endif /* __SIMPLE_HTTPD_H__ */
