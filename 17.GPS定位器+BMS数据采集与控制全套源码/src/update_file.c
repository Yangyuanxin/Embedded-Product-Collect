#include "update_file.h"
#include "update_service.h"
#include "config_service.h"
#include "log_service.h"
#include "gm_stdlib.h"
#include "gm_gprs.h"
#include "utility.h"
#include "gsm.h"
#include "gm_memory.h"
#include "gm_fs.h"
#include "hard_ware.h"
#include "gm_timer.h"
#include "system_state.h"

static SocketType s_file_socket= {-1,"",SOCKET_STATUS_MAX,};
static UpdateFileExtend s_file_extend;
static UpdateFileExtend s_file_extend_old;

#define UPDATE_UPGRADE_STATE_FILE                 L"Z:\\goome\\GmUpState\0"


static GM_ERRCODE update_filemod_transfer_status(u8 new_status);
static void update_filemod_init_proc(void);
static void update_filemod_connecting_proc(void);
static void update_filemod_work_proc(void);


static void update_msg_parse_response(u8 *pdata, u16 len);
static void update_msg_parse_file_data(u8 *pdata, u16 len);
static void update_msg_parse_check_block_bits(u16 block);
static GM_ERRCODE update_msg_parse_one_block(u16 block, u8 *pdata, u16 data_len);

static void update_msg_pack_result_report(u8 *pdata, u16 *idx, u16 len);

static GM_ERRCODE update_filemod_file_open(void);
static GM_ERRCODE update_filemod_file_create(u32 fs_len, int *sys_error);
static GM_ERRCODE update_filemod_file_write(u32 fs_seek, u8* pdata, u32 len);
static GM_ERRCODE update_filemod_file_finish(void);


static bool is_update_file_extend_same(UpdateFileExtend *e1, UpdateFileExtend *e2);
static GM_ERRCODE update_filemod_state_file_load(void);
static GM_ERRCODE update_filemod_state_file_save(void);


static GM_ERRCODE update_filemod_transfer_status(u8 new_status)
{
    u8 old_status = (u8)s_file_socket.status;
    GM_ERRCODE ret = GM_PARAM_ERROR;
    switch(s_file_socket.status)
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
        s_file_socket.status = new_status;
        s_file_socket.status_fail_count = 0;
        LOG(INFO,"clock(%d) update_filemod_transfer_status from %s to %s success", util_clock(), 
            gm_socket_status_string((SocketStatus)old_status), gm_socket_status_string((SocketStatus)new_status));
    }
    else
    {
        LOG(INFO,"clock(%d) update_filemod_transfer_status from %s to %s failed", util_clock(), 
            gm_socket_status_string((SocketStatus)old_status), gm_socket_status_string((SocketStatus)new_status));
    }

    return ret;

}


GM_ERRCODE update_filemod_create(void)
{
    u8 addr[2*GOOME_DNS_MAX_LENTH+1];
    u8 IP[4];
    u32 port = 0;
    u8 idx = 0;
	
    gm_socket_init(&s_file_socket, SOCKET_INDEX_UPDATE_FILE);

    GM_memset(addr, 0x00, sizeof(addr));
    idx = GM_sscanf((const char *)config_service_get_pointer(CFG_UPDATEFILESERVER), "%[^:]:%d", addr, &port);
    if (idx != 2)
    {
        LOG(WARN,"clock(%d) update_filemod_create assert(idx ==2) failed.", util_clock());
        return GM_PARAM_ERROR;
    }
    
    if(GM_SUCCESS != GM_ConvertIpAddr(addr, IP))
    {
        if(util_is_valid_dns(addr, GM_strlen((const char *)addr)))
        {
            gm_socket_set_addr(&s_file_socket, addr, GM_strlen((const char *)addr), port, config_service_update_socket_type());
        }
        else
        {
            LOG(WARN,"clock(%d) update_filemod_create assert(dns(%s)) failed.", util_clock(), addr);
            return GM_PARAM_ERROR;
        }
    }
    else
    {
        gm_socket_set_ip_port(&s_file_socket, IP, port, config_service_update_socket_type());
        system_state_set_ip_cache(SOCKET_INDEX_UPDATE_FILE, IP);
    }

    LOG(WARN,"clock(%d) update_filemod_create access_id(%d) fifo(%p).", util_clock(), s_file_socket.access_id, &s_file_socket.fifo);
	return GM_SUCCESS;
}

GM_ERRCODE update_filemod_destroy(void)
{
    if(s_file_extend.use_new_socket)
    {
        if(s_file_socket.id >=0)
        {
            GM_SocketClose(s_file_socket.id);
            s_file_socket.id=-1;
        }

        //必须重新create
        fifo_delete(&s_file_socket.fifo);
        update_filemod_transfer_status(SOCKET_STATUS_ERROR);
    }

    update_service_result_to_server();
	return GM_SUCCESS;
}


GM_ERRCODE update_filemod_timer_proc(void)
{
    if(!s_file_socket.fifo.base_addr)
    {
        return GM_SUCCESS;
    }

    switch(s_file_socket.status)
    {
    case SOCKET_STATUS_INIT:
        update_filemod_init_proc();
        break;
    case SOCKET_STATUS_CONNECTING:
        update_filemod_connecting_proc();
        break;
    case SOCKET_STATUS_WORK:
        update_filemod_work_proc();
        break;
    // 没有 SOCKET_STATUS_ERROR 状态,因为进该状态就会调update_service_destroy
    //case SOCKET_STATUS_ERROR:
        //break;
    default:
        LOG(WARN,"clock(%d) update_filemod_timer_proc assert(s_file_socket.status(%d)) unknown.", util_clock(),s_file_socket.status);
        return GM_ERROR_STATUS;
    }

    return GM_SUCCESS;
}


