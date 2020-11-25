#include <stdio.h>
#include "utility.h"
#include "gm_stdlib.h"
#include "gm_memory.h"
#include "gm_type.h"
#include "gsm.h"
#include "log_service.h"
#include "config_service.h"
#include "hard_ware.h"
#include "protocol.h"
#include "gps.h"
#include "protocol_goome.h"
#include "gps_service.h"
#include "sms.h"
#include "system_state.h"
#include "g_sensor.h"
#include "command.h"
#include "applied_math.h"
#include "agps_service.h"
#include "bms.h"
#include "gm_timer.h"

typedef enum
{
    PROTOCCOL_GOOME_LOGIN = 0x01,  // 登录包
    PROTOCCOL_GOOME_LOCAT = 0x02,  // 位置包
    PROTOCCOL_GOOME_HEART = 0x03,  // 心跳
    PROTOCCOL_GOOME_ALARM = 0x04,  // 报警
    PROTOCCOL_GOOME_STRING_MSG = 0x05, // 采集数据包    谷米暂无
    PROTOCCOL_GOOME_SMS_MSG = 0x06,   // 短信指令上传数据包
    PROTOCCOL_GOOME_HEART_EXPAND = 0x07, // 扩展心跳数据包
    PROTOCCOL_GOOME_TIME_CALIB = 0x08,  // 时间校准包    谷米暂无
    PROTOCCOL_GOOME_ICCID_MSG = 0x09,
    PROTOCCOL_GOOME_REMOTE_MSG = 0x80,
    PROTOCCOL_GOOME_GENERAL_MSG = 0x81, // 普通消息下发数据包 
    PROTOCCOL_GOOME_FULL_LBS = 0x91, // 多基站数据
    PROTOCCOL_GOOME_STATUS_MSG = 0x94, //信息传输通用包
    PROTOCCOL_GOOME_TRANSPARENT = 0xFE, //透传数据包
}ProtocolGoomeCmdEnum;;


/*用于记录发送消息的序列号, 收到消息的序列号等*/
typedef struct
{
    u16 msg_serial;  //上传数据序列号, 每次发消息加一
    u8 msg_flag;     // 信息标识, 服务器传过来, 原样传回
    u8 server_id[4]; // 服务器标志  , 服务器传过来, 原样传回
}GoomeMsgSave;

static GoomeMsgSave s_msg_save = {0, };

static void protocol_goome_pack_head(U8* pdata, u16 *idx, u16 len);
static void protocol_goome_pack_imei(U8* pdata, u16 *idx, u16 len);
static void protocol_goome_pack_language(U8* pdata, u16 *idx, u16 len);
static void protocol_goome_pack_id_len(U8* pdata, u8 id, u16 len);
static void protocol_goome_pack_iccid(U8* pdata, u16 *idx, u16 len);
static void protocol_goome_pack_lbs(U8* pdata, u16 *idx, u16 len);
static void protocol_goome_pack_lbses(U8* pdata, u16 *idx, u16 len);
static void protocol_goome_pack_locate_status(U8* pdata, u16 *idx, u16 len);
static void protocol_goome_pack_alarm(AlarmInfo *alarm,U8* pdata, u16 *idx, u16 len);
static void protocol_goome_pack_csq(U8* pdata, u16 *idx, u16 len);
static void protocol_goome_pack_power_voltage(U8* pdata, u16 *idx, u16 len);
static void protocol_goome_pack_gps_signal_quality(U8* pdata, u16 *idx, u16 len);
static void protocol_goome_pack_transprent(u8* pdata, u16 *idx, u16 len, char* trans_msg, u16 trans_len);
static void protocol_goome_parse_login_response(U8* pdata, u16 len);
static void protocol_goome_parse_remote_msg(U8* pdata, u16 len);


static void protocol_goome_pack_iccid(U8* pdata, u16 *idx, u16 len)
{
    GM_ERRCODE ret = GM_SUCCESS;
    u8 iccid[GM_ICCID_LEN + 1] = {0};
    if(GM_SUCCESS != (ret = gsm_get_iccid(iccid)))
    {
        LOG(INFO,"clock(%d) protocol_goome_pack_iccid can not get iccid, ret:%d.", util_clock(), ret);
    }
    
    if (0 == GM_strlen((const char *)iccid))
    {
        GM_memset(iccid, 0, sizeof(iccid));
    }

    if((*idx) + 10 > len)
    {
        LOG(WARN,"clock(%d) protocol_goome_pack_iccid assert(len(%d)) failed.", util_clock(), len);
        return;
    }
    
    pdata[(*idx)++] = MERGEBCD(util_chr(iccid[0]), util_chr(iccid[1]));
    pdata[(*idx)++] = MERGEBCD(util_chr(iccid[2]), util_chr(iccid[3]));
    pdata[(*idx)++] = MERGEBCD(util_chr(iccid[4]), util_chr(iccid[5]));
    pdata[(*idx)++] = MERGEBCD(util_chr(iccid[6]), util_chr(iccid[7]));
    pdata[(*idx)++] = MERGEBCD(util_chr(iccid[8]), util_chr(iccid[9]));
    pdata[(*idx)++] = MERGEBCD(util_chr(iccid[10]), util_chr(iccid[11]));
    pdata[(*idx)++] = MERGEBCD(util_chr(iccid[12]), util_chr(iccid[13]));
    pdata[(*idx)++] = MERGEBCD(util_chr(iccid[14]), util_chr(iccid[15]));
    pdata[(*idx)++] = MERGEBCD(util_chr(iccid[16]), util_chr(iccid[17]));
    pdata[(*idx)++] = MERGEBCD(util_chr(iccid[18]), util_chr(iccid[19]));

}


