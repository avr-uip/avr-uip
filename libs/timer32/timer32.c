/**
 * \addtogroup timer32
 * @{
 */

/**
 * \file
 * Timer library implementation.
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
 * $Id: timer32.c,v 1.2 2006/06/12 08:00:30 adam Exp $
 */

#include "clock.h"
#include "timer32.h"

/*---------------------------------------------------------------------------*/
/**
 * Set a timer32.
 *
 * This function is used to set a timer32 for a time sometime in the
 * future. The function timer32_expired() will evaluate to true after
 * the timer32 has expired.
 *
 * \param t A pointer to the timer32
 * \param interval The interval before the timer32 expires.
 *
 */
void
timer32_set(struct timer32 *t, clock_time32_t interval)
{
  t->interval = interval;
  t->start = clock_time32();
}
/*---------------------------------------------------------------------------*/
/**
 * Reset the timer32 with the same interval.
 *
 * This function resets the timer32 with the same interval that was
 * given to the timer32_set() function. The start point of the interval
 * is the exact time that the timer32 last expired. Therefore, this
 * function will cause the timer32 to be stable over time, unlike the
 * timer32_rester() function.
 *
 * \param t A pointer to the timer32.
 *
 * \sa timer32_restart()
 */
void
timer32_reset(struct timer32 *t)
{
  t->start += t->interval;
}
/*---------------------------------------------------------------------------*/
/**
 * Restart the timer32 from the current point in time
 *
 * This function restarts a timer32 with the same interval that was
 * given to the timer32_set() function. The timer32 will start at the
 * current time.
 *
 * \note A periodic timer32 will drift if this function is used to reset
 * it. For preioric timer32s, use the timer32_reset() function instead.
 *
 * \param t A pointer to the timer32.
 *
 * \sa timer32_reset()
 */
void
timer32_restart(struct timer32 *t)
{
  t->start = clock_time32();
}
/*---------------------------------------------------------------------------*/
/**
 * Check if a timer32 has expired.
 *
 * This function tests if a timer32 has expired and returns true or
 * false depending on its status.
 *
 * \param t A pointer to the timer32
 *
 * \return Non-zero if the timer32 has expired, zero otherwise.
 *
 */
bool
timer32_expired(struct timer32 *t)
{
  return (clock_time32_t)(clock_time32() - t->start) >= (clock_time32_t)t->interval;
}
/*---------------------------------------------------------------------------*/

/** @} */
