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
#include "protocol_jt808.h"
#include "gps_service.h"
#include "sms.h"
#include "system_state.h"
#include "hard_ware.h"
#include "command.h"
#include "agps_service.h"

#define JT_DATA_DEPART_FLAG      0x2000   //分包标志    0010 0000 0000 0000
#define JT_DATA_SEC_FLAG      0x1C00   //加密标志    0001 1100 0000 0000

typedef enum 
{
    AT_TYPE_STANDARD = 0, //标准
    AT_TYPE_CUSTOM = 1  //补充
} Jt08AtTypeEnum;

typedef enum
{
    JT_CMD_TERM_ACK = 0x0001,   //终端通用应答
    JT_CMD_HEART_BEAT = 0x0002,   //终端心跳
    JT_CMD_UNREGISTER = 0x0003,   //注销
    JT_CMD_PLAT_ACK = 0x8001,   //平台通用应答
    JT_CMD_REGISTER = 0x0100,   //终端注册
    JT_CMD_REGISTER_ACK = 0x8100,   //终端注册应答
    JT_CMD_ICCID = 0x0120,   //终端上报iccid
    JT_CMD_AUTH = 0x0102,   //终端鉴权
    JT_CMD_SET_PARAM = 0x8103,   //设置终端参数
    JT_CMD_GET_PARAM = 0x8104,   //查询终端参数
    JT_CMD_TERM_CTL = 0x8105,   //终端控制
    JT_CMD_GET_PARAM_ACK = 0x0104,   //查询终端参数应答
    JT_CMD_LOCATE = 0x0200,   //位置信息汇报
    JT_CMD_LOCATE_REQ = 0x8201,   //位置信息查询
    JT_CMD_LOCATE_REQ_ACK = 0x0201,   //位置信息查询应答
    JT_CMD_LOCATE_MULTI = 0x704,   //定位数据批量上传
    JT_CMD_TEXT_RESULT = 0x6006,  // 终端文本数据上报
    JT_CMD_CONFIRM_ALARM = 0x8203,   //确认紧急报警
    JT_CMD_TEXT = 0x8300    //文本信息下发 
}Jt08CmdType;

typedef enum
{
    JT_PARAM_HEART_INTERVAL = 0x0001,   //终端心跳发送间隔,单位为（s）
    JT_PARAM_APN = 0x0010,   //主服务器 APN,无线通信拨号访问点。
    JT_PARAM_APN_USR = 0x0011,   //主服务器 APN 拨号用户名
    JT_PARAM_APN_PWD = 0x0012,   //主服务器 APN,拨号密码
    JT_PARAM_IP = 0x0013,   //主服务器地址,IP 或域名
    JT_PARAM_IP_BAK = 0x0017,   //备份服务器地址,IP 或域名
    JT_PARAM_PORT = 0x0018,   //服务器 TCP 端口
    JT_PARAM_LOCATE_TYPE = 0x0020,   //位置汇报策略,0:定时汇报；1:定距汇报；2:定时和定距汇报
    JT_PARAM_HEART_LOCATE_INTERVAL = 0x0027,   //休眠时汇报时间间隔,单位为秒（s）,>0
    JT_PARAM_TIME_INTERVAL = 0x0029,   //缺省时间汇报间隔,单位为秒（s）,>0
    JT_PARAM_DISTANCE_INTERVAL = 0x002C,   //缺省距离汇报间隔,单位为米（m）,>0
    JT_PARAM_TURN_ANGLE = 0x0030,   //拐点补传角度,<180
    JT_PARAM_MAX_SPEED = 0x0055,   //最高速度,单位为千米每小时（km/h）
    JT_PARAM_EXCEED_SPEED_TIME = 0x0056,   //超速持续时间,单位为秒（s）
    JT_PARAM_MILIAGE = 0x0080,   //车辆里程表读数,1/10km
    JT_PARAM_PROVINCE = 0x0081,   //车辆所在的省域 ID,1～255
    JT_PARAM_CITY = 0x0082,   //车辆所在的市域 ID,1～255
    JT_PARAM_BRAND = 0x0083,   //公安交通同管理部门颁发的机劢车号牌
    JT_PARAM_BRAND_COLOR = 0x0084,   //车牌颜色,按照 JT/T415—2006 中 5.4.12 的规定
}Jt08ParamType;


/*用于记录发送消息的序列号, 收到消息的序列号等*/
typedef struct
{
    u16 msg_serial;  //上传数据序列号, 每次发消息加一
    bool last_gps_ok; //上一次的gps有数据
    GPSData last_gps; //上一次的gps

    u16 server_serial;  //数据序列号
    u16 msg_id;     // 消息号
    u8 msg_result;     // 消息号0:成功/确认；1:失败；2:消息有误；3:不支持； 4:报警处理确认
}Jt08MsgSave;

static Jt08MsgSave s_jt_msg_save = {0, 0, };

static void protocol_jt_pack_escape(u8 *src, u16 src_len, u8 *dest, u16 *dest_len);
static GM_ERRCODE protocol_jt_pack_unescape(u8 *src, u16 *src_len);
static void protocol_jt_pack_head(u8 *pdata, u16 *idx, u16 len, u16 total, u16 part_num );
static void protocol_jt_pack_id_len(u8 *pdata, u16 *idx, u16 cmd, u16 len, u8 sec, bool depart);

static void protocol_jt_pack_register_info(u8 *pdata, u16 *idx, u16 len, u16 *return_len);
static void protocol_jt_pack_auth_code(u8 *pdata, u16 *idx, u16 len, u16 *return_len);
static void protocol_jt_pack_iccid(u8 *pdata, u16 *idx, u16 len, u16 *return_len);
static void protocol_jt_pack_gps_info(GpsDataModeEnum mode, const GPSData *gps, u8 *pdata, u16 *idx, u16 len, u16 *return_len);
static void protocol_jt_parse_general_msg(U8* pdata, u16 len);
static void protocol_jt_pack_additional_mileage(u8 *pdata, u16 *idx, u16 len, u16 *return_len);
static void protocol_jt_pack_device_status(u8 *pdata, u16 *idx, u16 len, u16 *return_len,GPSData* p_data);
static void protocol_jt_pack_alarm_status(u8 *pdata, u16 *idx, u16 len, u16 *return_len);
static void protocol_jt_pack_additional_lbs(u8 *pdata, u16 *idx, u16 len, u16 *return_len);
static void protocol_jt_pack_server_info(u8 *pdata, u16 *idx, u16 len, u16 *return_len);
static void protocol_jt_pack_param_info(u8 *pdata, u16 *idx, u16 len, u16 *return_len);
static void protocol_jt_pack_item_num(u8 *pdata, u16 *idx, u32 item, u32 value, u8 item_len);
static void protocol_jt_pack_item_string(u8 *pdata, u16 *idx, u32 item, u8 *pitem, u8 item_len);


static void protocol_jt_parse_text(U8* pdata, u16 len);
static void protocol_jt_parse_gps_req(U8* pdata, u16 len);
static void protocol_jt_parse_terminal_control(U8* pdata, u16 len);
static void protocol_jt_parse_get_param(U8* pdata, u16 len);
static void protocol_jt_parse_set_param(U8* pdata, u16 len);
static void protocol_jt_parse_register_ack(U8* pdata, u16 len);
static void protocol_jt_parse_plat_ack(U8* pdata, u16 len);
static u32 get_value_by_length(u8 *p,u8 l);

static u32 get_value_by_length(u8 *p,u8 l)
{
    if(l==4)
    {
        return( ((u32)p[0]<<24)+((u32)p[1]<<16)+((u16)p[2]<<8)+p[3]);
    }
    else if(l==3)
    {
        return(((u32)p[0]<<16)+((u16)p[1]<<8)+p[2]);
    }
    else if(l==2)
    {
        return(((u16)p[0]<<8)+p[1]);
    }
    else
    {
        return(p[0]);
    }
}


static void protocol_jt_pack_escape(u8 *src, u16 src_len, u8 *dest, u16 *dest_len)
{
    u16 i = 0, j = 0;

    //首字节是 PROTOCOL_HEADER_JT808
    if((*dest_len) < j + 1)
    {
        LOG(INFO,"clock(%d) protocol_jt_pack_escape assert(dest_len(%d) >= %d) failed.", util_clock(), *dest_len, j + 1);
        return;
    }
    dest[j] = src[i];
    j++;

    // 最后一个字节也是 PROTOCOL_HEADER_JT808
    for(i = 1; i<src_len - 1; ++i)
    {
        if(src[i] == PROTOCOL_HEADER_JT808)
        {
            if((*dest_len) < j + 2)
            {
                LOG(INFO,"clock(%d) protocol_jt_pack_escape assert(dest_len(%d) >= %d) failed.", util_clock(), *dest_len, j + 2);
                return;
            }
            
            dest[j++] = PROTOCOL_HEADER_ESCAPE;
            dest[j++] = 0x02;
        }
        else if(src[i] == PROTOCOL_HEADER_ESCAPE)
        {
            if((*dest_len) < j + 2)
            {
                LOG(INFO,"clock(%d) protocol_jt_pack_escape assert(dest_len(%d) >= %d) failed.", util_clock(), *dest_len, j + 2);
                return;
            }
            
            dest[j++] = PROTOCOL_HEADER_ESCAPE;
            dest[j++] = 0x01;
        }
        else
        {
            if((*dest_len) < j + 1)
            {
                LOG(INFO,"clock(%d) protocol_jt_pack_escape assert(dest_len(%d) >= %d) failed.", util_clock(), *dest_len, j + 1);
                return;
            }
            
            dest[j++] = src[i];
        }
    }
    
    dest[j++] = src[i++];  //last byte 0x7e
    (*dest_len) = j;
}



