/**
 * \defgroup timer32 Timer library
 *
 * The timer32 library provides functions for setting, resetting and
 * restarting timer32s, and for checking if a timer32 has expired. An
 * application must "manually" check if its timer32s have expired; this
 * is not done automatically.
 *
 * A timer32 is declared as a \c struct \c timer32 and all access to the
 * timer32 is made by a pointer to the declared timer32.
 *
 * \note The timer32 library uses the \ref clock "Clock library" to
 * measure time. Intervals should be specified in the format used by
 * the clock library.
 *
 * @{
 */


/**
 * \file
 * Timer library header file.
 * \author
 * Adam Dunkels <adam@sics.se>
 */

/*
 * Copyright (c) 2004, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id: timer32.h,v 1.3 2006/06/11 21:46:39 adam Exp $
 */
#ifndef __TIMER_H__
#define __TIMER_H__

#include "clock.h"

/**
 * A timer32.
 *
 * This structure is used for declaring a timer32. The timer32 must be set
 * with timer32_set() before it can be used.
 *
 * \hideinitializer
 */
struct timer32 {
  clock_time32_t start;
  clock_time32_t interval;
};

void timer32_set(struct timer32 *t, clock_time_t interval);
void timer32_reset(struct timer32 *t);
void timer32_restart(struct timer32 *t);
int timer32_expired(struct timer32 *t);

#endif /* __TIMER_H__ */

/** @} */
