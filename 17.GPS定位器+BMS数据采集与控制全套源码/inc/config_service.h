/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        config_service.h
 * Author:           梁震       
 * Version:          1.0
 * Date:             2019-03-01
 * Description:      
 * Others:      
 * Function List:    
    1. 创建config_service模块
    2. 销毁config_service模块
    3. config_service模块定时处理入口
 * History: 
    1. Date:         2019-03-01
       Author:       梁震
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#ifndef __CONFIG_SERVICE_H__
#define __CONFIG_SERVICE_H__

#include "gm_type.h"
#include "error_code.h"
#include "socket.h"
#include "gps.h"

//发布版修改这个数字
#define VERSION_NUMBER  "2.0.75"


#define APN_DEFAULT "CMNET"
#define APN_USER_DEFAULT ""
#define APN_PWD_DEFAULT ""
#define GOOME_IMEI_DEFAULT "866717048888889"
#define UPDATE_OEM_CODE  "GOOME"
#define UPDATE_DEVICE_CODE "DEVICEID00001"
#define UPDATE_BOOT_CODE "FFFFFFFF"
#define CONFIG_LITE_SERVER_ADDERSS     "litedev.gmiot.net:10021"
#define CONFIG_LITE_SERVER_IP          "119.23.109.222"
#define CONFIG_GOOCAR_SERVER_ADDERSS   "gs03.szdatasource.com:8821"
#define CONFIG_GOOCAR_SERVER_IP        "54.222.183.200"
#define CONFIG_FACTORY_SERVER_ADDERSS  "factorytest.gpsorg.net:8821"
#define CONFIG_SERVER_ADDERSS          "config.gpsorg.net:39996"
#define CONFIG_SERVER_IP               "47.106.251.151"
#define CONFIG_AGPS_SERVER_ADDERSS     "agps.srv.gpsorg.net:8866"
#define CONFIG_AGPS_SERVER_IP          "119.23.200.49"
#define CONFIG_LOG_SERVER_ADDERSS      "firmwarelog.gpsorg.net:39998"
#define CONFIG_LOG_SERVER_IP           "47.106.251.151"
#define GOOME_UPDATE_SERVER_DNS        "firmware.gpsorg.net:39999"
#define GOOME_UPDATE_SERVER_IP         "47.106.251.151"

#define CONFIG_CMD_MAX_LEN         10
#define CONFIG_STRING_MAX_LEN      100

#define CONFIG_UPLOAD_TIME_MAX     10800ul
#define CONFIG_UPLOAD_TIME_DEFAULT 10 
#define CONFIG_UPLOAD_TIME_MIN     3 
#define CONFIG_SPEEDTHR_MAX        180
#define CONFIG_SPEEDTHR_MIN        5
#define CONFIG_STATIC_TIME_MAX     500
#define CONFIG_ACCTHR_MAX          2.0f
#define CONFIG_ACCTHR_MIN          0.1f
#define CONFIG_BRAKETHR_MAX        3.0f
#define CONFIG_BRAKETHR_MIN        0.5f
#define CONFIG_CRASHLTHR_MAX       6.0f
#define CONFIG_CRASHLTHR_MIN       1.0f
#define CONFIG_CRASHMTHR_MAX       8.0f
#define CONFIG_CRASHMTHR_MIN       4.0f
#define CONFIG_CRASHHTHR_MAX       15.0f
#define CONFIG_CRASHHTHR_MIN       8.0f
#define CONFIG_TURNTHR_MAX         5.0f
#define CONFIG_TURNTHR_MIN         0.1f
#define CONFIG_STATICTHR_MAX       1.0f
#define CONFIG_STATICTHR_MIN       0.0f
#define CONFIG_RUNTHR_MAX          1.0f
#define CONFIG_RUNTHR_MIN          0.0f
#define CONFIG_QUAKETHR_MAX        1.0f
#define CONFIG_QUAKETHR_MIN        0.0f
#define CONFIG_LOW_VOLTHR_MAX      15.0f
#define CONFIG_LOW_VOLTHR_MIN      0.0f
#define GOOME_LITEDEV_DNS          ".gmiot.net"
#define GOOME_GPSOO_DNS            ".szdatasource.com"
#define GOOME_HEARTBEAT_MAX        360ul
#define GOOME_HEARTBEAT_DEFAULT    180ul
#define GOOME_HEARTBEAT_MIN        3
#define GOOME_SLEEPTIME_MAX        30
#define GOOME_SLEEPTIME_DEFAULT    5
#define GOOME_SLEEPTIME_MIN        0
#define CONFIG_ANGLETIME_MAX       0xFF
#define CONFIG_ANGLETIME_DEFAULT   10
#define CONFIG_ANGLETIME_MIN       0
#define CONFIG_SHAKECOUT_MAX       100
#define CONFIG_SHAKECOUT_DEFAULT   5
#define CONFIG_SHAKECOUT_MIN       1
#define CONFIG_AUTODEFDELAY_MAX       1200
#define CONFIG_AUTODEFDELAY_DEFAULT   300
#define CONFIG_AUTODEFDELAY_MIN       1
#define CONFIG_SHAKEALARMDELAY_MAX       1200
#define CONFIG_SHAKEALARMDELAY_DEFAULT   30
#define CONFIG_SHAKEALARMDELAY_MIN       1
#define CONFIG_SHAKEALARMINTERVAL_MAX       3600
#define CONFIG_SHAKEALARMINTERVAL_DEFAULT   1800
#define CONFIG_SHAKEALARMINTERVAL_MIN       1

