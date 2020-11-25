/*****************************************************************************
*  Copyright Statement:
*  --------------------
* www.goome.net
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   gm_system.h 
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
 
#ifndef __GM_SYSTEM_H__
#define __GM_SYSTEM_H__

#include "gm_type.h"
#include "gm_gprs.h"


#define PROFILE_NUM              16
#define ARFCN_SECTION_NUM        12

#define NVRAM_EF_BAND_INFO_SIZE  6
#define NVRAM_EF_MM_EQPLMN_LOCIGPRS_SIZE 52


typedef enum 
{
    NVRAM_EF_IMEI_IMEISV_LID = 1,
    NVRAM_EF_L1_RAMPTABLE_GSM850_LID,
    NVRAM_EF_L1_RAMPTABLE_GSM900_LID,
    NVRAM_EF_L1_RAMPTABLE_DCS1800_LID,
    NVRAM_EF_L1_RAMPTABLE_PCS1900_LID,
    NVRAM_EF_PORT_SETTING_LID,
    NVRAM_EF_GM_CUSTOMER_PARAM_LID,
    NVRAM_EF_RAC_PREFERENCE_LID,   //149
    NVRAM_EF_CLASSMARK_RACAP_LID,
    NVRAM_EF_BAND_INFO_LID,
    NVRAM_EF_MM_EQPLMN_LOCIGPRS_LID,
    
    NVRAM_MAX
}goome_nvram_lib_enum;



typedef  struct
{
   unsigned char  point[2][16];

} sRAMPAREADATA;

typedef  struct
{
   signed short   max_arfcn;
   unsigned short mid_level;
   unsigned short hi_weight;
   unsigned short low_weight;

} sARFCN_SECTION;



typedef  struct
{
   signed   long  lowest_power;
   unsigned short power[16];
   sRAMPAREADATA  ramp[ PROFILE_NUM ];
   sARFCN_SECTION arfcn_weight[ ARFCN_SECTION_NUM ];
   unsigned short battery_compensate[3/*volt*/][3/*temp*/];
} sRAMPDATA;



typedef struct
{
   sRAMPDATA          rampData;
}l1cal_rampTable_T;



enum
{
    APP_GET_FUNC_NONE = 0,

    APP_GET_FUNC_MAX
};

enum
{
    APP_SET_FUNC_NONE=0,

    APP_SET_FUNC_MAX
};



typedef struct
{
    u16 type;
    u8 len;
    u8 data[32];
    PsFuncPtr callback;
}app_set_get_type;


/*各类型大小，固定值
    kal_uint8 200
    kal_uint16 100
    kal_uint32 50
    
    由于恢复出厂设置, 会将所有的配置文件删除, 所以需要
    把设备型号信息放到nvram中
*/
typedef struct
{
    kal_uint8  isAgps;
    kal_uint8  isServerLock;
    kal_uint8  isDoubleSen;
    kal_uint8  isBattUpload;
    kal_uint8  is90VEnable;
    kal_uint8  isRelayEnable;
    kal_uint8  isAccEnable;
    kal_uint8  isGpsDisable;
    kal_uint8  isUart2_9600;
    kal_uint8  data_u[191];

    //目前只有这个字段有用.  
    kal_uint16 DeviceType;  
    
    kal_uint16 data_u2[99];
    
    kal_uint32 data_u4[50];
} nvram_ef_goome_param_struct, *p_nvram_ef_goome_param_struct;



typedef struct
{
    kal_uint8 band;
    kal_uint8 arg1;
    kal_uint8 rat_mode;
    kal_uint8 preferred_rat;
    kal_uint8 umts_prefered_band[2];
    kal_uint8 preference;
} nvram_ef_rac_preference_struct;

typedef struct
{
    kal_uint8 byte1;
    kal_uint8 byte2;
    kal_uint8 byte3;
    kal_uint8 byte4;
    kal_uint16 byte5_byte6;
    kal_uint8 byte7;
    kal_uint8 byte8;
    kal_uint8 byte9;
    kal_uint8 byte10;
    kal_uint8 byte11;
    kal_uint8 byte12;
    kal_uint8 byte13;
    kal_uint8 byte14;
    kal_uint8 byte15;
    kal_uint8 byte16;
} nvram_ef_classmark_racap_struct;


typedef struct
{
    kal_uint8 raw_data[NVRAM_EF_BAND_INFO_SIZE];
} nvram_ef_band_info_struct;


typedef struct
{
    kal_uint8 raw_data[NVRAM_EF_MM_EQPLMN_LOCIGPRS_SIZE];
} nvram_ef_mm_eqplmn_locigprs_struct;



extern kal_char* release_verno(void);
extern char* GM_ReleaseVerno(u8* p);
extern char* GM_ReleaseHwVer(void);
extern char* GM_ReleaseBranch(void);
extern char* GM_ReleaseBuild(void);
extern char* GM_BuildDateTime(void);
extern U16 GM_Ucs2ToAsc(char *pOutBuffer, char *pInBuffer);
extern U16 GM_AscToUcs2(char *pOutBuffer, char *pInBuffer);
extern  void GM_SysUsdelay(kal_uint32 delay); //uint:us
extern  void GM_SysMsdelay(kal_uint32 delay); //uint:ms
extern u32 GM_ImageZiLimit(void);
extern u32 GM_ImageDummyBase(void);
extern s32  GM_IsGprsAutoOn(void);
extern s32  GM_GetTcpStatus(u8* p);
extern s32  GM_GetLibStatus(u8* p); 
extern s32  GM_SetUartFunc(u8* p);
extern s32  GM_ReadWriteNvram(U8 is_read, U16 nLID, U16 nRecordId, void *pBuffer, U16 nBufferSize, S32 *result);
extern s32  GM_GetNewAttachStatus(U32 *nw_attach_status);
extern s32  GM_IsWdtPwrOn(char *isWdtPwrOn);
extern s32 GM_DnsParserCallback(PsFuncPtr cb_fun);
extern s32  GM_app_get_func(app_set_get_type* p, u8* out);
extern s32  GM_app_set_func(app_set_get_type* p, u8* out);
extern s32  GM_ReadNvramLid(u16 lib_index);
extern s32 GM_GetChipRID(u8 *pUid);

#endif  // End-of __GM_SYSTEM_H__

