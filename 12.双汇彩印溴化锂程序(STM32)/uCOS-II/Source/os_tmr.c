/*
************************************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                            TIMER MANAGEMENT
*
*                              (c) Copyright 1992-2007, Micrium, Weston, FL
*                                           All Rights Reserved
*
*
* File    : OS_TMR.C
* By      : Jean J. Labrosse
* Version : V2.86
************************************************************************************************************************
*/

#include "ucos_ii.h"

/*
************************************************************************************************************************
*                                                        NOTES
*
* 1) Your application MUST define the following #define constants:
*
*    OS_TASK_TMR_PRIO          The priority of the Timer management task
*    OS_TASK_TMR_STK_SIZE      The size     of the Timer management task's stack
*
* 2) You must call OSTmrSignal() to notify the Timer management task that it's time to update the timers.
************************************************************************************************************************
*/

/*
************************************************************************************************************************
*                                                     CONSTANTS
************************************************************************************************************************
*/

#define  OS_TMR_LINK_DLY       0
#define  OS_TMR_LINK_PERIODIC  1

/*
************************************************************************************************************************
*                                                  LOCAL PROTOTYPES
************************************************************************************************************************
*/

#if OS_TMR_EN > 0
static  OS_TMR  *OSTmr_Alloc         (void);
static  void     OSTmr_Free          (OS_TMR *ptmr);
static  void     OSTmr_InitTask      (void);
static  void     OSTmr_Link          (OS_TMR *ptmr, INT8U type);
static  void     OSTmr_Unlink        (OS_TMR *ptmr);
static  void     OSTmr_Lock          (void);
static  void     OSTmr_Unlock        (void);
static  void     OSTmr_Task          (void   *p_arg);
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                   CREATE A TIMER
*
* Description: This function is called by your application code to create a timer.
*
* Arguments  : dly           Initial delay.
*                            If the timer is configured for ONE-SHOT mode, this is the timeout used
*                            If the timer is configured for PERIODIC mode, this is the first timeout to wait for
*                               before the timer starts entering periodic mode
*
*              period        The 'period' being repeated for the timer.
*                               If you specified 'OS_TMR_OPT_PERIODIC' as an option, when the timer expires, it will
*                               automatically restart with the same period.
*
*              opt           Specifies either:
*                               OS_TMR_OPT_ONE_SHOT       The timer counts down only once
*                               OS_TMR_OPT_PERIODIC       The timer counts down and then reloads itself
*
*              callback      Is a pointer to a callback function that will be called when the timer expires.  The
*                               callback function must be declared as follows:
*
*                               void MyCallback (OS_TMR *ptmr, void *p_arg);
*
*              callback_arg  Is an argument (a pointer) that is passed to the callback function when it is called.
*
*              pname         Is a pointer to an ASCII string that is used to name the timer.  Names are useful for
*                               debugging.  The length of the ASCII string for the name can be as big as:
*
*                               OS_TMR_CFG_NAME_SIZE and should be found in OS_CFG.H
*
*              perr          Is a pointer to an error code.  '*perr' will contain one of the following:
*                               OS_ERR_NONE
*                               OS_ERR_TMR_INVALID_DLY     you specified an invalid delay
*                               OS_ERR_TMR_INVALID_PERIOD  you specified an invalid period
*                               OS_ERR_TMR_INVALID_OPT     you specified an invalid option
*                               OS_ERR_TMR_ISR             if the call was made from an ISR
*                               OS_ERR_TMR_NON_AVAIL       if there are no free timers from the timer pool
*                               OS_ERR_TMR_NAME_TOO_LONG   if the timer name is too long to fit
*
* Returns    : A pointer to an OS_TMR data structure.  
*              This is the 'handle' that your application will use to reference the timer created.
************************************************************************************************************************
*/