static void update_filemod_init_proc(void)
{
    u8 IP[4];
    gm_socket_get_host_by_name_trigger(&s_file_socket);
    system_state_get_ip_cache(SOCKET_INDEX_UPDATE_FILE, IP);
    if(GM_SUCCESS == gm_is_valid_ip(IP))
    {
        GM_memcpy( s_file_socket.ip , IP, sizeof(IP));
        update_filemod_transfer_status(SOCKET_STATUS_CONNECTING);
        if(GM_SUCCESS == gm_socket_connect(&s_file_socket))
        {
        }
        // else do nothing .   connecting_proc will deal.
    }
    else if((!s_file_socket.excuted_get_host) && (GM_SUCCESS == gm_is_valid_ip(s_file_socket.ip)))
    {
        update_filemod_transfer_status(SOCKET_STATUS_CONNECTING);
        if(GM_SUCCESS == gm_socket_connect(&s_file_socket))
        {
        }
        // else do nothing .   connecting_proc will deal.
    }
}

void update_filemod_connection_failed(void)
{
    if(s_file_socket.status_fail_count >= MAX_CONNECT_REPEAT)
    {
        // if excuted get_host transfer to error statu, else get_host.
        if(s_file_socket.excuted_get_host || (s_file_socket.addr[0] == 0))
        {
            s_file_extend.result = REPORT_RESULT_FAILED;
            GM_strncpy((char *)s_file_extend.result_info, "connect to file server fail.", sizeof(s_file_extend.result_info));
            update_filemod_destroy();
        }
        else
        {
            update_filemod_transfer_status(SOCKET_STATUS_INIT);
        }
    }
    // else do nothing . wait connecting proc to deal.
}

void update_filemod_connection_ok(void)
{
    update_filemod_transfer_status(SOCKET_STATUS_WORK);

    update_msg_start_data_block_request(&s_file_socket);
}

void update_filemod_close_for_reconnect(void)
{
    update_filemod_destroy();
}


static void update_filemod_connecting_proc(void)
{
    u32 current_time = util_clock();

    if((current_time - s_file_socket.send_time) > CONNECT_TIME_OUT)
    {
        s_file_socket.status_fail_count ++;
        update_filemod_connection_failed();

        if(s_file_socket.status == SOCKET_STATUS_CONNECTING && 
            s_file_socket.status_fail_count < MAX_CONNECT_REPEAT)
        {
            if(GM_SUCCESS == gm_socket_connect(&s_file_socket))
            {
                //do nothing. wait callback
            }
        }
        
    }
}


static void update_filemod_work_proc(void)
{
    u8 one_send = 1;
    u32 current_time = util_clock();
    if((current_time - s_file_socket.send_time) > MESSAGE_TIME_OUT)
    {
        s_file_socket.status_fail_count ++;
        one_send = (STREAM_TYPE_DGRAM == config_service_update_socket_type())? UPDATE_MAX_PACK_ONE_SEND: UPDATE_MAX_PACK_ONE_SEND;
        
        if(s_file_socket.status_fail_count >= (MAX_MESSAGE_REPEAT * one_send))
        {
            LOG(INFO,"clock(%d) update_filemod_work_proc failed:%d", util_clock(), s_file_socket.status_fail_count);
            s_file_extend.result = REPORT_RESULT_FAILED;
            GM_strncpy((char *)s_file_extend.result_info, "request blocks fail(filemod_work_proc).", sizeof(s_file_extend.result_info));
            update_filemod_destroy();
        }
        else
        {
            update_msg_send_data_block_request(&s_file_socket);
        }
    }

    update_msg_receive(&s_file_socket);
}








