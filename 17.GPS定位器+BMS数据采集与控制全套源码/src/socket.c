#include "gm_type.h"
#include "stdio.h"
#include "gm_stdlib.h"
#include "gm_gprs.h"
#include "socket.h"
#include "gm_memory.h"
#include "gm_timer.h"
#include "config_service.h"
#include "gps_service.h"
#include "log_service.h"
#include "agps_service.h"
#include "update_service.h"
#include "update_file.h"
#include "utility.h"
#include "gm_system.h"
#include "system_state.h"

/*returned by apn register, used when creating socket. */
static int s_account_id = -1;
static SocketType *s_all_sockets[MAX_SOCKET_SUPPORTED];

#define SOCKET_STATUS_STRING_MAX_LEN 32
const char s_socket_status_string[SOCKET_STATUS_MAX][SOCKET_STATUS_STRING_MAX_LEN] = 
{
    "SOCKET_STATUS_INIT",
    "SOCKET_STATUS_GET_HOST",
    "SOCKET_STATUS_CONNECTING",
    "SOCKET_STATUS_LOGIN",
    "SOCKET_STATUS_WORK",
    "SOCKET_STATUS_DATA_FINISH",
    "SOCKET_STATUS_ERROR"
};


/*
这里将dns解析单独出来, 各模块只管去取ip地址 并设置 requested= true., 
没取到就等着, 
取到了就继续 
*/
typedef struct
{
    bool requested;         /* 是否触发了要请求host_name */
    u8 fail_count;  /* fail count of current status */
    u32 send_time;   /* record time of recent get host, connect, send msg etc*/
}GetHostItem;

GetHostItem s_get_host_list[SOCKET_INDEX_MAX];

typedef struct 
{
    SocketType *socket;
    u8 count; 
    CurrentGetHostByNameStatus status;
    char addr[GOOME_DNS_MAX_LENTH];    /*dns name*/
}CurrentGetHostByName;

static CurrentGetHostByName current_get_host;
static GM_ERRCODE gm_socket_get_host_by_name(SocketType *socket, GetHostItem *item);
static CurrentGetHostByNameStatus gm_socket_current_get_host_status(void);

/*由于gethostbyname同时只能有一个, 所以下面的函数让各service模块判断是否自已的域名解析操作*/
static u8 gm_socket_same_as_current_get_host(SocketType *socket);


static void init_all_sockets(void);
static GM_ERRCODE add_to_all_sockets(SocketType *p);
static void current_get_host_start(SocketType *socket);
static void gm_socket_connect_failed(SocketType *socket);


static void init_all_sockets(void)
{
    int i;
    for(i =0; i< MAX_SOCKET_SUPPORTED; ++i)
    {
        s_all_sockets[i] = NULL;
    }
    
    for(i = 0; i<SOCKET_INDEX_MAX; ++i )
    {
        s_get_host_list[i].requested = false;
        s_get_host_list[i].fail_count = 0;
        s_get_host_list[i].send_time = 0;
    }
    
    return;
}

static GM_ERRCODE add_to_all_sockets(SocketType *p)
{
    int i;

    //先确定是否已经加入了
    for(i =0; i< MAX_SOCKET_SUPPORTED; ++i)
    {
        if(s_all_sockets[i]->access_id == p->access_id)
        {
            s_all_sockets[i] = p;
            return GM_SUCCESS;
        }
    }

    //没加入的情况, 找一个空位加入
    for(i =0; i< MAX_SOCKET_SUPPORTED; ++i)
    {
        if(s_all_sockets[i] == NULL)
        {
            s_all_sockets[i] = p;
            return GM_SUCCESS;
        }
    }

    return GM_MEM_NOT_ENOUGH;
}


SocketType * get_socket_by_id(int id)
{
    int i;
    for(i =0; i< MAX_SOCKET_SUPPORTED; ++i)
    {
        if(s_all_sockets[i] && s_all_sockets[i]->id == id)
        {
            return s_all_sockets[i];
        }
    }
    return NULL;
}

