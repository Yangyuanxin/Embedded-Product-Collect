/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RT_TIMER.H
 *      Purpose: User timer functions
 *      Rev.:    V4.70
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2013 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

/* Variables */
extern struct OS_XTMR os_tmr;

/* Functions */
extern void  rt_tmr_tick   (void);
extern OS_ID rt_tmr_create (U16 tcnt, U16 info);
extern OS_ID rt_tmr_kill   (OS_ID timer);

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

