
/*****************************************************************************
*  Copyright Statement:
*  --------------------
* www.goome.net
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   gm_power.h 
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
 
#ifndef __GM_POWER_H__
#define __GM_POWER_H__


typedef enum {
    LED_LIGHT_LEVEL0   =0  ,  /* turn off */
    LED_LIGHT_LEVEL1       ,
    LED_LIGHT_LEVEL2       ,
    LED_LIGHT_LEVEL3       ,
    LED_LIGHT_LEVEL4       ,
    LED_LIGHT_LEVEL5       ,
    LED_LIGHT_LEVEL_MAX     
}gpio_device_led_level_typedef;


/* Enum of charger current List */
typedef enum
{
    PMU_CHARGE_CURRENT_0_00_MA = 0,
    PMU_CHARGE_CURRENT_50_00_MA = 5000,
    PMU_CHARGE_CURRENT_62_50_MA = 6250,
    PMU_CHARGE_CURRENT_70_00_MA = 7000,
    PMU_CHARGE_CURRENT_75_00_MA = 7500,
    PMU_CHARGE_CURRENT_87_50_MA = 8750,	
    PMU_CHARGE_CURRENT_99_00_MA = 9900,
    PMU_CHARGE_CURRENT_100_00_MA = 10000,
    PMU_CHARGE_CURRENT_133_00_MA = 13300,
    PMU_CHARGE_CURRENT_150_00_MA = 15000,
    PMU_CHARGE_CURRENT_200_00_MA = 20000,
    PMU_CHARGE_CURRENT_225_00_MA = 22500,	
    PMU_CHARGE_CURRENT_250_00_MA = 25000,
    PMU_CHARGE_CURRENT_300_00_MA = 30000,
    PMU_CHARGE_CURRENT_350_00_MA = 35000,
    PMU_CHARGE_CURRENT_366_00_MA = 36600,
    PMU_CHARGE_CURRENT_400_00_MA = 40000,
    PMU_CHARGE_CURRENT_425_00_MA = 42500,	
    PMU_CHARGE_CURRENT_433_00_MA = 43300,
    PMU_CHARGE_CURRENT_450_00_MA = 45000,
    PMU_CHARGE_CURRENT_466_00_MA = 46600,
    PMU_CHARGE_CURRENT_500_00_MA = 50000,
    PMU_CHARGE_CURRENT_533_00_MA = 53300,
    PMU_CHARGE_CURRENT_550_00_MA = 55000,
    PMU_CHARGE_CURRENT_600_00_MA = 60000,
    PMU_CHARGE_CURRENT_650_00_MA = 65000,
    PMU_CHARGE_CURRENT_666_00_MA = 66600,
    PMU_CHARGE_CURRENT_700_00_MA = 70000,
    PMU_CHARGE_CURRENT_733_00_MA = 73300,
    PMU_CHARGE_CURRENT_750_00_MA = 75000,
    PMU_CHARGE_CURRENT_800_00_MA = 80000,
    PMU_CHARGE_CURRENT_850_00_MA = 85000,
    PMU_CHARGE_CURRENT_866_00_MA = 86600,
    PMU_CHARGE_CURRENT_900_00_MA = 90000,
    PMU_CHARGE_CURRENT_933_00_MA = 93300,
    PMU_CHARGE_CURRENT_950_00_MA = 95000,
    PMU_CHARGE_CURRENT_1000_00_MA = 100000,
    PMU_CHARGE_CURRENT_1066_00_MA = 106600,
    PMU_CHARGE_CURRENT_1100_00_MA = 110000,
    PMU_CHARGE_CURRENT_1200_00_MA = 120000,
    PMU_CHARGE_CURRENT_1300_00_MA = 130000,
    PMU_CHARGE_CURRENT_1400_00_MA = 140000,
    PMU_CHARGE_CURRENT_1500_00_MA = 150000,
    PMU_CHARGE_CURRENT_1600_00_MA = 160000,
    PMU_CHARGE_CURRENT_1800_00_MA = 180000,
    PMU_CHARGE_CURRENT_2000_00_MA = 200000,
    PMU_CHARGE_CURRENT_MAX
}PMU_CHR_CURRENT_ENUM;