static GM_ERRCODE protocol_jt_pack_unescape(u8 *src, u16 *src_len)
{
    u16 i = 0, j = 0;

    //首,尾字节是0x7e

    //其余字节
    for(i = 1,j=1; i< (*src_len - 1); ++i,++j)
    {
        if(src[i] == PROTOCOL_HEADER_ESCAPE)
        {
            if(src[i + 1] == 0x01)
            {
                src[j] = PROTOCOL_HEADER_ESCAPE;
                i++;
            }
            else if(src[i + 1] == 0x02)
            {
                src[j] = PROTOCOL_HEADER_JT808;
                i++;
            }
            else
            {
                LOG(INFO,"clock(%d) protocol_jt_pack_unescape assert(%02x after 0x7d) failed.", util_clock(), src[i + 1]);
                return GM_PARAM_ERROR;
            }
        }
        else
        {
            if(i != j)
            {
                src[j] = src[i];
            }
        }
    }
    src[j++] = src[i++];  //last byte 0x7e
    (*src_len) = j;
    return GM_SUCCESS;
}

static void protocol_jt_pack_head(u8 *pdata, u16 *idx, u16 len, u16 total, u16 part_num )
{
    GM_ERRCODE ret = GM_SUCCESS;
    u8 imei[GM_IMEI_LEN + 1] = {0};
	u8 app_ver;
	
    if((*idx) + 13 > len)
    {
        LOG(WARN,"clock(%d) protocol_jt_pack_head assert(len(%d)) failed.", util_clock(), len);
        return;
    }
    
    pdata[(*idx)++]=PROTOCOL_HEADER_JT808;  // 1 bytes
    (*idx) += 4;  //id 2  len 2   total 4 bytes

    if(GM_SUCCESS != (ret = gsm_get_imei(imei)))
    {
        LOG(INFO,"clock(%d) protocol_jt_pack_head can not get imei, ret:%d.", util_clock(), ret);
    }
    
    if (0 == GM_strlen((const char *)imei))
    {
        GM_memset(imei, 0, sizeof(imei));
    }

    
    config_service_get(CFG_PROTOCOL_VER, TYPE_BYTE, &app_ver, sizeof(app_ver));
	//四川移动 取imei号 3~14位
    if(app_ver == PROTOCOL_VER_CMCC_JT808)
    {
        pdata[(*idx)++] = MERGEBCD(util_chr(imei[2]), util_chr(imei[3]));
	    pdata[(*idx)++] = MERGEBCD(util_chr(imei[4]), util_chr(imei[5]));
	    pdata[(*idx)++] = MERGEBCD(util_chr(imei[6]), util_chr(imei[7]));
	    pdata[(*idx)++] = MERGEBCD(util_chr(imei[8]), util_chr(imei[9]));
	    pdata[(*idx)++] = MERGEBCD(util_chr(imei[10]), util_chr(imei[11]));
		pdata[(*idx)++] = MERGEBCD(util_chr(imei[12]), util_chr(imei[13]));
    }
	else //取imei号后10位, 前面加"1"(0x31)
	{
		pdata[(*idx)++] = MERGEBCD(0, 0x31);
	    pdata[(*idx)++] = MERGEBCD(util_chr(imei[5]), util_chr(imei[6]));
	    pdata[(*idx)++] = MERGEBCD(util_chr(imei[7]), util_chr(imei[8]));
	    pdata[(*idx)++] = MERGEBCD(util_chr(imei[9]), util_chr(imei[10]));
	    pdata[(*idx)++] = MERGEBCD(util_chr(imei[11]), util_chr(imei[12]));
	    pdata[(*idx)++] = MERGEBCD(util_chr(imei[13]), util_chr(imei[14]));
	}
    
    // 6 bytes


    //消息流水号
    ++(s_jt_msg_save.msg_serial);
    pdata[(*idx)++] = BHIGH_BYTE(s_jt_msg_save.msg_serial);
    pdata[(*idx)++] = BLOW_BYTE(s_jt_msg_save.msg_serial);
    // 2 bytes

    if(total > 1)  // 此时 protocol_jt_pack_id_len的参数depart值应为true
    {
        if((*idx) + 4 > len)
        {
            LOG(WARN,"clock(%d) protocol_jt_pack_head assert(len(%d)) failed.", util_clock(), len);
            return;
        }
        
        pdata[(*idx)++] = BHIGH_BYTE(total);
        pdata[(*idx)++] = BLOW_BYTE(total);
        pdata[(*idx)++] = BHIGH_BYTE(part_num);
        pdata[(*idx)++] = BLOW_BYTE(part_num);
    }
}

static void protocol_jt_pack_id_len(u8 *pdata, u16 *idx, u16 cmd, u16 len, u8 sec, bool depart)
{
    u8  check_sum=0;
    u16 j;
    u16 attribute = 0;

    // msg id 2 bytes
    pdata[1] = BHIGH_BYTE(cmd);
    pdata[2] = BLOW_BYTE(cmd);

    if(depart)
    {
        depart = 1; 
    }
    else
    {
        depart = 0;
    }

    // len 2 bytes
    // 保留(15 14)  分包(13) 加密(12 11 10) 长度(9-0) 
    attribute = len & 0x3FF + (((u16)depart) << 13) + (((u16)(sec & 0x7)) << 10);
    pdata[3] = BHIGH_BYTE(attribute);
    pdata[4] = BLOW_BYTE(attribute);

    check_sum = 0;
    for (j=1; j<(*idx); j++)
    {
        check_sum ^= pdata[j];
    }
    pdata[(*idx)++] = check_sum;
    pdata[(*idx)++] = PROTOCOL_HEADER_JT808;
}


static void protocol_jt_pack_register_info(u8 *pdata, u16 *idx, u16 len, u16 *return_len)
{
    u8 value_u8; 
    u16 value_u16; 
    u8 at_type;
    u8 copy_len;
    u16 orginal_idx = *idx;
    u8 value_string[20]; 
	u8 app_ver;

    config_service_get(CFG_JT_AT_TYPE, TYPE_BYTE, &at_type, sizeof(at_type));
    
    //省域ID=2byte
    config_service_get(CFG_JT_PROVINCE, TYPE_SHORT, &value_u16, sizeof(value_u16));
    pdata[(*idx)++] = BHIGH_BYTE(value_u16);
    pdata[(*idx)++] = BLOW_BYTE(value_u16);

    //市域ID=2byte
    config_service_get(CFG_JT_CITY, TYPE_SHORT, &value_u16, sizeof(value_u16));
    pdata[(*idx)++] = BHIGH_BYTE(value_u16);
    pdata[(*idx)++] = BLOW_BYTE(value_u16);
    
    //制造商ID=5byte
    copy_len = 5;
    GM_memset(value_string, 0 , copy_len);
    config_service_get(CFG_JT_OEM_ID, TYPE_STRING, value_string, sizeof(value_string));
    GM_memcpy(&pdata[(*idx)], value_string, copy_len);
    (*idx) += copy_len;

	//四川移动 终端型号固定用20byte
	config_service_get(CFG_PROTOCOL_VER, TYPE_BYTE, &app_ver, sizeof(app_ver));
    //终端型号=8byte
	config_service_get(CFG_DEVICETYPE, TYPE_SHORT, &value_u16, sizeof(value_u16));
    if((at_type == AT_TYPE_CUSTOM) || (app_ver == PROTOCOL_VER_CMCC_JT808))
    {
        copy_len = 20;
        GM_memset(&pdata[(*idx)], 0 , copy_len);
        GM_strncpy((char *)&pdata[(*idx)],config_service_get_device_type(value_u16), copy_len);
        (*idx) += copy_len;
    }
    else
    {
        copy_len = 8;
        GM_memset(&pdata[(*idx)], 0 , copy_len);
        GM_strncpy((char *)&pdata[(*idx)],config_service_get_device_type(value_u16), copy_len);
        (*idx) += copy_len;
    }
    

    //终端ID=7byte
    copy_len = 7;
    GM_memset(value_string, 0 , copy_len);
    config_service_get(CFG_JT_DEVICE_ID, TYPE_STRING, value_string, sizeof(value_string));
    GM_memcpy(&pdata[(*idx)], value_string, copy_len);
    (*idx) += copy_len;
    
    //车身颜色
    config_service_get(CFG_JT_VEHICLE_COLOR, TYPE_BYTE, &value_u8, sizeof(value_u8));
    pdata[(*idx)++] = value_u8;
    
    
    //车牌号码=nbyte     //default 8bytes
    config_service_get(CFG_JT_VEHICLE_NUMBER, TYPE_STRING, value_string, sizeof(value_string));
    copy_len = util_remove_char(value_string, GM_strlen((const char *)value_string),' ');
    GM_memcpy(&pdata[(*idx)], value_string, copy_len);
    (*idx) += copy_len;
    
    *return_len = (*idx) - orginal_idx;
}

void protocol_jt_pack_regist_msg(u8 *pdata, u16 *idx, u16 len)
{
    u8 *send;
    u16 send_len = 0;
    u16 content_len = 0;
	send = (u8 *) GM_MemoryAlloc(len);
	if (send == NULL)
	{
        LOG(INFO,"clock(%d) protocol_jt_pack_regist_msg assert(GM_MemoryAlloc(%d)) failed.", util_clock(), len);
		return;
	}
    

    protocol_jt_pack_head(send, &send_len, len, 1, 0);  // 13bytes
    protocol_jt_pack_register_info(send, &send_len, len, &content_len);   // max 57 bytes; default 33
    if((send_len + 2) > len)
    {
        LOG(WARN,"clock(%d) protocol_jt_pack_regist_msg assert(len(%d)) failed.", util_clock(), len);
        GM_MemoryFree(send);
        return;
    }
    protocol_jt_pack_id_len(send, &send_len, JT_CMD_REGISTER, content_len, 0, false);  // 2bytes

    *idx = len;
    protocol_jt_pack_escape(send, send_len, pdata, idx);  

    GM_MemoryFree(send);
}