void update_msg_pack_head(u8 *pdata, u16 *idx, u16 len)
{
    u8 imei[GM_IMEI_LEN + 1] = {0};
    GM_ERRCODE ret = GM_SUCCESS;
    
    if((*idx) + 13 > len)
    {
        LOG(WARN,"clock(%d) update_msg_pack_head assert(len(%d)) failed.", util_clock(), len);
        return;
    }
    
    pdata[(*idx)++] = PROTOCOL_HEADER_UPDATE;
    pdata[(*idx)++] = PROTOCOL_HEADER_UPDATE;
    (*idx) = (*idx) + 3; // id 1 , len 2

    if(GM_SUCCESS != gsm_get_imei(imei))
    {
        LOG(INFO,"clock(%d) update_msg_pack_head can not get imei, ret:%d.", util_clock(), ret);
    }
    
    if (0 == GM_strlen((const char *)imei))
    {
        GM_memset(imei, 0, sizeof(imei));
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


void update_msg_pack_id_len(u8 *pdata, u8 id, u16 len)
{
    u16 idx = 0;
    u8 checksum = 0;
    u16 total = 0;
    u16 len_send = 0;

    if(13 > len)
    {
        LOG(WARN,"clock(%d) update_msg_pack_id_len assert(len(%d)) failed.", util_clock(), len);
        return;
    }

    //len now is pointing checksum byte
    total = 2 + len;
    len_send = len - 5;
    
    pdata[2] = id;         //id   1byte
    pdata[3] = BHIGH_BYTE(len_send);       //length  2bytes
    pdata[4] = BLOW_BYTE(len_send);

    //from head to end_of_data
    for(idx = 0; idx < len; ++ idx)
    {
        checksum ^= pdata[idx];
    }
    
    pdata[len] = checksum;   // xor value.   1byte

    pdata[total - 1] = PROTOCOL_TAIL_UPDATE;  //end
}



void update_msg_pack_request(u8 *pdata, u16 *idx, u16 len)
{
    u16 copy_len = 0;
    u16 copy_len_fix = 0;
    u32 checksum = 0;
    
    if((*idx) + 51 + 2 > len)   // aftter body(51 bytes), 1byte checksum, 1byte 0xD
    {
        LOG(WARN,"clock(%d) update_msg_pack_request assert(len(%d)) failed.", util_clock(), len);
        return;
    }


    /*u8 custom_code[6]; //客户代码*/
    copy_len_fix = copy_len = 6;
    GM_memset(pdata+(*idx),0, copy_len);
    copy_len= (config_service_get_length(CFG_CUSTOM_CODE, TYPE_STRING) > copy_len)?copy_len:
        config_service_get_length(CFG_CUSTOM_CODE, TYPE_STRING);
    GM_memcpy(pdata+(*idx),config_service_get_pointer(CFG_CUSTOM_CODE),copy_len);
    (*idx) += copy_len_fix;

    /*u8 terminal_type[5]; //终端型号ID*/
    copy_len_fix = copy_len = 5;
    GM_memset(pdata+(*idx),0, copy_len);
    copy_len= (config_service_get_length(CFG_TERM_MODEL, TYPE_STRING) > copy_len)?copy_len:
        config_service_get_length(CFG_TERM_MODEL, TYPE_STRING);
    GM_memcpy(pdata+(*idx),config_service_get_pointer(CFG_TERM_MODEL),copy_len);
    (*idx) += copy_len_fix;

    
    /*u8 termianl_version[20]; //终端当前版本号 */
    copy_len_fix = copy_len = 20;
    GM_memset(pdata+(*idx),0, copy_len);
    copy_len= (config_service_get_length(CFG_TERM_VERSION, TYPE_STRING) > copy_len)?copy_len:
        config_service_get_length(CFG_TERM_VERSION, TYPE_STRING);
    GM_memcpy(pdata+(*idx),config_service_get_pointer(CFG_TERM_VERSION),copy_len);
    (*idx) += copy_len_fix;


    /*u8 terminal_version_check[10]; //终端当前版本校验码 */
    copy_len_fix = copy_len = 10;
    GM_memset(pdata+(*idx),0, copy_len);

    //lz modified for only save at most 2 copies. 
    // master/minor both exist , use master
    if (GM_FS_CheckFile(UPDATE_TARGET_IMAGE) >= 0)
    {
        checksum = update_filemod_get_checksum(UPDATE_TARGET_IMAGE);
    }
    else  // UPDATE_MINOR_IMAGE 肯定存在, 否则不可能运行
    {
        checksum = update_filemod_get_checksum(UPDATE_MINOR_IMAGE);
    }
	LOG(INFO,"Bin file check sum:%4X",checksum);
    util_long_to_asc(checksum, pdata+(*idx), 0);
    (*idx) += copy_len_fix;

    
    /*u8 terminal_boot_check[10]; //终端当前版本BOOT校验码 */
    copy_len_fix = copy_len = 10;
    GM_memset(pdata+(*idx),0, copy_len);
    copy_len= (config_service_get_length(CFG_TERM_BOOT_CHECK, TYPE_STRING) > copy_len)?copy_len:
        config_service_get_length(CFG_TERM_BOOT_CHECK, TYPE_STRING);
    GM_memcpy(pdata+(*idx),config_service_get_pointer(CFG_TERM_BOOT_CHECK),copy_len);
    (*idx) += copy_len_fix;
    
}


void update_msg_receive(SocketType *socket)
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
    msg_len = msg_len + 7;

    if(msg_len > MAX_GPRS_MESSAGE_LEN)
    {
        LOG(WARN,"clock(%d) update_msg_receive assert(msg_len(%d)) failed.",util_clock(), msg_len);
        //clear fifo and restart socket.
        fifo_reset(&socket->fifo);
        gm_socket_close_for_reconnect(socket);
        return;
    }

    pdata = GM_MemoryAlloc(msg_len);
    if(pdata == NULL)
    {
        LOG(INFO,"clock(%d) update_msg_receive alloc buf failed. len:%d", util_clock(), msg_len);

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
            LOG(DEBUG,"clock(%d) update_msg_receive get msg failed. len:%d", util_clock(), msg_len);
            log_service_print_hex((const char*)head, sizeof(head));
            packet_error_start = util_clock();
        }
        else
        {
            if((util_clock() - packet_error_start) > MAX_GPRS_PART_MESSAGE_TIMEOUT)
            {
                LOG(WARN,"clock(%d) update_msg_receive MAX_GPRS_PART_MESSAGE_TIMEOUT.",util_clock());
                //clear fifo and restart socket.
                fifo_reset(&socket->fifo);
                s_file_extend.result = REPORT_RESULT_FAILED;
                update_filemod_close_for_reconnect();
                packet_error_start = 0;
            }
        }
        return;
    }
    packet_error_start = 0;
    fifo_pop_len(&socket->fifo, msg_len);

	LOG(DEBUG,"clock(%d) update_msg_receive msg len(%d)", util_clock(), msg_len);

    update_msg_parse(pdata, msg_len);
    GM_MemoryFree(pdata);
}


static void update_msg_parse(u8 *pdata, u16 len)
{
    u8 checksum = 0;
    u16 idx = 0;
    
    if(PROTOCOL_HEADER_UPDATE != pdata[0] || PROTOCOL_HEADER_UPDATE != pdata[1])
    {
        LOG(WARN,"clock(%d) update_msg_parse assert(header(%02x %02x)) failed.", util_clock(), pdata[0],pdata[1]);
        return;
    }
    if(PROTOCOL_TAIL_UPDATE != pdata[len-1] )
    {
        LOG(WARN,"clock(%d) update_msg_parse assert(tail(%d)) failed.",util_clock(), pdata[len-1] );
        return;
    }

    for(idx = 0; idx < (len-2); ++idx)
    {
        checksum ^= pdata[idx];
    }

    if(checksum != pdata[len-2])
    {
        LOG(WARN,"clock(%d) update_msg_parse assert(xor(%d != %d)) failed.",util_clock(), checksum, pdata[len-2]);
        return;
    }
    
    //协议号
    switch(pdata[2])
    {
        case PROTOCCOL_UPDATE_RESPONSE:
            update_msg_parse_response(pdata,len);
            break;
        case PROTOCCOL_UPDATE_FILE_DATA:
            update_msg_parse_file_data(pdata,len);
            break;
        default:
            LOG(WARN,"clock(%d) update_msg_parse assert(msgid(%d)) failed.",util_clock(), pdata[2]);
            break;
    }
    return;
}


