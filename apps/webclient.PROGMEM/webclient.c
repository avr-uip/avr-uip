/**
 * \addtogroup apps
 * @{
 */

/**
 * \defgroup webclient Web client
 * @{
 *
 * This example shows a HTTP client that is able to download web pages
 * and files from web servers. It requires a number of callback
 * functions to be implemented by the module that utilizes the code:
 * webclient_datahandler(), webclient_connected(),
 * webclient_timedout(), webclient_aborted(), webclient_closed().
 */

/**
 * \file
 * Implementation of the HTTP client.
 * \author Adam Dunkels <adam@dunkels.com>
 */

/*
 * Copyright (c) 2002, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: webclient.c,v 1.2 2006/06/11 21:46:37 adam Exp $
 *
 */

#include "uip.h"
#include "uiplib.h"
#include "webclient.h"
#include "resolv.h"

#include "webstrings_common.h"

#include <string.h>
//#include <stdlib.h>
#include "uart-t.h"

#define WEBCLIENT_TIMEOUT 100

#define WEBCLIENT_STATE_CLOSE      0
#define WEBCLIENT_STATE_STATUSLINE 1
#define WEBCLIENT_STATE_HEADERS    2
#define WEBCLIENT_STATE_DATA       3

#define HTTPFLAG_NONE   0
#define HTTPFLAG_OK     1
#define HTTPFLAG_MOVED  2
#define HTTPFLAG_ERROR  3


#define ISO_nl       0x0a
#define ISO_cr       0x0d
#define ISO_space    0x20

#if defined PORT_APP_MAPPER
    #warning "Webclient may only connect to port 80 on remote hosts when using port_app_mapper"
#endif

#if UIP_CONF_LOGGING == 1
extern void uip_log(char*);
extern void uip_log_P(prog_char*);
#else
#define uip_log(A) {}
#define uip_log_P(A) {}
#endif

static struct webclient_state s;

/*-----------------------------------------------------------------------------------*/
char *
webclient_mimetype(void)
{
  return s.mimetype;
}
/*-----------------------------------------------------------------------------------*/
char *
webclient_filename(void)
{
  return s.file;
}
/*-----------------------------------------------------------------------------------*/
char *
webclient_hostname(void)
{
  return s.host;
}
/*-----------------------------------------------------------------------------------*/
unsigned short
webclient_port(void)
{
  return s.port;
}
/*-----------------------------------------------------------------------------------*/
void
webclient_init(void)
{
  s.state = WEBCLIENT_STATE_CLOSE;
}
/*-----------------------------------------------------------------------------------*/
static void
init_connection(void)
{
  s.state = WEBCLIENT_STATE_STATUSLINE;

  s.getrequestleft = sizeof(http_get) - 1 + 1 +
    sizeof(http_10) - 1 +
    sizeof(http_crnl) - 1 +
    sizeof(http_host) - 1 +
    sizeof(http_crnl) - 1 +
    strlen_P(http_user_agent_fields) +
    strlen(s.file) + strlen(s.host) +
    sizeof(http_crnl) - 1 +
    sizeof(http_crnl) - 1;
  s.getrequestptr = 0;

  s.httpheaderlineptr = 0;
}
/*-----------------------------------------------------------------------------------*/
void
webclient_close(void)
{
  // clean everything up
  init_connection();
  webclient_init();
  uip_close();
}
/*-----------------------------------------------------------------------------------*/
// only called vai the webclient_get functions
unsigned char
webclient_get_real(const char *host, u16_t port, const char *file)
{
  struct uip_conn *conn;
  uip_ipaddr_t *ipaddr;
  static uip_ipaddr_t addr;
  
  /* First check if the host is an IP address. */
  ipaddr = &addr;
  if(uiplib_ipaddrconv(host, (unsigned char *)addr) == 0) {
    ipaddr = (uip_ipaddr_t *)resolv_lookup((char*)host);  // cast away const unsafe!
    
    if(ipaddr == NULL) {
      return 0;
    }
  }
  
  conn = uip_connect(ipaddr, htons(port));
  
  if(conn == NULL) {
    return 0;
  }

  init_connection();

  return 1;
}

