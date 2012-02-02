//Project specific configurations
#ifndef __PROD_CONF_H__
#define __PROD_CONF_H__

// move net_conf out to make space for prod_conf
#define NET_CONF_EEMEM_BASE 0x040

#define PROD_CONF_MAY_UPGRADE     0x01 // may upgrade
#define PROD_CONF_SERIAL_UPGRADE  0x02 // upgrade via serial enable
#define PROD_CONF_NETWORK_UPGRADE 0x04 // upgrade via network enable
#define PROD_CONF_PERFORM_UPGRADE 0x80 // tells the boot loader
                                       // if network upgrade, check external
                                       // storage for an image.
                                       // if no network or no image, wait on
                                       // serial port for upgrade.

// function protos
uint8_t *prod_conf_get_serial_number (void);
void     prod_conf_set_serial_number (const uint8_t *new_serial_number);
int8_t   prod_conf_get_serial_number_string (char* sn_string, int8_t sn_string_len);

void     prod_conf_set_aes_key (const uint8_t *new_aes_key);
uint8_t *prod_conf_get_aes_key (void);

void     prod_conf_set_rev  (const uint16_t *new_rev);
uint16_t *prod_conf_get_rev (void);

void    prod_conf_set_may_upgrade   (uint8_t mode);
uint8_t prod_conf_may_upgrade       (void);
void    prod_conf_set_upgrade_flags (uint8_t new_flags);
uint8_t prod_conf_get_upgrade_flags (void);

void prod_conf_save (void);
void prod_conf_load (void);

#endif /*__PROD_CONF_H__*/