static void protocol_goome_pack_imei(U8* pdata, u16 *idx, u16 len)
{
    GM_ERRCODE ret = GM_SUCCESS;
    u8 imei[GM_IMEI_LEN + 1] = {0};
    if(GM_SUCCESS != (ret = gsm_get_imei(imei)))
    {
        LOG(INFO,"clock(%d) protocol_goome_pack_imei can not get imei, ret:%d.", util_clock(), ret);
    }
    
    if (0 == GM_strlen((const char *)imei))
    {
        GM_memset(imei, 0, sizeof(imei));
    }

    if((*idx) + 8 > len)
    {
        LOG(WARN,"clock(%d) protocol_goome_pack_imei assert(len(%d)) failed.", util_clock(), len);
        return;
    }
    
    pdata[(*idx)++] = util_chr(imei[0]);
    pdata[(*idx)++] = MERGEBCD(util_chr(imei[1]), util_chr(imei[2]));
    pdata[(*idx)++] = MERGEBCD(util_chr(imei[3]), util_chr(imei[4]));
    pdata[(*idx)++] = MERGEBCD(util_chr(imei[5]), util_chr(imei[6]));
    pdata[(*idx)++] = MERGEBCD(util_chr(imei[7]), util_chr(imei[8]));
    pdata[(*idx)++] = MERGEBCD(util_chr(imei[9]), util_chr(imei[10]));
    pdata[(*idx)++] = MERGEBCD(util_chr(imei[11]), util_chr(imei[12]));
    pdata[(*idx)++] = MERGEBCD(util_chr(imei[13]), util_chr(imei[14]));

}


static void protocol_goome_pack_head(U8* pdata, u16 *idx, u16 len)
{
    if((*idx) + 7 > len)
    {
        LOG(WARN,"clock(%d) protocol_goome_pack_head assert(len(%d)) failed.", util_clock(), len);
        return;
    }
    
    pdata[(*idx)++] = PROTOCOL_HEADER_GOOME;
    pdata[(*idx)++] = PROTOCOL_HEADER_GOOME;
    (*idx) = (*idx) + 3; // id 1 , len 2

    ++(s_msg_save.msg_serial);
    
    pdata[(*idx)++] = BHIGH_BYTE(s_msg_save.msg_serial);
    pdata[(*idx)++] = BLOW_BYTE(s_msg_save.msg_serial);
    
}

static void protocol_goome_pack_language(U8* pdata, u16 *idx, u16 len)
{
    u16 language = 0;

    if((*idx) + 1 > len)
    {
        LOG(WARN,"clock(%d) protocol_goome_pack_language assert(len(%d)) failed.", util_clock(), len);
        return;
    }

    config_service_get(CFG_LANGUAGE, TYPE_SHORT, &language, sizeof(language));
    pdata[(*idx)++] = (1 == language) ? 0x00 : 0x01;  // 00简体中文  01英文
}

static void protocol_goome_pack_id_len(U8* pdata, u8 id, u16 len)
{
    pdata[2] = id;
    len -= 5;
    pdata[3] = BHIGH_BYTE(len);
    pdata[4] = BLOW_BYTE(len);
}

static void protocol_goome_pack_power_voltage(U8* pdata, u16 *idx, u16 len)
{
    U8 percent = 0.0;
    hard_ware_get_internal_battery_percent(&percent);
    pdata[(*idx)++] = percent;
}

static void protocol_goome_pack_csq(U8* pdata, u16 *idx, u16 len)
{
    u8 csq;
    csq = gsm_get_csq();
    
    if (csq < 5)
    {
        pdata[(*idx)++] = 0;    // 0x00: 无信号
    }
    else if (csq <= 10)
    {
        pdata[(*idx)++] = 1;    // 0x01：信号极弱
    }
    else if (csq <= 16)
    {
        pdata[(*idx)++] = 2;    // 0x02：信号较弱
    }
    else if (csq <= 25)
    {
        pdata[(*idx)++] = 3;    // 0x03：信号良好
    }
    else if (csq <= 31)
    {
        pdata[(*idx)++] = 4;    // 0x04：信号强
    }
    else if (csq > 31)
    {
        pdata[(*idx)++] = 0;    // 0x00: 无信号
    }
    else
    {
        pdata[(*idx)++] = 0;    // 0x00: 无信号
    }
}