typedef enum {
	VBAT_UEM_CHR_OUT=0,
	VBAT_UEM_CHR_OUT_FIRST=1,
	VBAT_UEM_CHR_IN=2,
	VBAT_UEM_CHR_IN_FISRT=3
}VBAT_UEM_CHR_ENUM;




typedef enum {
   CHARGER_PLUG_IN = 0,
   CHARGER_PLUG_OUT = 1,
   CHARGER_UNINIT = 0xff
}CHARGER_IN_OUT_STATUS;


/* Enum of Charger type */
typedef enum
{
   PW_AC_CHR=0,			/* AC CHARGER */
   PW_USB_CHR,			/* USB CHARGER */
   PW_AC_NON_STD_CHR, /* NON-STANDARD AC CHARGER */
   PW_USB_CHARGING_HOST_CHR,  /* USB CHARGING HOST */
   PW_NO_CHR    // Indicate NO charger
}CHR_DET_TYPE_ENUM;


typedef enum{
    PWRKEYPWRON = 0, /* power on reason is powerkey */
    CHRPWRON	= 1,      /* power on reason is charger in */
    RTCPWRON = 2,	    /* power on reason is rtc alarm */
    CHRPWROFF = 3,     /* reserved */
    WDTRESET = 4, 	   /* reserved */
    ABNRESET = 5,       /* power on reason is abnormal reboot */
    USBPWRON = 6,     /* power on reason is usb cable in */
    USBPWRON_WDT = 7, /* power on reason is usb cable in and abnormal reboot*/
    PRECHRPWRON = 8,   /* power on reason is precharger power on*/
    UNKNOWN_PWRON = 0xF9	/* power on reason is unknown*/
}PW_CTRL_POWER_ON_REASON;




typedef struct
{
    kal_int32 VBAT;
    kal_int32 ICHARGE;
    kal_int32 BATTMP;
    kal_int32 VCHARGER;
    kal_int32 ISense_Offset;
    kal_uint8 bat_state;
    kal_uint8 pmictrl_state;
    kal_uint8 BatType;
    kal_uint8 highfull;
    VBAT_UEM_CHR_ENUM VBAT_UEM;   // state of UEM VBAT measurement 
    CHARGER_IN_OUT_STATUS chr_plug;  //充电插入
    CHR_DET_TYPE_ENUM chr_type;
    kal_bool is_chr_valid;
    PW_CTRL_POWER_ON_REASON power_on_type;
    kal_uint8 reserved;
}gm_chr_status_struct;



extern s32 GM_SystemReboot(void);
extern s32  GM_SystemPowerOff(void);
extern s32 GM_SleepEnable(void);
extern s32 GM_SleepDisable(void);
extern s32 GM_IsinkBacklightCtrl(u8 status, kal_uint8 gpio_dev_level);
extern s32 GM_KpledLevelCtrl(u8 status, kal_uint8 gpio_dev_level);
extern s32 GM_GetChrStatus(kal_uint8* chr_status);
extern s32 GM_ChrCurrentConfig(kal_int32 chr_current);
extern s32 GM_ChrCurrentGet(void);
/*****************************************************************
* Function:     GM_PowerKeyRegister 
* 
* Description:
*               Register the callback for PWRKEY indication.
*               the callback function will be triggered when the power
*               KEY pressed down or release.
*
* Parameters:
*               callback_pwrKey:
*                   callback function for PWRKEY indication.
*
* Return:        
*               GM_RET_OK indicates this function successes.
*               GM_RET_ERR_INVALID_PARAMETER, indicates the wrong input parameter.
*****************************************************************/
extern s32 GM_PowerKeyRegister(PsFuncPtr callback_pwrKey);


#endif  /*__GM_POWER_H__*/
