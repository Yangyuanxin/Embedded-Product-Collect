/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RT_LIST.H
 *      Purpose: Functions for the management of different lists
 *      Rev.:    V4.70
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2013 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

/* Definitions */

/* Values for 'cb_type' */
#define TCB             0
#define MCB             1
#define SCB             2
#define MUCB            3
#define HCB             4

/* Variables */
extern struct OS_XCB os_rdy;
extern struct OS_XCB os_dly;

/* Functions */
extern void  rt_put_prio      (P_XCB p_CB, P_TCB p_task);
extern P_TCB rt_get_first     (P_XCB p_CB);
extern void  rt_put_rdy_first (P_TCB p_task);
extern P_TCB rt_get_same_rdy_prio (void);
extern void  rt_resort_prio   (P_TCB p_task);
extern void  rt_put_dly       (P_TCB p_task, U16 delay);
extern void  rt_dec_dly       (void);
extern void  rt_rmv_list      (P_TCB p_task);
extern void  rt_rmv_dly       (P_TCB p_task);
extern void  rt_psq_enq       (OS_ID entry, U32 arg);

/* This is a fast macro generating in-line code */
#define rt_rdy_prio(void) (os_rdy.p_lnk->prio)


/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