SocketType * get_socket_by_accessid(int access_id)
{
    int i;
    for(i =0; i< MAX_SOCKET_SUPPORTED; ++i)
    {
        if(s_all_sockets[i] && s_all_sockets[i]->access_id == access_id)
        {
            return s_all_sockets[i];
        }
    }
    return NULL;
}


GM_ERRCODE gm_socket_global_init(void)
{
    init_all_sockets();
    current_get_host_init();
    return GM_SUCCESS;
}


/* init socket*/
GM_ERRCODE gm_socket_init(SocketType *socket, SocketIndexEnum access_id)
{
    socket->id=-1;
    socket->status = SOCKET_STATUS_INIT;
    socket->send_time = 0;
    socket->status_fail_count = 0;
    socket->excuted_get_host = 0;
    socket->last_ack_seq = 0;
    socket->addr[0] = 0;
    socket->ip[0] = socket->ip[1] = socket->ip[2] = socket->ip[3] = 0;
    socket->port = 0;
    socket->access_id = access_id;

    if(access_id==SOCKET_INDEX_UPDATE)
    {
        fifo_init(&(socket->fifo), 5*SIZE_OF_SOCK_FIFO);
    }
    else
    {
        fifo_init(&(socket->fifo), SIZE_OF_SOCK_FIFO);
    }
    add_to_all_sockets(socket);
    return GM_SUCCESS;
}


GM_ERRCODE gm_socket_set_addr(SocketType *socket, u8 *addr, u8 addr_len, u16 port, StreamType type)
{
    if(addr_len >= sizeof(socket->addr))
    {
        return GM_PARAM_ERROR;
    }
    GM_memcpy(socket->addr, addr , addr_len);
    socket->addr[addr_len] = 0;
    socket->port = port;
    socket->type = type;
    return GM_SUCCESS;
}

void gm_socket_set_account_id(int account_id)
{
    s_account_id = account_id;
}

GM_ERRCODE gm_socket_set_ip_port(SocketType *socket, u8 ip[4], u16 port, StreamType type)
{
    socket->ip[0] = ip[0];
    socket->ip[1] = ip[1];
    socket->ip[2] = ip[2];
    socket->ip[3] = ip[3];
    socket->port = port;
    socket->type = type;
    return GM_SUCCESS;
}

void gm_socket_destroy(SocketType * socket)
{
	LOG(INFO,"clock(%d) gm_socket_destroy socket->access_id(%d) .", 
		util_clock(), socket->access_id);
    switch(socket->access_id)
    {
    case SOCKET_INDEX_MAIN:
        gps_service_destroy();
        break;
    case SOCKET_INDEX_EPO: //SOCKET_INDEX_AGPS
        agps_service_destroy();
        break;
    case SOCKET_INDEX_LOG:
        log_service_destroy();
        break;
    case SOCKET_INDEX_UPDATE:
        update_service_destroy();
        break;
    case SOCKET_INDEX_UPDATE_FILE:
        update_filemod_destroy();
        break;
    case SOCKET_INDEX_CONFIG:
        config_service_destroy();
        break;
    default:
        LOG(WARN,"clock(%d) gm_socket_destroy assert(socket->access_id(%d) unknown.", 
			util_clock(), socket->access_id);
        break;
    }
}

void gm_socket_global_destroy(void)
{
    int i;
    
    for(i =0; i< MAX_SOCKET_SUPPORTED; ++i)
    {
        if(s_all_sockets[i] == NULL)
        {
            continue;
        }
        gm_socket_destroy(s_all_sockets[i]);
    }
}


void current_get_host_init(void)
{
    current_get_host.socket = NULL;
    current_get_host.count = 0;
    current_get_host.status = CURRENT_GET_HOST_INIT;
    current_get_host.addr[0] = 0;
}

/* get host can only start one by one .*/
static void current_get_host_start(SocketType *socket)
{
    current_get_host.socket = socket;
    current_get_host.count = 0;
    current_get_host.status = CURRENT_GET_HOST_CONTINUE;
    GM_strncpy(current_get_host.addr,socket->addr,sizeof(current_get_host.addr));
}


