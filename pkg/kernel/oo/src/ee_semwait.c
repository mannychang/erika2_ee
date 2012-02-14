/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2008  Evidence Srl
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

/*
 * Author: 2002 Paolo Gai
 * CVS: $Id: ee_semwait.c,v 1.2 2006/12/03 22:07:50 pj Exp $
 */

#include "ee_internal.h"

/* WaitSem:
   - can be called from an extended task only
   - the task state is put to wait until a call to PostSem wakes it up
   - if the task blocks -> reschedulig + internal resource released
   - returns (only extended status)
       E_OS_RESOURCE task occupies a resource
       E_OS_ACCESS   if the task is not an extended task
       E_OS_CALLEVEL called at interrupt level
*/


#if defined(__OO_ECC1__) || defined(__OO_ECC2__)
#ifndef __PRIVATE_WAITSEM__

#ifdef __OO_EXTENDED_STATUS__
StatusType EE_oo_WaitSem(SemRefType Sem)
#else
void EE_oo_WaitSem(SemRefType Sem)
#endif
{
  TaskType current;
  register EE_FREG np_flags;

  EE_ORTI_set_service_in(EE_SERVICETRACE_WAITSEM);

  current = EE_stk_queryfirst();

#ifdef __OO_EXTENDED_STATUS__

  /* check for a call at interrupt level:
   * Note: this must be the FIRST error check!!!
   */
  if (EE_hal_get_IRQ_nesting_level() || (current == EE_NIL)) {
    EE_ORTI_set_lasterror(E_OS_CALLEVEL);

    np_flags = EE_hal_begin_nested_primitive();
    EE_oo_notify_error_WaitSem(Sem, E_OS_CALLEVEL);
    EE_hal_end_nested_primitive(np_flags);

    EE_ORTI_set_service_out(EE_SERVICETRACE_WAITSEM);

    return E_OS_CALLEVEL;
  }

#ifndef __OO_NO_RESOURCES__
  /* check for busy resources */ 
  if (EE_th_resource_last[current] != EE_UREG_MINUS1) {
    EE_ORTI_set_lasterror(E_OS_RESOURCE);

    np_flags = EE_hal_begin_nested_primitive();
    EE_oo_notify_error_WaitSem(Sem, E_OS_RESOURCE);
    EE_hal_end_nested_primitive(np_flags);

    EE_ORTI_set_service_out(EE_SERVICETRACE_WAITSEM);

    return E_OS_RESOURCE;
  }
#endif /* __OO_NO_RESOURCES__ */

  /* check if the task is an extended task */
  if (!EE_th_is_extended[current]) {
    EE_ORTI_set_lasterror(E_OS_ACCESS);

    np_flags = EE_hal_begin_nested_primitive();
    EE_oo_notify_error_WaitSem(Sem, E_OS_ACCESS);
    EE_hal_end_nested_primitive(np_flags);

    EE_ORTI_set_service_out(EE_SERVICETRACE_WAITSEM);

    return E_OS_ACCESS;
  }
#endif /* __OO_EXTENDED_STATUS__ */

  np_flags = EE_hal_begin_nested_primitive();

  /* handle a local semaphore queue */
  /* check if we have to wait */
  if (Sem->count != 0U) {
    Sem->count--;
  }
  else {
    /* extract the task from the stk data structure
     * current was filled at the beginning of the function
     */
    EE_stk_getfirst();

    /* the task must go into the WAITING state */
    EE_th_status[current] = WAITING;

    /* reset the thread priority bit in the system_ceiling */
    EE_sys_ceiling &= ~EE_th_dispatch_prio[current];
    /* the ready priority is not touched, it is not the same as Schedule! */

    EE_ORTI_set_th_priority(current, 0U);

    /* queue the task inside the semaphore queue */
    if (Sem->first != EE_NIL)
      /* the semaphore queue is not empty */
      EE_th_next[Sem->last] = current;
    else
      /* the semaphore queue is empty */
      Sem->first = current;

    Sem->last = current;
    EE_th_next[current] = EE_NIL;

    /* since the task blocks, it has to be woken up by another
       EE_hal_stkchange */
    EE_th_waswaiting[current] = 1U;

    /* then, the task will be woken up by a PostSem using a EE_hal_stkchange... */

    /* Yeld to the next task:
     * check if there is to schedule a ready thread or pop a preempted
     * thread 
     */
    EE_oo_yeld();

  }

  EE_hal_end_nested_primitive(np_flags);
  EE_ORTI_set_service_out(EE_SERVICETRACE_WAITSEM);

#ifdef __OO_EXTENDED_STATUS__
  return E_OK;
#endif
}

#endif /* __PRIVATE_WAITSEM__ */
#endif /* defined(__OO_ECC1__) || defined(__OO_ECC2__) */