#define GOOME_MAPS_URL_DEFAULT "http://ditu.google.cn/maps?q="
//部标厂商标志.    后面可通过参数配置来修改.
#define LOW_VOLTAGE_ALARM_PERCENT 0.8
#define JT_OEM_ID "11111"   
#define JT_DEV_ID "9887125"
#define JT_VEHICLE_NUMBER "粤B99988"
#define GB_ATTYPE_STANDARD     0x00   //标准
#define GB_ATTYPE_CUSTOM       0x01   //补充
#define LBS_UPLOAD_DEFAULT 60
#define GPS_NOT_FIXED_DEFAULT 60


typedef enum
{
    //为兼容的原因, 这些配置要连在一起 BEGIN
    CFG_SERVERADDR,           // 主服务器地址,xxx.xxx.xxx.xxx:port      = 0
    CFG_UPLOADTIME,           // 上传gps间隔
    CFG_SPEEDTHR,             // 超速阀值
    CFG_PROTOCOL,             // ConfigProtocolEnum
    CFG_CUTOFFALM_DISABLE,    // 断电报警开关 0,打开,1:关闭
    CFG_LOWBATTALM_DISABLE,   // 电池低电报警开关  0,打开,1:关闭
    CFG_SLEEP_TIME,           // 休眠时间
    CFG_CUTOILFUNC,           // 断油电功能开关
    CFG_ACCFUNC,              // ACC功能开关
    CFG_DEVICETYPE,           // 设备型号         //= 10
    CFG_SERVERLOCK,           // 是否锁IP（域名）
    CFG_BATTUPLOAD_DISABLE,   // 是否上传电量   1 :不上传  0:上传 默认0
    CFG_VIBSETWAY,            // 震动报警是否手动设防
    CFG_VOLTAGETHR,           // 工作电压阈值 
    CFG_GSENNUM,              // 加速度传感器数量
    CFG_GSENRANGE,            // 加速度传感器量程
    CFG_HBPROTOCOL,           // 心跳协议ConfigHearUploadEnum
    CFG_SEN_RAPID_ACLR,       // ACC阈值   ACCTHR
    CFG_SEN_EMERGENCY_BRAKE,  // 急刹车阈值     BRAKETHR
    CFG_SEN_SLIGHT_COLLISION, // 轻微碰撞阈值 CRASHTHR_L         //= 20
    CFG_SEN_NORMAL_COLLISION, // 一般碰撞阈值 CRASHTHR_M
    CFG_SEN_SERIOUS_COLLISION,// 严重碰撞阈值 CRASHTHR_H
    CFG_SEN_SUDDEN_TURN,      // 急转弯阈值 TURNTHR
    CFG_SEN_STATIC,           // 静止阈值 STATICTHR
    CFG_SEN_RUN,              // 运动阈值 RUNTHR
    CFG_SEN_QUAKE_ALARM,      // 震动报警阈值 QUAKETHR
    CFG_SEN_LOWVOlTAGE_ALARM, // 低于最高电压百分多少算低电压（目前没用） LOWVOLPERT
    CFG_JT_HBINTERVAL,        // 部标位置数据心跳的发送间隔 替代CFG_HBTPOSTHR
    //为兼容的原因, 这些配置连在一起 END

    //中心号(28)
    CFG_CENTER_NUMBER,
    
    //以下四个number的id要连在一起
    CFG_USER1_NUMBER,    // = 30
    CFG_USER2_NUMBER,
    CFG_USER3_NUMBER,
    CFG_USER4_NUMBER,

    //1 agps  0 epo  由于有CFG_GPS_TYPE,该配置已无用
    CFG_OPEN_APGS, 
    
	//GPS关闭:0——不关闭,1——关闭
	CFG_GPS_CLOSE,

	//GPS模块型号[TYPE_INT]:0——未知；1——内置GPS模块（串口速率115200）；2——泰斗（串口速率9600）；3-中科微（串口速率9600）,added by bruce
	CFG_GPS_TYPE,

    //GPS定位检测时间,默认5秒
    CFG_GPS_UPDATE_TIME,

    CFG_WAIT_GPS_TIME,

    CFG_LBS_INTERVAL,

    CFG_HEART_INTERVAL,      // = 40

	//最小SNR
    CFG_MIN_SNR,

	//安装方向显示时间, 默认10分钟, 0xFF一直显示
    CFG_SEN_ANGLE_SHOW_TIME, 

	//拐点角度阈值[TYPE_INT],默认20度
	CFG_TURN_ANGLE,

    //在多少秒内[TYPE_BYTE],默认10秒
	CFG_AWAKE_CHECK_TIME, 

	//接上,震动几次开始唤醒[TYPE_BYTE],默认3次
    CFG_AWAKE_COUNT, 
	
	//传感器中断灵敏度[TYPE_INT],1-127,    1最灵敏,默认2
	CFG_SHAKE_LEVEL,

	//G-SENSOR震动阈值(mg)[TYPE_INT],默认40mg
	CFG_SHAKE_THRESHOLD,
	
    //震动报警判断次数,默认5次
    CFG_SHAKE_COUNT,   
    
    //震动检测间隔时间,默认10秒
    CFG_SHAKE_CHECK_TIME,   

    //自动设防模式下震动报警延时,默认120秒
    CFG_SHAKE_ALARM_DELAY,          // = 50

    //震动报警间隔(单位分钟),默认30分钟
    CFG_SHAKE_ALARM_INTERVAL,

	//ACC OFF后多久设防,默认300秒
    CFG_AUTO_DEFENCE_DELAY, 

    //静止上传检测时间默认20秒
    CFG_STATIC_CHECK_TIME,   
    
    //静止上传检测最小速度默认6km
    CFG_STATIC_CHECK_SPEED,    
    
    //静止上传检测次数默认3次
    CFG_STATIC_CHECK_COUNT,   

    // 上传时间间隔内,小速度漂移距离(56)
    CFG_DISTANCE_FOR_LOW_SPEED,    

    // 是否关闭防漂移, 0打开防漂移   ,1 关闭防漂移
    CFG_CLOSE_DRIFT_DETECT,    

    // 超速报警开关, 1打开   , 0 关闭
    CFG_SPEED_ALARM_ENABLE,   
    
    CFG_SPEED_CHECK_TIME,    

    //断电报警检测时间(默认5秒)
    CFG_POWER_CHECK_TIME,     // = 60
    
    //断电报警最小充电时间
    CFG_POWER_CHARGE_MIN_TIME,
    
    //断电报警ACC跳变检测时间
    CFG_POWER_ACC_CHECK_TIME,
    
    //断电报警方式 00: 只GPRS方式 01:SMS+GPRS  10: GPRS+SMS+CALL 11:GPRS+CALL 默认:01
    CFG_POWER_ALARM_MODE,
    CFG_SENSOR_ALARM_MODE, // g_sensor 报警方式 00: 只GPRS方式 01:SMS+GPRS  10: GPRS+SMS+CALL 11:GPRS+CALL 默认:01

    //打开GPS多久后AGPS准备好要重新打开GPS
	CFG_REOPEN_GSP_TIME,
	
    //ACC错误检测时间最小 10分钟,最大24小时
    CFG_ACC_CHECK_TIME,
    
	//是否应用层管理电池
	CFG_APP_BATTERT_MGR,
	
    //0——发送所有sos；1——只发送第一个报警号码
    CFG_ALARM_SMS_ONE,

    //开机充电延迟时间(外部控制用):分钟;
    CFG_CHARGE_DELAY,

    /*配置服务器地址,xxx.xxx.xxx.xxx:port*/
    CFG_CFGSERVERADDR,     //=70
    
    /*测试服务器地址,xxx.xxx.xxx.xxx:port*/
    CFG_TEST_SERVERADDR,
    
    /*AGPS服务器地址,xxx.xxx.xxx.xxx:port*/
    CFG_AGPSSERVERADDR,
    
    /*日志服务器地址,xxx.xxx.xxx.xxx:port*/
    CFG_LOGSERVERADDR,
    
    /*更新服务器地址,xxx.xxx.xxx.xxx:port*/
    CFG_UPDATESERVERADDR,
    
    /*文件服务器地址,xxx.xxx.xxx.xxx:port*/
    CFG_UPDATEFILESERVER,
    

    CFG_MAP_URL,       // 地图url
    CFG_PROTOCOL_VER,  // APP协议号
    CFG_SIM,           // sim号
    CFG_APN_CHECK,     // 自适应APN开关
    CFG_APN_NAME,   //80
    CFG_APN_USER,
    CFG_APN_PWD,
	
	//语言    
	// concox 语言  无报警中文:0x00 0x01    无报警英文:0x00 0x02    
	// goome 01 中文  00 英文
	CFG_LANGUAGE,
	
    //时区[TYPE_BYTE]
    CFG_TIME_ZONE,

    //0 自动设防模式,1 手动设防模式,默认自动设防
    CFG_IS_MANUAL_DEFENCE,
    //0 关闭震动报警,1 打开震动报警,默认关闭
	CFG_IS_SHAKE_ALARM_ENABLE,
    //休眠时是否把串口设置为IO口状态 0: 不设置  1:设置 默认 0
    CFG_IS_UART_IO_WHEN_SLEEP,
    //串口2波特率 0:115200 1:9600
	CFG_IS_UART_9600,              //90
    //是否平滑处理轨迹
	CFG_SMOOTH_TRACK,
	//是否支持90V电源,added by bruce
	CFG_IS_90V_POWER,
    //是否支持断油路功能 0:不支持,  1:支持
	CFG_IS_RELAY_ENABLE,
	//基站上传开关默认关闭  
    CFG_IS_LBS_ENABLE,
	// 静止上传定位点开关 默认关闭  
    CFG_IS_STATIC_UPLOAD,
    //是否开启车辆移动报警
    CFG_IS_MOVEALARM_ENABLE,
    //是否开启加速度报警(急加速、急减速、急转弯报警)
    CFG_IS_ACLRALARM_ENABLE,
    //是否开启BMS放电MOS开关
    CFG_IS_BMS_MOS_CUT_OFF,
    
    
    CFG_CUSTOM_CODE,      //客户代码
    CFG_TERM_MODEL,       //终端型号ID
    CFG_TERM_VERSION,     //终端当前版本号     //100
    CFG_TERM_BOOT_CHECK,  //终端当前版本BOOT校验码

    //部标 是否已注册,只在设备开电后注册一次,改设备号则要重新注册
    CFG_JT_ISREGISTERED,
    // 部标协议类型
    CFG_JT_AT_TYPE, 
    // 终端ID
    CFG_JT_DEVICE_ID,
    // 车牌号码
    CFG_JT_VEHICLE_NUMBER,
    // 车牌颜色
    CFG_JT_VEHICLE_COLOR,
    // 厂商ID
    CFG_JT_OEM_ID,
    // 省域ID
    CFG_JT_PROVINCE,
    // 市域ID
    CFG_JT_CITY,
    //验证码
    CFG_JT_AUTH_CODE, // 110

    //当前是否测试模式
    CFG_IS_TEST_MODE,

    CFG_PARAM_MAX
}ConfigParamEnum;