void socket_get_host_by_name_callback(void *msg)
{
    if(current_get_host.socket == NULL)
    {
        LOG(ERROR,"clock(%d) socket_get_host_by_name_callback assert(current_get_host.socket != NULL) failed.", 
			util_clock());
        current_get_host.status = CURRENT_GET_HOST_INIT;
        return;
    }
    
    if(GM_strcmp(current_get_host.addr, current_get_host.socket->addr) != 0)
    {
        LOG(INFO,"clock(%d) socket_get_host_by_name_callback (%s != %s).", 
			util_clock(),current_get_host.addr, current_get_host.socket->addr);
        current_get_host.status = CURRENT_GET_HOST_INIT;
        return;
    }

    if(GM_SUCCESS != gm_is_valid_ip((const u8 *)msg))
    {
        LOG(INFO,"clock(%d) socket_get_host_by_name_callback(%s) return error msg %s.", 
			util_clock(), current_get_host.socket->addr, (const char *)msg);
        current_get_host.status = CURRENT_GET_HOST_FAILED;
        return;
    }

    system_state_set_ip_cache(current_get_host.socket->access_id, (const U8*)msg);
    current_get_host.status = CURRENT_GET_HOST_SUCCESS;

	LOG(INFO,"clock(%d) socket_get_host_by_name_callback (%s)(%d.%d.%d.%d).", 
		util_clock(), current_get_host.socket->addr, ((const U8*)msg)[0], 
		((const U8*)msg)[1], ((const U8*)msg)[2], ((const U8*)msg)[3]);
}





static CurrentGetHostByNameStatus gm_socket_current_get_host_status()
{
    return current_get_host.status;
}

u8 gm_socket_same_as_current_get_host(SocketType *socket)
{
    if(current_get_host.socket == socket)
    {
        return 1;
    }
    return 0;
}

GM_ERRCODE gm_socket_get_host_by_name(SocketType *socket, GetHostItem *item)
{
	u8 result = 0;

    if(socket->addr[0] == 0)
    {
        LOG(INFO,"clock(%d) gm_socket_get_host_by_name socket(%d)->addr is empty.", util_clock(), socket->access_id );
        return GM_PARAM_ERROR;
    }
    
    if(gm_socket_current_get_host_status() != CURRENT_GET_HOST_INIT)
    {
        //LOG(DEBUG,"gm_socket_get_host_by_name socket not init.");
        return GM_NOT_INIT;
    }

    item->send_time = util_clock();
    socket->excuted_get_host=1;
    current_get_host_start(socket);
    
    GM_GetHostByName(socket->addr, s_account_id, &result);
	LOG(INFO,"clock(%d) gm_socket_get_host_by_name socket->addr(%s) result(%d).", 
		util_clock(), socket->addr, result);
    return GM_SUCCESS;
}


GM_ERRCODE gm_is_valid_ip(const u8*data)
{
    if ((data[0]==0) && (data[1]==0)
        && (data[2]==0) && (data[3]==0))
    {
        return GM_PARAM_ERROR;
    }
        
    if ((data[0]==255) && (data[1]==255)
    && (data[2]==255) && (data[3]==255))
    {
        return GM_PARAM_ERROR;
    }
    
    return GM_SUCCESS;
}



void gm_socket_connect_ok(SocketType *socket)
{
    switch(socket->access_id)
    {
    case SOCKET_INDEX_MAIN:
        gps_service_connection_ok();
        break;
    case SOCKET_INDEX_EPO: //SOCKET_INDEX_AGPS
        // udp not need callback
        agps_service_connection_ok();
        break;
    case SOCKET_INDEX_LOG:
        log_service_connection_ok();
        break;
    case SOCKET_INDEX_UPDATE:
        update_service_connection_ok();
        break;
    case SOCKET_INDEX_UPDATE_FILE:
        update_filemod_connection_ok();
        break;
    case SOCKET_INDEX_CONFIG:
        config_service_connection_ok();
        break;
    }
}

