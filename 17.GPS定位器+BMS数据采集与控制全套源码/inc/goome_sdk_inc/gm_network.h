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
 *   gm_network.h 
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
 
#ifndef __GM_NETWORK_H__
#define __GM_NETWORK_H__

#include "gm_type.h"


typedef enum
{
	GM_ACTIVE_CARD_EMPTY = 0,
	GM_ACTIVE_CARD_SIM1,
	GM_ACTIVE_CARD_SIM2
}GM_SIM_ENUM;



typedef struct
{
    U16 mcc;  // simCC;  460: 0x1CC 国家代码
    U16 mnc;  // simNC; 00: 移动 01:  营运商
    U16 lac;  // wLac; 小区编号Lca     区号262C
    U16 ci;    // wLci 小区ID     CellId     塔号 EBA
    U16 arfcn;
    U8 bsic;
    U8 rxlev;
}gm_cell_id_struct;


typedef struct
{
    gm_cell_id_struct serv_info;
    kal_uint8 ta;
    kal_uint8 nbr_cell_num;
    gm_cell_id_struct nbr_cell_info[6];
}gm_cell_info_struct;


#define MAX_PLMN_LEN             6

typedef struct
{
    kal_uint8   ref_count;
    kal_uint16  msg_len;
    kal_uint8   status; /* l4c_rac_response_enum */
    kal_uint8   plmn[MAX_PLMN_LEN+1];
    kal_uint8   gsm_state;
    kal_uint8   gprs_state;
    kal_uint8   gprs_status; /* l4c_gprs_status_enum */
    kal_uint8   rat;
    kal_uint8   cell_support_egprs;
    kal_uint8   lac[2];
    kal_uint8   rac;
    kal_uint16  cell_id;
    kal_uint8   cause;
    kal_uint8   data_speed_support;
    kal_bool    is_on_hplmn;
    kal_uint8   domain; /* domain_id_enum */
}mmi_nw_attach_ind_struct;



#ifdef DSFSD
typedef enum
{
    SRV_NW_INFO_SIM_DN_STATUS_NONE             = 0x00,   /* No coverage */
    SRV_NW_INFO_SIM_DN_STATUS_COVERAGE         = 0x01,   /* In coverage */
    SRV_NW_INFO_SIM_DN_STATUS_ATTACHED         = 0x02,   /* Attached */
    SRV_NW_INFO_SIM_DN_STATUS_PDP_ACTIVATED    = 0x04,   /* PDP activated */
#ifdef __MMI_NW_INFO_DN_STATUS_ULINK_DLINK_ICONS__
    SRV_NW_INFO_SIM_DN_STATUS_UPLINK           = 0x08,   /* DN uplink */
    SRV_NW_INFO_SIM_DN_STATUS_DOWNLINK         = 0x10,   /* DN downlink */
#endif // __MMI_NW_INFO_DN_STATUS_ULINK_DLINK_ICONS__
    SRV_NW_INFO_SIM_DN_STATUS_END_OF_ENUM
} srv_nw_info_sim_dn_status_enum;
#endif


typedef struct
{
    u16 mcc;
    u16 mnc;
    char apn[30];
    char usrname[30];
    char usrpass[30];
}goome_auto_apn_struct;



extern GM_SIM_ENUM gm_get_active_card(void);
extern s32 GM_CheckSimValid(void);//查询是否插卡
extern u8 GM_GetImsi(char *imsi_data);
extern void GM_GetImei(PsFuncPtr funcPtr); 
extern void GM_GetPlmn(char *plmn); //查询SIM卡MCCMNC
extern s32  GM_GetServiceAvailability(void); //查询网络是否注册
extern s32 GM_GetSignalValue (void);/*以整数表示的百分比*/
extern s32 GM_GetLbsInfo(PsFuncPtr handler_func_ptr);
extern void GM_GetIccid(PsFuncPtr CallBack);//获取ICCID
extern u32 GM_GetPdpStatus(void); // 获取PDP状态
extern u32 GM_GetGsmStatus(void); // 获取G网连接状态 CREG
extern u32 GM_GetCregStatus(u8 *status);
extern s32 GM_GetBarcode(u8* pdata);
extern kal_uint8 GM_GetFakeCellFeature (kal_bool *feature);
extern s32 GM_AutoApnCheck(goome_auto_apn_struct *Apn);

#endif

