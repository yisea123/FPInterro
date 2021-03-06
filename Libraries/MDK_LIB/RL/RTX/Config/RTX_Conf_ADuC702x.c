/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RTX_CONFIG.C
 *      Purpose: Configuration of RTX Kernel for ADI ADuC702x
 *      Rev.:    V4.70
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2013 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <RTL.h>
#include <ADuC7024.H>

/*----------------------------------------------------------------------------
 *      RTX User configuration part BEGIN
 *---------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <h>Task Configuration
// =====================
//
//   <o>Number of concurrent running tasks <0-250>
//   <i> Define max. number of tasks that will run at the same time.
//   <i> Default: 6
#ifndef OS_TASKCNT
 #define OS_TASKCNT     6
#endif

//   <o>Number of tasks with user-provided stack <0-250>
//   <i> Define the number of tasks that will use a bigger stack.
//   <i> The memory space for the stack is provided by the user.
//   <i> Default: 0
#ifndef OS_PRIVCNT
 #define OS_PRIVCNT     0
#endif

//   <o>Task stack size [bytes] <20-4096:8><#/4>
//   <i> Set the stack size for tasks which is assigned by the system.
//   <i> Default: 200
#ifndef OS_STKSIZE
 #define OS_STKSIZE     50
#endif

// <q>Check for the stack overflow
// ===============================
// <i> Include the stack checking code for a stack overflow.
// <i> Note that additional code reduces the RTX performance.
#ifndef OS_STKCHECK
 #define OS_STKCHECK    1
#endif

// </h>
// <h>Tick Timer Configuration
// =============================
//   <o>Hardware timer <0=> Timer 0 (RTOS)
//   <i> Define the on-chip timer used as a time-base for RTX.
//   <i> Default: Timer 0
#ifndef OS_TIMER
 #define OS_TIMER       0
#endif

//   <o>Timer clock value [Hz] <1-1000000000>
//   <i> Set the timer clock value for selected timer.
//   <i> Default: 1409024  (1.409MHz at 22.544MHz Core Clock and prescaler by 16)
#ifndef OS_CLOCK
 #define OS_CLOCK       1409024
#endif

//   <o>Timer tick value [us] <1-1000000>
//   <i> Set the timer tick value for selected timer.
//   <i> Default: 10000  (10ms)
#ifndef OS_TICK
 #define OS_TICK        10000
#endif

// </h>

// <h>System Configuration
// =======================
// <e>Round-Robin Task switching
// =============================
// <i> Enable Round-Robin Task switching.
#ifndef OS_ROBIN
 #define OS_ROBIN       1
#endif

//   <o>Round-Robin Timeout [ticks] <1-1000>
//   <i> Define how long a task will execute before a task switch.
//   <i> Default: 5
#ifndef OS_ROBINTOUT
 #define OS_ROBINTOUT   5
#endif

// </e>

//   <o>Number of user timers <0-250>
//   <i> Define max. number of user timers that will run at the same time.
//   <i> Default: 0  (User timers disabled)
#ifndef OS_TIMERCNT
 #define OS_TIMERCNT    0
#endif

//   <o>ISR FIFO Queue size<4=>   4 entries  <8=>   8 entries
//                         <12=> 12 entries  <16=> 16 entries
//                         <24=> 24 entries  <32=> 32 entries
//                         <48=> 48 entries  <64=> 64 entries
//                         <96=> 96 entries
//   <i> ISR functions store requests to this buffer,
//   <i> when they are called from the IRQ handler.
//   <i> Default: 16 entries
#ifndef OS_FIFOSZ
 #define OS_FIFOSZ      16
#endif

// </h>


//------------- <<< end of configuration section >>> -----------------------

// Standard library system mutexes
// ===============================
//  Define max. number system mutexes that are used to protect 
//  the arm standard runtime library. For microlib they are not used.
#ifndef OS_MUTEXCNT
 #define OS_MUTEXCNT    8
#endif

/*----------------------------------------------------------------------------
 *      RTX User configuration part END
 *---------------------------------------------------------------------------*/

