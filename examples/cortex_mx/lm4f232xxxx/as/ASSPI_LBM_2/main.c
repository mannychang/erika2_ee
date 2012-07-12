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
 * Simple project to test the AUTOSAR SPI Handler/Driver:
 * - 1 Sequence
 * - 2 Jobs
 *   - 4 Channels 
 *   - 7 Channels
 * - All 4 SSI Hardware Unit
 *   - Loop-back Mode
 *   - 2 Units Asynchronous
 *   - 2 Units Synchronous
 * - Write/AsyncTransmit/Read (IB)
 *   - Interrupt Mode
 *   - Polling Mode
 * - Setup/AsyncTransmit (EB)
 *   - Interrupt Mode
 *   - Polling Mode
 * - Write/SyncTransmit/Read (IB)
 * - Setup/SyncTransmit (EB)
 *
 * Author: 2012,  Giuseppe Serano
 */

#include "Dio.h"
#include "Dma.h"
#include "Mcu.h"
#include "Port.h"
#include "Spi.h"
#include "test/assert/inc/ee_assert.h"

#include <string.h>

/* Assertions */
enum EE_ASSERTIONS {
  EE_ASSERT_FIN = 0,			/*   0 */
  EE_ASSERT_INIT,			/*   1 */
  EE_ASSERT_VERSION,			/*   2 */
  EE_ASSERT_CLOCK_INIT,			/*   3 */
  EE_ASSERT_PLL_LOCKED,			/*   4 */
  EE_ASSERT_DMA_INIT,			/*   5 */
  EE_ASSERT_SPI_UNINIT,			/*   6 */
  EE_ASSERT_SPI_INIT,			/*   7 */
  EE_ASSERT_SPI_INTERRUPT_MODE,		/*   8 */
  EE_ASSERT_SPI_INT_WRITE_IB_0,		/*   9 */
  EE_ASSERT_SPI_INT_WRITE_IB_1,		/*  10 */
  EE_ASSERT_SPI_INT_WRITE_IB_2,		/*  11 */
  EE_ASSERT_SPI_INT_WRITE_IB_3,		/*  12 */
  EE_ASSERT_SPI_INT_WRITE_IB_4,		/*  13 */
  EE_ASSERT_SPI_INT_WRITE_IB_5,		/*  14 */
  EE_ASSERT_SPI_INT_WRITE_IB_6,		/*  15 */
  EE_ASSERT_SPI_INT_WRITE_IB_7,		/*  16 */
  EE_ASSERT_SPI_INT_WRITE_IB_8,		/*  17 */
  EE_ASSERT_SPI_INT_WRITE_IB_9,		/*  18 */
  EE_ASSERT_SPI_INT_WRITE_IB_10,	/*  19 */
  EE_ASSERT_SPI_INT_ASYNC_TX,		/*  20 */
  EE_ASSERT_SPI_INT_JOB_END,		/*  21 */
  EE_ASSERT_SPI_INT_JOB_OK,		/*  22 */
  EE_ASSERT_SPI_INT_SEQ_END,		/*  23 */
  EE_ASSERT_SPI_INT_SEQ_OK,		/*  24 */
  EE_ASSERT_SPI_INT_READ_IB_0,		/*  25 */
  EE_ASSERT_SPI_INT_READ_IB_1,		/*  26 */
  EE_ASSERT_SPI_INT_READ_IB_2,		/*  27 */
  EE_ASSERT_SPI_INT_READ_IB_3,		/*  28 */
  EE_ASSERT_SPI_INT_READ_IB_4,		/*  29 */
  EE_ASSERT_SPI_INT_READ_IB_5,		/*  30 */
  EE_ASSERT_SPI_INT_READ_IB_6,		/*  31 */
  EE_ASSERT_SPI_INT_READ_IB_7,		/*  32 */
  EE_ASSERT_SPI_INT_READ_IB_8,		/*  33 */
  EE_ASSERT_SPI_INT_READ_IB_9,		/*  34 */
  EE_ASSERT_SPI_INT_READ_IB_10,		/*  35 */
  EE_ASSERT_SPI_INT_READ_VALIDATE_0,	/*  36 */
  EE_ASSERT_SPI_INT_READ_VALIDATE_1,	/*  37 */
  EE_ASSERT_SPI_INT_READ_VALIDATE_2,	/*  38 */
  EE_ASSERT_SPI_INT_READ_VALIDATE_3,	/*  39 */
  EE_ASSERT_SPI_INT_READ_VALIDATE_4,	/*  40 */
  EE_ASSERT_SPI_INT_READ_VALIDATE_5,	/*  41 */
  EE_ASSERT_SPI_INT_READ_VALIDATE_6,	/*  42 */
  EE_ASSERT_SPI_INT_READ_VALIDATE_7,	/*  43 */
  EE_ASSERT_SPI_INT_READ_VALIDATE_8,	/*  44 */
  EE_ASSERT_SPI_INT_READ_VALIDATE_9,	/*  45 */
  EE_ASSERT_SPI_INT_READ_VALIDATE_10,	/*  46 */
  EE_ASSERT_SPI_POLLING_MODE,		/*  47 */
  EE_ASSERT_SPI_POLL_WRITE_IB_0,	/*  48 */
  EE_ASSERT_SPI_POLL_WRITE_IB_1,	/*  49 */
  EE_ASSERT_SPI_POLL_WRITE_IB_2,	/*  50 */
  EE_ASSERT_SPI_POLL_WRITE_IB_3,	/*  51 */
  EE_ASSERT_SPI_POLL_WRITE_IB_4,	/*  52 */
  EE_ASSERT_SPI_POLL_WRITE_IB_5,	/*  53 */
  EE_ASSERT_SPI_POLL_WRITE_IB_6,	/*  54 */
  EE_ASSERT_SPI_POLL_WRITE_IB_7,	/*  55 */
  EE_ASSERT_SPI_POLL_WRITE_IB_8,	/*  56 */
  EE_ASSERT_SPI_POLL_WRITE_IB_9,	/*  57 */
  EE_ASSERT_SPI_POLL_WRITE_IB_10,	/*  58 */
  EE_ASSERT_SPI_POLL_ASYNC_TX,		/*  59 */
  EE_ASSERT_SPI_POLL_JOB_END,		/*  60 */
  EE_ASSERT_SPI_POLL_JOB_OK,		/*  61 */
  EE_ASSERT_SPI_POLL_SEQ_END,		/*  62 */
  EE_ASSERT_SPI_POLL_SEQ_OK,		/*  63 */
  EE_ASSERT_SPI_POLL_READ_IB_0,		/*  64 */
  EE_ASSERT_SPI_POLL_READ_IB_1,		/*  65 */
  EE_ASSERT_SPI_POLL_READ_IB_2,		/*  66 */
  EE_ASSERT_SPI_POLL_READ_IB_3,		/*  67 */
  EE_ASSERT_SPI_POLL_READ_IB_4,		/*  68 */
  EE_ASSERT_SPI_POLL_READ_IB_5,		/*  69 */
  EE_ASSERT_SPI_POLL_READ_IB_6,		/*  70 */
  EE_ASSERT_SPI_POLL_READ_IB_7,		/*  71 */
  EE_ASSERT_SPI_POLL_READ_IB_8,		/*  72 */
  EE_ASSERT_SPI_POLL_READ_IB_9,		/*  73 */
  EE_ASSERT_SPI_POLL_READ_IB_10,	/*  74 */
  EE_ASSERT_SPI_POLL_READ_VALIDATE_0,	/*  75 */
  EE_ASSERT_SPI_POLL_READ_VALIDATE_1,	/*  76 */
  EE_ASSERT_SPI_POLL_READ_VALIDATE_2,	/*  77 */
  EE_ASSERT_SPI_POLL_READ_VALIDATE_3,	/*  78 */
  EE_ASSERT_SPI_POLL_READ_VALIDATE_4,	/*  79 */
  EE_ASSERT_SPI_POLL_READ_VALIDATE_5,	/*  80 */
  EE_ASSERT_SPI_POLL_READ_VALIDATE_6,	/*  81 */
  EE_ASSERT_SPI_POLL_READ_VALIDATE_7,	/*  82 */
  EE_ASSERT_SPI_POLL_READ_VALIDATE_8,	/*  83 */
  EE_ASSERT_SPI_POLL_READ_VALIDATE_9,	/*  84 */
  EE_ASSERT_SPI_POLL_READ_VALIDATE_10,	/*  85 */
  EE_ASSERT_SPI_EB_INT_MODE,		/*  86 */
  EE_ASSERT_SPI_EB_INT_SETUP_0,		/*  87 */
  EE_ASSERT_SPI_EB_INT_SETUP_1,		/*  88 */
  EE_ASSERT_SPI_EB_INT_SETUP_2,		/*  89 */
  EE_ASSERT_SPI_EB_INT_SETUP_3,		/*  90 */
  EE_ASSERT_SPI_EB_INT_SETUP_4,		/*  91 */
  EE_ASSERT_SPI_EB_INT_SETUP_5,		/*  92 */
  EE_ASSERT_SPI_EB_INT_SETUP_6,		/*  93 */
  EE_ASSERT_SPI_EB_INT_SETUP_7,		/*  94 */
  EE_ASSERT_SPI_EB_INT_SETUP_8,		/*  95 */
  EE_ASSERT_SPI_EB_INT_SETUP_9,		/*  96 */
  EE_ASSERT_SPI_EB_INT_SETUP_10,	/*  97 */
  EE_ASSERT_SPI_EB_INT_ASYNC_TX,	/*  98 */
  EE_ASSERT_SPI_EB_INT_JOB_END,		/*  99 */
  EE_ASSERT_SPI_EB_INT_JOB_OK,		/* 100 */
  EE_ASSERT_SPI_EB_INT_SEQ_END,		/* 101 */
  EE_ASSERT_SPI_EB_INT_SEQ_OK,		/* 102 */
  EE_ASSERT_SPI_EB_INT_VALIDATE_0,	/* 103 */
  EE_ASSERT_SPI_EB_INT_VALIDATE_1,	/* 104 */
  EE_ASSERT_SPI_EB_INT_VALIDATE_2,	/* 105 */
  EE_ASSERT_SPI_EB_INT_VALIDATE_3,	/* 106 */
  EE_ASSERT_SPI_EB_INT_VALIDATE_4,	/* 107 */
  EE_ASSERT_SPI_EB_INT_VALIDATE_5,	/* 108 */
  EE_ASSERT_SPI_EB_INT_VALIDATE_6,	/* 109 */
  EE_ASSERT_SPI_EB_INT_VALIDATE_7,	/* 110 */
  EE_ASSERT_SPI_EB_INT_VALIDATE_8,	/* 111 */
  EE_ASSERT_SPI_EB_INT_VALIDATE_9,	/* 112 */
  EE_ASSERT_SPI_EB_INT_VALIDATE_10,	/* 113 */
  EE_ASSERT_SPI_EB_POLL_MODE,		/* 114 */
  EE_ASSERT_SPI_EB_POLL_SETUP_0,	/* 115 */
  EE_ASSERT_SPI_EB_POLL_SETUP_1,	/* 116 */
  EE_ASSERT_SPI_EB_POLL_SETUP_2,	/* 117 */
  EE_ASSERT_SPI_EB_POLL_SETUP_3,	/* 118 */
  EE_ASSERT_SPI_EB_POLL_SETUP_4,	/* 119 */
  EE_ASSERT_SPI_EB_POLL_SETUP_5,	/* 120 */
  EE_ASSERT_SPI_EB_POLL_SETUP_6,	/* 121 */
  EE_ASSERT_SPI_EB_POLL_SETUP_7,	/* 122 */
  EE_ASSERT_SPI_EB_POLL_SETUP_8,	/* 123 */
  EE_ASSERT_SPI_EB_POLL_SETUP_9,	/* 124 */
  EE_ASSERT_SPI_EB_POLL_SETUP_10,	/* 125 */
  EE_ASSERT_SPI_EB_POLL_ASYNC_TX,	/* 126 */
  EE_ASSERT_SPI_EB_POLL_JOB_END,	/* 127 */
  EE_ASSERT_SPI_EB_POLL_JOB_OK,		/* 128 */
  EE_ASSERT_SPI_EB_POLL_SEQ_END,	/* 129 */
  EE_ASSERT_SPI_EB_POLL_SEQ_OK,		/* 130 */
  EE_ASSERT_SPI_EB_POLL_VALIDATE_0,	/* 131 */
  EE_ASSERT_SPI_EB_POLL_VALIDATE_1,	/* 132 */
  EE_ASSERT_SPI_EB_POLL_VALIDATE_2,	/* 133 */
  EE_ASSERT_SPI_EB_POLL_VALIDATE_3,	/* 134 */
  EE_ASSERT_SPI_EB_POLL_VALIDATE_4,	/* 135 */
  EE_ASSERT_SPI_EB_POLL_VALIDATE_5,	/* 136 */
  EE_ASSERT_SPI_EB_POLL_VALIDATE_6,	/* 137 */
  EE_ASSERT_SPI_EB_POLL_VALIDATE_7,	/* 138 */
  EE_ASSERT_SPI_EB_POLL_VALIDATE_8,	/* 139 */
  EE_ASSERT_SPI_EB_POLL_VALIDATE_9,	/* 140 */
  EE_ASSERT_SPI_EB_POLL_VALIDATE_10,	/* 141 */
  EE_ASSERT_SPI_SYNC_WRITE_IB_0,	/* 142 */
  EE_ASSERT_SPI_SYNC_WRITE_IB_1,	/* 143 */
  EE_ASSERT_SPI_SYNC_WRITE_IB_2,	/* 144 */
  EE_ASSERT_SPI_SYNC_WRITE_IB_3,	/* 145 */
  EE_ASSERT_SPI_SYNC_WRITE_IB_4,	/* 146 */
  EE_ASSERT_SPI_SYNC_WRITE_IB_5,	/* 147 */
  EE_ASSERT_SPI_SYNC_WRITE_IB_6,	/* 148 */
  EE_ASSERT_SPI_SYNC_WRITE_IB_7,	/* 149 */
  EE_ASSERT_SPI_SYNC_WRITE_IB_8,	/* 150 */
  EE_ASSERT_SPI_SYNC_WRITE_IB_9,	/* 151 */
  EE_ASSERT_SPI_SYNC_WRITE_IB_10,	/* 152 */
  EE_ASSERT_SPI_SYNC_TX,		/* 153 */
  EE_ASSERT_SPI_SYNC_JOB_1_OK,		/* 154 */
  EE_ASSERT_SPI_SYNC_JOB_2_OK,		/* 155 */
  EE_ASSERT_SPI_SYNC_SEQ_OK,		/* 156 */
  EE_ASSERT_SPI_SYNC_READ_IB_0,		/* 157 */
  EE_ASSERT_SPI_SYNC_READ_IB_1,		/* 158 */
  EE_ASSERT_SPI_SYNC_READ_IB_2,		/* 159 */
  EE_ASSERT_SPI_SYNC_READ_IB_3,		/* 160 */
  EE_ASSERT_SPI_SYNC_READ_IB_4,		/* 161 */
  EE_ASSERT_SPI_SYNC_READ_IB_5,		/* 162 */
  EE_ASSERT_SPI_SYNC_READ_IB_6,		/* 163 */
  EE_ASSERT_SPI_SYNC_READ_IB_7,		/* 164 */
  EE_ASSERT_SPI_SYNC_READ_IB_8,		/* 165 */
  EE_ASSERT_SPI_SYNC_READ_IB_9,		/* 166 */
  EE_ASSERT_SPI_SYNC_READ_IB_10,	/* 167 */
  EE_ASSERT_SPI_SYNC_VALIDATE_0,	/* 168 */
  EE_ASSERT_SPI_SYNC_VALIDATE_1,	/* 169 */
  EE_ASSERT_SPI_SYNC_VALIDATE_2,	/* 170 */
  EE_ASSERT_SPI_SYNC_VALIDATE_3,	/* 171 */
  EE_ASSERT_SPI_SYNC_VALIDATE_4,	/* 172 */
  EE_ASSERT_SPI_SYNC_VALIDATE_5,	/* 173 */
  EE_ASSERT_SPI_SYNC_VALIDATE_6,	/* 174 */
  EE_ASSERT_SPI_SYNC_VALIDATE_7,	/* 175 */
  EE_ASSERT_SPI_SYNC_VALIDATE_8,	/* 176 */
  EE_ASSERT_SPI_SYNC_VALIDATE_9,	/* 177 */
  EE_ASSERT_SPI_SYNC_VALIDATE_10,	/* 178 */
  EE_ASSERT_SPI_EB_SYNC_SETUP_0,	/* 179 */
  EE_ASSERT_SPI_EB_SYNC_SETUP_1,	/* 180 */
  EE_ASSERT_SPI_EB_SYNC_SETUP_2,	/* 181 */
  EE_ASSERT_SPI_EB_SYNC_SETUP_3,	/* 182 */
  EE_ASSERT_SPI_EB_SYNC_SETUP_4,	/* 183 */
  EE_ASSERT_SPI_EB_SYNC_SETUP_5,	/* 184 */
  EE_ASSERT_SPI_EB_SYNC_SETUP_6,	/* 185 */
  EE_ASSERT_SPI_EB_SYNC_SETUP_7,	/* 186 */
  EE_ASSERT_SPI_EB_SYNC_SETUP_8,	/* 187 */
  EE_ASSERT_SPI_EB_SYNC_SETUP_9,	/* 188 */
  EE_ASSERT_SPI_EB_SYNC_SETUP_10,	/* 189 */
  EE_ASSERT_SPI_EB_SYNC_TX,		/* 190 */
  EE_ASSERT_SPI_EB_SYNC_JOB_1_OK,	/* 191 */
  EE_ASSERT_SPI_EB_SYNC_JOB_2_OK,	/* 192 */
  EE_ASSERT_SPI_EB_SYNC_SEQ_OK,		/* 193 */
  EE_ASSERT_SPI_EB_SYNC_VALIDATE_0,	/* 194 */
  EE_ASSERT_SPI_EB_SYNC_VALIDATE_1,	/* 195 */
  EE_ASSERT_SPI_EB_SYNC_VALIDATE_2,	/* 196 */
  EE_ASSERT_SPI_EB_SYNC_VALIDATE_3,	/* 197 */
  EE_ASSERT_SPI_EB_SYNC_VALIDATE_4,	/* 198 */
  EE_ASSERT_SPI_EB_SYNC_VALIDATE_5,	/* 199 */
  EE_ASSERT_SPI_EB_SYNC_VALIDATE_6,	/* 200 */
  EE_ASSERT_SPI_EB_SYNC_VALIDATE_7,	/* 201 */
  EE_ASSERT_SPI_EB_SYNC_VALIDATE_8,	/* 202 */
  EE_ASSERT_SPI_EB_SYNC_VALIDATE_9,	/* 203 */
  EE_ASSERT_SPI_EB_SYNC_VALIDATE_10,	/* 204 */
  EE_ASSERT_SPI_DEINIT,			/* 205 */
  EE_ASSERT_DIM
};
EE_TYPEASSERTVALUE EE_assertions[EE_ASSERT_DIM];

