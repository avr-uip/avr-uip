#include "global-conf.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>

#include "clock-arch.h"

//Counted time
clock_time_t clock_datetime = 0;


//Overflow itnerrupt
ISR(TIMER1_COMPA_vect)
{
    clock_datetime += 1;
}

//Initialise the clock
void clock_init()
{
    cli(); // stop interrupts while we meddle with the clocks

    TCNT1=0x0000; // reset counter to 0 at start
    TCCR1B |= (1 << WGM12) | (1 << CS12); // Configure timer 1 for CTC mode
    OCR1A   = 0xB40; // Compare value  14745600 / 256 = 57600 ( 57600/20 = 2880 = 0xB40)
    TCCR1A |= (1 << CS12); // Start timer at Fcpu/256
   
    TIMSK1 |= (1 << OCIE1A); // Enable CTC interrupt
     
    sei();
}

//Return time
clock_time_t clock_time(void)
{
	return clock_datetime;
}