static void protocol_jt_pack_auth_code(u8 *pdata, u16 *idx, u16 len, u16 *return_len)
{
    u8 content_len;
    u8 auth_code[100];

    GM_memset(auth_code, 0x00, sizeof(auth_code));
    config_service_get(CFG_JT_AUTH_CODE, TYPE_STRING, &auth_code, sizeof(auth_code));

    //鉴权码=nbyte
    content_len = auth_code[0];
    if (content_len > 100)
    {
        auth_code[0] = 0;
        content_len = 0;
    }
    
    if((*idx) + content_len > len)
    {
        LOG(WARN,"clock(%d) protocol_jt_pack_auth_code assert(len(%d)) failed.", util_clock(), content_len);
        return;
    }
    
    GM_memcpy(&pdata[*idx], &auth_code[1], content_len);
    (*idx) += content_len;
    *return_len = content_len;
}

void protocol_jt_pack_auth_msg(u8 *pdata, u16 *idx, u16 len)
{
    u8 *send;
    u16 send_len = 0;
    u16 content_len = 0;
	send = (u8 *) GM_MemoryAlloc(len);
	if (send == NULL)
	{
        LOG(INFO,"clock(%d) protocol_jt_pack_auth_msg assert(GM_MemoryAlloc(%d)) failed.", util_clock(), len);
		return;
	}
    

    protocol_jt_pack_head(send, &send_len, len, 1, 0);  // 13bytes | 17bytes
    protocol_jt_pack_auth_code(send, &send_len, len, &content_len); 
    if((send_len + 2) > len)
    {
        LOG(WARN,"clock(%d) protocol_jt_pack_auth_msg assert(len(%d)) failed.", util_clock(), len);
        GM_MemoryFree(send);
        return;
    }
    protocol_jt_pack_id_len(send, &send_len, JT_CMD_AUTH, content_len, 0, false);  // 2bytes
    
    *idx = len;
    protocol_jt_pack_escape(send, send_len, pdata, idx);  

    GM_MemoryFree(send);
    return;
}


static void protocol_jt_pack_iccid(u8 *pdata, u16 *idx, u16 len, u16 *return_len)
{
    GM_ERRCODE ret = GM_SUCCESS;
    u8 iccid[GM_ICCID_LEN + 1] = {0};
    u16 content_len = 0;
    
    if(GM_SUCCESS != (ret = gsm_get_iccid(iccid)))
    {
        LOG(INFO,"clock(%d) protocol_jt_pack_iccid can not get iccid, ret:%d.", util_clock(), ret);
    }
    content_len = GM_strlen((const char *)iccid);
    if (0 == content_len)
    {
        GM_memset(iccid, 0, sizeof(iccid));
    }

    if((*idx) + content_len > len)
    {
        LOG(WARN,"clock(%d) protocol_jt_pack_iccid assert(len(%d)) failed.", util_clock(), content_len);
        return;
    }
    
    GM_memcpy(&pdata[*idx], &iccid[0], content_len);
    (*idx) += content_len;
    *return_len = content_len;
}



void protocol_jt_pack_iccid_msg(u8 *pdata, u16 *idx, u16 len)
{
    u8 *send;
    u16 send_len = 0;
    u16 content_len = 0;
	send = (u8 *) GM_MemoryAlloc(len);
	if (send == NULL)
	{
        LOG(INFO,"clock(%d) protocol_jt_pack_iccid_msg assert(GM_MemoryAlloc(%d)) failed.", util_clock(), len);
		return;
	}
    

    protocol_jt_pack_head(send, &send_len, len, 1, 0);  // 13bytes | 17bytes
    protocol_jt_pack_iccid(send, &send_len, len, &content_len);  //20 bytes
    if((send_len + 2) > len)
    {
        LOG(WARN,"clock(%d) protocol_jt_pack_iccid_msg assert(len(%d)) failed.", util_clock(), len);
        GM_MemoryFree(send);
        return;
    }
    protocol_jt_pack_id_len(send, &send_len, JT_CMD_ICCID, content_len, 0, false);  // 2bytes
    *idx = len;
    protocol_jt_pack_escape(send, send_len, pdata, idx);  

    GM_MemoryFree(send);
    return;
}



void protocol_jt_pack_logout_msg(u8 *pdata, u16 *idx, u16 len)
{
    u8 *send;
    u16 send_len = 0;
    u16 content_len = 0;
	send = (u8 *) GM_MemoryAlloc(len);
	if (send == NULL)
	{
        LOG(INFO,"clock(%d) protocol_jt_pack_logout_msg assert(GM_MemoryAlloc(%d)) failed.", util_clock(), len);
		return;
	}
    

    protocol_jt_pack_head(send, &send_len, len, 1, 0);  // 13bytes | 17bytes
    if((send_len + 2) > len)
    {
        LOG(WARN,"clock(%d) protocol_jt_pack_logout_msg assert(len(%d)) failed.", util_clock(), len);
        GM_MemoryFree(send);
        return;
    }
    protocol_jt_pack_id_len(send, &send_len, JT_CMD_UNREGISTER, content_len, 0, false);  // 2bytes
    *idx = len;
    protocol_jt_pack_escape(send, send_len, pdata, idx);  

    GM_MemoryFree(send);
    return;
}

void protocol_jt_pack_heartbeat_msg(u8 *pdata, u16 *idx, u16 len)
{
    u8 *send;
    u16 send_len = 0;
    u16 content_len = 0;
	send = (u8 *) GM_MemoryAlloc(len);
	if (send == NULL)
	{
        LOG(INFO,"clock(%d) protocol_jt_pack_heartbeat_msg assert(GM_MemoryAlloc(%d)) failed.", util_clock(), len);
		return;
	}
    

    protocol_jt_pack_head(send, &send_len, len, 1, 0);  // 13bytes | 17bytes
    if((send_len + 2) > len)
    {
        LOG(WARN,"clock(%d) protocol_jt_pack_heartbeat_msg assert(len(%d)) failed.", util_clock(), len);
        GM_MemoryFree(send);
        return;
    }
    protocol_jt_pack_id_len(send, &send_len, JT_CMD_HEART_BEAT, content_len, 0, false);  // 2bytes
    *idx = len;
    protocol_jt_pack_escape(send, send_len, pdata, idx);  

    GM_MemoryFree(send);
    return;
}


static void protocol_jt_pack_gps_info(GpsDataModeEnum mode, const GPSData *gps, u8 *pdata, u16 *idx, u16 len, u16 *return_len)
{
    u16 orginal_idx = *idx;
    u32 latitudev = 0;
    u32 longitudev = 0;
    u16 gps_speed = 0;
    u16 gps_angle = 0;
    u16 gps_hightv = 0;
    u8 bcd_tim[6];
    u8 zone;
    time_t gps_time;
    u8 i;
    
    latitudev = (gps->lat > 0) ? (gps->lat*1000000) : (gps->lat*(-1)*1000000);
    longitudev = (gps->lng > 0) ? (gps->lng*1000000) : (gps->lng*(-1)*1000000);
    gps_hightv = (gps->alt >= 8000)? 0:(gps->alt);
    gps_speed = ((u16)gps->speed > 180) ? 180 : ((u16)gps->speed);
    gps_angle = ((u16)gps->course > 360) ? 0 : ((u16)gps->course);
    gps_time = gps->gps_time;
    zone = config_service_get_zone();
    util_utc_sec_to_bcdtime_base2000(gps_time, bcd_tim, zone);

    LOG(DEBUG,"clock(%d) protocol_jt_pack_gps_info (%d , %d) gps_time(%d:%d:%d) (%02x-%02x-%02x %02x:%02x:%02x).", util_clock(),
        longitudev, latitudev, gps->gps_time, gps_time,util_get_utc_time(), bcd_tim[0],bcd_tim[1],bcd_tim[2],bcd_tim[3],bcd_tim[4],bcd_tim[5]);
    
    pdata[(*idx)++] = BHIGH_BYTE(WHIGH_WORD(latitudev));
    pdata[(*idx)++] = BLOW_BYTE(WHIGH_WORD(latitudev));
    pdata[(*idx)++] = BHIGH_BYTE(WLOW_WORD(latitudev));
    pdata[(*idx)++] = BLOW_BYTE(WLOW_WORD(latitudev));

    pdata[(*idx)++] = BHIGH_BYTE(WHIGH_WORD(longitudev));
    pdata[(*idx)++] = BLOW_BYTE(WHIGH_WORD(longitudev));
    pdata[(*idx)++] = BHIGH_BYTE(WLOW_WORD(longitudev));
    pdata[(*idx)++] = BLOW_BYTE(WLOW_WORD(longitudev));

    pdata[(*idx)++] = BHIGH_BYTE(gps_hightv);
    pdata[(*idx)++] = BLOW_BYTE(gps_hightv);

    gps_speed = gps_speed * 10;
    pdata[(*idx)++] = BHIGH_BYTE(gps_speed);
    pdata[(*idx)++] = BLOW_BYTE(gps_speed);

    pdata[(*idx)++] = BHIGH_BYTE(gps_angle);
    pdata[(*idx)++] = BLOW_BYTE(gps_angle);

    for(i = 0; i < sizeof(bcd_tim); ++i)
    {
        pdata[(*idx)++] = bcd_tim[i];
    }
    
    *return_len = (*idx) - orginal_idx;
}


static void protocol_jt_pack_additional_mileage(u8 *pdata, u16 *idx, u16 len, u16 *return_len)
{
    u32 dwtmp;
    
    pdata[(*idx)++] = 0x01;
    pdata[(*idx)++] = 0x04;   
    dwtmp = system_state_get_mileage() / 100;
    
    pdata[(*idx)++] = BHIGH_BYTE(WHIGH_WORD(dwtmp));
    pdata[(*idx)++] = BLOW_BYTE(WHIGH_WORD(dwtmp));
    pdata[(*idx)++] = BHIGH_BYTE(WLOW_WORD(dwtmp));
    pdata[(*idx)++] = BLOW_BYTE(WLOW_WORD(dwtmp));
        
    *return_len = 6;
}


