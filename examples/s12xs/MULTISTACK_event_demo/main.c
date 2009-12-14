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
 * Copyright (C) 2002-2008 Paolo Gai
 */

#include "ee.h"
#include "cpu/cosmic_hs12xs/inc/ee_irqstub.h"
#include "myapp.h"

DeclareTask(Task1);
DeclareTask(Task2);
DeclareEvent(TimerEvent);
DeclareEvent(ButtonEvent);

volatile int timer_fired = 0;
volatile int button_fired = 0;
volatile int dummit_counter = 0;
volatile int led_status = 0;
volatile int myErrorCounter;

/* just a dummy delay */ 
void mydelay(unsigned long int del)
{
  int i;
  for (i=0; i<del; i++);
}

/* sets and resets a led configuration passed as parameter, leaving the other
 * bits unchanged...
 */
void led_blink(unsigned char theled)
{
  DisableAllInterrupts();
  led_status |= theled;
  EE_leds(led_status);
  EnableAllInterrupts();

  mydelay((long int)125000);

  DisableAllInterrupts();
  led_status &= ~theled;
  EE_leds(led_status);
  EnableAllInterrupts();
}

TASK(Task1)
{
  EventMaskType mask;

  while (1) {
    WaitEvent(TimerEvent|ButtonEvent);
    GetEvent(Task1, &mask);

    if (mask & TimerEvent) {
      led_blink(0x01);
      ClearEvent(TimerEvent);
    }

    if (mask & ButtonEvent) {
      led_blink(0x02);
      ClearEvent(ButtonEvent);
    }
  }
  TerminateTask();
}

TASK(Task2)
{
  led_blink(0x04);
  TerminateTask();
}

void ErrorHook(StatusType Error)
{
  myErrorCounter++;
  led_blink(0xFF);
}

/* 
 * The StartupHook in this case is used to initialize the Button and timer
 * interrupts
 */
void StartupHook(void)
{
  /* Init devices */
  EE_buttons_init(BUTTON_0,3);
  
  ///* Program Timer 1 to raise interrupts */
  EE_PIT0_init(99, 14, 2);
}


/* MAIN */
int main(void)
{
  /* Serial interface */
  //EE_SCIOpenCommunication(SCI_0);

  /* Init leds */
  EE_leds_init();

  /* just a nice subliminal welcome message :-) */
  mydelay(1000);
  //message();
	
  StartOS(OSDEFAULTAPPMODE);

  /* Background activities, none in this demo! */
  while(1);
  
  return 0;
}

///* Other functions */
//void message(void)
//{
//	char * msg = "I Love OSEK and Erika Enterprise!!!";
//	EE_SCISendChars(SCI_0, msg,ALL);
//	EE_SCISendBuffer(SCI_0,'\n');
//	return;	
//}