static void protocol_goome_pack_extern_power(U8* pdata, u16 *idx, u16 len)
{
    U8 percent = 0.0;
    float voltage = 0.0;
    u8 value_u8;
    u16 value_u16;
    
    config_service_get(CFG_BATTUPLOAD_DISABLE, TYPE_BYTE, &value_u8, sizeof(value_u8));
    if(0 == value_u8)
    {
        hard_ware_get_extern_battery_percent(&percent);
        pdata[(*idx)++] = (u8)percent;

        hard_ware_get_power_voltage(&voltage);
        value_u16 = (u16)voltage*100;
        pdata[(*idx)++] = BHIGH_BYTE(value_u16);
        pdata[(*idx)++] = BLOW_BYTE(value_u16);
    }
    else
    {
        pdata[(*idx)++] = 0x00;
        pdata[(*idx)++] = 0x00;
        pdata[(*idx)++] = 0x00;
    }
}

static void protocol_goome_pack_gps_signal_quality(U8* pdata, u16 *idx, u16 len)
{
    GPSData gps;
    GM_memset(&gps,0,sizeof(gps));
    gps_get_last_data(&gps);
    pdata[(*idx)++] = gps.signal_intensity_grade;
}


//6767 07 000B 0003 02EE 043700000F0000
static void protocol_goome_pack_status(U8* pdata, u16 *idx, u16 len)
{
    u16 data = 0;
    u8 value_u8;
	bool acc_on;
    ConfigHearUploadEnum heart_mode;
    
    if((*idx) + 2 > len)
    {
        LOG(WARN,"clock(%d) protocol_goome_pack_status assert(len(%d)) failed.", util_clock(), len);
        return;
    }

    if (gps_is_fixed())
    {
        SET_BIT0(data);
    }

    if (GM_SUCCESS == hard_ware_get_acc_level(&acc_on))
    {
        if(acc_on)
        {
            SET_BIT1(data);
            SET_BIT2(data);
        }
        else
        {
            SET_BIT1(data);
        }
    }
    
    //config_service_get(CFG_IS_MANUAL_DEFENCE, TYPE_BOOL, &value_u8, sizeof(value_u8));
    if(system_state_get_defence())
    {
        SET_BIT3(data);
        SET_BIT4(data);
    }
    else
    {
        SET_BIT3(data);
    }

    config_service_get(CFG_IS_RELAY_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));
    if(value_u8)
    {
        if (system_state_get_device_relay_state())
        {
            //油电断开
            SET_BIT5(data);
        }
        else
        {
            SET_BIT5(data);
            SET_BIT6(data);
        }
    }
    //else 无断油电功能

    if(system_state_get_has_started_charge())
    {
        SET_BIT7(data);
        SET_BIT8(data);
    }
    else
    {
        SET_BIT7(data);
    }

    switch( system_state_get_vehicle_state())
    {
        case VEHICLE_STATE_RUN:
            SET_BIT9(data);
            SET_BIT10(data);
            break;
        case VEHICLE_STATE_STATIC:
            SET_BIT9(data);
            break;
        default:
            SET_BIT10(data);
            break;
    }

    pdata[(*idx)++] = BHIGH_BYTE(data);
    pdata[(*idx)++] = BLOW_BYTE(data);


    heart_mode = config_service_get_heartbeat_protocol();
    if (HEART_EXPAND == heart_mode)
    {	
        protocol_goome_pack_csq(pdata, idx, len); //1byte
        protocol_goome_pack_power_voltage(pdata, idx, len); //1byte
        protocol_goome_pack_extern_power(pdata, idx, len);  //3bytes
        pdata[(*idx)++] = g_sensor_get_angle();
		protocol_goome_pack_gps_signal_quality(pdata, idx, len);

		//上电时间大于配置时间则不显示
        config_service_get(CFG_SEN_ANGLE_SHOW_TIME, TYPE_BYTE, &value_u8, sizeof(value_u8));
        if(system_state_get_start_time() > value_u8*SECONDS_PER_MIN || 0xFF == value_u8)
        {
			//APP不显示姿态，注意0是显示，1是不显示
			pdata[(*idx)++] = 1;
        }
        else
        {

		    //APP显示姿态，注意0是显示，1是不显示
            pdata[(*idx)++] = 0;
        }
    }
}