#if OS_TMR_EN > 0
OS_TMR  *OSTmrCreate (INT32U           dly,
                      INT32U           period,
                      INT8U            opt,
                      OS_TMR_CALLBACK  callback,
                      void            *callback_arg,
                      INT8U           *pname,
                      INT8U           *perr)
{
    OS_TMR   *ptmr;
#if OS_TMR_CFG_NAME_SIZE > 0
    INT8U     len;
#endif


#if OS_ARG_CHK_EN > 0
    if (perr == (INT8U *)0) {                               /* Validate arguments                                     */
        return ((OS_TMR *)0);
    }
    switch (opt) {
        case OS_TMR_OPT_PERIODIC:
             if (period == 0) {
                 *perr = OS_ERR_TMR_INVALID_PERIOD;
                 return ((OS_TMR *)0);
             }
             break;

        case OS_TMR_OPT_ONE_SHOT:
             if (dly == 0) {
                 *perr = OS_ERR_TMR_INVALID_DLY;
                 return ((OS_TMR *)0);
             }
             break;

        default:
             *perr = OS_ERR_TMR_INVALID_OPT;
             return ((OS_TMR *)0);
    }
#endif
    if (OSIntNesting > 0) {                                 /* See if trying to call from an ISR                      */
        *perr  = OS_ERR_TMR_ISR;
        return ((OS_TMR *)0);
    }
    OSTmr_Lock();
    ptmr = OSTmr_Alloc();                                   /* Obtain a timer from the free pool                      */
    if (ptmr == (OS_TMR *)0) {
        OSTmr_Unlock();
        *perr = OS_ERR_TMR_NON_AVAIL;
        return ((OS_TMR *)0);
    }
    ptmr->OSTmrState       = OS_TMR_STATE_STOPPED;          /* Indicate that timer is not running yet                 */
    ptmr->OSTmrDly         = dly;
    ptmr->OSTmrPeriod      = period;
    ptmr->OSTmrOpt         = opt;
    ptmr->OSTmrCallback    = callback;
    ptmr->OSTmrCallbackArg = callback_arg;
#if OS_TMR_CFG_NAME_SIZE > 0
    if (pname !=(INT8U *)0) {
        len = OS_StrLen(pname);                             /* Copy timer name                                        */
        if (len < OS_TMR_CFG_NAME_SIZE) {
            (void)OS_StrCopy(ptmr->OSTmrName, pname);
        } else {
#if OS_TMR_CFG_NAME_SIZE > 1
            ptmr->OSTmrName[0] = '#';                       /* Invalid size specified                                 */
            ptmr->OSTmrName[1] = OS_ASCII_NUL;
#endif
            *perr              = OS_ERR_TMR_NAME_TOO_LONG;
            OSTmr_Unlock();
            return (ptmr);
        }
    }
#endif
    OSTmr_Unlock();
    *perr = OS_ERR_NONE;
    return (ptmr);
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                   DELETE A TIMER
*
* Description: This function is called by your application code to delete a timer.
*
* Arguments  : ptmr          Is a pointer to the timer to stop and delete.
*
*              perr          Is a pointer to an error code.  '*perr' will contain one of the following:
*                               OS_ERR_NONE
*                               OS_ERR_TMR_INVALID        'ptmr'  is a NULL pointer
*                               OS_ERR_TMR_INVALID_TYPE   'ptmr'  is not pointing to an OS_TMR
*                               OS_ERR_TMR_ISR            if the function was called from an ISR
*                               OS_ERR_TMR_INACTIVE       if the timer was not created
*                               OS_ERR_TMR_INVALID_STATE  the timer is in an invalid state
*
* Returns    : OS_TRUE       If the call was successful
*              OS_FALSE      If not
************************************************************************************************************************
*/

#if OS_TMR_EN > 0
BOOLEAN  OSTmrDel (OS_TMR  *ptmr,
                   INT8U   *perr)
{
#if OS_ARG_CHK_EN > 0
    if (perr == (INT8U *)0) {                               /* Validate arguments                                     */
        return (OS_FALSE);
    }
    if (ptmr == (OS_TMR *)0) {
        *perr = OS_ERR_TMR_INVALID;
        return (OS_FALSE);
    }
#endif
    if (ptmr->OSTmrType != OS_TMR_TYPE) {                   /* Validate timer structure                               */
        *perr = OS_ERR_TMR_INVALID_TYPE;
        return (OS_FALSE);
    }
    if (OSIntNesting > 0) {                                 /* See if trying to call from an ISR                      */
        *perr  = OS_ERR_TMR_ISR;
        return (OS_FALSE);
    }
    OSTmr_Lock();
    switch (ptmr->OSTmrState) {
        case OS_TMR_STATE_RUNNING:
             OSTmr_Unlink(ptmr);                            /* Remove from current wheel spoke                        */
             OSTmr_Free(ptmr);                              /* Return timer to free list of timers                    */
             OSTmr_Unlock();
             *perr = OS_ERR_NONE;
             return (OS_TRUE);

        case OS_TMR_STATE_STOPPED:                          /* Timer has not started or ...                           */
        case OS_TMR_STATE_COMPLETED:                        /* ... timer has completed the ONE-SHOT time              */
             OSTmr_Free(ptmr);                              /* Return timer to free list of timers                    */
             OSTmr_Unlock();
             *perr = OS_ERR_NONE;
             return (OS_TRUE);

        case OS_TMR_STATE_UNUSED:                           /* Already deleted                                        */
             OSTmr_Unlock();
             *perr = OS_ERR_TMR_INACTIVE;
             return (OS_FALSE);

        default:
             OSTmr_Unlock();
             *perr = OS_ERR_TMR_INVALID_STATE;
             return (OS_FALSE);
    }
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                             GET THE NAME OF A TIMER
*
* Description: This function is called to obtain the name of a timer.
*
* Arguments  : ptmr          Is a pointer to the timer to obtain the name for
*
*              pdest         Is a pointer to where the name of the timer will be placed.  It is the caller's responsibility
*                            to ensure that he has sufficient storage in the destination, i.e. at least OS_TMR_CFG_NAME_SIZE
*
*              perr          Is a pointer to an error code.  '*perr' will contain one of the following:
*                               OS_ERR_NONE               The call was successful
*                               OS_ERR_TMR_INVALID_DEST   'pdest' is a NULL pointer
*                               OS_ERR_TMR_INVALID        'ptmr'  is a NULL pointer
*                               OS_ERR_TMR_INVALID_TYPE   'ptmr'  is not pointing to an OS_TMR
*                               OS_ERR_NAME_GET_ISR       if the call was made from an ISR
*                               OS_ERR_TMR_INACTIVE       'ptmr'  points to a timer that is not active
*                               OS_ERR_TMR_INVALID_STATE  the timer is in an invalid state
*
* Returns    : The length of the string or 0 if the timer does not exist.
************************************************************************************************************************
*/

#if OS_TMR_EN > 0 && OS_TMR_CFG_NAME_SIZE > 0
INT8U  OSTmrNameGet (OS_TMR  *ptmr,
                     INT8U   *pdest,
                     INT8U   *perr)
{
    INT8U  len;


#if OS_ARG_CHK_EN > 0
    if (perr == (INT8U *)0) {
        return (0);
    }
    if (pdest == (INT8U *)0) {
        *perr = OS_ERR_TMR_INVALID_DEST;
        return (0);
    }
    if (ptmr == (OS_TMR *)0) {
        *perr = OS_ERR_TMR_INVALID;
        return (0);
    }
#endif
    if (ptmr->OSTmrType != OS_TMR_TYPE) {              /* Validate timer structure                                    */
        *perr = OS_ERR_TMR_INVALID_TYPE;
        return (0);
    }
    if (OSIntNesting > 0) {                            /* See if trying to call from an ISR                           */
        *perr = OS_ERR_NAME_GET_ISR;
        return (0);
    }
    OSTmr_Lock();
    switch (ptmr->OSTmrState) {
        case OS_TMR_STATE_RUNNING:
        case OS_TMR_STATE_STOPPED:
        case OS_TMR_STATE_COMPLETED:
             len   = OS_StrCopy(pdest, ptmr->OSTmrName);
             OSTmr_Unlock();
             *perr = OS_ERR_NONE;
             return (len);

        case OS_TMR_STATE_UNUSED:                      /* Timer is not allocated                                      */
             OSTmr_Unlock();
             *perr = OS_ERR_TMR_INACTIVE;
             return (0);

        default:
             OSTmr_Unlock();
             *perr = OS_ERR_TMR_INVALID_STATE;
             return (0);
    }
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                    GET HOW MUCH TIME IS LEFT BEFORE A TIMER EXPIRES
*
* Description: This function is called to get the number of ticks before a timer times out.
*
* Arguments  : ptmr          Is a pointer to the timer to obtain the remaining time from.
*
*              perr          Is a pointer to an error code.  '*perr' will contain one of the following:
*                               OS_ERR_NONE
*                               OS_ERR_TMR_INVALID        'ptmr' is a NULL pointer
*                               OS_ERR_TMR_INVALID_TYPE   'ptmr'  is not pointing to an OS_TMR
*                               OS_ERR_TMR_ISR            if the call was made from an ISR
*                               OS_ERR_TMR_INACTIVE       'ptmr' points to a timer that is not active
*                               OS_ERR_TMR_INVALID_STATE  the timer is in an invalid state
*
* Returns    : The time remaining for the timer to expire.  The time represents 'timer' increments.  In other words, if
*              OSTmr_Task() is signaled every 1/10 of a second then the returned value represents the number of 1/10 of
*              a second remaining before the timer expires.
************************************************************************************************************************
*/

#if OS_TMR_EN > 0
INT32U  OSTmrRemainGet (OS_TMR  *ptmr,
                        INT8U   *perr)
{
    INT32U  remain;


#if OS_ARG_CHK_EN > 0
    if (perr == (INT8U *)0) {
        return (0);
    }
    if (ptmr == (OS_TMR *)0) {
        *perr = OS_ERR_TMR_INVALID;
        return (0);
    }
#endif
    if (ptmr->OSTmrType != OS_TMR_TYPE) {              /* Validate timer structure                                    */
        *perr = OS_ERR_TMR_INVALID_TYPE;
        return (0);
    }
    if (OSIntNesting > 0) {                            /* See if trying to call from an ISR                           */
        *perr = OS_ERR_TMR_ISR;
        return (0);
    }
    OSTmr_Lock();
    switch (ptmr->OSTmrState) {
        case OS_TMR_STATE_RUNNING:
             remain = ptmr->OSTmrMatch - OSTmrTime;    /* Determine how much time is left to timeout                  */
             OSTmr_Unlock();
             *perr  = OS_ERR_NONE;
             return (remain);

        case OS_TMR_STATE_STOPPED:                     /* It's assumed that the timer has not started yet             */
             switch (ptmr->OSTmrOpt) {
                 case OS_TMR_OPT_PERIODIC:
                      if (ptmr->OSTmrDly == 0) {
                          remain = ptmr->OSTmrPeriod;
                      } else {
                          remain = ptmr->OSTmrDly;
                      }
                      OSTmr_Unlock();
                      *perr  = OS_ERR_NONE;
                      break;

                 case OS_TMR_OPT_ONE_SHOT:
                 default:
                      remain = ptmr->OSTmrDly;
                      OSTmr_Unlock();
                      *perr  = OS_ERR_NONE;
                      break;
             }
             return (remain);

        case OS_TMR_STATE_COMPLETED:                   /* Only ONE-SHOT that timed out can be in this state           */
             OSTmr_Unlock();
             *perr = OS_ERR_NONE;
             return (0);

        case OS_TMR_STATE_UNUSED:
             OSTmr_Unlock();
             *perr = OS_ERR_TMR_INACTIVE;
             return (0);

        default:
             OSTmr_Unlock();
             *perr = OS_ERR_TMR_INVALID_STATE;
             return (0);
    }
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                    FIND OUT WHAT STATE A TIMER IS IN
*
* Description: This function is called to determine what state the timer is in:
*
*                  OS_TMR_STATE_UNUSED     the timer has not been created
*                  OS_TMR_STATE_STOPPED    the timer has been created but has not been started or has been stopped
*                  OS_TMR_COMPLETED        the timer is in ONE-SHOT mode and has completed it's timeout
*                  OS_TMR_RUNNING          the timer is currently running
*
* Arguments  : ptmr          Is a pointer to the desired timer
*
*              perr          Is a pointer to an error code.  '*perr' will contain one of the following:
*                               OS_ERR_NONE
*                               OS_ERR_TMR_INVALID        'ptmr' is a NULL pointer
*                               OS_ERR_TMR_INVALID_TYPE   'ptmr'  is not pointing to an OS_TMR
*                               OS_ERR_TMR_ISR            if the call was made from an ISR
*                               OS_ERR_TMR_INACTIVE       'ptmr' points to a timer that is not active
*                               OS_ERR_TMR_INVALID_STATE  if the timer is not in a valid state
*
* Returns    : The current state of the timer (see description).
************************************************************************************************************************
*/

#if OS_TMR_EN > 0
INT8U  OSTmrStateGet (OS_TMR  *ptmr,
                      INT8U   *perr)
{
    INT8U  state;


#if OS_ARG_CHK_EN > 0
    if (perr == (INT8U *)0) {
        return (0);
    }
    if (ptmr == (OS_TMR *)0) {
        *perr = OS_ERR_TMR_INVALID;
        return (0);
    }
#endif
    if (ptmr->OSTmrType != OS_TMR_TYPE) {              /* Validate timer structure                                    */
        *perr = OS_ERR_TMR_INVALID_TYPE;
        return (0);
    }
    if (OSIntNesting > 0) {                            /* See if trying to call from an ISR                           */
        *perr = OS_ERR_TMR_ISR;
        return (0);
    }
    OSTmr_Lock();
    state = ptmr->OSTmrState;
    switch (state) {
        case OS_TMR_STATE_UNUSED:   
        case OS_TMR_STATE_STOPPED:  
        case OS_TMR_STATE_COMPLETED:
        case OS_TMR_STATE_RUNNING:  
             *perr = OS_ERR_NONE;
             break;
             
        default:
             *perr = OS_ERR_TMR_INVALID_STATE;
             break;
    }
    OSTmr_Unlock();
    return (state);
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                   START A TIMER
*
* Description: This function is called by your application code to start a timer.
*
* Arguments  : ptmr          Is a pointer to an OS_TMR
*
*              perr          Is a pointer to an error code.  '*perr' will contain one of the following:
*                               OS_ERR_NONE
*                               OS_ERR_TMR_INVALID
*                               OS_ERR_TMR_INVALID_TYPE    'ptmr'  is not pointing to an OS_TMR
*                               OS_ERR_TMR_ISR             if the call was made from an ISR
*                               OS_ERR_TMR_INACTIVE        if the timer was not created
*                               OS_ERR_TMR_INVALID_STATE   the timer is in an invalid state
*
* Returns    : OS_TRUE    if the timer was started
*              OS_FALSE   if an error was detected
************************************************************************************************************************
*/

#if OS_TMR_EN > 0
BOOLEAN  OSTmrStart (OS_TMR   *ptmr,
                     INT8U    *perr)
{
#if OS_ARG_CHK_EN > 0
    if (perr == (INT8U *)0) {                               /* Validate arguments                                     */
        return (OS_FALSE);
    }
    if (ptmr == (OS_TMR *)0) {
        *perr = OS_ERR_TMR_INVALID;
        return (OS_FALSE);
    }
#endif
    if (ptmr->OSTmrType != OS_TMR_TYPE) {                   /* Validate timer structure                               */
        *perr = OS_ERR_TMR_INVALID_TYPE;
        return (OS_FALSE);
    }
    if (OSIntNesting > 0) {                                 /* See if trying to call from an ISR                      */
        *perr  = OS_ERR_TMR_ISR;
        return (OS_FALSE);
    }
    OSTmr_Lock();
    switch (ptmr->OSTmrState) {
        case OS_TMR_STATE_RUNNING:                          /* Restart the timer                                      */
             OSTmr_Unlink(ptmr);                            /* ... Stop the timer                                     */
             OSTmr_Link(ptmr, OS_TMR_LINK_DLY);             /* ... Link timer to timer wheel                          */
             OSTmr_Unlock();
             *perr = OS_ERR_NONE;
             return (OS_TRUE);

        case OS_TMR_STATE_STOPPED:                          /* Start the timer                                        */
        case OS_TMR_STATE_COMPLETED:
             OSTmr_Link(ptmr, OS_TMR_LINK_DLY);             /* ... Link timer to timer wheel                          */
             OSTmr_Unlock();
             *perr = OS_ERR_NONE;
             return (OS_TRUE);

        case OS_TMR_STATE_UNUSED:                           /* Timer not created                                      */
             OSTmr_Unlock();
             *perr = OS_ERR_TMR_INACTIVE;
             return (OS_FALSE);

        default:
             OSTmr_Unlock();
             *perr = OS_ERR_TMR_INVALID_STATE;
             return (OS_FALSE);
    }
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                   STOP A TIMER
*
* Description: This function is called by your application code to stop a timer.
*
* Arguments  : ptmr          Is a pointer to the timer to stop.
*
*              opt           Allows you to specify an option to this functions which can be:
*
*                               OS_TMR_OPT_NONE          Do nothing special but stop the timer
*                               OS_TMR_OPT_CALLBACK      Execute the callback function, pass it the callback argument
*                                                        specified when the timer was created.
*                               OS_TMR_OPT_CALLBACK_ARG  Execute the callback function, pass it the callback argument
*                                                        specified in THIS function call
*
*              callback_arg  Is a pointer to a 'new' callback argument that can be passed to the callback function
*                               instead of the timer's callback argument.  In other words, use 'callback_arg' passed in
*                               THIS function INSTEAD of ptmr->OSTmrCallbackArg
*
*              perr          Is a pointer to an error code.  '*perr' will contain one of the following:
*                               OS_ERR_NONE
*                               OS_ERR_TMR_INVALID         'ptmr' is a NULL pointer
*                               OS_ERR_TMR_INVALID_TYPE    'ptmr'  is not pointing to an OS_TMR
*                               OS_ERR_TMR_ISR             if the function was called from an ISR
*                               OS_ERR_TMR_INACTIVE        if the timer was not created
*                               OS_ERR_TMR_INVALID_OPT     if you specified an invalid option for 'opt'
*                               OS_ERR_TMR_STOPPED         if the timer was already stopped
*                               OS_ERR_TMR_INVALID_STATE   the timer is in an invalid state
*                               OS_ERR_TMR_NO_CALLBACK     if the timer does not have a callback function defined
*
* Returns    : OS_TRUE       If we stopped the timer (if the timer is already stopped, we also return OS_TRUE)
*              OS_FALSE      If not
************************************************************************************************************************
*/

#if OS_TMR_EN > 0
BOOLEAN  OSTmrStop (OS_TMR  *ptmr,
                    INT8U    opt,
                    void    *callback_arg,
                    INT8U   *perr)
{
    OS_TMR_CALLBACK  pfnct;


#if OS_ARG_CHK_EN > 0
    if (perr == (INT8U *)0) {                                     /* Validate arguments                               */
        return (OS_FALSE);
    }
    if (ptmr == (OS_TMR *)0) {
        *perr = OS_ERR_TMR_INVALID;
        return (OS_FALSE);
    }
#endif
    if (ptmr->OSTmrType != OS_TMR_TYPE) {                         /* Validate timer structure                         */
        *perr = OS_ERR_TMR_INVALID_TYPE;
        return (OS_FALSE);
    }
    if (OSIntNesting > 0) {                                       /* See if trying to call from an ISR                */
        *perr  = OS_ERR_TMR_ISR;
        return (OS_FALSE);
    }
    OSTmr_Lock();
    switch (ptmr->OSTmrState) {
        case OS_TMR_STATE_RUNNING:
             OSTmr_Unlink(ptmr);                                  /* Remove from current wheel spoke                  */
             *perr = OS_ERR_NONE;
             switch (opt) {
                 case OS_TMR_OPT_CALLBACK:
                      pfnct = ptmr->OSTmrCallback;                /* Execute callback function if available ...       */
                      if (pfnct != (OS_TMR_CALLBACK)0) {
                          (*pfnct)((void *)ptmr, ptmr->OSTmrCallbackArg);  /* Use callback arg when timer was created */
                      } else {
                          *perr = OS_ERR_TMR_NO_CALLBACK;
                      }
                      break;

                 case OS_TMR_OPT_CALLBACK_ARG:
                      pfnct = ptmr->OSTmrCallback;                /* Execute callback function if available ...       */
                      if (pfnct != (OS_TMR_CALLBACK)0) {
                          (*pfnct)((void *)ptmr, callback_arg);   /* ... using the 'callback_arg' provided in call    */
                      } else {
                          *perr = OS_ERR_TMR_NO_CALLBACK;
                      }
                      break;

                 case OS_TMR_OPT_NONE:
                      break;

                 default:
                     *perr = OS_ERR_TMR_INVALID_OPT;
                     break;
             }
             OSTmr_Unlock();
             return (OS_TRUE);

        case OS_TMR_STATE_COMPLETED:                              /* Timer has already completed the ONE-SHOT or ...  */
        case OS_TMR_STATE_STOPPED:                                /* ... timer has not started yet.                   */
             OSTmr_Unlock();
             *perr = OS_ERR_TMR_STOPPED;
             return (OS_TRUE);

        case OS_TMR_STATE_UNUSED:                                 /* Timer was not created                            */
             OSTmr_Unlock();
             *perr = OS_ERR_TMR_INACTIVE;
             return (OS_FALSE);

        default:
             OSTmr_Unlock();
             *perr = OS_ERR_TMR_INVALID_STATE;
             return (OS_FALSE);
    }
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                      SIGNAL THAT IT'S TIME TO UPDATE THE TIMERS
*
* Description: This function is typically called by the ISR that occurs at the timer tick rate and is used to signal to
*              OSTmr_Task() that it's time to update the timers.
*
* Arguments  : none
*
* Returns    : OS_ERR_NONE         The call was successful and the timer task was signaled.
*              OS_ERR_SEM_OVF      If OSTmrSignal() was called more often than OSTmr_Task() can handle the timers.  
*                                  This would indicate that your system is heavily loaded.
*              OS_ERR_EVENT_TYPE   Unlikely you would get this error because the semaphore used for signaling is created 
*                                  by uC/OS-II.
*              OS_ERR_PEVENT_NULL  Again, unlikely you would ever get this error because the semaphore used for signaling 
*                                  is created by uC/OS-II.
************************************************************************************************************************
*/

#if OS_TMR_EN > 0
INT8U  OSTmrSignal (void)
{
    INT8U  err;


    err = OSSemPost(OSTmrSemSignal);
    return (err);
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                               ALLOCATE AND FREE A TIMER
*
* Description: This function is called to allocate a timer.
*
* Arguments  : none
*
* Returns    : a pointer to a timer if one is available
************************************************************************************************************************
*/

#if OS_TMR_EN > 0
static  OS_TMR  *OSTmr_Alloc (void)
{
    OS_TMR *ptmr;


    if (OSTmrFreeList == (OS_TMR *)0) {
        return ((OS_TMR *)0);
    }
    ptmr            = (OS_TMR *)OSTmrFreeList;
    OSTmrFreeList   = (OS_TMR *)ptmr->OSTmrNext;
    ptmr->OSTmrNext = (OS_TCB *)0;
    ptmr->OSTmrPrev = (OS_TCB *)0;
    OSTmrUsed++;
    OSTmrFree--;
    return (ptmr);
}
#endif


/*
************************************************************************************************************************
*                                             RETURN A TIMER TO THE FREE LIST
*
* Description: This function is called to return a timer object to the free list of timers.
*
* Arguments  : ptmr     is a pointer to the timer to free
*
* Returns    : none
************************************************************************************************************************
*/

#if OS_TMR_EN > 0
static  void  OSTmr_Free (OS_TMR *ptmr)
{
    ptmr->OSTmrState       = OS_TMR_STATE_UNUSED;      /* Clear timer object fields                                   */
    ptmr->OSTmrOpt         = OS_TMR_OPT_NONE;
    ptmr->OSTmrPeriod      = 0;
    ptmr->OSTmrMatch       = 0;
    ptmr->OSTmrCallback    = (OS_TMR_CALLBACK)0;
    ptmr->OSTmrCallbackArg = (void *)0;
#if OS_TMR_CFG_NAME_SIZE > 1
    ptmr->OSTmrName[0]     = '?';                      /* Unknown name                                                */
    ptmr->OSTmrName[1]     = OS_ASCII_NUL;
#endif

    ptmr->OSTmrPrev        = (OS_TCB *)0;              /* Chain timer to free list                                    */
    ptmr->OSTmrNext        = OSTmrFreeList;
    OSTmrFreeList          = ptmr;

    OSTmrUsed--;                                       /* Update timer object statistics                              */
    OSTmrFree++;
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                    INITIALIZATION
*                                          INITIALIZE THE FREE LIST OF TIMERS
*
* Description: This function is called by OSInit() to initialize the free list of OS_TMRs.
*
* Arguments  : none
*
* Returns    : none
************************************************************************************************************************
*/

#if OS_TMR_EN > 0
void  OSTmr_Init (void)
{
#if OS_EVENT_NAME_SIZE > 10
    INT8U    err;
#endif
    INT16U   i;
    OS_TMR  *ptmr1;
    OS_TMR  *ptmr2;


    OS_MemClr((INT8U *)&OSTmrTbl[0],      sizeof(OSTmrTbl));            /* Clear all the TMRs                         */
    OS_MemClr((INT8U *)&OSTmrWheelTbl[0], sizeof(OSTmrWheelTbl));       /* Clear the timer wheel                      */

    ptmr1 = &OSTmrTbl[0];
    ptmr2 = &OSTmrTbl[1];
    for (i = 0; i < (OS_TMR_CFG_MAX - 1); i++) {                        /* Init. list of free TMRs                    */
        ptmr1->OSTmrType    = OS_TMR_TYPE;
        ptmr1->OSTmrState   = OS_TMR_STATE_UNUSED;                      /* Indicate that timer is inactive            */
        ptmr1->OSTmrNext    = (void *)ptmr2;                            /* Link to next timer                         */
#if OS_TMR_CFG_NAME_SIZE > 1
        ptmr1->OSTmrName[0] = '?';                                      /* Unknown name                               */
        ptmr1->OSTmrName[1] = OS_ASCII_NUL;
#endif
        ptmr1++;
        ptmr2++;
    }
    ptmr1->OSTmrType    = OS_TMR_TYPE;
    ptmr1->OSTmrState   = OS_TMR_STATE_UNUSED;                          /* Indicate that timer is inactive            */
    ptmr1->OSTmrNext    = (void *)0;                                    /* Last OS_TMR                                */
#if OS_TMR_CFG_NAME_SIZE > 1
    ptmr1->OSTmrName[0] = '?';                                          /* Unknown name                               */
    ptmr1->OSTmrName[1] = OS_ASCII_NUL;
#endif
    OSTmrTime           = 0;
    OSTmrUsed           = 0;
    OSTmrFree           = OS_TMR_CFG_MAX;
    OSTmrFreeList       = &OSTmrTbl[0];
    OSTmrSem            = OSSemCreate(1);
    OSTmrSemSignal      = OSSemCreate(0);

#if OS_EVENT_NAME_SIZE > 18
    OSEventNameSet(OSTmrSem,       (INT8U *)"uC/OS-II TmrLock",   &err);/* Assign names to semaphores                 */
#else
#if OS_EVENT_NAME_SIZE > 10
    OSEventNameSet(OSTmrSem,       (INT8U *)"OS-TmrLock",         &err);
#endif
#endif

#if OS_EVENT_NAME_SIZE > 18
    OSEventNameSet(OSTmrSemSignal, (INT8U *)"uC/OS-II TmrSignal", &err);
#else
#if OS_EVENT_NAME_SIZE > 10
    OSEventNameSet(OSTmrSemSignal, (INT8U *)"OS-TmrSig",          &err);
#endif
#endif

    OSTmr_InitTask();
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                          INITIALIZE THE TIMER MANAGEMENT TASK
*
* Description: This function is called by OSTmrInit() to create the timer management task.
*
* Arguments  : none
*
* Returns    : none
************************************************************************************************************************
*/

#if OS_TMR_EN > 0
static  void  OSTmr_InitTask (void)
{
#if OS_TASK_NAME_SIZE > 6
    INT8U  err;
#endif


#if OS_TASK_CREATE_EXT_EN > 0
    #if OS_STK_GROWTH == 1
    (void)OSTaskCreateExt(OSTmr_Task,
                          (void *)0,                                       /* No arguments passed to OSTmrTask()      */
                          &OSTmrTaskStk[OS_TASK_TMR_STK_SIZE - 1],         /* Set Top-Of-Stack                        */
                          OS_TASK_TMR_PRIO,
                          OS_TASK_TMR_ID,
                          &OSTmrTaskStk[0],                                /* Set Bottom-Of-Stack                     */
                          OS_TASK_TMR_STK_SIZE,
                          (void *)0,                                       /* No TCB extension                        */
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);      /* Enable stack checking + clear stack     */
    #else
    (void)OSTaskCreateExt(OSTmr_Task,
                          (void *)0,                                       /* No arguments passed to OSTmrTask()      */
                          &OSTmrTaskStk[0],                                /* Set Top-Of-Stack                        */
                          OS_TASK_TMR_PRIO,
                          OS_TASK_TMR_ID,
                          &OSTmrTaskStk[OS_TASK_TMR_STK_SIZE - 1],         /* Set Bottom-Of-Stack                     */
                          OS_TASK_TMR_STK_SIZE,
                          (void *)0,                                       /* No TCB extension                        */
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);      /* Enable stack checking + clear stack     */
    #endif
#else
    #if OS_STK_GROWTH == 1
    (void)OSTaskCreate(OSTmr_Task,
                       (void *)0,
                       &OSTmrTaskStk[OS_TASK_TMR_STK_SIZE - 1],
                       OS_TASK_TMR_PRIO);
    #else
    (void)OSTaskCreate(OSTmr_Task,
                       (void *)0,
                       &OSTmrTaskStk[0],
                       OS_TASK_TMR_PRIO);
    #endif
#endif

#if OS_TASK_NAME_SIZE > 12
    OSTaskNameSet(OS_TASK_TMR_PRIO, (INT8U *)"uC/OS-II Tmr", &err);
#else
#if OS_TASK_NAME_SIZE > 6
    OSTaskNameSet(OS_TASK_TMR_PRIO, (INT8U *)"OS-Tmr", &err);
#endif
#endif
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                         INSERT A TIMER INTO THE TIMER WHEEL
*
* Description: This function is called to insert the timer into the timer wheel.  The timer is always inserted at the
*              beginning of the list.
*
* Arguments  : ptmr          Is a pointer to the timer to insert.
*
*              type          Is either:
*                               OS_TMR_LINK_PERIODIC    Means to re-insert the timer after a period expired
*                               OS_TMR_LINK_DLY         Means to insert    the timer the first time
*
* Returns    : none
************************************************************************************************************************
*/

#if OS_TMR_EN > 0
static  void  OSTmr_Link (OS_TMR *ptmr, INT8U type)
{
    OS_TMR       *ptmr1;
    OS_TMR_WHEEL *pspoke;
    INT16U        spoke;


    ptmr->OSTmrState = OS_TMR_STATE_RUNNING;
    if (type == OS_TMR_LINK_PERIODIC) {                            /* Determine when timer will expire                */
        ptmr->OSTmrMatch = ptmr->OSTmrPeriod + OSTmrTime;
    } else {
        if (ptmr->OSTmrDly == 0) {
            ptmr->OSTmrMatch = ptmr->OSTmrPeriod + OSTmrTime;
        } else {
            ptmr->OSTmrMatch = ptmr->OSTmrDly    + OSTmrTime;
        }
    }
    spoke  = (INT16U)(ptmr->OSTmrMatch % OS_TMR_CFG_WHEEL_SIZE);
    pspoke = &OSTmrWheelTbl[spoke];

    if (pspoke->OSTmrFirst == (OS_TMR *)0) {                       /* Link into timer wheel                           */
        pspoke->OSTmrFirst   = ptmr;
        ptmr->OSTmrNext      = (OS_TMR *)0;
        pspoke->OSTmrEntries = 1;
    } else {
        ptmr1                = pspoke->OSTmrFirst;                 /* Point to first timer in the spoke               */
        pspoke->OSTmrFirst   = ptmr;
        ptmr->OSTmrNext      = (void *)ptmr1;
        ptmr1->OSTmrPrev     = (void *)ptmr;
        pspoke->OSTmrEntries++;
    }
    ptmr->OSTmrPrev = (void *)0;                                   /* Timer always inserted as first node in list     */
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                         REMOVE A TIMER FROM THE TIMER WHEEL
*
* Description: This function is called to remove the timer from the timer wheel.
*
* Arguments  : ptmr          Is a pointer to the timer to remove.
*
* Returns    : none
************************************************************************************************************************
*/

#if OS_TMR_EN > 0
static  void  OSTmr_Unlink (OS_TMR *ptmr)
{
    OS_TMR        *ptmr1;
    OS_TMR        *ptmr2;
    OS_TMR_WHEEL  *pspoke;
    INT16U         spoke;


    spoke  = (INT16U)(ptmr->OSTmrMatch % OS_TMR_CFG_WHEEL_SIZE);
    pspoke = &OSTmrWheelTbl[spoke];

    if (pspoke->OSTmrFirst == ptmr) {                       /* See if timer to remove is at the beginning of list     */
        ptmr1              = (OS_TMR *)ptmr->OSTmrNext;
        pspoke->OSTmrFirst = (OS_TMR *)ptmr1;
        if (ptmr1 != (OS_TMR *)0) {
            ptmr1->OSTmrPrev = (void *)0;
        }
    } else {
        ptmr1            = (OS_TMR *)ptmr->OSTmrPrev;       /* Remove timer from somewhere in the list                */
        ptmr2            = (OS_TMR *)ptmr->OSTmrNext;
        ptmr1->OSTmrNext = ptmr2;
        if (ptmr2 != (OS_TMR *)0) {
            ptmr2->OSTmrPrev = (void *)ptmr1;
        }
    }
    ptmr->OSTmrState = OS_TMR_STATE_STOPPED;
    ptmr->OSTmrNext  = (void *)0;
    ptmr->OSTmrPrev  = (void *)0;
    pspoke->OSTmrEntries--;
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                       TIMER MANAGER DATA STRUCTURE LOCKING MECHANISM
*
* Description: These functions are used to gain exclusive access to timer management data structures.
*
* Arguments  : none
*
* Returns    : none
************************************************************************************************************************
*/

#if OS_TMR_EN > 0
static  void  OSTmr_Lock (void)
{
    INT8U  err;


    OSSemPend(OSTmrSem, 0, &err);
    (void)err;
}
#endif



#if OS_TMR_EN > 0
static  void  OSTmr_Unlock (void)
{
    (void)OSSemPost(OSTmrSem);
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                 TIMER MANAGEMENT TASK
*
* Description: This task is created by OSTmrInit().
*
* Arguments  : none
*
* Returns    : none
************************************************************************************************************************
*/

#if OS_TMR_EN > 0
static  void  OSTmr_Task (void *p_arg)
{
    INT8U            err;
    OS_TMR          *ptmr;
    OS_TMR          *ptmr_next;
    OS_TMR_CALLBACK  pfnct;
    OS_TMR_WHEEL    *pspoke;
    INT16U           spoke;


    (void)p_arg;                                                 /* Not using 'p_arg', prevent compiler warning       */
    for (;;) {
        OSSemPend(OSTmrSemSignal, 0, &err);                      /* Wait for signal indicating time to update timers  */
        OSTmr_Lock();
        OSTmrTime++;                                             /* Increment the current time                        */
        spoke  = (INT16U)(OSTmrTime % OS_TMR_CFG_WHEEL_SIZE);    /* Position on current timer wheel entry             */
        pspoke = &OSTmrWheelTbl[spoke];
        ptmr   = pspoke->OSTmrFirst;
        while (ptmr != (OS_TMR *)0) {
            ptmr_next = (OS_TMR *)ptmr->OSTmrNext;               /* Point to next timer to update because current ... */
                                                                 /* ... timer could get unlinked from the wheel.      */
            if (OSTmrTime == ptmr->OSTmrMatch) {                 /* Process each timer that expires                   */
                pfnct = ptmr->OSTmrCallback;                     /* Execute callback function if available            */
                if (pfnct != (OS_TMR_CALLBACK)0) {
                    (*pfnct)((void *)ptmr, ptmr->OSTmrCallbackArg);
                }
                OSTmr_Unlink(ptmr);                              /* Remove from current wheel spoke                   */
                if (ptmr->OSTmrOpt == OS_TMR_OPT_PERIODIC) {
                    OSTmr_Link(ptmr, OS_TMR_LINK_PERIODIC);      /* Recalculate new position of timer in wheel        */
                } else {
                    ptmr->OSTmrState = OS_TMR_STATE_COMPLETED;   /* Indicate that the timer has completed             */
                }
            }
            ptmr = ptmr_next;
        }
        OSTmr_Unlock();
    }
}
#endif