static void protocol_jt_pack_additional_auth_code(u8 *pdata, u16 *idx, u16 len, u16 *return_len)
{
    u8 index;
	u16 content_len;
    
    pdata[(*idx)++] = 0x40;
	index = *idx;
    pdata[(*idx)++] = 0x00;
	
    protocol_jt_pack_auth_code(pdata, idx, len, &content_len);
	pdata[index] = content_len;
    *return_len = content_len + 2;
}


static void protocol_jt_pack_additional_imsi(u8 *pdata, u16 *idx, u16 len, u16 *return_len)
{
    GM_ERRCODE ret = GM_SUCCESS;
    u8 imsi[GM_IMSI_LEN + 1] = {0};
    u16 content_len = 0;
    
    if(GM_SUCCESS != (ret = gsm_get_imsi(imsi)))
    {
        LOG(INFO,"clock(%d) protocol_jt_pack_imsi can not get imsi, ret:%d.", util_clock(), ret);
    }
    content_len = GM_strlen((const char *)imsi);
    if (0 == content_len)
    {
        GM_memset(imsi, 0, sizeof(imsi));
    }

    if((*idx) + content_len > len)
    {
        LOG(WARN,"clock(%d) protocol_jt_pack_imsi assert(len(%d)) failed.", util_clock(), content_len);
        return;
    }

	pdata[(*idx)++] = 0x41;
	pdata[(*idx)++] = content_len;
    GM_memcpy(&pdata[*idx], &imsi[0], content_len);
    (*idx) += content_len;
    *return_len = content_len + 2;
}



static void protocol_jt_pack_device_status(u8 *pdata, u16 *idx, u16 len, u16 *return_len, GPSData* p_data)
{
    bool value_bool;
    u32 device_status = 0;
    
    if(GM_SUCCESS  !=  hard_ware_get_acc_level(&value_bool))
    {
        LOG(WARN,"clock(%d) protocol_jt_pack_device_status assert(hard_ware_get_acc_level) failed.", util_clock());
    }
    if (value_bool)
    {
        SET_BIT0(device_status);
    }


    //休眠前, 按是否定位设置定位标志, 体眠时, 按之前是否有定位设置定位标志
    if(GM_SYSTEM_STATE_WORK == system_state_get_work_state())
    {
        if (gps_is_fixed())
        {
            SET_BIT1(device_status);
        }
    }
    else
    {
        GPSData gps_data;
        if (gps_get_last_data(&gps_data))
        {
            SET_BIT1(device_status);
        }
    }
    
    // 纬度 负数为南纬
    if (p_data->lat < 0.0f)
    {
        SET_BIT2(device_status);  //南纬
    }
    
    // 经度  负数为西经
    if (p_data->lng < 0.0f)
    {
        SET_BIT3(device_status);  //西经
    }

    if (system_state_get_device_relay_state())
    {
        SET_BIT10(device_status);
    }

    pdata[(*idx)++] = BHIGH_BYTE(WHIGH_WORD(device_status));
    pdata[(*idx)++] = BLOW_BYTE(WHIGH_WORD(device_status));
    pdata[(*idx)++] = BHIGH_BYTE(WLOW_WORD(device_status));
    pdata[(*idx)++] = BLOW_BYTE(WLOW_WORD(device_status));
    
    *return_len = 4;
}

static void protocol_jt_pack_alarm_status(u8 *pdata, u16 *idx, u16 len, u16 *return_len)
{
    u32 sys_state;
    u32 alarm_state = 0;
    
    if((*idx) + 4 > len)
    {
        LOG(WARN,"clock(%d) protocol_jt_pack_alarm_status assert(len(%d)) failed.", util_clock(), len);
        return;
    }

    sys_state = system_state_get_status_bits();


    if(sys_state & SYSBIT_ALARM_SPEED)
    {
        SET_BIT1(alarm_state);
    }
    
    if(sys_state & SYSBIT_ALARM_LOW_POWER)
    {
        SET_BIT7(alarm_state);
    }

    if(sys_state & SYSBIT_ALARM_NO_POWER)
    {
        SET_BIT8(alarm_state);
    }

    if(sys_state & SYSBIT_ALARM_COLLIDE)
    {
        SET_BIT16(alarm_state);
    }

    if(sys_state & SYSBIT_ALARM_SPEED_UP)
    {
        SET_BIT17(alarm_state);
    }

    if(sys_state & SYSBIT_ALARM_TURN_OVER)
    {
        SET_BIT29(alarm_state);
    }
    
    if(sys_state & SYSBIT_ALARM_SPEED_DOWN)
    {
        SET_BIT30(alarm_state);
    }

    if(sys_state & SYSBIT_ALARM_SHARP_TURN)
    {
        SET_BIT31(alarm_state);
    }
    
    pdata[(*idx)++] = BHIGH_BYTE(WHIGH_WORD(alarm_state));
    pdata[(*idx)++] = BLOW_BYTE(WHIGH_WORD(alarm_state));
    pdata[(*idx)++] = BHIGH_BYTE(WLOW_WORD(alarm_state));
    pdata[(*idx)++] = BLOW_BYTE(WLOW_WORD(alarm_state));
    *return_len = 4;
}

void protocol_jt_pack_gps_msg(GpsDataModeEnum mode, const GPSData *gps, u8 *pdata, u16 *idx, u16 len)
{
    u8 *send;
    u16 send_len = 0;
    u16 content_len = 0;
    u16 content_all = 0;
    GPSData tmp_gps;
	u8 app_ver;

	send = (u8 *) GM_MemoryAlloc(len);
	if (send == NULL)
	{
        LOG(INFO,"clock(%d) protocol_jt_pack_gps_msg assert(GM_MemoryAlloc(%d)) failed.", util_clock(), len);
		return;
	}

    GM_memcpy(&tmp_gps, gps, sizeof(tmp_gps));    
    if(tmp_gps.gps_time == 0)
    {
        system_state_get_last_gps(&tmp_gps);
    }
    
    if(tmp_gps.gps_time == (time_t)0)
    {
        tmp_gps.gps_time = util_get_utc_time();
        tmp_gps.lat = agps_service_get_unfix_lat();
        tmp_gps.lng = agps_service_get_unfix_lng();
    }

    protocol_jt_pack_head(send, &send_len, len, 1, 0);  // 13bytes | 17bytes
    protocol_jt_pack_alarm_status(send, &send_len, len, &content_len);  //4 bytes
    content_all += content_len;
    protocol_jt_pack_device_status(send, &send_len, len, &content_len, &tmp_gps);  //4 bytes
    content_all += content_len;
    protocol_jt_pack_gps_info(mode, &tmp_gps, send, &send_len, len, &content_len);  //20 bytes
    content_all += content_len;
    protocol_jt_pack_additional_mileage(send, &send_len, len, &content_len);  //6 bytes
    content_all += content_len;

	config_service_get(CFG_PROTOCOL_VER, TYPE_BYTE, &app_ver, sizeof(app_ver));
	//四川移动 固定增加鉴权码及IMSI
    if(app_ver == PROTOCOL_VER_CMCC_JT808)
	{
		protocol_jt_pack_additional_auth_code(send, &send_len, len, &content_len); //max 102byte
		content_all += content_len;
		protocol_jt_pack_additional_imsi(send, &send_len, len, &content_len); //17byte
		content_all += content_len;
	}
	
    if((send_len + 2) > len)
    {
        LOG(WARN,"clock(%d) protocol_jt_pack_gps_msg assert(len(%d)) failed.", util_clock(), len);
        GM_MemoryFree(send);
        return;
    }
    protocol_jt_pack_id_len(send, &send_len, JT_CMD_LOCATE, content_all, 0, false);  // 2bytes
    *idx = len;
    protocol_jt_pack_escape(send, send_len, pdata, idx);
    
    GM_MemoryFree(send);
    return;
}


static void protocol_jt_pack_additional_lbs(u8 *pdata, u16 *idx, u16 len, u16 *return_len)
{
    u16 orginal_idx = *idx;
    u16 k,num=0;
    GM_ERRCODE ret;
    
    gm_cell_info_struct lbs;
    GM_memset(&lbs,0, sizeof(lbs));
    
    if((*idx) + 9 > len)
    {
        LOG(WARN,"clock(%d) protocol_jt_pack_additional_lbs assert(len(%d)) failed.", util_clock(), len);
        return;
    }

    ret = gsm_get_cell_info(&lbs);
    LOG(DEBUG,"clock(%d) protocol_jt_pack_additional_lbs ret(%d) lbs(%d).", util_clock(), ret, lbs.nbr_cell_num);
    
    num = (lbs.nbr_cell_num > 5) ? 5: lbs.nbr_cell_num;
    pdata[(*idx)++] = 0x53;//基站信息
    pdata[(*idx)++] = 1+8*(num + 1);//数据长度
    pdata[(*idx)++] = (num + 1);//基站个数
    
    pdata[(*idx)++] = BHIGH_BYTE(lbs.serv_info.mcc); 
    pdata[(*idx)++] = BLOW_BYTE(lbs.serv_info.mcc);
    pdata[(*idx)++] = BLOW_BYTE(lbs.serv_info.mnc); 
    pdata[(*idx)++] = BHIGH_BYTE(lbs.serv_info.lac); 
    pdata[(*idx)++] = BLOW_BYTE(lbs.serv_info.lac);
    pdata[(*idx)++] = BHIGH_BYTE(lbs.serv_info.ci);
    pdata[(*idx)++] = BLOW_BYTE(lbs.serv_info.ci); 
    pdata[(*idx)++] = lbs.serv_info.rxlev;// 信号强度  
        
    GM_memset(&pdata[(*idx)], 0, 40);
    for (k=0; k<num; k++) 
    {
        pdata[(*idx)++] = BHIGH_BYTE(lbs.nbr_cell_info[k].mcc); 
        pdata[(*idx)++] = BLOW_BYTE(lbs.nbr_cell_info[k].mcc);
        pdata[(*idx)++] = BLOW_BYTE(lbs.nbr_cell_info[k].mnc); 
        pdata[(*idx)++] = BHIGH_BYTE(lbs.nbr_cell_info[k].lac); 
        pdata[(*idx)++] = BLOW_BYTE(lbs.nbr_cell_info[k].lac);
        pdata[(*idx)++] = BHIGH_BYTE(lbs.nbr_cell_info[k].ci);
        pdata[(*idx)++] = BLOW_BYTE(lbs.nbr_cell_info[k].ci); 
        pdata[(*idx)++] = lbs.nbr_cell_info[k].rxlev;// 信号强度 
    }
    
    *return_len = (*idx) - orginal_idx;
}


