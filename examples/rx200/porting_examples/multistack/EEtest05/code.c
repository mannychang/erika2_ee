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
 * A minimal EE demo that demonstrates how to work with the MULTISTACK mode and
 * semaphores.
 * In this demo two tasks are activated in the main function and work with
 * different stacks.
 * The demo is a classical producer/consumer problem implemented with two
 * semaphores P and V.
 * The demo is used to show the following features of ERIKA Enterprise OS: 
 * - 	Multistack configuration: The Producer and the Consumer task need a
 * 	separate stack because they block calling the WaitSem primitive. 
 *
 * - 	Separate stack for IRQs: the OIL file allocates a separate stack for
 * 	ISR Type 2 by setting: "IRQ_STACK = TRUE { SYS_SIZE=64; };".
 *
 * - 	Semaphores: ERIKA Enterprise supports counting semaphores.
 * 	The demo show how to initialize the semaphores, and how to use the main
 *	semaphore primitives (PostSem and WaitSem).
 *
 * Author: 2012  Gianluca Franchino
 * Based on examples/cortex_mx/lm4f232xxxx/porting_examples_keil/EETest05
 */

#include "ee.h"
#include "kernel/sem/inc/ee_sem.h"
#include "test/assert/inc/ee_assert.h"


#define TRUE 1

/* Assertions */
enum EE_ASSERTIONS {
  EE_ASSERT_FIN = 0,
  EE_ASSERT_INIT,
  EE_ASSERT_TASKP_WAIT,
  EE_ASSERT_TASKC_WAIT,
  EE_ASSERT_TASKC_POST,
  EE_ASSERT_TASKP_POST,
  EE_ASSERT_PRODUCED,
  EE_ASSERT_CONSUMED,
  EE_ASSERT_DIM
};
EE_TYPEASSERTVALUE EE_assertions[EE_ASSERT_DIM];

/* Final result */
volatile EE_TYPEASSERTVALUE result;

/* This semaphore is initialized automatically */
SemType P = STATICSEM(1);

/* This semaphore is initialized inside the Background Task */
SemType V;

volatile int taskp_counter = 0;
volatile int taskc_counter = 0;
volatile int counter = 0;

#define	PRODUCTION_CYCLES	1000000
/*
 * TASK PRODUCER
 */
TASK(Producer)
{
  int i;
  static int pcounter = 0;

  taskp_counter++;

  /* 
   * Note: Please note that this task structure is different from the typical
   *       one-shot structure presented in other examples.
   *       This structure with a forever loop inside the task body is much more
   *       similar to the one typically used in the POSIX standard, and it
   *       requires that every task has a PRIVATE STACK, especially if the
   *       task calls blocking functions like WaitSem.
   */
  for (;;) {
    pcounter++;
    if(pcounter==2)
      EE_assert(EE_ASSERT_TASKP_WAIT, pcounter == 2, EE_ASSERT_INIT);
    WaitSem(&P);

    /* take some time to produce an item */
    for (i=0; i < PRODUCTION_CYCLES; i++);

    /* the item has been produced! */
    EE_rskrx210_led0_on();
    if(pcounter==2)
      EE_assert(EE_ASSERT_TASKP_POST, pcounter==2, EE_ASSERT_TASKC_POST);
    PostSem(&V);
    if(pcounter==2)
      EE_assert(EE_ASSERT_PRODUCED, pcounter==2, EE_ASSERT_TASKP_POST);
  }
}

#define	CONSUMING_CYCLES	PRODUCTION_CYCLES
/*
 * TASK CONSUMER
 */
TASK(Consumer)
{
  int i;
  static int ccounter = 0;

  taskc_counter++;
  
  for (;;) {
    ccounter++;
    if(ccounter==1)
      EE_assert(EE_ASSERT_TASKC_WAIT, ccounter==1, EE_ASSERT_TASKP_WAIT);
    WaitSem(&V);

    /* take some time to consume an item */
    for (i=0; i < CONSUMING_CYCLES; i++);

    /* the item has been consumed! */
	EE_rskrx210_led0_off();
    if(ccounter==1)
      EE_assert(EE_ASSERT_TASKC_POST, ccounter==1, EE_ASSERT_TASKC_WAIT);
    PostSem(&P); 
    if(ccounter==1)
    {
      EE_assert(EE_ASSERT_CONSUMED, ccounter==1, EE_ASSERT_PRODUCED);
      EE_assert_range(EE_ASSERT_FIN, EE_ASSERT_INIT, EE_ASSERT_CONSUMED);
      result = EE_assert_last();
    }
  }
}

/*
 * MAIN TASK
 */
int main(void)
{
	/*Initilize the operating frequencies (CPU, Peripheral etc.)*/
	/*PCKD = PCKC = PCKB = PCKA = BCKPLL/8 = 10MHz*/
	/*ICK = PLL/2 = 40 MHz*/
	/*FCK = PLL/4 = 20 MHz*/
	EE_rskrx210_op_freq_setup();
	
	EE_rskrx210_leds_init(0xF);

	EE_assert(EE_ASSERT_INIT, TRUE, EE_ASSERT_NIL);

	/* Initialization of the second semaphore of the example;
	* the first semaphore is initialized inside the definition */
	InitSem(V, 0);

	/* Activate the Producer.
	* The consumer preempts the background task, executes and finally blocks
	* waiting for the Consumer to consume the item produced.
	* After the Producer Blocks, the Background task resumes and the
	* ActivateTask returns. */
	ActivateTask(Producer);

	/* Activate the Consumer.
	* The consumer is activated, preempting the background task.
	* After that, the Consumer and the Producer Task will activate each other
	* forever, and the background task will never execute again. */
	ActivateTask(Consumer);


	/* Forever loop: background activities (if any) should go here */
	for (;result == 1;) {
		while (counter % 100000) counter++;
		EE_rskrx210_leds_toggle();
		counter++;
	}

}
