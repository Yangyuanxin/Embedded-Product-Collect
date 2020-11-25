/*****************************************************************************
*  Copyright Statement:
*  --------------------
* www.goome.net
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   gm_sms.h 
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *  System related APIs
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
 
#ifndef __GM_SMS_H__
#define __GM_SMS_H__

typedef enum
{
   GM_DEFAULT_DCS     = 0x00,  /* GSM 7-bit */
   GM_8BIT_DCS           = 0x04,  /* 8-bit */
   GM_UCS2_DCS          = 0x08  /* UCS2 */  
} ENUM_SMSAL_DCS;

typedef struct
{
   char asciiNum[22];
   U8 smstype;
   S32 messageLen;
   char* content;
} gm_sms_new_msg_struct;

extern s32 GM_SendSMS(char* pAsciiDstNumber, char * pSMSContent, ENUM_SMSAL_DCS contentDcs, PsFuncPtr callback);

#endif  /*__GM_SMS_H__*/
