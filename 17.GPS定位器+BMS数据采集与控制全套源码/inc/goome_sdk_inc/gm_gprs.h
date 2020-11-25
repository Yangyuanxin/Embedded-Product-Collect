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
 *   gm_gprs.h 
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *   GPIO API defines.
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


#ifndef __GM_GPRS_H__
#define __GM_GPRS_H__


#define MAX_GPRS_USER_NAME_LEN 32
#define MAX_GPRS_PASSWORD_LEN  32
#define MAX_GPRS_APN_LEN       100
#define MAX_GPRS_MESSAGE_LEN       1048576
#define MAX_GPRS_PART_MESSAGE_TIMEOUT    3

#define  SW_DNS_REQUEST_ID	         0x055A5121

typedef struct {
    u8 apnName[MAX_GPRS_APN_LEN];
    u8 apnUserId[MAX_GPRS_USER_NAME_LEN]; 
    u8 apnPasswd[MAX_GPRS_PASSWORD_LEN]; 
    u8 authtype; // pap or chap

} ST_GprsConfig;



/* event */
typedef enum
{
    GM_SOC_READ    = 0x01,  /* Notify for read */
    GM_SOC_WRITE   = 0x02,  /* Notify for write */
    GM_SOC_ACCEPT  = 0x04,  /* Notify for accept */
    GM_SOC_CONNECT = 0x08,  /* Notify for connect */
    GM_SOC_CLOSE   = 0x10   /* Notify for close */
} gm_soc_event_enum;

/* Socket return codes, negative values stand for errors */
typedef enum
{
    GM_SOC_SUCCESS           = 0,     /* success */
    GM_SOC_ERROR             = -1,    /* error */
    GM_SOC_WOULDBLOCK        = -2,    /* not done yet */
    GM_SOC_LIMIT_RESOURCE    = -3,    /* limited resource */
    GM_SOC_INVALID_SOCKET    = -4,    /* invalid socket */
    GM_SOC_INVALID_ACCOUNT   = -5,    /* invalid account id */
    GM_SOC_NAMETOOLONG       = -6,    /* address too long */
    GM_SOC_ALREADY           = -7,    /* operation already in progress */
    GM_SOC_OPNOTSUPP         = -8,    /* operation not support */
    GM_SOC_CONNABORTED       = -9,    /* Software caused connection abort */
    GM_SOC_INVAL             = -10,   /* invalid argument */
    GM_SOC_PIPE              = -11,   /* broken pipe */
    GM_SOC_NOTCONN           = -12,   /* socket is not connected */
    GM_SOC_MSGSIZE           = -13,   /* msg is too long */
    GM_SOC_BEARER_FAIL       = -14,   /* bearer is broken */
    GM_SOC_CONNRESET         = -15,   /* TCP half-write close, i.e., FINED */
    GM_SOC_DHCP_ERROR        = -16,   /* DHCP error */
    GM_SOC_IP_CHANGED        = -17,   /* IP has changed */
    GM_SOC_ADDRINUSE         = -18,   /* address already in use */
    GM_SOC_CANCEL_ACT_BEARER = -19,    /* cancel the activation of bearer */
    
    GM_LOG_LOGIN_FAIL = -115,  // Dec-08-2016  登录无应答       5 
    GM_LOG_CONFIG_PARAM = -116,  // 4  参数配置                4
    GM_LOG_ACK_ERR = -117,  // 3 +120                         3
    GM_LOG_HEART_ERROR = -118, // 2,
    GM_LOG_SIM_DROP = -119, //  1,
} gm_soc_error_enum;


typedef struct
{
   kal_uint8       ref_count;
   kal_uint16      msg_len;	
   kal_int8        socket_id;    /* socket ID */
   gm_soc_event_enum  event_type;   /* soc_event_enum */
   kal_bool        result;       /* notification result. KAL_TRUE: success, KAL_FALSE: error */
   gm_soc_error_enum  error_cause;  /* used only when EVENT is close/connect */
   kal_int32       detail_cause; /* refer to ps_cause_enum if error_cause is
                                  * SOC_BEARER_FAIL */
} gm_soc_notify_ind_struct;






extern S32 GM_ApnConfig(ST_GprsConfig* apn, S32 *account_id);
extern s32 GM_GetApnMatch(kal_uint8 *dtcnt);
extern s32 GM_DeleteApn(kal_uint8 *Apn);
extern  u8 GM_GetHostByName(const char *hostName, U32 acctId, u8 *result);
extern int GM_SocketCreate(U32 account_id,U8 socType);
extern U32 GM_SocketConnect(S32 request_id, const char *hostName, U16 dstPort,U32 acctId, U32 access_id,U8 socType);
extern S32 GM_SocketRecv (int socketId, const char *buf, S32 len);
extern int GM_SocketClose(int socketId);
extern S32 GM_SocketSend(int socketId, const char *buf, S32 len);
extern s32 GM_SocketRegisterCallBack(PsFuncPtr callback);

extern u8 GM_GetBearerStatus(s32 *bear_status);
extern s32 GM_AccountIdClose(void);
extern char GM_GetSetFlightMode(char is_set, char *on_off);
extern  u8 GM_CloseBearerAppId(u8 is_close_bearer, kal_int8 *result);
extern  u8 GM_GetLocalIP(u8 *local_ip);
/*****************************************************************
* Function:     GM_ConvertIpAddr
*
* Description:
*               This function checks whether an IP address is valid
*               IP address or not. If 'yes', each segment of the
*               IP address string will be converted into integer to
*               store in ipaddr parameter.
*
* Parameters:
*               addrString:
*                   [in] IP address string.
*
*               ipaddr:
*                   [out] Point to a IPv4 address, each byte stores the IP 
digit
*                   converted from the corresponding IP string.
*
* Return:
*               If no error occurs, this function returns SOC_SUCCESS (0).
*               Otherwise, a value of Enum_SocErrCode is returned.
*****************************************************************/
extern s32 GM_ConvertIpAddr(u8 *addrString, u8* ipAddr);



#endif