void gm_socket_close_for_reconnect(SocketType *socket)
{
    switch(socket->access_id)
    {
    case SOCKET_INDEX_MAIN:
        gps_service_close_for_reconnect();
        break;
    case SOCKET_INDEX_EPO: //SOCKET_INDEX_AGPS
        // udp not need callback
        agps_service_close_for_reconnect();
        break;
    case SOCKET_INDEX_LOG:
        log_service_close_for_reconnect();
        break;
    case SOCKET_INDEX_UPDATE:
        update_service_finish(UPDATE_PING_TIME);
        break;
    case SOCKET_INDEX_UPDATE_FILE:
        update_filemod_close_for_reconnect();
        break;
    case SOCKET_INDEX_CONFIG:
        config_service_close_for_reconnect();
        break;
    }
}


static void gm_socket_connect_failed(SocketType *socket)
{
	/*	different socket have different dealing
	gps_service retry N times should reinit gprs
	other services ignore failue */
    switch(socket->access_id)
    {
    case SOCKET_INDEX_MAIN:
        gps_service_connection_failed();
        break;
    case SOCKET_INDEX_EPO: //SOCKET_INDEX_AGPS
        agps_service_connection_failed();
        break;
    case SOCKET_INDEX_LOG:
        log_service_connection_failed();
        break;
    case SOCKET_INDEX_UPDATE:
        update_service_connection_failed();
        break;
    case SOCKET_INDEX_UPDATE_FILE:
        update_filemod_connection_failed();
        break;
    case SOCKET_INDEX_CONFIG:
        config_service_connection_failed();
        break;
    }
}

GM_ERRCODE gm_socket_connect(SocketType *socket)
{
	char ip_addr[16];
	
    if (socket->id >= 0)
    {
        return GM_SUCCESS;
    }
    
    LOG(INFO,"clock(%d) gm_socket_connect type(%d)(%d.%d.%d.%d:%d).", 
        util_clock(), socket->type, socket->ip[0], socket->ip[1], socket->ip[2], socket->ip[3], socket->port);
    if(GM_SUCCESS != gm_is_valid_ip(socket->ip))
    {
        return GM_PARAM_ERROR;
    }

    socket->send_time = util_clock();
    socket->id = GM_SocketCreate(s_account_id, socket->type);
    if (socket->id >= 0)
    {
		GM_snprintf(ip_addr, sizeof(ip_addr), "%d.%d.%d.%d", socket->ip[0], socket->ip[1], socket->ip[2], socket->ip[3]);
        if (GM_SocketConnect(socket->id, ip_addr, socket->port, s_account_id, 
			socket->access_id, socket->type))
        {
            if(socket->type == STREAM_TYPE_STREAM)
            {
                // wait call back
            }
            else if(socket->type == STREAM_TYPE_DGRAM)
            {
                LOG(INFO,"clock(%d) gm_socket_connect udp(%d.%d.%d.%d:%d) id(%d) access_id(%d) success.", 
                    util_clock(), socket->ip[0], socket->ip[1], socket->ip[2], socket->ip[3], socket->port, socket->id, socket->access_id);
                gm_socket_connect_ok(socket);
            }
            else
            {
                LOG(ERROR,"clock(%d) gm_socket_connect unknown type(%d)(%d.%d.%d.%d:%d).", 
                    util_clock(), socket->type, socket->ip[0], socket->ip[1], socket->ip[2], socket->ip[3], socket->port);
				return GM_UNKNOWN;
            }
        }
        else
        {
            LOG(INFO,"clock(%d) gm_socket_connect type(%d)(%d.%d.%d.%d:%d) failed.", 
                util_clock(), socket->type, socket->ip[0], socket->ip[1], socket->ip[2], socket->ip[3], socket->port);

            gm_socket_connect_failed(socket);
            return GM_UNKNOWN;
        }
    }
    else
    {
        LOG(INFO,"clock(%d) gm_socket_connect type(%d)(%d.%d.%d.%d:%d) create failed.", 
            util_clock(), socket->type, socket->ip[0], socket->ip[1], socket->ip[2], socket->ip[3], socket->port);
        return GM_UNKNOWN;
    }
    
    return GM_SUCCESS;
}