typedef enum
{
    TYPE_NONE = 0x00,
    TYPE_INT = 0x01,
    TYPE_SHORT = 0x02,
    TYPE_STRING = 0x03,
    TYPE_BOOL = 0X04,
    TYPE_FLOAT = 0X05,
    TYPE_BYTE = 0x06,
    TYPE_MAX
}ConfigParamDataType;

typedef enum
{
    CFG_CMD_REQ = 0x13,
    CFG_CMD_ACK = 0x92,
    CFG_CMD_RESULT = 0x14,
    CFG_CMD_MAX
}ConfigCmdEnum;

typedef enum
{
    CFG_CMD_REQ_ALL = 0x00,
    CFG_CMD_REQ_ONCE = 0x01,
    CFG_CMD_REQ_PERMANENT= 0x02,
}ConfigCmdReqType;

typedef enum
{
    HEART_SMART = 0x00,  //0x03 or 0x07
    HEART_NORMAL = 0x01,  //0x03
    HEART_EXPAND = 0x02,  //0x07
    HEART_MAX
}ConfigHearUploadEnum;


typedef struct
{
    ConfigParamDataType type;
    union
    {
        u8*  str;
        u32  i;
        u16  s;
        u8   b;
        float f;
    }data;
    u16 len;
} ConfigParamItems;