static void protocol_goome_pack_gps(const GPSData *gps, U8* pdata, u16 *idx, u16 len)
{
    u32 curtime = 0;
    u32 latitudev = 0;
    u32 longitudev = 0;
    u16 gps_speed = 0;
    u16 gps_angle = 0;

    if((*idx) + 15 > len)
    {
        LOG(WARN,"clock(%d) protocol_goome_pack_gps assert(len(%d)) failed.", util_clock(), len);
        return;
    }

    if (!gps_is_fixed())
    {
        curtime = util_get_utc_time();
    }
    else
    {
        curtime = gps->gps_time;
    }
    
    // 占用4个字节 ,即卫星时间的UTC秒数
    pdata[(*idx)++] = BHIGH_BYTE(WHIGH_WORD(curtime));
    pdata[(*idx)++] = BLOW_BYTE(WHIGH_WORD(curtime));
    pdata[(*idx)++] = BHIGH_BYTE(WLOW_WORD(curtime));
    pdata[(*idx)++] = BLOW_BYTE(WLOW_WORD(curtime));

    // 纬度
    if(gps->gps_time == 0)
    {
        s32 tmpval = agps_service_get_unfix_lat() * 1800000.0f;
        latitudev = (u32)tmpval;
    }
    else
    {
        // latitudev = (u32)(gps->lat * 1800000.0f);  latitudev会得到0
        s32 tmpval = gps->lat * 1800000.0f;
        latitudev = (u32)tmpval;
    }

    pdata[(*idx)++] = BHIGH_BYTE(WHIGH_WORD(latitudev));
    pdata[(*idx)++] = BLOW_BYTE(WHIGH_WORD(latitudev));
    pdata[(*idx)++] = BHIGH_BYTE(WLOW_WORD(latitudev));
    pdata[(*idx)++] = BLOW_BYTE(WLOW_WORD(latitudev));
    
    // 经度
    if(gps->gps_time == 0)
    {
        s32 tmpval = agps_service_get_unfix_lng() * 1800000.0f;
        longitudev = (u32)tmpval;
    }
    else
    {
        s32 tmpval = gps->lng*1800000.0f;
        longitudev = (u32)tmpval;
    }
    
    pdata[(*idx)++] = BHIGH_BYTE(WHIGH_WORD(longitudev));
    pdata[(*idx)++] = BLOW_BYTE(WHIGH_WORD(longitudev));
    pdata[(*idx)++] = BHIGH_BYTE(WLOW_WORD(longitudev));
    pdata[(*idx)++] = BLOW_BYTE(WLOW_WORD(longitudev));
    
    
    // 速度
    gps_speed = gps->speed;
    if (gps_speed > 180)
    {
        gps_speed = 180;
    }
    pdata[(*idx)++] = (u8)gps_speed;

     // 角度
    gps_angle = (u16)gps->course;
    if (gps_angle >= 360)
    {
        gps_angle = 0;
    }
    
    pdata[(*idx)++] = BHIGH_BYTE(gps_angle);
    pdata[(*idx)++] = BLOW_BYTE(gps_angle);
}



static void protocol_goome_pack_lbs(U8* pdata, u16 *idx, u16 len)
{
    gm_cell_info_struct lbs;
    GM_ERRCODE ret;
    
    GM_memset(&lbs,0, sizeof(lbs));
    
    if((*idx) + 9 > len)
    {
        LOG(WARN,"clock(%d) protocol_goome_pack_lbs assert(len(%d)) failed.", util_clock(), len);
        return;
    }

    ret = gsm_get_cell_info(&lbs);
    LOG(DEBUG,"clock(%d) protocol_goome_pack_lbs ret(%d) lbs(%d).", util_clock(), ret, lbs.nbr_cell_num);
    
    pdata[(*idx)++] = BHIGH_BYTE(lbs.serv_info.mcc);  // MCC 2
    pdata[(*idx)++] = BLOW_BYTE(lbs.serv_info.mcc);
    
    pdata[(*idx)++] = BHIGH_BYTE(lbs.serv_info.mnc); // MNC 2
    pdata[(*idx)++] = BLOW_BYTE(lbs.serv_info.mnc); 
    
    pdata[(*idx)++] = BHIGH_BYTE(lbs.serv_info.lac);  // LAC 2
    pdata[(*idx)++] = BLOW_BYTE(lbs.serv_info.lac);
    
    
    pdata[(*idx)++] = 0x00;                            //CI 3
    pdata[(*idx)++] = BHIGH_BYTE(lbs.serv_info.ci);
    pdata[(*idx)++] = BLOW_BYTE(lbs.serv_info.ci);
    
}


static void protocol_goome_pack_transprent(u8* pdata, u16 *idx, u16 len, char* trans_msg, u16 trans_len)
{
	if((*idx) + trans_len > len)
    {
        LOG(WARN,"clock(%d) protocol_goome_pack_transprent assert(len(%d)) failed.", util_clock(), len);
        return;
    }

	GM_memcpy(&pdata[(*idx)], trans_msg, trans_len);
	(*idx) = (*idx) + trans_len;
}



void protocol_goome_pack_transprent_msg(u8* pdata, u16 *idx, u16 len, char* trans_msg, u16 trans_len)
{
    protocol_goome_pack_head(pdata, idx, len);  //7 bytes
    
    protocol_goome_pack_transprent(pdata, idx, len, trans_msg, trans_len); //不定长

    protocol_goome_pack_id_len(pdata, PROTOCCOL_GOOME_TRANSPARENT, *idx);
}



void protocol_goome_pack_login_msg(U8* pdata, u16 *idx, u16 len)
{
    protocol_goome_pack_head(pdata, idx, len);  //7 bytes
    
    protocol_goome_pack_imei(pdata, idx, len);  //8bytes
    
    protocol_goome_pack_language(pdata, idx, len);  //1bytes

    protocol_goome_pack_id_len(pdata, PROTOCCOL_GOOME_LOGIN, *idx);
}



