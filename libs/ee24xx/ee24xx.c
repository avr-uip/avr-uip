/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 */
/*
 * ----------------------------------------------------------------------------
 * Updated to handle larger devices having 16-bit addresses
 *                                                 (2007-09-05) Ruwan Jayanetti
 * ----------------------------------------------------------------------------
 */

/* $Id: twitest.c 2187 2010-09-22 11:34:27Z aboyapati $ */

/*
 * Simple demo program that talks to a 24Cxx I�C EEPROM using the
 * builtin TWI interface of an ATmega device.
 */

/*
 * MOD example code into a lib for use in avr-uip project
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include <avr/io.h>
#include <util/twi.h>		/* Note [1] */

#define DEBUG 1


/*
 * Note [3]
 * TWI address for 24Cxx EEPROM:
 *
 * 1 0 1 0 E2 E1 E0 R/~W	24C01/24C02
 * 1 0 1 0 E2 E1 A8 R/~W	24C04
 * 1 0 1 0 E2 A9 A8 R/~W	24C08
 * 1 0 1 0 A10 A9 A8 R/~W	24C16
 */
#define TWI_SLA_24CXX	0xa0	/* E2 E1 E0 = 0 0 0 */

/*
 * Note [3a]
 * Device word address length for 24Cxx EEPROM
 * Larger EEPROM devices (from 24C32) have 16-bit address
 * Define or undefine according to the used device
 */
#define WORD_ADDRESS_16BIT // enable by default as avr-uip plans to use large chips.

/*
 * Maximal number of iterations to wait for a device to respond for a
 * selection.  Should be large enough to allow for a pending write to
 * complete, but low enough to properly abort an infinite loop in case
 * a slave is broken or not present at all.  With 100 kHz TWI clock,
 * transfering the start condition and SLA+R/W packet takes about 10
 * �s.  The longest write period is supposed to not exceed ~ 10 ms.
 * Thus, normal operation should not require more than 100 iterations
 * to get the device to respond to a selection.
 */
#define MAX_ITER	200

/*
 * Number of bytes that can be written in a row, see comments for
 * ee24xx_write_page() below.  Some vendor's devices would accept 16,
 * but 8 seems to be the lowest common denominator.
 *
 * Note that the page size must be a power of two, this simplifies the
 * page boundary calculations below.
 */
#define PAGE_SIZE 8

/*
 * Saved TWI status register, for error messages only.  We need to
 * save it in a variable, since the datasheet only guarantees the TWSR
 * register to have valid contents while the TWINT bit in TWCR is set.
 */
uint8_t twst;

/*
 * init TWI clock.
 */
void
ee24xx_twi_init(void)
{

#if F_CPU < 3600000UL
  TWBR = 10;			/* smallest TWBR value, see note [5] */
#else
  TWBR = (F_CPU / 100000UL - 16) / 2;
#endif
}


/*
 * Note [7]
 *
 * Read "len" bytes from EEPROM starting at "eeaddr" into "buf".
 *
 * This requires two bus cycles: during the first cycle, the device
 * will be selected (master transmitter mode), and the address
 * transfered.
 * Address bits exceeding 256 are transfered in the
 * E2/E1/E0 bits (subaddress bits) of the device selector.
 * Address is sent in two dedicated 8 bit transfers
 * for 16 bit address devices (larger EEPROM devices)
 *
 * The second bus cycle will reselect the device (repeated start
 * condition, going into master receiver mode), and transfer the data
 * from the device to the TWI master.  Multiple bytes can be
 * transfered by ACKing the client's transfer.  The last transfer will
 * be NACKed, which the client will take as an indication to not
 * initiate further transfers.
 */