typedef enum
{
    PROTOCOL_NONE = 0x00,
    PROTOCOL_GOOME = 0x01,
    PROTOCOL_CONCOX = 0x02,
    PROTOCOL_JT808 = 0x03,
    PROTOCOL_BSJ = 0x04,
    PROTOCOL_MAX
}ConfigProtocolEnum;


typedef enum
{
    PROTOCOL_VER_GT02 = 0x01,//康凯斯V3协议版本 GT02D
    PROTOCOL_VER_JT808 = 0x01,//标准808协议
    PROTOCOL_VER_GT06 = 0x02,//康凯斯GT06协议
    PROTOCOL_VER_CMCC_JT808 = 0x02, //四川移动808协议
    PROTOCOL_APP_MAX
}ConfigProtocolVer;


typedef enum
{
    DEVICE_NONE = 0,
    DEVICE_GS03A, //4线单sensor
    DEVICE_AS03A, //4线单sensor
    DEVICE_GS03B, //2线单sensor 无电池
    DEVICE_AS03B, //2线单sensor 无电池
    DEVICE_GS03F,//2线单sensor 有电池
    DEVICE_AS03F,//2线单sensor 有电池
    DEVICE_GS07A,//4线单sensor 90V
    DEVICE_AS07A,//4线单sensor 90V
    DEVICE_GS07B,//2线单sensor 有电池90V
    DEVICE_AS07B,//2线单sensor 有电池90V
    DEVICE_GS03I, //2线双sensor 无电池
    DEVICE_AS03I, //2线双sensor 无电池
    DEVICE_GS03H, //4线双sensor
    DEVICE_GS05A, //4线单sensor 90V
    DEVICE_GS05B, //2线单sensor 无电池 90V
    DEVICE_GS05F, //2线单sensor 电池90V
    DEVICE_GS05I, //2线双sensor 无电池
    DEVICE_GS05H, //4线双sensor
    DEVICE_GM06E,//同GS03A,客户定制
    DEVICE_GS10, //5V供电 单sensor 无电池 BMS

    DEVICE_MAX,
}ConfigDeviceTypeEnum;


