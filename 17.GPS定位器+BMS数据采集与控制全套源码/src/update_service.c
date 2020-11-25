/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        update_service.h
 * Author:           王志华       
 * Version:          1.0
 * Date:             2019-02-28
 * Description:      模块升级服务
 * Others:      
 * Function List:    
    1. 创建
    2. 销毁 
    3. 
 * History: 
    1. Date:         2019-02-28
       Author:       王志华
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#include "update_service.h"
#include "update_file.h"
#include "config_service.h"
#include "log_service.h"
#include "gm_stdlib.h"
#include "gm_gprs.h"
#include "utility.h"
#include "gm_fs.h"
#include "system_state.h"

static SocketType s_update_socket= {-1,"",SOCKET_STATUS_MAX,};

typedef struct
{
    u8 getting_data;    //是否正在请求数据文件
    u32 last_ok_time;   //上一次执行升级检测的时间
    u32 wait;           //执行升级检测的间隔
}SocketTypeUpdateExtend;

static SocketTypeUpdateExtend s_update_socket_extend;

static GM_ERRCODE update_service_transfer_status(u8 new_status);
static void update_service_init_proc(void);
static void update_service_connecting_proc(void);
static void update_service_work_proc(void);
static void update_service_close(void);


static GM_ERRCODE update_msg_send_request(void);
static void update_service_data_finish_proc(void);



static GM_ERRCODE update_service_transfer_status(u8 new_status)
{
    u8 old_status = (u8)s_update_socket.status;
    GM_ERRCODE ret = GM_PARAM_ERROR;
    switch(s_update_socket.status)
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
        s_update_socket.status = new_status;
        s_update_socket.status_fail_count = 0;
        LOG(INFO,"clock(%d) update_service_transfer_status from %s to %s success", util_clock(), 
            gm_socket_status_string((SocketStatus)old_status), gm_socket_status_string((SocketStatus)new_status));
    }
    else
    {
        LOG(INFO,"clock(%d) update_service_transfer_status from %s to %s failed", util_clock(), 
            gm_socket_status_string((SocketStatus)old_status), gm_socket_status_string((SocketStatus)new_status));
    }

    return ret;

}


GM_ERRCODE update_service_create(bool first_create)
{
    u8 addr[2*GOOME_DNS_MAX_LENTH+1];
    u8 IP[4];
    u32 port = 0;
    u8 idx = 0;

    if(!first_create)
    {
        if(s_update_socket.fifo.base_addr)
        {
            return GM_SUCCESS;
        }
    }
    
    gm_socket_init(&s_update_socket, SOCKET_INDEX_UPDATE);

    GM_memset(addr, 0x00, sizeof(addr));
    idx = GM_sscanf((const char *)config_service_get_pointer(CFG_UPDATESERVERADDR), "%[^:]:%d", addr, &port);
    if (idx != 2)
    {
        LOG(WARN,"clock(%d) update_service_create assert(idx ==2) failed.", util_clock());
        return GM_PARAM_ERROR;
    }
    
    if(GM_SUCCESS != GM_ConvertIpAddr(addr, IP))
    {
        if(util_is_valid_dns(addr, GM_strlen((const char *)addr)))
        {
            gm_socket_set_addr(&s_update_socket, addr, GM_strlen((const char *)addr), port, config_service_update_socket_type());
        }
        else
        {
            LOG(WARN,"clock(%d) update_service_create assert(dns(%s)) failed.", util_clock(), addr);
            return GM_PARAM_ERROR;
        }
    }
    else
    {
        gm_socket_set_ip_port(&s_update_socket, IP, port, config_service_update_socket_type());
        system_state_set_ip_cache(SOCKET_INDEX_UPDATE, IP);
    }

    s_update_socket_extend.getting_data = 0;
    s_update_socket_extend.last_ok_time = 0;
    s_update_socket_extend.wait = UPDATE_PING_TIME;

    LOG(INFO,"clock(%d) update_service_create access_id(%d) fifo(%p).", util_clock(), s_update_socket.access_id, &s_update_socket.fifo);

    init_file_extend(true);
	return GM_SUCCESS;
}

GM_ERRCODE update_service_destroy(void)
{
    if(SOCKET_STATUS_ERROR == s_update_socket.status)
    {
        return GM_SUCCESS;
    }
    
    update_service_close();

    //update socket only reconnect, not recreated.
    //fifo_delete(&s_update_socket.fifo);
    
    update_service_transfer_status(SOCKET_STATUS_ERROR);

	return GM_SUCCESS;
}

SocketStatus update_service_get_status(void)
{
	return (SocketStatus)(s_update_socket.status);
}