int
ee24xx_read_bytes(uint16_t eeaddr, int len, uint8_t *buf)
{
  uint8_t sla, twcr, n = 0;
  int rv = 0;

#ifndef WORD_ADDRESS_16BIT
  /* patch high bits of EEPROM address into SLA */
  sla = TWI_SLA_24CXX | (((eeaddr >> 8) & 0x07) << 1);
#else
  /* 16-bit address devices need only TWI Device Address */
  sla = TWI_SLA_24CXX;
#endif

  /*
   * Note [8]
   * First cycle: master transmitter mode
   */
  restart:
  if (n++ >= MAX_ITER)
    return -1;
  begin:

  TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); /* send start condition */
  while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
  switch ((twst = TW_STATUS))
    {
    case TW_REP_START:		/* OK, but should not happen */
    case TW_START:
      break;

    case TW_MT_ARB_LOST:	/* Note [9] */
      goto begin;

    default:
      return -1;		/* error: not in start condition */
				/* NB: do /not/ send stop condition */
    }

  /* Note [10] */
  /* send SLA+W */
  TWDR = sla | TW_WRITE;
  TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
  while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
  switch ((twst = TW_STATUS))
    {
    case TW_MT_SLA_ACK:
      break;

    case TW_MT_SLA_NACK:	/* nack during select: device busy writing */
				/* Note [11] */
      goto restart;

    case TW_MT_ARB_LOST:	/* re-arbitrate */
      goto begin;

    default:
      goto error;		/* must send stop condition */
    }

#ifdef WORD_ADDRESS_16BIT
  TWDR = (eeaddr >> 8);		/* 16-bit word address device, send high 8 bits of addr */
  TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
  while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
  switch ((twst = TW_STATUS))
    {
    case TW_MT_DATA_ACK:
      break;

    case TW_MT_DATA_NACK:
      goto quit;

    case TW_MT_ARB_LOST:
      goto begin;

    default:
      goto error;		/* must send stop condition */
    }
#endif

  TWDR = eeaddr;		/* low 8 bits of addr */
  TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
  while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
  switch ((twst = TW_STATUS))
    {
    case TW_MT_DATA_ACK:
      break;

    case TW_MT_DATA_NACK:
      goto quit;

    case TW_MT_ARB_LOST:
      goto begin;

    default:
      goto error;		/* must send stop condition */
    }

  /*
   * Note [12]
   * Next cycle(s): master receiver mode
   */
  TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); /* send (rep.) start condition */
  while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
  switch ((twst = TW_STATUS))
    {
    case TW_START:		/* OK, but should not happen */
    case TW_REP_START:
      break;

    case TW_MT_ARB_LOST:
      goto begin;

    default:
      goto error;
    }

  /* send SLA+R */
  TWDR = sla | TW_READ;
  TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
  while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
  switch ((twst = TW_STATUS))
    {
    case TW_MR_SLA_ACK:
      break;

    case TW_MR_SLA_NACK:
      goto quit;

    case TW_MR_ARB_LOST:
      goto begin;

    default:
      goto error;
    }

  for (twcr = _BV(TWINT) | _BV(TWEN) | _BV(TWEA) /* Note [13] */;
       len > 0;
       len--)
    {
      if (len == 1)
	twcr = _BV(TWINT) | _BV(TWEN); /* send NAK this time */
      TWCR = twcr;		/* clear int to start transmission */
      while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
      switch ((twst = TW_STATUS))
	{
	case TW_MR_DATA_NACK:
	  len = 0;		/* force end of loop */
	  /* FALLTHROUGH */
	case TW_MR_DATA_ACK:
	  *buf++ = TWDR;
	  rv++;
	  break;

	default:
	  goto error;
	}
    }
  quit:
  /* Note [14] */
  TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); /* send stop condition */

  return rv;

  error:
  rv = -1;
  goto quit;
}

/*
 * Write "len" bytes into EEPROM starting at "eeaddr" from "buf".
 *
 * This is a bit simpler than the previous function since both, the
 * address and the data bytes will be transfered in master transmitter
 * mode, thus no reselection of the device is necessary.  However, the
 * EEPROMs are only capable of writing one "page" simultaneously, so
 * care must be taken to not cross a page boundary within one write
 * cycle.  The amount of data one page consists of varies from
 * manufacturer to manufacturer: some vendors only use 8-byte pages
 * for the smaller devices, and 16-byte pages for the larger devices,
 * while other vendors generally use 16-byte pages.  We thus use the
 * smallest common denominator of 8 bytes per page, declared by the
 * macro PAGE_SIZE above.
 *
 * The function simply returns after writing one page, returning the
 * actual number of data byte written.  It is up to the caller to
 * re-invoke it in order to write further data.
 */