void protocol_goome_pack_heartbeat_msg(U8* pdata, u16 *idx, u16 len)
{
    protocol_goome_pack_head(pdata, idx, len);  //7 bytes
    
    protocol_goome_pack_status(pdata, idx, len);  // 2|10 bytes

    if(HEART_NORMAL == config_service_get_heartbeat_protocol())
    {
        protocol_goome_pack_id_len(pdata, PROTOCCOL_GOOME_HEART, *idx);
    }
    else
    {
        protocol_goome_pack_id_len(pdata, PROTOCCOL_GOOME_HEART_EXPAND, *idx);
    }
}

static void protocol_goome_pack_locate_status(U8* pdata, u16 *idx, u16 len)
{
    bool acc_on = false;
	
    if (gps_is_fixed())
    {
        SET_BIT0(pdata[(*idx)]);
    }
    else
    {
        CLR_BIT0(pdata[(*idx)]);
    }

    if (GM_SUCCESS == hard_ware_get_acc_level(&acc_on) && acc_on)
    {
        SET_BIT1(pdata[(*idx)]);
    }
    else
    {
        CLR_BIT1(pdata[(*idx)]);
    }
    
    (*idx)++;
}


void protocol_goome_pack_gps_msg(GpsDataModeEnum mode, const GPSData *gps, U8* pdata, u16 *idx, u16 len)
{
    protocol_goome_pack_head(pdata, idx, len);  //7 bytes
    protocol_goome_pack_gps(gps, pdata, idx, len);  //15 bytes
    protocol_goome_pack_lbs(pdata, idx, len);  //9 bytes
    protocol_goome_pack_locate_status(pdata, idx, len);  //1 bytes
    
    protocol_goome_pack_id_len(pdata, PROTOCCOL_GOOME_LOCAT, *idx);
}


static void protocol_goome_pack_lbses(U8* pdata, u16 *idx, u16 len)
{
    gm_cell_info_struct lbs;
    u32 cur_time;
    u16 k = 0;
    u16 num=0;
    u16 lbsidx=0;
    GM_ERRCODE ret;
    
    GM_memset(&lbs,0, sizeof(lbs));
    
    if((*idx) + 39 > len)
    {
        LOG(WARN,"clock(%d) protocol_goome_pack_lbses assert(len(%d)) failed.", util_clock(), len);
        return;
    }

    ret = gsm_get_cell_info(&lbs);
    LOG(DEBUG,"clock(%d) protocol_goome_pack_lbses ret(%d) lbs(%d).", util_clock(), ret, lbs.nbr_cell_num);

    //日期时间
    cur_time = util_get_utc_time();
    pdata[(*idx)++] = BHIGH_BYTE(WHIGH_WORD(cur_time));
    pdata[(*idx)++] = BLOW_BYTE(WHIGH_WORD(cur_time));
    pdata[(*idx)++] = BHIGH_BYTE(WLOW_WORD(cur_time));
    pdata[(*idx)++] = BLOW_BYTE(WLOW_WORD(cur_time));
    
    if (lbs.nbr_cell_num > 4)
    {
        num = 4;
    }
    else
    {
        num = lbs.nbr_cell_num;
    }
    
    // TA(1B) + MCC(2B) + MNC(1B) + 基站数(1B)
    pdata[(*idx)++] = 0xFF;
    pdata[(*idx)++] = BHIGH_BYTE(lbs.serv_info.mcc);  // MCC
    pdata[(*idx)++] = BLOW_BYTE(lbs.serv_info.mcc);
    pdata[(*idx)++] = BLOW_BYTE(lbs.serv_info.mnc); // MNC
    pdata[(*idx)++] = num+1;  // 基站数
    
    
    // 主服务基站信息  LAC(2B)+CI(3B)+RSSI(1B)
    pdata[(*idx)++] = BHIGH_BYTE(lbs.serv_info.lac);  // LAC
    pdata[(*idx)++] = BLOW_BYTE(lbs.serv_info.lac);
    pdata[(*idx)++] = 0x00;  // CELL ID
    pdata[(*idx)++] = BHIGH_BYTE(lbs.serv_info.ci);  // CELL ID
    pdata[(*idx)++] = BLOW_BYTE(lbs.serv_info.ci);
    pdata[(*idx)++] = lbs.serv_info.rxlev;// 信号强度      RSSI
    
    
    // 剩余4个基站 LAC(2B)+CI(3B)+RSSI(1B)
    lbsidx = (*idx);
    GM_memset(&pdata[lbsidx], 0, 24);
    for (k=0; k<num; k++) // 24B
    {
        pdata[lbsidx++] = BHIGH_BYTE(lbs.nbr_cell_info[k].lac);  // LAC
        pdata[lbsidx++] = BLOW_BYTE(lbs.nbr_cell_info[k].lac);
        
        pdata[lbsidx++] = 0x00;  // CELL ID
        pdata[lbsidx++] = BHIGH_BYTE(lbs.nbr_cell_info[k].ci);  // CELL ID
        pdata[lbsidx++] = BLOW_BYTE(lbs.nbr_cell_info[k].ci);
        
        pdata[lbsidx++] = lbs.nbr_cell_info[k].rxlev;// 信号强度
    }
    
    (*idx) = (*idx) + 24;   // 4个基站空间 每个6字节
    
}

