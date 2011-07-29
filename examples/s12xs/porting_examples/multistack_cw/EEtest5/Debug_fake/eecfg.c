
#include "ee.h"


//////////////////////////////////////////////////////////////////////////////
//  
//   Stack definition for Freescale S12
//  
/////////////////////////////////////////////////////////////////////////////
    #define STACK_1_SIZE 256 // size = 512 bytes 
    #define STACK_2_SIZE 256 // size = 512 bytes 
    #define STACK_3_SIZE 256 // size = 512 bytes 

    int EE_s12xs_stack_1[STACK_1_SIZE];	// Task 0 (Producer)
    int EE_s12xs_stack_2[STACK_2_SIZE];	// Task 1 (Consumer)
    int EE_s12xs_stack_3[STACK_3_SIZE];	// irq stack

    EE_UREG EE_s12xs_thread_tos[EE_MAX_TASK+1] = {
        0,	// dummy
        1,	// Producer
        2 	// Consumer
    };

    struct EE_TOS EE_s12xs_system_tos[3] = {
        {0},	// Task   (dummy)
        {(EE_DADD)(&EE_s12xs_stack_1[STACK_1_SIZE -3])},	// Task 0 (Producer)
        {(EE_DADD)(&EE_s12xs_stack_2[STACK_2_SIZE -3])} 	// Task 1 (Consumer)
    };

    EE_UREG EE_s12xs_active_tos = 0; // dummy

    // stack used only by IRQ handlers
    struct EE_TOS EE_s12xs_IRQ_tos = {
        (EE_DADD)(&EE_s12xs_stack_3[STACK_3_SIZE -3])
    };

#include "ee.h"


//////////////////////////////////////////////////////////////////////////////
//  
//   Kernel ( CPU 0 )
//  
/////////////////////////////////////////////////////////////////////////////
    // Definition of task's body
    DeclareTask(Producer);
    DeclareTask(Consumer);

    const EE_ADDR EE_hal_thread_body[EE_MAX_TASK] = {
        (EE_ADDR)FuncProducer,		// thread Producer
        (EE_ADDR)FuncConsumer 		// thread Consumer

    };

    // ready priority
    const EE_TYPEPRIO EE_th_ready_prio[EE_MAX_TASK] = {
        0x2,		// thread Producer
        0x1 		// thread Consumer
    };

    // dispatch priority
    const EE_TYPEPRIO EE_th_dispatch_prio[EE_MAX_TASK] = {
        0x2,		// thread Producer
        0x1 		// thread Consumer
    };

    // thread status
    #if defined(__MULTI__) || defined(__WITH_STATUS__)
        EE_TYPESTATUS EE_th_status[EE_MAX_TASK] = {
            EE_READY,
            EE_READY
        };
    #endif

    // next thread
    EE_TID EE_th_next[EE_MAX_TASK] = {
        EE_NIL,
        EE_NIL
    };

    EE_TYPEPRIO EE_th_nact[EE_MAX_TASK];
    // The first stacked task
    EE_TID EE_stkfirst = EE_NIL;

    // The first task into the ready queue
    EE_TID EE_rqfirst  = EE_NIL;

    // system ceiling
    EE_TYPEPRIO EE_sys_ceiling= 0x0000;