//断电报警方式 00: 只GPRS方式 01:SMS+GPRS  10: GPRS+SMS+CALL 11:GPRS+CALL 默认:01
typedef enum
{
    PWRALM_GPRS = 0,
    PWRALM_GPRS_SMS = 1,
    PWRALM_GPRS_SMS_CALL = 2,
    PWRALM_GPRS_CALL = 3,
}PowerAlarmMode;

/**
 * Function:   创建config_service模块
 * Description:创建config_service模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   使用前必须调用,否则调用其它接口返回失败错误码
 */
GM_ERRCODE config_service_create(void);

/**
 * Function:   销毁config_service模块
 * Description:销毁config_service模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE config_service_destroy(void);

/**
 * Function:   config_service模块定时处理入口
 * Description:config_service模块定时处理入口
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   1秒钟调用1次
 */
GM_ERRCODE config_service_timer_proc(void);


/**
 * Function:   
 * Description: 配置socket连接正常
 * Input:	   无
 * Output:	   无
 * Return:	   无
 * Others:	   无
 */
void config_service_connection_ok(void);

/**
 * Function:   
 * Description: 配置socket连接失败
 * Input:	   无
 * Output:	   无
 * Return:	   无
 * Others:	   无
 */
void config_service_connection_failed(void);
void config_service_close_for_reconnect(void);
   