void protocol_goome_pack_lbs_msg(U8* pdata, u16 *idx, u16 len)
{
    protocol_goome_pack_head(pdata, idx, len);  //7 bytes
    protocol_goome_pack_lbses(pdata, idx, len);  //39 bytes
    protocol_goome_pack_locate_status(pdata, idx, len);  //1 bytes
    
    protocol_goome_pack_id_len(pdata, PROTOCCOL_GOOME_FULL_LBS, *idx);
}

static void protocol_goome_pack_alarm(AlarmInfo *alarm,U8* pdata, u16 *idx, u16 len)
{
    if((*idx) + 3 > len)
    {
        LOG(WARN,"clock(%d) protocol_goome_pack_alarm assert(len(%d)) failed.", util_clock(), len);
        return;
    }
    
    pdata[(*idx)++] = alarm->type;
    pdata[(*idx)++] = BHIGH_BYTE(alarm->info);
    pdata[(*idx)++] = BLOW_BYTE(alarm->info); 
}

void protocol_goome_pack_alarm_msg(AlarmInfo *alarm,U8* pdata, u16 *idx, u16 len)
{
    GPSData gps;
    GM_memset(&gps,0,sizeof(gps));
    
    gps_get_last_data(&gps);
    if(gps.gps_time == 0)
    {
        system_state_get_last_gps(&gps);
    }
    
    protocol_goome_pack_head(pdata, idx, len);  //7 bytes
    protocol_goome_pack_gps(&gps, pdata, idx, len);  //15 bytes
    protocol_goome_pack_lbs(pdata, idx, len);  //9 bytes
    protocol_goome_pack_locate_status(pdata, idx, len);  //1 bytes
    protocol_goome_pack_alarm(alarm, pdata, idx, len);  //3 bytes
    
    protocol_goome_pack_id_len(pdata, PROTOCCOL_GOOME_ALARM, *idx);
    return;
}


static void protocol_goome_parse_login_response(U8* pdata, u16 len)
{
    if(len != 7)
    {
        LOG(WARN,"clock(%d) protocol_goome_parse_login_response assert(len(%d) != 7) failed.", util_clock(), len);
        return;
    }
    
    //发送iccid
    gps_service_after_login_response();
	GM_StartTimer(GM_TIMER_BMS_TRANSPRENT, 1000, bms_transprent_callback);
    
    return;
}



void protocol_goome_parse_msg(U8* pdata, u16 len)
{
    if(PROTOCOL_HEADER_GOOME != pdata[0] || PROTOCOL_HEADER_GOOME != pdata[1])
    {
        LOG(WARN,"clock(%d) protocol_goome_parse_msg assert(head(%02x %02x)) failed.", util_clock(), pdata[0],pdata[1]);
        return;
    }

    //协议号
    switch(pdata[2])
    {
        case PROTOCCOL_GOOME_LOGIN:
            LOG(DEBUG,"clock(%d) protocol_goome_parse_msg(PROTOCCOL_GOOME_LOGIN).", util_clock());
            protocol_goome_parse_login_response(pdata,len);
            break;
        case PROTOCCOL_GOOME_ICCID_MSG:
            LOG(DEBUG,"clock(%d) protocol_goome_parse_msg(PROTOCCOL_GOOME_ICCID_MSG).", util_clock());
            break;
        case PROTOCCOL_GOOME_HEART:
            LOG(DEBUG,"clock(%d) protocol_goome_parse_msg(PROTOCCOL_GOOME_HEART).", util_clock());
            gps_service_after_receive_heartbeat();
            break;
        case PROTOCCOL_GOOME_HEART_EXPAND:
            LOG(DEBUG,"clock(%d) protocol_goome_parse_msg(PROTOCCOL_GOOME_HEART_EXPAND).", util_clock());
            gps_service_after_receive_heartbeat();
            break;
        case PROTOCCOL_GOOME_LOCAT:  //定位也相当于有心跳
            LOG(DEBUG,"clock(%d) protocol_goome_parse_msg(PROTOCCOL_GOOME_LOCAT).", util_clock());
            gps_service_after_receive_heartbeat();
            break;
        case PROTOCCOL_GOOME_ALARM:
            LOG(DEBUG,"clock(%d) protocol_goome_parse_msg(PROTOCCOL_GOOME_ALARM).", util_clock());
            protocol_goome_alarm_ask(pdata, len);
            break;
        case PROTOCCOL_GOOME_SMS_MSG:
            LOG(DEBUG,"clock(%d) protocol_goome_parse_msg(PROTOCCOL_GOOME_SMS_MSG).", util_clock());
            protocol_goome_sms_ask(&pdata[7], len-7);
            break;
        case PROTOCCOL_GOOME_REMOTE_MSG:
            LOG(DEBUG,"clock(%d) protocol_goome_parse_msg(PROTOCCOL_GOOME_REMOTE_MSG).", util_clock());
            protocol_goome_parse_remote_msg(pdata, len);
            break;
        case PROTOCCOL_GOOME_GENERAL_MSG:
            LOG(DEBUG,"clock(%d) protocol_goome_parse_msg(PROTOCCOL_GOOME_GENERAL_MSG).", util_clock());
            break;
        default:
            LOG(WARN,"clock(%d) protocol_goome_parse_msg assert(msgid(%d)) failed.", util_clock(), pdata[2]);
            break;
    }
    return;
}