void protocol_jt_pack_lbs_msg(u8 *pdata, u16 *idx, u16 len)
{
    u8 *send;
    u16 send_len = 0;
    u16 content_len = 0;
    u16 content_all = 0;
    GPSData gps;
	u8 app_ver;

	send = (u8 *) GM_MemoryAlloc(len);
	if (send == NULL)
	{
        LOG(INFO,"clock(%d) protocol_jt_pack_lbs_msg assert(GM_MemoryAlloc(%d)) failed.", util_clock(), len);
		return;
	}
    
    GM_memset(&gps,0,sizeof(gps));
    gps_get_last_data(&gps);
    if(gps.gps_time == 0)
    {
        system_state_get_last_gps(&gps);
    }
    
    
    if(gps.gps_time == (time_t)0)
    {
        gps.lat = agps_service_get_unfix_lat();
        gps.lng = agps_service_get_unfix_lng();

        if(gps.lat < 0.00001f)
        {
            gps.gps_time = 0;
        }
		else
		{
			//use current time.
			gps.gps_time = util_get_utc_time();
		}
    }
	else
	{
	    //use current time.
	    gps.gps_time = util_get_utc_time();
	}


    protocol_jt_pack_head(send, &send_len, len, 1, 0);  // 13bytes | 17bytes
    protocol_jt_pack_alarm_status(send, &send_len, len, &content_len);  //4 bytes
    content_all += content_len;
    protocol_jt_pack_device_status(send, &send_len, len, &content_len, &gps);  //4 bytes
    content_all += content_len;
    protocol_jt_pack_gps_info(GPS_MODE_FIX_TIME, &gps, send, &send_len, len, &content_len);  //20 bytes
    content_all += content_len;
    protocol_jt_pack_additional_mileage(send, &send_len, len, &content_len);  //6 bytes
    content_all += content_len;
	config_service_get(CFG_PROTOCOL_VER, TYPE_BYTE, &app_ver, sizeof(app_ver));
	//四川移动 固定增加鉴权码及IMSI
    if(app_ver == PROTOCOL_VER_CMCC_JT808)
	{
		protocol_jt_pack_additional_auth_code(send, &send_len, len, &content_len); //max 102byte
		content_all += content_len;
		protocol_jt_pack_additional_imsi(send, &send_len, len, &content_len); //17byte
		content_all += content_len;
	}
	else
	{
		protocol_jt_pack_additional_lbs(send, &send_len, len, &content_len);  //max 51 bytes
    	content_all += content_len;
	}
    if((send_len + 2) > len)
    {
        LOG(WARN,"clock(%d) protocol_jt_pack_lbs_msg assert(len(%d)) failed.", util_clock(), len);
        GM_MemoryFree(send);
        return;
    }
    protocol_jt_pack_id_len(send, &send_len, JT_CMD_LOCATE, content_all, 0, false);  // 2bytes
    *idx = len;
    protocol_jt_pack_escape(send, send_len, pdata, idx);  

    GM_MemoryFree(send);
    return;
}

void protocol_jt_pack_gps_msg2(u8 *pdata, u16 *idx, u16 len)
{
    GPSData gps;

    GM_memset(&gps,0,sizeof(gps));
    gps_get_last_data(&gps);
    
    //use current time.
    gps.gps_time = util_get_utc_time();

    protocol_jt_pack_gps_msg(GPS_MODE_FIX_TIME, &gps, pdata, idx, len);

    return;
}

void protocol_jt_parse_msg(u8 *pdata, u16 len)
{
    u16 j;
    u8 check_sum=0;
    u16 cmd;

    //log_service_print_hex((const char *)pdata,len);
    if(GM_SUCCESS != protocol_jt_pack_unescape(pdata, &len))
    {
        return;
    }

    // check_sum 1byte  end_tag 1byte
    for(j=1;j<(len-2);j++)
    {
        check_sum ^= pdata[j];
    }

    if(check_sum != pdata[len-2])
    {
        LOG(WARN,"clock(%d) protocol_jt_parse_msg assert(checksum(%02x=%02x, %02x, %02x)) failed.", util_clock(), check_sum, pdata[len-3], pdata[len-2], pdata[len-1]);
        return;
    }

    cmd=((u16)pdata[1]<<8)+pdata[2];
    switch(cmd)
    {
        case JT_CMD_PLAT_ACK:
            LOG(DEBUG,"clock(%d) protocol_jt_parse_msg(JT_CMD_PLAT_ACK).", util_clock());
            protocol_jt_parse_plat_ack(pdata,len);
            break;
        case JT_CMD_REGISTER_ACK:
            LOG(DEBUG,"clock(%d) protocol_jt_parse_msg(JT_CMD_REGISTER_ACK).", util_clock());
            protocol_jt_parse_register_ack(pdata,len);
            break;
        case JT_CMD_SET_PARAM:
            LOG(DEBUG,"clock(%d) protocol_jt_parse_msg(JT_CMD_SET_PARAM).", util_clock());
            protocol_jt_parse_set_param(pdata,len);
            break;
        case JT_CMD_GET_PARAM:
            LOG(DEBUG,"clock(%d) protocol_jt_parse_msg(JT_CMD_GET_PARAM).", util_clock());
            protocol_jt_parse_get_param(pdata,len);
            break;
        case JT_CMD_TERM_CTL:
            LOG(DEBUG,"clock(%d) protocol_jt_parse_msg(JT_CMD_TERM_CTL).", util_clock());
            protocol_jt_parse_terminal_control(pdata,len);
            break;
        case JT_CMD_LOCATE_REQ:
            LOG(DEBUG,"clock(%d) protocol_jt_parse_msg(JT_CMD_LOCATE_REQ).", util_clock());
            protocol_jt_parse_gps_req(pdata,len);
            break;
        case JT_CMD_CONFIRM_ALARM:
            LOG(DEBUG,"clock(%d) protocol_jt_parse_msg(JT_CMD_CONFIRM_ALARM).", util_clock());
            protocol_jt_parse_general_msg(pdata,len);
            break;
        case JT_CMD_TEXT:
            LOG(DEBUG,"clock(%d) protocol_jt_parse_msg(JT_CMD_TEXT).", util_clock());
            protocol_jt_parse_text(pdata,len);
            break;
        default:
            LOG(DEBUG,"clock(%d) protocol_jt_parse_msg(%04x).", util_clock(),cmd);
            log_service_print_hex((const char *)pdata,len);
            protocol_jt_parse_general_msg(pdata,len);
            break;
    }
}

static void protocol_jt_parse_text(U8* pdata, u16 len)
{
    U8* pOut = NULL;
    u16 out_len = 0;
    u8 idx = 0;
    u16 msg_info;
    u16 msg_len;
    u8 msg_depart;
    u8 msg_sec;
    u8 data_start;

    msg_info = ((u16)pdata[3]<<8)+pdata[4];
    
    // 保留(15 14)  分包(13) 加密(12 11 10) 长度(9-0) 
    msg_len = msg_info & 0x3FF;
    msg_depart =(msg_info & JT_DATA_DEPART_FLAG)?1:0;
    msg_sec = (msg_info & JT_DATA_SEC_FLAG) >> 10;
    data_start = msg_depart ? 17: 13;
	msg_sec = msg_sec;

    if(len != (data_start + msg_len + 2))
    {
        LOG(INFO,"clock(%d) protocol_jt_parse_text assert(len(%d)).", util_clock(), len);
        return;
    }
    
    s_jt_msg_save.msg_id = ((u16)pdata[1]<<8)+pdata[2];
    s_jt_msg_save.server_serial = ((u16)pdata[11]<<8)+pdata[12];
    s_jt_msg_save.msg_result = 0;  //成功

    pOut = GM_MemoryAlloc(CMD_MAX_LEN + 1);
    if (NULL == pOut)
    {
        LOG(WARN,"protocol_concox_parse_remote_msg assert(GM_MemoryAlloc(%d)) failed.", CMD_MAX_LEN + 1);
        return;
    }
    
    GM_memset(pOut, 0x00, CMD_MAX_LEN + 1);

    // flag(1) command(N)
    idx = data_start + 1;
	command_on_receive_data(COMMAND_GPRS, (char *)&pdata[idx], msg_len - 1, (char *)pOut,NULL);
    out_len = GM_strlen((char *)pOut);
    
    if (out_len > 0)
    {
        gps_service_after_receive_remote_msg(pOut, out_len);
    }
    
    //发送ack   
    gps_service_after_server_req();
    GM_MemoryFree(pOut);
}