/* Final result */
volatile EE_TYPEASSERTVALUE result;

/* counter */
volatile int test = 0;

/* counter */
volatile int counter = 0;

/*
 * TASK Job Error.
 */
TASK(TaskJobError)
{
  counter++;
}


/*
 * TASK Job Ok.
 */
TASK(TaskJobOk)
{

  switch ( test ) {

    case 0:
      EE_assert( EE_ASSERT_SPI_INT_JOB_OK, TRUE, EE_ASSERT_SPI_INT_JOB_END );
      break;

    case 1:
      EE_assert( EE_ASSERT_SPI_POLL_JOB_OK, TRUE, EE_ASSERT_SPI_POLL_JOB_END );
      break;

    case 2:
      EE_assert(
	EE_ASSERT_SPI_EB_INT_JOB_OK, TRUE, EE_ASSERT_SPI_EB_INT_JOB_END
      );
      break;

    case 3:
      EE_assert(
	EE_ASSERT_SPI_EB_POLL_JOB_OK, TRUE, EE_ASSERT_SPI_EB_POLL_JOB_END
      );
      break;

  }

}

/*
 * LBM Job 1 End Notification.
 */
void Spi_LBMJob1EndNotification()
{

  switch ( test ) {

    case 0:
      EE_assert( EE_ASSERT_SPI_INT_JOB_END, TRUE, EE_ASSERT_SPI_INT_WRITE_IB_3 );
      if ( Spi_GetJobResult(SPI_LBM_JOB_1) == SPI_JOB_OK ) {
	ActivateTask(TaskJobOk);
      }
      else {
	ActivateTask(TaskJobError);
      }
      break;

    case 1:
      EE_assert(
	EE_ASSERT_SPI_POLL_JOB_END, TRUE, EE_ASSERT_SPI_POLL_ASYNC_TX
      );
      if ( Spi_GetJobResult(SPI_LBM_JOB_1) == SPI_JOB_OK ) {
	ActivateTask(TaskJobOk);
      }
      else {
	ActivateTask(TaskJobError);
      }
      break;

    case 2:
      EE_assert(
	EE_ASSERT_SPI_EB_INT_JOB_END, TRUE, EE_ASSERT_SPI_EB_INT_SETUP_3
      );
      if ( Spi_GetJobResult(SPI_LBM_JOB_1_EB) == SPI_JOB_OK ) {
	ActivateTask(TaskJobOk);
      }
      else {
	ActivateTask(TaskJobError);
      }
      break;

    case 3:
      EE_assert(
	EE_ASSERT_SPI_EB_POLL_JOB_END, TRUE, EE_ASSERT_SPI_EB_POLL_ASYNC_TX
      );
      if ( Spi_GetJobResult(SPI_LBM_JOB_1_EB) == SPI_JOB_OK ) {
	ActivateTask(TaskJobOk);
      }
      else {
	ActivateTask(TaskJobError);
      }
      break;
  }

}