void protocol_goome_pack_iccid_msg(U8* pdata, u16 *idx, u16 len)
{
    protocol_goome_pack_head(pdata, idx, len);  //7 bytes
    
    protocol_goome_pack_iccid(pdata, idx, len);  //10bytes
    
    protocol_goome_pack_id_len(pdata, PROTOCCOL_GOOME_ICCID_MSG, *idx);
}


void protocol_goome_alarm_ask(U8* pdata, u16 len)
{
    U8* recv_msg = NULL;
    u16 *recv_msg_w = NULL;
    u8 number[24];
    u16 number_len = 0;
    u16 index;
    u16 msg_len = 0;

    if(len <=7)
    {
        LOG(DEBUG,"clock(%d) protocol_goome_alarm_ask msglen(%d).", util_clock(), len);
        return;
    }
    
    recv_msg = GM_MemoryAlloc(CMD_MAX_LEN+1);
    if (recv_msg == NULL)
    {
        LOG(WARN,"clock(%d) protocol_goome_alarm_ask assert(GM_MemoryAlloc(%d)) failed.", util_clock(), CMD_MAX_LEN+1);
        return;
    }
    recv_msg_w = GM_MemoryAlloc(CMD_MAX_LEN/2+1);
    if (recv_msg_w == NULL)
    {
        GM_MemoryFree(recv_msg);
        LOG(WARN,"clock(%d) protocol_goome_alarm_ask assert(GM_MemoryAlloc(%d)) failed.", util_clock(), CMD_MAX_LEN/2+1);
        return;
    }
    
    GM_memset(recv_msg, 0x00, CMD_MAX_LEN+1);
    GM_memset(recv_msg_w, 0x00, CMD_MAX_LEN/2+1);
    
    //把UTF-8转成UNICODE
    index = util_utf8_to_unicode(&pdata[7], len-7, recv_msg_w, CMD_MAX_LEN/2);    
    
    //把双字节的UNICODE转成单字节的UNICODE
    msg_len = util_ucs2_u16_to_u8(recv_msg_w, index,recv_msg );
    
    for (index=0; index<=3; index++)
    {
        number_len = config_service_get_sos_number(index, number, sizeof(number));
        number[number_len] = 0;
        if (number_len > 0)
        {
            sms_send((char*)recv_msg, msg_len, (char*)number, GM_UCS2_DCS);
        }
        
        if (config_service_is_alarm_sms_to_one())
        {
            break;  //只发第一个号
        }
    }
    
    GM_MemoryFree(recv_msg);
    GM_MemoryFree(recv_msg_w);

}


/*
设备接收到123短信,上发到服务器,  收到时间经纬度和短信内容后, 通过短信发给源发送方
676706 0033 0026 57E4DFC7 026DDEC0 0C3BFEF0 000154 01CC0000262C000EBA 01 313036343839393038303031380000000000000000 313233  SMS指令数据

6767
06
0081
6FBE
000000000000000031303634383939313932363435
E4BD8DE7BDAE3AE5B9BFE4B89CE79C812EE6B7B1E59CB3E5B8822EE58D97E5B1B1E58CBA2EE7A791E68A80E58D97E58D81E4BA8CE8B7AF2EE7A6BBE4B8ADE59BBDE993B6E8A18C28E7A791E68A80E58D97E694AFE8A18C2928E8A5BFE58C9729E7BAA6313330E7B1B32E
平台下发位置:
设备把下发内容转给短信发送者
*/
void protocol_goome_sms_ask(U8* pdata, u16 len)
{
    U8* recv_msg = NULL;
    u16 *recv_msg_w = NULL;
    u8 number[24];
    u16 number_len = 0;
    u16 index;

    if(len < 21)
    {
        LOG(DEBUG,"clock(%d) protocol_goome_sms_ask msglen(%d).", util_clock(), len);
        return;
    }

    GM_strncpy((char *)number, (const char *)pdata, 21);
    number[20] = 0;
    number_len = GM_strlen((const char *)number);
    if(number_len == 0)
    {
        LOG(INFO,"clock(%d) protocol_goome_sms_ask number_len(%d).", util_clock(), number_len);
        return;
    }
    
    recv_msg = GM_MemoryAlloc(CMD_MAX_LEN+1);
    if (recv_msg == NULL)
    {
        LOG(WARN,"clock(%d) protocol_goome_sms_ask assert(GM_MemoryAlloc(%d)) failed.", util_clock(), CMD_MAX_LEN+1);
        return;
    }
    recv_msg_w = GM_MemoryAlloc(CMD_MAX_LEN/2+1);
    if (recv_msg_w == NULL)
    {
        GM_MemoryFree(recv_msg);
        LOG(WARN,"clock(%d) protocol_goome_sms_ask assert(GM_MemoryAlloc(%d)) failed.", util_clock(), CMD_MAX_LEN/2+1);
        return;
    }
    
    GM_memset(recv_msg, 0x00, CMD_MAX_LEN+1);
    GM_memset(recv_msg_w, 0x00, CMD_MAX_LEN/2+1);
    
    //把UTF-8转成UNICODE
    index = util_utf8_to_unicode(&pdata[21], len-21, recv_msg_w, CMD_MAX_LEN/2);    
    
    //把双字节的UNICODE 字节序转换
    index = util_ucs2_u16_to_u8(recv_msg_w, index, recv_msg);

    LOG(DEBUG,"clock(%d) protocol_goome_sms_ask send sms to(%s)) len(%d).", util_clock(), number, index);
    sms_send((char*)recv_msg, index, (char*)number, GM_UCS2_DCS);

    GM_MemoryFree(recv_msg);
    GM_MemoryFree(recv_msg_w);
}


