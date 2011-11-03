/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2011  Evidence Srl
 *
 * This file is part of ERIKA Enterprise.
 *
 * ERIKA Enterprise is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation,
 * (with a special exception described below).
 *
 * Linking this code statically or dynamically with other modules is
 * making a combined work based on this code.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this code with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this code, you may extend
 * this exception to your version of the code, but you are not
 * obligated to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 *
 * ERIKA Enterprise is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with ERIKA Enterprise; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 * ###*E*### */

/**
    @file   ee_timer.h
    @brief  TIMER library header file.

    Simple TIMER API. With this API you can supply a period (in us or directly in tics),
    start a free running timer and receive a notification through a given callback,
    every period.
    Only EE_TIMER_1 and EE_TIMER_2 are already implemented.

    @author Errico Guidieri
    @date   2011

**/

#ifndef __INCLUDE_MICROCHIP_DSPIC_TIMER_H__
#define __INCLUDE_MICROCHIP_DSPIC_TIMER_H__

#include "ee.h"

/** Utility Macro that convert an amount of ms in number of ticks of a given
    frequency **/
#define MILLISECONDS_TO_TICKS(X_MS, REF_FREQ_HZ)                            \
    ((X_MS) * ((REF_FREQ_HZ) / 1000UL))

/** Utility Macro that convert an amount of us in number of ticks of a given
    frequency **/
#define MICROSECONDS_TO_TICKS(X_MICROSECS, REF_FREQ_HZ)                     \
    (((X_MICROSECS) / 1000UL)?                                              \
        (MILLISECONDS_TO_TICKS(((X_MICROSECS) / 1000UL), (REF_FREQ_HZ))):   \
        (MILLISECONDS_TO_TICKS((X_MICROSECS), (REF_FREQ_HZ)) / 1000UL))

/** Timer Prescale Factors **/
typedef enum _EE_TimerPrescaleFactor {
    EE_TIMER_PS1   = 0,
    EE_TIMER_PS8   = 1,
    EE_TIMER_PS64  = 2,
    EE_TIMER_PS256 = 3
} EE_TimerPrescaleFactor;

/** Timer IDs **/
typedef enum _EE_TimerId {
    EE_TIMER_1,
    EE_TIMER_2,
    EE_TIMER_3,
    EE_TIMER_23,
} EE_TimerId;

/** error value. No errors happened **/
#define EE_TIMER_NO_ERRORS           0
/** error value. Wrong timer ID **/
#define EE_TIMER_ERR_BAD_TIMER_ID   -1
/** error value. Wrong funtion arguments values **/
#define EE_TIMER_ERR_BAD_ARGS       -2
/** error value. Timer is supposed to exist but is not yet implemented **/
#define EE_TIMER_ERR_UNIMPLEMENTED  -10

/**
    @brief Initialization timer procedure, with explicit ticks period value.

    @param id (EE_TimerId): id timer to configure
    @param period (EE_UINT32): period value as number of clock ticks
    @param ps (EE_TimerPrescaleFactor): prescale factor values

    @return EE_TIMER_NO_ERRORS if no errors happend.
        Otherwise appropriate error value.
 */
EE_INT8 EE_timer_hard_init(EE_TimerId id, EE_UINT32 period, EE_TimerPrescaleFactor ps);

/**
    @brief Initialization timer procedure, with period in microseconds.

    @param id (EE_TimerId): id timer to configure
    @param period_us (EE_UINT32): period value as number of microseconds
    @param ps (EE_TimerPrescaleFactor): prescale factor values

    @return EE_TIMER_NO_ERRORS if no errors happend.
        Otherwise appropriate error value.
 */
EE_INT8 EE_timer_soft_init(EE_TimerId id, EE_UINT32 period_us);

/** @brief Set a timer's callback.

    @param id (EE_TimerId): id timer to set the callback of
    @param func (EE_ISR_callback): timer callback callback, called every end of period.

    @return EE_TIMER_NO_ERRORS
*/
EE_INT8 EE_timer_set_callback(EE_TimerId id, EE_ISR_callback func);

/**
    @brief Start a timer.
    @param id (EE_TimerId): id timer to start

    @return EE_TIMER_NO_ERRORS
*/
EE_INT8 EE_timer_start(EE_TimerId id);

/**
    @brief Stop a timer
    @param id (EE_TimerId): id timer to start

    @return EE_TIMER_NO_ERRORS
*/
EE_INT8 EE_timer_stop(EE_TimerId id);

/* Following API make sense only if alarms are defined */
#if defined(__ALARMS__) | ( ( defined(__OO_BCC1__) | defined( __OO_BCC2__) | defined(__OO_ECC1__) | defined(__OO_ECC2__) ) & !defined(__OO_NO_ALARMS__) )
/**
    @brief Alternatively of setting callback you can tie the timer with a Counter

    @param id (EE_TimerId): id timer to set the callback of
    @param counterId (CounterType): ID of the counter to drive

    @return EE_TIMER_NO_ERRORS
**/
EE_INT8 EE_timer_set_counter(EE_TimerId id, CounterType counterId);
#endif /* defined(__ALARMS__) | ( defined(__OO_BCC1__) | defined( __OO_BCC2__) | defined(__OO_ECC1__) | defined(__OO_ECC2__) & !defined(__OO_NO_ALARMS__) ) */

/* TODO EE_INT8 EE_timer_get_val(EE_TimerId id, EE_UINT16 *v); */
/* TODO EE_INT8 EE_timer_get_val32(EE_TimerId id, EE_UINT32 *v); */
#endif /* __INCLUDE_MICROCHIP_DSPIC_TIMER_H__ */

