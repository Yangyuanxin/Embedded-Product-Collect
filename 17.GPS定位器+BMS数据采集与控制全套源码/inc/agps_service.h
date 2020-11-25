/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        agps_service.h
 * Author:           梁震       
 * Version:          1.0
 * Date:             2019-03-01
 * Description:      
 * Others:      
 * Function List:    
    1. 创建agps_service模块
    2. 销毁agps_service模块
    3. agps_service模块定时处理入口
 * History: 
    1. Date:         2019-03-01
       Author:       梁震
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#ifndef __agps_service_H__
#define __agps_service_H__

#include "gm_type.h"
#include "error_code.h"
#include "socket.h"


typedef enum
{
    AGPS_REQ_TIMING = 0xDA,  // 校时
    AGPS_ACK_TIMING = 0xDB,  // 校时响应
    AGPS_REQ_EPO = 0xDC, // 请求epo数据
    AGPS_EPO_ACK = 0xDD,
    AGPS_REQ_TAIDOU = 0xDE, // 请求泰斗A-GNSS文件数据
    AGPS_TAIDOU_ACK = 0xDF,
    AGPS_REQ_ZKW = 0xEA, // 请求中科微A-GNSS文件数据  //AT6558
    AGPS_ZKW_ACK = 0xEB,
}ProtocolAGPSEnum;

/**
 * Function:   创建agps_service模块
 * Description:创建agps_service模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   使用前必须调用,否则调用其它接口返回失败错误码
 */
GM_ERRCODE agps_service_create(bool first_create);

/**
 * Function:   销毁agps_service模块
 * Description:销毁agps_service模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE agps_service_destroy(void);

/**
 * Function:   agps_service模块定时处理入口
 * Description:agps_service模块定时处理入口
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   1秒钟调用1次
 */
GM_ERRCODE agps_service_timer_proc(void);


void agps_service_connection_failed(void);
void agps_service_connection_ok(void);
void agps_service_close_for_reconnect(void);
float agps_service_get_unfix_lng(void);
float agps_service_get_unfix_lat(void);

typedef enum
{
    AGPS_TO_GPS_LNGLAT_TIME,
    AGPS_TO_GPS_DATA,
}AgpsToGpsEnum;


/**
 * Function:   gps模块请求写参考数据
 * Description: gps准备好后,请求agps模块写数据, 如果此时agps模块无数据,将等到准备好后再写,
 * Input:	   type:数据类型;re_download:是否重新下载数据
 * Output:	   无
 * Return:	   true——有数据没写完；false——已经写完最后一片数据
 * Others:	   无
 */
bool agps_service_require_to_gps(AgpsToGpsEnum type,bool re_download);

void agps_service_delele_file(void);

#endif