/*
 * TASK Sequence Error
 */
TASK(TaskSeqError)
{

  counter++;

}

/*
 * TASK Sequence Ok
 */
TASK(TaskSeqOk)
{

  switch ( test ) {

    case 0:
      EE_assert( EE_ASSERT_SPI_INT_SEQ_OK, TRUE, EE_ASSERT_SPI_INT_SEQ_END );
      break;

    case 1:
      EE_assert( EE_ASSERT_SPI_POLL_SEQ_OK, TRUE, EE_ASSERT_SPI_POLL_SEQ_END );
      break;

    case 2:
      EE_assert(
	EE_ASSERT_SPI_EB_INT_SEQ_OK, TRUE, EE_ASSERT_SPI_EB_INT_SEQ_END
      );
      break;

    case 3:
      EE_assert(
	EE_ASSERT_SPI_EB_POLL_SEQ_OK, TRUE, EE_ASSERT_SPI_EB_POLL_SEQ_END
      );
      break;

  }

}

/*
 * LBM Sequence 0 End Notification.
 */
void Spi_LBMSeq0EndNotification()
{

  switch ( test ) {

    case 0:
      EE_assert(
	EE_ASSERT_SPI_INT_SEQ_END,
	( Spi_GetJobResult(SPI_LBM_JOB_2) == SPI_JOB_OK ),
	EE_ASSERT_SPI_INT_JOB_END
      );
      if ( Spi_GetSeqResult(SPI_LBM_SEQ_0) == SPI_SEQ_OK ) {
	ActivateTask(TaskSeqOk);
      }
      else {
	ActivateTask(TaskSeqError);
      }
      break;

    case 1:
      EE_assert(
	EE_ASSERT_SPI_POLL_SEQ_END,
	( Spi_GetJobResult(SPI_LBM_JOB_2) == SPI_JOB_OK ),
	EE_ASSERT_SPI_POLL_JOB_END
      );
      if ( Spi_GetSeqResult(SPI_LBM_SEQ_0) == SPI_SEQ_OK ) {
	ActivateTask(TaskSeqOk);
      }
      else {
	ActivateTask(TaskSeqError);
      }
      break;

    case 2:
      EE_assert(
	EE_ASSERT_SPI_EB_INT_SEQ_END,
	( Spi_GetJobResult(SPI_LBM_JOB_2_EB) == SPI_JOB_OK ),
	EE_ASSERT_SPI_EB_INT_JOB_END
      );
      if ( Spi_GetSeqResult(SPI_LBM_SEQ_0_EB) == SPI_SEQ_OK ) {
	ActivateTask(TaskSeqOk);
      }
      else {
	ActivateTask(TaskSeqError);
      }
      break;

    case 3:
      EE_assert(
	EE_ASSERT_SPI_EB_POLL_SEQ_END,
	( Spi_GetJobResult(SPI_LBM_JOB_2) == SPI_JOB_OK ),
	EE_ASSERT_SPI_EB_POLL_JOB_END
      );
      if ( Spi_GetSeqResult(SPI_LBM_SEQ_0_EB) == SPI_SEQ_OK ) {
	ActivateTask(TaskSeqOk);
      }
      else {
	ActivateTask(TaskSeqError);
      }
      break;

  }

}

#define	MCU_PLL_LOCK_MAX_ATTEMPTS	0x0000000A

#define	SPI_TX_RX_DATA_NULL	0x00U

#define	SPI_CHANNEL_NUMBER	0x0BU

Spi_DataType	rx[SPI_CHANNEL_NUMBER][SPI_IB_MAX_N_BUFFERS],
		tx[SPI_CHANNEL_NUMBER][SPI_IB_MAX_N_BUFFERS];

void setup_buff(
  void
)
{

  Spi_NumberOfDataType	i, j;

  for (i = 0; i < SPI_CHANNEL_NUMBER; i++) {

    for (j = 0; j < SPI_IB_MAX_N_BUFFERS; j++) {

      rx[i][j] = SPI_TX_RX_DATA_NULL;
      tx[i][j] = counter + test + i + j;

    }

  }

}

boolean cmp_buff(
  Spi_NumberOfDataType	idx
)
{

  Spi_NumberOfDataType	i;
  boolean		rv = TRUE;

  for (i = 0; i < ( idx + 1 ); i++) {

    if ( rx[idx][i] != tx[idx][i] ) {

      rv = FALSE;

    }

  }

  return rv;

}

/*
 * MAIN TASK
 */