GM_ERRCODE gm_socket_send(SocketType *socket, u8 *data, u16 len)
{  
    s32 ret;

	if(socket->status <= SOCKET_STATUS_CONNECTING || socket->status >= SOCKET_STATUS_DATA_FINISH)
    {
        LOG(ERROR,"clock(%d) gm_socket_send status(%d) error. type(%d)(%d.%d.%d.%d:%d) id(%d) len(%d).", 
            util_clock(), socket->status, socket->type, socket->ip[0], socket->ip[1], socket->ip[2], socket->ip[3], 
            socket->port,socket->id ,len);
        return GM_ERROR_STATUS;
    }

    if(len > MAX_SOCKET_SEND_MSG_LEN)
    {
        LOG(ERROR,"clock(%d) gm_socket_send type(%d)(%d.%d.%d.%d:%d) id(%d) len(%d) too long.", 
            util_clock(), socket->type, socket->ip[0], socket->ip[1], socket->ip[2], socket->ip[3], 
            socket->port,socket->id,len);
        return GM_PARAM_ERROR;
    }
    if(len <= 0)
    {
        return GM_PARAM_ERROR;
    }
    
    //发一部分的没有出现过,发送失败会返回负数
    gm_socket_get_ackseq(socket, &socket->last_ack_seq);
    ret = GM_SocketSend(socket->id, (char *)data, len);
    if (ret != len)
    {
        LOG(INFO,"clock(%d) gm_socket_send type(%d)(%d.%d.%d.%d:%d) id(%d) ret(%d)!=len(%d).", 
            util_clock(), socket->type, socket->ip[0], socket->ip[1], socket->ip[2], socket->ip[3], 
            socket->port,socket->id,ret,len);
        
        return GM_UNKNOWN;
    }
    
    return GM_SUCCESS;

}



GM_ERRCODE gm_socket_recv(SocketType *socket)
{
    char *recv_buff = NULL;
    int recvlen=0;
	GM_ERRCODE ret;
	
	if(socket->status <= SOCKET_STATUS_CONNECTING || socket->status >= SOCKET_STATUS_DATA_FINISH)
    {
        LOG(ERROR,"clock(%d) gm_socket_recv status(%d) error. type(%d)(%d.%d.%d.%d:%d) id(%d).", 
            util_clock(), socket->status, socket->type, socket->ip[0], socket->ip[1], socket->ip[2], socket->ip[3], 
            socket->port,socket->id);
        return GM_ERROR_STATUS;
    }

    recv_buff = GM_MemoryAlloc(MAX_SOCKET_RECV_MSG_LEN);
    if (NULL == recv_buff)
    {
        LOG(ERROR,"clock(%d) gm_socket_recv type(%d)(%d.%d.%d.%d:%d) id(%d) memory not enough.", 
            util_clock(), socket->type, socket->ip[0], socket->ip[1], socket->ip[2], socket->ip[3], 
            socket->port,socket->id);
        return GM_MEM_NOT_ENOUGH;
    }

    do
    {
        //GM_SocketRecv必须把所有消息接收完， 否则后面会收不了消息，正因为如此， fifo的缓冲要足够大，放得下
        GM_memset(recv_buff, 0x00, MAX_SOCKET_RECV_MSG_LEN);
        recvlen = GM_SocketRecv(socket->id, recv_buff, MAX_SOCKET_RECV_MSG_LEN);
        if(recvlen > 0)
        {
            LOG(INFO,"clock(%d) gm_socket_recv type(%d)(%d.%d.%d.%d:%d) id(%d) recvlen(%d).", 
                util_clock(), socket->type, socket->ip[0], socket->ip[1], socket->ip[2], socket->ip[3], 
                socket->port,socket->id,recvlen);
        }

       
        if (recvlen > 0)
        {
            gps_service_confirm_gps_cache(socket);

            ret = fifo_insert(&socket->fifo, (u8 *)recv_buff, recvlen);
    		if(ret != GM_SUCCESS)
    		{
    			LOG(ERROR,"clock(%d) fifo insert failed, return(%d)", util_clock(), ret );
                if(socket->access_id != SOCKET_INDEX_UPDATE)  //update_service 会重发
                {
                    //socket need rebuild.
                    fifo_reset(&socket->fifo);
                    if(socket->access_id == SOCKET_INDEX_MAIN)
                    {
                        char reason[30];
                        snprintf(reason,sizeof(reason),"fifo_insert return %d", ret);
                        reason[sizeof(reason) - 1] = 0;
                        system_state_set_gpss_reboot_reason(reason);
                    }
                    gm_socket_close_for_reconnect(socket);
                    GM_MemoryFree(recv_buff);
                    recv_buff = NULL;
                    return GM_MEM_NOT_ENOUGH;
                }
    		}
        }
    }while(recvlen > 0);

    GM_MemoryFree(recv_buff);
    recv_buff = NULL;

    if(recvlen != -2)  //SOC_WOULDBLOCK
    {
        if(socket->access_id == SOCKET_INDEX_MAIN)
        {
            char reason[30];
            snprintf(reason,sizeof(reason),"socket recv %d", recvlen);
            reason[sizeof(reason) - 1] = 0;
            system_state_set_gpss_reboot_reason(reason);
        }
        gm_socket_close_for_reconnect(socket);
    }

    return GM_SUCCESS;
}


