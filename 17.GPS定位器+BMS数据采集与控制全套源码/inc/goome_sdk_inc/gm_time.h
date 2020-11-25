/*****************************************************************************
*  Copyright Statement:
*  --------------------
* www.goome.net
*
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   gm_time.h 
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *  Timer related APIs
 *
 * Author:
 * -------
 * -------
 *
 *============================================================================
 *             HISTORY
 *----------------------------------------------------------------------------
 * 
 ****************************************************************************/
 
 

#ifndef __GM_TIME_H__
#define __GM_TIME_H__
#include "gm_type.h"

typedef struct {
    s32 year;
    s32 month;
    s32 day;
    s32 hour;
    s32 minute;
    s32 second;
    s32 dayindex; /* 0=Sunday */
    float timezone;
}ST_Time;


/*****************************************************************
* Function:     GM_SetLocalTime 
* 
* Description:
*               Set the current local date and time.
*
* Parameters:
*               dateTime:
*                       [in] Point to the ST_Time object
* Return:        
*               GM_RET_OK indicates this function is executed successesfully.
*               GM_RET_ERR_PARAM, indicates the parameter is error.
*
*****************************************************************/
extern s32 GM_SetLocalTime(ST_Time* dateTime);

/*****************************************************************
* Function:     GM_GetLocalTime 
* 
* Description:
*               Get the current local date and time.
*
* Parameters:
*               dateTime:
*                       [out] Point  to the ST_Time object
* Return:        
*               if succeed,return the current local date and time
*               , NULL means get failure.
*****************************************************************/
extern s32 GM_GetLocalTime(ST_Time* dateTime,float timezone);

/*****************************************************************
* Function:     GM_GetTime 
* 
* Description:
*               Get the UTC time,the time since the Epoch (00:00:00 UTC, January 1, 1970)
*
* Parameters:
*            
*                       
* Return:       the UTC time      
*               
*****************************************************************/
extern U32 GM_GetTime(void);

#endif
