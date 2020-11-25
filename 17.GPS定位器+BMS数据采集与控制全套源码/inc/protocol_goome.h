/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        protocol_goome.h
 * Author:           梁震     
 * Version:          1.0
 * Date:             2019-03-01
 * Description:      
 * Others:      
 * Function List:    
    1. 创建protocol_goome模块
    2. 销毁protocol_goome模块
    3. protocol_goome模块定时处理入口
 * History: 
    1. Date:         2019-03-01
       Author:       梁震
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#ifndef __PROTOCOL_GOOME_H__
#define __PROTOCOL_GOOME_H__

#include "gm_type.h"
#include "error_code.h"
#include "socket.h"

#include "gps.h"
#include "gps_service.h"


/*idx 是输入输出参数*/
void protocol_goome_pack_transprent_msg(u8* pdata, u16 *idx, u16 len, char* trans_msg, u16 trans_len);
void protocol_goome_pack_login_msg(u8 *pdata, u16 *idx, u16 len);
void protocol_goome_pack_iccid_msg(u8 *pdata, u16 *idx, u16 len);
void protocol_goome_pack_heartbeat_msg(u8 *pdata, u16 *idx, u16 len);
void protocol_goome_pack_remote_ack(u8 *pdata, u16 *idx, u16 len, u8 *pRet, u16 retlen);

struct GPSData;
void protocol_goome_pack_gps_msg(GpsDataModeEnum mode, const GPSData *gps, u8 *pdata, u16 *idx, u16 len);
void protocol_goome_pack_lbs_msg(u8 *pdata, u16 *idx, u16 len);
void protocol_goome_pack_alarm_msg(AlarmInfo *alarm,u8 *pdata, u16 *idx, u16 len);
void protocol_goome_pack_position_request_msg(u8 *mobile_num, u8 num_len, u8 *command, u8 cmd_len,u8 *pdata, u16 *idx, u16 len);



void protocol_goome_parse_msg( u8 *pdata, u16 len);

void protocol_goome_alarm_ask(u8 *pdata, u16 len);
void protocol_goome_sms_ask(u8 *pdata, u16 len);


#endif