const char * gm_socket_status_string(SocketStatus statu)
{
    return s_socket_status_string[statu];
}

GM_ERRCODE gm_socket_get_ackseq(SocketType *socket, u32 *ack_seq)
{
    u8 buff[20];
    
    buff[0] = socket->id;
    buff[4] = buff[3] = buff[2] = buff[1] = 0;
    GM_GetTcpStatus(buff);
    (*ack_seq) = MKDWORD(buff[4], buff[3], buff[2], buff[1]);
    
    return GM_SUCCESS;
}



void gm_socket_get_host_by_name_trigger(SocketType *socket)
{
    if(socket->addr[0])
    {
        s_get_host_list[socket->access_id].requested = true;
    }
}

/*
   循环处理所有的get_host_by_name请求
*/
void gm_socket_get_host_timer_proc(void)
{
    int i = 0;
    SocketType * socket = NULL;
    
    for(i = 0; i < SOCKET_INDEX_MAX; ++i)
    {
        if(s_get_host_list[i].requested)
        {
            break;
        }
    }

    
    if(i >= SOCKET_INDEX_MAX)
    {
        // no need get host
        return;
    }

    
    socket = get_socket_by_accessid(i);
    if(!socket)
    {
        s_get_host_list[i].requested = false;
        LOG(ERROR,"clock(%d) gm_socket_get_host_timer_proc assert(socket_idx(%d)) failed.", util_clock(), i );
        return;
    }

    if(current_get_host.socket)
    {
        u32 current_time = util_clock();
        bool same_as_current_get_host = false;
        int current_idx = current_get_host.socket->access_id;
        if(gm_socket_same_as_current_get_host(socket))
        {
            same_as_current_get_host = true;
        }
        
        //或者成功, 或者失败,或者超时
        if((gm_socket_current_get_host_status() == CURRENT_GET_HOST_SUCCESS) || 
            (gm_socket_current_get_host_status() == CURRENT_GET_HOST_FAILED))
        {
            // 释放资源,后面去抢
            s_get_host_list[current_idx].requested = false;
            s_get_host_list[current_idx].fail_count = 0;
            s_get_host_list[current_idx].send_time = 0;
            current_get_host_init();
            if(same_as_current_get_host) return;
        }
        else if((current_time - s_get_host_list[current_idx].send_time) > GET_HOST_TIME_OUT)
        {
            s_get_host_list[current_idx].fail_count ++;
            current_get_host_init(); //clear ，后面 竞争

            if(s_get_host_list[current_idx].fail_count >= MAX_GET_HOST_REPEAT)
            {
                s_get_host_list[current_idx].requested = false;
                s_get_host_list[current_idx].fail_count = 0;
                s_get_host_list[current_idx].send_time = 0;
                if(same_as_current_get_host) return;
            }
        }
        else
        {
            //正在执行, 未超时
            return;
        }
    }

    if(GM_SUCCESS == gm_socket_get_host_by_name(socket, &s_get_host_list[i]))
    {
        LOG(DEBUG,"clock(%d) gm_socket_get_host_timer_proc start dns(%s).", util_clock(), socket->addr );
        //do nothing. wait callback
    }
    // else do nothing .
    

}

