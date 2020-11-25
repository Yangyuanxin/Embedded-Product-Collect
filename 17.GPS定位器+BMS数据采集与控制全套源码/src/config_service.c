#include "gm_type.h"
#include "stdio.h"
#include "gm_stdlib.h"
#include "gm_gprs.h"
#include "fifo.h"
#include "utility.h"
#include "gsm.h"
#include "gm_memory.h"
#include "config_service.h"
#include "gps_service.h"
#include "log_service.h"
#include "agps_service.h"
#include "update_service.h"
#include "g_sensor.h"
#include "auto_test.h"
#include "gprs.h"

#define CONFIG_PING_TIME  43200

typedef struct
{
    u32 last_ok_time;   //上一次执行配置检测的时间
    u32 wait;           //执行配置检测的间隔
}ConfigServiceExtend;

static SocketType s_config_socket = {-1,"",SOCKET_STATUS_ERROR,};
static ConfigServiceExtend s_config_socket_extend;

static ConfigParamItems s_configs[CFG_PARAM_MAX];


typedef struct
{
    char cmd_string[CONFIG_CMD_MAX_LEN+1];
    u16 index;
}ConfigParamStruct;

/*****************************************
DeviceType	0	终端型号
ServerAddr	1	域名端口
ServerLock	2	是否锁死平台
Protocol	3	协议转换
ProtoVer	4	协议版本号
HBProtocol	5	心跳协议
UploadTime	6	上传时间间隔
HBTS	    7	心跳时间间隔
LBS	        8	上传基站定位
AngleTime	9	角度上报时间
MoveAlarm	10	车辆移动报警
BattUpload	11	电量上传
CutoffALM	12	断电报警
LowBattALM	13	低电报警
SpeedTHR	14	超速阈值(km/h)
AclrAlarm	15	加速度报警
ShakeAlarm	16	震动报警开关
ShakeCount	17	震动报警检测次数
ShakeTime	18	震动检测时间
VibSetWay	19	是否手动设防
DefenceDel	20	自动设防时间
ShakeDelay	21	震动报警延时
ShakeInter	22	震动报警间隔
Lang	    23	语言
Zone	    24	时区
AccTHR	    25	急加速（g）
TurnTHR	    26	急转弯（g）
BrakeTHR	27	急减速（g）
CrashTHR_L	28	轻微碰撞（g）
CrashTHR_M	29	一般碰撞（g）
CrashTHR_H	30	严重碰撞（g）
*****************************************/
static const ConfigParamStruct s_config_param[] = 
{
	{"DEVICETYPE",                      CFG_DEVICETYPE},	
    {"SERVERADDR",                      CFG_SERVERADDR},
    {"SERVERLOCK",                      CFG_SERVERLOCK},
    {"PROTOCOL",                        CFG_PROTOCOL},
    {"PROTOVER",                        CFG_PROTOCOL_VER},
    {"HBPROTOCOL",                      CFG_HBPROTOCOL},
    {"UPLOADTIME",                      CFG_UPLOADTIME},
    {"HBTS",                            CFG_HEART_INTERVAL},
    {"SENDS",                           CFG_SLEEP_TIME},
    {"LBS",                             CFG_IS_LBS_ENABLE},
    {"ANGLETIME",                       CFG_SEN_ANGLE_SHOW_TIME},
    {"MOVEALARM",                       CFG_IS_MOVEALARM_ENABLE},
    {"BATTUPLOAD",                      CFG_BATTUPLOAD_DISABLE},
    {"CUTOFFALM",                       CFG_CUTOFFALM_DISABLE},
    {"LOWBATTALM",                      CFG_LOWBATTALM_DISABLE},
    {"SPEEDTHR",                        CFG_SPEEDTHR},
    {"ACLRALARM",                       CFG_IS_ACLRALARM_ENABLE},
	{"SHAKEALARM",                      CFG_IS_SHAKE_ALARM_ENABLE},
	{"SHAKECOUNT",                      CFG_SHAKE_COUNT},
	{"SHAKETIME",                       CFG_SHAKE_CHECK_TIME},
	{"VIBSETWAY",                       CFG_IS_MANUAL_DEFENCE},
	{"DEFDELAY",                        CFG_AUTO_DEFENCE_DELAY},
	{"SHAKEDELAY",                      CFG_SHAKE_ALARM_DELAY},
	{"SHAKEINTER",                      CFG_SHAKE_ALARM_INTERVAL},
	{"LANG",                            CFG_LANGUAGE},
	{"ZONE",                            CFG_TIME_ZONE}, 
    {"ACCTHR",                          CFG_SEN_RAPID_ACLR},
    {"TURNTHR",                         CFG_SEN_SUDDEN_TURN},
    {"BRAKETHR",                        CFG_SEN_EMERGENCY_BRAKE},
    {"CRASHTHR_L",                      CFG_SEN_SLIGHT_COLLISION},
    {"CRASHTHR_M",                      CFG_SEN_NORMAL_COLLISION},
    {"CRASHTHR_H",                      CFG_SEN_SERIOUS_COLLISION},
	{"SMOOTH",                          CFG_SMOOTH_TRACK},
	{"MINSNR",                          CFG_MIN_SNR},
};

typedef struct
{
    char device_str[10];
    u16 idx;
}ConfigDeviceTypeStruct;

static const ConfigDeviceTypeStruct s_config_device[DEVICE_MAX] = 
{
    {"UNKNOW",                                      DEVICE_NONE},
    {"GS03A",                                       DEVICE_GS03A},
    {"AS03A",                                       DEVICE_AS03A},
    {"GS03B",                                       DEVICE_GS03B},
    {"AS03B",                                       DEVICE_AS03B},
    {"GS03F",                                       DEVICE_GS03F},
    {"AS03F",                                       DEVICE_AS03F},
    {"GS07A",                                       DEVICE_GS07A},
    {"AS07A",                                       DEVICE_AS07A},
    {"GS07B",                                       DEVICE_GS07B},
    {"AS07B",                                       DEVICE_AS07B},
    {"GS03I",                                       DEVICE_GS03I},
    {"AS03I",                                       DEVICE_AS03I},
    {"GS03H",                                       DEVICE_GS03H},
    {"GS05A",                                       DEVICE_GS05A},
    {"GS05B",                                       DEVICE_GS05B},
    {"GS05F",                                       DEVICE_GS05F},
    {"GS05I",                                       DEVICE_GS05I},
    {"GS05H",                                       DEVICE_GS05H},
    {"GM06E",                                       DEVICE_GM06E},
    {"GS10",										DEVICE_GS10},
};

static GM_ERRCODE config_service_transfer_status(u8 new_status);
static void config_service_init_proc(void);
static void config_service_connecting_proc(void);
static void config_service_work_proc(void);
static void config_service_finish_proc(void);
static void config_service_send_request_failed(void);
static void config_srevice_create_other_services(void);
static void config_service_close(void);
static GM_ERRCODE config_msg_upload(u8 cmd, const u8 *msg, u16 len);
static void config_msg_request(void);
static void config_msg_response(void);
static void config_msg_receive(SocketType *socket);
static void config_msg_content_parse(u8 *pdata, u16 len);
static u16 config_msg_get_item(const ConfigParamDataType data_type, u8 *pOut, const u8 *pSrc, u8 *len);
static void config_msg_param_set(u16 index, u8 *pMsg, u8 len);
static void config_service_device_type(u16 index, u8 *pMsg, u8 len);
static void config_service_set_range_word(u16 index, u8 *pMsg, u8 len, u32 min, u32 max);
static void config_service_set_protocol(u16 index, u8 *pMsg, u8 len);
static void config_service_set_app_protocol(u16 index, u8 *pMsg, u8 len);
static void config_service_set_addr(u16 index, u8 *pMsg, u8 len);
static void config_service_heart_protocol(u16 index, u8 *pMsg, u8 len);
static void config_service_gsensor_set_float(u16 index, u8 *pMsg, u8 len, float min, float max);
static void config_service_finish(u32 wait);
static void config_service_retry_config(void);