//信息头(67 67) + 协议号(1B) + 包长度(2B) + 序列号(2B)  + 内容(NB)
//6767 80 000D FFFF 01 00736F69 574845524523
//6767 80 000D FFFF 01 00736FD5 574845524523
// 1B:信息标示  0x01:指令  0x02:互动指令
// 4B:服务器标示
static void protocol_goome_parse_remote_msg(U8* pdata, u16 len)
{
    U8* pOut = NULL;
    u16 out_len = 0;
    u8 idx = 0;
    u16 index = 0;
	u16 msg_len = 0;
    
    idx = 7;
    s_msg_save.msg_flag = pdata[idx++];   // 信息标识
    GM_memcpy(s_msg_save.server_id, &pdata[idx], sizeof(s_msg_save.server_id));  // 服务器标志  
    idx += 4;
    msg_len = len - idx;
    
    pOut = GM_MemoryAlloc(CMD_MAX_LEN + 1);
    if (NULL == pOut)
    {
        LOG(WARN,"clock(%d) protocol_goome_parse_remote_msg assert(GM_MemoryAlloc(%d)) failed.", util_clock(), CMD_MAX_LEN + 1);
        return;
    }
    
    GM_memset(pOut, 0x00, CMD_MAX_LEN + 1);

    for (index=0; index< msg_len; index++)
    {
        if ((',' == pdata[idx+index]) || ('#' == pdata[idx+index]) || ('*' == pdata[idx+index]))
        {
            break;
        }
    }
    //LOG(INFO,"Receive server command:%s",&pdata[idx]);
	command_on_receive_data(COMMAND_GPRS, (char *)&pdata[idx], msg_len, (char *)pOut,NULL);
    out_len = GM_strlen((char *)pOut);
    if (out_len > 0)
    {
        gps_service_after_receive_remote_msg(pOut, out_len);
    }

    GM_MemoryFree(pOut);
    return;
}


void protocol_goome_pack_remote_ack(U8* pdata, u16 *idx, u16 len, U8* pRet, u16 retlen)
{
    protocol_goome_pack_head(pdata, idx, len);  //7 bytes
    pdata[(*idx)++] = s_msg_save.msg_flag;
    GM_memcpy(&pdata[(*idx)], s_msg_save.server_id, sizeof(s_msg_save.server_id));
    (*idx) += sizeof(s_msg_save.server_id);
    GM_memcpy(&pdata[(*idx)], pRet, retlen);
    (*idx) += retlen;
    protocol_goome_pack_id_len(pdata, PROTOCCOL_GOOME_REMOTE_MSG, *idx);
}


void protocol_goome_pack_position_request_msg(u8 *mobile_num, u8 num_len, u8 *command, u8 cmd_len,u8 *pdata, u16 *idx, u16 len)
{
    GPSData gps;
    GM_memset(&gps,0,sizeof(gps));
    
    gps_get_last_data(&gps);
    if(gps.gps_time == 0)
    {
        system_state_get_last_gps(&gps);
    }
    
    protocol_goome_pack_head(pdata, idx, len);  //7 bytes
    protocol_goome_pack_gps(&gps, pdata, idx, len);  //15 bytes
    protocol_goome_pack_lbs(pdata, idx, len);  //9 bytes
    protocol_goome_pack_locate_status(pdata, idx, len);  //1 bytes
    GM_memset(&pdata[(*idx)],0,21);
    num_len = num_len < 21?num_len:20;
    GM_memcpy(&pdata[(*idx)],mobile_num,num_len);
    (*idx)+=21;   //21 bytes
    GM_memcpy(&pdata[(*idx)],command,cmd_len);
    (*idx) += cmd_len;
    protocol_goome_pack_id_len(pdata, PROTOCCOL_GOOME_SMS_MSG, *idx);
}

