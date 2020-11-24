/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RT_SYSTEM.H
 *      Purpose: System Task Manager definitions
 *      Rev.:    V4.70
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2013 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

/* Variables */
#define os_psq  ((P_PSQ)&os_fifo)
extern int os_tick_irqn;

/* Functions */
extern U32  rt_suspend    (void);
extern void rt_resume     (U32 sleep_time);
extern void rt_tsk_lock   (void);
extern void rt_tsk_unlock (void);
extern void rt_psh_req    (void);
extern void rt_pop_req    (void);
extern void rt_systick    (void);
extern void rt_stk_check  (void);

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