/**
 * Function:   
 * Description: 获取指定id对应的配置
 * Input:	   id:配置id    type:数据类型
 * Output:	   无
 * Return:	   数据长度
 * Others:	   该函数只有字符串类型的配置, 不知道长度里, 需要先调用以获知长度
 */
u16 config_service_get_length(ConfigParamEnum id, ConfigParamDataType type);

/**
 * Function:   
 * Description: 根据id与type 获取配置到buf中.
 * Input:	   id:配置id    type:数据类型      buf:输出,用来存储配置数据            len:buf长度
 * Output:	   无
 * Return:	   无
 * Others:	   无
 */
GM_ERRCODE config_service_get(ConfigParamEnum id, ConfigParamDataType type, void* buf, u16 len);

/**
 * Function:   
 * Description: 根据id与type 把 buf中的内容 设置到config_service模块
 * Input:	   id:配置id    type:数据类型      buf:用来存储配置数据            len:buf长度
 * Output:	   无
 * Return:	   无
 * Others:	   无
 */
void config_service_set(ConfigParamEnum id, ConfigParamDataType type, const void* buf, u16 len);

/**
 * Function:   
 * Description: 此函数可代替config_service_get, 特别是string类型, 会少掉字符串拷贝的时间
 * Input:	   id:配置id 
 * Output:	   无
 * Return:	   指向id对应配置的指针
 * Others:	   无
 */
void* config_service_get_pointer(ConfigParamEnum id);

    
/**
 * Function:   
 * Description: 获取当前协议配置
 * Input:	   无
 * Output:	   无
 * Return:	   当前协议
 * Others:	   无
 */
ConfigProtocolEnum config_service_get_app_protocol(void);

/**
 * Function:   
 * Description: 获取当前心跳协议配置
 * Input:	   无
 * Output:	   无
 * Return:	   心跳协议
 * Others:	   无
 */
ConfigHearUploadEnum config_service_get_heartbeat_protocol(void);

/**
 * Function:   
 * Description: 获取当前时区配置
 * Input:	   无
 * Output:	   无
 * Return:	   时区
 * Others:	   无
 */
S8 config_service_get_zone(void);


/**
 * Function:   
 * Description: 用何种心跳协议
 * Input:	   无
 * Output:	   无
 * Return:	   心跳协议
 * Others:	   无
 */
bool config_service_is_main_server_goome(void);



/**
 * Function:   
 * Description: 告警只发送给一个号码
 * Input:	   无
 * Output:	   无
 * Return:	   心跳协议
 * Others:	   无
 */
bool config_service_is_alarm_sms_to_one(void);

/**
 * Function:   
 * Description: 获取短信发送号码
 * Input:	   无
 * Output:	   无
 * Return:	   号码长度
 * Others:	   无
 */
u8 config_service_get_sos_number(u8 index, u8 *buf, u16 len);



/**
 * Function:   
 * Description: 是否启用agps, 如果不是, 表示启用epo
 * Input:	   无
 * Output:	   无
 * Return:	   1 apgs    0 epo
 * Others:	   无
 */