bool update_service_is_waiting_reboot(void)
{
    if(update_service_get_status() != SOCKET_STATUS_DATA_FINISH)
    {
        return false;
    }
    
    if(get_file_extend()->result == REPORT_RESULT_UPDATED)
    {
        return true;
    }
    
    return false;
}

GM_ERRCODE update_service_timer_proc(void)
{
    if(!s_update_socket.fifo.base_addr)
    {
        return GM_SUCCESS;
    }

    switch(s_update_socket.status)
    {
    case SOCKET_STATUS_INIT:
        update_service_init_proc();
        break;
    case SOCKET_STATUS_CONNECTING:
        update_service_connecting_proc();
        break;
    case SOCKET_STATUS_WORK:
        update_service_work_proc();
        break;
    case SOCKET_STATUS_DATA_FINISH:
        update_service_data_finish_proc();
        break;
    case SOCKET_STATUS_ERROR:
        update_service_data_finish_proc();
        break;
    default:
        LOG(WARN,"clock(%d) update_service_timer_proc assert(s_update_socket.status(%d)) unknown.", util_clock(),s_update_socket.status);
        return GM_ERROR_STATUS;
    }

    return GM_SUCCESS;
}


static void update_service_init_proc(void)
{
    u8 IP[4];
    gm_socket_get_host_by_name_trigger(&s_update_socket);
    system_state_get_ip_cache(SOCKET_INDEX_UPDATE, IP);
    if(GM_SUCCESS == gm_is_valid_ip(IP))
    {
        GM_memcpy( s_update_socket.ip , IP, sizeof(IP));
        update_service_transfer_status(SOCKET_STATUS_CONNECTING);
        if(GM_SUCCESS == gm_socket_connect(&s_update_socket))
        {
        }
        // else do nothing .   connecting_proc will deal.
    }
    else if((!s_update_socket.excuted_get_host) && (GM_SUCCESS == gm_is_valid_ip(s_update_socket.ip)))
    {
        update_service_transfer_status(SOCKET_STATUS_CONNECTING);
        if(GM_SUCCESS == gm_socket_connect(&s_update_socket))
        {
        }
        // else do nothing .   connecting_proc will deal.
    }
}

void update_service_connection_failed(void)
{
    update_service_close();
    
    if(s_update_socket.status_fail_count >= MAX_CONNECT_REPEAT)
    {
        // if excuted get_host transfer to error statu, else get_host.
        if(s_update_socket.excuted_get_host || (s_update_socket.addr[0] == 0))
        {
            update_service_finish(UPDATE_PING_TIME);
        }
        else
        {
            update_service_transfer_status(SOCKET_STATUS_INIT);
        }
    }
    // else do nothing . wait connecting proc to deal.
}

void update_service_connection_ok(void)
{
    update_service_transfer_status(SOCKET_STATUS_WORK);

    update_msg_send_request();
}

void update_service_finish(u32 wait)
{
    update_service_close();

    s_update_socket_extend.last_ok_time=util_clock();
    s_update_socket_extend.wait=wait;
    
    update_service_transfer_status(SOCKET_STATUS_DATA_FINISH);
}


static void update_service_close(void)
{
    if(s_update_socket.id >=0)
    {
        GM_SocketClose(s_update_socket.id);
        s_update_socket.id=-1;
    }
}


static void update_service_connecting_proc(void)
{
    u32 current_time = util_clock();

    if((current_time - s_update_socket.send_time) > CONNECT_TIME_OUT)
    {
        s_update_socket.status_fail_count ++;
        update_service_connection_failed();

        if(s_update_socket.status == SOCKET_STATUS_CONNECTING && 
            s_update_socket.status_fail_count < MAX_CONNECT_REPEAT)
        {
            if(GM_SUCCESS == gm_socket_connect(&s_update_socket))
            {
                //do nothing. wait callback
            }
        }
        
    }
}