static void update_msg_parse_response(u8 *pdata, u16 len)
{
    u8 req_result = 0;
    u16 idx;
    u8 addr[22]; //xxx.xxx.xxx.xxx:xxxxx
    u32 ip;
    u16 port;
    
    req_result = (ProtocolUpdateResultEnum)pdata[13];
    switch(req_result)
    {
        case UPDATE_NONE:
            LOG(INFO,"clock(%d) update_msg_parse_response no need to update, try %dh later again.", util_clock(), UPDATE_PING_TIME/3600);
            update_service_finish(UPDATE_PING_TIME);
            break;
    
        case UPDATE_QUE:
            LOG(DEBUG,"clock(%d) update_msg_parse_response queued to update, try %ds later again.", util_clock(), UPDATE_RETRY_TIME);
            update_service_finish(UPDATE_RETRY_TIME);
            break;
    
        case UPDATE_READY:
            idx = 14;
            port = MKWORD(pdata[idx+4] , pdata[idx+5]);
            GM_snprintf((char *)addr, sizeof(addr),"%d.%d.%d.%d:%d\0",pdata[idx],pdata[idx+1],pdata[idx+2],pdata[idx+3],port);
            ip =MKDWORD(pdata[idx],pdata[idx+1],pdata[idx+2],pdata[idx+3]);
            idx += 6;

            //file information
            s_file_extend.check_sum =MKDWORD(pdata[idx],pdata[idx+1],pdata[idx+2],pdata[idx+3]);
            idx += 4;
            s_file_extend.total_len =MKDWORD(pdata[idx],pdata[idx+1],pdata[idx+2],pdata[idx+3]);
            idx += 4;
    
            LOG(INFO,"clock(%d) update_msg_parse_response file check_sum:0x%04X, len:%u.",util_clock(), s_file_extend.check_sum,s_file_extend.total_len);
            GM_memcpy(s_file_extend.custom_code, &pdata[idx], 6);
            s_file_extend.custom_code[6] = 0;
            idx+=6;
    
            GM_memcpy(s_file_extend.model, &pdata[idx], 5);
            s_file_extend.model[5] = 0;
            idx += 5;
    
            GM_memcpy(s_file_extend.file_id, &pdata[idx], 8);
            s_file_extend.file_id[8] = 0;
            idx += 8;
            
            GM_memcpy(s_file_extend.task_id, &pdata[idx], 8);
            s_file_extend.task_id[8] = 0;
            idx += 8;
            
            if (GM_strcmp((const char *)s_file_extend.custom_code, (const char *)config_service_get_pointer(CFG_CUSTOM_CODE)) !=0)
            {
                LOG(INFO,"clock(%d) update_msg_parse_response compare custom_code(%s!=%s).",
                    util_clock(), s_file_extend.custom_code, config_service_get_pointer(CFG_CUSTOM_CODE));
                break;
            }
    
            
            if(ip !=0 || port != 0)
            {
                s_file_extend.use_new_socket = 1;
                config_service_set(CFG_UPDATEFILESERVER, TYPE_STRING, addr, GM_strlen((const char *)addr));
                update_filemod_create();
                update_service_after_response(s_file_extend.use_new_socket , s_file_extend.total_len );
            }
            else
            {
                s_file_extend.use_new_socket = 0;
                update_service_after_response(s_file_extend.use_new_socket , s_file_extend.total_len);
            }
            
            break;
    }
    return;
}

static void update_msg_parse_file_data(u8 *pdata, u16 len)
{
    u16 current_idx = 0;  //从s_file_extend.block_current计, 第 几个. [0-9]
    u16 block_number;
    u32 check_bit = 0x01;
    GM_ERRCODE ret = GM_SUCCESS;
    
    u8 one_send = 1;
    one_send = (STREAM_TYPE_DGRAM == config_service_update_socket_type())? UPDATE_MAX_PACK_ONE_SEND: UPDATE_MAX_PACK_ONE_SEND;
    
    block_number = MKWORD(pdata[13], pdata[14]);
    if(block_number < s_file_extend.block_current)
    {
        // already dealed data
        return;
    }

    current_idx = block_number - s_file_extend.block_current;
    if(current_idx) check_bit = check_bit << current_idx;
    if(check_bit & s_file_extend.block_status)
    {
        // already dealed data
        if(current_idx < (one_send  - 1))
        {
            return;
        }
        //else 最后一个包要触发下一批请求
    }

    /*
    2字节 1字节 2字节 8字节 N字节 1字节 1字节
    包头  命令字   报文长度    终端ID    数据内容    校验  结束符
    0x68 0x68   CMD LEN ID  DATA    CHK 0x0D
    */
    ret = update_msg_parse_one_block(block_number, &pdata[15], len - 17);
    LOG(INFO,"clock(%d) update_msg_parse_file_data update_msg_parse_one_block(%d/%d) ret(%d).",
        util_clock(), block_number + 1,s_file_extend.total_blocks, ret);

    if(GM_SUCCESS == ret)
    {
        update_msg_parse_check_block_bits(block_number);
    }
    
    return;
}



static GM_ERRCODE update_msg_parse_one_block(u16 block, u8 *pdata, u16 data_len)
{
    int idx;
    u32 fs_seek = 0;

    if(block == 0)
    {
        data_len = (data_len>UPD_RESERVER_LEN) ? (data_len-UPD_RESERVER_LEN) : data_len;
        for (idx=0; idx<data_len; idx++)
        {
            pdata[idx] = pdata[idx+UPD_RESERVER_LEN] ^ 0x0A ^ 0x05;
        }
    }
    else
    {
        fs_seek = block * s_file_extend.block_size - UPD_RESERVER_LEN;
        for (idx=0; idx<data_len; idx++)
        {
            pdata[idx] = pdata[idx] ^ 0x0A ^ 0x05;
        }
    }
    
    if(GM_SUCCESS != update_filemod_file_write(fs_seek, pdata, data_len))
    {
        // normally it will not happen, except file system error.
        s_file_extend.result = REPORT_RESULT_FAILED;
        GM_strncpy((char *)s_file_extend.result_info, "request blocks fail(parse_one_block).", sizeof(s_file_extend.result_info));
        update_filemod_destroy();
        return GM_SYSTEM_ERROR;
    }
        
    return GM_SUCCESS;
}