bool config_service_is_agps(void);



/**
 * Function:   
 * Description: 获取当前gps类型
 * Input:	   无
 * Output:	   无
 * Return:	   时区
 * Others:	   无
 */
GPSChipType config_service_get_gps_type(void);



/**
 * Function:   
 * Description: 从本地配置文件中获取配置信息
 * Input:	   configs列表指针
 * Output:	   configs列表指针
 * Return:	   GM_SUCCESS 成功, 其它失败
 * Others:	   无
 */
GM_ERRCODE config_service_read_from_local(void);

/**
 * Function:   
 * Description: 保存配置到本地文件中
 * Input:	   configs列表指针
 * Output:	   无
 * Return:	   GM_SUCCESS 成功, 其它失败
 * Others:	   无
 */
GM_ERRCODE config_service_save_to_local(void);


/**
 * Function:    设置设备型号
 * Description: 设置当前设备型号
 * Input:	    configs列表指针
 * Output:	    configs列表指针
 * Return:	    GM_SUCCESS 成功, 其它失败
 * Others:	    无
 */
void config_service_set_device(ConfigDeviceTypeEnum type);


/**
 * Function:   根据ID获取设备型号
 * Description:获取设备型号(字符串)
 * Input:	   设备型号
 * Output:	   
 * Return:	   设备型号(字符串)
 * Others:	   无
 */
const char * config_service_get_device_type(u16 index);

/**
 * Function:   
 * Description: 获取设备型号ID
 * Input:	    设备型号(字符串)
 * Output:	   
 * Return:	    设备型号ID
 * Others:	    无
 */
ConfigDeviceTypeEnum config_service_get_device_type_id(const char* dev_type);

/**
 * Function:   
 * Description: 恢复出厂设置
 * Input:	   configs列表指针     is_all(是不是包括ip地址设置)
 * Output:	   configs列表指针
 * Return:	   GM_SUCCESS 成功, 其它失败
 * Others:	   无
 */
void config_service_restore_factory_config(bool is_all);

/**
 * Function:   
 * Description: 设置测试模式
 * Input:	   state:true——进入测试模式;false——退出测试模式
 * Output:	   无
 * Return:	   无
 * Others:	   无
 */
void config_service_set_test_mode(bool state);


/**
 * Function:   
 * Description: 判断当前是不是测试模式
 * Input:	   无
 * Output:	   无
 * Return:	   true 测试模式 , false 非测试模式
 * Others:	   无
 */
bool config_service_is_test_mode(void);

/**
 * Function:   
 * Description: 判断当前的imei是否写死在代码中的默认imei
 * Input:	   无
 * Output:	   无
 * Return:	   true GOOME_IMEI_DEFAULT , false 非 GOOME_IMEI_DEFAULT
 * Others:	   无
 */
bool config_service_is_default_imei(void);

/**
 * Function:   
 * Description: 设置设备的sim号
 * Input:	   sim
 * Output:	   GM_SUCCESS 成功, 其它失败
 * Return:	   无
 * Others:	   无
 */
GM_ERRCODE config_service_set_device_sim(u8 *pdata);


/**
 * Function:   
 * Description: 改变ip地址
 * Input:	   idx:配置id,其值是(xxx.xxx.xxx.xxx:port)形式;   buf: dns  ;len: dns长度
 * Output:	   无
 * Return:	   无
 * Others:	   无
 */
void config_service_change_ip(ConfigParamEnum idx, u8 *buf, u16 len);

/**
 * Function:   
 * Description: 改变port
 * Input:	   idx:配置id,其值是(xxx.xxx.xxx.xxx:port)形式;   port:端 口
 * Output:	   无
 * Return:	   无
 * Others:	   无
 */
void config_service_change_port(ConfigParamEnum idx, u16 port);


/*
    
*/
/**
 * Function:   
 * Description: 升级使用的socket类型
 * Input:      无
 * Output:     无
 * Return:     升级使用的socket类型
 * Others:     无
 */
StreamType config_service_update_socket_type(void);

StreamType config_service_agps_socket_type(void);

#endif


