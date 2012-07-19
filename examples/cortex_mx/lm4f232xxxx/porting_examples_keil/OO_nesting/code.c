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


/*
 * A minimal EE demo that demonstrates how to work with the MULTISTACK mode with
 * nested interrupts.
 * In this demo two tasks are activated inside two ISRs and work with different
 * stacks.
 * The ISRs use another stack.
 * With this demo you can test the ISR nesting mechanism and the behaviour of the
 * ISR stack.
 * Author: 2011  Giuseppe Serano
 * Based on examples/s12xs/porting_examples/multistack/EEtest4
 */


#include "ee.h"
#include "ee_irq.h"
#include "cpu/cortex_mx/inc/ee_svc.h"
#include "test/assert/inc/ee_assert.h"

#define TRUE 1

/* Assertions */
enum EE_ASSERTIONS {
  EE_ASSERT_FIN = 0,
  EE_ASSERT_INIT,
  EE_ASSERT_SVC_ISR_FIRED,
  EE_ASSERT_SYSTICK_ISR_FIRED,
  EE_ASSERT_TASK2_FIRED,
  EE_ASSERT_TASK2_TEN_INSTANCES,
  EE_ASSERT_TASK1_FIRED,
  EE_ASSERT_TASKS_ENDED,
  EE_ASSERT_DIM
};
EE_TYPEASSERTVALUE EE_assertions[EE_ASSERT_DIM];

/* Final result */
volatile EE_TYPEASSERTVALUE result;

volatile int task1_fired = 0;
volatile int task2_fired = 0;
volatile int task1_ended = 0;
volatile int task2_ended = 0;
volatile int svcounter = 0;
volatile int stcounter = 0;
volatile int divisor = 0;
volatile int counter = 0;

/*
 * SVCall ISR2
 */
ISR2(svcall_handler)
{
  svcounter++;
  EE_assert(EE_ASSERT_SVC_ISR_FIRED, svcounter == 1, EE_ASSERT_INIT);
  ActivateTask(Task1);
  while (stcounter < 10);
  EE_systick_stop();
}


/*
 * SysTick ISR2
 */
ISR2(systick_handler)
{
  stcounter++;
  if (stcounter == 1)
    EE_assert(
      EE_ASSERT_SYSTICK_ISR_FIRED, stcounter == 1, EE_ASSERT_SVC_ISR_FIRED
    );
  ActivateTask(Task2);
}

/*
 * TASK 1
 */
TASK(Task1)
{
  task1_fired++;
  EE_assert(
    EE_ASSERT_TASK1_FIRED, task1_fired == 1, EE_ASSERT_TASK2_TEN_INSTANCES
  );
  task1_ended++;
}

/*
 * TASK 2
 */
TASK(Task2)
{
  task2_fired++;
  if (task2_fired == 1)
    EE_assert(
      EE_ASSERT_TASK2_FIRED, task2_fired == 1, EE_ASSERT_SYSTICK_ISR_FIRED
    );
  else
    if (task2_fired >= 10)
      EE_assert(
        EE_ASSERT_TASK2_TEN_INSTANCES, task2_fired == 10, EE_ASSERT_TASK2_FIRED
      );
  task2_ended++;
}

TASK(Task3)
{

  counter++;

  EE_svc_0();

  counter++;
}

TASK(BackGround)
{

  EE_user_led_init();

  EE_systick_set_period(100000);
  EE_systick_enable_int();
  EE_systick_start();

  EE_assert(EE_ASSERT_INIT, TRUE, EE_ASSERT_NIL);

  ActivateTask(Task3);

  counter++;

  EE_assert(
    EE_ASSERT_TASKS_ENDED,
    task1_ended && (task2_ended == 10),
    EE_ASSERT_TASK1_FIRED
  );
  EE_assert_range(EE_ASSERT_FIN, EE_ASSERT_INIT, EE_ASSERT_TASKS_ENDED);
  result = EE_assert_last();

  /* Forever loop: background activities (if any) should go here */
  for (;result == 1;)
  {
    while (counter % 100000) counter++;
    EE_user_led_toggle();
    ActivateTask(Task3);
    counter++;
  }

}


/*
 * MAIN TASK
 */
int main(void)
{

  StartOS(OSDEFAULTAPPMODE);

  return 0;

}