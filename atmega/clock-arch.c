#include "uip.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>

#include "clock-arch.h"

//TODO: This file needs documentation, further testing, comments and porting to various AVRs without 16bit timer, or where 16bit timer is used else where. Also I don't know if the software prescale at 512 is a good number.

uint32_t clock_datetime = 0;

bool clock_is_initiated = false;

ISR(TIMER1_OVF_vect)
{
	clock_datetime += 128;
}

void clock_init(){
	if(!clock_is_initiated)
	{
		cli();
		TIMSK1 |= (1<<TOIE1);
		TCCR1B |= ((1<<CS12)|(1<<CS10));
		sei();
		clock_is_initiated = true;
	}
}

clock_time_t clock_time(){
	if(!clock_is_initiated)
		clock_init();
	uint32_t time;
	cli();
	time = clock_datetime;
	time += TCNT1/(512);
	sei();
	return time;
}