static GM_ERRCODE config_service_transfer_status(u8 new_status)
{
    u8 old_status = (u8)s_config_socket.status;
    GM_ERRCODE ret = GM_PARAM_ERROR;
    switch(s_config_socket.status)
    {
        case SOCKET_STATUS_INIT:
            switch(new_status)
            {
                case SOCKET_STATUS_INIT:
                    ret = GM_SUCCESS;
                    break;
                case SOCKET_STATUS_GET_HOST:
                    ret = GM_SUCCESS;
                    break;
                case SOCKET_STATUS_CONNECTING:
                    ret = GM_SUCCESS;
                    break;
                case SOCKET_STATUS_WORK:
                    break;
                case SOCKET_STATUS_DATA_FINISH:
                    ret = GM_SUCCESS;
                    break;
                case SOCKET_STATUS_ERROR:
                    ret = GM_SUCCESS;
                    break;
                default:
                    break;
            }
            break;
        case SOCKET_STATUS_GET_HOST:
            switch(new_status)
            {
                case SOCKET_STATUS_INIT:
                    ret = GM_SUCCESS;
                    break;
                case SOCKET_STATUS_GET_HOST:
                    break;
                case SOCKET_STATUS_CONNECTING:
                    ret = GM_SUCCESS;
                    break;
                case SOCKET_STATUS_WORK:
                    break;
                case SOCKET_STATUS_DATA_FINISH:
                    ret = GM_SUCCESS;
                    break;
                case SOCKET_STATUS_ERROR:
                    ret = GM_SUCCESS;
                    break;
                default:
                    break;
            }
            break;
        case SOCKET_STATUS_CONNECTING:
            switch(new_status)
            {
                case SOCKET_STATUS_INIT:
                    ret = GM_SUCCESS;
                    break;
                case SOCKET_STATUS_GET_HOST:
                    ret = GM_SUCCESS;
                    break;
                case SOCKET_STATUS_CONNECTING:
                    break;
                case SOCKET_STATUS_WORK:
                    ret = GM_SUCCESS;
                    break;
                case SOCKET_STATUS_DATA_FINISH:
                    ret = GM_SUCCESS;
                    break;
                case SOCKET_STATUS_ERROR:
                    ret = GM_SUCCESS;
                    break;
                default:
                    break;
            }
            break;
        case SOCKET_STATUS_WORK:
            switch(new_status)
            {
                case SOCKET_STATUS_INIT:
                    ret = GM_SUCCESS;
                    break;
                case SOCKET_STATUS_GET_HOST:
                    ret = GM_SUCCESS;
                    break;
                case SOCKET_STATUS_CONNECTING:
                    break;
                case SOCKET_STATUS_WORK:
                    break;
                case SOCKET_STATUS_DATA_FINISH:
                    ret = GM_SUCCESS;
                    break;
                case SOCKET_STATUS_ERROR:
                    ret = GM_SUCCESS;
                    break;
                default:
                    break;
            }
            break;
        case SOCKET_STATUS_DATA_FINISH:
            switch(new_status)
            {
                case SOCKET_STATUS_INIT:
                    ret = GM_SUCCESS;
                    break;
                case SOCKET_STATUS_GET_HOST:
                    ret = GM_SUCCESS;
                    break;
                case SOCKET_STATUS_CONNECTING:
                    break;
                case SOCKET_STATUS_WORK:
                    break;
                case SOCKET_STATUS_DATA_FINISH:
                    break;
                case SOCKET_STATUS_ERROR:
                    ret = GM_SUCCESS;
                    break;
                default:
                    break;
            }
            break;
        case SOCKET_STATUS_ERROR:
            switch(new_status)
            {
                case SOCKET_STATUS_INIT:
                    break;
                case SOCKET_STATUS_GET_HOST:
                    break;
                case SOCKET_STATUS_CONNECTING:
                    break;
                case SOCKET_STATUS_WORK:
                    break;
                case SOCKET_STATUS_DATA_FINISH:
                    break;
                case SOCKET_STATUS_ERROR:
                    ret = GM_SUCCESS;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }


    if(GM_SUCCESS == ret)
    {
        s_config_socket.status = new_status;
        s_config_socket.status_fail_count = 0;
        LOG(INFO,"clock(%d) config_service_transfer_status from %s to %s success", util_clock(), 
            gm_socket_status_string((SocketStatus)old_status), gm_socket_status_string((SocketStatus)new_status));
    }
    else
    {
        LOG(INFO,"clock(%d) config_service_transfer_status from %s to %s failed", util_clock(), 
            gm_socket_status_string((SocketStatus)old_status), gm_socket_status_string((SocketStatus)new_status));
    }

    return ret;

}

/*
// have ip, use ip to connect
// no ip, call gm_socket_get_host_by_name 
*/
static void config_service_init_proc(void)
{
    u8 IP[4];
    gm_socket_get_host_by_name_trigger(&s_config_socket);
    system_state_get_ip_cache(SOCKET_INDEX_CONFIG, IP);
    if(GM_SUCCESS == gm_is_valid_ip(IP))
    {
        GM_memcpy( s_config_socket.ip , IP, sizeof(IP));
        config_service_transfer_status(SOCKET_STATUS_CONNECTING);
        if(GM_SUCCESS == gm_socket_connect(&s_config_socket))
        {
        }
        // else do nothing .   connecting_proc will deal.
    }
    else if((!s_config_socket.excuted_get_host) && (GM_SUCCESS == gm_is_valid_ip(s_config_socket.ip)))
    {
        config_service_transfer_status(SOCKET_STATUS_CONNECTING);
        if(GM_SUCCESS == gm_socket_connect(&s_config_socket))
        {
        }
        // else do nothing .   connecting_proc will deal.
    }
}


static void config_service_connecting_proc(void)
{
    u32 current_time = util_clock();

    if((current_time - s_config_socket.send_time) > CONNECT_TIME_OUT)
    {
        s_config_socket.status_fail_count ++;
        config_service_connection_failed();

        if(s_config_socket.status == SOCKET_STATUS_CONNECTING &&
            s_config_socket.status_fail_count < MAX_CONNECT_REPEAT)
        {
            if(GM_SUCCESS == gm_socket_connect(&s_config_socket))
            {
                //do nothing. wait callback
            }
        }
    }
}


static GM_ERRCODE config_msg_upload(u8 cmd, const u8 *msg, u16 len)
{
    u16 k, msglen;
    u8 csm=0;
    u8 buff[50];
    u8 imei[GM_IMEI_LEN + 1];
    GM_ERRCODE ret;

    GM_memset(buff, 0x00, sizeof(buff));
    
    buff[0] = 0x66; 
    buff[1] = 0x66;
    buff[2] = cmd;
    buff[3] = 0x00;
    buff[4] = 0x08;
    
    msglen = 5;

    if(GM_SUCCESS != (ret = gsm_get_imei(imei)))
    {
        LOG(INFO,"clock(%d) config_msg_upload can not get imei, ret:%d.", util_clock(), ret);
        return ret;
    }
    
    buff[msglen++] = util_chr(imei[0]);
    buff[msglen++] = MERGEBCD(util_chr(imei[1]), util_chr(imei[2]));
    buff[msglen++] = MERGEBCD(util_chr(imei[3]), util_chr(imei[4]));
    buff[msglen++] = MERGEBCD(util_chr(imei[5]), util_chr(imei[6]));
    buff[msglen++] = MERGEBCD(util_chr(imei[7]), util_chr(imei[8]));
    buff[msglen++] = MERGEBCD(util_chr(imei[9]), util_chr(imei[10]));
    buff[msglen++] = MERGEBCD(util_chr(imei[11]), util_chr(imei[12]));
    buff[msglen++] = MERGEBCD(util_chr(imei[13]), util_chr(imei[14]));
    
    if ((len > 0) && (NULL != msg))
    {
        GM_memcpy(&buff[msglen], msg, len);
        msglen += len;
    }
    
    buff[3] = UPPER_BYTE(len + 8);
    buff[4] = LOWER_BYTE(len + 8);
    
    for (k=0; k<msglen; k++)
    {
        csm ^= buff[k];
    }
    
    buff[msglen++] = csm;
    buff[msglen++] = 0x0D;

    LOG(DEBUG,"clock(%d) config_msg_upload msglen(%d)", util_clock(), msglen);
    ret = gm_socket_send(&s_config_socket,buff, msglen);
    return ret;
}



static void config_msg_request(void)
{
    U8 msg[1];
    u32 current_time = util_clock();
	
    msg[0] = CFG_CMD_REQ_ALL;  // req all 

    s_config_socket.send_time = current_time; 
    if(GM_SUCCESS != config_msg_upload(CFG_CMD_REQ, msg,1))
    {
        config_service_finish(CONFIG_PING_TIME);
    }
}

static void config_msg_response(void)
{
    U8 msg[1];
	
    msg[0] = CFG_CMD_REQ_ALL;  // req all 

    config_msg_upload(CFG_CMD_RESULT, msg,1); //ignore send fail
}


static void config_msg_receive(SocketType *socket)
{
    // parse buf msg
    // if OK, after creating other socket, transfer to finish
    // not ok, ignore msg.
    u8 checksum = 0;
    u8 cmd = 0;
    u8 idx = 0;
    u8 jval = 0;
    u8 resp_imei[GM_IMEI_LEN + 1];
    u16 index = 0;

    #define CONFIG_MSG_HEAD_LEN 5
    #define CONFIG_MSG_TAIL_LEN 2
    u8 head[CONFIG_MSG_HEAD_LEN];
    u8 *pdata;
    u32 msg_len = sizeof(head);
    //u8 *buf;

    GM_ERRCODE ret;
    u8 imei[GM_IMEI_LEN + 1];
    static u32 packet_error_start = 0;

    GM_memset(resp_imei, 0, sizeof(resp_imei));
    
    if(SOCKET_STATUS_WORK != socket->status)
    {
        return;
    }
    
    //get head then body
    if(GM_SUCCESS != fifo_peek(&socket->fifo, head, msg_len))
    {
        return;
    }
    
    msg_len = MKWORD(head[3], head[4]);
    msg_len = msg_len + CONFIG_MSG_HEAD_LEN + CONFIG_MSG_TAIL_LEN;

    if(msg_len > MAX_GPRS_MESSAGE_LEN)
    {
        LOG(WARN,"clock(%d) config_msg_receive assert(msg_len(%d)) failed.",util_clock(), msg_len);
        //clear fifo and restart socket.
        fifo_reset(&socket->fifo);
        gm_socket_close_for_reconnect(socket);
        return;
    }

    pdata = GM_MemoryAlloc(msg_len);
    if(pdata == NULL)
    {
        LOG(WARN,"clock(%d) config_msg_receive assert(GM_MemoryAlloc(%d)) failed.",util_clock(), msg_len);

        //clear fifo and restart socket.
        fifo_reset(&socket->fifo);
        gm_socket_close_for_reconnect(socket);
        return;
    }
    
    if(GM_SUCCESS != fifo_peek(&socket->fifo, pdata, msg_len))
    {
        GM_MemoryFree(pdata);
        
        if(packet_error_start == 0)
        {
            LOG(DEBUG,"clock(%d) config_msg_receive get msg failed. len:%d", util_clock(), msg_len);
            log_service_print_hex((const char*)head, sizeof(head));
            packet_error_start = util_clock();
        }
        else
        {
            if((util_clock() - packet_error_start) > MAX_GPRS_PART_MESSAGE_TIMEOUT)
            {
                LOG(WARN,"clock(%d) config_msg_receive MAX_GPRS_PART_MESSAGE_TIMEOUT.",util_clock());
                //clear fifo and restart socket.
                fifo_reset(&socket->fifo);
                gm_socket_close_for_reconnect(socket);
                packet_error_start = 0;
            }
        }
        return;
    }

    fifo_pop_len(&socket->fifo, msg_len);

	LOG(DEBUG,"clock(%d) config_msg_receive msg fifo(%p) len(%d)", 
		util_clock(), &socket->fifo, msg_len);

    if(GM_SUCCESS != (ret = gsm_get_imei(imei)))
    {
        LOG(INFO,"clock(%d) config_msg_receive can not get imei, ret:%d.", util_clock(), ret);
        GM_MemoryFree(pdata);
        return;
    }

    for (index=0; index<msg_len-2; index++)
    {
        checksum = checksum^pdata[index];
    }
    
    if (checksum != pdata[msg_len-2])
    {
        LOG(INFO,"clock(%d) config_msg_receive checksum err. %d != %d", util_clock(), pdata[msg_len-2], checksum);
        GM_MemoryFree(pdata);
        return;
    }

    GM_memset(resp_imei, 0x00, sizeof(resp_imei));
    
    cmd = pdata[2];
    switch(cmd)
    {
        case CFG_CMD_ACK:
            index = 3;
            index += 2;

            resp_imei[0] = util_asc(pdata[index], 'x');
            for (idx=1,jval=1; jval<8; idx+=2,jval++)
            {
                resp_imei[idx] = util_asc((pdata[index+jval] >> 4) & 0x0F, 'x');
                resp_imei[idx+1] = util_asc(pdata[index+jval] & 0x0F, 'x');
            }

            if (0 != GM_memcmp(resp_imei, imei, GM_strlen((const char *)imei)))
            {
                LOG(INFO,"clock(%d) config_msg_receive imei err. %s != %s", util_clock(), resp_imei, imei);
                break;
            }
            
            index += 8;
            if (0x00 != pdata[index])
            {
	            config_msg_content_parse(&pdata[index+1], msg_len - index-2);
	            config_service_save_to_local();
            }

            config_msg_response();
            config_service_finish(CONFIG_PING_TIME);
            break;
                        
        default:
            break;
    }

    GM_MemoryFree(pdata);
}


/*
    get msg from fifo
*/
static void config_service_work_proc(void)
{
    u32 current_time = util_clock();

    if((current_time - s_config_socket.send_time) > MESSAGE_TIME_OUT)
    {
        s_config_socket.status_fail_count ++;
        config_service_send_request_failed();

        if(s_config_socket.status == SOCKET_STATUS_WORK &&
            s_config_socket.status_fail_count < MAX_CONNECT_REPEAT)
        {
            config_msg_request();
        }
    }

    config_msg_receive(&s_config_socket);
}


static void config_service_retry_config(void)
{
    u32 current_time = util_clock();

    if((current_time - s_config_socket_extend.last_ok_time) > s_config_socket_extend.wait)
    {
        LOG(DEBUG,"clock(%d) config_service_finish_proc cur(%d) - lastok(%d) > wait(%d).",
            util_clock(), current_time, s_config_socket_extend.last_ok_time,s_config_socket_extend.wait);
        s_config_socket_extend.last_ok_time = 0;
        s_config_socket_extend.wait = CONFIG_PING_TIME;
        config_service_transfer_status(SOCKET_STATUS_INIT);
    }

}



static void config_service_finish_proc(void)
{
    if(!gprs_is_ok())
    {
        return;
    }

    config_srevice_create_other_services();

    config_service_retry_config();
}



void config_service_connection_ok(void)
{
    config_service_transfer_status(SOCKET_STATUS_WORK);
    config_msg_request();
}

void config_service_close_for_reconnect(void)
{
    config_service_destroy();
}


void config_service_connection_failed(void)
{
    config_service_close();
    
    if(s_config_socket.status_fail_count >= MAX_CONNECT_REPEAT)
    {
        // if excuted get_host transfer to error statu, else get_host.
        if(s_config_socket.excuted_get_host || (s_config_socket.addr[0] == 0))
        {
            config_service_finish(CONFIG_PING_TIME);
        }
        else
        {
            config_service_transfer_status(SOCKET_STATUS_INIT);
        }
    }
    // else do nothing . wait connecting proc to deal.
}


static void config_service_send_request_failed(void)
{
    if(s_config_socket.status_fail_count >= MAX_CONNECT_REPEAT)
    {
        LOG(INFO,"clock(%d) config_service_send_request_failed s_config_socket.status_fail_count(%d) > %d.", 
			util_clock(), s_config_socket.status_fail_count,MAX_CONNECT_REPEAT);
        config_service_finish(CONFIG_PING_TIME);
		GM_memset(s_config_socket.ip,0,sizeof(s_config_socket.ip));
    }
    // else do nothing .
}

GM_ERRCODE config_service_create(void)
{
    u8 addr[2*GOOME_DNS_MAX_LENTH+1];
    u8 IP[4];
    u32 port = 0;
    u8 idx = 0;

    gm_socket_init(&s_config_socket, SOCKET_INDEX_CONFIG);

    GM_memset(addr, 0x00, sizeof(addr));
    idx = GM_sscanf((const char*)config_service_get_pointer(CFG_CFGSERVERADDR), "%[^:]:%d", addr, &port);
    if (idx != 2)
    {
        LOG(WARN,"clock(%d) config_service_create assert(idx ==2) failed.", util_clock());
        return GM_PARAM_ERROR;
    }
    
    if(GM_SUCCESS != GM_ConvertIpAddr(addr, IP))
    {
        if(util_is_valid_dns(addr, GM_strlen((const char *)addr)))
        {
            gm_socket_set_addr(&s_config_socket, addr, GM_strlen((const char*)addr), port, STREAM_TYPE_DGRAM);		
			system_state_get_ip_cache(SOCKET_INDEX_CONFIG, IP);
			gm_socket_set_ip_port(&s_config_socket, IP, port, STREAM_TYPE_DGRAM);
        }
        else
        {
            LOG(WARN,"clock(%d) config_service_create assert(dns(%s)) failed.", util_clock(), addr);
            return GM_PARAM_ERROR;
        }
    }
    else
    {
        gm_socket_set_ip_port(&s_config_socket, IP, port, STREAM_TYPE_DGRAM);
        system_state_set_ip_cache(SOCKET_INDEX_CONFIG, IP);
    }

    s_config_socket_extend.last_ok_time = 0;
    s_config_socket_extend.wait = CONFIG_PING_TIME;
    
    LOG(WARN,"clock(%d) config_service_create access_id(%d) fifo(%p).", util_clock(), s_config_socket.access_id, &s_config_socket.fifo);
	return GM_SUCCESS;
}


static void config_service_finish(u32 wait)
{
    config_service_close();

    s_config_socket_extend.last_ok_time=util_clock();
    s_config_socket_extend.wait=wait;
    
    config_service_transfer_status(SOCKET_STATUS_DATA_FINISH);
}

GM_ERRCODE config_service_destroy(void)
{
    if(SOCKET_STATUS_ERROR == s_config_socket.status)
    {
        return GM_SUCCESS;
    }
    
    config_service_close();

    if(s_config_socket.status < SOCKET_STATUS_DATA_FINISH)
    {
        // 配置正在获取过程中断了网, 重建连接
        config_service_transfer_status(SOCKET_STATUS_INIT);
        return GM_SUCCESS;
    }

    //config_service是在gprs_create中创建, 后面 重建连接, 所以保留fifo
    
    s_config_socket_extend.last_ok_time=util_clock();
    s_config_socket_extend.wait=CONFIG_PING_TIME;
    
    config_service_transfer_status(SOCKET_STATUS_ERROR);
	return GM_SUCCESS;
}

static void config_service_close(void)
{
    if(s_config_socket.id >=0)
    {
        GM_SocketClose(s_config_socket.id);
        s_config_socket.id=-1;
    }
}



GM_ERRCODE config_service_timer_proc(void)
{
    if(!s_config_socket.fifo.base_addr)
    {
        return GM_SUCCESS;
    }

    switch(s_config_socket.status)
    {
    case SOCKET_STATUS_INIT:
        config_service_init_proc();
        break;
    case SOCKET_STATUS_CONNECTING:
        config_service_connecting_proc();
        break;
    case SOCKET_STATUS_WORK:
        config_service_work_proc();
        break;
    case SOCKET_STATUS_DATA_FINISH:
        config_service_finish_proc();
        break;
    case SOCKET_STATUS_ERROR:
        config_service_finish_proc();
        break;
    default:
        LOG(WARN,"clock(%d) config_service_timer_proc assert(s_config_socket.status(%d)) unknown.", util_clock(), s_config_socket.status);
        return GM_ERROR_STATUS;
    }

    return GM_SUCCESS;
}

void config_service_set(ConfigParamEnum id, ConfigParamDataType type, const void* buf, u16 len)
{
	
    s_configs[id].type = type;
    
    switch(s_configs[id].type)
    {
        case TYPE_STRING:
            s_configs[id].len = len;
			GM_MemoryFree(s_configs[id].data.str);
            s_configs[id].data.str = GM_MemoryAlloc(len + 1);
            GM_memcpy(s_configs[id].data.str, buf, len);
            s_configs[id].data.str[len] = 0;
			break;
        case TYPE_INT:
            s_configs[id].len = 4;
            GM_memcpy(&s_configs[id].data.i, buf, s_configs[id].len);
			break;
        case TYPE_SHORT:
            s_configs[id].len = 2;
            GM_memcpy(&s_configs[id].data.s, buf, s_configs[id].len);
			break;
        case TYPE_BYTE:
            s_configs[id].len = 1;
            s_configs[id].data.b = *((u8 *)buf);
            break;
        case TYPE_BOOL:
            s_configs[id].len = 1;
            s_configs[id].data.b = *((u8 *)buf);
			break;
        case TYPE_FLOAT:
            s_configs[id].len = 4;
            GM_memcpy(&s_configs[id].data.s, buf, s_configs[id].len);
			break;
        default:
            LOG(WARN,"clock(%d) config_service_set assert(s_configs[%d].type(%d)) unknown.", util_clock(),id,s_configs[id].type);
            return;
    }

    if(len != s_configs[id].len)
    {
        LOG(WARN,"clock(%d) config_service_set assert(len == s_configs[id].len) failed,s_configs[%d].len=%d", util_clock(),id,s_configs[id].len);
    }
}




/*获取指定id对应的配置        */
GM_ERRCODE config_service_get(ConfigParamEnum id, ConfigParamDataType type, void* buf, u16 len)
{	
	if(type != s_configs[id].type)
	{
		LOG(WARN,"clock(%d) config_service_get assert(s_configs[%d].type(%d)== %d) failed.", util_clock(),id,s_configs[id].type, type);
		return GM_PARAM_ERROR;
	}
	
    switch(s_configs[id].type)
    {
        case TYPE_STRING:
            if(len < s_configs[id].len)
            {
                LOG(WARN,"clock(%d) config_service_get assert(s_configs[%d].type(%d)) len:%d < %d.", util_clock(),id,s_configs[id].type,len,s_configs[id].len);
                return GM_PARAM_ERROR;
            }
            if(s_configs[id].data.str)
            {
                GM_memcpy(buf, s_configs[id].data.str, s_configs[id].len);
            }
            else
            {
                GM_memcpy(buf, "", 1);
            }
            if(len > s_configs[id].len)
            {
                ((u8 *)buf)[s_configs[id].len] = 0;
            }
            else
            {
                ((u8 *)buf)[len -1] = 0;
            }
            break;
        case TYPE_INT:
            if(s_configs[id].len != len)
            {
                LOG(WARN,"clock(%d) config_service_get assert(s_configs[%d].type(%d)) len:%d != %d.", util_clock(),id,s_configs[id].type,len,s_configs[id].len);
                return GM_PARAM_ERROR;
            }
            GM_memcpy(buf, &s_configs[id].data.i, s_configs[id].len);
            break;
        case TYPE_SHORT:
            if(s_configs[id].len != len)
            {
                LOG(WARN,"clock(%d) config_service_get assert(s_configs[%d].type(%d)) len:%d != %d.", util_clock(),id,s_configs[id].type,len,s_configs[id].len);
                return GM_PARAM_ERROR;
            }
            GM_memcpy(buf, &s_configs[id].data.s, s_configs[id].len);
            break;
        case TYPE_BYTE:
            if(s_configs[id].len != len)
            {
                LOG(WARN,"clock(%d) config_service_get assert(s_configs[%d].type(%d)) len:%d != %d.", util_clock(),id,s_configs[id].type,len,s_configs[id].len);
                return GM_PARAM_ERROR;
            }
            *((u8 *)buf) = s_configs[id].data.b;
            break;
        case TYPE_BOOL:
            if(s_configs[id].len != len)
            {
                LOG(WARN,"clock(%d) config_service_get assert(s_configs[%d].type(%d)) len:%d != %d.", util_clock(),id,s_configs[id].type,len,s_configs[id].len);
                return GM_PARAM_ERROR;
            }
            *((u8 *)buf) = s_configs[id].data.b;
            break;
        case TYPE_FLOAT:
            if(s_configs[id].len != len)
            {
                LOG(WARN,"clock(%d) config_service_get assert(s_configs[%d].type(%d)) len:%d != %d.", util_clock(),id,s_configs[id].type,len,s_configs[id].len);
                return GM_PARAM_ERROR;
            }
            GM_memcpy(buf, &s_configs[id].data.f, s_configs[id].len);
            break;
        default:
            LOG(WARN,"clock(%d) config_service_get assert(s_configs[%d].type(%d)) unknown.", util_clock(),id,s_configs[id].type);
            return GM_PARAM_ERROR;
    }
	return GM_SUCCESS;
}


u16 config_service_get_length(ConfigParamEnum id, ConfigParamDataType type)
{
    if(id >= CFG_PARAM_MAX )
    {
        LOG(WARN,"clock(%d) config_service_get assert(id(%d)) failed.", util_clock(),id);
        return 0;
    }

	return s_configs[id].len;
}


void* config_service_get_pointer(ConfigParamEnum id)
{
    static u8 empty_string[1] = "";
    
    switch(s_configs[id].type)
    {
        case TYPE_STRING:
            if(s_configs[id].data.str)
            {
                return s_configs[id].data.str;
            }
            else
            {
                return empty_string;
            }
        case TYPE_INT:
            return &s_configs[id].data.i;
        case TYPE_SHORT:
            return &s_configs[id].data.s;
        case TYPE_BYTE:
            return &s_configs[id].data.b;
        case TYPE_BOOL:
            return &s_configs[id].data.b;
        case TYPE_FLOAT:
            return &s_configs[id].data.f;
        default:
            LOG(WARN,"clock(%d) config_service_get_pointer assert(s_configs[%d].type(%d)) unknown.", util_clock(),id,s_configs[id].type);
            return NULL;
    }
}



void config_msg_content_parse(u8 *pdata, u16 len)
{
    ConfigParamDataType data_type = TYPE_NONE;
    u8 *psrc = pdata;
    u8 index = 0;
    u16 parse_len = 0;
    u8 cmd_string[CONFIG_CMD_MAX_LEN+1] = {0};
    u8 msg_buff[CONFIG_STRING_MAX_LEN+1] = {0};
    u8 msg_len = 0;
	bool has_config = false;

	JsonObject* p_json_log = json_create();
	json_add_string(p_json_log, "event", "config");
	
    while(parse_len <= (len-12))
    {
        data_type = (ConfigParamDataType)(psrc[parse_len]);
        parse_len++;
		
		
        GM_memcpy(cmd_string, &psrc[parse_len], CONFIG_CMD_MAX_LEN);
        cmd_string[CONFIG_CMD_MAX_LEN] = 0;
        parse_len += CONFIG_CMD_MAX_LEN;

		GM_memset(msg_buff,0,sizeof(msg_buff));
		parse_len += config_msg_get_item(data_type, msg_buff, &psrc[parse_len], &msg_len);

		switch (data_type)
		{
			case 1:
				json_add_int(p_json_log, (const char*)cmd_string, MKDWORD(msg_buff[0], msg_buff[1], msg_buff[2], msg_buff[3]));
				break;
			case 2:
				json_add_int(p_json_log, (const char*)cmd_string, MKDWORD(msg_buff[0], msg_buff[1], msg_buff[2], msg_buff[3])/1000.0);
				break;
			case 3:
				json_add_string(p_json_log, (const char*)cmd_string, (const char*)msg_buff);
				break;
			case 4:
				json_add_int(p_json_log, (const char*)cmd_string, msg_buff[0]);
				break;
			default:
				json_add_string(p_json_log, (const char*)cmd_string, "unknown config name");
				break;	
		}

		
        util_remove_char(cmd_string, GM_strlen((const char*)cmd_string),' ');
        util_string_upper(cmd_string, GM_strlen((const char*)cmd_string));

		
		//特殊配置，进行校准修复
		if (0 == GM_strcmp((const char*)cmd_string, "REPAIRRAMP") && msg_buff[0])
		{
			auto_test_repair_ramp();
			has_config = true;
		}
		
        for (index=0; index< (sizeof(s_config_param)/sizeof(ConfigParamStruct)); index++)
        {
            if (0 == GM_strcmp((const char*)cmd_string, (const char*)s_config_param[index].cmd_string))
            {
                config_msg_param_set(s_config_param[index].index, msg_buff, msg_len);
				has_config = true;
                break;
            }
        }
    };

	if (has_config)
	{
		config_service_save_to_local();
    	gps_service_change_config();
		log_service_upload(INFO, p_json_log);
	}
	else
	{
		json_destroy(p_json_log);
		p_json_log = NULL;
	}
  
    return;
}

static void config_service_set_addr(u16 index, u8 *pMsg, u8 len)
{
    u8 addr[100];
    u8 IP[4];
    u32 port = 0;
    u8 idx = 0;

    GM_memset(addr, 0x00, sizeof(addr));
    idx = GM_sscanf((const char*)pMsg, "%[^:]:%d", addr, &port);
    if (idx != 2)
    {
        LOG(INFO,"clock(%d) config_service_set_addr assert(2 fields(%s)) failed.", util_clock(), pMsg);
        return;
    }
    
    if(GM_SUCCESS != GM_ConvertIpAddr(addr, IP))
    {
        if(util_is_valid_dns(addr, GM_strlen((const char *)addr)))
        {
        	LOG(INFO,"clock(%d) config_service_set_addr(%s) success.", util_clock(), pMsg);
            config_service_set((ConfigParamEnum)index, TYPE_STRING, pMsg, len);
        }
        else
        {
            LOG(WARN,"clock(%d) config_service_set_addr assert(dns(%s)) failed.", util_clock(), addr);
            return;
        }
    }
    else
    {
        LOG(INFO,"clock(%d) config_service_set_addr(%s) success.", util_clock(), pMsg);
        config_service_set((ConfigParamEnum)index, TYPE_STRING, pMsg, len);
    }
}

static void config_service_set_byte(u16 index, u8 *pMsg, u8 len)
{
	if (NULL == pMsg || len < sizeof(U32))
	{
		LOG(INFO,"clock(%d) config_service_set_range_word index(%d) len %d failed.", util_clock(),index,len);
		return;
	}
    config_service_set((ConfigParamEnum)index, TYPE_BYTE, pMsg, sizeof(U8));
    LOG(INFO,"clock(%d) config_service_set_byte index(%d) value %d.", util_clock(),index,pMsg[0]);
}


static void config_service_set_range_word(u16 index, u8 *pMsg, u8 len, u32 min, u32 max)
{
    U32 dword_value = 0;
    if (NULL == pMsg || len < sizeof(U32))
    {
        LOG(INFO,"clock(%d) config_service_set_range_word index(%d) len %d failed.", util_clock(),index,len);
		return;
    }
    dword_value = MKDWORD(pMsg[0], pMsg[1],pMsg[2], pMsg[3]);
    if (dword_value >= min && dword_value <= max)
    {
        config_service_set((ConfigParamEnum)index, TYPE_SHORT, &dword_value, sizeof(U16));
        LOG(INFO,"clock(%d) config_service_set_range_word index(%d) value %d.", util_clock(),index,dword_value);
    }
    else
    {
        LOG(WARN,"clock(%d) config_service_set_range_word index(%d) value %d failed.", util_clock(),index,dword_value);
    }
}

static void config_service_set_protocol(u16 index, u8 *pMsg, u8 len)
{
    u32 dword_value;
    u8 byte_value;
    
    if(len < 4)
    {
        LOG(INFO,"clock(%d) config_service_set_protocol index(%d) len %d failed.", util_clock(),index,len);
    }
    dword_value = MKDWORD(pMsg[0], pMsg[1], pMsg[2], pMsg[3]);
    byte_value = dword_value;
    if (byte_value >= PROTOCOL_GOOME && byte_value <= PROTOCOL_BSJ)
    {
        config_service_set(CFG_PROTOCOL, TYPE_BYTE, &byte_value, sizeof(byte_value));
        LOG(INFO,"clock(%d) config_service_set_protocol CFG_PROTOCOL value %d.", util_clock(),byte_value);
    }
    else
    {
        LOG(INFO,"clock(%d) config_service_set_protocol CFG_PROTOCOL value %d failed.", util_clock(),byte_value);
    }
}


static void config_service_set_app_protocol(u16 index, u8 *pMsg, u8 len)
{
    u32 dword_value;
    u8 byte_value;
    
    if(len < 4)
    {
        LOG(INFO,"clock(%d) config_service_set_app_protocol index(%d) len %d failed.", util_clock(),index,len);
    }
    dword_value = MKDWORD(pMsg[0], pMsg[1], pMsg[2], pMsg[3]);
    byte_value = dword_value;
    if (byte_value >= PROTOCOL_VER_GT02 && byte_value <= PROTOCOL_VER_GT06)
    {
        config_service_set(CFG_PROTOCOL_VER, TYPE_BYTE, &byte_value, sizeof(byte_value));
        LOG(INFO,"clock(%d) config_service_set_protocol CFG_PROTOCOL_VER value %d.", util_clock(),byte_value);
    }
    else
    {
        LOG(INFO,"clock(%d) config_service_set_app_protocol CFG_PROTOCOL_VER value %d failed.", util_clock(),byte_value);
    }
}


static void config_service_heart_protocol(u16 index, u8 *pMsg, u8 len)
{
    u32 dword_value;
    u8 heart_mode;
    
    if(len < 4)
    {
        LOG(INFO,"clock(%d) config_service_gsensor_range index(%d) len %d failed.", util_clock(),index,len);
    }
    dword_value = MKDWORD(pMsg[0], pMsg[1], pMsg[2], pMsg[3]);
    switch(dword_value)
    {
        case 3:
            dword_value = HEART_NORMAL;
            break;
        case 7:
            dword_value = HEART_EXPAND;
            break;
        case 0:
            dword_value = HEART_SMART;
            break;
        default:
            LOG(INFO,"clock(%d) config_service_heart_protocol (%d) failed.", util_clock(),dword_value);
            return;
    }
    
    heart_mode = (u8)dword_value;
    config_service_set((ConfigParamEnum)index, TYPE_BYTE, &heart_mode, sizeof(heart_mode));
    LOG(INFO,"clock(%d) config_service_heart_protocol index(%d) value %d.", util_clock(),index,heart_mode);
}


static void config_service_gsensor_set_float(u16 index, u8 *pMsg, u8 len, float min, float max)
{
    u32 dword_value;
    float float_value;
    if(len < 4)
    {
        LOG(INFO,"clock(%d) config_service_gsensor_set_float index(%d) len %d failed.", util_clock(),index,len);
    }
    dword_value = MKDWORD(pMsg[0], pMsg[1], pMsg[2], pMsg[3]);
    float_value = dword_value / 1000.0f;
    if (float_value >= min && float_value <= max)
    {
        config_service_set((ConfigParamEnum)index, TYPE_FLOAT, &float_value, sizeof(float_value));
        LOG(INFO,"clock(%d) config_service_gsensor_set_float index(%d) value %f.", util_clock(),index,float_value);
    }
    else
    {
        LOG(ERROR,"clock(%d) config_service_gsensor_set_float CFG_ACCTHR[%f] failed.", util_clock(),float_value);
    }

}


static void config_service_device_type(u16 index, u8 *pMsg, u8 len)
{
    u16 idx;
	
    for (idx=0; idx<DEVICE_MAX; idx++)
    {
        if (0 == GM_strcmp((const char*)pMsg, s_config_device[idx].device_str))
        {
            config_service_set_device((ConfigDeviceTypeEnum)s_config_device[idx].idx);
            LOG(INFO,"config_service_device_type (CFG_DEVICETYPE(%d).",s_config_device[idx].idx);
            break;
        }
    }
    return;
}

const char * config_service_get_device_type(u16 index)
{
    if(index >= DEVICE_MAX)
    {
        LOG(INFO,"clock(%d) config_service_get_device_type assert(device_type(%d)) failed.", util_clock(),index);
        return NULL;
    }
    return s_config_device[index].device_str;
}

ConfigDeviceTypeEnum config_service_get_device_type_id(const char* dev_type)
{
	U8 index = 0;
	for (index = 0; index < DEVICE_MAX; ++index)
	{
		if(!GM_strcmp(s_config_device[index].device_str,dev_type))
		{
			return (ConfigDeviceTypeEnum)s_config_device[index].idx;
		}
	}
	return DEVICE_MAX;
}

static void config_msg_param_set(u16 index, u8 *pMsg, u8 len)
{
    u8 value_u8;
    switch(index)
    {
    	case CFG_DEVICETYPE:
            config_service_device_type(index, pMsg, len);
            break;
		
        case CFG_SERVERADDR:
            config_service_set_addr(index, pMsg, len);
            break;
		
        case CFG_SERVERLOCK:
			config_service_set_byte(index, pMsg, sizeof(U32));
            break;
		
        case CFG_PROTOCOL:
            config_service_set_protocol(index, pMsg, len);
            break;
		
		case CFG_PROTOCOL_VER:
			config_service_set_app_protocol(index, pMsg, len);
			break;
		
        case CFG_HBPROTOCOL:
            config_service_heart_protocol(index, pMsg, len);
            break;
		
        case CFG_UPLOADTIME:
			config_service_set_range_word((ConfigParamEnum)index, pMsg, sizeof(U32),CONFIG_UPLOAD_TIME_MIN,CONFIG_UPLOAD_TIME_MAX);
            break;

		case CFG_HEART_INTERVAL:
			config_service_set_range_word((ConfigParamEnum)index, pMsg, sizeof(U32),GOOME_HEARTBEAT_MIN,GOOME_HEARTBEAT_MAX);
			break;
		
		case CFG_SLEEP_TIME:
			config_service_set_range_word((ConfigParamEnum)index, pMsg, sizeof(U32),GOOME_SLEEPTIME_MIN,GOOME_SLEEPTIME_MAX);
			break;

		case CFG_IS_LBS_ENABLE:
        config_service_set((ConfigParamEnum)index, TYPE_BOOL, pMsg, sizeof(bool));
			break;
		
		case CFG_SEN_ANGLE_SHOW_TIME:
			config_service_set_byte(index, &pMsg[3], sizeof(U32));
			break;

		case CFG_IS_MOVEALARM_ENABLE:
            config_service_set((ConfigParamEnum)index, TYPE_BOOL, pMsg, sizeof(u8));
            break;
		
		case CFG_SMOOTH_TRACK:
			config_service_set((ConfigParamEnum)index, TYPE_BOOL, pMsg, sizeof(u8));
			break;
		
		case CFG_BATTUPLOAD_DISABLE:
            value_u8 = *pMsg;
            value_u8 = value_u8?0:1;
            config_service_set((ConfigParamEnum)index, TYPE_BYTE, &value_u8, sizeof(u8));
            break;
		
        case CFG_CUTOFFALM_DISABLE:
        {
			//后台配置1是打开0是关闭;设备中存储0是打开1是关闭
			U8 cut_off_alm_disable = pMsg[0] ? 0 : 1;
            config_service_set((ConfigParamEnum)index, TYPE_BYTE, &cut_off_alm_disable, sizeof(cut_off_alm_disable));
        }
            break;
		
        case CFG_LOWBATTALM_DISABLE:
        {
			//后台配置1是打开0是关闭;设备中存储0是打开1是关闭
			U8 low_batt_alm_disable = pMsg[0] ? 0 : 1;
            config_service_set((ConfigParamEnum)index, TYPE_BYTE, &low_batt_alm_disable, sizeof(low_batt_alm_disable));
        }
            break;
		
        case CFG_SPEEDTHR:
			config_service_set_byte(index, &pMsg[3], sizeof(U32));
			break;

		case CFG_IS_ACLRALARM_ENABLE:
            config_service_set((ConfigParamEnum)index, TYPE_BOOL, pMsg, sizeof(u8));
            break;

		case CFG_IS_SHAKE_ALARM_ENABLE:
            config_service_set((ConfigParamEnum)index, TYPE_BOOL, pMsg, sizeof(u8));
            break;

		case CFG_SHAKE_COUNT:
			config_service_set_byte(index, &pMsg[3], sizeof(U32));
            break;
		
		case CFG_SHAKE_CHECK_TIME:
			config_service_set_byte(index, &pMsg[3], sizeof(U32));
            break;

		case CFG_IS_MANUAL_DEFENCE:
            config_service_set((ConfigParamEnum)index, TYPE_BOOL, pMsg, sizeof(u8));
            break;

		case CFG_AUTO_DEFENCE_DELAY:
			config_service_set_range_word((ConfigParamEnum)index, pMsg, sizeof(U32),CONFIG_AUTODEFDELAY_MIN,CONFIG_AUTODEFDELAY_MAX);
            break;

		case CFG_SHAKE_ALARM_DELAY:
			config_service_set_range_word((ConfigParamEnum)index, pMsg, sizeof(U32),CONFIG_SHAKEALARMDELAY_MIN,CONFIG_SHAKEALARMDELAY_MAX);
            break;

		case CFG_SHAKE_ALARM_INTERVAL:
			config_service_set_range_word((ConfigParamEnum)index, pMsg, sizeof(U32),CONFIG_SHAKEALARMINTERVAL_MIN,CONFIG_SHAKEALARMINTERVAL_MAX);
            break;

		case CFG_LANGUAGE:			
			config_service_set_range_word((ConfigParamEnum)index, pMsg, sizeof(U32),0,1);
			break;

		case CFG_TIME_ZONE:			
			config_service_set_byte(index, &pMsg[3], sizeof(U32));
            break;

        case CFG_SEN_RAPID_ACLR:
            config_service_gsensor_set_float(index, pMsg, len, CONFIG_ACCTHR_MIN, CONFIG_ACCTHR_MAX);
            break;
		
        case CFG_SEN_SUDDEN_TURN:
            config_service_gsensor_set_float(index, pMsg, len, CONFIG_TURNTHR_MIN, CONFIG_TURNTHR_MAX);
            break;

        case CFG_SEN_EMERGENCY_BRAKE:
            config_service_gsensor_set_float(index, pMsg, len, CONFIG_BRAKETHR_MIN, CONFIG_BRAKETHR_MAX);
            break;

        case CFG_SEN_SLIGHT_COLLISION:
            config_service_gsensor_set_float(index, pMsg, len, CONFIG_CRASHLTHR_MIN, CONFIG_CRASHLTHR_MAX);
            break;

        case CFG_SEN_NORMAL_COLLISION:
            config_service_gsensor_set_float(index, pMsg, len, CONFIG_CRASHMTHR_MIN, CONFIG_CRASHMTHR_MAX);
            break;
            
        case CFG_SEN_SERIOUS_COLLISION:
            config_service_gsensor_set_float(index, pMsg, len, CONFIG_CRASHHTHR_MIN, CONFIG_CRASHHTHR_MAX);
            break;

		case CFG_MIN_SNR:
			config_service_set_byte(index, &pMsg[3], sizeof(U32));
            break;

        default:
            break;
    }

    return;
}



static u16 config_msg_get_item(const ConfigParamDataType data_type, u8 *pOut, const u8 *pSrc, u8 *len)
{
    u16 msg_len = 4;
    u8 seek = 0;

    *len = msg_len;
    if ((NULL == pOut) || (NULL == pSrc))
    {
        return 0;
    }

    if (TYPE_STRING == data_type)
    {
        //第一字节是长度
        msg_len = *pSrc;
        *len = (msg_len > CONFIG_STRING_MAX_LEN) ? CONFIG_STRING_MAX_LEN : msg_len;
        seek = 1;
        pSrc++;
        if(*len)
        {
            GM_memcpy(pOut, pSrc, *len);
            pOut[*len] = 0;
        }
        else
        {
            pOut[0] = 0;
        }
    }
    else if (TYPE_BOOL == data_type)
    {
        *len = msg_len = 1;
        GM_memcpy(pOut, pSrc, *len);
    }
    else
    {
        //*len = msg_len = 4;
        GM_memcpy(pOut, pSrc, *len);
    }
    return msg_len + seek;
}

static void config_srevice_create_other_services(void)
{
    static bool first_create = false;  // already created at gprs_init
    gps_service_create(first_create);

    if(first_create)
    {
        // after first create, all later create is not first create.
        first_create = false;
    }
}

ConfigProtocolEnum config_service_get_app_protocol()
{
    u8 byte_value;
    config_service_get(CFG_PROTOCOL, TYPE_BYTE, &byte_value, sizeof(byte_value));
    switch (byte_value)
    {
        case PROTOCOL_GOOME:
            return PROTOCOL_GOOME;
        case PROTOCOL_CONCOX:
            return PROTOCOL_CONCOX;
        case PROTOCOL_JT808:
            return PROTOCOL_JT808;
        case PROTOCOL_BSJ:
            return PROTOCOL_BSJ;
        default:
            LOG(DEBUG,"config_service_get_app_protocol (CFG_PROTOCOL(PROTOCOL_NONE).");
            return PROTOCOL_NONE;
    }
}

ConfigHearUploadEnum config_service_get_heartbeat_protocol(void)
{
    u8 byte_value;
    config_service_get(CFG_HBPROTOCOL, TYPE_BYTE, &byte_value, sizeof(byte_value));
    switch (byte_value)
    {
        case HEART_SMART:
            if(config_service_is_main_server_goome() && (PROTOCOL_GOOME== config_service_get_app_protocol()))
            {
                return HEART_EXPAND;
            }
            else
            {
                return HEART_NORMAL;
            }
        case HEART_NORMAL:
            return HEART_NORMAL;
        case HEART_EXPAND:
            return HEART_EXPAND;
        default:
            LOG(WARN,"clock(%d) config_service_get_heartbeat_protocol assert(heart_protocol(%d)) failed.", util_clock(),byte_value);
            return HEART_NORMAL;
    }
}


S8 config_service_get_zone(void)
{
    GM_ERRCODE ret;
    s8 time_zone = 0;
    
	//获取本地时间
	ret = config_service_get(CFG_TIME_ZONE, TYPE_BYTE, &time_zone, sizeof(time_zone));
	if (GM_SUCCESS != ret)
	{
		LOG(INFO,"clock(%d) config_service_get_zone failed,ret=%d", util_clock(),ret);
	}
    return time_zone;
}

bool config_service_is_main_server_goome(void)
{
    /*char *addr;
    addr = config_service_get_pointer(CFG_SERVERADDR);*/
    char addr[50];
	
	GM_memset(addr, 0, sizeof(addr));
	GM_memcpy(addr, config_service_get_pointer(CFG_SERVERADDR), GM_strlen(config_service_get_pointer(CFG_SERVERADDR)));
	util_string_lower((u8 *)&addr, GM_strlen(addr));
    if (GM_strstr(addr, GOOME_GPSOO_DNS) > 0)
    {
        return true;
    }

    if (GM_strstr(addr, GOOME_LITEDEV_DNS) > 0)
    {
        return true;
    }
    
    return false;
}

bool config_service_is_alarm_sms_to_one(void)
{
    u8 value_u8;
    config_service_get(CFG_ALARM_SMS_ONE, TYPE_BOOL, &value_u8, sizeof(value_u8));
    return (value_u8 != 0);
}


u8 config_service_get_sos_number(u8 index, u8 *buf, u16 len)
{
    GM_ERRCODE ret;
    
    if(index > 3)
    {
        LOG(WARN,"clock(%d) config_service_get_sos_number assert(index(%d)) failed.", util_clock(),index);
        return 0;
    }
    
    index = index + CFG_USER1_NUMBER;
	ret = config_service_get((ConfigParamEnum)index, TYPE_STRING, buf, len);
	if (GM_SUCCESS != ret)
	{
		LOG(INFO,"clock(%d) config_service_get_sos_number failed,ret=%d", util_clock(),ret);
        return 0;
	}
    
    len = config_service_get_length((ConfigParamEnum)index, TYPE_STRING);
    if (len > 15)
    {
        len = 15;
    }
    
    return len;
}


bool config_service_is_agps(void)
{
    u8 value_u8;
    config_service_get(CFG_OPEN_APGS, TYPE_BOOL, &value_u8, sizeof(value_u8));
    return (value_u8 != 0);
}



GPSChipType config_service_get_gps_type(void)
{
    GM_ERRCODE ret;
    GPSChipType gps_chip_type = GM_GPS_TYPE_UNKNOWN;
   
	ret = config_service_get(CFG_GPS_TYPE, TYPE_BYTE, &gps_chip_type, sizeof(gps_chip_type));
	if (GM_SUCCESS != ret)
	{
		LOG(INFO,"clock(%d) config_service_get_gps_type failed,ret=%d", util_clock(),ret);
	}

    if(gps_chip_type > (u32)GM_GPS_TYPE_AT_AGPS)
    {
        LOG(INFO,"clock(%d) config_service_get_gps_type assert(gps_chip_type(%d) failed.", util_clock(),gps_chip_type);
        gps_chip_type = GM_GPS_TYPE_UNKNOWN;
    }
    
    return (GPSChipType)gps_chip_type;
}


void config_service_change_ip(ConfigParamEnum idx, u8 *buf, u16 len)
{
    u8 dns_str[GOOME_DNS_MAX_LENTH];
    u8 value_str[CONFIG_STRING_MAX_LEN];
    u8 IP[4];
    u32 value_u32;
	s32 index;

    if(len >=  GOOME_DNS_MAX_LENTH)
    {
        LOG(WARN,"clock(%d) config_service_change_ip assert(len(%d) of CFG_SERVERADDR failed.", util_clock(),len);
        return;
    }
    
    GM_memset(value_str, 0, sizeof(value_str));
    GM_memcpy(value_str, buf, len);
    if(GM_SUCCESS != GM_ConvertIpAddr(value_str, IP))
    {
        if(!util_is_valid_dns(value_str, len))
        {
            LOG(WARN,"clock(%d) config_service_change_ip assert(valid dns(%s) failed.", util_clock(), value_str);
            return;
        }
    }

    GM_memset(dns_str, 0x00, sizeof(dns_str));
    index = GM_sscanf((const char*)config_service_get_pointer(idx), "%[^:]:%d", dns_str, &value_u32);
    if ((index != 2) || (GM_strlen((const char*)dns_str) >= GOOME_DNS_MAX_LENTH) || (value_u32>65535))
    {
        LOG(WARN,"clock(%d) config_service_change_ip assert(idx ==2) of CFG_SERVERADDR failed.", util_clock());
        return;
    }

    GM_strcpy((char *)dns_str,(char *)value_str);
    GM_snprintf((char *)value_str, sizeof(value_str),"%s:%d", dns_str, value_u32);
    config_service_set(idx, TYPE_STRING, value_str, GM_strlen((const char*)value_str));
}


void config_service_change_port(ConfigParamEnum idx, u16 port)
{
    u8 dns_str[GOOME_DNS_MAX_LENTH];
    u8 value_str[CONFIG_STRING_MAX_LEN];
    u32 value_u32;
	s32 index;

    GM_memset(dns_str, 0x00, sizeof(dns_str));
    index = GM_sscanf((const char*)config_service_get_pointer(idx), "%[^:]:%d", dns_str, &value_u32);
    if ((index != 2) || (GM_strlen((const char*)dns_str) >= GOOME_DNS_MAX_LENTH) || (value_u32>65535))
    {
        LOG(WARN,"clock(%d) config_service_change_port assert(idx ==2) of CFG_SERVERADDR failed.", util_clock());
        return;
    }

    GM_snprintf((char *)value_str, sizeof(value_str),"%s:%d", dns_str, port);
    config_service_set(idx, TYPE_STRING, value_str, GM_strlen((const char*)value_str));
}


StreamType config_service_update_socket_type(void)
{
    //return STREAM_TYPE_DGRAM;
    return STREAM_TYPE_STREAM;
}

StreamType config_service_agps_socket_type(void)
{
    //return STREAM_TYPE_DGRAM;
    return STREAM_TYPE_STREAM;
}