int
ee24xx_write_page(uint16_t eeaddr, int len, uint8_t *buf)
{
  uint8_t sla, n = 0;
  int rv = 0;
  uint16_t endaddr;

  if (eeaddr + len <= (eeaddr | (PAGE_SIZE - 1)))
    endaddr = eeaddr + len;
  else
    endaddr = (eeaddr | (PAGE_SIZE - 1)) + 1;
  len = endaddr - eeaddr;

#ifndef WORD_ADDRESS_16BIT
  /* patch high bits of EEPROM address into SLA */
  sla = TWI_SLA_24CXX | (((eeaddr >> 8) & 0x07) << 1);
#else
  /* 16-bit address devices need only TWI Device Address */
  sla = TWI_SLA_24CXX;
#endif

  restart:
  if (n++ >= MAX_ITER)
    return -1;
  begin:

  /* Note [15] */
  TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); /* send start condition */
  while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
  switch ((twst = TW_STATUS))
    {
    case TW_REP_START:		/* OK, but should not happen */
    case TW_START:
      break;

    case TW_MT_ARB_LOST:
      goto begin;

    default:
      return -1;		/* error: not in start condition */
				/* NB: do /not/ send stop condition */
    }

  /* send SLA+W */
  TWDR = sla | TW_WRITE;
  TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
  while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
  switch ((twst = TW_STATUS))
    {
    case TW_MT_SLA_ACK:
      break;

    case TW_MT_SLA_NACK:	/* nack during select: device busy writing */
      goto restart;

    case TW_MT_ARB_LOST:	/* re-arbitrate */
      goto begin;

    default:
      goto error;		/* must send stop condition */
    }

#ifdef WORD_ADDRESS_16BIT
  TWDR = (eeaddr>>8);		/* 16 bit word address device, send high 8 bits of addr */
  TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
  while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
  switch ((twst = TW_STATUS))
    {
    case TW_MT_DATA_ACK:
      break;

    case TW_MT_DATA_NACK:
      goto quit;

    case TW_MT_ARB_LOST:
      goto begin;

    default:
      goto error;		/* must send stop condition */
    }
#endif

  TWDR = eeaddr;		/* low 8 bits of addr */
  TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
  while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
  switch ((twst = TW_STATUS))
    {
    case TW_MT_DATA_ACK:
      break;

    case TW_MT_DATA_NACK:
      goto quit;

    case TW_MT_ARB_LOST:
      goto begin;

    default:
      goto error;		/* must send stop condition */
    }

  for (; len > 0; len--)
    {
      TWDR = *buf++;
      TWCR = _BV(TWINT) | _BV(TWEN); /* start transmission */
      while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
      switch ((twst = TW_STATUS))
	{
	case TW_MT_DATA_NACK:
	  goto error;		/* device write protected -- Note [16] */

	case TW_MT_DATA_ACK:
	  rv++;
	  break;

	default:
	  goto error;
	}
    }
  quit:
  TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); /* send stop condition */

  return rv;

  error:
  rv = -1;
  goto quit;
}

/*
 * Wrapper around ee24xx_write_page() that repeats calling this
 * function until either an error has been returned, or all bytes
 * have been written.
 */
int
ee24xx_write_bytes(uint16_t eeaddr, int len, uint8_t *buf)
{
  int rv, total;

  total = 0;
  do
    {
#if DEBUG
      printf("Calling ee24xx_write_page(%d, %d, %p)",
	     eeaddr, len, buf);
#endif
      rv = ee24xx_write_page(eeaddr, len, buf);
#if DEBUG
      printf(" => %d\n", rv);
#endif
      if (rv == -1)
	return -1;
      eeaddr += rv;
      len -= rv;
      buf += rv;
      total += rv;
    }
  while (len > 0);

  return total;
}