static void update_msg_pack_file_request(u8 *pdata, u16 *idx, u16 len, u16 current)
{
    if((*idx) + 14> len)
    {
        LOG(WARN,"clock(%d) update_msg_pack_file_request assert(len(%d)) failed.",util_clock(),len);
        return;
    }
    
    pdata[(*idx)++] = BHIGH_BYTE(s_file_extend.block_size);
    pdata[(*idx)++] = BLOW_BYTE(s_file_extend.block_size);
    
    pdata[(*idx)++] = BHIGH_BYTE(current); 
    pdata[(*idx)++] = BLOW_BYTE(current);
    GM_memcpy(&pdata[(*idx)], s_file_extend.file_id, 8);
    (*idx) += 8;
}

GM_ERRCODE update_msg_send_data_block_request(SocketType *socket)
{
    u8 buff[30];
    u16 len = sizeof(buff);
    u16 idx = 0, idx_save = 0;  //current place
    u16 current_block = 0; //从0一直到 s_file_extend.total_blocks
    u16 current_idx = 0;  //从s_file_extend.block_current计, 第 几个. [0-9]
    u8 one_send = 1;

    one_send = (STREAM_TYPE_DGRAM == config_service_update_socket_type())? UPDATE_MAX_PACK_ONE_SEND: UPDATE_MAX_PACK_ONE_SEND;
    update_msg_pack_head(buff, &idx, len);  //13 bytes
    idx_save = idx; // save idx
    
    for(current_block =s_file_extend.block_current,current_idx = 0; 
        current_block < s_file_extend.total_blocks && current_idx < one_send; 
        ++ current_block, ++ current_idx)
    {
        u32 check_bit = 0x01;
        if(current_idx) check_bit =check_bit << current_idx;
        if (s_file_extend.block_status & check_bit)
        {
            // already dealed data
            if(current_idx < (one_send  - 1))
            {
                continue;
            }
            //else 最后一个包要触发下一批请求
        }

        idx = idx_save;  // restore idx for each block
        len = sizeof(buff); // restore len
        
        update_msg_pack_file_request(buff, &idx, len, current_block);  //14 bytes
        update_msg_pack_id_len(buff, PROTOCCOL_UPDATE_REQUEST_FILE, idx);

        len=idx+2;  // 1byte checksum , 1byte 0xD

        LOG(DEBUG,"clock(%d) update_msg_send_data_block_request len(%d) current_block(%d|%d|%x).",
            util_clock(),len,current_block,s_file_extend.block_current,s_file_extend.block_status);

        if(GM_SUCCESS == gm_socket_send(socket, buff, len))
        {
            socket->send_time = util_clock();
        }
        else
        {
            s_file_extend.result = REPORT_RESULT_FAILED;
            GM_strncpy((char *)s_file_extend.result_info, "request blocks fail(data_block_request).", sizeof(s_file_extend.result_info));
            update_filemod_destroy();
            return GM_SYSTEM_ERROR;
        }
    }
    return GM_UNKNOWN;
}


void update_msg_start_data_block_request(SocketType *socket)
{
    GM_ERRCODE ret;
    int sys_error;
    bool bret = false;
    
    //init block status before sending messages.
    s_file_extend.block_size = UPDATE_PAKET_MAX_LEN;
    s_file_extend.block_status = 0;
    s_file_extend.result = REPORT_RESULT_FAILED;
    s_file_extend.result_info[0] = 0;
    s_file_extend.block_current = 0;
    
    s_file_extend.total_blocks = s_file_extend.total_len / s_file_extend.block_size ;
    if(s_file_extend.total_len % s_file_extend.block_size != 0)
    {
        s_file_extend.total_blocks += 1;
    }

    LOG(DEBUG,"clock(%d) update_msg_start_data_block_request. use_new_socket(%d) check_sum(0x%04X) total_len(%d) "
        "custom_code(%s) model(%s) file_id(%s) task_id(%s) total_blocks(%d) block_size(%d) block_current(%d) block_status(%d).",
        util_clock(),s_file_extend.use_new_socket,s_file_extend.check_sum,s_file_extend.total_len,s_file_extend.custom_code,s_file_extend.model,
        s_file_extend.file_id,s_file_extend.task_id,s_file_extend.total_blocks,s_file_extend.block_size,
        s_file_extend.block_current,s_file_extend.block_status);
    
    
    //从文件中读取上一次的s_file_extend, 与这次的比较, 如果相同, 就用上次的. 实现断点续传.
    ret = update_filemod_state_file_load();
    if(GM_SUCCESS == ret)
    {
        bret = is_update_file_extend_same(&s_file_extend, &s_file_extend_old);
    }

    if(UPD_RESERVER_LEN >= s_file_extend.total_len)
    {
        LOG(ERROR,"clock(%d) update_msg_start_data_block_request assert(total_len(%d)) failed.", util_clock(),s_file_extend.total_len);
        return;
    }

    //lz modified for only save at most 2 copies. 
    // master/minor both exist , delete minor
    if (GM_FS_CheckFile(UPDATE_TARGET_IMAGE) >= 0 && GM_FS_CheckFile(UPDATE_MINOR_IMAGE) >= 0)
    {
        util_delete_file(UPDATE_MINOR_IMAGE);
    }

    if(s_file_extend.handle >= 0)
    {
        // not possible 
        JsonObject* p_log_root = json_create();
        json_add_string(p_log_root, "event", "update_filemod_file_create need close");
        log_service_upload(INFO,p_log_root);
        GM_FS_Close(s_file_extend.handle);
        s_file_extend.handle = -1;
    }
    
    if(!bret)
    {
        util_delete_file(UPDATE_UPGRADE_FILE);

        //create file for write.
        ret = update_filemod_file_create(s_file_extend.total_len - UPD_RESERVER_LEN, &sys_error);
        if(GM_SUCCESS != ret)
        {
            s_file_extend.result = REPORT_RESULT_FAILED;
			GM_snprintf((char *)s_file_extend.result_info, sizeof(s_file_extend.result_info),"create file fail(!same).errcode=%d,%d", ret,sys_error);
            update_filemod_destroy();
            return;
        }
        
        s_file_extend.block_current = 0;
        s_file_extend.block_status = 0;
        LOG(DEBUG,"clock(%d) update_msg_start_data_block_request create new update file.", util_clock());
    
    }
    else
    {
        if(GM_SUCCESS != update_filemod_file_open())
        {
            //打不开文件, 删除状态文件重新开始
            LOG(WARN,"clock(%d) update_msg_start_data_block_request open() failed, redo update.", util_clock());

            util_delete_file(UPDATE_UPGRADE_STATE_FILE);
            s_file_extend.block_current = 0;
            s_file_extend.block_status = 0;

            util_delete_file(UPDATE_UPGRADE_FILE);
            
            //create file for write.
            ret = update_filemod_file_create(s_file_extend.total_len - UPD_RESERVER_LEN, &sys_error);
            if(GM_SUCCESS != ret)
            {
                s_file_extend.result = REPORT_RESULT_FAILED;
                GM_snprintf((char *)s_file_extend.result_info, sizeof(s_file_extend.result_info),"create file fail(same).errcode=%d,%d", ret,sys_error);
                update_filemod_destroy();
                return;
            }
        }
        else
        {
            LOG(DEBUG,"clock(%d) update_msg_start_data_block_request use old update file start at(%d).", 
                util_clock(), s_file_extend.block_current);
        }
    }
    
    update_msg_send_data_block_request(socket);
    return;
}

