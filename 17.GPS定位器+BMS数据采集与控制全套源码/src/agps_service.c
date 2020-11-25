#include "agps_service.h"
#include "config_service.h"
#include "log_service.h"
#include "gm_stdlib.h"
#include "gm_gprs.h"
#include "utility.h"
#include "gsm.h"
#include "gm_memory.h"
#include "applied_math.h"
#include "gm_fs.h"
#include "gm_timer.h"
#include "gps_service.h"
#include "system_state.h"

#define PROTOCOL_HEADER_AGPS 0x29
#define PROTOCOL_TAIL_AGPS   0x0D

//由于udp受MTU限制,500比较安全
#define AGPS_MSG_REQ_LENTH 500  

#define AGPS_MSG_PACKAGE_LENTH  21
#define TWO_HOUR_SECONDS   (3600*2)
#define SIX_HOUR_SECONDS   (3600*6)
#define EPO_GPS_DATA_6HOURS_LENTH   2304
#define AGPS_FILE_PATH L"Z:\\goome\\GmAgpsFile\0"
#define EPO_FILE_PATH L"Z:\\goome\\GmEpoFile.DAT\0"

static SocketType s_agps_socket = {-1,"",SOCKET_STATUS_ERROR,};

static u8 s_at6558_flag[] = {0x41,0x47,0x4E,0x53,0x53,0x20,0x64,0x61,0x74,0x61,0x20,0x66,0x72,0x6F,0x6D,0x20,0x43,0x41,0x53,0x49,0x43};
static u8 s_techtotop_flag[] = {0x54, 0x65, 0x63, 0x68, 0x74, 0x6F, 0x74, 0x6F, 0x70, 0x20, 0x41, 0x2D, 0x47, 0x4E, 0x53, 0x53, 0x20, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x0A};

typedef struct
{
	//是否gps模块申请了gps
    bool required_agps;   

	 //是否正在请求校时
    bool timing;   

    // epo/agps 文件信息
    // 消息包总大小
    u32 data_len;  

	// 请求文件时,数据起始位置
    u32 data_start;   
}SocketTypeAgpsExtend;

static SocketTypeAgpsExtend s_agps_socket_extend = {0,0,0,0};

typedef struct
{
    u16 crc;         // applied_math_calc_common_crc16
    u8 gps_dev_type; //GPSChipType
    u8 leap_seconds; // 润秒数
    u32 file_flag;   // 文件flag
    u32 len;         // pdata长度
    u32 start_time;  //utc 开始时间
    float lng;       //经度
    float lat;       //纬度
    u32 last;        // 此文件能管多久,一般是2小时, 服务器每半小时拿一次数据,如果数据中不带时间的话, 则取1.5小时
    u8 data[AGPS_MSG_REQ_LENTH*8];  //content.
}AgpsFileExtend;

static AgpsFileExtend s_agps_file_extend = {0};
static AgpsFileExtend s_agps_file_inuse = {0};

#define AGPS_FILE_EXTEND_HEAD_SIZE (sizeof(AgpsFileExtend) - (AGPS_MSG_REQ_LENTH*8))

static GM_ERRCODE agps_service_transfer_status(u8 new_status);
static void agps_service_init_proc(void);
static void agps_service_connecting_proc(void);
static void agps_service_work_proc(void);
static void agps_service_data_finish_proc(void);
static void agps_service_close(void);

static void agps_msg_send_timing(void);
static void agps_msg_send_get_file(u16 data_len);
static void agps_msg_receive(SocketType *socket);

static void agps_msg_pack_head(u8 *pdata, u16 *idx, u16 len);
static void agps_msg_pack_timing(u8 *pdata, u16 *idx, u16 len);
static void agps_msg_pack_id_len(u8 *pdata, u8 id, u16 len);
static void agps_msg_pack_get_file(u8 *pdata, u16 *idx, u16 len,u16 data_len);


static void agps_msg_parse(u8 *pdata, u16 len);
static void agps_msg_parse_timing_ack(u8 *pdata, u16 len);
static void agps_msg_parse_epo_ack(u8 *pdata, u16 len);
static void agps_msg_parse_taidou_ack(u8 *pdata, u16 len);
static void agps_msg_parse_zkw_ack(u8 *pdata, u16 len);


static u8 agps_data_check_item_num(u8 *pdata, u16 len);
static u8 goome_agps_at6558_check_item_num(u8 *pdata, u16 len);
static void agps_write_data_file(const U16 * file_name);
static void agps_read_data_file(const U16 * file_name);
static void agps_service_check_and_read_file(void);
static bool agps_service_write_to_gps(void);
static void agps_service_reset(void);

