/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2012  Evidence Srl
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
 * Author: 2012 Carlo Caione
 */ 


#include "ee.h"
#include "test/assert/inc/ee_assert.h"
#include "stm32f4xx_conf.h"
#include "stm32f4_discovery.h"

#define TRUE 1

/* assertion data */
enum EE_ASSERTIONS {
  EE_ASSERT_FIN = 0,
  EE_ASSERT_INIT,
  EE_ASSERT_DIM
};
EE_TYPEASSERTVALUE EE_assertions[EE_ASSERT_DIM];

/* Final result */
volatile EE_TYPEASSERTVALUE result;

/* SysTick Counter */
volatile unsigned long SysTickCnt;

/**
  * @brief	SysTick handler sub-routine (1ms)
  * @param	None
  * @return	None
  */
void SysTick_Handler (void)
{
  SysTickCnt++;
}

/**
  * @brief	Delay function
  * @param	tick - number milisecond of delay time
  * @return	None
  */
void Delay (unsigned long tick)
{
  unsigned long systickcnt;

  systickcnt = SysTickCnt;
  while ((SysTickCnt - systickcnt) < tick);
}


TASK(Task1)
{
}

/*
 * MAIN TASK
*/
int main(void)
{
  EE_assert(EE_ASSERT_INIT, TRUE, EE_ASSERT_NIL);

  SysTick_Config(SystemCoreClock/1000 - 1); /* Generate interrupt each 1 ms   */

  STM_EVAL_LEDInit(LED3);

  EE_assert_range(EE_ASSERT_FIN, EE_ASSERT_INIT, EE_ASSERT_INIT);
  result = EE_assert_last();

  // Forever loop: background activities (if any) should go here
  for (;;) {

	STM_EVAL_LEDOn(LED3);
	Delay(500);

	STM_EVAL_LEDOff(LED3);
	Delay(500);

  }

}