#if   (OS_TIMER == 0)                                   /* Timer 0 (RTOS)   */
 #define OS_TID_        2                               /*  Timer ID        */
 #define OS_TIM_        (1 << OS_TID_)                  /*  Interrupt Mask  */
 #define OS_TRV         ((U16)(((double)OS_CLOCK*(double)OS_TICK)/1E6)-1)
 #define OS_TVAL        (OS_TRV - T0VAL)                /*  Timer Value     */
 #define OS_TOVF        (IRQSIG & OS_TIM_)              /*  Overflow Flag   */
 #define OS_TFIRQ()     if (IRQEN & OS_TIM_) SWICFG |= 2;/* Force Interrupt */ \
                        else          force_irq = __TRUE;
 #define OS_TIACK()     SWICFG &= ~2;                   /*  Interrupt Ack   */ \
                        T0CLRI  =  1;                                          \
                        force_irq = __FALSE;
 #define OS_TINIT()     T0LD    =  OS_TRV;              /*  Initialization  */ \
                        T0CON   =  0xC4;
#else
 #error OS_TIMER invalid
#endif

#define OS_IACK()       ;                               /* Interrupt Ack    */

#define OS_LOCK()       IRQCLR  =  OS_TIM_;             /* Task Lock        */
#define OS_UNLOCK()     IRQEN  |=  OS_TIM_;             /* Task Unlock      */ \
                        if (force_irq) SWICFG |= 2;

/* WARNING: Using IDLE mode might cause you troubles while debugging. */
#define _idle_()        ;

static BIT force_irq;

/*----------------------------------------------------------------------------
 *      Global Functions
 *---------------------------------------------------------------------------*/

extern void os_clock_interrupt (void);
void os_def_interrupt   (void) __irq;

/*--------------------------- IRQ_Handler -----------------------------------*/

/* User Interrupt Functions
extern __irq void IRQx (void);                 // User IRQx Function
extern __irq void IRQy (void);                 // User IRQy Function

#define mIRQx       0x00000080                 // User IRQx Mask
#define mIRQy       0x00004000                 // User IRQy Mask
*/

__asm void IRQ_Handler (void) {
   /* Common IRQ Handler */
        PRESERVE8
        ARM

        STMDB   SP!,{R0}                       ; Save R0
        LDR     R0,=__cpp((U32)&IRQSTA)        ; Load IRQSTA Address
        LDR     R0,[R0]                        ; Load IRQSTA Value

//      TST     R0,#mIRQx                      ; Check IRQx Flag
//      LDMNEIA SP!,{R0}                       ; Restore R0
//      LDRNE   PC,=__cpp(IRQx)                ; IRQx Function

//      TST     R0,#mIRQy                      ; Check IRQy Flag
//      LDMNEIA SP!,{R0}                       ; Restore R0
//      LDRNE   PC,=__cpp(IRQy)                ; IRQy Function

        TST     R0,#(SWI_BIT :OR: RTOS_TIMER_BIT) ; Check OS IRQ Flag
        LDMIA   SP!,{R0}                       ; Restore R0
        LDRNE   PC,=__cpp(os_clock_interrupt)  ; OS Clock IRQ Function

        LDR     PC,=__cpp(os_def_interrupt)    ; OS Default IRQ Function
}


/*--------------------------- os_idle_demon ---------------------------------*/

__task void os_idle_demon (void) {
  /* The idle demon is a system task, running when no other task is ready */
  /* to run. The 'os_xxx' function calls are not allowed from this task.  */

  for (;;) {
  /* HERE: include optional user code to be executed when no task runs.*/
  }
}


/*--------------------------- os_tmr_call -----------------------------------*/

void os_tmr_call (U16 info) {
  /* This function is called when the user timer has expired. Parameter  */
  /* 'info' holds the value, defined when the timer was created.         */

  /* HERE: include optional user code to be executed on timeout. */
}


/*--------------------------- os_error --------------------------------------*/

void os_error (U32 err_code) {
  /* This function is called when a runtime error is detected. Parameter */
  /* 'err_code' holds the runtime error code (defined in RTL.H).         */

  /* HERE: include optional code to be executed on runtime error. */
  for (;;);
}


/*----------------------------------------------------------------------------
 *      RTX Configuration Functions
 *---------------------------------------------------------------------------*/

void os_def_interrupt (void) __irq  {
  /* Default Interrupt Function: may be called when timer ISR is disabled */
  OS_IACK();
}


#include <RTX_lib.c>

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