static void protocol_jt_parse_gps_req(U8* pdata, u16 len)
{
    u16 msg_info;
    u16 msg_len;
    u8 msg_depart;
    u8 msg_sec;
    u8 data_start;

    msg_info = ((u16)pdata[3]<<8)+pdata[4];
    
    // 保留(15 14)  分包(13) 加密(12 11 10) 长度(9-0) 
    msg_len = msg_info & 0x3FF;
    msg_depart =(msg_info & JT_DATA_DEPART_FLAG)?1:0;
    msg_sec = (msg_info & JT_DATA_SEC_FLAG) >> 10;
    data_start = msg_depart ? 17: 13;
	msg_sec = msg_sec;

    if(len != (data_start + msg_len + 2))
    {
        LOG(INFO,"clock(%d) protocol_jt_parse_gps_req assert(len(%d)).", util_clock(), len);
        return;
    }
    
    s_jt_msg_save.msg_id = ((u16)pdata[1]<<8)+pdata[2];
    s_jt_msg_save.server_serial = ((u16)pdata[11]<<8)+pdata[12];
    s_jt_msg_save.msg_result = 0;  //成功

    
    //发送定位数据
    gps_service_after_server_locate_req();
}

static void protocol_jt_parse_general_msg(U8* pdata, u16 len)
{
    u16 msg_info;
    u16 msg_len;
    u8 msg_depart;
    u8 msg_sec;
    u8 data_start;

    msg_info = ((u16)pdata[3]<<8)+pdata[4];
    
    // 保留(15 14)  分包(13) 加密(12 11 10) 长度(9-0) 
    msg_len = msg_info & 0x3FF;
    msg_depart =(msg_info & JT_DATA_DEPART_FLAG)?1:0;
    msg_sec = (msg_info & JT_DATA_SEC_FLAG) >> 10;
    data_start = msg_depart ? 17: 13;
	msg_sec = msg_sec;

    if(len != (data_start + msg_len + 2))
    {
        LOG(INFO,"clock(%d) protocol_jt_parse_general_msg assert(len(%d)).", util_clock(), len);
        return;
    }
    
    s_jt_msg_save.msg_id = ((u16)pdata[1]<<8)+pdata[2];
    s_jt_msg_save.server_serial = ((u16)pdata[11]<<8)+pdata[12];
    s_jt_msg_save.msg_result = 0;  //成功
    
    //发送ack   
    gps_service_after_server_req();
}

static void protocol_jt_parse_terminal_control(U8* pdata, u16 len)
{
    u16 msg_info;
    u16 msg_len;
    u8 msg_depart;
    u8 msg_sec;
    u8 data_start;
	GM_ERRCODE ret = GM_SUCCESS;
	
    msg_info = ((u16)pdata[3]<<8)+pdata[4];
    
    // 保留(15 14)  分包(13) 加密(12 11 10) 长度(9-0) 
    msg_len = msg_info & 0x3FF;
    msg_depart =(msg_info & JT_DATA_DEPART_FLAG)?1:0;
    msg_sec = (msg_info & JT_DATA_SEC_FLAG) >> 10;
    data_start = msg_depart ? 17: 13;
	msg_sec = msg_sec;

    if(len != (data_start + msg_len + 2))
    {
        LOG(INFO,"clock(%d) protocol_jt_parse_terminal_control assert(len(%d)).", util_clock(), len);
        return;
    }
    
    s_jt_msg_save.msg_id = ((u16)pdata[1]<<8)+pdata[2];
    s_jt_msg_save.server_serial = ((u16)pdata[11]<<8)+pdata[12];

    switch(pdata[data_start])
    {
        case 4: //终端复位
            ret = hard_ware_reboot(GM_REBOOT_CMD,1);
            LOG(DEBUG,"clock(%d) protocol_jt_parse_terminal_control hard_ware_reboot.", util_clock());
            break;
        case 0x64: //断油电 
            ret = hard_ware_set_relay(true);
            LOG(DEBUG,"clock(%d) protocol_jt_parse_terminal_control hard_ware_set_relay broke.", util_clock());
            break;
        case 0x65: //通油电
            ret = hard_ware_set_relay(false);
            LOG(DEBUG,"clock(%d) protocol_jt_parse_terminal_control hard_ware_set_relay good.", util_clock());
            break;
    }
    
    if(GM_SUCCESS == ret)
    {
        s_jt_msg_save.msg_result = 0;  //成功
    }
    else
    {
        s_jt_msg_save.msg_result = 1;  //失败
    }
    
    //发送ack   
    gps_service_after_server_req();
}

static void protocol_jt_parse_get_param(U8* pdata, u16 len)
{
    u16 msg_info;
    u16 msg_len;
    u8 msg_depart;
    u8 msg_sec;
    u8 data_start;

    msg_info = ((u16)pdata[3]<<8)+pdata[4];
    
    // 保留(15 14)  分包(13) 加密(12 11 10) 长度(9-0) 
    msg_len = msg_info & 0x3FF;
    msg_depart =(msg_info & JT_DATA_DEPART_FLAG)?1:0;
    msg_sec = (msg_info & JT_DATA_SEC_FLAG) >> 10;
    data_start = msg_depart ? 17: 13;
	msg_sec = msg_sec;

    if(len != (data_start + msg_len + 2))
    {
        LOG(INFO,"clock(%d) protocol_jt_parse_get_param assert(len(%d)).", util_clock(), len);
        return;
    }
    
    s_jt_msg_save.msg_id = ((u16)pdata[1]<<8)+pdata[2];
    s_jt_msg_save.server_serial = ((u16)pdata[11]<<8)+pdata[12];

    gps_service_after_param_get();
}

static void protocol_jt_parse_set_param(U8* pdata, u16 len)
{
    u16 msg_info = 0;
    u16 msg_len = 0;
    u8 msg_depart = 0;
    u8 msg_sec = 0;
    
    u8 data_start = 0;
    u8 data_count = 0;
    u8 para_idx = 0;
    u32 para_id = 0;
    u8 para_len = 0;

    u16 value_u16;
    u8 value_u8;
    u32 value_u32;
    u64 value_u64;
    
    s_jt_msg_save.msg_id = ((u16)pdata[1]<<8)+pdata[2];
    s_jt_msg_save.server_serial = ((u16)pdata[11]<<8)+pdata[12];
    msg_info = ((u16)pdata[3]<<8)+pdata[4];
    
    // 保留(15 14)  分包(13) 加密(12 11 10) 长度(9-0) 
    msg_len = msg_info & 0x3FF;
    msg_depart =(msg_info & JT_DATA_DEPART_FLAG)?1:0;
    msg_sec = (msg_info & JT_DATA_SEC_FLAG) >> 10;
    data_start = msg_depart ? 17: 13;
	msg_sec = msg_sec;

    if(len != (data_start + msg_len + 2))
    {
        LOG(INFO,"clock(%d) protocol_jt_parse_set_param assert(len(%d)).", util_clock(), len);
        return;
    }

    //参数总数
    data_count = pdata[data_start++];
    while(para_idx < data_count)
    {
        ++para_idx;
        //参数 ID(2)     参数长度(1)         参数值

        para_id = ((u32)pdata[data_start]<<24)+((u32)pdata[data_start+1]<<16)+((u32)pdata[data_start+2]<<8)+((u32)pdata[data_start+3]);
        para_len = pdata[data_start+4];
        data_start += 5;

        if(len < (data_start + para_len + 2))
        {
            LOG(INFO,"clock(%d) protocol_jt_parse_set_param assert(len(%d)).", util_clock(), len);
            return;
        }

        switch(para_id)
        {
            case JT_PARAM_HEART_INTERVAL:
                value_u16 = (u16)get_value_by_length(&pdata[data_start], para_len);
                if(value_u16 >= GOOME_HEARTBEAT_MIN && value_u16 <= GOOME_HEARTBEAT_MAX)
                {
                    config_service_set(CFG_HEART_INTERVAL, TYPE_SHORT, &value_u16, sizeof(value_u16));
                }
                else
                {
                    LOG(INFO,"clock(%d) protocol_jt_parse_set_param assert(JT_PARAM_HEART_INTERVAL(%d)) failed.", util_clock(), value_u16);
                }
                break;
            case JT_PARAM_APN:
                config_service_set(CFG_APN_NAME, TYPE_STRING, &pdata[data_start], para_len);
                break;
            case JT_PARAM_APN_USR:
                config_service_set(CFG_APN_USER, TYPE_STRING, &pdata[data_start], para_len);
                break;
            case JT_PARAM_APN_PWD:
                config_service_set(CFG_APN_PWD, TYPE_STRING, &pdata[data_start], para_len);
                break;
            case JT_PARAM_IP:
                config_service_get(CFG_SERVERLOCK, TYPE_BYTE, &value_u8, sizeof(value_u8));
                if(! value_u8)
                {
                    config_service_change_ip(CFG_SERVERADDR, &pdata[data_start], para_len);
                    gps_service_change_config();
                }
            case JT_PARAM_PORT:
                value_u16 = (u16)get_value_by_length(&pdata[data_start], para_len);
                config_service_change_port(CFG_SERVERADDR,value_u16);
                gps_service_change_config();
                break;
            case JT_PARAM_HEART_LOCATE_INTERVAL:
                value_u16 = (u16)get_value_by_length(&pdata[data_start], para_len);
                if(value_u16 >= GOOME_HEARTBEAT_MIN && value_u16 <= CONFIG_UPLOAD_TIME_MAX)
                {
                    config_service_set(CFG_JT_HBINTERVAL, TYPE_SHORT, &value_u16, sizeof(value_u16));
                }
                else
                {
                    LOG(INFO,"clock(%d) protocol_jt_parse_set_param assert(JT_PARAM_HEART_LOCATE_INTERVAL(%d)) failed.", util_clock(), value_u16);
                }
                break;
            case JT_PARAM_TIME_INTERVAL:
                value_u16 = (u16)get_value_by_length(&pdata[data_start], para_len);
                if(value_u16 >= CONFIG_UPLOAD_TIME_MIN && value_u16 <= CONFIG_UPLOAD_TIME_MAX)
                {
                    config_service_set(CFG_UPLOADTIME, TYPE_SHORT, &value_u16, sizeof(value_u16));
                }
                else
                {
                    LOG(INFO,"clock(%d) protocol_jt_parse_set_param assert(JT_PARAM_TIME_INTERVAL(%d)) failed.", util_clock(), value_u16);
                }
                break;
            case JT_PARAM_TURN_ANGLE:
                value_u16 = (u16)get_value_by_length(&pdata[data_start], para_len);
                if(value_u16 < 180)
                {
                    config_service_set(CFG_TURN_ANGLE, TYPE_SHORT, &value_u16, sizeof(value_u16));
                }
                else
                {
                    LOG(INFO,"clock(%d) protocol_jt_parse_set_param assert(JT_PARAM_TURN_ANGLE(%d)) failed.", util_clock(), value_u16);
                }
                break;
            case JT_PARAM_MAX_SPEED:
                value_u8 = (u8)get_value_by_length(&pdata[data_start], para_len);
                if(0 == value_u8)
                {
                    //只修改开关,不修改速度值
                    config_service_set(CFG_SPEED_ALARM_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));
                }
                else if(value_u8 >=5 && value_u8 <= 180)
                {
                    config_service_set(CFG_SPEEDTHR, TYPE_BYTE, &value_u8, sizeof(value_u8));

                    //打开开关
                    value_u8 = 1;
                    config_service_set(CFG_SPEED_ALARM_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));
                }
                else
                {
                    LOG(INFO,"clock(%d) protocol_jt_parse_set_param assert(JT_PARAM_MAX_SPEED(%d)) failed.", util_clock(), value_u8);
                }
                break;
            case JT_PARAM_EXCEED_SPEED_TIME:
                value_u8 = (u8)get_value_by_length(&pdata[data_start], para_len);
                config_service_set(CFG_SPEED_CHECK_TIME, TYPE_BYTE, &value_u8, sizeof(value_u8));
                break;
            case JT_PARAM_PROVINCE:
                value_u16 = (u16)get_value_by_length(&pdata[data_start], para_len);
                config_service_set(CFG_JT_PROVINCE, TYPE_SHORT, &value_u16, sizeof(value_u16));
                break;
            case JT_PARAM_CITY:
                value_u16 = (u16)get_value_by_length(&pdata[data_start], para_len);
                config_service_set(CFG_JT_CITY, TYPE_SHORT, &value_u16, sizeof(value_u16));
                break;
            case JT_PARAM_BRAND:
                value_u8 = (para_len > 12) ? 12 : para_len;
                config_service_set(CFG_JT_VEHICLE_NUMBER, TYPE_STRING, &pdata[data_start], value_u8);
                break;
            case JT_PARAM_BRAND_COLOR:
                value_u16 = (u16)get_value_by_length(&pdata[data_start], para_len);
                config_service_set(CFG_JT_VEHICLE_COLOR, TYPE_SHORT, &value_u16, sizeof(value_u16));
                break;
            case JT_PARAM_MILIAGE:
                value_u32 = get_value_by_length(&pdata[data_start], para_len);
                value_u64 = value_u32 * 100;
                system_state_set_mileage(value_u64);
                break;
            default:
                break;
        }

        data_start += para_len;
    }
    
    if(data_count > 0)
    {
        config_service_save_to_local();
        
        //发送ack   
        gps_service_after_server_req();
        
        gps_service_change_config();
    }
}

