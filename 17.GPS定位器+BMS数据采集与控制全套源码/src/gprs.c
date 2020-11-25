#include "gm_type.h"
#include "stdio.h"
#include "gm_stdlib.h"
#include "gm_gprs.h"
#include "socket.h"
#include "gm_memory.h"
#include "gm_network.h"
#include "config_service.h"
#include "gps_service.h"
#include "log_service.h"
#include "agps_service.h"
#include "update_service.h"
#include "update_file.h"
#include "utility.h"
#include "gm_system.h"
#include "gprs.h"
#include "socket.h"
#include "hard_ware.h"
#include "gsm.h"
#include "led.h"


typedef enum 
{
    CURRENT_GPRS_INIT = 0,
    CURRENT_GPRS_CALL_OK = 1,  // 语音ok, 可以注册apn了
    CURRENT_GPRS_ACTIVATED = 2,  // 网络准备好了,可以建socket了
    CURRENT_GPRS_TIME_PROC = 3,  //after create services
    CURRENT_GPRS_FAILED = 4,
	CURRENT_GPRS_STATUS_MAX,
}CurrentGetGprsConnectStatusEnum;


typedef struct
{
    u8 status;      /* current status */
    u32 failed_time;  //故障, 准备重新开始注册时间
    u32 start_apn_time;   //开始注册apn时间
    u32 last_good_time;  // 上次网络正常的时间
    u32 call_ok_count;   //CURRENT_GPRS_INIT->CURRENT_GPRS_CALL_OK 次数
}GprsType;
static GprsType s_gprs;

#define GPRS_REREGIST_INTERVALL    5
#define GPRS_REREGIST_NEED_TIME_MAX  90
#define GPRS_STATUS_STRING_MAX_LEN 32
#define GPRS_REBOOT_HEART_FAILEDTIMES  7

const char s_gprs_status_string[CURRENT_GPRS_STATUS_MAX][GPRS_STATUS_STRING_MAX_LEN] = 
{
    "CURRENT_GPRS_INIT",
    "CURRENT_GPRS_CALL_OK",
    "CURRENT_GPRS_ACTIVATED",
    "CURRENT_GPRS_TIME_PROC",
    "CURRENT_GPRS_FAILED",
};

static GM_ERRCODE gprs_transfer_status(u8 new_status);
static void gprs_init_proc(void);
static void gprs_call_ok_proc(void);
static void gprs_failed_proc(void);
static void gprs_config_apn(void);

const char * gprs_status_string(u8 statu)
{
    return s_gprs_status_string[statu];
}

static void gprs_config_apn(void)
{
    int account_id = -1;
    ST_GprsConfig apnConfig;
    goome_auto_apn_struct auto_apn;
    gm_cell_info_struct cell_info;
    u8 imsi[GM_IMSI_LEN + 1];
    u8 auto_check_apn = false;

    GM_memset(&auto_apn, 0 , sizeof(auto_apn));
    GM_memset(&apnConfig, 0 , sizeof(apnConfig));
    config_service_get(CFG_APN_CHECK, TYPE_BOOL, &auto_check_apn, sizeof(auto_check_apn));

    LOG(INFO,"gprs_config_apn auto_check_apn(%d).",auto_check_apn);
    if(auto_check_apn)
    {
        // 从sim卡中读出imsi
        // 第一次启动一定拿不到IMSI，所以这里一定是失败的，会先用默认的CMNET，如果CMNET失败了再重新初始化的时候就可以拿到IMSI了
        if((GM_SUCCESS == gsm_get_imsi(imsi)) && (GM_SUCCESS == gsm_get_cell_info(&cell_info)))
        {
            GM_memset(&auto_apn, 0x00, sizeof(goome_auto_apn_struct));
            auto_apn.mcc = cell_info.serv_info.mcc;
            if (cell_info.serv_info.mnc > 100)
            {
                auto_apn.mnc = util_chr(imsi[3]) * 100 + util_chr(imsi[4]) * 10 + util_chr(imsi[5]);
            }
            else
            {
                auto_apn.mnc = util_chr(imsi[3]) * 10 + util_chr(imsi[4]);
            }

            //自动检测apn
            if (0 == GM_AutoApnCheck(&auto_apn))
            {
                GM_strncpy((char*)apnConfig.apnName, (const char*)auto_apn.apn, sizeof(apnConfig.apnName));
                GM_strncpy((char*)apnConfig.apnUserId, (const char*)auto_apn.usrname, sizeof(apnConfig.apnUserId));
                GM_strncpy((char*)apnConfig.apnPasswd, (const char*)auto_apn.usrpass, sizeof(apnConfig.apnPasswd));
                LOG(INFO,"config_service_apn auto config(%s).",apnConfig.apnName);
            }
            else
            {
                LOG(ERROR,"config_service_apn assert(GM_AutoApnCheck) failed.");
                GM_strcpy((char*)apnConfig.apnName,"CMNET");
            }
        }
        else
        {
            LOG(WARN,"config_service_apn assert(gsm_get_imsi) failed.");
            GM_strcpy((char*)apnConfig.apnName,"CMNET");
        }
    }

    //手工设置apn
    if(0 == GM_strlen((char*)auto_apn.apn))
    {
		GM_strncpy((char*)apnConfig.apnName, (const char*)config_service_get_pointer(CFG_APN_NAME), sizeof(apnConfig.apnName));
		GM_strncpy((char*)apnConfig.apnUserId, (const char*)config_service_get_pointer(CFG_APN_USER), sizeof(apnConfig.apnUserId));
		GM_strncpy((char*)apnConfig.apnPasswd, (const char*)config_service_get_pointer(CFG_APN_PWD), sizeof(apnConfig.apnPasswd));
    }

    //注册apn后, 就可以通过 GM_GetBearerStatus 检测网络状态
    GM_ApnConfig(&apnConfig, &account_id);
    LOG(INFO,"config_service_apn get account_id(%d). name(%s),user(%s),pwd(%s),auth(%d)", account_id, 
		apnConfig.apnName, apnConfig.apnUserId, apnConfig.apnPasswd, apnConfig.authtype);

    gm_socket_set_account_id(account_id);
}

