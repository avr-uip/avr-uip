//Direkte kopi fra TuxGraphics softwaren, bliver vist også brugt i mange andre projekter som kompatibilitets lag
/* vim: sw=8 ts=8 si et: */
/****************************************************************************
* Title   :   compatibility module for forward compatibility with
              newer ARV C compiler which does not have older
              macros
* Authors:    Guido Socher
* Copyright: GPL
*
*        
*****************************************************************************/
#ifndef AVR_COMPAT_H
#define AVR_COMPAT_H

// uncomment the following line if you have avr-1.2.x or less:
//#define ALIBC_OLD 1

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#ifndef inb
#define inb(sfr) _SFR_BYTE(sfr) 
#endif

#ifndef outb
#define outb(sfr, val) (_SFR_BYTE(sfr) = (val))
#endif

#ifndef inw
#define inw(sfr) _SFR_WORD(sfr)
#endif

#ifndef outw
#define outw(sfr, val) ( _SFR_WORD(sfr) = (val)) 
#endif

#ifndef outp
#define outp(val, sfr) outb(sfr, val) 
#endif

#ifndef inp
#define inp(sfr) inb(sfr) 
#endif

#ifndef BV
#define BV(bit) _BV(bit)
#endif

/* compatibility macro for libc 1.0 to 1.2 */
#ifndef PRG_RDB
#define PRG_RDB(addr)       pgm_read_byte(addr)
#endif

//compatibility with old avr-libc BUG:http://www.mail-archive.com/avr-libc-dev@nongnu.org/msg01509.html
/*
#ifndef SPSR
#define SPSR    SPSR0
#endif

#ifndef SPIF
#define SPIF    SPIF0
#endif

#ifndef SPDR
#define SPDR    SPDR0
#endif

#ifndef SPCR
#define SPCR    SPCR0
#endif

#ifndef SPE
#define SPE     SPE0
#endif

#ifndef MSTR
#define MSTR    MSTR0
#endif

#ifndef SPI2X
#define SPI2X   SPI2X0
#endif
*/


#endif //AVR_COMPAT_H