static void update_service_work_proc(void)
{
    u32 current_time = util_clock();
    u8 one_send = 1;

    one_send = (STREAM_TYPE_DGRAM == config_service_update_socket_type())? UPDATE_MAX_PACK_ONE_SEND: UPDATE_MAX_PACK_ONE_SEND;
    if(! s_update_socket_extend.getting_data )
    {
        //发送请求阶段 
        if((current_time - s_update_socket.send_time) > MESSAGE_TIME_OUT)
        {
            s_update_socket.status_fail_count ++;
            if(s_update_socket.status_fail_count >= MAX_MESSAGE_REPEAT)
            {
                LOG(INFO,"clock(%d) update_service_work_proc failed:%d", util_clock(), s_update_socket.status_fail_count);
                update_service_finish(UPDATE_RETRY_TIME);
            }
            else
            {
                update_msg_send_request();
            }
        }
    }
    else
    {
        //请求数据阶段 
        if(! (get_file_extend()->use_new_socket))
        {
            if((current_time - s_update_socket.send_time) > MESSAGE_TIME_OUT)
            {
                s_update_socket.status_fail_count ++;
                if(s_update_socket.status_fail_count >= (MAX_MESSAGE_REPEAT * one_send))
                {
                    LOG(INFO,"clock(%d) update_filemod_work_proc failed:%d", util_clock(), s_update_socket.status_fail_count);
                    get_file_extend()->result = REPORT_RESULT_FAILED;
                    GM_strncpy((char *)get_file_extend()->result_info, "request blocks fail(work_proc).", sizeof(get_file_extend()->result_info));
                    update_service_result_to_server();
                }
                else
                {
                    update_msg_send_data_block_request(&s_update_socket);
                }
            }
            
        }
        else
        {
            update_filemod_timer_proc();
        }
    }

    update_msg_receive(&s_update_socket);
}



static void update_service_data_finish_proc(void)
{
    u32 current_time = util_clock();

    if((current_time - s_update_socket_extend.last_ok_time) > s_update_socket_extend.wait)
    {
        LOG(DEBUG,"clock(%d) update_service_data_finish_proc cur(%d) - lastok(%d) > wait(%d).",
            util_clock(), current_time, s_update_socket_extend.last_ok_time,s_update_socket_extend.wait);
        init_file_extend(false);
        s_update_socket_extend.getting_data = 0;
        s_update_socket_extend.last_ok_time = 0;
        s_update_socket_extend.wait = UPDATE_PING_TIME;
        update_service_transfer_status(SOCKET_STATUS_INIT);
    }
}




static GM_ERRCODE update_msg_send_request(void)
{
    u8 buff[100];
    u16 len = sizeof(buff);
    u16 idx = 0;  //current place

    update_msg_pack_head(buff, &idx, len);  //13 bytes
    update_msg_pack_request(buff, &idx, len);  //53 bytes
    update_msg_pack_id_len(buff, PROTOCCOL_UPDATE_REQUEST, idx);

    len=idx+2;  // 1byte checksum , 1byte 0xD

    
    if(GM_SUCCESS == gm_socket_send(&s_update_socket, buff, len))
    {
        s_update_socket.send_time = util_clock();
        LOG(DEBUG,"clock(%d) update_msg_send_request msglen:%d success", util_clock(), len);
        return GM_SUCCESS;
    }
    else
    {
        LOG(DEBUG,"clock(%d) update_msg_send_request msglen:%d failed.", util_clock(), len);
        update_service_finish(UPDATE_PING_TIME);
    }
    return GM_ERROR_STATUS;
}




void update_service_after_response(bool newsocket, u32 total_len)
{
    //已处于获取数据状态了, 多余的消息不处理.
    if(s_update_socket_extend.getting_data)
    {
        return;
    }
    
    if (total_len <= UPDATAE_FILE_MAX_SIZE)
    {
        //准备获取文件
        s_update_socket_extend.getting_data = 1;
        
        if(!get_file_extend()->use_new_socket)
        {
            update_msg_start_data_block_request(&s_update_socket);
        }
        //else  update_filemod_timer_proc() will send request
    }
    else
    {
        LOG(INFO,"clock(%d) update_service_after_response assert(file_total_len(%d)) failed.", util_clock(), total_len);
        update_service_finish(UPDATE_PING_TIME);
    }
}


void update_service_result_to_server(void)
{
    if(get_file_extend()->result == REPORT_RESULT_UPDATED)
    {
        //lz modified for only save at most 2 copies. 
        // master/minor which not exist , write to corresponding file, old file delete.
        if (GM_FS_CheckFile(UPDATE_TARGET_IMAGE) < 0)
        {
            GM_FS_Rename(UPDATE_UPGRADE_FILE, UPDATE_TARGET_IMAGE);
            util_delete_file(UPDATE_MINOR_IMAGE);
        }
        else
        {
            GM_FS_Rename(UPDATE_UPGRADE_FILE, UPDATE_MINOR_IMAGE);
            util_delete_file(UPDATE_TARGET_IMAGE);
        }
    }
    
    update_msg_send_result_to_server(&s_update_socket);
}

void update_service_after_blocks_finish(void)
{
    s_update_socket.status_fail_count = 0;
    update_msg_send_data_block_request(&s_update_socket);
}