static void protocol_jt_parse_register_ack(U8* pdata, u16 len)
{
    u16 msg_info;
    u16 msg_len;
    u8 msg_depart;
    u8 msg_sec;
    u8 data_start;
    u8 auth_code[100];
    u8 value_u8;

    msg_info = ((u16)pdata[3]<<8)+pdata[4];
    
    // 保留(15 14)  分包(13) 加密(12 11 10) 长度(9-0) 
    msg_len = msg_info & 0x3FF;
    msg_depart =(msg_info & JT_DATA_DEPART_FLAG)?1:0;
    msg_sec = (msg_info & JT_DATA_SEC_FLAG) >> 10;
    data_start = msg_depart ? 17: 13;
	msg_sec = msg_sec;

    // {流水号2;结果1;鉴权码n;}checksum 1;flag 1;
    if(len != (data_start + msg_len + 2))
    {
        LOG(INFO,"clock(%d) protocol_jt_parse_register_ack assert(len(%d)).", util_clock(), len);
        return;
    }
    

    //0 注册成功   3 终端 已注册
    if(pdata[data_start + 2] == 0 || pdata[data_start + 2] == 3)
    {
        LOG(INFO,"clock(%d) protocol_jt_parse_register_ack regist ok(%d).", util_clock(), pdata[data_start + 2]);
        GM_memset(auth_code, 0x00, sizeof(auth_code));
        auth_code[0] = msg_len - 3; //为兼容,第一个字节是长度
        if(auth_code[0])
        {
            GM_memcpy(&auth_code[1], &pdata[data_start + 3], auth_code[0]);
            config_service_set(CFG_JT_AUTH_CODE, TYPE_STRING, &auth_code, auth_code[0] + 1);
        }

        //记录已注册成功
        value_u8 = 1;
        config_service_set(CFG_JT_ISREGISTERED, TYPE_BOOL, &value_u8, sizeof(value_u8));

        config_service_save_to_local();

        gps_service_after_register_response();
    }
    else
    {
        LOG(INFO,"clock(%d) protocol_jt_parse_register_ack regist failed(%d).", util_clock(), pdata[data_start + 2]);
    }
}


static void protocol_jt_parse_plat_ack(U8* pdata, u16 len)
{
    u16 msg_info;
    u16 msg_len;
    u8 msg_depart;
    u8 msg_sec;
    u8 data_start;
    u16 msg_id;

    msg_info = ((u16)pdata[3]<<8)+pdata[4];
    
    // 保留(15 14)  分包(13) 加密(12 11 10) 长度(9-0) 
    msg_len = msg_info & 0x3FF;
    msg_depart =(msg_info & JT_DATA_DEPART_FLAG)?1:0;
    msg_sec = (msg_info & JT_DATA_SEC_FLAG) >> 10;
    data_start = msg_depart ? 17: 13;
	msg_len = msg_len;
	msg_sec = msg_sec;

    // 流水号 2   消息号2 结果1 checksum 1 flag 1
    if(len != (data_start + msg_len + 2))
    {
        LOG(INFO,"clock(%d) protocol_jt_parse_plat_ack assert(len(%d)).", util_clock(), len);
        return;
    }
    
    msg_id =  ((u16)pdata[data_start + 2]<<8)+pdata[data_start + 3];
    switch(msg_id)
    {
        case JT_CMD_AUTH:
            // 0 成功,确认  ; 1失败; 2消息有误; 3不支持; 4报警;
            if(pdata[data_start + 4] == 0)
            {
                LOG(INFO,"clock(%d) protocol_jt_parse_plat_ack auth ok(%d).", util_clock(), pdata[4]);
                gps_service_after_login_response();
            }
            else
            {
                u8 value_u8 = 0;
                LOG(INFO,"clock(%d) protocol_jt_parse_plat_ack auth failed(%d).", util_clock(), pdata[4]);
                config_service_set(CFG_JT_ISREGISTERED, TYPE_BOOL, &value_u8, sizeof(value_u8));
                // config_service_save_to_local();  // not so important to write to disk
            }
            break;
        case JT_CMD_HEART_BEAT:
            gps_service_after_receive_heartbeat();
        case JT_CMD_UNREGISTER:
            break;
        case JT_CMD_LOCATE:
            gps_service_after_receive_heartbeat();
            break;
        case JT_CMD_LOCATE_MULTI:
            break;
    }
}



void protocol_jt_pack_remote_ack(u8 *pdata, u16 *idx, u16 len, u8 *pRet, u16 retlen)
{
    const char prefix_string[] = "返回:";
    u8 *send;
    u16 send_len = 0;
    u16 content_len = 0;
	send = (u8 *) GM_MemoryAlloc(len);
	if (send == NULL)
	{
        LOG(INFO,"clock(%d) protocol_jt_pack_remote_ack assert(GM_MemoryAlloc(%d)) failed.", util_clock(), len);
		return;
	}
    

    protocol_jt_pack_head(send, &send_len, len, 1, 0);  // 13bytes | 17bytes
    content_len=retlen + GM_strlen(prefix_string);
    content_len=GM_snprintf((char *)&send[send_len], content_len, "%s%s",prefix_string,pRet);
    send_len += content_len;
    if((send_len + 2) > len)
    {
        LOG(WARN,"clock(%d) protocol_jt_pack_remote_ack assert(len(%d)) failed content_len(%d) send_len(%d).", 
            util_clock(), len,content_len, send_len);
        GM_MemoryFree(send);
        return;
    }
    protocol_jt_pack_id_len(send, &send_len, JT_CMD_TEXT_RESULT, content_len, 0, false);  // 2bytes
    *idx = len;
    protocol_jt_pack_escape(send, send_len, pdata, idx);  

    GM_MemoryFree(send);
    return;
}

static void protocol_jt_pack_server_info(u8 *pdata, u16 *idx, u16 len, u16 *return_len)
{
    pdata[(*idx)++] = BHIGH_BYTE(s_jt_msg_save.server_serial);
    pdata[(*idx)++] = BLOW_BYTE(s_jt_msg_save.server_serial);
    pdata[(*idx)++] = BHIGH_BYTE(s_jt_msg_save.msg_id);
    pdata[(*idx)++] = BLOW_BYTE(s_jt_msg_save.msg_id);
    pdata[(*idx)++] = s_jt_msg_save.msg_result;

    *return_len = 5;
}