static void update_msg_parse_check_block_bits(u16 block)
{
    u16 current_idx = 0;  //从s_file_extend.block_current计, 第 几个. [0-9]
    u32 check_bit = 0x01;
    u8 one_send = 1;

    current_idx = block - s_file_extend.block_current;
    if(current_idx) check_bit=check_bit << current_idx;

    s_file_extend.block_status = s_file_extend.block_status | check_bit;
    one_send = (STREAM_TYPE_DGRAM == config_service_update_socket_type())? UPDATE_MAX_PACK_ONE_SEND: UPDATE_MAX_PACK_ONE_SEND;

    for(current_idx = 0; current_idx < one_send; ++ current_idx)
    {
		check_bit = 0x01;
        if(current_idx) check_bit=check_bit << current_idx;
        if (!(s_file_extend.block_status & check_bit))
        {
            return;
        }

        // block_current==0, current_idx==0 means first block.
        if((s_file_extend.block_current + current_idx + 1) >= s_file_extend.total_blocks)
        {
            // come to end block.
            break;
        }
    }

    // here all bits is set, 
    s_file_extend.block_current += one_send;
    if(s_file_extend.block_current >= s_file_extend.total_blocks)
    {
        s_file_extend.block_current = s_file_extend.total_blocks;
        if(GM_SUCCESS != update_filemod_file_finish())
        {
            s_file_extend.result = REPORT_RESULT_FINISH;
            GM_strncpy((char *)s_file_extend.result_info, "check sum failed.",sizeof(s_file_extend.result_info));
            LOG(WARN,"clock(%d) update_msg_parse_check_block_bits assert(update_filemod_file_finish) failed.", util_clock());
            update_filemod_destroy();
        }
        else
        {
            s_file_extend.result = REPORT_RESULT_UPDATED;
            LOG(INFO,"clock(%d) update_msg_parse_check_block_bits update_filemod_file_finish success.", util_clock());
            update_filemod_destroy();
        }
        
        s_file_extend.block_status = 0;
    }
    else
    {
        s_file_extend.block_status = 0;

        LOG(INFO,"clock(%d) update_msg_parse_check_block_bits allok(%d) one_send(%d).",
            util_clock(), s_file_extend.block_current, one_send);

        //写断点续传信息
        update_filemod_state_file_save();

        //接着请求后续包
        update_service_after_blocks_finish();
    }
}



static GM_ERRCODE update_filemod_file_finish(void)
{
    u32 check_sum=0;

    GM_FS_Close(s_file_extend.handle);
    s_file_extend.handle = -1;
    check_sum = update_filemod_get_checksum(UPDATE_UPGRADE_FILE);
    
    if(check_sum != s_file_extend.check_sum)
    {
        LOG(INFO,"clock(%d) check_sum(0x%04x) != (0x%04x).", util_clock(), check_sum,s_file_extend.check_sum);
        return GM_UNKNOWN;
    }

    s_file_extend.result = REPORT_RESULT_UPDATED;
    
    return GM_SUCCESS;
}



static GM_ERRCODE update_filemod_file_write(u32 fs_seek, u8* pdata, u32 len)
{
    int fret;
    u32 writen = 0;
    
    if(s_file_extend.handle < 0)
    {
        LOG(WARN,"clock(%d) update_filemod_file_write assert(handle(%d)) failed.", util_clock(), s_file_extend.handle);
        return GM_SYSTEM_ERROR;
    }
    
    fret = GM_FS_Seek(s_file_extend.handle, fs_seek, SEEK_SET);
    if (fret < 0)
    {
        LOG(INFO,"clock(%d) update_filemod_file_write GM_FS_Seek failed, ret:%d.", util_clock(),fret);
        GM_FS_Close(s_file_extend.handle);
        s_file_extend.handle = -1;
        return GM_SYSTEM_ERROR;
    }
    
    fret = GM_FS_Write(s_file_extend.handle, (void *)pdata, len, &writen);
    if (fret < 0 || writen != len)
    {
        LOG(INFO,"clock(%d) update_filemod_file_write GM_FS_Write failed. ret:%d, writen:%d.", util_clock(), fret, writen);
        GM_FS_Close(s_file_extend.handle);
        s_file_extend.handle = -1;
        return GM_SYSTEM_ERROR;
    }
    LOG(DEBUG,"clock(%d) update_filemod_file_write seek(%d) len(%d).", util_clock(), fs_seek, len);

    return GM_SUCCESS;
}

