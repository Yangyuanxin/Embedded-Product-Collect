/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        gsm.h
 * Author:           李耀轩       
 * Version:          1.0
 * Date:             2019-03-01
 * Description:      
 * Others:      
 * Function List:    
    1. 创建gsm模块
    2. 销毁gsm模块
    3. gsm模块定时处理入口
 * History: 
    1. Date:         2019-03-01
       Author:       李耀轩
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#ifndef __GSM_H__
#define __GSM_H__

#include "gm_type.h"
#include "error_code.h"
#include "gm_network.h"

#define GM_IMEI_LEN 15
#define GM_IMSI_LEN 15
#define GM_IMEI_HEX_LEN 10
#define GM_ICCID_LEN 20


typedef enum 
{
    FAKE_CELL_NORMAL = 0,
    FAKE_CELL_VIEW = 1,
    FAKE_CELL_ALARM = 2,
}GsmFakeCellState;


typedef enum 
{
    GSM_CREG_UNREGISTER = 0,           //未注册,终端当前并未搜寻新的运营商
    GSM_CREG_REGISTER_LOCAL = 1,       //已注册本地网络
    GSM_CREG_REGISTERING = 2,          //未注册,终端正在搜寻基站
    GSM_CREG_REGISTER_UNKNOW = 4,      //未知代码(SIM卡无效)
    GSM_CREG_REGISTER_ROAM = 5,        //已注册,处于漫游状态
}GsmRegState;

/**
 * Function:   创建gsm模块
 * Description:创建gsm模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   使用前必须调用,否则调用其它接口返回失败错误码
 */
GM_ERRCODE gsm_create(void);

/**
 * Function:   销毁gsm模块
 * Description:销毁gsm模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE gsm_destroy(void);

/**
 * Function:   获取SIM卡ICCID值
 * Description:获取SIM卡ICCID值
 * Input:	   无
 * Output:	   SIM卡ICCID,以'\0'结束
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:
 */
GM_ERRCODE gsm_get_iccid(U8* p_iccid);


/**
 * Function:   获取SIM卡IMSI值
 * Description:获取SIM卡IMSI值
 * Input:	   无
 * Output:	   SIM卡IMSI号,以'\0'结束
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:
 */
GM_ERRCODE gsm_get_imsi(U8* p_imsi);

/**
 * Function:   获取IMEI号
 * Description:获取IMEI号
 * Input:	   无
 * Output:	   IMEI号,以'\0'结束
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:
 */
GM_ERRCODE gsm_get_imei(U8* p_imei);

/**
 * Function:   设置IMEI号
 * Description:设置IMEI号
 * Input:	   无
 * Output:	   IMEI号,以'\0'结束
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:
 */
GM_ERRCODE gsm_set_imei(U8* p_imei);

/**
 * Function:   获取CSQ信号值
 * Description:获取CSQ信号值
 * Input:	   无
 * Output:     无
 * Return:     信号强度[0,31]
 * Others:
 */
U8 gsm_get_csq(void);


/**
 * Function:   获取基站信息
 * Description:获取基站信息
 * Input:	   无
 * Output:	   基站信息
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:
 */
GM_ERRCODE gsm_get_cell_info(gm_cell_info_struct* p_cell_info);

#endif



