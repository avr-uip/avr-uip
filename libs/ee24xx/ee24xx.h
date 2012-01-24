#ifndef __EE24XX_H__
#define __EE24XX_H__

int ee24xx_read_bytes(uint16_t eeaddr, int len, uint8_t *buf);
int ee24xx_write_bytes(uint16_t eeaddr, int len, uint8_t *buf);

#endif // end - ifndef __EE24XX_H__