static GM_ERRCODE gprs_transfer_status(u8 new_status)
{
    u8 old_status = (u8)s_gprs.status;
    GM_ERRCODE ret = GM_PARAM_ERROR;
    switch(s_gprs.status)
    {
        case CURRENT_GPRS_INIT:
            switch(new_status)
            {
                case CURRENT_GPRS_INIT:
                    break;
                case CURRENT_GPRS_CALL_OK:
                    s_gprs.call_ok_count ++;
                    ret = GM_SUCCESS;
                    break;
                case CURRENT_GPRS_ACTIVATED:
                    break;
                case CURRENT_GPRS_TIME_PROC:
                    break;
                case CURRENT_GPRS_FAILED:
                    ret = GM_SUCCESS;
                    break;
                default:
                    break;
            }
            break;
        case CURRENT_GPRS_CALL_OK:
            switch(new_status)
            {
                case CURRENT_GPRS_INIT:
                    break;
                case CURRENT_GPRS_CALL_OK:
                    break;
                case CURRENT_GPRS_ACTIVATED:
                    ret = GM_SUCCESS;
                    break;
                case CURRENT_GPRS_TIME_PROC:
                    break;
                case CURRENT_GPRS_FAILED:
                    ret = GM_SUCCESS;
                    break;
                default:
                    break;
            }
            break;
        case CURRENT_GPRS_ACTIVATED:
            switch(new_status)
            {
                case CURRENT_GPRS_INIT:
                    break;
                case CURRENT_GPRS_ACTIVATED:
                    break;
                case CURRENT_GPRS_TIME_PROC:
                    ret = GM_SUCCESS;
                    break;
                case CURRENT_GPRS_FAILED:
                    ret = GM_SUCCESS;
                    break;
                default:
                    break;
            }
            break;
        case CURRENT_GPRS_TIME_PROC:
            switch(new_status)
            {
                case CURRENT_GPRS_INIT:
                    break;
                case CURRENT_GPRS_ACTIVATED:
                    break;
                case CURRENT_GPRS_TIME_PROC:
                    break;
                case CURRENT_GPRS_FAILED:
                    ret = GM_SUCCESS;
                    break;
                default:
                    break;
            }
            break;
        case CURRENT_GPRS_FAILED:
            switch(new_status)
            {
                case CURRENT_GPRS_INIT:
                    ret = GM_SUCCESS;
                    break;
                case CURRENT_GPRS_ACTIVATED:
                    break;
                case CURRENT_GPRS_TIME_PROC:
                    break;
                case CURRENT_GPRS_FAILED:
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
        s_gprs.status = new_status;
        LOG(INFO,"clock(%d) gprs_transfer_status from %s to %s success", util_clock(), gprs_status_string(old_status),gprs_status_string(new_status));
    }
    else
    {
        LOG(WARN,"clock(%d) gprs_transfer_status assert(from %s to %s) failed", util_clock(), gprs_status_string(old_status),gprs_status_string(new_status));
    }

    return ret;

}


void gprs_socket_notify(void* msg_ptr)
{
    SocketType * socket;
    gm_soc_notify_ind_struct *msg = (gm_soc_notify_ind_struct *)msg_ptr;
    
    socket = get_socket_by_id(msg->socket_id);
    if(!socket)
    {
        LOG(ERROR,"clock(%d) gprs_socket_notify assert(socket_id(%d)) failed.", util_clock(), msg->socket_id);
        GM_SocketClose(msg->socket_id);
        return;
    }
    else
    {
        LOG(DEBUG,"clock(%d) gprs_socket_notify socket->access_id(%d) id(%d) result(%d) event(%d).", 
            util_clock(), socket->access_id, socket->id, msg->result, msg->event_type);
    }
    
    if (msg->result)
    {
        switch (msg->event_type)
        {
            case GM_SOC_READ:
                gm_socket_recv(socket);
                break;
                
            case GM_SOC_WRITE:          
                LOG(DEBUG,"clock(%d) gprs_socket_notify GM_SOC_WRITE(%d).", util_clock(), msg->socket_id);
                break;
                
            case GM_SOC_ACCEPT:
                LOG(DEBUG,"clock(%d) gprs_socket_notify GM_SOC_ACCEPT(%d).", util_clock(), msg->socket_id);
                break;
                
            case GM_SOC_CONNECT:
            {
                LOG(INFO,"clock(%d) gm_socket_connect tcp(%d.%d.%d.%d:%d) id(%d) access_id(%d) success.", 
                    util_clock(), socket->ip[0], socket->ip[1], socket->ip[2], socket->ip[3], socket->port, socket->id, socket->access_id);
                gm_socket_connect_ok(socket);
                break;
            }
            
            case GM_SOC_CLOSE:
            {
                LOG(INFO,"clock(%d) gprs_socket_notify id(%d) close. error_cause(%d)", util_clock(), msg->socket_id,msg->error_cause);
                break;
            }
            default:
            {
                LOG(DEBUG,"clock(%d) gprs_socket_notify id(%d) unknown. error_cause(%d).", util_clock(), msg->socket_id,msg->error_cause);
                break;
            }
        }
    }
    else
    {
        char reason[30];
        snprintf(reason,sizeof(reason),"s(%d) err(%d)", socket->access_id, msg->error_cause);
        reason[sizeof(reason) - 1] = 0;
        system_state_set_gpss_reboot_reason(reason);

        LOG(DEBUG,"clock(%d) gprs_socket_notify socket_id(%d) error_cause(%d).", util_clock(), msg->socket_id,msg->error_cause);
        if (GM_CheckSimValid() == 0)
        {
            LOG(INFO,"clock(%d) gprs_socket_notify GM_CheckSimValid failed.", util_clock());
        }

        if(socket->access_id == SOCKET_INDEX_MAIN)
        {
            gps_service_destroy_gprs();
        }
        else
        {
            gm_socket_destroy(socket);
        }
    }
}


GM_ERRCODE gprs_create(void)
{
    s_gprs.status = CURRENT_GPRS_INIT;

    // 在gprs_init_proc中需要检测
    s_gprs.last_good_time = s_gprs.failed_time = util_clock();
    s_gprs.call_ok_count = 0;
    
    gm_socket_global_init();
	agps_service_create(true);
    config_service_create();
    gps_service_create(true);
	update_service_create(true);
	log_service_create();
    GM_DnsParserCallback(socket_get_host_by_name_callback);
    
    
    return GM_SUCCESS;
}

static void gprs_init_proc(void)
{
    u32 current_time = 0;
    
    if (GM_GetServiceAvailability())
    {
        // 注网成功(包括cg 与creg)
        U8 iccid[30] = {0};
		JsonObject* p_log_root = json_create();
		
		gsm_get_iccid(iccid);
		json_add_string(p_log_root, "event", "GPRS call ok");
		json_add_string(p_log_root, "iccid", (char*)iccid);
		json_add_int(p_log_root, "csq", gsm_get_csq());
		
        gprs_config_apn();
        s_gprs.start_apn_time = util_clock();
        gprs_transfer_status(CURRENT_GPRS_CALL_OK);

		GM_SocketRegisterCallBack(gprs_socket_notify);
		
		log_service_upload(INFO,p_log_root);
		
    }
    else
    {
        led_set_gsm_state(GM_LED_OFF);
        current_time = util_clock();
        if((current_time - s_gprs.failed_time) > GPRS_REREGIST_NEED_TIME_MAX)
        {
            LOG(DEBUG,"clock(%d) gprs_init_proc exceed(%d)",util_clock(),GPRS_REREGIST_NEED_TIME_MAX);
            //system_state_set_gpss_reboot_reason("gprs_init_proc");
            gprs_destroy();
        }
        // else recheck.
    }
}


static void gprs_call_ok_proc(void)
{
    u32 current_time = 0;
    s32 bear_status = 0;
    GM_GetBearerStatus(&bear_status);

    if (4 == bear_status)
    {
        u8 local_ip[4] = {0};
        u8 ret = 0;
		JsonObject* p_log_root = json_create();
        char print_string[100];
        //此时网络可用了
        led_set_gsm_state(GM_LED_FLASH);
        gprs_transfer_status(CURRENT_GPRS_ACTIVATED);


        // 网络可用了
        ret = GM_GetLocalIP(local_ip);
        sprintf(print_string,"GPRS net ok. ret=%d, ip=%d.%d.%d.%d", ret, local_ip[0], local_ip[1], local_ip[2], local_ip[3]);
		json_add_string(p_log_root, "event", print_string);
		json_add_int(p_log_root, "csq", gsm_get_csq());
		log_service_upload(INFO,p_log_root);
    }
    else
    {
        current_time = util_clock();
        if((current_time - s_gprs.start_apn_time) > GPRS_REREGIST_NEED_TIME_MAX)
        {
            /*char reason[30];
            snprintf(reason,sizeof(reason),"bear_status(%d)", bear_status);
            reason[sizeof(reason) - 1] = 0;
            system_state_set_gpss_reboot_reason(reason);
            */
            LOG(INFO,"clock(%d) gprs_call_ok_proc gprs_destroy because bear_status(%d)",util_clock(),bear_status);
            gprs_destroy();
        }
    }
}

GM_ERRCODE gprs_destroy(void)
{
    //上传日志
    JsonObject* p_log_root = json_create();
    char ip_str[16] = {0};
	U8 iccid[30] = {0};

	U8* ip = gps_service_get_current_ip();

	gsm_get_iccid(iccid);
	GM_snprintf(ip_str, 16, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
	json_add_string(p_log_root, "event", "GPRS Failed");
	json_add_string(p_log_root, "addr", config_service_get_pointer(CFG_SERVERADDR));
	json_add_string(p_log_root, "ip", ip_str);
	json_add_string(p_log_root, "iccid", (char*)iccid);
	json_add_int(p_log_root, "csq", gsm_get_csq());
	log_service_upload(INFO,p_log_root);
	
    gm_socket_global_destroy();

    //关闭gprs网络等待重新注册成功
    GM_AccountIdClose();
    led_set_gsm_state(GM_LED_FLASH);

    s_gprs.failed_time = util_clock();
    gprs_transfer_status(CURRENT_GPRS_FAILED);
	GM_SocketRegisterCallBack(NULL);
    return GM_SUCCESS;
}

static void gprs_failed_proc(void)
{
    u32 current_time = util_clock();

    //卡失效时，要进入飞行模式再退出才可用。但飞行模式用得少，还是重启靠谱
	if(gprs_check_need_reboot(s_gprs.last_good_time))
	{
        return;
	}
	
    if((current_time - s_gprs.failed_time) >= GPRS_REREGIST_INTERVALL)
    {
        gprs_transfer_status(CURRENT_GPRS_INIT);
    }

}

GM_ERRCODE gprs_timer_proc(void)
{
    switch(s_gprs.status)
    {
        case CURRENT_GPRS_INIT:
            gprs_init_proc();
            break;
        case CURRENT_GPRS_CALL_OK:
            gprs_call_ok_proc();
            break;
        case CURRENT_GPRS_ACTIVATED:
            //本意是在此创建各service, 但目前用不上
            gprs_transfer_status(CURRENT_GPRS_TIME_PROC);
            break;
        case CURRENT_GPRS_TIME_PROC:
            s_gprs.last_good_time = util_clock();
            gm_socket_get_host_timer_proc();
            agps_service_timer_proc();
            config_service_timer_proc();
            
            // config_service 放前面, 确保第一次能创建service
            update_service_timer_proc();
            gps_service_timer_proc();
            log_service_timer_proc();
            break;
        case CURRENT_GPRS_FAILED:
            gprs_failed_proc();
            break;
    }
    return GM_SUCCESS;
}

bool gprs_is_ok(void)
{
 	return (CURRENT_GPRS_TIME_PROC == s_gprs.status);
}


bool gprs_check_need_reboot(u32 check_time)
{
    //保命手段 , 网络一直不好, 重启
    U16 value_u16 = 0xFFFF;
    config_service_get(CFG_HEART_INTERVAL, TYPE_SHORT, &value_u16, sizeof(value_u16));
	//7次心跳失败重启，考虑时间误差再加5秒
    if((util_clock() - check_time) > (GPRS_REBOOT_HEART_FAILEDTIMES*value_u16 + 5))
    {
        hard_ware_reboot(GM_REBOOT_GPRS,1);
        return true;
    }
    return false;
}

u32 gprs_get_last_good_time(void)
{
    return s_gprs.last_good_time;
}

u32 gprs_get_call_ok_count(void)
{
    return s_gprs.call_ok_count;
}