/*-----------------------------------------------------------------------------------*/
unsigned char
webclient_get(const char *host, u16_t port, const char *file)
{
  if(s.state != WEBCLIENT_STATE_CLOSE) {
    return 0;
  }

  s.port = port;
  strncpy(s.file, file, sizeof(s.file));
  strncpy(s.host, host, sizeof(s.host));

  return (webclient_get_real(s.host, s.port, s.file));  
}
/*-----------------------------------------------------------------------------------*/
unsigned char
webclient_get_P(const prog_char *host, u16_t port, const prog_char *file)
{
  if(s.state != WEBCLIENT_STATE_CLOSE) {
    return 0;
  }

  s.port = port;
  strcpy_P(s.host, host);
  strcpy_P(s.file, file);

  return (webclient_get_real(s.host, s.port, s.file));  
}
/*-----------------------------------------------------------------------------------*/
static char *
copy_string(char *dest,
	    const char *src, unsigned char len)
{
  strncpy(dest, src, len);
  return dest + len;
}
/*-----------------------------------------------------------------------------------*/
static char *
copy_string_P(char *dest,
	    const prog_char *src, unsigned char len)
{
  strncpy_P(dest, src, len);
  return dest + len;
}
/*-----------------------------------------------------------------------------------*/
static void
senddata(void)
{
  u16_t len;
  char *getrequest;
  char *cptr;
  
  if(s.getrequestleft > 0) {
    cptr = getrequest = (char *)uip_appdata;

    cptr = copy_string_P(cptr, http_get, sizeof(http_get) - 1);
    cptr = copy_string(cptr, s.file, strlen(s.file));
    *cptr++ = ISO_space;
    cptr = copy_string_P(cptr, http_10, sizeof(http_10) - 1);

    cptr = copy_string_P(cptr, http_crnl, sizeof(http_crnl) - 1);
    
    cptr = copy_string_P(cptr, http_host, sizeof(http_host) - 1);
    cptr = copy_string(cptr, s.host, strlen(s.host));
    cptr = copy_string_P(cptr, http_crnl, sizeof(http_crnl) - 1);

    cptr = copy_string_P(cptr, http_user_agent_fields,
		       strlen_P(http_user_agent_fields));
    cptr = copy_string_P(cptr, http_crnl, sizeof(http_crnl) - 1);
    cptr = copy_string_P(cptr, http_crnl, sizeof(http_crnl) - 1);
    
    len = s.getrequestleft > uip_mss()?
      uip_mss():
      s.getrequestleft;
    uip_send(&(getrequest[s.getrequestptr]), len);
  }
}
/*-----------------------------------------------------------------------------------*/
static void
acked(void)
{
  u16_t len;
  
  if(s.getrequestleft > 0) {
    len = s.getrequestleft > uip_mss()?
      uip_mss():
      s.getrequestleft;
    s.getrequestleft -= len;
    s.getrequestptr += len;
  }
}
/*-----------------------------------------------------------------------------------*/
char *uip_appdata_ptr;
/*-----------------------------------------------------------------------------------*/
static u16_t
parse_statusline(u16_t len)
{
  char *cptr;
  uint8_t result_code_at;
  
  while(len > 0 && s.httpheaderlineptr < sizeof(s.httpheaderline)) {
    s.httpheaderline[s.httpheaderlineptr] = *uip_appdata_ptr++;
    --len;
    if(s.httpheaderline[s.httpheaderlineptr] == ISO_nl) {

      result_code_at = 0; 
      if((strncmp_P(s.httpheaderline, http_10, sizeof(http_10) - 1) == 0))
        result_code_at = sizeof(http_10);
      else if((strncmp_P(s.httpheaderline, http_11, sizeof(http_11) - 1) == 0))
        result_code_at = sizeof(http_11);
      else if((strncmp_P(s.httpheaderline, http_icy, sizeof(http_icy) - 1) == 0))
        result_code_at = sizeof(http_icy);

      if(result_code_at) {
	cptr = &(s.httpheaderline[result_code_at]);
	s.httpflag = HTTPFLAG_NONE;
	if(strncmp_P(cptr, http_200, sizeof(http_200) - 1) == 0) {
	  /* 200 OK */
	  s.httpflag = HTTPFLAG_OK;
	  uip_log_P(PSTR("HTTP OK"));
	} else if(strncmp_P(cptr, http_301, sizeof(http_301) - 1) == 0 ||
		  strncmp_P(cptr, http_302, sizeof(http_302) - 1) == 0) {
	  /* 301 Moved permanently or 302 Found. Location: header line
	     will contain thw new location. */
	  s.httpflag = HTTPFLAG_MOVED;
	  uip_log_P(PSTR("HTTP MOVED"));
	} else {
	  s.httpheaderline[s.httpheaderlineptr - 1] = 0;
          uip_log(s.httpheaderline);
	}
      } else {
	uip_log_P(PSTR("HTTP FAILED"));
        s.httpheaderline[s.httpheaderlineptr - 1] = 0;
        uip_log(s.httpheaderline);
	uip_abort();
	webclient_aborted();
	return 0;
      }
      
      /* We're done parsing the status line, so we reset the pointer
	 and start parsing the HTTP headers.*/
      s.httpheaderlineptr = 0;
      s.state = WEBCLIENT_STATE_HEADERS;
      break;
    } else {
      ++s.httpheaderlineptr;
    }
  }
  return len;
}
/*-----------------------------------------------------------------------------------*/
#if 0
// defined but not used
static char
casecmp(char *str1, const char *str2, char len)
{
  static char c;
  
  while(len > 0) {
    c = *str1;
    /* Force lower-case characters. */
    if(c & 0x40) {
      c |= 0x20;
    }
    if(*str2 != c) {
      return 1;
    }
    ++str1;
    ++str2;
    --len;
  }
  return 0;
}
#endif
/*-----------------------------------------------------------------------------------*/
static char
casecmp_P(char *str1, const prog_char *str2, char len)
{
  static char c;
  
  while(len > 0) {
    c = *str1;
    /* Force lower-case characters. */
    if(c & 0x40) {
      c |= 0x20;
    }
    if(pgm_read_byte(str2) != c) {
      return 1;
    }
    ++str1;
    ++str2;
    --len;
  }
  return 0;
}
/*-----------------------------------------------------------------------------------*/
static u16_t
parse_headers(u16_t len)
{
  char *cptr;
  static unsigned char i;

  while(len > 0 && s.httpheaderlineptr < sizeof(s.httpheaderline)) {
    s.httpheaderline[s.httpheaderlineptr] = *uip_appdata_ptr++;
    --len;
    if(s.httpheaderline[s.httpheaderlineptr] == ISO_nl) {
      /* We have an entire HTTP header line in s.httpheaderline, so
	 we parse it. */
      if(s.httpheaderline[0] == ISO_cr) {
	/* This was the last header line (i.e., and empty "\r\n"), so
	   we are done with the headers and proceed with the actual
	   data. */
	s.state = WEBCLIENT_STATE_DATA;
        uip_log_P(PSTR("WEBCLIENT_STATE_DATA"));
	return len;
      }

      s.httpheaderline[s.httpheaderlineptr - 1] = 0;
      uip_log(s.httpheaderline);
      /* Check for specific HTTP header fields. */
      if(casecmp_P(s.httpheaderline, http_content_type,
		     sizeof(http_content_type) - 1) == 0) {
	/* Found Content-type field. */
	cptr = strchr((s.httpheaderline + sizeof(http_content_type)), ISO_cr);
	if(cptr != NULL) {
	  *cptr = 0;
	}
        snprintf(s.mimetype, sizeof(s.mimetype), "%s",
                 (s.httpheaderline + sizeof(http_content_type) - 1));        

      } else if(casecmp_P(s.httpheaderline, http_location,
			    sizeof(http_location) - 1) == 0) {
	cptr = s.httpheaderline +
	  sizeof(http_location) - 1;
	
	if(strncmp_P(cptr, http_http, 7) == 0) {
	  cptr += 7;
	  for(i = 0; i < s.httpheaderlineptr - 7; ++i) {
	    if(*cptr == 0 ||
	       *cptr == '/' ||
	       *cptr == ' ' ||
	       *cptr == ':') {
	      s.host[i] = 0;
	      break;
	    }
	    s.host[i] = *cptr;
	    ++cptr;
	  }
	}
	strncpy(s.file, cptr, sizeof(s.file));
	//	s.file[s.httpheaderlineptr - i] = 0;
      }


      /* We're done parsing, so we reset the pointer and start the
	 next line. */
      s.httpheaderlineptr = 0;
    } else {
      ++s.httpheaderlineptr;
    }
  }

  /* Handle header line overflow */
  if ( s.httpheaderlineptr == sizeof(s.httpheaderline) ) {
    s.httpheaderline[s.httpheaderlineptr - i] = 0;
    uip_log(s.httpheaderline);
      s.httpheaderlineptr = 0;
  }
  return len;
}
/*-----------------------------------------------------------------------------------*/
static void
newdata(void)
{
  u16_t len;

  len = uip_datalen();
  uip_appdata_ptr = uip_appdata;

  if(s.state == WEBCLIENT_STATE_STATUSLINE) {
    len = parse_statusline(len);
  }
  
  if(s.state == WEBCLIENT_STATE_HEADERS && len > 0) {
    len = parse_headers(len);
  }

  if(len > 0 && s.state == WEBCLIENT_STATE_DATA &&
     s.httpflag != HTTPFLAG_MOVED) {
    webclient_datahandler((char *)(uip_appdata + (uip_datalen() - len)), len);
  }
}
/*-----------------------------------------------------------------------------------*/
void
webclient_appcall(void)
{
  if(uip_connected()) {
    s.timer = 0;
    s.state = WEBCLIENT_STATE_STATUSLINE;
    senddata();
    webclient_connected();
    return;
  }

  if(s.state == WEBCLIENT_STATE_CLOSE) {
    webclient_closed();
    uip_abort();
    return;
  }

  if(uip_aborted()) {
    webclient_aborted();
  }
  if(uip_timedout()) {
    webclient_timedout();
  }

  
  if(uip_acked()) {
    s.timer = 0;
    acked();
  }
  if(uip_newdata()) {
    s.timer = 0;
    newdata();
  }
  if(uip_rexmit() ||
     uip_newdata() ||
     uip_acked()) {
    senddata();
  } else if(uip_poll()) {
    ++s.timer;
    if(s.timer == WEBCLIENT_TIMEOUT) {
      webclient_timedout();
      uip_abort();
      return;
    }
        /*    senddata();*/
  }

  if(uip_closed()) {
sendString("webclinet: in the app call connection closed\r\n");
/*
    if(s.httpflag != HTTPFLAG_MOVED) {
      // Send NULL data to signal EOF.
      webclient_datahandler(NULL, 0);
    } else {
      if(resolv_lookup(s.host) == NULL) {
	resolv_query(s.host);
      }
      webclient_get(s.host, s.port, s.file);
    }
*/
    
  }
}
/*-----------------------------------------------------------------------------------*/
#ifndef WEBCLIENT_CALLBACKS_CUSTOM
void
webclient_closed(void)
{
  sendString("Webclient: connection closed\r\n");
}
void
webclient_aborted(void)
{
  sendString("Webclient: connection aborted\r\n");
  webclient_close();
}
void
webclient_timedout(void)
{
  sendString("Webclient: connection timed out\r\n");
  webclient_close();
}
void
webclient_connected(void)
{
  sendString("Webclient: connected, waiting for data...\r\n");
}
#endif
/** @} */
/** @} */