static GM_ERRCODE agps_service_transfer_status(u8 new_status)
{
    u8 old_status = (u8)s_agps_socket.status;
    GM_ERRCODE ret = GM_PARAM_ERROR;
    switch(s_agps_socket.status)
    {
        case SOCKET_STATUS_INIT:
            switch(new_status)
            {
                case SOCKET_STATUS_INIT:
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
        default:
            break;
    }


    if(GM_SUCCESS == ret)
    {
        s_agps_socket.status = new_status;
        s_agps_socket.status_fail_count = 0;
        LOG(INFO,"clock(%d) agps_service_transfer_status from %s to %s success", util_clock(), 
            gm_socket_status_string((SocketStatus)old_status), gm_socket_status_string((SocketStatus)new_status));
    }
    else
    {
        LOG(INFO,"clock(%d) agps_service_transfer_status from %s to %s failed", util_clock(), 
            gm_socket_status_string((SocketStatus)old_status), gm_socket_status_string((SocketStatus)new_status));
    }

    return ret;

}


GM_ERRCODE agps_service_create(bool first_create)
{
    u8 addr[2*GOOME_DNS_MAX_LENTH+1];
    u8 IP[4];
    u32 port = 0;
    u8 idx = 0;

    if(!first_create)
    {
        if(s_agps_socket.fifo.base_addr)
        {
            return GM_SUCCESS;
        }
    }
	
    gm_socket_init(&s_agps_socket, SOCKET_INDEX_AGPS);

    GM_memset(addr, 0x00, sizeof(addr));
    idx = GM_sscanf((const char *)config_service_get_pointer(CFG_AGPSSERVERADDR), "%[^:]:%d", addr, &port);
    if (idx != 2)
    {
        LOG(WARN,"clock(%d) agps_service_create assert(idx(%d) ==2) failed.", util_clock(),idx);
        return GM_PARAM_ERROR;
    }
    
    if(GM_SUCCESS != GM_ConvertIpAddr(addr, IP))
    {
        if(util_is_valid_dns(addr, GM_strlen((const char *)addr)))
        {
            gm_socket_set_addr(&s_agps_socket, addr, GM_strlen((const char *)addr), port, config_service_agps_socket_type());
			system_state_get_ip_cache(SOCKET_INDEX_AGPS, IP);
			gm_socket_set_ip_port(&s_agps_socket, IP, port, config_service_agps_socket_type());
        }
        else
        {
            LOG(WARN,"clock(%d) agps_service_create assert(dns(%s)) failed.", util_clock(), addr);
            return GM_PARAM_ERROR;
        }
    }
    else
    {
        gm_socket_set_ip_port(&s_agps_socket, IP, port, config_service_agps_socket_type());
        system_state_set_ip_cache(SOCKET_INDEX_AGPS, IP);
    }


    LOG(INFO,"clock(%d) agps_service_create access_id(%d) fifo(%p).", util_clock(), s_agps_socket.access_id, &s_agps_socket.fifo);

    GM_memset(&s_agps_file_extend, 0, sizeof(s_agps_file_extend));
    GM_memset(&s_agps_file_inuse, 0, sizeof(s_agps_file_inuse));
	return GM_SUCCESS;
}

GM_ERRCODE agps_service_destroy(void)
{
    if(SOCKET_STATUS_ERROR == s_agps_socket.status)
    {
        return GM_SUCCESS;
    }
    
    agps_service_close();
    
    //agps socket only reconnect, not recreated.
    //fifo_delete(&s_agps_socket.fifo);

    agps_service_transfer_status(SOCKET_STATUS_ERROR);
	return GM_SUCCESS;
}

static void agps_service_close(void)
{
    if(s_agps_socket.id >=0)
    {
        GM_SocketClose(s_agps_socket.id);
        s_agps_socket.id=-1;
    }
}

GM_ERRCODE agps_service_timer_proc(void)
{
    if(!s_agps_socket.fifo.base_addr)
    {
        return GM_SUCCESS;
    }

    switch(s_agps_socket.status)
    {
    case SOCKET_STATUS_INIT:
        agps_service_init_proc();
        break;
    case SOCKET_STATUS_CONNECTING:
        agps_service_connecting_proc();
        break;
    case SOCKET_STATUS_WORK:
        agps_service_work_proc();
        break;
    case SOCKET_STATUS_DATA_FINISH:
        agps_service_data_finish_proc();
        break;
    case SOCKET_STATUS_ERROR:
        agps_service_data_finish_proc();
        break;
    default:
        LOG(WARN,"clock(%d) agps_service_timer_proc assert(s_agps_socket.status(%d)) unknown.", util_clock(),s_agps_socket.status);
        return GM_ERROR_STATUS;
    }

    return GM_SUCCESS;
}


static bool agps_service_write_to_gps(void)
{
    agps_service_check_and_read_file();

    //由于agps文件是多管半小时的, 所以提前拿文件没必要, 多加180是防止本地时间不太准
    if((s_agps_file_inuse.start_time <= (util_get_utc_time()+180)) &&
        (s_agps_file_inuse.start_time+s_agps_file_inuse.last ) > util_get_utc_time() )
    {
        //数据有效
		LOG(INFO,"clock(%d) agps_service_write_to_gps ok, start(%d) last(%d) until(%d).", 
            util_clock(),s_agps_file_inuse.start_time,s_agps_file_inuse.last, util_get_utc_time());
        if(s_agps_socket_extend.required_agps && (!gps_is_fixed()))
        {
            agps_service_require_to_gps(AGPS_TO_GPS_LNGLAT_TIME,false);
        }
        return true;
    }
    LOG(DEBUG,"clock(%d) agps_service_write_to_gps file failed, start(%d) last(%d) until(%d).", 
        util_clock(),s_agps_file_inuse.start_time,s_agps_file_inuse.last,util_get_utc_time());

    return false;
}

static void agps_service_init_proc(void)
{
    GM_ERRCODE ret;
    gm_cell_info_struct lbs;
    bool write_agps_succ = false;
    u8 IP[4];

    if(gps_is_fixed())
    {
        write_agps_succ = agps_service_write_to_gps(); //here will read file
        {
            //report logs 
            JsonObject* p_json_log = json_create();
            json_add_string(p_json_log,"event","read_agps1");
            json_add_int(p_json_log,"start",s_agps_file_inuse.start_time);
            json_add_int(p_json_log,"last",s_agps_file_inuse.last);
            json_add_int(p_json_log,"ret",write_agps_succ?1:0);
            log_service_upload(INFO, p_json_log);
        }

        //由于agps文件是多管半小时的, 所以提前拿文件没必要, 多加180是防止本地时间不太准
        if((s_agps_file_inuse.start_time <= (util_get_utc_time()+180)) &&
            (s_agps_file_inuse.start_time+s_agps_file_inuse.last ) > util_get_utc_time() )
        {
            LOG(INFO,"clock(%d) agps_service_init_proc fix and agps data is ok.",   util_clock());
            // 数据有效,则等下次再获取
            agps_service_close_for_reconnect();
            return;
        }
    }
    else
    {
        //得到lbs信息后启动校时
        ret = gsm_get_cell_info(&lbs);
        if (GM_SUCCESS!=ret)
        {
            return;
        }
    }

    
    gm_socket_get_host_by_name_trigger(&s_agps_socket);
    system_state_get_ip_cache(SOCKET_INDEX_AGPS, IP);
    if(GM_SUCCESS == gm_is_valid_ip(IP))
    {
        GM_memcpy( s_agps_socket.ip , IP, sizeof(IP));
        agps_service_transfer_status(SOCKET_STATUS_CONNECTING);
        if(GM_SUCCESS == gm_socket_connect(&s_agps_socket))
        {
        }
        // else do nothing .   connecting_proc will deal.
    }
    else if((!s_agps_socket.excuted_get_host) && (GM_SUCCESS == gm_is_valid_ip(s_agps_socket.ip)))
    {
        agps_service_transfer_status(SOCKET_STATUS_CONNECTING);
        if(GM_SUCCESS == gm_socket_connect(&s_agps_socket))
        {
        }
        // else do nothing .   connecting_proc will deal.
    }
}

void agps_service_connection_failed(void)
{
    agps_service_close();
    
    if(s_agps_socket.status_fail_count >= MAX_CONNECT_REPEAT)
    {
        // if excuted get_host transfer to error statu, else get_host.
        if(s_agps_socket.excuted_get_host || (s_agps_socket.addr[0] == 0))
        {
            agps_service_transfer_status(SOCKET_STATUS_DATA_FINISH);
        }
        else
        {
            agps_service_transfer_status(SOCKET_STATUS_INIT);
        }
    }
    // else do nothing . wait connecting proc to deal.
}



static void agps_service_connecting_proc(void)
{
    u32 current_time = util_clock();

    if((current_time - s_agps_socket.send_time) > CONNECT_TIME_OUT)
    {
        s_agps_socket.status_fail_count ++;
        agps_service_connection_failed();

        if(s_agps_socket.status == SOCKET_STATUS_CONNECTING && 
            s_agps_socket.status_fail_count < MAX_CONNECT_REPEAT)
        {
            if(GM_SUCCESS == gm_socket_connect(&s_agps_socket))
            {
                //do nothing. wait callback
            }
        }
        
    }
}



void agps_service_connection_ok(void)
{
    agps_service_transfer_status(SOCKET_STATUS_WORK);
    agps_msg_send_timing();
}

void agps_service_close_for_reconnect(void)
{
    agps_service_close();
    agps_service_transfer_status(SOCKET_STATUS_DATA_FINISH);
}

static void agps_service_work_proc(void)
{
    u32 current_time = util_clock();
    u8 repeat_count;
    repeat_count = config_service_agps_socket_type() == STREAM_TYPE_STREAM?1:MAX_MESSAGE_REPEAT;
    
    if((current_time - s_agps_socket.send_time) >= MESSAGE_TIME_OUT)
    {
        if(s_agps_socket_extend.timing)
        {
            //发送请求阶段 
            s_agps_socket.status_fail_count ++;
            if(s_agps_socket.status_fail_count >= repeat_count)
            {
                LOG(INFO,"clock(%d) agps_service_work_proc failed:%d", util_clock(), s_agps_socket.status_fail_count);
                agps_service_close_for_reconnect();
            }
            else
            {
                agps_msg_send_timing();
            }
        }
        else
        {
            //发送请求frame阶段 
            s_agps_socket.status_fail_count ++;
            if(s_agps_socket.status_fail_count >= repeat_count)
            {
                LOG(INFO,"clock(%d) agps_service_work_proc failed:%d", util_clock(), s_agps_socket.status_fail_count);
                agps_service_close_for_reconnect();
            }
            else
            {
                agps_msg_send_get_file(AGPS_MSG_REQ_LENTH);
            }
        }
    }
    
    agps_msg_receive(&s_agps_socket);
    
    return;
}


static void agps_service_data_finish_proc(void)
{
    u32 current_time = util_clock();
    static u32 last_execute_time = 0;

    //在已定位/或休眠状态下没必要去获取agps数据,等gps模块请求了再获取
    if((GM_GPS_OFF == gps_get_state()) || (GM_GPS_FIX_3D <= gps_get_state()))
    {
        s_agps_socket_extend.required_agps = 0;
    }
    
    if(!s_agps_socket_extend.required_agps)
    {
        return;
    }

    //由于agps文件是多管半小时的, 所以提前拿文件没必要, 多加180是防止本地时间不太准
    if((s_agps_file_inuse.start_time > (util_get_utc_time()+180)) ||
        (s_agps_file_inuse.start_time+s_agps_file_inuse.last ) < util_get_utc_time() )
    {
        int gap = 3;
        
        // 距离上次取agps最少3 sec，以免不停循环取
        if((current_time - last_execute_time) >= gap)
        {
            last_execute_time = current_time;
            LOG(DEBUG,"clock(%d) agps_service_data_finish_proc start(%d) + last(%d) < now gap(%d).",
                util_clock(), s_agps_file_inuse.start_time, s_agps_file_inuse.last, gap);
            s_agps_file_extend.file_flag = 0;
            s_agps_socket_extend.data_len = 0;
            s_agps_socket_extend.data_start = 1;
            s_agps_socket_extend.timing = 0;
            agps_service_transfer_status(SOCKET_STATUS_INIT);
        }
    }
}


static void agps_msg_send_timing(void)
{
    u8 buff[100];
    u16 len = sizeof(buff);
    u16 idx = 0;  //current place

    agps_msg_pack_head(buff, &idx, len);  //9 bytes
    agps_msg_pack_timing(buff, &idx, len);  //72 bytes
    if((idx + 2) > len)
    {
        LOG(WARN,"clock(%d) agps_msg_send_timing assert(len(%d)) failed.", util_clock(), len);
        return;
    }
    agps_msg_pack_id_len(buff, AGPS_REQ_TIMING, idx); //2 bytes
    len=idx+2;  // 1byte checksum , 1byte 0xD
    
    if(GM_SUCCESS == gm_socket_send(&s_agps_socket, buff, len))
    {
        s_agps_socket.send_time = util_clock();
        s_agps_socket_extend.timing = 1;
        LOG(DEBUG,"clock(%d) agps_msg_send_timing msglen(%d)", util_clock(), len);
        return;
    }
    else
    {
        agps_service_close_for_reconnect();
    }
    return;
}

static void agps_msg_pack_head(u8 *pdata, u16 *idx, u16 len)
{
    u8 imei[GM_IMEI_LEN + 1] = {0};
    GM_ERRCODE ret = GM_SUCCESS;
    
    if((*idx) + 9 > len)
    {
        LOG(WARN,"clock(%d) agps_msg_pack_head assert(len(%d)) failed.", util_clock(), len);
        return;
    }
    
    pdata[(*idx)++] = PROTOCOL_HEADER_AGPS;
    pdata[(*idx)++] = PROTOCOL_HEADER_AGPS;
    (*idx) = (*idx) + 3; // id 1 , len 2

    if(GM_SUCCESS != (ret = gsm_get_imei(imei)))
    {
        LOG(INFO,"clock(%d) agps_msg_pack_head can not get imei, ret:%d.", util_clock(), ret);
    }
    
    if (0 == GM_strlen((const char *)imei))
    {
        GM_memset(imei, 0, sizeof(imei));
    }

    pdata[(*idx)++] = MERGEBCD(util_chr(imei[7]), util_chr(imei[8]));
    pdata[(*idx)++] = MERGEBCD(util_chr(imei[9]), util_chr(imei[10]));
    pdata[(*idx)++] = MERGEBCD(util_chr(imei[11]), util_chr(imei[12]));
    pdata[(*idx)++] = MERGEBCD(util_chr(imei[13]), util_chr(imei[14]));

}


static void agps_msg_pack_timing(u8 *pdata, u16 *idx, u16 len)
{
    u8 bcd_tim[6];
    ST_Time current_time;
    u8 zone;
    gm_cell_info_struct lbs;
    u16 k = 0;
    u16 cell_num=0;
    u16 lbsidx=0;
    GM_ERRCODE ret;
    
    GM_memset(&lbs,0, sizeof(lbs));
    
    if(((*idx) + 34 + 38) > len)
    {
        LOG(WARN,"clock(%d) agps_msg_pack_timing assert(len(%d)) failed.", util_clock(), len);
        return;
    }


    zone = config_service_get_zone();
    util_get_current_local_time(bcd_tim, &current_time, zone);
    
    // 占用6个字节 YMDHMS:年月日时分秒
    GM_memcpy(&pdata[(*idx)], bcd_tim, sizeof(bcd_tim));
    (*idx) += 6;

    // 纬度   由于每台车位置不同, 所以下面的位置数据没有意义
    pdata[(*idx)++] = 0x02;
    pdata[(*idx)++] = 0x23;
    pdata[(*idx)++] = 0x24;
    pdata[(*idx)++] = 0x78;
    
    // 经度
    pdata[(*idx)++] = 0x11;
    pdata[(*idx)++] = 0x35;
    pdata[(*idx)++] = 0x70;
    pdata[(*idx)++] = 0x84;
    
    // 速度
    pdata[(*idx)++] = 0x00;
    pdata[(*idx)++] = 0x00;

     // 角度
    pdata[(*idx)++] = 0x01;
    pdata[(*idx)++] = 0x80;

    GM_memset(&pdata[(*idx)], 0xFF, 16);
    (*idx) += 16;

    //基站数据
    pdata[(*idx)++] = 0x00;
    pdata[(*idx)++] = 0x24;

    pdata[(*idx)++] = 0x00;
    pdata[(*idx)++] = 0xA9;


    ret = gsm_get_cell_info(&lbs);
    LOG(DEBUG,"clock(%d) agps_msg_pack_timing ret(%d) lbs(%d).", util_clock(), ret, lbs.nbr_cell_num);
    if (lbs.nbr_cell_num > 5)
    {
        cell_num = 5;
    }
    else
    {
        cell_num = lbs.nbr_cell_num;
    }
    
    //  MCC(2B) + MNC(1B) + 基站数(1B)
    pdata[(*idx)++] = BHIGH_BYTE(lbs.serv_info.mcc);  // MCC
    pdata[(*idx)++] = BLOW_BYTE(lbs.serv_info.mcc);
    pdata[(*idx)++] = BLOW_BYTE(lbs.serv_info.mnc); // MNC
    pdata[(*idx)++] = cell_num+1;  // 基站数
    
    
    // 主服务基站信息  LAC(2B)+CI(3B)+RSSI(1B)
    pdata[(*idx)++] = BHIGH_BYTE(lbs.serv_info.lac);  // LAC
    pdata[(*idx)++] = BLOW_BYTE(lbs.serv_info.lac);
    pdata[(*idx)++] = BHIGH_BYTE(lbs.serv_info.ci);  // CELL ID
    pdata[(*idx)++] = BLOW_BYTE(lbs.serv_info.ci);
    pdata[(*idx)++] = lbs.serv_info.rxlev;// 信号强度      RSSI
    
    
    // 剩余4个基站 LAC(2B)+CI(3B)+RSSI(1B)
    lbsidx = (*idx);
    for (k=0; k<cell_num; k++) // 24B
    {
        pdata[lbsidx++] = BHIGH_BYTE(lbs.nbr_cell_info[k].lac);  // LAC
        pdata[lbsidx++] = BLOW_BYTE(lbs.nbr_cell_info[k].lac);
        
        pdata[lbsidx++] = BHIGH_BYTE(lbs.nbr_cell_info[k].ci);  // CELL ID
        pdata[lbsidx++] = BLOW_BYTE(lbs.nbr_cell_info[k].ci);
        
        pdata[lbsidx++] = lbs.nbr_cell_info[k].rxlev;// 信号强度
    }
    
    (*idx) = (*idx) + 25;   // 5个基站空间 每个5字节
}


static void agps_msg_pack_id_len(u8 *pdata, u8 id, u16 len)
{
    u16 idx = 0;
    u8 checksum = 0;
    u16 total = 0;
    u16 len_send = 0;

    if(5 > len)
    {
        LOG(WARN,"clock(%d) update_msg_pack_id_len assert(len(%d)) failed.", util_clock(), len);
        return;
    }

    //len now is pointing checksum byte
    total = 2 + len;
    len_send = total - 5;
    
    pdata[2] = id;         //id   1byte
    pdata[3] = BHIGH_BYTE(len_send);       //length  2bytes
    pdata[4] = BLOW_BYTE(len_send);

    //from head to end_of_data
    for(idx = 0; idx < len; ++ idx)
    {
        checksum ^= pdata[idx];
    }
    
    pdata[len] = checksum;   // xor value.   1byte

    pdata[total - 1] = PROTOCOL_TAIL_AGPS;  //end
}


static void agps_msg_receive(SocketType *socket)
{
    u8 head[7];
    u32 len = sizeof(head);
    u16 msg_len;
    u8 *pdata;
    static u32 packet_error_start = 0;

    /*
    update协议   最短7
        信息头(0x68 0x68)    2 协议号1    包长度2(下一字节至data_end)         check2 0xD
    */

    if(GM_SUCCESS != fifo_peek(&socket->fifo, head, len))
    {
        // no msg
        return;
    }

    msg_len = MKWORD(head[3], head[4]);
    msg_len = msg_len + 5;

    if(msg_len > MAX_GPRS_MESSAGE_LEN)
    {
        LOG(WARN,"clock(%d) agps_msg_receive assert(msg_len(%d)) failed.",util_clock(), msg_len);
        //clear fifo and restart socket.
        fifo_reset(&socket->fifo);
        gm_socket_close_for_reconnect(socket);
        return;
    }
    
    pdata = GM_MemoryAlloc(msg_len);
    if(pdata == NULL)
    {
        LOG(WARN,"clock(%d) agps_msg_receive alloc buf failed. len:%d", util_clock(), msg_len);

        //clear fifo and restart socket.
        fifo_reset(&socket->fifo);
        gm_socket_close_for_reconnect(socket);
        return;
    }

    if(GM_SUCCESS != fifo_peek(&socket->fifo, pdata, msg_len))
    {
        // GM_EMPTY_BUF
        GM_MemoryFree(pdata);
        
        if(packet_error_start == 0)
        {
            LOG(DEBUG,"clock(%d) agps_msg_receive get msg failed. len:%d", util_clock(), msg_len);
            log_service_print_hex((const char*)head, sizeof(head));
            packet_error_start = util_clock();
        }
        else
        {
            if((util_clock() - packet_error_start) > MAX_GPRS_PART_MESSAGE_TIMEOUT)
            {
                LOG(WARN,"clock(%d) agps_msg_receive MAX_GPRS_PART_MESSAGE_TIMEOUT.",util_clock());
                //clear fifo and restart socket.
                fifo_reset(&socket->fifo);
                gm_socket_close_for_reconnect(socket);
                packet_error_start = 0;
            }
        }
        return;
    }
    fifo_pop_len(&socket->fifo, msg_len);

	LOG(DEBUG,"clock(%d) agps_msg_receive msg len(%d)", util_clock(), msg_len);


    agps_msg_parse(pdata, msg_len);
    GM_MemoryFree(pdata);
}



static void agps_msg_parse(u8 *pdata, u16 len)
{
    u8 checksum = 0;
    u16 idx = 0;
    
    if(PROTOCOL_HEADER_AGPS != pdata[0] || PROTOCOL_HEADER_AGPS != pdata[1])
    {
        LOG(WARN,"clock(%d) agps_msg_parse assert(header(%02x %02x)) failed.", util_clock(), pdata[0],pdata[1]);
        return;
    }
    if(PROTOCOL_TAIL_AGPS != pdata[len-1] )
    {
        LOG(WARN,"clock(%d) update_msg_parse assert(tail(%d)) failed.", util_clock(), pdata[len-1] );
        return;
    }

    for(idx = 0; idx < (len-2); ++idx)
    {
        checksum ^= pdata[idx];
    }

    if(checksum != pdata[len-2])
    {
        LOG(WARN,"clock(%d) update_msg_parse assert(xor(%d != %d)) failed.", util_clock(), checksum, pdata[len-2]);
        return;
    }
    
    //协议号
    switch(pdata[2])
    {
        case AGPS_ACK_TIMING:
            agps_msg_parse_timing_ack(pdata,len);
            break;
        case AGPS_EPO_ACK:
            agps_msg_parse_epo_ack(pdata,len);
            break;
        case AGPS_TAIDOU_ACK:
            agps_msg_parse_taidou_ack(pdata,len);
            break;
        case AGPS_ZKW_ACK:
            agps_msg_parse_zkw_ack(pdata,len);
            break;
        default:
            LOG(WARN,"clock(%d) agps_msg_parse assert(msgid(%d)) failed.", util_clock(), pdata[2]);
            break;
    }
    return;
}


static void agps_msg_parse_timing_ack(u8 *pdata, u16 len)
{
    ST_Time mt;
    struct tm tm_t;
    time_t t;
    bool write_agps_succ = false;

    if(! s_agps_socket_extend.timing)
    {
        LOG(WARN,"clock(%d) agps_msg_parse_timing_ack assert(timing) failed.", util_clock());
        return;
    }
    
    if (len < 24)
    {
        LOG(WARN,"clock(%d) agps_msg_parse_timing_ack assert(len(%d)) failed.", util_clock(), len);
        return;
    }
    
    //服务器时间为东八区时间
    mt.year = 2000 + BCD2HEX(pdata[9]);
    mt.month = BCD2HEX(pdata[10]);
    mt.day = BCD2HEX(pdata[11]);
    
    mt.hour = BCD2HEX(pdata[12]);
    mt.minute = BCD2HEX(pdata[13]);
    mt.second= BCD2HEX(pdata[14]);
	mt.timezone = 0;
    LOG(DEBUG,"clock(%d) agps_msg_parse_timing_ack GM_SetLocalTime(%d-%02d-%02d %02d:%02d:%02d)).",
        util_clock(), mt.year,mt.month,mt.day,mt.hour,mt.minute,mt.second);

    util_mtktime_to_tm(&mt, &tm_t);
    t = util_mktime(&tm_t);
    if(t == (time_t)-1)
    {
        LOG(INFO,"clock(%d) agps_msg_parse_timing_ack GM_SetLocalTime(%d-%02d-%02d %02d:%02d:%02d)) failed.",
            util_clock(), mt.year,mt.month,mt.day,mt.hour,mt.minute,mt.second);
        return;
    }

    t -= 28800; //服务器是东八区 8*60*60
    tm_t = *util_localtime(&t);
    util_tm_to_mtktime(&tm_t, &mt);
            

    //如果agps文件中带有润秒数,则会覆盖这里的赋值
    s_agps_file_extend.leap_seconds = pdata[15];

    //有线设备只有休眠(静止超过5分钟才关gps)
    if (!gps_is_fixed())
    {
        LOG(INFO,"clock(%d) agps_msg_parse_timing_ack leap(%d) time(%d-%02d-%02d %02d:%02d:%02d)).", 
            util_clock(), s_agps_file_extend.leap_seconds,mt.year,mt.month,mt.day,mt.hour,mt.minute,mt.second);
        GM_SetLocalTime(&mt);
    }
    else
    {
        LOG(INFO,"clock(%d) agps_msg_parse_timing_ack leap(%d) ignore(%d-%02d-%02d %02d:%02d:%02d)).", 
            util_clock(), s_agps_file_extend.leap_seconds,mt.year,mt.month,mt.day,mt.hour,mt.minute,mt.second);
    }
    

    //经纬度用于1分钟后不能定位时上报基站数据
    s_agps_file_extend.lat = (BCD_LOW(pdata[16])*1000000 +BCD_HIGH(pdata[17])*100000 + (BCD_LOW(pdata[17])*10000 + 
        BCD_HIGH(pdata[18])*1000 + BCD_LOW(pdata[18])*100 + BCD_HIGH(pdata[19])*10 + BCD_LOW(pdata[19]))*100.0f/60.0f);
    s_agps_file_extend.lat /= 100000.0f;
    s_agps_file_extend.lat =(pdata[16] & 0x80) ? (-s_agps_file_extend.lat) : (s_agps_file_extend.lat);
    s_agps_file_extend.lng =  (BCD_HIGH(pdata[20] & 0x7F)*10000000 + BCD_LOW(pdata[20])*1000000 + 
        BCD_HIGH(pdata[21])*100000 + (BCD_LOW(pdata[21])*10000 + BCD_HIGH(pdata[22])*1000 + 
        BCD_LOW(pdata[22])*100 + BCD_HIGH(pdata[23])*10 + BCD_LOW(pdata[23]))*100.0f/60.0f);
    s_agps_file_extend.lng /= 100000.0f;
    s_agps_file_extend.lng = (pdata[20] & 0x80) ? (-s_agps_file_extend.lng) : (s_agps_file_extend.lng);

                                                                        
    s_agps_socket_extend.timing = 0;
    s_agps_file_extend.file_flag = 0;
    s_agps_socket_extend.data_len = 0;
    s_agps_socket_extend.data_start = 1;

    //校时完,本地有文件的话,先用本地文件
    write_agps_succ = agps_service_write_to_gps();
    {
        //report logs 
        JsonObject* p_json_log = json_create();
        json_add_string(p_json_log,"event","write_agps2");
        json_add_int(p_json_log,"start",s_agps_file_inuse.start_time);
        json_add_int(p_json_log,"last",s_agps_file_inuse.last);
        json_add_double(p_json_log,"lng",s_agps_file_inuse.lng);
        json_add_double(p_json_log,"lat",s_agps_file_inuse.lat);
        json_add_int(p_json_log,"ret",write_agps_succ?1:0);
        log_service_upload(INFO, p_json_log);
    }
    

    //由于agps文件是多管半小时的, 所以提前拿文件没必要, 多加180是防止本地时间不太准
    if((s_agps_file_inuse.start_time <= (util_get_utc_time()+180)) &&
        (s_agps_file_inuse.start_time+s_agps_file_inuse.last ) > util_get_utc_time() )
    {
        // 数据有效,则等下次再获取
        agps_service_close_for_reconnect();
    }
    else
    {
        agps_msg_send_get_file(AGPS_MSG_REQ_LENTH);
    }
}


static void agps_msg_send_get_file(u16 data_len)
{
    u8 buff[100];
    u16 len = sizeof(buff);
    u16 idx = 0;  //current place
    u8 cmd = 0;
	
    switch(config_service_get_gps_type())
    {
        case GM_GPS_TYPE_MTK_EPO:
            cmd = AGPS_REQ_EPO;
            break;           
        case GM_GPS_TYPE_TD_AGPS:
            cmd = AGPS_REQ_TAIDOU;
            break;           
        case GM_GPS_TYPE_AT_AGPS:
            cmd = AGPS_REQ_ZKW;
            break; 
        default:
            LOG(WARN,"clock(%d) agps_msg_send_get_file assert(config_service_get_gps_type(%d) failed.",util_clock(),config_service_get_gps_type());
            return;
    }

    agps_msg_pack_head(buff, &idx, len);  //9 bytes
    agps_msg_pack_get_file(buff, &idx, len,data_len);  //6
    if((idx + 2) > len)
    {
        LOG(WARN,"clock(%d) agps_msg_send_get_file assert(len(%d)) failed.", util_clock(), len);
        return;
    }
    agps_msg_pack_id_len(buff, cmd, idx);  //2bytes
    len=idx+2;  // 1byte checksum , 1byte 0xD
    
    if(GM_SUCCESS == gm_socket_send(&s_agps_socket, buff, len))
    {
        s_agps_socket.send_time = util_clock();
        LOG(DEBUG,"clock(%d) agps_msg_send_get_file cmd(%d) msglen(%d) start(%d) total(%d),get_len(%d)", 
            util_clock(), cmd, len, s_agps_socket_extend.data_start, s_agps_socket_extend.data_len, data_len);
        return;
    }
    else
    {
        agps_service_close_for_reconnect();
    }
    return;
}

static void agps_msg_pack_get_file(u8 *pdata, u16 *idx, u16 len,u16 data_len)
{
    u32 data_start = 0;

    data_start = s_agps_socket_extend.data_start;
    
    pdata[(*idx)++] = BHIGH_BYTE(WHIGH_WORD(data_start));
    pdata[(*idx)++] = BLOW_BYTE(WHIGH_WORD(data_start));
    pdata[(*idx)++] = BHIGH_BYTE(WLOW_WORD(data_start));
    pdata[(*idx)++] = BLOW_BYTE(WLOW_WORD(data_start));
    
    pdata[(*idx)++] = BHIGH_BYTE(data_len);
    pdata[(*idx)++] = BLOW_BYTE(data_len);
}


static void agps_msg_parse_epo_ack(u8 *pdata, u16 len)
{
    u32 cur_epo_hour = 0;
    u32 cur_rtc_hour = 0;
    u32 file_flag;
    u16 data_len = 0;
    file_flag = MKDWORD(pdata[9], pdata[10], pdata[11], pdata[12]);
    
    if(s_agps_file_extend.file_flag == 0)
    {
        //首包
        cur_epo_hour = MKDWORD(0,pdata[21],pdata[20],pdata[19]);
        
        cur_rtc_hour = (util_get_utc_time() - SECONDS_FROM_UTC_TO_GPS_START)/3600;
        LOG(DEBUG,"clock(%d) agps_msg_parse_epo_ack cur_rtc_hour(%d)  cur_epo_hour(%d).", util_clock(), cur_rtc_hour, cur_epo_hour);
        if(cur_rtc_hour < cur_epo_hour)
        {
            cur_rtc_hour = cur_epo_hour;  //本地时间落后时直接取文件
        }
        s_agps_socket_extend.data_start = ((cur_rtc_hour - cur_epo_hour) / 6 ) * EPO_GPS_DATA_6HOURS_LENTH + 1;
        s_agps_file_extend.file_flag = file_flag;
        s_agps_socket_extend.data_len = EPO_GPS_DATA_6HOURS_LENTH;

        //要先取整(/6) 再乘6 才是正确的开始时间
        s_agps_file_extend.start_time = (cur_rtc_hour/6)*6*3600 + SECONDS_FROM_UTC_TO_GPS_START;
        s_agps_file_extend.last = SIX_HOUR_SECONDS;
        s_agps_file_extend.gps_dev_type = (u8)GM_GPS_TYPE_MTK_EPO;
        s_agps_file_extend.len = 0;
        
        LOG(DEBUG,"clock(%d) agps_msg_parse_epo_ack start(%d)  total(%d).", util_clock(), s_agps_socket_extend.data_start, s_agps_socket_extend.data_len);

        data_len = s_agps_socket_extend.data_len - s_agps_file_extend.len;
        data_len = data_len > AGPS_MSG_REQ_LENTH?AGPS_MSG_REQ_LENTH:data_len;
        agps_msg_send_get_file(data_len);
    }
    else
    {
        if(file_flag != s_agps_file_extend.file_flag)
        {
            LOG(WARN,"clock(%d) agps_msg_parse_taidou_ack file_flag changed(%04x->%04x) current start %d.",util_clock(), 
                s_agps_file_extend.file_flag, file_flag, s_agps_socket_extend.data_start);
            s_agps_file_extend.file_flag = 0;
            s_agps_socket_extend.data_len = 0;
            s_agps_socket_extend.data_start = 1;
            agps_msg_send_get_file(AGPS_MSG_REQ_LENTH);
            return;
        }
        
        //后继包
        GM_memcpy(&s_agps_file_extend.data[s_agps_file_extend.len], &pdata[19], len - AGPS_MSG_PACKAGE_LENTH);
        s_agps_file_extend.len += (len - AGPS_MSG_PACKAGE_LENTH);
        s_agps_socket_extend.data_start += (len - AGPS_MSG_PACKAGE_LENTH);

        LOG(DEBUG,"clock(%d) agps_msg_parse_epo_ack file_len(%d) start(%d).",
            util_clock(),s_agps_file_extend.len, s_agps_socket_extend.data_start);

        if(s_agps_file_extend.len < s_agps_socket_extend.data_len)
        {
            data_len = s_agps_socket_extend.data_len - s_agps_file_extend.len;
            data_len = data_len > AGPS_MSG_REQ_LENTH?AGPS_MSG_REQ_LENTH:data_len;
            agps_msg_send_get_file(data_len);
        }
        else
        {
            s_agps_file_extend.len = s_agps_socket_extend.data_len;
            agps_write_data_file(EPO_FILE_PATH);
            agps_service_close_for_reconnect();
        }
    }
}

static void agps_msg_parse_taidou_ack(u8 *pdata, u16 len)
{
    u16 idx = 0;
    u8 item_num = 0;
    u32 file_flag;
    struct tm agps_time;
    time_t agps_utc;
    u16 data_len = 0;

    if (len <= AGPS_MSG_PACKAGE_LENTH)
    {
        LOG(WARN,"clock(%d) agps_msg_parse_taidou_ack assert(len(%d)) failed.", util_clock(), len);
        return;
    }

    file_flag = MKDWORD(pdata[9], pdata[10], pdata[11], pdata[12]);

    if(s_agps_socket_extend.data_start == 1)
    {
        if(0 == GM_memcmp(&pdata[19],s_techtotop_flag,sizeof(s_techtotop_flag)))
        {
            //首包
            s_agps_file_extend.file_flag = file_flag;
            s_agps_file_extend.gps_dev_type = (u8)GM_GPS_TYPE_TD_AGPS;

            idx = 19 + 36;
            item_num = agps_data_check_item_num(&pdata[idx], len-idx);
            if (4 == item_num)
            {
                s_agps_socket_extend.data_len = (pdata[idx] & 0x0F) * 1000 + (pdata[idx+1] & 0x0F) * 100 + (pdata[idx+2] & 0x0F) * 10 + (pdata[idx+3] & 0x0F) - 1;
            }
            if ((26 >= s_agps_socket_extend.data_len) || (4 != item_num))
            {
                LOG(WARN,"clock(%d) agps_msg_parse_taidou_ack assert(data_len(%d,%d)) failed.", util_clock(), s_agps_socket_extend.data_len,item_num);
                agps_service_close_for_reconnect();
                return;
            }
            idx += (item_num + 1);
            if ((0x23 == pdata[idx]) && (0x3E == pdata[idx+1]) && (0x04 == pdata[idx+2]) && (0x02 == pdata[idx+3]))
            {
                // 泰斗的闰秒是BDT润秒+28
                s_agps_file_extend.leap_seconds = pdata[idx +6];
                agps_time.tm_year = MKWORD(pdata[idx +8], pdata[idx +7]) - 1900;
                agps_time.tm_mon = pdata[idx +9] - 1;
                agps_time.tm_mday = pdata[idx +10];
                agps_time.tm_hour = pdata[idx +11];
                agps_time.tm_min = pdata[idx +12];
                agps_time.tm_sec = pdata[idx +13];
                agps_time.tm_isdst = 0;

                agps_utc = util_mktime(&agps_time);
                // agps_utc -= 28800; // 东8区
                if(agps_utc <= (time_t)0)
                {
                    LOG(WARN,"clock(%d) agps_msg_parse_taidou_ack assert(agps_time(%d-%02d-%02d %02d:%02d:%02d)) failed.", 
                        util_clock(),MKWORD(pdata[idx +8], pdata[idx +7]),pdata[idx +9],pdata[idx +10],pdata[idx +11],pdata[idx +12],pdata[idx +13]);
                    agps_service_close_for_reconnect();
                    return;
                }
                
                s_agps_file_extend.start_time = agps_utc;
                s_agps_file_extend.last = TWO_HOUR_SECONDS;
                idx += 26;
            }
            else
            {
                LOG(WARN,"clock(%d) agps_msg_parse_taidou_ack assert(agps_time) failed.",util_clock());
                agps_service_close_for_reconnect();
                return;
            }
            s_agps_socket_extend.data_len -= 26;
            LOG(DEBUG,"clock(%d) agps_msg_parse_taidou_ack agps_time(%d-%02d-%02d %02d:%02d:%02d) leap(%d) file_total(%d).",
                util_clock(),agps_time.tm_year + 1900, agps_time.tm_mon+1,agps_time.tm_mday,agps_time.tm_hour,agps_time.tm_min,agps_time.tm_sec,
                s_agps_file_extend.leap_seconds, s_agps_socket_extend.data_len);
            
            if (len <= (idx + 2))
            {
                LOG(WARN,"clock(%d) agps_msg_parse_taidou_ack assert(len(%d>%d)) failed.",util_clock(),len, (idx + 2));
                agps_service_close_for_reconnect();
                return;
            }
            
            s_agps_file_extend.len = len - idx - 2;  // idx include 19 bytes header
            GM_memcpy(&s_agps_file_extend.data[0], &pdata[idx], s_agps_file_extend.len);
            //data_start从开始
            s_agps_socket_extend.data_start = len - AGPS_MSG_PACKAGE_LENTH + 1;
            LOG(DEBUG,"clock(%d) agps_msg_parse_taidou_ack from(%d) file_len(%d) start_time(%d).",
                util_clock(),idx, s_agps_file_extend.len, s_agps_socket_extend.data_start);

            data_len = s_agps_socket_extend.data_len - s_agps_file_extend.len;
            data_len = data_len > AGPS_MSG_REQ_LENTH?AGPS_MSG_REQ_LENTH:data_len;
            agps_msg_send_get_file(data_len);
        }
        else
        {
            LOG(WARN,"clock(%d) agps_msg_parse_taidou_ack assert(s_techtotop_flag) failed.",util_clock());
            agps_service_close_for_reconnect();
            return;
        }
    }
    else
    {
        if(file_flag != s_agps_file_extend.file_flag)
        {
            LOG(WARN,"clock(%d) agps_msg_parse_taidou_ack file_flag changed(%04x->%04x) current start %d.",util_clock(), 
                s_agps_file_extend.file_flag, file_flag, s_agps_socket_extend.data_start);
            s_agps_file_extend.file_flag = 0;
            s_agps_socket_extend.data_len = 0;
            s_agps_socket_extend.data_start = 1;
            agps_msg_send_get_file(AGPS_MSG_REQ_LENTH);
            return;
        }
        
        //后继包
        GM_memcpy(&s_agps_file_extend.data[s_agps_file_extend.len], &pdata[19], len - AGPS_MSG_PACKAGE_LENTH);
        s_agps_file_extend.len += (len - AGPS_MSG_PACKAGE_LENTH);
        s_agps_socket_extend.data_start += (len - AGPS_MSG_PACKAGE_LENTH);
        
        LOG(DEBUG,"clock(%d) agps_msg_parse_taidou_ack file_len(%d) start(%d).",
            util_clock(),s_agps_file_extend.len, s_agps_socket_extend.data_start);

        if(s_agps_file_extend.len < s_agps_socket_extend.data_len)
        {
            data_len = s_agps_socket_extend.data_len - s_agps_file_extend.len;
            data_len = data_len > AGPS_MSG_REQ_LENTH?AGPS_MSG_REQ_LENTH:data_len;
            agps_msg_send_get_file(data_len);
        }
        else
        {
            s_agps_file_extend.len = s_agps_socket_extend.data_len;
            agps_write_data_file(AGPS_FILE_PATH);
            agps_service_close_for_reconnect();
        }
    }
}

static void agps_msg_parse_zkw_ack(u8 *pdata, u16 len)
{
    u16 idx = 0;
    u8 item_num = 0;
    u32 file_flag;
    u16 data_len = 0;
    file_flag = MKDWORD(pdata[9], pdata[10], pdata[11], pdata[12]);

    if(s_agps_socket_extend.data_start == 1)
    {
        if(0 == GM_memcmp(&pdata[19],s_at6558_flag,sizeof(s_at6558_flag)))
        {
            //首包
            s_agps_file_extend.file_flag = file_flag;
            s_agps_file_extend.gps_dev_type = (u8)GM_GPS_TYPE_AT_AGPS;

            idx = 19 + 35;
            item_num = goome_agps_at6558_check_item_num(&pdata[idx], len-idx);
            if (4 == item_num)
            {
                s_agps_socket_extend.data_len = (pdata[idx] & 0x0F) * 1000 + (pdata[idx+1] & 0x0F) * 100 + (pdata[idx+2] & 0x0F) * 10 + (pdata[idx+3] & 0x0F);
            }
            else
            {
                LOG(WARN,"clock(%d) agps_msg_parse_zkw_ack assert(data_len(%d,%d)) failed.",util_clock(), s_agps_socket_extend.data_len,item_num);
                agps_service_close_for_reconnect();
                return;
            }
            idx += (item_num+2);
            item_num = goome_agps_at6558_check_item_num(&pdata[idx], len-idx);
            idx += (item_num+2);  //idx减header长度 是s_agps_file_extend.data数据应该开始的位置
            
            s_agps_file_extend.start_time = file_flag; //取服务器时间, 据説四小时内有效,但这里只取2小时
            s_agps_file_extend.last = TWO_HOUR_SECONDS - 1800; //服务器每半小时获取一次数据,所以减半小时

            s_agps_file_extend.len = len - idx - 2;  // idx include 19 bytes header
            GM_memcpy(&s_agps_file_extend.data[0], &pdata[idx], s_agps_file_extend.len);
            //data_start从开始
            s_agps_socket_extend.data_start = len - AGPS_MSG_PACKAGE_LENTH + 1;
            LOG(DEBUG,"clock(%d) agps_msg_parse_zkw_ack file_len(%d) start(%d).",
                util_clock(),s_agps_file_extend.len, s_agps_socket_extend.data_start);

            data_len = s_agps_socket_extend.data_len - s_agps_file_extend.len;
            data_len = data_len > AGPS_MSG_REQ_LENTH?AGPS_MSG_REQ_LENTH:data_len;
            agps_msg_send_get_file(data_len);
        }
        else
        {
            LOG(WARN,"clock(%d) agps_msg_parse_zkw_ack assert(s_at6558_flag) failed.",util_clock());
            agps_service_close_for_reconnect();
            return;
        }
    }
    else
    {
        if(file_flag != s_agps_file_extend.file_flag)
        {
            LOG(WARN,"clock(%d) agps_msg_parse_taidou_ack file_flag changed(%04x->%04x) current start %d.",util_clock(), 
                s_agps_file_extend.file_flag, file_flag, s_agps_socket_extend.data_start);
            s_agps_file_extend.file_flag = 0;
            s_agps_socket_extend.data_len = 0;
            s_agps_socket_extend.data_start = 1;
            agps_msg_send_get_file(AGPS_MSG_REQ_LENTH);
            return;
        }
        
        //后继包
        GM_memcpy(&s_agps_file_extend.data[s_agps_file_extend.len], &pdata[19], len - AGPS_MSG_PACKAGE_LENTH);
        s_agps_file_extend.len += (len - AGPS_MSG_PACKAGE_LENTH);
        s_agps_socket_extend.data_start += (len - AGPS_MSG_PACKAGE_LENTH);

        LOG(DEBUG,"clock(%d) agps_msg_parse_zkw_ack file_len(%d) start(%d).",
            util_clock(),s_agps_file_extend.len, s_agps_socket_extend.data_start);
        
        if(s_agps_file_extend.len < s_agps_socket_extend.data_len)
        {
            data_len = s_agps_socket_extend.data_len - s_agps_file_extend.len;
            data_len = data_len > AGPS_MSG_REQ_LENTH?AGPS_MSG_REQ_LENTH:data_len;
            agps_msg_send_get_file(data_len);
        }
        else
        {
            s_agps_file_extend.len = s_agps_socket_extend.data_len;
            agps_write_data_file(AGPS_FILE_PATH);
            agps_service_close_for_reconnect();
        }
    }
}


static u8 agps_data_check_item_num(u8 *pdata, u16 len)
{
    u16 idx = 0;
    u16 index = 0;
    
    for (idx=0; idx<len; idx++)
    {
        if (0x0A == pdata[idx])
        {
            break;
        }
        
        index++;
    }
    
    return index;
}

//用于检测DataLength:后面的数据长度有几个字节,防止长度不是4位数
static u8 goome_agps_at6558_check_item_num(u8 *pdata, u16 len)
{
    u16 idx = 0;
    u16 index = 0;
    
    for (idx=0; idx<len; idx++)
    {
        if ((0x2E == pdata[idx]) && (0x0A == pdata[idx+1]))
        {
            break;
        }
        
        index++;
    }
    
    return index;
}


static void agps_write_data_file(const U16 * file_name)
{
    u32 writen = 0;
    u32 writen_total = 0;
    u32 will_write = 0;
    int handle = -1;
    u32 fs_len = s_agps_file_extend.len + AGPS_FILE_EXTEND_HEAD_SIZE;
    int ret;
	u8 * p;
    bool write_agps_succ = false;

    if(s_agps_file_extend.len > (sizeof(AgpsFileExtend) - AGPS_FILE_EXTEND_HEAD_SIZE ))
    {
        LOG(DEBUG,"clock(%d) agps_write_data_file assert(len(%d)) failed.",util_clock(),s_agps_file_extend.len);
        return;
    }
    GM_memcpy(&s_agps_file_inuse, &s_agps_file_extend, fs_len);

    //如果需要写串口,就写
    write_agps_succ = agps_service_write_to_gps();
    {
        //report logs 
        JsonObject* p_json_log = json_create();
        json_add_string(p_json_log,"event","write_agps3");
        json_add_int(p_json_log,"start",s_agps_file_inuse.start_time);
        json_add_int(p_json_log,"last",s_agps_file_inuse.last);
        json_add_int(p_json_log,"ret",write_agps_succ?1:0);
        log_service_upload(INFO, p_json_log);
    }

    p = (u8 * )&s_agps_file_extend.gps_dev_type;
    s_agps_file_extend.crc = applied_math_calc_common_crc16(p , (fs_len-2));
    LOG(DEBUG,"clock(%d) agps_write_data_file crc(%d) devtype(%d) leap(%d) flag(%d) len(%d) start(%d) lng(%f) lat(%f) last(%d).",
        util_clock(),s_agps_file_extend.crc,s_agps_file_extend.gps_dev_type,s_agps_file_extend.leap_seconds,
        s_agps_file_extend.file_flag,s_agps_file_extend.len,s_agps_file_extend.start_time,
        s_agps_file_extend.lng,s_agps_file_extend.lat,s_agps_file_extend.last);

    util_delete_file(file_name);
    handle = GM_FS_Open(file_name, GM_FS_READ_WRITE | GM_FS_ATTR_ARCHIVE | GM_FS_CREATE);
    if (handle < 0)
    {
        LOG(DEBUG,"clock(%d) agps_write_data_file assert(GM_FS_Open) failed. return %d.",util_clock(),handle);
        return;
    }


    // 预先占用指定长度
    p = (u8 * )&s_agps_file_extend.crc;
    will_write = fs_len;
    while(writen_total != fs_len)
    {
        if((fs_len - writen_total) < will_write)
        {
            will_write = (fs_len - writen_total);
        }
        
        ret = GM_FS_Write(handle, (void *)(p + writen_total), will_write, &writen);
        if ((ret < 0) || (writen==0))
        {
            LOG(DEBUG,"clock(%d) agps_write_data_file assert(GM_FS_Write(ret:%d, writen:%d)) failed.",util_clock(), ret, writen);
            GM_FS_Close(handle);
            handle = -1;
            return;
        }
        writen_total += writen;
    }
    GM_FS_Close(handle);
    handle = -1;
    LOG(DEBUG,"clock(%d) agps_write_data_file length(%d).",util_clock(),fs_len);
    return;
}



static void agps_read_data_file(const U16 * file_name)
{
    u32 will_read = 0;
    u32 read_total = 0;
    u32 read_len = 0;
    int handle = -1;
    int ret;

    handle = GM_FS_Open(file_name, GM_FS_READ_ONLY | GM_FS_ATTR_ARCHIVE );
    if (handle < 0)
    {
        GM_memset(&s_agps_file_inuse, 0, sizeof(s_agps_file_inuse));
        return;
    }

    
    ret = GM_FS_Read(handle, (void *)&s_agps_file_inuse.crc, AGPS_FILE_EXTEND_HEAD_SIZE, &read_len);
    if (ret != 0 || AGPS_FILE_EXTEND_HEAD_SIZE != read_len)
    {
        LOG(INFO,"clock(%d) agps_read_data_file GM_FS_Read failed, ret:%d, read_len:%d.",util_clock(), ret, read_len);
        GM_FS_Close(handle);
        handle = -1;
        GM_memset(&s_agps_file_inuse, 0, sizeof(s_agps_file_inuse));
        return;
    }

    LOG(DEBUG,"clock(%d) agps_read_data_file crc(%d) devtype(%d) leap(%d) flag(%d) len(%d) start(%d) lng(%f) lat(%f) last(%d).",
        util_clock(),s_agps_file_inuse.crc,s_agps_file_inuse.gps_dev_type,s_agps_file_inuse.leap_seconds,
        s_agps_file_inuse.file_flag,s_agps_file_inuse.len,s_agps_file_inuse.start_time,
        s_agps_file_inuse.lng,s_agps_file_inuse.lat,s_agps_file_inuse.last);

    //长度合法
    if(s_agps_file_inuse.len > sizeof(s_agps_file_inuse.data))
    {
        LOG(INFO,"clock(%d) agps_read_data_file assert(datalen(%d)) failed.",util_clock(), s_agps_file_inuse.len);
        GM_FS_Close(handle);
        handle = -1;
        GM_memset(&s_agps_file_inuse, 0, sizeof(s_agps_file_inuse));
        return;
    }

    //读数据区
    read_total = 0;
    while(read_total !=  s_agps_file_inuse.len)
    {
        will_read = (s_agps_file_inuse.len - read_total);
        
        ret = GM_FS_Read(handle, (void *)(s_agps_file_inuse.data + read_total), will_read, &read_len);
        if ((ret < 0) || (read_len==0))
        {
            LOG(DEBUG,"clock(%d) agps_read_data_file assert(GM_FS_Read(ret:%d, read_len:%d)) failed.",util_clock(), ret, read_len);
            GM_FS_Close(handle);
            handle = -1;
            GM_memset(&s_agps_file_inuse, 0, sizeof(s_agps_file_inuse));
            return;
        }
        read_total += read_len;
    }
    GM_FS_Close(handle);
    handle = -1;

    if(s_agps_file_inuse.crc != applied_math_calc_common_crc16(&s_agps_file_inuse.gps_dev_type , s_agps_file_inuse.len + (AGPS_FILE_EXTEND_HEAD_SIZE - 2)))
    {
        LOG(DEBUG,"clock(%d) agps_read_data_file assert(GM_FS_Read(ret:%d, read_len:%d)) failed.",util_clock(), 
            s_agps_file_inuse.crc, applied_math_calc_common_crc16(&s_agps_file_inuse.gps_dev_type , s_agps_file_inuse.len + (AGPS_FILE_EXTEND_HEAD_SIZE - 2)));
        GM_memset(&s_agps_file_inuse, 0, sizeof(s_agps_file_inuse));
        return;
    }

	//s_agps_file_extend是刚刚从服务器获取的最新数据
	s_agps_file_inuse.lng = s_agps_file_extend.lng;
	s_agps_file_inuse.lat = s_agps_file_extend.lat;
	s_agps_file_inuse.leap_seconds = s_agps_file_extend.leap_seconds;
    //数据合法
    return;
}


bool agps_service_require_to_gps(AgpsToGpsEnum type,bool re_download)
{
    static u16 seg_index = 0;
    u16 seg_size = 0;
    u16 left_size = 0;
    GM_ERRCODE ret;

    if(! s_agps_socket_extend.required_agps)
    {
        s_agps_socket_extend.required_agps = 1;
    }
    
    if(0 == s_agps_file_inuse.start_time)
    {
        //等待校时
        LOG(DEBUG,"clock(%d) agps_service_require_to_gps waiting for timing.",util_clock());
        return false;;
    }
    
    //由于agps文件是多管半小时的, 所以提前拿文件没必要, 多加180是防止本地时间不太准
    if((s_agps_file_inuse.start_time > (util_get_utc_time()+180)) ||
        (s_agps_file_inuse.start_time+s_agps_file_inuse.last ) < util_get_utc_time() )
    {
        seg_index = 0;
        LOG(DEBUG,"clock(%d) agps_service_require_to_gps assert(s_agps_file_inuse) failed.",util_clock());
        return false;
    }

    if(type == AGPS_TO_GPS_LNGLAT_TIME)
    {
        ret = gps_write_agps_info(s_agps_file_inuse.lng, s_agps_file_inuse.lat, s_agps_file_inuse.leap_seconds,s_agps_file_inuse.start_time);
        seg_index = 0;
        LOG(DEBUG,"clock(%d) agps_service_require_to_gps write lng(%f),lat(%f),leap(%d).",
            util_clock(),s_agps_file_inuse.lng, s_agps_file_inuse.lat, s_agps_file_inuse.leap_seconds);
        return (GM_SUCCESS == ret);
    }

    if(re_download && seg_index > 0)
    {
        // 数据错误(re_download), 则重新获取
        agps_service_reset();
        seg_index = 0;
        return false;
    }
    

    switch(s_agps_file_inuse.gps_dev_type)
    {
        case GM_GPS_TYPE_MTK_EPO:
            seg_size = sizeof(U32)*18;   // 一次18个U32
            break;
        case GM_GPS_TYPE_TD_AGPS:
            seg_size = 73;   // 一次一个辅助信息长度
            break;
        case GM_GPS_TYPE_AT_AGPS:
            seg_size = s_agps_file_inuse.len;  //可以整个儿发送
            break;
        default:
            seg_index = 0;
            LOG(DEBUG,"clock(%d) agps_service_require_to_gps assert(dev_type(%d)) failed.",util_clock(),s_agps_file_inuse.gps_dev_type);
            return false;
    }

    // AGPS_TO_GPS_DATA
    left_size = s_agps_file_inuse.len - seg_index * seg_size;
    if(left_size > seg_size)
    {
		//序号要从1开始
        ret = gps_write_agps_data(seg_index + 1, &s_agps_file_inuse.data[seg_index * seg_size], seg_size);
        seg_index++;
        //LOG(DEBUG,"clock(%d) agps_service_require_to_gps write seg_size(%d).",util_clock(),seg_size);
        return (GM_SUCCESS == ret);
    }
    else
    {
        ret = gps_write_agps_data(seg_index + 1, &s_agps_file_inuse.data[seg_index * seg_size], left_size);

        seg_index = 0;

        LOG(DEBUG,"clock(%d) Wrote the last segment(%d).",util_clock(),left_size);
        s_agps_socket_extend.required_agps = 0;


        //false 中止agps_service_require_to_gps 后继回调
        return false;
    }

}

void agps_service_delele_file(void)
{
	util_delete_file(AGPS_FILE_PATH);
	util_delete_file(EPO_FILE_PATH);
}


static void agps_service_check_and_read_file(void)
{
    if(0 == s_agps_file_inuse.start_time)
    {
        switch(config_service_get_gps_type())
        {
            case GM_GPS_TYPE_MTK_EPO:
                agps_read_data_file(EPO_FILE_PATH);
                break;           
            case GM_GPS_TYPE_TD_AGPS:
                agps_read_data_file(AGPS_FILE_PATH);
                break;           
            case GM_GPS_TYPE_AT_AGPS:
                agps_read_data_file(AGPS_FILE_PATH);
                break; 
            default:
                LOG(WARN,"clock(%d) agps_service_check_and_read_file assert(config_service_get_gps_type(%d) failed.", util_clock(),config_service_get_gps_type());
                break;
        }
    }

    //lz add for print agps file content
    {
        u16 seg_size = 0;
        static u16 print_start = 0;
        u16 loop_count = 0;
        switch(config_service_get_gps_type())
        {
            case GM_GPS_TYPE_MTK_EPO:
                seg_size = sizeof(U32)*18;   // 一次18个U32
                break;           
            case GM_GPS_TYPE_TD_AGPS:
                seg_size = 73;   // 一次一个辅助信息长度
                break;           
            case GM_GPS_TYPE_AT_AGPS:
                seg_size = 100;  //可以整个儿发送
                break; 
            default:
                break;
        }
        for(; print_start < s_agps_file_inuse.len; print_start+=seg_size)
        {
            u16 print_len = seg_size;
            ++loop_count;
            if((print_start + print_len) >= s_agps_file_inuse.len)
            {
                print_len = s_agps_file_inuse.len - print_start - 1;
            }
            //log_service_print_hex((const char *)&s_agps_file_inuse.data[print_start], print_len);
            if(loop_count > 4)
            {
                break;
            }
        }
    }
}

float agps_service_get_unfix_lng(void)
{
    return s_agps_file_extend.lng;
}

float agps_service_get_unfix_lat(void)
{
    return s_agps_file_extend.lat;
}

/*
agps_service模块传送数据给gps后, 长时间无法响应, 一般是文件内容错, 要求重新获取.
*/
static void agps_service_reset(void)
{
	agps_service_delele_file();
	GM_memset(&s_agps_file_inuse, 0, sizeof(s_agps_file_inuse));
	GM_memset(&s_agps_file_extend, 0, sizeof(s_agps_file_extend));
    agps_service_transfer_status(SOCKET_STATUS_INIT);
}


