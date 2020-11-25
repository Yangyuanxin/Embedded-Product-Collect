#ifndef __GPS_SERVICE_H__
#define __GPS_SERVICE_H__

#include "gm_type.h"
#include "error_code.h"
#include "socket.h"
#include "gps.h"
#include "gps_save.h"


#define GPS_LOGIN_MSG_FAIL_MAX 1
#define GPS_HEART_MSG_RECEIVE_TIME_OUT 420


/**
 * Function:   创建gps_service模块
 * Description:创建gps_service模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   使用前必须调用,否则调用其它接口返回失败错误码
 */
GM_ERRCODE gps_service_create(bool first_create);

/**
 * Function:   
 * Description: 配置变化,重启gps_service模块
                在断连服务器, 重建连接时,会使用新信息
 * Input:      无
 * Output:     无
 * Return:     无
 * Others:     
 */
GM_ERRCODE gps_service_change_config(void);


/**
 * Function:   销毁gps_service模块
 * Description:销毁gps_service模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE gps_service_destroy(void);


/**
 * Function:   gps_service模块定时处理入口
 * Description:gps_service模块定时处理入口
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   1秒钟调用1次
 */
GM_ERRCODE gps_service_timer_proc(void);




//状态处理辅助函数
void gps_service_connection_ok(void);
void gps_service_connection_failed(void);
void gps_service_close_for_reconnect(void);

/*
    给config_service用于判断
*/
u8 gps_service_socket_is_same(void);

void gps_service_after_register_response(void);
void gps_service_after_login_response(void);
void gps_service_after_receive_heartbeat(void);
void gps_service_after_receive_remote_msg(u8 *pRet, u16 len);
void gps_service_after_server_req(void);
void gps_service_after_param_get(void);
void gps_service_after_server_locate_req(void);


//GPS数据点上报类型 
typedef enum
{
    GPS_MODE_NONE = -1,           // 不上传
    GPS_MODE_FIX_TIME = 0,        // 定时上传 
    GPS_MODE_FIX_DISTANCE = 1,    // 定距上传（终端未做） 
    GPS_MODE_TURN_POINT = 2,      // 拐点上传 
    GPS_MODE_STATUS_CHANGE = 3,   // ACC状态改变上传 
    GPS_MODE_STATIC_POSITION = 4, // 静止最后位置上传 
    GPS_MODE_POWER_UP = 5         // 上电登录成功后直接上传
}GpsDataModeEnum;

/**
 * Function:   after gps module generate gps data. it push to gps_service module
 * Description:
 * Input:	   GPSData,如果是NULL则表示定位失败需要上报LBS
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE gps_service_push_gps(GpsDataModeEnum mode, const GPSData *p_gps_data);

/**
 * Function:   ACC等状态发生变化后，立即触发一次心跳
 * Description:
 * Input:	   无
 * Output:	   无
 * Return:	   无
 * Others:	   
 */
void gps_service_heart_atonce(void);


/**
 * Function:   获取心跳成功次数
 * Description:
 * Input:	   无
 * Output:	   无
 * Return:	   心跳成功次数
 * Others:	   
 */
U32 gps_service_get_heart_counts(void);

/**
 * Function:   获取心跳成功次数
 * Description:
 * Input:	   无
 * Output:	   无
 * Return:	   心跳成功次数
 * Others:	   
 */
U32 gps_service_get_location_counts(void);

/**
 * Function:   
 * Description: jt808使用gps定位包代替心跳
 * Input:	   mode
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE gps_service_send_one_locate(GpsDataModeEnum mode, bool use_lbs);


/**
 * Function:   收到指令时, 触发上报基站信息, 
               开机超时未定位, 触发上报基站信息, 
               未开启gps定位时, 定时上报lbs数据
 * Description:
 * Input:	   force: true表示未开启lbs上报的情况下也要发送
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE gps_service_push_lbs(void);

typedef enum
{
    ALARM_NONE = 0,
    ALARM_POWER_OFF = 0x01,      //电源断电报警,
    ALARM_BATTERY_LOW = 0x03,    //电池低电报警
    ALARM_SHOCK = 0x04,          //震动报警
	ALARM_MOVE = 0x05,			 //车辆移动报警（使用原位移报警位）
    ALARM_SPEED = 0x0d,          //超速报警
    ALARM_FAKE_CELL = 0x0f,      //伪基站报警
    ALARM_POWER_HIGH = 0x11,     //电源电压过高报警
    ALARM_COLLISION = 0x12,      //碰撞报警
    ALARM_SPEED_UP = 0x13,       //急加速报警
    ALARM_SPEED_DOWN = 0x14,     //急减速报警
    ALARM_TURN_OVER = 0x15,      //翻转报警
    ALARM_SHARP_TURN = 0x16,     //急转弯报警
    ALARM_REMOVE = 0x17,         //拆动报警
    ALARM_MAX
}AlarmTypeEnum;

typedef struct
{
    AlarmTypeEnum type;
    u16 info;
}AlarmInfo;


/**
 * Function:   发送报警数据
 * Description:
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE gps_service_push_alarm(AlarmInfo *alarm);


/**
 * Function:   发送地址请求
 * Description:
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE gps_service_push_position_request(u8 *mobile_num, u8 num_len, u8 *command, u8 cmd_len);


/**
 * Function:   关闭gprs连接, 如果正在升级就只关闭gps连接
 * Description:升级完毕后仍然会关闭gprs连接
 * Input:	   无
 * Output:	   无
 * Return:	   无
 * Others:	   
 */
void gps_service_destroy_gprs(void);

/**
 * Function:   查询是否登录成功
 * Description:
 * Input:	   无
 * Output:	   无
 * Return:	   true——成功;false——没有成功
 * Others:	   
 */
bool gps_service_is_login_ok(void);


/**
 * Function:   先放到cache中以防丢失，再发送。
 * Description:
 * Input:	   data:待发数据    len:待发数据长度
 * Output:	   无
 * Return:	   无
 * Others:	   
 */
GM_ERRCODE gps_service_cache_send(u8 *data, u8 len);


/**
 * Function:   根据socket ack决定数据是否从缓存中删除
 * Description:
 * Input:	   无
 * Output:	   无
 * Return:	   无
 * Others:	   
 */
void gps_service_confirm_gps_cache(SocketType *socket);

/**
 * Function:   获取当前正在使用的IP地址
 * Description:
 * Input:	   无
 * Output:	   无
 * Return:	   IP地址首地址
 * Others:	   
 */
U8* gps_service_get_current_ip(void);


#endif


