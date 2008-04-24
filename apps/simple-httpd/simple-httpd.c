
#include "uip.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "simple-httpd.h"
static int handle_connection(struct simple_httpd_state *s);

void simple_httpd_init(void)
{
  uip_listen(HTONS(80));
}

void simple_httpd_appcall(void)
{
  struct simple_httpd_state *s = &(uip_conn->appstate);

  if(uip_connected()) {
		PSOCK_INIT(&s->p, NULL, 0);
  }

  handle_connection(s);
}

static int handle_connection(struct simple_httpd_state *s)
{
  PSOCK_BEGIN(&s->p);
  PSOCK_SEND_STR(&s->p, "HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\n\r\n\Hello World, From a simple httpd.");
  PSOCK_CLOSE(&s->p);
  PSOCK_END(&s->p);
}