void protocol_jt_pack_general_ack(u8 *pdata, u16 *idx, u16 len)
{
    u8 *send;
    u16 send_len = 0;
    u16 content_len = 0;
	send = (u8 *) GM_MemoryAlloc(len);
	if (send == NULL)
	{
        LOG(INFO,"clock(%d) protocol_jt_pack_general_ack assert(GM_MemoryAlloc(%d)) failed.", util_clock(), len);
		return;
	}
    
    protocol_jt_pack_head(send, &send_len, len, 1, 0);  // 13bytes | 17bytes
    protocol_jt_pack_server_info(send, &send_len, len, &content_len);  //5 bytes
    if((send_len + 2) > len)
    {
        LOG(WARN,"clock(%d) protocol_jt_pack_general_ack assert(len(%d)) failed.", util_clock(), len);
        GM_MemoryFree(send);
        return;
    }
    protocol_jt_pack_id_len(send, &send_len, JT_CMD_TERM_ACK, content_len, 0, false);  // 2bytes
    *idx = len;
    protocol_jt_pack_escape(send, send_len, pdata, idx);  

    GM_MemoryFree(send);
    return;
}

static void protocol_jt_pack_item_num(u8 *pdata, u16 *idx, u32 item, u32 value, u8 item_len)
{
    u16 value_u16;
    u8 value_u8;
    
    pdata[(*idx)++] = BHIGH_BYTE(WHIGH_WORD(item));
    pdata[(*idx)++] = BLOW_BYTE(WHIGH_WORD(item));
    pdata[(*idx)++] = BHIGH_BYTE(WLOW_WORD(item));
    pdata[(*idx)++] = BLOW_BYTE(WLOW_WORD(item));
    pdata[(*idx)++] = item_len;
    
    if(item_len == 4)
    {
        pdata[(*idx)++] = BHIGH_BYTE(WHIGH_WORD(value));
        pdata[(*idx)++] = BLOW_BYTE(WHIGH_WORD(value));
        pdata[(*idx)++] = BHIGH_BYTE(WLOW_WORD(value));
        pdata[(*idx)++] = BLOW_BYTE(WLOW_WORD(value));
    }
    else if (item_len == 2)
    {
        value_u16 = value;
        pdata[(*idx)++] = BHIGH_BYTE(value_u16);
        pdata[(*idx)++] = BLOW_BYTE(value_u16);
    }
    else // if (item_len == 1)
    {
        value_u8 = value;
        pdata[(*idx)++] = value_u8;
    }
    
    //(*idx) += item_len;
}


static void protocol_jt_pack_item_string(u8 *pdata, u16 *idx, u32 item, u8 *pitem, u8 item_len)
{
    pdata[(*idx)++] = BHIGH_BYTE(WHIGH_WORD(item));
    pdata[(*idx)++] = BLOW_BYTE(WHIGH_WORD(item));
    pdata[(*idx)++] = BHIGH_BYTE(WLOW_WORD(item));
    pdata[(*idx)++] = BLOW_BYTE(WLOW_WORD(item));
    pdata[(*idx)++] = item_len;
    
    GM_memcpy(&pdata[(*idx)], pitem, item_len);
    (*idx) += item_len;
}

static void protocol_jt_pack_param_info(u8 *pdata, u16 *idx, u16 len, u16 *return_len)
{
    u16 orginal_idx = *idx;
    u16 value_u16;
    u8 value_u8;
    u8 value_str[26];
    u32 value_u32 = 0;
    
    pdata[(*idx)++] = BHIGH_BYTE(s_jt_msg_save.server_serial);
    pdata[(*idx)++] = BLOW_BYTE(s_jt_msg_save.server_serial);
    pdata[(*idx)++] = 40;  // total 40 items

    config_service_get(CFG_HEART_INTERVAL, TYPE_SHORT, &value_u16, sizeof(value_u16));
    protocol_jt_pack_item_num(pdata, idx, JT_PARAM_HEART_INTERVAL, value_u16, 4);
    
    protocol_jt_pack_item_num(pdata, idx, 0x0002, 0, 4);
    protocol_jt_pack_item_num(pdata, idx, 0x0003, 0, 4);
    protocol_jt_pack_item_num(pdata, idx, 0x0004, 0, 4);
    protocol_jt_pack_item_num(pdata, idx, 0x0005, 0, 4);

    
    protocol_jt_pack_item_num(pdata, idx, 0x0006, 0, 4);
    protocol_jt_pack_item_num(pdata, idx, 0x0007, 0, 4);

    config_service_get(CFG_APN_NAME, TYPE_STRING, value_str, sizeof(value_str));
    protocol_jt_pack_item_string(pdata, idx, JT_PARAM_APN, value_str, GM_strlen((const char *)value_str));

    protocol_jt_pack_item_string(pdata, idx, 0x0011, value_str, 0);
    protocol_jt_pack_item_string(pdata, idx, 0x0012, value_str, 0);

    
    protocol_jt_pack_item_string(pdata, idx, 0x0013, value_str, 0);
    protocol_jt_pack_item_string(pdata, idx, 0x0014, value_str, 0);
    protocol_jt_pack_item_string(pdata, idx, 0x0015, value_str, 0);
    protocol_jt_pack_item_string(pdata, idx, 0x0016, value_str, 0);
    protocol_jt_pack_item_string(pdata, idx, 0x0017, value_str, 0);


    protocol_jt_pack_item_num(pdata, idx, 0x0018, 0, 4);
    protocol_jt_pack_item_num(pdata, idx, 0x0019, 0, 4);
    protocol_jt_pack_item_num(pdata, idx, 0x0020, 0, 4);
    protocol_jt_pack_item_num(pdata, idx, 0x0021, 0, 4);
    protocol_jt_pack_item_num(pdata, idx, 0x0022, 0, 4);

    config_service_get(CFG_JT_HBINTERVAL, TYPE_SHORT, &value_u16, sizeof(value_u16));
    protocol_jt_pack_item_num(pdata, idx, JT_PARAM_HEART_LOCATE_INTERVAL, value_u16, 4);

    protocol_jt_pack_item_num(pdata, idx, 0x0028, 0, 4);

    config_service_get(CFG_UPLOADTIME, TYPE_SHORT, &value_u16, sizeof(value_u16));
    protocol_jt_pack_item_num(pdata, idx, JT_PARAM_TIME_INTERVAL, value_u16, 4);

    config_service_get(CFG_TURN_ANGLE, TYPE_SHORT, &value_u16, sizeof(value_u16));
    protocol_jt_pack_item_num(pdata, idx, JT_PARAM_TURN_ANGLE, value_u16, 4);

    config_service_get(CFG_SPEEDTHR, TYPE_BYTE, &value_u8, sizeof(value_u8));
    protocol_jt_pack_item_num(pdata, idx, JT_PARAM_MAX_SPEED, value_u8, 4);

    config_service_get(CFG_SPEED_CHECK_TIME, TYPE_BYTE, &value_u8, sizeof(value_u8));
    protocol_jt_pack_item_num(pdata, idx, JT_PARAM_EXCEED_SPEED_TIME, value_u16, 4);

    protocol_jt_pack_item_num(pdata, idx, 0x0057, 0, 4);
    protocol_jt_pack_item_num(pdata, idx, 0x0058, 0, 4);
    protocol_jt_pack_item_num(pdata, idx, 0x0059, 0, 4);
    protocol_jt_pack_item_num(pdata, idx, 0x005A, 0, 4);

    
    protocol_jt_pack_item_num(pdata, idx, 0x0070, 0, 4);
    protocol_jt_pack_item_num(pdata, idx, 0x0071, 0, 4);
    protocol_jt_pack_item_num(pdata, idx, 0x0072, 0, 4);
    protocol_jt_pack_item_num(pdata, idx, 0x0073, 0, 4);
    protocol_jt_pack_item_num(pdata, idx, 0x0074, 0, 4);

    value_u32 = system_state_get_mileage()/100;
    protocol_jt_pack_item_num(pdata, idx, JT_PARAM_MILIAGE, value_u32, 4);

    config_service_get(CFG_JT_PROVINCE, TYPE_SHORT, &value_u16, sizeof(value_u16));
    protocol_jt_pack_item_num(pdata, idx, JT_PARAM_PROVINCE, value_u16, 4);

    config_service_get(CFG_JT_CITY, TYPE_SHORT, &value_u16, sizeof(value_u16));
    protocol_jt_pack_item_num(pdata, idx, JT_PARAM_CITY, value_u16, 4);

    config_service_get(CFG_JT_VEHICLE_NUMBER, TYPE_STRING, value_str, sizeof(value_str));
    protocol_jt_pack_item_string(pdata, idx, JT_PARAM_BRAND, value_str, GM_strlen((const char *)value_str));

    config_service_get(CFG_JT_VEHICLE_COLOR, TYPE_BYTE, &value_u8, sizeof(value_u8));
    protocol_jt_pack_item_num(pdata, idx, JT_PARAM_BRAND_COLOR, value_u8, 4);
    
    *return_len = (*idx) - orginal_idx;
}

void protocol_jt_pack_param_ack(u8 *pdata, u16 *idx, u16 len)
{
    u8 *send;
    u16 send_len = 0;
    u16 content_len = 0;
	send = (u8 *) GM_MemoryAlloc(len);
	if (send == NULL)
	{
        LOG(INFO,"clock(%d) protocol_jt_pack_general_ack assert(GM_MemoryAlloc(%d)) failed.", util_clock(), len);
		return;
	}
    

    protocol_jt_pack_head(send, &send_len, len, 1, 0);  // 13bytes | 17bytes
    protocol_jt_pack_param_info(send, &send_len, len, &content_len);  //5 bytes
    if((send_len + 2) > len)
    {
        LOG(WARN,"clock(%d) protocol_jt_pack_general_ack assert(len(%d)) failed.", util_clock(), len);
        GM_MemoryFree(send);
        return;
    }
    protocol_jt_pack_id_len(send, &send_len, JT_CMD_GET_PARAM_ACK, content_len, 0, false);  // 2bytes
    *idx = len;
    protocol_jt_pack_escape(send, send_len, pdata, idx);  

    GM_MemoryFree(send);
    return;
}


