#ifndef __UIP_CONF_H__
#define __UIP_CONF_H__

#include <inttypes.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdbool.h>

/**
 * 8 bit datatype
 *
 * This typedef defines the 8-bit type used throughout uIP.
 *
 * \hideinitializer
 */
typedef uint8_t u8_t;

/**
 * 16 bit datatype
 *
 * This typedef defines the 16-bit type used throughout uIP.
 *
 * \hideinitializer
 */
typedef uint16_t u16_t;

/**
 * Statistics datatype
 *
 * This typedef defines the dataype used for keeping statistics in
 * uIP.
 *
 * \hideinitializer
 */
typedef unsigned short uip_stats_t;

/**
 * Maximum number of TCP connections.
 *
 * \hideinitializer
 */
#define UIP_CONF_MAX_CONNECTIONS 1

/**
 * Maximum number of listening TCP ports.
 *
 * \hideinitializer
 */
#define UIP_CONF_MAX_LISTENPORTS 1

/**
 * uIP buffer size.
 *
 * \hideinitializer
 */
#define UIP_CONF_BUFFER_SIZE     300
/**
 * CPU byte order.
 *
 * \hideinitializer
 */
#define UIP_CONF_BYTE_ORDER      LITTLE_ENDIAN

/**
 * Logging on or off
 *
 * \hideinitializer
 */
#define UIP_CONF_LOGGING         0

/**
 * UDP support on or off
 *
 * \hideinitializer
 */
#define UIP_CONF_UDP             0

/**
 * UDP checksums on or off
 *
 * \hideinitializer
 */
#define UIP_CONF_UDP_CHECKSUMS   0

/**
 * uIP statistics on or off
 *
 * \hideinitializer
 */
#define UIP_CONF_STATISTICS      0

/**
 * Broadcast support.
 *
 * \hideinitializer
 */
#define UIP_CONF_BROADCAST		0

/**
 * The maximum amount of concurrent UDP connections.
 *
 * \hideinitializer
 */
#define UIP_CONF_UDP_CONNS		1

//Include app configuration
#include "apps-conf.h"

#endif /* __UIP_CONF_H__ */

/** @} */
/** @} */