static GM_ERRCODE update_filemod_file_open(void)
{
    UINT file_size; 
    s_file_extend.handle = GM_FS_Open(UPDATE_UPGRADE_FILE, GM_FS_READ_WRITE | GM_FS_ATTR_ARCHIVE);
    if (s_file_extend.handle >= 0)
    {
        GM_FS_GetFileSize(s_file_extend.handle, &file_size);
        if(file_size != (s_file_extend.total_len - UPD_RESERVER_LEN))
        {
            LOG(INFO,"clock(%d) update_filemod_file_open assert(filesize==%d) failed.", util_clock(),s_file_extend.total_len);
            GM_FS_Close(s_file_extend.handle);
            s_file_extend.handle = -1;
            return GM_SYSTEM_ERROR;
        }
        return GM_SUCCESS;
    }

    
    return GM_NOT_INIT;
}


static GM_ERRCODE update_filemod_file_create(u32 fs_len, int *sys_error)
{
    int ret;
    u8 byte[1024];
    u32 writen = 0;
    u32 writen_total = 0;
    u32 will_write = 0;

    *sys_error = 0;
    s_file_extend.handle = GM_FS_Open(UPDATE_UPGRADE_FILE, GM_FS_READ_WRITE | GM_FS_ATTR_ARCHIVE | GM_FS_CREATE);
    if (s_file_extend.handle < 0)
    {
        LOG(DEBUG,"clock(%d) update_filemod_file_create assert(GM_FS_Open) failed.", util_clock());
        *sys_error = s_file_extend.handle;
        return GM_HARD_WARE_ERROR;
    }

    GM_memset(byte, 0, sizeof(byte));

    // 预先占用指定长度
    will_write = sizeof(byte);
    while(writen_total !=  fs_len)
    {
        if((fs_len - writen_total) < will_write)
        {
            will_write = (fs_len - writen_total);
        }
        
        ret = GM_FS_Write(s_file_extend.handle, (void *)byte, will_write, &writen);
        if ((ret < 0) || (writen==0))
        {
            LOG(DEBUG,"clock(%d) update_filemod_file_create assert(GM_FS_Write(ret:%d, writen:%d)) failed.", util_clock(), ret, writen);
            GM_FS_Close(s_file_extend.handle);
            s_file_extend.handle = -1;
            *sys_error = s_file_extend.handle;
            return GM_SYSTEM_ERROR;
        }
        writen_total += writen;
    }
    
    return GM_SUCCESS;
}



u32 update_filemod_get_checksum(const u16 *FileName)
{
    int fret = 0;
    u32 retlen = 0;
    u32 total_size = 0;
    int handle = -1;
    u8 buff[1024] = {0};
    u32 idx = 0;
    u32 check_sum=0;
    u32 read_total = 0;
    
    // 打开升级文件
    handle = GM_FS_Open(FileName, GM_FS_READ_WRITE | GM_FS_ATTR_ARCHIVE);
    if (handle < 0)
    {
        LOG(DEBUG,"clock(%d) update_filemod_get_checksum assert(GM_FS_Open) failed. ret:%d", util_clock(), handle);
        GM_FS_Close(handle);
        return 0;
    }
    
    // 获取文件长度
    GM_FS_GetFileSize(handle, &total_size);
    
    // 读取数据
    check_sum = 0x00;
    while(read_total < total_size)
    {
        fret = GM_FS_Read(handle, (void *)&buff, sizeof(buff), &retlen);
        if (fret != 0)
        {
            LOG(INFO,"clock(%d) update_filemod_get_checksum GM_FS_Read failed, ret:%d.", util_clock(), fret);
            GM_FS_Close(handle);
            return 0;
        }

        read_total += retlen;
        for (idx = 0; idx < retlen; idx++)
        {
            check_sum += buff[idx];
        }
    }
    
    GM_FS_Close(handle);
    
    return check_sum;
}


static void update_msg_pack_result_report(u8 *pdata, u16 *idx, u16 len)
{
    if((*idx) + 12 + GM_strlen((const char *)s_file_extend.result_info) > len)
    {
        LOG(WARN,"clock(%d) update_msg_pack_file_request assert(len(%d)) failed.", util_clock(), len);
        return;
    }

    // task id
    GM_memcpy(&pdata[(*idx)], s_file_extend.task_id, 8);
    (*idx) += 8;

    // result
    pdata[(*idx)++] = (u8)s_file_extend.result;

    //info len
    pdata[(*idx)++] = GM_strlen((const char *)s_file_extend.result_info);

    // info
    GM_memcpy(&pdata[(*idx)], s_file_extend.result_info, pdata[(*idx)-1]);
    (*idx) += pdata[(*idx)-1];
}



GM_ERRCODE update_msg_send_result_to_server(SocketType *socket)
{
    GM_ERRCODE ret;
    u8 buff[100];
    u16 len = sizeof(buff);
    u16 idx = 0;  //current place

    update_msg_pack_head(buff, &idx, len);  //13 bytes
    update_msg_pack_result_report(buff, &idx, len);  //12+N bytes
    update_msg_pack_id_len(buff, PROTOCCOL_UPDATE_REPORT, idx);
    
    len=idx+2;  // 1byte checksum , 1byte 0xD

    LOG(DEBUG,"clock(%d) update_msg_send_result_to_server len(%d) result(%d).",util_clock(),len,s_file_extend.result);

    if(GM_SUCCESS == (ret = gm_socket_send(socket, buff, len)))
    {
        socket->send_time = util_clock();
    }

    //成功升级,重启
    if(REPORT_RESULT_UPDATED == s_file_extend.result)
    {
        //确保result发给了服务器
        hard_ware_reboot(GM_REBOOT_UPDATE, 5);
    }

    if(REPORT_RESULT_FAILED != s_file_extend.result)
    {
        update_service_finish(UPDATE_PING_TIME);
    }
    else
    {
        static u32 s_fail_count  = 0;
        ++s_fail_count;
        if(s_fail_count < 100)
        {
            //尽快重新update
            update_service_finish(UPDATE_RETRY_TIME);
        }
        else
        {
            //失败太多次数了, 估计后面也是失败, 没必要增加服务器压力.
            update_service_finish(UPDATE_PING_TIME);
        }
    }
    
    return ret;
}


