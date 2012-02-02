#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#include <prod_conf.h>

#include <ctype.h>


// TCP/IP parameters in data memory
uint8_t prod_conf_serial_number[10];
uint8_t prod_conf_firmware_aes_key[16];
uint8_t prod_conf_server_aes_key[16];
/*
 * upgrade flags are
 * 0x01 = may upgrade
 * 0x02 = upgrade via serial enable
 * 0x04 = upgrade via network enable
 */
uint8_t  prod_conf_upgrade_flags; // upgrade enable and flags
uint16_t prod_conf_rev; // source control rev number of current firmware

// OTHER 19 bytes are reserved at this time

//EEPROM parameters
//
// The following defines must stay in order
// or you will loose your settings on upgrade
//
#ifndef PROD_CONF_EEMEM_BASE
#define PROD_CONF_EEMEM_BASE 0x00  // start of serial number, flags, and upgrade server ip allocation space
#endif
// add to the END of the list!
#define PROD_CONF_EEMEM_SERIAL_NUMBER    PROD_CONF_EEMEM_BASE
#define PROD_CONF_EEMEM_FIRMWARE_AES_KEY PROD_CONF_EEMEM_SERIAL_NUMBER + sizeof(prod_conf_serial_number)
#define PROD_CONF_EEMEM_SERVER_AES_KEY   PROD_CONF_EEMEM_FIRMWARE_AES_KEY + sizeof(prod_conf_firmware_aes_key)
#define PROD_CONF_EEMEM_UPGRADE_FLAGS    PROD_CONF_EEMEM_SERVER_AES_KEY + sizeof(prod_conf_server_aes_key)
#define PROD_CONF_EEMEM_REV              PROD_CONF_EEMEM_UPGRADE_FLAGS + sizeof(prod_conf_upgrade_flags)

uint8_t *prod_conf_get_serial_number (void)
{
    return prod_conf_serial_number;
}

void prod_conf_set_serial_number (const uint8_t *new_serial_number)
{
    memcpy(prod_conf_serial_number, new_serial_number, sizeof(prod_conf_serial_number));
}

int8_t prod_conf_get_serial_number_string (char* sn_string, int8_t sn_string_len)
{
    return(snprintf_P(sn_string, sn_string_len,
                      PSTR("%02x%02x%02x%02x%02x%02x%02x%02x%02x"),
                      prod_conf_serial_number[0], prod_conf_serial_number[1],
                      prod_conf_serial_number[2], prod_conf_serial_number[3],
                      prod_conf_serial_number[4], prod_conf_serial_number[5],
                      prod_conf_serial_number[6], prod_conf_serial_number[7],
                      prod_conf_serial_number[8], prod_conf_serial_number[9]));
}

void prod_conf_set_firmware_aes_key (const uint8_t *new_aes_key)
{
    memcpy(prod_conf_firmware_aes_key, new_aes_key, sizeof(prod_conf_firmware_aes_key));
}

uint8_t *prod_conf_get_firmware_aes_key (void)
{
    return(prod_conf_firmware_aes_key);
}

void prod_conf_set_server_aes_key (const uint8_t *new_aes_key)
{
    memcpy(prod_conf_server_aes_key, new_aes_key, sizeof(prod_conf_server_aes_key));
}

uint8_t *prod_conf_get_server_aes_key (void)
{
    return(prod_conf_server_aes_key);
}



// 0 == off , 1 == on 
void prod_conf_set_may_upgrade(uint8_t mode)
{
    prod_conf_upgrade_flags &= 0xFE; // clear enable bit
    prod_conf_upgrade_flags |= (mode & 0x01); // only set single bit
}

// is the system in dhcp mode
// 0 == no, 1 == true
uint8_t prod_conf_may_upgrade(void)
{
    return (prod_conf_upgrade_flags & 0x1);
}

void prod_conf_set_upgrade_flags(uint8_t new_flags)
{
    prod_conf_upgrade_flags = new_flags;
}

uint8_t prod_conf_get_upgrade_flags(void)
{
    return (prod_conf_upgrade_flags);
}

void prod_conf_set_rev  (const uint16_t *new_rev)
{
    prod_conf_rev = *new_rev;
}

uint16_t prod_conf_get_rev (void)
{
    return (prod_conf_rev);
}

void prod_conf_save(void)
{
// update functions write if data is different
#if defined(eeprom_update_block)
    // added to avr-lib @ version 1.6.7
    eeprom_update_block ((const void *)prod_conf_serial_number, (void *)PROD_CONF_EEMEM_SERIAL_NUMBER, sizeof(prod_conf_serial_number));
    eeprom_update_block ((const void *)prod_conf_firmware_aes_key,(void *)PROD_CONF_EEMEM_FIRMWARE_AES_KEY, sizeof(prod_conf_firmware_aes_key));
    eeprom_update_block ((const void *)prod_conf_server_aes_key,(void *)PROD_CONF_EEMEM_SERVER_AES_KEY, sizeof(prod_conf_server_aes_key));
    eeprom_update_byte  ((uint8_t *)PROD_CONF_EEMEM_UPGRADE_FLAGS, prod_conf_upgrade_flags);
    eeprom_update_word  ((uint16_t *)PROD_CONF_EEMEM_REV, prod_conf_rev);
#else
    eeprom_write_block ((const void *)prod_conf_serial_number, (void *)PROD_CONF_EEMEM_SERIAL_NUMBER, sizeof(prod_conf_serial_number));
    eeprom_write_block ((const void *)prod_conf_firmware_aes_key,(void *)PROD_CONF_EEMEM_FIRMWARE_AES_KEY, sizeof(prod_conf_firmware_aes_key));
    eeprom_write_block ((const void *)prod_conf_server_aes_key,(void *)PROD_CONF_EEMEM_SERVER_AES_KEY, sizeof(prod_conf_server_aes_key));
    eeprom_write_byte  ((uint8_t *)PROD_CONF_EEMEM_UPGRADE_FLAGS, prod_conf_upgrade_flags);
    eeprom_write_word  ((uint16_t *)PROD_CONF_EEMEM_REV, prod_conf_rev);
#endif
}

void prod_conf_load(void)
{
    eeprom_read_block ((void *)prod_conf_serial_number, (const void *)PROD_CONF_EEMEM_SERIAL_NUMBER, sizeof(prod_conf_serial_number));
    eeprom_read_block ((void *)prod_conf_firmware_aes_key, (const void *)PROD_CONF_EEMEM_FIRMWARE_AES_KEY, sizeof(prod_conf_firmware_aes_key));
    eeprom_read_block ((void *)prod_conf_server_aes_key, (const void *)PROD_CONF_EEMEM_SERVER_AES_KEY, sizeof(prod_conf_server_aes_key));
    prod_conf_upgrade_flags = eeprom_read_byte((uint8_t *)PROD_CONF_EEMEM_UPGRADE_FLAGS);
    prod_conf_rev = eeprom_read_word((uint16_t *)PROD_CONF_EEMEM_REV);
}

