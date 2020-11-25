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
 *   gm_callback.h 
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
 

#ifndef __GM_CALLBACK_H__
#define __GM_CALLBACK_H__

#include <gm_type.h>

typedef enum {
    GM_CB_UART1_RECEIVE,
    GM_CB_UART2_RECEIVE,
    GM_CB_UART3_RECEIVE,
    GM_CB_SMS_RECEIVE,
    GM_CB_CALL_RECEIVE,
    GM_CB_FAKE_CELL_RECEIVE,	
    GM_CB_RECEIVE_END
}GmCallbackIdEnum;

extern S32 GM_RegisterCallBack(GmCallbackIdEnum call_back_id,U32 call_back_function);


#endif  /*__GM_CALLBACK_H__*/