UpdateFileExtend *get_file_extend(void)
{
    return &s_file_extend;
}


void init_file_extend(bool boot_init)
{
    if(!boot_init)
    {
        if(s_file_extend.handle >= 0)
        {
            GM_FS_Close(s_file_extend.handle);
        }
    }
    GM_memset(&s_file_extend, 0, sizeof(s_file_extend));
    s_file_extend.handle = -1;
}


static bool is_update_file_extend_same(UpdateFileExtend *e1, UpdateFileExtend *e2)
{
    if(e1->block_size != e2->block_size)
    {
        return false;
    }

    if(e1->total_blocks != e2->total_blocks)
    {
        return false;
    }

    if(e1->check_sum != e2->check_sum)
    {
        return false;
    }
    
    if(e1->total_len != e2->total_len)
    {
        return false;
    }

    if(GM_strncmp((const char *)e1->custom_code,(const char *)e2->custom_code, sizeof(e1->custom_code)))
    {
        return false;
    }

    if(GM_strncmp((const char *)e1->model,(const char *)e2->model, sizeof(e1->custom_code)))
    {
        return false;
    }

    LOG(INFO,"clock(%d) is_update_file_extend_same same file start from:%d.", util_clock(), e2->block_current);

    //相同的情况下, 用旧的block_current
    e1->block_current = e2->block_current; //当前包序号
    e1->block_status = e2->block_status; // 从低到高, 每一位代表从当前包开始,往后的回收情况,收到回包为1,未收到为0
    return true;
}




static GM_ERRCODE update_filemod_state_file_save(void)
{
    int handle; // file handle
    int fret;
    u32 writen = 0;
    
    int idx;
    char *pbuf = (char *)&s_file_extend;

    // 由于要断点续传,所以收到的包要写到了文件系统中.
    GM_FS_Commit(s_file_extend.handle);

    s_file_extend.state_sum = 0;
    for (idx = 0; idx < (sizeof(s_file_extend)- sizeof(s_file_extend.state_sum)); idx++)
    {
        s_file_extend.state_sum += pbuf[idx];
    }

    handle = GM_FS_Open(UPDATE_UPGRADE_STATE_FILE, GM_FS_READ_WRITE | GM_FS_ATTR_ARCHIVE | GM_FS_CREATE);
    if (handle < 0)
    {
        LOG(DEBUG,"clock(%d) update_filemod_state_file_save assert(GM_FS_Open) failed.", util_clock());
        return GM_HARD_WARE_ERROR;
    }

    
    fret = GM_FS_Write(handle, &s_file_extend, sizeof(s_file_extend), &writen);
    if (fret < 0 || (writen != sizeof(s_file_extend)))
    {
        LOG(INFO,"clock(%d) update_filemod_state_file_save GM_FS_Write failed. ret:%d, writen:%d.", util_clock(), fret, writen);
        GM_FS_Close(handle);
        return GM_SYSTEM_ERROR;
    }
    LOG(INFO,"clock(%d) update_filemod_state_file_save.", util_clock());
    
    GM_FS_Close(handle);
    return GM_SUCCESS;
}


static GM_ERRCODE update_filemod_state_file_load(void)
{
    int handle; // file handle
    int fret;
    u32 readed = 0;
    int idx;
    char *pbuf;
    u32 sum = 0;

    handle = GM_FS_Open(UPDATE_UPGRADE_STATE_FILE, GM_FS_READ_WRITE | GM_FS_ATTR_ARCHIVE);
    if (handle < 0)
    {
        return GM_NOT_INIT;
    }

    
    fret = GM_FS_Read(handle, &s_file_extend_old, sizeof(s_file_extend_old), &readed);
    if (fret < 0 || (readed != sizeof(s_file_extend_old)))
    {
        LOG(INFO,"clock(%d) update_filemod_state_file_load GM_FS_Read failed. ret:%d, writen:%d.", util_clock(), fret, readed);
        GM_FS_Close(handle);
        return GM_SYSTEM_ERROR;
    }
    
    GM_FS_Close(handle);

    pbuf = (char *)&s_file_extend_old;
    for (idx = 0; idx < (sizeof(s_file_extend_old)- sizeof(s_file_extend_old.state_sum)); idx++)
    {
        sum += pbuf[idx];
    }

    if(sum == s_file_extend_old.state_sum)
    {
        LOG(DEBUG,"clock(%d) update_filemod_state_file_load use_new_socket(%d) check_sum(%d) total_len(%d) "
            "custom_code(%s) model(%s) file_id(%s) task_id(%s) total_blocks(%d) block_size(%d) block_current(%d) block_status(%d).",
            util_clock(),s_file_extend_old.use_new_socket,s_file_extend_old.check_sum,s_file_extend_old.total_len,s_file_extend_old.custom_code,s_file_extend_old.model,
            s_file_extend_old.file_id,s_file_extend_old.task_id,s_file_extend_old.total_blocks,s_file_extend_old.block_size,
            s_file_extend_old.block_current,s_file_extend_old.block_status);
        return GM_SUCCESS;
    }
    else
    {
        LOG(INFO,"clock(%d) update_filemod_state_file_load assert(sum) failed.", util_clock());
        return GM_NOT_INIT;
    }
}


