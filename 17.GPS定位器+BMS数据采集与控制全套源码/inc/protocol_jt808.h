/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        protocol_jt808.h
 * Author:           梁震     
 * Version:          1.0
 * Date:             2019-03-01
 * Description:      
 * Others:      
 * Function List:    
    1. 创建protocol_jt808模块
    2. 销毁protocol_jt808模块
    3. protocol_jt808模块定时处理入口
 * History: 
    1. Date:         2019-03-01
       Author:       梁震
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#ifndef __PROTOCOL_JT808_H__
#define __PROTOCOL_JT808_H__

#include "gm_type.h"
#include "error_code.h"

#include "gps_service.h"

/*plen 是输入输出参数*/
void protocol_jt_pack_regist_msg(u8 *pdata, u16 *idx, u16 len);
void protocol_jt_pack_auth_msg(u8 *pdata, u16 *idx, u16 len);
void protocol_jt_pack_iccid_msg(u8 *pdata, u16 *idx, u16 len);


void protocol_jt_pack_logout_msg(u8 *pdata, u16 *idx, u16 len);
void protocol_jt_pack_heartbeat_msg(u8 *pdata, u16 *idx, u16 len);

struct GPSData;
void protocol_jt_pack_gps_msg(GpsDataModeEnum mode, const GPSData *gps, u8 *pdata, u16 *idx, u16 len);
void protocol_jt_pack_lbs_msg(u8 *pdata, u16 *idx, u16 len);
void protocol_jt_pack_gps_msg2(u8 *pdata, u16 *idx, u16 len);
void protocol_jt_pack_remote_ack(u8 *pdata, u16 *idx, u16 len, u8 *pRet, u16 retlen);
void protocol_jt_pack_general_ack(u8 *pdata, u16 *idx, u16 len);
void protocol_jt_pack_param_ack(u8 *pdata, u16 *idx, u16 len);



void protocol_jt_parse_msg(u8 *pdata, u16 len);


#endif