int main(void)
{

  Std_VersionInfoType	version;

  Dio_PortLevelType	lvl;

  counter = 0;

  EE_assert(EE_ASSERT_INIT, TRUE, EE_ASSERT_NIL);

  Spi_GetVersionInfo(&version);

  EE_assert(
    EE_ASSERT_VERSION,
    (
      (version.vendorID == 0) &&
      (version.moduleID == 83) &&
      (version.sw_major_version == 1) &&
      (version.sw_minor_version == 0) &&
      (version.sw_patch_version == 0)
    ),
    EE_ASSERT_INIT
  );

  Mcu_Init(MCU_CONFIG_DEFAULT_PTR);

  if ( Mcu_InitClock(MCU_CLOCK_MODE_PRIOSC_4_PLL) != E_OK ) {

    /* Clock Initialization Error. */
    Mcu_PerformReset();

  }

  EE_assert(EE_ASSERT_CLOCK_INIT, TRUE, EE_ASSERT_VERSION);

  counter = 0;
  while (
    ( Mcu_GetPllStatus() != MCU_PLL_LOCKED ) &&
    ( counter < MCU_PLL_LOCK_MAX_ATTEMPTS )
  ) {
    counter++;
  }

  if ( counter >= MCU_PLL_LOCK_MAX_ATTEMPTS ) {

    /* PLL Lock Error. */
    Mcu_PerformReset();

  }

  EE_assert(
    EE_ASSERT_PLL_LOCKED,
    Mcu_GetPllStatus() == MCU_PLL_LOCKED,
    EE_ASSERT_CLOCK_INIT
  );

  Mcu_DistributePllClock();

  Port_Init(PORT_CONFIG_DEFAULT_PTR);

  Dio_Init(DIO_CONFIG_USER_LED_PTR);

  Dma_Init(DMA_CONFIG_SPI_PTR);

  EE_assert(
    EE_ASSERT_DMA_INIT,
    ( Dma_GetStatus() == SPI_IDLE ),
    EE_ASSERT_PLL_LOCKED
  );


  EE_assert(
    EE_ASSERT_SPI_UNINIT,
    ( Spi_GetStatus() == SPI_UNINIT ),
    EE_ASSERT_DMA_INIT
  );

  Spi_Init(SPI_CONFIG_LBM_PTR);

  EE_assert(
    EE_ASSERT_SPI_INIT, ( Spi_GetStatus() == SPI_IDLE ), EE_ASSERT_SPI_UNINIT
  );

  test = 0;

  EE_assert(
    EE_ASSERT_SPI_INTERRUPT_MODE,
    ( Spi_SetAsyncMode(SPI_INTERRUPT_MODE) == E_OK ),
    EE_ASSERT_SPI_INIT
  );

  setup_buff();

  EE_assert(
    EE_ASSERT_SPI_INT_WRITE_IB_0,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_0, tx[0]) == E_OK ),
    EE_ASSERT_SPI_INTERRUPT_MODE
  );

  EE_assert(
    EE_ASSERT_SPI_INT_WRITE_IB_1,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_1, tx[1]) == E_OK ),
    EE_ASSERT_SPI_INT_WRITE_IB_0
  );

  EE_assert(
    EE_ASSERT_SPI_INT_WRITE_IB_2,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_2, tx[2]) == E_OK ),
    EE_ASSERT_SPI_INT_WRITE_IB_1
  );

  EE_assert(
    EE_ASSERT_SPI_INT_WRITE_IB_3,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_3, tx[3]) == E_OK ),
    EE_ASSERT_SPI_INT_WRITE_IB_2
  );

  EE_assert(
    EE_ASSERT_SPI_INT_WRITE_IB_4,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_4, tx[4]) == E_OK ),
    EE_ASSERT_SPI_INT_WRITE_IB_3
  );

  EE_assert(
    EE_ASSERT_SPI_INT_WRITE_IB_5,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_5, tx[5]) == E_OK ),
    EE_ASSERT_SPI_INT_WRITE_IB_4
  );

  EE_assert(
    EE_ASSERT_SPI_INT_WRITE_IB_6,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_6, tx[6]) == E_OK ),
    EE_ASSERT_SPI_INT_WRITE_IB_5
  );

  EE_assert(
    EE_ASSERT_SPI_INT_WRITE_IB_7,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_7, tx[7]) == E_OK ),
    EE_ASSERT_SPI_INT_WRITE_IB_6
  );

  EE_assert(
    EE_ASSERT_SPI_INT_WRITE_IB_8,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_8, tx[8]) == E_OK ),
    EE_ASSERT_SPI_INT_WRITE_IB_7
  );

  EE_assert(
    EE_ASSERT_SPI_INT_WRITE_IB_9,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_9, tx[9]) == E_OK ),
    EE_ASSERT_SPI_INT_WRITE_IB_8
  );

  EE_assert(
    EE_ASSERT_SPI_INT_WRITE_IB_10,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_10, tx[10]) == E_OK ),
    EE_ASSERT_SPI_INT_WRITE_IB_9
  );

  EE_assert(
    EE_ASSERT_SPI_INT_ASYNC_TX,
    ( Spi_AsyncTransmit(SPI_LBM_SEQ_0) == E_OK ),
    EE_ASSERT_SPI_INT_WRITE_IB_10
  );

  while ( Spi_GetSeqResult(SPI_LBM_SEQ_0) == SPI_SEQ_PENDING ) {
    ;
  }

  if ( Spi_GetSeqResult(SPI_LBM_SEQ_0) == SPI_SEQ_OK ) {

    EE_assert(
      EE_ASSERT_SPI_INT_READ_IB_0,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_0, rx[0]) == E_OK ),
      EE_ASSERT_SPI_INT_SEQ_OK
    );

    EE_assert(
      EE_ASSERT_SPI_INT_READ_IB_1,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_1, rx[1]) == E_OK ),
      EE_ASSERT_SPI_INT_READ_IB_0
    );

    EE_assert(
      EE_ASSERT_SPI_INT_READ_IB_2,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_2, rx[2]) == E_OK ),
      EE_ASSERT_SPI_INT_READ_IB_1
    );

    EE_assert(
      EE_ASSERT_SPI_INT_READ_IB_3,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_3, rx[3]) == E_OK ),
      EE_ASSERT_SPI_INT_READ_IB_2
    );

    EE_assert(
      EE_ASSERT_SPI_INT_READ_IB_4,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_4, rx[4]) == E_OK ),
      EE_ASSERT_SPI_INT_READ_IB_3
    );

    EE_assert(
      EE_ASSERT_SPI_INT_READ_IB_5,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_5, rx[5]) == E_OK ),
      EE_ASSERT_SPI_INT_READ_IB_4
    );

    EE_assert(
      EE_ASSERT_SPI_INT_READ_IB_6,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_6, rx[6]) == E_OK ),
      EE_ASSERT_SPI_INT_READ_IB_5
    );

    EE_assert(
      EE_ASSERT_SPI_INT_READ_IB_7,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_7, rx[7]) == E_OK ),
      EE_ASSERT_SPI_INT_READ_IB_6
    );

    EE_assert(
      EE_ASSERT_SPI_INT_READ_IB_8,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_8, rx[8]) == E_OK ),
      EE_ASSERT_SPI_INT_READ_IB_7
    );

    EE_assert(
      EE_ASSERT_SPI_INT_READ_IB_9,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_9, rx[9]) == E_OK ),
      EE_ASSERT_SPI_INT_READ_IB_8
    );

    EE_assert(
      EE_ASSERT_SPI_INT_READ_IB_10,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_10, rx[10]) == E_OK ),
      EE_ASSERT_SPI_INT_READ_IB_9
    );

    EE_assert(
      EE_ASSERT_SPI_INT_READ_VALIDATE_0,
      cmp_buff(0),
      EE_ASSERT_SPI_INT_READ_IB_10
    );

    EE_assert(
      EE_ASSERT_SPI_INT_READ_VALIDATE_1,
      cmp_buff(1),
      EE_ASSERT_SPI_INT_READ_VALIDATE_0
    );

    EE_assert(
      EE_ASSERT_SPI_INT_READ_VALIDATE_2,
      cmp_buff(2),
      EE_ASSERT_SPI_INT_READ_VALIDATE_1
    );

    EE_assert(
      EE_ASSERT_SPI_INT_READ_VALIDATE_3,
      cmp_buff(3),
      EE_ASSERT_SPI_INT_READ_VALIDATE_2
    );

    EE_assert(
      EE_ASSERT_SPI_INT_READ_VALIDATE_4,
      cmp_buff(4),
      EE_ASSERT_SPI_INT_READ_VALIDATE_3
    );

    EE_assert(
      EE_ASSERT_SPI_INT_READ_VALIDATE_5,
      cmp_buff(5),
      EE_ASSERT_SPI_INT_READ_VALIDATE_4
    );

    EE_assert(
      EE_ASSERT_SPI_INT_READ_VALIDATE_6,
      cmp_buff(6),
      EE_ASSERT_SPI_INT_READ_VALIDATE_5
    );

    EE_assert(
      EE_ASSERT_SPI_INT_READ_VALIDATE_7,
      cmp_buff(7),
      EE_ASSERT_SPI_INT_READ_VALIDATE_6
    );

    EE_assert(
      EE_ASSERT_SPI_INT_READ_VALIDATE_8,
      cmp_buff(8),
      EE_ASSERT_SPI_INT_READ_VALIDATE_7
    );

    EE_assert(
      EE_ASSERT_SPI_INT_READ_VALIDATE_9,
      cmp_buff(9),
      EE_ASSERT_SPI_INT_READ_VALIDATE_8
    );

    EE_assert(
      EE_ASSERT_SPI_INT_READ_VALIDATE_10,
      cmp_buff(10),
      EE_ASSERT_SPI_INT_READ_VALIDATE_9
    );

  }

  test++;

  EE_assert(
    EE_ASSERT_SPI_POLLING_MODE,
    ( Spi_SetAsyncMode(SPI_POLLING_MODE) == E_OK ),
    EE_ASSERT_SPI_INT_READ_VALIDATE_10
  );

  setup_buff();

  EE_assert(
    EE_ASSERT_SPI_POLL_WRITE_IB_0,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_0, tx[0]) == E_OK ),
    EE_ASSERT_SPI_POLLING_MODE
  );

  EE_assert(
    EE_ASSERT_SPI_POLL_WRITE_IB_1,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_1, tx[1]) == E_OK ),
    EE_ASSERT_SPI_POLL_WRITE_IB_0
  );

  EE_assert(
    EE_ASSERT_SPI_POLL_WRITE_IB_2,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_2, tx[2]) == E_OK ),
    EE_ASSERT_SPI_POLL_WRITE_IB_1
  );

  EE_assert(
    EE_ASSERT_SPI_POLL_WRITE_IB_3,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_3, tx[3]) == E_OK ),
    EE_ASSERT_SPI_POLL_WRITE_IB_2
  );

  EE_assert(
    EE_ASSERT_SPI_POLL_WRITE_IB_4,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_4, tx[4]) == E_OK ),
    EE_ASSERT_SPI_POLL_WRITE_IB_3
  );

  EE_assert(
    EE_ASSERT_SPI_POLL_WRITE_IB_5,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_5, tx[5]) == E_OK ),
    EE_ASSERT_SPI_POLL_WRITE_IB_4
  );

  EE_assert(
    EE_ASSERT_SPI_POLL_WRITE_IB_6,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_6, tx[6]) == E_OK ),
    EE_ASSERT_SPI_POLL_WRITE_IB_5
  );

  EE_assert(
    EE_ASSERT_SPI_POLL_WRITE_IB_7,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_7, tx[7]) == E_OK ),
    EE_ASSERT_SPI_POLL_WRITE_IB_6
  );

  EE_assert(
    EE_ASSERT_SPI_POLL_WRITE_IB_8,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_8, tx[8]) == E_OK ),
    EE_ASSERT_SPI_POLL_WRITE_IB_7
  );

  EE_assert(
    EE_ASSERT_SPI_POLL_WRITE_IB_9,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_9, tx[9]) == E_OK ),
    EE_ASSERT_SPI_POLL_WRITE_IB_8
  );

  EE_assert(
    EE_ASSERT_SPI_POLL_WRITE_IB_10,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_10, tx[10]) == E_OK ),
    EE_ASSERT_SPI_POLL_WRITE_IB_9
  );

  EE_assert(
    EE_ASSERT_SPI_POLL_ASYNC_TX,
    ( Spi_AsyncTransmit(SPI_LBM_SEQ_0) == E_OK ),
    EE_ASSERT_SPI_POLL_WRITE_IB_10
  );

  while ( Spi_GetSeqResult(SPI_LBM_SEQ_0) == SPI_SEQ_PENDING ) {

    Spi_MainFunction_Handling();

  }

  if ( Spi_GetSeqResult(SPI_LBM_SEQ_0) == SPI_SEQ_OK ) {

    EE_assert(
      EE_ASSERT_SPI_POLL_READ_IB_0,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_0, rx[0]) == E_OK ),
      EE_ASSERT_SPI_POLL_SEQ_OK
    );

    EE_assert(
      EE_ASSERT_SPI_POLL_READ_IB_1,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_1, rx[1]) == E_OK ),
      EE_ASSERT_SPI_POLL_READ_IB_0
    );

    EE_assert(
      EE_ASSERT_SPI_POLL_READ_IB_2,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_2, rx[2]) == E_OK ),
      EE_ASSERT_SPI_POLL_READ_IB_1
    );

    EE_assert(
      EE_ASSERT_SPI_POLL_READ_IB_3,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_3, rx[3]) == E_OK ),
      EE_ASSERT_SPI_POLL_READ_IB_2
    );

    EE_assert(
      EE_ASSERT_SPI_POLL_READ_IB_4,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_4, rx[4]) == E_OK ),
      EE_ASSERT_SPI_POLL_READ_IB_3
    );

    EE_assert(
      EE_ASSERT_SPI_POLL_READ_IB_5,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_5, rx[5]) == E_OK ),
      EE_ASSERT_SPI_POLL_READ_IB_4
    );

    EE_assert(
      EE_ASSERT_SPI_POLL_READ_IB_6,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_6, rx[6]) == E_OK ),
      EE_ASSERT_SPI_POLL_READ_IB_5
    );

    EE_assert(
      EE_ASSERT_SPI_POLL_READ_IB_7,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_7, rx[7]) == E_OK ),
      EE_ASSERT_SPI_POLL_READ_IB_6
    );

    EE_assert(
      EE_ASSERT_SPI_POLL_READ_IB_8,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_8, rx[8]) == E_OK ),
      EE_ASSERT_SPI_POLL_READ_IB_7
    );

    EE_assert(
      EE_ASSERT_SPI_POLL_READ_IB_9,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_9, rx[9]) == E_OK ),
      EE_ASSERT_SPI_POLL_READ_IB_8
    );

    EE_assert(
      EE_ASSERT_SPI_POLL_READ_IB_10,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_10, rx[10]) == E_OK ),
      EE_ASSERT_SPI_POLL_READ_IB_9
    );

    EE_assert(
      EE_ASSERT_SPI_POLL_READ_VALIDATE_0,
      cmp_buff(0),
      EE_ASSERT_SPI_POLL_READ_IB_10
    );

    EE_assert(
      EE_ASSERT_SPI_POLL_READ_VALIDATE_1,
      cmp_buff(1),
      EE_ASSERT_SPI_POLL_READ_VALIDATE_0
    );

    EE_assert(
      EE_ASSERT_SPI_POLL_READ_VALIDATE_2,
      cmp_buff(2),
      EE_ASSERT_SPI_POLL_READ_VALIDATE_1
    );

    EE_assert(
      EE_ASSERT_SPI_POLL_READ_VALIDATE_3,
      cmp_buff(3),
      EE_ASSERT_SPI_POLL_READ_VALIDATE_2
    );

    EE_assert(
      EE_ASSERT_SPI_POLL_READ_VALIDATE_4,
      cmp_buff(4),
      EE_ASSERT_SPI_POLL_READ_VALIDATE_3
    );

    EE_assert(
      EE_ASSERT_SPI_POLL_READ_VALIDATE_5,
      cmp_buff(5),
      EE_ASSERT_SPI_POLL_READ_VALIDATE_4
    );

    EE_assert(
      EE_ASSERT_SPI_POLL_READ_VALIDATE_6,
      cmp_buff(6),
      EE_ASSERT_SPI_POLL_READ_VALIDATE_5
    );

    EE_assert(
      EE_ASSERT_SPI_POLL_READ_VALIDATE_7,
      cmp_buff(7),
      EE_ASSERT_SPI_POLL_READ_VALIDATE_6
    );

    EE_assert(
      EE_ASSERT_SPI_POLL_READ_VALIDATE_8,
      cmp_buff(8),
      EE_ASSERT_SPI_POLL_READ_VALIDATE_7
    );

    EE_assert(
      EE_ASSERT_SPI_POLL_READ_VALIDATE_9,
      cmp_buff(9),
      EE_ASSERT_SPI_POLL_READ_VALIDATE_8
    );

    EE_assert(
      EE_ASSERT_SPI_POLL_READ_VALIDATE_10,
      cmp_buff(10),
      EE_ASSERT_SPI_POLL_READ_VALIDATE_9
    );

  }

  test++;

  EE_assert(
    EE_ASSERT_SPI_EB_INT_MODE,
    ( Spi_SetAsyncMode(SPI_INTERRUPT_MODE) == E_OK ),
    EE_ASSERT_SPI_POLL_READ_VALIDATE_10
  );

  setup_buff();

  EE_assert(
    EE_ASSERT_SPI_EB_INT_SETUP_0,
    (
      Spi_SetupEB(SPI_LBM_CHANNEL_0_EB, tx[0], rx[0], 1)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_INT_MODE
  );

  EE_assert(
    EE_ASSERT_SPI_EB_INT_SETUP_1,
    (
      Spi_SetupEB(SPI_LBM_CHANNEL_1_EB, tx[1], rx[1], 2)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_INT_SETUP_0
  );

  EE_assert(
    EE_ASSERT_SPI_EB_INT_SETUP_2,
    (
      Spi_SetupEB(SPI_LBM_CHANNEL_2_EB, tx[2], rx[2], 3)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_INT_SETUP_1
  );

  EE_assert(
    EE_ASSERT_SPI_EB_INT_SETUP_3,
    (
      Spi_SetupEB(SPI_LBM_CHANNEL_3_EB, tx[3], rx[3], 4)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_INT_SETUP_2
  );

  EE_assert(
    EE_ASSERT_SPI_EB_INT_SETUP_4,
    (
      Spi_SetupEB(SPI_LBM_CHANNEL_4_EB, tx[4], rx[4], 5)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_INT_SETUP_3
  );

  EE_assert(
    EE_ASSERT_SPI_EB_INT_SETUP_5,
    (
      Spi_SetupEB(SPI_LBM_CHANNEL_5_EB, tx[5], rx[5], 6)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_INT_SETUP_4
  );

  EE_assert(
    EE_ASSERT_SPI_EB_INT_SETUP_6,
    (
      Spi_SetupEB(SPI_LBM_CHANNEL_6_EB, tx[6], rx[6], 7)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_INT_SETUP_5
  );

  EE_assert(
    EE_ASSERT_SPI_EB_INT_SETUP_7,
    (
      Spi_SetupEB(SPI_LBM_CHANNEL_7_EB, tx[7], rx[7], 8)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_INT_SETUP_6
  );

  EE_assert(
    EE_ASSERT_SPI_EB_INT_SETUP_8,
    (
      Spi_SetupEB(SPI_LBM_CHANNEL_8_EB, tx[8], rx[8], 9)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_INT_SETUP_7
  );

  EE_assert(
    EE_ASSERT_SPI_EB_INT_SETUP_9,
    (
      Spi_SetupEB(SPI_LBM_CHANNEL_9_EB, tx[9], rx[9], 10)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_INT_SETUP_8
  );

  EE_assert(
    EE_ASSERT_SPI_EB_INT_SETUP_10,
    (
      Spi_SetupEB(SPI_LBM_CHANNEL_10_EB, tx[10], rx[10], 11)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_INT_SETUP_9
  );

  EE_assert(
    EE_ASSERT_SPI_EB_INT_ASYNC_TX,
    ( Spi_AsyncTransmit(SPI_LBM_SEQ_0_EB) == E_OK ),
    EE_ASSERT_SPI_EB_INT_SETUP_10
  );

  while ( Spi_GetSeqResult(SPI_LBM_SEQ_0_EB) == SPI_SEQ_PENDING ) {
    ;
  }

  if ( Spi_GetSeqResult(SPI_LBM_SEQ_0_EB) == SPI_SEQ_OK ) {

    EE_assert(
      EE_ASSERT_SPI_EB_INT_VALIDATE_0,
      cmp_buff(0),
      EE_ASSERT_SPI_EB_INT_ASYNC_TX
    );

    EE_assert(
      EE_ASSERT_SPI_EB_INT_VALIDATE_1,
      cmp_buff(1),
      EE_ASSERT_SPI_EB_INT_VALIDATE_0
    );

    EE_assert(
      EE_ASSERT_SPI_EB_INT_VALIDATE_2,
      cmp_buff(2),
      EE_ASSERT_SPI_EB_INT_VALIDATE_1
    );

    EE_assert(
      EE_ASSERT_SPI_EB_INT_VALIDATE_3,
      cmp_buff(3),
      EE_ASSERT_SPI_EB_INT_VALIDATE_2
    );

    EE_assert(
      EE_ASSERT_SPI_EB_INT_VALIDATE_4,
      cmp_buff(4),
      EE_ASSERT_SPI_EB_INT_VALIDATE_3
    );

    EE_assert(
      EE_ASSERT_SPI_EB_INT_VALIDATE_5,
      cmp_buff(5),
      EE_ASSERT_SPI_EB_INT_VALIDATE_4
    );

    EE_assert(
      EE_ASSERT_SPI_EB_INT_VALIDATE_6,
      cmp_buff(6),
      EE_ASSERT_SPI_EB_INT_VALIDATE_5
    );

    EE_assert(
      EE_ASSERT_SPI_EB_INT_VALIDATE_7,
      cmp_buff(7),
      EE_ASSERT_SPI_EB_INT_VALIDATE_6
    );

    EE_assert(
      EE_ASSERT_SPI_EB_INT_VALIDATE_8,
      cmp_buff(8),
      EE_ASSERT_SPI_EB_INT_VALIDATE_7
    );

    EE_assert(
      EE_ASSERT_SPI_EB_INT_VALIDATE_9,
      cmp_buff(9),
      EE_ASSERT_SPI_EB_INT_VALIDATE_8
    );

    EE_assert(
      EE_ASSERT_SPI_EB_INT_VALIDATE_10,
      cmp_buff(10),
      EE_ASSERT_SPI_EB_INT_VALIDATE_9
    );

  }

  test++;

  EE_assert(
    EE_ASSERT_SPI_EB_POLL_MODE,
    ( Spi_SetAsyncMode(SPI_POLLING_MODE) == E_OK ),
    EE_ASSERT_SPI_EB_INT_VALIDATE_10
  );

  setup_buff();

  EE_assert(
    EE_ASSERT_SPI_EB_POLL_SETUP_0,
    (
      Spi_SetupEB(SPI_LBM_CHANNEL_0_EB, tx[0], rx[0], 1)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_POLL_MODE
  );

  EE_assert(
    EE_ASSERT_SPI_EB_POLL_SETUP_1,
    (
      Spi_SetupEB(SPI_LBM_CHANNEL_1_EB, tx[1], rx[1], 2)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_POLL_SETUP_0
  );

  EE_assert(
    EE_ASSERT_SPI_EB_POLL_SETUP_2,
    (
      Spi_SetupEB(SPI_LBM_CHANNEL_2_EB, tx[2], rx[2], 3)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_POLL_SETUP_1
  );

  EE_assert(
    EE_ASSERT_SPI_EB_POLL_SETUP_3,
    (
      Spi_SetupEB(SPI_LBM_CHANNEL_3_EB, tx[3], rx[3], 4)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_POLL_SETUP_2
  );

  EE_assert(
    EE_ASSERT_SPI_EB_POLL_SETUP_4,
    (
      Spi_SetupEB(SPI_LBM_CHANNEL_4_EB, tx[4], rx[4], 5)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_POLL_SETUP_3
  );

  EE_assert(
    EE_ASSERT_SPI_EB_POLL_SETUP_5,
    (
      Spi_SetupEB(SPI_LBM_CHANNEL_5_EB, tx[5], rx[5], 6)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_POLL_SETUP_4
  );

  EE_assert(
    EE_ASSERT_SPI_EB_POLL_SETUP_6,
    (
      Spi_SetupEB(SPI_LBM_CHANNEL_6_EB, tx[6], rx[6], 7)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_POLL_SETUP_5
  );

  EE_assert(
    EE_ASSERT_SPI_EB_POLL_SETUP_7,
    (
      Spi_SetupEB(SPI_LBM_CHANNEL_7_EB, tx[7], rx[7], 8)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_POLL_SETUP_6
  );

  EE_assert(
    EE_ASSERT_SPI_EB_POLL_SETUP_8,
    (
      Spi_SetupEB(SPI_LBM_CHANNEL_8_EB, tx[8], rx[8], 9)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_POLL_SETUP_7
  );

  EE_assert(
    EE_ASSERT_SPI_EB_POLL_SETUP_9,
    (
      Spi_SetupEB(SPI_LBM_CHANNEL_9_EB, tx[9], rx[9], 10)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_POLL_SETUP_8
  );

  EE_assert(
    EE_ASSERT_SPI_EB_POLL_SETUP_10,
    (
      Spi_SetupEB(SPI_LBM_CHANNEL_10_EB, tx[10], rx[10], 11)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_POLL_SETUP_9
  );

  EE_assert(
    EE_ASSERT_SPI_EB_POLL_ASYNC_TX,
    ( Spi_AsyncTransmit(SPI_LBM_SEQ_0_EB) == E_OK ),
    EE_ASSERT_SPI_EB_POLL_SETUP_10
  );

  while ( Spi_GetSeqResult(SPI_LBM_SEQ_0_EB) == SPI_SEQ_PENDING ) {

    Spi_MainFunction_Handling();

  }

  if ( Spi_GetSeqResult(SPI_LBM_SEQ_0_EB) == SPI_SEQ_OK ) {

    EE_assert(
      EE_ASSERT_SPI_EB_POLL_VALIDATE_0,
      cmp_buff(0),
      EE_ASSERT_SPI_EB_POLL_ASYNC_TX
    );

    EE_assert(
      EE_ASSERT_SPI_EB_POLL_VALIDATE_1,
      cmp_buff(1),
      EE_ASSERT_SPI_EB_POLL_VALIDATE_0
    );

    EE_assert(
      EE_ASSERT_SPI_EB_POLL_VALIDATE_2,
      cmp_buff(2),
      EE_ASSERT_SPI_EB_POLL_VALIDATE_1
    );

    EE_assert(
      EE_ASSERT_SPI_EB_POLL_VALIDATE_3,
      cmp_buff(3),
      EE_ASSERT_SPI_EB_POLL_VALIDATE_2
    );

    EE_assert(
      EE_ASSERT_SPI_EB_POLL_VALIDATE_4,
      cmp_buff(4),
      EE_ASSERT_SPI_EB_POLL_VALIDATE_3
    );

    EE_assert(
      EE_ASSERT_SPI_EB_POLL_VALIDATE_5,
      cmp_buff(5),
      EE_ASSERT_SPI_EB_POLL_VALIDATE_4
    );

    EE_assert(
      EE_ASSERT_SPI_EB_POLL_VALIDATE_6,
      cmp_buff(6),
      EE_ASSERT_SPI_EB_POLL_VALIDATE_5
    );

    EE_assert(
      EE_ASSERT_SPI_EB_POLL_VALIDATE_7,
      cmp_buff(7),
      EE_ASSERT_SPI_EB_POLL_VALIDATE_6
    );

    EE_assert(
      EE_ASSERT_SPI_EB_POLL_VALIDATE_8,
      cmp_buff(8),
      EE_ASSERT_SPI_EB_POLL_VALIDATE_7
    );

    EE_assert(
      EE_ASSERT_SPI_EB_POLL_VALIDATE_9,
      cmp_buff(9),
      EE_ASSERT_SPI_EB_POLL_VALIDATE_8
    );

    EE_assert(
      EE_ASSERT_SPI_EB_POLL_VALIDATE_10,
      cmp_buff(10),
      EE_ASSERT_SPI_EB_POLL_VALIDATE_9
    );

  }

  test++;

  setup_buff();

  EE_assert(
    EE_ASSERT_SPI_SYNC_WRITE_IB_0,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_0, tx[0]) == E_OK ),
    EE_ASSERT_SPI_EB_POLL_VALIDATE_10
  );

  EE_assert(
    EE_ASSERT_SPI_SYNC_WRITE_IB_1,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_1, tx[1]) == E_OK ),
    EE_ASSERT_SPI_SYNC_WRITE_IB_0
  );

  EE_assert(
    EE_ASSERT_SPI_SYNC_WRITE_IB_2,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_2, tx[2]) == E_OK ),
    EE_ASSERT_SPI_SYNC_WRITE_IB_1
  );

  EE_assert(
    EE_ASSERT_SPI_SYNC_WRITE_IB_3,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_3, tx[3]) == E_OK ),
    EE_ASSERT_SPI_SYNC_WRITE_IB_2
  );

  EE_assert(
    EE_ASSERT_SPI_SYNC_WRITE_IB_4,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_4, tx[4]) == E_OK ),
    EE_ASSERT_SPI_SYNC_WRITE_IB_3
  );

  EE_assert(
    EE_ASSERT_SPI_SYNC_WRITE_IB_5,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_5, tx[5]) == E_OK ),
    EE_ASSERT_SPI_SYNC_WRITE_IB_4
  );

  EE_assert(
    EE_ASSERT_SPI_SYNC_WRITE_IB_6,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_6, tx[6]) == E_OK ),
    EE_ASSERT_SPI_SYNC_WRITE_IB_5
  );

  EE_assert(
    EE_ASSERT_SPI_SYNC_WRITE_IB_7,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_7, tx[7]) == E_OK ),
    EE_ASSERT_SPI_SYNC_WRITE_IB_6
  );

  EE_assert(
    EE_ASSERT_SPI_SYNC_WRITE_IB_8,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_8, tx[8]) == E_OK ),
    EE_ASSERT_SPI_SYNC_WRITE_IB_7
  );

  EE_assert(
    EE_ASSERT_SPI_SYNC_WRITE_IB_9,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_9, tx[9]) == E_OK ),
    EE_ASSERT_SPI_SYNC_WRITE_IB_8
  );

  EE_assert(
    EE_ASSERT_SPI_SYNC_WRITE_IB_10,
    ( Spi_WriteIB(SPI_LBM_CHANNEL_10, tx[10]) == E_OK ),
    EE_ASSERT_SPI_SYNC_WRITE_IB_9
  );

  EE_assert(
    EE_ASSERT_SPI_SYNC_TX,
    ( Spi_SyncTransmit(SPI_LBM_SEQ_0_SYNC) == E_OK ),
    EE_ASSERT_SPI_SYNC_WRITE_IB_10
  );

  EE_assert(
    EE_ASSERT_SPI_SYNC_JOB_1_OK,
    ( Spi_GetJobResult(SPI_LBM_JOB_1_SYNC) == SPI_JOB_OK ),
    EE_ASSERT_SPI_SYNC_TX
  );

  EE_assert(
    EE_ASSERT_SPI_SYNC_JOB_2_OK,
    ( Spi_GetJobResult(SPI_LBM_JOB_2_SYNC) == SPI_JOB_OK ),
    EE_ASSERT_SPI_SYNC_JOB_1_OK
  );

  EE_assert(
    EE_ASSERT_SPI_SYNC_SEQ_OK,
    ( Spi_GetSeqResult(SPI_LBM_SEQ_0_SYNC) == SPI_SEQ_OK ),
    EE_ASSERT_SPI_SYNC_JOB_2_OK
  );

  if ( Spi_GetSeqResult(SPI_LBM_SEQ_0_SYNC) == SPI_SEQ_OK ) {

    EE_assert(
      EE_ASSERT_SPI_SYNC_READ_IB_0,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_0, rx[0]) == E_OK ),
      EE_ASSERT_SPI_SYNC_SEQ_OK
    );

    EE_assert(
      EE_ASSERT_SPI_SYNC_READ_IB_1,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_1, rx[1]) == E_OK ),
      EE_ASSERT_SPI_SYNC_READ_IB_0
    );

    EE_assert(
      EE_ASSERT_SPI_SYNC_READ_IB_2,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_2, rx[2]) == E_OK ),
      EE_ASSERT_SPI_SYNC_READ_IB_1
    );

    EE_assert(
      EE_ASSERT_SPI_SYNC_READ_IB_3,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_3, rx[3]) == E_OK ),
      EE_ASSERT_SPI_SYNC_READ_IB_2
    );

    EE_assert(
      EE_ASSERT_SPI_SYNC_READ_IB_4,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_4, rx[4]) == E_OK ),
      EE_ASSERT_SPI_SYNC_READ_IB_3
    );

    EE_assert(
      EE_ASSERT_SPI_SYNC_READ_IB_5,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_5, rx[5]) == E_OK ),
      EE_ASSERT_SPI_SYNC_READ_IB_4
    );

    EE_assert(
      EE_ASSERT_SPI_SYNC_READ_IB_6,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_6, rx[6]) == E_OK ),
      EE_ASSERT_SPI_SYNC_READ_IB_5
    );

    EE_assert(
      EE_ASSERT_SPI_SYNC_READ_IB_7,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_7, rx[7]) == E_OK ),
      EE_ASSERT_SPI_SYNC_READ_IB_6
    );

    EE_assert(
      EE_ASSERT_SPI_SYNC_READ_IB_8,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_8, rx[8]) == E_OK ),
      EE_ASSERT_SPI_SYNC_READ_IB_7
    );

    EE_assert(
      EE_ASSERT_SPI_SYNC_READ_IB_9,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_9, rx[9]) == E_OK ),
      EE_ASSERT_SPI_SYNC_READ_IB_8
    );

    EE_assert(
      EE_ASSERT_SPI_SYNC_READ_IB_10,
      ( Spi_ReadIB(SPI_LBM_CHANNEL_10, rx[10]) == E_OK ),
      EE_ASSERT_SPI_SYNC_READ_IB_9
    );

    EE_assert(
      EE_ASSERT_SPI_SYNC_VALIDATE_0,
      cmp_buff(0),
      EE_ASSERT_SPI_SYNC_READ_IB_10
    );

    EE_assert(
      EE_ASSERT_SPI_SYNC_VALIDATE_1,
      cmp_buff(1),
      EE_ASSERT_SPI_SYNC_VALIDATE_0
    );

    EE_assert(
      EE_ASSERT_SPI_SYNC_VALIDATE_2,
      cmp_buff(2),
      EE_ASSERT_SPI_SYNC_VALIDATE_1
    );

    EE_assert(
      EE_ASSERT_SPI_SYNC_VALIDATE_3,
      cmp_buff(3),
      EE_ASSERT_SPI_SYNC_VALIDATE_2
    );

    EE_assert(
      EE_ASSERT_SPI_SYNC_VALIDATE_4,
      cmp_buff(4),
      EE_ASSERT_SPI_SYNC_VALIDATE_3
    );

    EE_assert(
      EE_ASSERT_SPI_SYNC_VALIDATE_5,
      cmp_buff(5),
      EE_ASSERT_SPI_SYNC_VALIDATE_4
    );

    EE_assert(
      EE_ASSERT_SPI_SYNC_VALIDATE_6,
      cmp_buff(6),
      EE_ASSERT_SPI_SYNC_VALIDATE_5
    );

    EE_assert(
      EE_ASSERT_SPI_SYNC_VALIDATE_7,
      cmp_buff(7),
      EE_ASSERT_SPI_SYNC_VALIDATE_6
    );

    EE_assert(
      EE_ASSERT_SPI_SYNC_VALIDATE_8,
      cmp_buff(8),
      EE_ASSERT_SPI_SYNC_VALIDATE_7
    );

    EE_assert(
      EE_ASSERT_SPI_SYNC_VALIDATE_9,
      cmp_buff(9),
      EE_ASSERT_SPI_SYNC_VALIDATE_8
    );

    EE_assert(
      EE_ASSERT_SPI_SYNC_VALIDATE_10,
      cmp_buff(10),
      EE_ASSERT_SPI_SYNC_VALIDATE_9
    );

  }

  test++;

  setup_buff();

  EE_assert(
    EE_ASSERT_SPI_EB_SYNC_SETUP_0,
    ( 
      Spi_SetupEB(SPI_LBM_CHANNEL_0_EB, tx[0], rx[0], 1)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_SYNC_VALIDATE_10
  );

  EE_assert(
    EE_ASSERT_SPI_EB_SYNC_SETUP_1,
    ( 
      Spi_SetupEB(SPI_LBM_CHANNEL_1_EB, tx[1], rx[1], 2)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_SYNC_SETUP_0
  );

  EE_assert(
    EE_ASSERT_SPI_EB_SYNC_SETUP_2,
    ( 
      Spi_SetupEB(SPI_LBM_CHANNEL_2_EB, tx[2], rx[2], 3)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_SYNC_SETUP_1
  );

  EE_assert(
    EE_ASSERT_SPI_EB_SYNC_SETUP_3,
    ( 
      Spi_SetupEB(SPI_LBM_CHANNEL_3_EB, tx[3], rx[3], 4)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_SYNC_SETUP_2
  );

  EE_assert(
    EE_ASSERT_SPI_EB_SYNC_SETUP_4,
    ( 
      Spi_SetupEB(SPI_LBM_CHANNEL_4_EB, tx[4], rx[4], 5)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_SYNC_SETUP_3
  );

  EE_assert(
    EE_ASSERT_SPI_EB_SYNC_SETUP_5,
    ( 
      Spi_SetupEB(SPI_LBM_CHANNEL_5_EB, tx[5], rx[5], 6)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_SYNC_SETUP_4
  );

  EE_assert(
    EE_ASSERT_SPI_EB_SYNC_SETUP_6,
    ( 
      Spi_SetupEB(SPI_LBM_CHANNEL_6_EB, tx[6], rx[6], 7)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_SYNC_SETUP_5
  );

  EE_assert(
    EE_ASSERT_SPI_EB_SYNC_SETUP_7,
    ( 
      Spi_SetupEB(SPI_LBM_CHANNEL_7_EB, tx[7], rx[7], 8)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_SYNC_SETUP_6
  );

  EE_assert(
    EE_ASSERT_SPI_EB_SYNC_SETUP_8,
    ( 
      Spi_SetupEB(SPI_LBM_CHANNEL_8_EB, tx[8], rx[8], 9)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_SYNC_SETUP_7
  );

  EE_assert(
    EE_ASSERT_SPI_EB_SYNC_SETUP_9,
    ( 
      Spi_SetupEB(SPI_LBM_CHANNEL_9_EB, tx[9], rx[9], 10)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_SYNC_SETUP_8
  );

  EE_assert(
    EE_ASSERT_SPI_EB_SYNC_SETUP_10,
    ( 
      Spi_SetupEB(SPI_LBM_CHANNEL_10_EB, tx[10], rx[10], 11)
      ==
      E_OK
    ),
    EE_ASSERT_SPI_EB_SYNC_SETUP_9
  );

  EE_assert(
    EE_ASSERT_SPI_EB_SYNC_TX,
    ( Spi_SyncTransmit(SPI_LBM_SEQ_0_EB_SYNC) == E_OK ),
    EE_ASSERT_SPI_EB_SYNC_SETUP_10
  );

  EE_assert(
    EE_ASSERT_SPI_EB_SYNC_JOB_1_OK,
    ( Spi_GetJobResult(SPI_LBM_JOB_1_EB_SYNC) == SPI_JOB_OK ),
    EE_ASSERT_SPI_EB_SYNC_TX
  );

  EE_assert(
    EE_ASSERT_SPI_EB_SYNC_JOB_2_OK,
    ( Spi_GetJobResult(SPI_LBM_JOB_2_EB_SYNC) == SPI_JOB_OK ),
    EE_ASSERT_SPI_EB_SYNC_JOB_1_OK
  );

  EE_assert(
    EE_ASSERT_SPI_EB_SYNC_SEQ_OK,
    ( Spi_GetSeqResult(SPI_LBM_SEQ_0_EB_SYNC) == SPI_SEQ_OK ),
    EE_ASSERT_SPI_EB_SYNC_JOB_2_OK
  );

  if ( Spi_GetSeqResult(SPI_LBM_SEQ_0_EB_SYNC) == SPI_SEQ_OK ) {

    EE_assert(
      EE_ASSERT_SPI_EB_SYNC_VALIDATE_0,
      cmp_buff(0),
      EE_ASSERT_SPI_EB_SYNC_SEQ_OK
    );

    EE_assert(
      EE_ASSERT_SPI_EB_SYNC_VALIDATE_1,
      cmp_buff(1),
      EE_ASSERT_SPI_EB_SYNC_VALIDATE_0
    );

    EE_assert(
      EE_ASSERT_SPI_EB_SYNC_VALIDATE_2,
      cmp_buff(2),
      EE_ASSERT_SPI_EB_SYNC_VALIDATE_1
    );

    EE_assert(
      EE_ASSERT_SPI_EB_SYNC_VALIDATE_3,
      cmp_buff(3),
      EE_ASSERT_SPI_EB_SYNC_VALIDATE_2
    );

    EE_assert(
      EE_ASSERT_SPI_EB_SYNC_VALIDATE_4,
      cmp_buff(4),
      EE_ASSERT_SPI_EB_SYNC_VALIDATE_3
    );

    EE_assert(
      EE_ASSERT_SPI_EB_SYNC_VALIDATE_5,
      cmp_buff(5),
      EE_ASSERT_SPI_EB_SYNC_VALIDATE_4
    );

    EE_assert(
      EE_ASSERT_SPI_EB_SYNC_VALIDATE_6,
      cmp_buff(6),
      EE_ASSERT_SPI_EB_SYNC_VALIDATE_5
    );

    EE_assert(
      EE_ASSERT_SPI_EB_SYNC_VALIDATE_7,
      cmp_buff(7),
      EE_ASSERT_SPI_EB_SYNC_VALIDATE_6
    );

    EE_assert(
      EE_ASSERT_SPI_EB_SYNC_VALIDATE_8,
      cmp_buff(8),
      EE_ASSERT_SPI_EB_SYNC_VALIDATE_7
    );

    EE_assert(
      EE_ASSERT_SPI_EB_SYNC_VALIDATE_9,
      cmp_buff(9),
      EE_ASSERT_SPI_EB_SYNC_VALIDATE_8
    );

    EE_assert(
      EE_ASSERT_SPI_EB_SYNC_VALIDATE_10,
      cmp_buff(10),
      EE_ASSERT_SPI_EB_SYNC_VALIDATE_9
    );

  }

  EE_assert(
    EE_ASSERT_SPI_DEINIT,
    ( Spi_DeInit() == E_OK ),
    EE_ASSERT_SPI_EB_SYNC_VALIDATE_10
  );

  EE_assert_range(
    EE_ASSERT_FIN,
    ( Spi_GetStatus() == SPI_UNINIT ),
    EE_ASSERT_SPI_DEINIT
  );

  result = EE_assert_last();

  /* Forever loop: background activities (if any) should go here */
  for (;result == 1;)
  {
    while (counter % 100000) counter++;

    lvl = Dio_ReadChannelGroup(DIO_CHANNEL_GROUP_USER_LED);

    if (lvl == STD_HIGH)
      Dio_WriteChannelGroup(DIO_CHANNEL_GROUP_USER_LED, STD_LOW);
    else
      Dio_WriteChannelGroup(DIO_CHANNEL_GROUP_USER_LED, STD_HIGH);

    counter++;
  }

}