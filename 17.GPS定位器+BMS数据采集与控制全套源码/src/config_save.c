#include "gm_type.h"
#include "stdio.h"
#include "gm_stdlib.h"
#include "gm_gprs.h"
#include "config_save.h"
#include "config_service.h"
#include "log_service.h"
#include "utility.h"
#include "gm_memory.h"
#include "gm_fs.h"
#include "g_sensor.h"
#include "system_state.h"
#include "gm_system.h"
#include "g_sensor.h"
#include "gsm.h"
#include "applied_math.h"

#define goome_para_main L"Z:\\goome\\GmParaMain\0"
#define goome_para_minor L"Z:\\goome\\GmParmMinor\0"
#define MAGIC_NUMBER 0xABCDDCBA

enum
{
    UOP_FAIL = 0,  // 错误
    UOP_OK,         // 正确
    
    UOP_CMD_ERR,  // 命令错误
    UOP_LEN_ERR,   // 长度错误
    UOP_PARA_ERR, // 参数错误
    UOP_CSM_ERR,  // 校验值错误
    UOP_CSM_OK,
    UOP_EXIST,  // 7
    
    UOP_MAGIC_ERR,
    UOP_MAGIC_OK,
    UOP_STATUS_1,
    UOP_STATUS_2,
    UOP_STATUS_3,
    UOP_STATUS_4,
    UOP_STATUS_5,
    UOP_STATUS_6,
    UOP_STATUS_7,
    UOP_STATUS_8,
    UOP_STATUS_9,
    UOP_STATUS_10,
    
    UOP_NONE
};

typedef struct
{
   u8 IP[4];
   u16 PORT;
} UserTypeIPV4; 

typedef struct
{
   u8 dns[GOOME_DNS_MAX_LENTH];
   u16 port;
} UserTypeDns; 

typedef enum
{
    TCP_IP = 0,
    UDP_IP,
    DNS_IP,
    IP_MAX
}UserIPTypeEnum;

typedef struct
{
    u8 sock_type;
    UserTypeIPV4 sock_ip[IP_MAX];
    UserTypeDns  src_dns;
}SocketParamStruct;


typedef struct
{
    u8 OEM_id[5];  //  
    u16 province_id;     // 省域ID
    u16 city_id;     // 市域ID
    u8 vehicle_number[12];  // 机动车号牌
    u8 vehicle_color;   // 车牌颜色
    u8 AT_protocol;       // 部标协议类型(标准 补充)
    u8 device_id[7];     // 终端ID
}Jt808LoginStruct;



// 关键参数结构体
//#pragma pack(1)
typedef struct
{
    u32 len;
    u32 crc;
    u32 magic;

    u8 is_valid; // UOP_OK:正常  UOP_PARA_ERR:参数错误
    u16 para_none;
    u16 para_err;
    u16 para_open_err;
    u16 para_crc_err;
    u16 para_mg_err;
    
    u16 para_from_mem;
    u16 para_from_file;
    u16 para_from_stm8;
    u16 para_from_server;
    u16 para_write_total;
    u16 para_res1;
    u16 para_res2;
    u16 para_res3;
    u16 para_res4;

    u16 para2_none;
    u16 para2_err;
    u16 para2_open_err;
    u16 para2_crc_err;
    u16 para2_mg_err;
    
    u16 para2_from_mem;
    u16 para2_from_file;
    u16 para2_write_total;
    u16 para2_from_stm8;
    u16 para2_from_server;
    u16 para2_res1;
    u16 para2_res2;
    u16 para2_res3;
    u16 device_type;

    SocketParamStruct    sock[6];
    u8 Apn[GOOME_APN_MAX_LENGTH];
    u8 para_user[GOOME_APN_MAX_LENGTH];
    u8 para_pwd[GOOME_APN_MAX_LENGTH];   
    u8 para_sim[17];
    u8 reserved_u8_1[13];
    u32 reserved_u32_1[10];
    
    u16 reserved_u16_1;  //RESERVED
    u16 jt_location_heart_time;  //用于判断部标位置数据心跳的发送间隔
    u16 upload_time;    //  定时判断时间
    u16 heart_interval;
    u32 reserved_u32_2;
    
    u16 reserved_u16_2;
    u16 distance_of_slow;  // 上传时间间隔内,小速度漂移距离
    u8  gps_update_gap;  //GPS定位检测时间,默认5秒
    u8  heart_protocol;  //0 自动检测模式   1 谷米正常心跳(0x03)  2 谷米扩展心跳(0x07)
    
    u8  local_time_hour;
    u8  reserved_u8_2;

    u8  speed_switch;  //默认关
    u8  speed_threshold;   // 超速阈?
    u8  speed_detect_time;   // 超速检测时?
    u8  reserved_u8_3;
    u8  reserved_u8_4;
    /*
    bit0   0 自动设防模式,1 手动设防模式,默认自动设防
    bit1   0:关闭震动报警,1 开启震动报警,默认关闭
    bit3-bit2   震动报警方式 00: 只GPRS方式 01:SMS+GPRS  10: GPRS+SMS+CALL 11:GPRS+CALL 默认:00
    bit4 号码设置权限  
            0: a、任意号码可设置SOS号码；
                b、中心号码只有SOS号码可设置
            1: a、当中心号码为空时,任意号码可设置；
                        当中心号码已设置,只有中心号码发指令本身可修改；
                b、只有?行暮怕耄ɑ騍OS号码）可设置SOS号码

    bit5  0-1；参数设置查询权限范围；默认值:0
    bit6  0-1 报警号码权限； 默认值:0   
    bit7  电池低压报警开关,默认0,打开,1:关闭
    bit8  电池低压报警方式,0:gprs   1: gprs+sms  默认0
    bit9  断电报警开关,0打开  1关闭  默认0
    bit11-bit10   断电报警方式 00: 只GPRS方式 01:SMS+GPRS  10: GPRS+SMS+CALL 11:GPRS+CALL 默认:01
    bit12 CONCOX协议类型,1新协议  0旧协议   //由protocol_type替代，不再使用
    bit13 静止上传定位点开关  0:开启  1:关闭
    bit14 基站上传开关默认关闭  0:关闭 1:开启
    bit15 自适应APN开关  0:打开  1:关闭 默认打开
    bit16 拐点补传开关  0:打开  1:关闭 默认打开
    bit17 亮灯模式,0:亮灯模式  1:灭灯模式  默认0
    bit18 后台日志上传 0:上传 1:不上传 默认 0
    bit19 电池充电模式  0 底层充电控制  1 APP充电控制  默认0
    bit20 启用EPO/AGPS  0:启用EPO  1:启用AGPS
    bit21 支持电压范围 0: 9-36V   1:9-90V  默认 0
    bit22 休眠时是否把串口设置为IO口状态 0: 不设置  1:设置 默认 0
    bit23 是否平滑轨迹
    bit24 是否上传电量 1 :不上传  0:上传 默认0
    bit25 是否有GPS模块 0:有GPS模块,1:无GPS模块
    bit26 串口2波特率 0:115200 1:9600
    bit27 是否支持断油路功能 0:不支持  1:支持
    bit28 是否上传sensor状态日志 0:不上传 1:上传
    bit29 是否开启车辆移动报警 0:关闭 1:打开
    bit30 是否开启加速度报警(急加速、急减速、急转弯报警) 0:关闭 1:打开
    */
    u32 param_bits;
    
    u8  reserved_u8_5;     // 休眠模式
    u8  center_number[20];    // 中心号码
    u8  user_number_1[16];  // 管理号码1
    u8  user_number_2[16];  // 管理号码2
    u8  user_number_3[16];  // 管理号码3
    u8  user_number_4[16];  // 管理号码4
    u8  reserved_u8_6[40];
    
    u8  reserved_u8_7;
    u8  reserved_u8_8;
    u8  reserved_u8_9;
    u8  reserved_u8_10;
    u16 power_alarm_check_tim;  //断电报警检测时间
    u16 power_alarm_chr_tim;  //断电报警最小充电时间
    
    u16 turn_angle; // 拐点补传角度  wFlexInitAng
    u8 gps_type;
    u16 power_alarm_acc_tim;  //断电报警ACC跳变检测时间
    u16 reserved_u16_3;
    
    u16  shake_threshold;   // G-SENSOR 震动值(mg) 震动阈值  wShakeVal
    u8 reserved_u8_11;
    Jt808LoginStruct login;
    u8 shock_alarm_count;   //震动报警判断次数
    u8 map_url[51];
    u8 reserved_u8_12[9];
    
    u8 led_ctrl_mode; // 0xA5:灭灯模式LED_MOD_ALL_OFF  其它:正常灯模式 LED_MOD_NORMAL
    u16 language;  // 语言  无报警中文:0x00 0x01    无报警英文:0x00 0x02
    u8 angle_view_time;  //安装方向显示时间 默认10分钟 0xFF一直显示
    u8 reserved_u8_13;

    u8 shake_level;
    u8 shake_time;
    u8 shake_cnt;
    u8 sf_switch;  //是否关闭防漂移, 0x00打开   ,0x55 关闭
    u8 shake_durn_tim;
    u16 sleep_tim;   // 睡眠时间 min
    u8 shake_interval;  //震动检测间隔时间

     //低四位应用协议版本 0 :未知,1:V1.0, 2:V1.1  
     //高四位应用协议类型 0 :未知,1: goome  2 : concox  3: 808   4 : bsj
    u8 protocol_type; 
    u8 jt_registered;//注册标志掉电保存
    u8 volt_grade;  //外部电压等级12V-24V-36V-48V-60V-72V-84V-96V
    u8 chr_delay_tim; // 开机充电延迟时间(外部控制用):分钟;
    u8 reopen_gps_time;  // 打开GPS多久后AGPS准备好要重新打开GPS
    u8 ip_lock_mode;        // 是否锁IP  Dec-11-2018 0:不锁 1:锁汽车在线 2:锁万物在线 3:锁当前设置
    VehicleState init_state;
    u8 min_snr;       // MTK GPS定位的最小信噪比参数
    
    u16 shake_alarm_timeout;  //自动设防模式下震动报警延时,默认值为:180秒
    u16 shake_alarm_interval;  //震动报警间隔,默认30分钟
    u16 defende_tim;  //自动设防延时时间,默认10分钟(600s)
    u16 reserved_u16_4;
    u16 static_check_interval;  //静止上传检测时间默认20秒
    u8  static_check_speed;  //静止上传检测最小速度默认6km
    u8  static_check_count;  //静止上传检测次数默认3次
    u16 lbs_interval;
    u16 gps_not_fix_time;

    u16 acc_check_interval;  //ACC错误检测时间最小 10分钟,最大24小时
    u16 reserved_u16_5;
    u16 reserved_u16_6;
    u16 reserved_u16_7;
    u16 reserved_u16_8;
    u16 reserved_u16_9;
    u16 reserved_u16_10;
    u16 reserved_u16_11;

    Vector3D reserved_3d_1;
    Vector3D reserved_3d_2;
    float sensor_threshold[THRESHOLD_ARRAY_LEN];
    
    u8  jt_authority_code[100];//鉴权码最大100字节
    u8 reserved_buff[156];

}GprsParaFileType, *T_GprsParam;


static T_GprsParam           p_param_mem = NULL;
static p_nvram_ef_goome_param_struct s_nvram = NULL;

static GprsParaFileType * s_Para = NULL;
static GprsParaFileType * s_Para2 = NULL;




/*
由于要兼容老配置, 所以直接使用了原来的函数与结构,
这里提供的对外接口,则主要是做结构转换.
*/

static void convert_cfg_to_para(GprsParaFileType *para);
static void convert_para_to_cfg(const GprsParaFileType *para);

static u32 goome_file_param_main_write(void);
static u32 goome_file_param_main_read(void);
static u32 goome_file_write_minor_param(void);
static u32 goome_file_read_minor_param(void);
static s32 goome_delete_file(u8 *file);
static u8 param_write_back_free_mem(void);
static u8 param_read_back_free_mem(void);
static void config_service_set_deault(void);
static bool check_para_ok(const GprsParaFileType *para);
static bool check_address_para_ok(u8 * param);
static void goome_file_nvram_save(void);





/************************************************************************
    Function :
    Description :
    Parameter : 
    Return : 
    Author: 
    Date:  Aug-29-2018
************************************************************************/
static u32 goome_file_param_main_write(void)
{
    u32 fs_len;
    int handle, ret;

    handle = GM_FS_Open(goome_para_main, GM_FS_READ_WRITE | GM_FS_ATTR_ARCHIVE | GM_FS_CREATE);
    if (handle < 0)
    {
        LOG(INFO,"goome_file_param_main_write param main file: open fail [%d]", handle);
        return 0;
    }
    
    s_Para->magic = MAGIC_NUMBER;
    s_Para->is_valid = UOP_OK;
    
    s_Para->para_write_total++;
    s_Para2->para_write_total++;
    
    s_Para->len = sizeof(GprsParaFileType);
    s_Para->crc = applied_math_calc_common_crc16((u8*)&s_Para->magic, sizeof(GprsParaFileType)-8);  // 968
    
    
    ret = GM_FS_Write(handle, (void *)s_Para, sizeof(GprsParaFileType), &fs_len);
    if (ret < 0)
    {
        LOG(INFO,"goome_file_param_main_write param main file: write fail [%d]", ret);
        
        GM_FS_Close(handle);
        return 0;
    }
    
    GM_FS_Close(handle);
    
    LOG(INFO,"goome_file_param_main_write param main file, len:%d", fs_len);
    
    
    return 1;
}


/************************************************************************
    Function :
    Description :
    Parameter : 
    Return : 
    Author:  
    Date: Aug-29-2018
************************************************************************/
static u32 goome_file_param_main_read(void)
{
    u32 fs_len, crc;
    int handle, ret;
    
    handle = GM_FS_Open(goome_para_main, GM_FS_READ_ONLY | GM_FS_ATTR_ARCHIVE | GM_FS_CREATE);
    if (handle < 0)
    {
        LOG(INFO,"goome_file_param_main_read param main read: open fail [%d]", handle);
        s_Para->para_open_err++;
        return UOP_FAIL;
    }
    
    
    ret = GM_FS_Read(handle, (void *)s_Para, sizeof(GprsParaFileType), &fs_len);
    if (ret < 0 || fs_len != sizeof(GprsParaFileType))
    {
        LOG(INFO,"goome_file_param_main_read param main read: read fail len:%d ret:%d.",fs_len, ret);
        s_Para->para_open_err++;
        GM_FS_Close(handle);
        return UOP_FAIL;
    }
    
    GM_FS_Close(handle);
    
    crc = applied_math_calc_common_crc16((u8*)&s_Para->magic, sizeof(GprsParaFileType)-8);
    
    if (GM_strlen((const char* )s_Para->sock[SOCKET_INDEX_MAIN].src_dns.dns) >= sizeof(s_Para->sock[SOCKET_INDEX_MAIN].src_dns.dns)-1)
    {        
        GM_memset(&s_Para->sock[SOCKET_INDEX_MAIN].src_dns.dns, 0x00, sizeof(s_Para->sock[SOCKET_INDEX_MAIN].src_dns.dns));
    }
    
    if (s_Para->crc != crc)
    {
        LOG(INFO,"goome_file_param_main_read param main crc err:%x, %x ", s_Para->crc, crc);
        s_Para->para_crc_err++;
        
        return UOP_CSM_ERR;
    }
    
    if (s_Para->magic != MAGIC_NUMBER)
    {
        LOG(INFO,"goome_file_param_main_read param main magic err:%x, ", s_Para->magic);
        s_Para->para_mg_err++;
        return UOP_MAGIC_ERR;
    }

    return UOP_OK;;
}






/************************************************************************
    Function :
    Description :
    Parameter : 
    Return : 
    Author: 
    Date:  Sep-18-2018
************************************************************************/
static u32 goome_file_write_minor_param(void)
{
    u32 fs_len;
    int handle, ret;
        
    handle = GM_FS_Open(goome_para_minor, GM_FS_READ_WRITE | GM_FS_ATTR_ARCHIVE | GM_FS_CREATE);
    if (handle < 0)
    {
        LOG(INFO,"goome_file_write_minor_param open minor file fail [%d] \r\n", handle);
        return 0;
    }
    
    s_Para2->magic = MAGIC_NUMBER;
    s_Para2->is_valid = UOP_OK;
    
    s_Para->para2_write_total++;
    s_Para2->para2_write_total++;
    
    s_Para2->len = sizeof(GprsParaFileType);
    s_Para2->crc = applied_math_calc_common_crc16((u8*)&s_Para2->magic, sizeof(GprsParaFileType)-8);  // 968
    
    
    ret = GM_FS_Write(handle, (void *)s_Para2, sizeof(GprsParaFileType), &fs_len);
    if (ret < 0)
    {
        LOG(INFO,"goome_file_write_minor_param write minor file fail [%d] \r\n", ret);
        
        GM_FS_Close(handle);
        return 0;
    }
    
    GM_FS_Close(handle);
        
    return 1;
}




/************************************************************************
    Function :
    Description :
    Parameter : 
    Return : 
    Author:  
    Date: Sep-18-2018
************************************************************************/
static u32 goome_file_read_minor_param(void)
{
    u32 fs_len, crc;
    int handle, ret;
    
    handle = GM_FS_Open(goome_para_minor, GM_FS_READ_ONLY | GM_FS_ATTR_ARCHIVE | GM_FS_CREATE);
    if (handle < 0)
    {
        LOG(INFO,"goome_file_read_minor_param open minor file read fail [%d] \r\n", handle);
        s_Para->para2_open_err++;
        s_Para2->para2_open_err++;
        
        return UOP_FAIL;
    }
    
    
    ret = GM_FS_Read(handle, (void *)s_Para2, sizeof(GprsParaFileType), &fs_len);
    if (ret < 0)
    {
        LOG(INFO,"goome_file_read_minor_param read minor file read fail [%d] \r\n", ret);
        s_Para->para2_open_err++;
        s_Para2->para2_open_err++;
        
        GM_FS_Close(handle);
        return UOP_FAIL;
    }
    
    GM_FS_Close(handle);
    
    crc = applied_math_calc_common_crc16((u8*)&s_Para2->magic, sizeof(GprsParaFileType)-8); // -18
    
    
    if (GM_strlen((const char* )s_Para2->sock[SOCKET_INDEX_MAIN].src_dns.dns) >= sizeof(s_Para2->sock[SOCKET_INDEX_MAIN].src_dns.dns)-1)
    {       
        GM_memset(s_Para2->sock[SOCKET_INDEX_MAIN].src_dns.dns, 0x00, sizeof(s_Para2->sock[SOCKET_INDEX_MAIN].src_dns.dns));
    }
    
    if (s_Para2->crc != crc)
    {
        LOG(INFO,"goome_file_read_minor_param read minor file read crc err:%x, %x ", s_Para2->crc, crc);
        s_Para->para2_crc_err++;
        s_Para2->para2_crc_err++;
        
        return UOP_CSM_ERR;
    }
    
    if (s_Para2->magic != MAGIC_NUMBER)
    {
        LOG(INFO,"goome_file_read_minor_param read minor file read magic err:%x, ", s_Para2->magic);
        s_Para->para2_mg_err++;
        s_Para2->para2_mg_err++;
        
        return UOP_MAGIC_ERR;
    }
        
    return UOP_OK;
}




/************************************************************************
    Function :
    Description : 
    Parameter : 
    Return : 
    Author:  
    Date: May-12-2016
************************************************************************/
static s32 goome_delete_file(u8 *file)
{
    s32  ret=0;
    s32  handle; 
    u8 buff[100];
    
    
    // 检查文件是否存在
    handle = GM_FS_CheckFile((u16 *)file);
    if (handle >= 0)
    {
        // 文件存在删掉,重新创建
        ret = GM_FS_Delete((u16 *)file);
        
        #if 1
        util_ucs2_to_ascii((u8*)file, buff, 160);
        
        LOG(INFO,"goome_delete_file deletfile:%.66s,ret:%d \r\n", buff,ret);
        #endif
    }
    
    
    return ret;
}


/************************************************************************
    Function :
    Description :  
    Parameter : 
    Return : 
    Author:  
    Date: Sep-18-2018
************************************************************************/
static u8 param_write_back_free_mem(void)
{
    s_Para->magic = MAGIC_NUMBER;
    s_Para->is_valid = UOP_OK;
    s_Para->len = sizeof(GprsParaFileType);
    s_Para->crc = applied_math_calc_common_crc16((u8*)&s_Para->magic, sizeof(GprsParaFileType)-8);  // 968

    GM_memcpy(p_param_mem, s_Para, sizeof(GprsParaFileType));
    
    LOG(INFO,"param_write_back_free_mem param_write_mem:%x %d", s_Para->crc, sizeof(GprsParaFileType)-8);
    
    return UOP_OK;
}

/************************************************************************
    Function :
    Description :  
    Parameter : 
    Return : 
    Author:  
    Date: Sep-18-2018
************************************************************************/
static u8 param_read_back_free_mem(void)
{
    u32 crc;
    
    if (p_param_mem->magic != MAGIC_NUMBER)
    {
        LOG(INFO,"param_read_back_free_mem goome param file free mem magic error:%x ", p_param_mem->magic);
        return UOP_MAGIC_ERR;
    }
    
    crc = applied_math_calc_common_crc16((u8*)&p_param_mem->magic, sizeof(GprsParaFileType)-8); // -18
    if (crc != p_param_mem->crc)
    {
        LOG(INFO,"param_read_back_free_mem goome param file free mem Crc error:%x,%x %d", crc, p_param_mem->crc, sizeof(GprsParaFileType)-8);
        return UOP_CSM_ERR;
    }
    
    return UOP_OK;
}




/************************************************************************
    Function :
    Description : 读取系统参数,FreeMem,主参数文件,备份参数文件
    Parameter : 
    Return : 
    Author: 
    Date: Oct-10-2018
************************************************************************/
u8 read_param_from_file(void)
{
    u8 file1=0, file2=0, free_mem=0;
    
    GM_memset(s_Para, 0x00, sizeof(GprsParaFileType));
    GM_memset(s_Para2, 0x00, sizeof(GprsParaFileType));
    
    file1=0;
    file2=0;
    
    if (GM_FS_CheckFile((const U16*)goome_para_main) < 0)
    {
        LOG(INFO,"read_param_from_file no find para_main_file");
        
        s_Para->para_none++;
        s_Para2->para_none++;
        file1 = UOP_FAIL;
    }
    else
    {
        file1 = goome_file_param_main_read();
    }
    
    
    if (GM_FS_CheckFile((const U16*)goome_para_minor) < 0)
    {
        LOG(INFO,"read_param_from_file no find para_minor_file");
        
        s_Para->para2_none++;
        s_Para2->para2_none++;
        file2 = UOP_FAIL;
    }
    else
    {
        file2 = goome_file_read_minor_param();
    }
    
    free_mem = param_read_back_free_mem();
    if (free_mem == UOP_OK)
    {        
        if (file1 != UOP_OK)
        {
            GM_memcpy(s_Para, p_param_mem, sizeof(GprsParaFileType));
            
            s_Para->para_from_mem++;
            s_Para2->para_from_mem++;
            
            goome_delete_file((u8 *)goome_para_main);
            goome_file_param_main_write();
        }
        
        if (file2 != UOP_OK)
        {
            GM_memcpy(s_Para2, p_param_mem, sizeof(GprsParaFileType));
            
            s_Para->para2_from_mem++;
            s_Para2->para2_from_mem++;
            
            goome_delete_file((u8 *)goome_para_minor);
            goome_file_write_minor_param();
        }
    }
    else
    {
        if(system_state_get_boot_reason(false) == GM_RREBOOT_UNKNOWN)
        {
    	    system_state_set_boot_reason(GM_REBOOT_POWER_ON);
        }

        if ((file1 == UOP_OK) && (file2 != UOP_OK)) 
        {            
            GM_memcpy(s_Para2, s_Para, sizeof(GprsParaFileType));
            
            s_Para->para2_from_file++;
            s_Para2->para2_from_file++;
            
            goome_delete_file((u8 *)goome_para_minor);
            goome_file_write_minor_param();
            param_write_back_free_mem();
        }
        else if ((file1 != UOP_OK) && (file2 == UOP_OK)) 
        {
            GM_memcpy(s_Para, s_Para2, sizeof(GprsParaFileType));
            
            s_Para->para_from_file++;
            s_Para2->para_from_file++;
            
            goome_delete_file((u8 *)goome_para_main);
            goome_file_param_main_write();
            param_write_back_free_mem();
        }
        else if ((file1 != UOP_OK) && (file2 != UOP_OK))
        {
            s_Para->is_valid = UOP_PARA_ERR;
        }
    }
    
    return 0;
}

static void goome_file_nvram_save(void)
{
    s32 lib_index = -1;
    s32 result = -1;

    if (NULL == s_nvram)
    {
        s_nvram = (p_nvram_ef_goome_param_struct )GM_MemoryAlloc(sizeof(nvram_ef_goome_param_struct));
        if (NULL == s_nvram)
        {
            LOG(ERROR,"goome_file_nvram_save assert(s_nvram) failed.");
            return;
        }
    }
    if (NULL == s_Para)
    {
        LOG(ERROR,"goome_file_nvram_save assert(s_Para) failed.");
        return;
    }
    
    s_nvram->DeviceType = s_Para->device_type;
    
    lib_index = GM_ReadNvramLid(NVRAM_EF_GM_CUSTOMER_PARAM_LID);
    GM_ReadWriteNvram(0, lib_index, 1, s_nvram, sizeof(nvram_ef_goome_param_struct), &result);

    //只分配一次,后面一直使用
    //GM_MemoryFree(s_nvram);
    //s_nvram = NULL;
}


static void read_nvram_device_type(void)
{
    s32 lib_index = -1;
    s32 result = -1;

    if (NULL == s_nvram)
    {
        s_nvram = (p_nvram_ef_goome_param_struct)GM_MemoryAlloc(sizeof(nvram_ef_goome_param_struct));
        if (NULL == s_nvram)
        {
            LOG(ERROR,"goome_file_nvram_save assert(s_nvram) failed.");
            return;
        }
    }
    if (NULL == s_Para)
    {
        LOG(ERROR,"goome_file_nvram_save assert(s_Para) failed.");
        return;
    }
    
    lib_index = GM_ReadNvramLid(NVRAM_EF_GM_CUSTOMER_PARAM_LID);
    GM_ReadWriteNvram(1, lib_index, 1, s_nvram, sizeof(nvram_ef_goome_param_struct), &result);
}



/************************************************************************
    Function :
    Description :
    Parameter : 
    Return : 
    Author: 
    Date:  Sep-18-2018
************************************************************************/
static u8 goome_file_param_save(void)
{
    param_write_back_free_mem();
    goome_file_param_main_write();
    
    GM_memcpy(s_Para2, s_Para, sizeof(GprsParaFileType));
    goome_file_write_minor_param();
    goome_file_nvram_save();
    
    return 1;
}

GM_ERRCODE config_service_save_to_local(void)
{
    bool allocate_here1 = false;
    bool allocate_here2 = false;
    
    if(NULL == s_Para)
    {
        s_Para = (GprsParaFileType *)GM_MemoryAlloc(sizeof(GprsParaFileType));
        if(!s_Para)
        {
            LOG(ERROR,"config_service_read_from_local assert(s_Para) failed.");
            return GM_MEM_NOT_ENOUGH;
        }
        allocate_here1 = true;
    }
    
    if(NULL == s_Para2)
    {
        s_Para2 = (GprsParaFileType *)GM_MemoryAlloc(sizeof(GprsParaFileType));
        if(!s_Para2)
        {
            LOG(ERROR,"config_service_read_from_local assert(s_Para) failed.");
            GM_MemoryFree(s_Para);
            s_Para = NULL;
            return GM_MEM_NOT_ENOUGH;
        }
        allocate_here2 = true;
    }

    convert_cfg_to_para(s_Para);
    goome_file_param_save();

    if(allocate_here1)
    {
        GM_MemoryFree(s_Para);
        s_Para = NULL;
    }
    
    if(allocate_here2)
    {
        GM_MemoryFree(s_Para2);
        s_Para2 = NULL;
    }
	return GM_SUCCESS;
}


GM_ERRCODE config_service_read_from_local(void)
{
    if(!p_param_mem)
    {
        p_param_mem = (T_GprsParam)(GM_ImageDummyBase()-2*1024);
    }
    
    if(NULL == s_Para)
    {
        s_Para = (GprsParaFileType *)GM_MemoryAlloc(sizeof(GprsParaFileType));
        if(!s_Para)
        {
            LOG(ERROR,"config_service_read_from_local assert(s_Para) failed.");
            return GM_MEM_NOT_ENOUGH;
        }
    }
    
    if(NULL == s_Para2)
    {
        s_Para2 = (GprsParaFileType *)GM_MemoryAlloc(sizeof(GprsParaFileType));
        if(!s_Para2)
        {
            LOG(ERROR,"config_service_read_from_local assert(s_Para) failed.");
            GM_MemoryFree(s_Para);
            s_Para = NULL;
            return GM_MEM_NOT_ENOUGH;
        }
    }
    
	//有些新参数在旧设备上没有,要先设置为默认参数,从文件读取后再覆盖
    config_service_set_deault();
    read_nvram_device_type();
    config_service_set_device((ConfigDeviceTypeEnum)s_nvram->DeviceType);

	read_param_from_file();
    if(UOP_PARA_ERR != s_Para->is_valid && check_para_ok(s_Para))
    {
        convert_para_to_cfg(s_Para);
		LOG(INFO,"convert_para_to_cfg");
    }
    else
    {
        config_service_save_to_local();
		LOG(INFO,"config_service_save_to_local");
    }


    GM_MemoryFree(s_Para);
    s_Para = NULL;
    GM_MemoryFree(s_Para2);
    s_Para2 = NULL;
    return GM_SUCCESS;
}


/*
设置出厂时的默认参数
不包括 sim, socket, heart_mode, apn, protocol配置.
*/
static void config_service_set_factory_deault(void)
{
    u8 value_u8 = 0;
	s8 value_s8 = 0;
    u16 value_u16 = 0;
    u32 value_u32 = 0;
    float value_float = 0;

	value_u8 = 1;
	config_service_set(CFG_SERVERLOCK, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u16 = GOOME_HEARTBEAT_DEFAULT;
    config_service_set(CFG_HEART_INTERVAL, TYPE_SHORT, &value_u16, sizeof(value_u16));

    value_u16 = GOOME_HEARTBEAT_DEFAULT;
    config_service_set(CFG_JT_HBINTERVAL, TYPE_SHORT, &value_u16, sizeof(value_u16));

    value_u16 = CONFIG_UPLOAD_TIME_DEFAULT;
    config_service_set(CFG_UPLOADTIME, TYPE_SHORT, &value_u16, sizeof(value_u16));
    
    // 两点间距判断
    value_u16 = 20;
    config_service_set(CFG_DISTANCE_FOR_LOW_SPEED, TYPE_SHORT, &value_u16, sizeof(value_u16));

	//防漂移功能默认打开
    value_u8 = false;   
    config_service_set(CFG_CLOSE_DRIFT_DETECT, TYPE_BOOL, &value_u8, sizeof(value_u8));


    value_u8 = false;
    config_service_set(CFG_SPEED_ALARM_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));

    value_u8 = 120;
    config_service_set(CFG_SPEEDTHR, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u8 = 15;
    config_service_set(CFG_SPEED_CHECK_TIME, TYPE_BYTE, &value_u8, sizeof(value_u8));


    value_s8 = 8;
	config_service_set(CFG_TIME_ZONE, TYPE_BYTE, &value_s8, sizeof(value_s8));

	value_u8 = GM_GPS_TYPE_UNKNOWN;
	config_service_set(CFG_GPS_TYPE, TYPE_BYTE, &value_u8, sizeof(value_u8));
	
    value_u8 = 5;
	config_service_set(CFG_GPS_UPDATE_TIME, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u16 = 20;
	config_service_set(CFG_TURN_ANGLE, TYPE_SHORT, &value_u16, sizeof(value_u16));

    value_u32 = 60;
    config_service_set(CFG_SHAKE_THRESHOLD, TYPE_INT, &value_u32, sizeof(value_u32));

    value_u8 = 2;
    config_service_set(CFG_SHAKE_LEVEL, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u8 = CONFIG_SHAKECOUT_DEFAULT;
    config_service_set(CFG_SHAKE_COUNT, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u8 = SHAKE_TIME_SECONDS;
    config_service_set(CFG_AWAKE_CHECK_TIME, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u8 = SHAKE_COUNT_TO_AWAKE;
    config_service_set(CFG_AWAKE_COUNT, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u8 = 10;
    config_service_set(CFG_SHAKE_CHECK_TIME, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u16 = GOOME_SLEEPTIME_DEFAULT;
    config_service_set(CFG_SLEEP_TIME, TYPE_SHORT, &value_u16, sizeof(value_u16));

	value_u8 = false;
    config_service_set(CFG_IS_MANUAL_DEFENCE, TYPE_BOOL, &value_u8, sizeof(value_u8));
    
    value_u8 = 0;
    config_service_set(CFG_IS_SHAKE_ALARM_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));

	value_u8 = false;
    config_service_set(CFG_IS_STATIC_UPLOAD, TYPE_BOOL, &value_u8, sizeof(value_u8));

	value_u8 = false;
    config_service_set(CFG_IS_MOVEALARM_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));

	value_u8 = true;
    config_service_set(CFG_SMOOTH_TRACK, TYPE_BOOL, &value_u8, sizeof(value_u8));

	value_u8 = false;
    config_service_set(CFG_IS_ACLRALARM_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));

	value_u8 = false;
	config_service_set(CFG_IS_BMS_MOS_CUT_OFF, TYPE_BOOL, &value_u8, sizeof(value_u8));
	
	value_u8 = false;
    config_service_set(CFG_IS_LBS_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));

    value_u16 = CONFIG_SHAKEALARMDELAY_DEFAULT;
    config_service_set(CFG_SHAKE_ALARM_DELAY, TYPE_SHORT, &value_u16, sizeof(value_u16));
    
    value_u16 = CONFIG_SHAKEALARMINTERVAL_DEFAULT;
    config_service_set(CFG_SHAKE_ALARM_INTERVAL, TYPE_SHORT, &value_u16, sizeof(value_u16));
    
    value_u16 = 120;
    config_service_set(CFG_AUTO_DEFENCE_DELAY, TYPE_SHORT, &value_u16, sizeof(value_u16));

    value_u16 = 0;
    config_service_set(CFG_CHARGE_DELAY, TYPE_SHORT, &value_u16, sizeof(value_u16));
    
    config_service_set(CFG_MAP_URL, TYPE_STRING, GOOME_MAPS_URL_DEFAULT, GM_strlen(GOOME_MAPS_URL_DEFAULT));

    value_u16 = 5;
    config_service_set(CFG_POWER_CHECK_TIME, TYPE_SHORT, &value_u16, sizeof(value_u16));

	value_u8 = 0;
	config_service_set(CFG_CUTOFFALM_DISABLE, TYPE_BYTE, &value_u8, sizeof(value_u8));
    
    value_u16 = 300;
    config_service_set(CFG_POWER_CHARGE_MIN_TIME, TYPE_SHORT, &value_u16, sizeof(value_u16));
    
    value_u16 = 0;
    config_service_set(CFG_POWER_ACC_CHECK_TIME, TYPE_SHORT, &value_u16, sizeof(value_u16));
    
    value_u8 = PWRALM_GPRS_SMS;
    config_service_set(CFG_POWER_ALARM_MODE, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u8 = PWRALM_GPRS;
    config_service_set(CFG_SENSOR_ALARM_MODE, TYPE_BYTE, &value_u8, sizeof(value_u8));


    value_u8 = (u8)GB_ATTYPE_STANDARD;
    config_service_set(CFG_JT_AT_TYPE, TYPE_BYTE, &value_u8, sizeof(value_u8));

    config_service_set(CFG_JT_DEVICE_ID, TYPE_STRING, JT_DEV_ID, GM_strlen(JT_DEV_ID));
    config_service_set(CFG_JT_VEHICLE_NUMBER, TYPE_STRING, JT_VEHICLE_NUMBER, GM_strlen(JT_VEHICLE_NUMBER));
    config_service_set(CFG_JT_OEM_ID, TYPE_STRING, JT_OEM_ID, GM_strlen(JT_OEM_ID));
    config_service_set(CFG_JT_AUTH_CODE, TYPE_STRING, JT_OEM_ID, GM_strlen(JT_OEM_ID));

    value_u16 = 44;
    config_service_set(CFG_JT_PROVINCE, TYPE_SHORT, &value_u16, sizeof(value_u16));
    
    value_u16 = 303;
    config_service_set(CFG_JT_CITY, TYPE_SHORT, &value_u16, sizeof(value_u16));

	value_u8 = 0;
    config_service_set(CFG_IS_TEST_MODE, TYPE_BOOL, &value_u8, sizeof(value_u8));

    value_u8 = (u8)0x01;
    config_service_set(CFG_JT_VEHICLE_COLOR, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u16 = 0x0001;
    config_service_set(CFG_LANGUAGE, TYPE_SHORT, &value_u16, sizeof(value_u16));

    value_u16 = 20;
    config_service_set(CFG_STATIC_CHECK_TIME, TYPE_SHORT, &value_u16, sizeof(value_u16));

    value_u8 = 6;
    config_service_set(CFG_STATIC_CHECK_SPEED, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u8 = 3;
    config_service_set(CFG_STATIC_CHECK_COUNT, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u16 = LBS_UPLOAD_DEFAULT;
    config_service_set(CFG_LBS_INTERVAL, TYPE_SHORT, &value_u16, sizeof(value_u16));

    value_u16 = GPS_NOT_FIXED_DEFAULT;
    config_service_set(CFG_WAIT_GPS_TIME, TYPE_SHORT, &value_u16, sizeof(value_u16));

    value_u8 = 9;
    config_service_set(CFG_REOPEN_GSP_TIME, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u16 = 720;
    config_service_set(CFG_ACC_CHECK_TIME, TYPE_SHORT, &value_u16, sizeof(value_u16));

    value_u8 = 16;
    config_service_set(CFG_MIN_SNR, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u8 = CONFIG_ANGLETIME_DEFAULT;
    config_service_set(CFG_SEN_ANGLE_SHOW_TIME, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_float = RAPID_ACLR_THRESHOLD;
	config_service_set(CFG_SEN_RAPID_ACLR, TYPE_FLOAT, &value_float, sizeof(value_float));

    value_float = EMERGENCY_BRAKE_THRESHOLD;
	config_service_set(CFG_SEN_EMERGENCY_BRAKE, TYPE_FLOAT, &value_float, sizeof(value_float));

    value_float = SLIGHT_COLLISION_MIN_ACLR;
	config_service_set(CFG_SEN_SLIGHT_COLLISION, TYPE_FLOAT, &value_float, sizeof(value_float));

    value_float = NORMAL_COLLISION_MIN_ACLR;
	config_service_set(CFG_SEN_NORMAL_COLLISION, TYPE_FLOAT, &value_float, sizeof(value_float));

    value_float = SERIOUS_COLLISION_MIN_ACLR;
	config_service_set(CFG_SEN_SERIOUS_COLLISION, TYPE_FLOAT, &value_float, sizeof(value_float));

    value_float = SUDDEN_TURN_THRESHOLD;
	config_service_set(CFG_SEN_SUDDEN_TURN, TYPE_FLOAT, &value_float, sizeof(value_float));

    value_float = LOW_VOLTAGE_ALARM_PERCENT;
	config_service_set(CFG_SEN_LOWVOlTAGE_ALARM, TYPE_FLOAT, &value_float, sizeof(value_float));

    value_float = QUAKE_ACLR_THRESHOLD;
	config_service_set(CFG_SEN_QUAKE_ALARM, TYPE_FLOAT, &value_float, sizeof(value_float));

    value_float = STATIC_ACLR_THRESHOLD;
	config_service_set(CFG_SEN_STATIC, TYPE_FLOAT, &value_float, sizeof(value_float));

    value_float = RUN_ACLR_THRESHOLD;
	config_service_set(CFG_SEN_RUN, TYPE_FLOAT, &value_float, sizeof(value_float));

    value_u8 = false;
    config_service_set(CFG_JT_ISREGISTERED, TYPE_BOOL, &value_u8, sizeof(value_u8));

    config_service_set(CFG_CENTER_NUMBER, TYPE_STRING, "", 0);
    config_service_set(CFG_USER1_NUMBER, TYPE_STRING, "", 0);
    config_service_set(CFG_USER2_NUMBER, TYPE_STRING, "", 0);
    config_service_set(CFG_USER3_NUMBER, TYPE_STRING, "", 0);
    config_service_set(CFG_USER4_NUMBER, TYPE_STRING, "", 0);

    value_u8 = true;
    config_service_set(CFG_ALARM_SMS_ONE, TYPE_BOOL, &value_u8, sizeof(value_u8));

    //upgrade preset parameters
    config_service_set(CFG_TERM_VERSION, TYPE_STRING, VERSION_NUMBER, GM_strlen(VERSION_NUMBER));
    LOG(INFO,"current version(%s).",VERSION_NUMBER);
    
    config_service_set(CFG_CUSTOM_CODE, TYPE_STRING,UPDATE_OEM_CODE, GM_strlen(UPDATE_OEM_CODE));
    config_service_set(CFG_TERM_MODEL, TYPE_STRING, UPDATE_DEVICE_CODE, GM_strlen(UPDATE_DEVICE_CODE));
    config_service_set(CFG_TERM_BOOT_CHECK, TYPE_STRING,UPDATE_BOOT_CODE, GM_strlen(UPDATE_BOOT_CODE));
}



/*
设置 默认参数
包括 sim, socket, heart_mode, apn, protocol配置.
*/
static void config_service_set_deault(void)
{
    u8 value_u8;
    //u16 value_u16;
    //u32 value_u32;
    //float value_float;

    config_service_set_factory_deault();
    
    config_service_set(CFG_SERVERADDR, TYPE_STRING, CONFIG_GOOCAR_SERVER_ADDERSS, GM_strlen(CONFIG_GOOCAR_SERVER_ADDERSS));
    config_service_set(CFG_CFGSERVERADDR, TYPE_STRING, CONFIG_SERVER_ADDERSS, GM_strlen(CONFIG_SERVER_ADDERSS));
    config_service_set(CFG_TEST_SERVERADDR, TYPE_STRING, CONFIG_FACTORY_SERVER_ADDERSS, GM_strlen(CONFIG_FACTORY_SERVER_ADDERSS));
    config_service_set(CFG_AGPSSERVERADDR, TYPE_STRING, CONFIG_AGPS_SERVER_ADDERSS, GM_strlen(CONFIG_AGPS_SERVER_ADDERSS));
    config_service_set(CFG_LOGSERVERADDR, TYPE_STRING, CONFIG_LOG_SERVER_ADDERSS, GM_strlen(CONFIG_LOG_SERVER_ADDERSS));
    config_service_set(CFG_UPDATESERVERADDR, TYPE_STRING, GOOME_UPDATE_SERVER_DNS, GM_strlen(GOOME_UPDATE_SERVER_DNS));
    config_service_set(CFG_UPDATEFILESERVER, TYPE_STRING, GOOME_UPDATE_SERVER_DNS, GM_strlen(GOOME_UPDATE_SERVER_DNS));

    
    config_service_set(CFG_APN_NAME, TYPE_STRING, APN_DEFAULT, GM_strlen((const char *)APN_DEFAULT));
    config_service_set(CFG_APN_USER, TYPE_STRING, APN_USER_DEFAULT, GM_strlen((const char *)APN_USER_DEFAULT));
    config_service_set(CFG_APN_PWD, TYPE_STRING, APN_USER_DEFAULT, GM_strlen((const char *)APN_USER_DEFAULT));


    value_u8 = (u8)PROTOCOL_GOOME;
    config_service_set(CFG_PROTOCOL, TYPE_BYTE, &value_u8, sizeof(value_u8));
    
    value_u8 = (u8)PROTOCOL_VER_GT02;
    config_service_set(CFG_PROTOCOL_VER, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u8 = (u8)HEART_SMART;
    config_service_set(CFG_HBPROTOCOL, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u8 = true;
    config_service_set(CFG_APN_CHECK, TYPE_BOOL, &value_u8, sizeof(value_u8));
}



/*
设置当前设备型号
并根据设备型号设置相关默认配置
*/
void config_service_set_device(ConfigDeviceTypeEnum type)
{
    u8 value_u8;
    u16 value_u16;

    value_u16 = (u16)type;
    if(value_u16 >= DEVICE_MAX || value_u16 <= DEVICE_NONE)
    {
        LOG(ERROR,"config_service_set_device assert(type(%d) failed.",value_u16);

        //先给个型号
		value_u16 = DEVICE_GS03A;
    }
    
    config_service_set(CFG_DEVICETYPE, TYPE_SHORT, &value_u16, sizeof(value_u16));

    value_u8 = false;
	config_service_set(CFG_GPS_CLOSE, TYPE_BOOL, &value_u8, sizeof(value_u8));

    value_u8 = true;
	config_service_set(CFG_IS_90V_POWER, TYPE_BOOL, &value_u8, sizeof(value_u8));

    if(config_service_is_main_server_goome())
    {
        value_u8 = 3;
    }
    else
    {
        value_u8 = 0;
    }
    config_service_set(CFG_SERVERLOCK, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u8 = false;
    config_service_set(CFG_OPEN_APGS, TYPE_BOOL, &value_u8, sizeof(value_u8));

    value_u8 = false;
    config_service_set(CFG_APP_BATTERT_MGR, TYPE_BOOL, &value_u8, sizeof(value_u8));

    value_u8 = true;
    config_service_set(CFG_IS_RELAY_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));

    //文件中只有CFG_IS_DOUBLE_GSENSOR,所以要 与 CFG_GSENNUM要一起设置
    value_u8 = 1;
    config_service_set(CFG_GSENNUM, TYPE_BYTE, &value_u8, sizeof(value_u8));
	
    value_u8 = false;
    config_service_set(CFG_IS_UART_9600, TYPE_BOOL, &value_u8, sizeof(value_u8));
    
    value_u8 = true;
    config_service_set(CFG_IS_UART_IO_WHEN_SLEEP, TYPE_BOOL, &value_u8, sizeof(value_u8));

    value_u8 = false;
    config_service_set(CFG_IS_MANUAL_DEFENCE, TYPE_BOOL, &value_u8, sizeof(value_u8));
    
    value_u8 = 0;
    config_service_set(CFG_BATTUPLOAD_DISABLE, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u8 = 0;
    config_service_set(CFG_LOWBATTALM_DISABLE, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u16 = GOOME_HEARTBEAT_DEFAULT;
    config_service_set(CFG_JT_HBINTERVAL, TYPE_SHORT, &value_u16, sizeof(value_u16));
	
	value_u8 = true;
	config_service_set(CFG_OPEN_APGS, TYPE_BOOL, &value_u8, sizeof(value_u8));

    switch(type)
    {
        case DEVICE_GS03A:
        case DEVICE_GM06E:
        case DEVICE_AS03A:
            value_u8 = false;
            config_service_set(CFG_IS_90V_POWER, TYPE_BOOL, &value_u8, sizeof(value_u8));
            break;

        case DEVICE_GS03F:
        case DEVICE_AS03F:
            value_u8 = false;
            config_service_set(CFG_IS_90V_POWER, TYPE_BOOL, &value_u8, sizeof(value_u8));
            value_u8 = false;
            config_service_set(CFG_IS_RELAY_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));
            value_u8 = true;
            config_service_set(CFG_IS_MANUAL_DEFENCE, TYPE_BOOL, &value_u8, sizeof(value_u8));
            break;
            
        case DEVICE_GS03B:
        case DEVICE_AS03B:
            value_u8 = false;
            config_service_set(CFG_IS_90V_POWER, TYPE_BOOL, &value_u8, sizeof(value_u8));
            value_u8 = 1;
            config_service_set(CFG_LOWBATTALM_DISABLE, TYPE_BYTE, &value_u8, sizeof(value_u8));
            value_u8 = false;
            config_service_set(CFG_IS_RELAY_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));
            value_u8 = true;
            config_service_set(CFG_IS_MANUAL_DEFENCE, TYPE_BOOL, &value_u8, sizeof(value_u8));
            break;
            
        case DEVICE_GS07B:
        case DEVICE_AS07B:
            value_u8 = false;
            config_service_set(CFG_IS_RELAY_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));
            value_u8 = true;
            config_service_set(CFG_IS_MANUAL_DEFENCE, TYPE_BOOL, &value_u8, sizeof(value_u8));
            break;

        case DEVICE_GS03I:
        case DEVICE_AS03I:
            value_u8 = false;
            config_service_set(CFG_IS_90V_POWER, TYPE_BOOL, &value_u8, sizeof(value_u8));
            value_u8 = 1;
            config_service_set(CFG_LOWBATTALM_DISABLE, TYPE_BYTE, &value_u8, sizeof(value_u8));
            value_u8 = false;
            config_service_set(CFG_IS_RELAY_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));

            value_u8 = 2;
            config_service_set(CFG_GSENNUM, TYPE_BYTE, &value_u8, sizeof(value_u8));
            value_u8 = true;
            config_service_set(CFG_IS_MANUAL_DEFENCE, TYPE_BOOL, &value_u8, sizeof(value_u8));
            break;

        case DEVICE_GS03H:
            value_u8 = false;
            config_service_set(CFG_IS_90V_POWER, TYPE_BOOL, &value_u8, sizeof(value_u8));
            value_u8 = 2;
            config_service_set(CFG_GSENNUM, TYPE_BYTE, &value_u8, sizeof(value_u8));
            break;

        case DEVICE_GS05A:
            value_u8 = true;
            config_service_set(CFG_APP_BATTERT_MGR, TYPE_BOOL, &value_u8, sizeof(value_u8));
            value_u8 = true;
            config_service_set(CFG_IS_UART_9600, TYPE_BOOL, &value_u8, sizeof(value_u8));
            break;

        case DEVICE_GS05B:
            value_u8 = 1;
            config_service_set(CFG_LOWBATTALM_DISABLE, TYPE_BYTE, &value_u8, sizeof(value_u8));
            value_u8 = true;
            config_service_set(CFG_IS_UART_9600, TYPE_BOOL, &value_u8, sizeof(value_u8));
            value_u8 = false;
            config_service_set(CFG_IS_RELAY_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));
            value_u8 = true;
            config_service_set(CFG_IS_MANUAL_DEFENCE, TYPE_BOOL, &value_u8, sizeof(value_u8));
            value_u8 = true;
            config_service_set(CFG_APP_BATTERT_MGR, TYPE_BOOL, &value_u8, sizeof(value_u8));
            break;

        case DEVICE_GS05F:
            value_u8 = true;
            config_service_set(CFG_IS_UART_9600, TYPE_BOOL, &value_u8, sizeof(value_u8));
            value_u8 = false;
            config_service_set(CFG_IS_RELAY_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));
            value_u8 = true;
            config_service_set(CFG_IS_MANUAL_DEFENCE, TYPE_BOOL, &value_u8, sizeof(value_u8));
            value_u8 = true;
            config_service_set(CFG_APP_BATTERT_MGR, TYPE_BOOL, &value_u8, sizeof(value_u8));
            break;

        case DEVICE_GS05I:
            value_u8 = 1;
            config_service_set(CFG_LOWBATTALM_DISABLE, TYPE_BYTE, &value_u8, sizeof(value_u8));
            value_u8 = 2;
            config_service_set(CFG_GSENNUM, TYPE_BYTE, &value_u8, sizeof(value_u8));
            value_u8 = true;
            config_service_set(CFG_IS_UART_9600, TYPE_BOOL, &value_u8, sizeof(value_u8));
            value_u8 = false;
            config_service_set(CFG_IS_RELAY_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));
            value_u8 = true;
            config_service_set(CFG_IS_MANUAL_DEFENCE, TYPE_BOOL, &value_u8, sizeof(value_u8));
            value_u8 = true;
            config_service_set(CFG_APP_BATTERT_MGR, TYPE_BOOL, &value_u8, sizeof(value_u8));
            break;

        case DEVICE_GS05H:
            value_u8 = true;
            config_service_set(CFG_APP_BATTERT_MGR, TYPE_BOOL, &value_u8, sizeof(value_u8));
            value_u8 = true;
            value_u8 = 2;
            config_service_set(CFG_GSENNUM, TYPE_BYTE, &value_u8, sizeof(value_u8));
            value_u8 = true;
            config_service_set(CFG_IS_UART_9600, TYPE_BOOL, &value_u8, sizeof(value_u8));
            break;
                    
        //case DEVICE_GS07A:
        //case DEVICE_AS07A:
        default:
            break;
    }
}


void config_service_restore_factory_config(bool is_all)
{
    if(is_all)
    {
        config_service_set_deault();
    }
    else
    {
        config_service_set_factory_deault();
    }
    
    config_service_set_device((ConfigDeviceTypeEnum)s_nvram->DeviceType);
    config_service_save_to_local();
}


GM_ERRCODE config_service_set_device_sim(u8 *pdata)
{
    u8 device_sim[20];
    u8 device_len = 0;
    
    if (GM_strstr((const char *)pdata, "#") == 0)
    {
        return GM_PARAM_ERROR;
    }

    GM_memset(device_sim, 0x00, sizeof(device_sim));
    GM_sscanf((const char *)pdata, "%[^#]#", device_sim);
    device_len = GM_strlen((const char *)device_sim);
    if ((device_len > 16) || (device_len < 11))
    {
        return GM_PARAM_ERROR;
    }

    config_service_set(CFG_SIM, TYPE_STRING, device_sim, device_len);

    return GM_SUCCESS;
}


void config_service_set_test_mode(bool state)
{
	config_service_set(CFG_IS_TEST_MODE, TYPE_BOOL, &state, sizeof(state));
}

bool config_service_is_test_mode(void)
{
    bool is_test_mode = false;
	config_service_get(CFG_IS_TEST_MODE, TYPE_BOOL, &is_test_mode, sizeof(is_test_mode));
    return is_test_mode;
}

bool config_service_is_default_imei(void)
{
    u8 imei[GM_IMEI_LEN + 1] = {0};
	gsm_get_imei(imei);
    if (0 == GM_strcmp((const char *)imei, (const char *)GOOME_IMEI_DEFAULT))
    {
        LOG(INFO,"clock(%d) config_service_is_default_imei(%s) = true.", util_clock(), imei);
        return true;
    }
	LOG(INFO,"clock(%d) config_service_is_default_imei(%s) = false.", util_clock(), imei);
    return false;
}

static void convert_cfg_to_para(GprsParaFileType *para)
{
    u8 value_u8,value_u8_2;
	char value_char = 0;
    u16 value_u16;
    u32 value_u32;
    float value_float;
    u8 value_str[CONFIG_STRING_MAX_LEN];
    s32 idx;

    GM_memset(para,0, sizeof(GprsParaFileType));
    para->len = sizeof(GprsParaFileType);
    para->magic = MAGIC_NUMBER;
    para->is_valid = UOP_OK;
    
	config_service_get(CFG_DEVICETYPE, TYPE_SHORT, &value_u16, sizeof(value_u16));
    para->device_type = value_u16;

    para->sock[SOCKET_INDEX_MAIN].sock_type = STREAM_TYPE_STREAM;
    GM_memset(value_str, 0x00, sizeof(value_str));
    idx = GM_sscanf((const char*)config_service_get_pointer(CFG_SERVERADDR), "%[^:]:%d", value_str, &value_u32);
    if (idx != 2 || GM_strlen((const char*)value_str) >= sizeof(para->sock[SOCKET_INDEX_MAIN].src_dns.dns) || value_u32>65535)
    {
        LOG(WARN,"convert_cfg_to_para assert(idx ==2) of CFG_SERVERADDR failed.");
        return;
    }
    GM_strcpy((char*)para->sock[SOCKET_INDEX_MAIN].src_dns.dns, (const char*)value_str);
    para->sock[SOCKET_INDEX_MAIN].src_dns.port = value_u32;

    para->sock[SOCKET_INDEX_AGPS].sock_type = STREAM_TYPE_DGRAM;
    GM_memset(value_str, 0x00, sizeof(value_str));
    idx = GM_sscanf((const char*)config_service_get_pointer(CFG_AGPSSERVERADDR), "%[^:]:%d", value_str, &value_u32);
    if (idx != 2 || GM_strlen((const char*)value_str) >= sizeof(para->sock[SOCKET_INDEX_AGPS].src_dns.dns) || value_u32>65535)
    {
        LOG(WARN,"convert_cfg_to_para assert(idx ==2) of CFG_AGPSSERVERADDR failed.");
        return;
    }
    GM_strcpy((char*)para->sock[SOCKET_INDEX_AGPS].src_dns.dns, (const char*)value_str);
    para->sock[SOCKET_INDEX_AGPS].src_dns.port = value_u32;
    
    para->sock[SOCKET_INDEX_LOG].sock_type = STREAM_TYPE_DGRAM;
    GM_memset(value_str, 0x00, sizeof(value_str));
    idx = GM_sscanf((const char*)config_service_get_pointer(CFG_LOGSERVERADDR), "%[^:]:%d", value_str, &value_u32);
    if (idx != 2 || GM_strlen((const char*)value_str) >= sizeof(para->sock[SOCKET_INDEX_LOG].src_dns.dns) || value_u32>65535)
    {
        LOG(WARN,"convert_cfg_to_para assert(idx ==2) of CFG_LOGSERVERADDR failed.");
        return;
    }
    GM_strcpy((char*)para->sock[SOCKET_INDEX_LOG].src_dns.dns, (const char*)value_str);
    para->sock[SOCKET_INDEX_LOG].src_dns.port = value_u32;
    
    para->sock[SOCKET_INDEX_UPDATE].sock_type = STREAM_TYPE_DGRAM;
    GM_memset(value_str, 0x00, sizeof(value_str));
    idx = GM_sscanf((const char*)config_service_get_pointer(CFG_UPDATESERVERADDR), "%[^:]:%d", value_str, &value_u32);
    if (idx != 2 || GM_strlen((const char*)value_str) >= sizeof(para->sock[SOCKET_INDEX_UPDATE].src_dns.dns) || value_u32>65535)
    {
        LOG(WARN,"convert_cfg_to_para assert(idx ==2) of CFG_UPDATESERVERADDR failed.");
        return;
    }
    GM_strcpy((char*)para->sock[SOCKET_INDEX_UPDATE].src_dns.dns, (const char*)value_str);
    para->sock[SOCKET_INDEX_UPDATE].src_dns.port = value_u32;
    
    para->sock[SOCKET_INDEX_CONFIG].sock_type = STREAM_TYPE_DGRAM;
    GM_memset(value_str, 0x00, sizeof(value_str));
    idx = GM_sscanf((const char*)config_service_get_pointer(CFG_CFGSERVERADDR), "%[^:]:%d", value_str, &value_u32);
    if (idx != 2 || GM_strlen((const char*)value_str) >= sizeof(para->sock[SOCKET_INDEX_CONFIG].src_dns.dns) || value_u32>65535)
    {
        LOG(WARN,"convert_cfg_to_para assert(idx ==2) of CFG_CFGSERVERADDR failed.");
        return;
    }
    GM_strcpy((char*)para->sock[SOCKET_INDEX_CONFIG].src_dns.dns, (const char*)value_str);
    para->sock[SOCKET_INDEX_CONFIG].src_dns.port = value_u32;
    
    config_service_get(CFG_APN_NAME, TYPE_STRING, value_str, sizeof(value_str));
    GM_strcpy((char *)para->Apn,(const char*)value_str);
    
    config_service_get(CFG_APN_USER, TYPE_STRING, value_str, sizeof(value_str));
    GM_strcpy((char *)para->para_user,(const char*)value_str);

    config_service_get(CFG_APN_PWD, TYPE_STRING, value_str, sizeof(value_str));
    GM_strcpy((char *)para->para_pwd,(const char*)value_str);


    config_service_get(CFG_JT_HBINTERVAL, TYPE_SHORT, &value_u16, sizeof(value_u16));
    para->jt_location_heart_time = value_u16;

    config_service_get(CFG_UPLOADTIME, TYPE_SHORT, &value_u16, sizeof(value_u16));
    para->upload_time = value_u16;

    // 心跳间隔
    config_service_get(CFG_HEART_INTERVAL, TYPE_SHORT, &value_u16, sizeof(value_u16));
    para->heart_interval = value_u16;
    
    config_service_get(CFG_DISTANCE_FOR_LOW_SPEED, TYPE_SHORT, &value_u16, sizeof(value_u16));
    para->distance_of_slow = value_u16;

	config_service_get(CFG_GPS_UPDATE_TIME, TYPE_BYTE, &value_u8, sizeof(value_u8));
    para->gps_update_gap = value_u8;

    config_service_get(CFG_HBPROTOCOL, TYPE_BYTE, &value_u8, sizeof(value_u8));
    para->heart_protocol = value_u8;

    config_service_get(CFG_TIME_ZONE, TYPE_BYTE, &value_char, sizeof(value_char));
    para->local_time_hour = value_char;

    config_service_get(CFG_SPEED_ALARM_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));
    para->speed_switch = value_u8;

    config_service_get(CFG_SPEEDTHR, TYPE_BYTE, &value_u8, sizeof(value_u8));
    para->speed_threshold = value_u8;

    config_service_get(CFG_SPEED_CHECK_TIME, TYPE_BYTE, &value_u8, sizeof(value_u8));
    para->speed_detect_time = value_u8;

    para->reserved_u8_3 = 0;
    para->reserved_u8_4 = 0;



    config_service_get(CFG_IS_MANUAL_DEFENCE, TYPE_BOOL, &value_u8, sizeof(value_u8));
    if (value_u8)
    {
        SET_BIT0(para->param_bits);
    }
    else
    {
        CLR_BIT0(para->param_bits);
    }
	
	config_service_get(CFG_IS_SHAKE_ALARM_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));
    if (value_u8)
    {
        SET_BIT1(para->param_bits);
    }
    else
    {
        CLR_BIT1(para->param_bits);
    }

    config_service_get(CFG_SENSOR_ALARM_MODE, TYPE_BYTE, &value_u8, sizeof(value_u8));
    switch(value_u8)
    {
        case PWRALM_GPRS:
            CLR_BIT2(para->param_bits);
            CLR_BIT3(para->param_bits);
            break;
        case PWRALM_GPRS_SMS:
            SET_BIT2(para->param_bits);
            CLR_BIT3(para->param_bits);
            break;
        case PWRALM_GPRS_SMS_CALL:
            CLR_BIT2(para->param_bits);
            SET_BIT3(para->param_bits);
            break;
        case PWRALM_GPRS_CALL:
            SET_BIT2(para->param_bits);
            SET_BIT3(para->param_bits);
            break;
    }

    config_service_get(CFG_ALARM_SMS_ONE, TYPE_BOOL, &value_u8, sizeof(value_u8));
    if (value_u8)
    {
        SET_BIT6(para->param_bits);
    }
    else
    {
        CLR_BIT6(para->param_bits);
    }


    config_service_get(CFG_LOWBATTALM_DISABLE, TYPE_BYTE, &value_u8, sizeof(value_u8));
    if (value_u8)
    {
        SET_BIT7(para->param_bits);
    }
    else
    {
        CLR_BIT7(para->param_bits);
    }

    config_service_get(CFG_CUTOFFALM_DISABLE, TYPE_BYTE, &value_u8, sizeof(value_u8));
    if (value_u8)
    {
        SET_BIT9(para->param_bits);
    }
    else
    {
        CLR_BIT9(para->param_bits);
    }


    config_service_get(CFG_APN_CHECK, TYPE_BOOL, &value_u8, sizeof(value_u8));
    if (value_u8)
    {
        SET_BIT15(para->param_bits);
    }
    else
    {
        CLR_BIT15(para->param_bits);
    }

    config_service_get(CFG_APP_BATTERT_MGR, TYPE_BOOL, &value_u8, sizeof(value_u8));
    if (value_u8)
    {
        SET_BIT19(para->param_bits);
    }
    else
    {
        CLR_BIT19(para->param_bits);
    }
    
    config_service_get(CFG_POWER_ALARM_MODE, TYPE_BYTE, &value_u8, sizeof(value_u8));
    switch(value_u8)
    {
        case PWRALM_GPRS: 
            CLR_BIT10(para->param_bits);
            CLR_BIT11(para->param_bits);
            break;
        case PWRALM_GPRS_SMS:
            SET_BIT10(para->param_bits);
            CLR_BIT11(para->param_bits);
            break;
        case PWRALM_GPRS_SMS_CALL:
            CLR_BIT10(para->param_bits);
            SET_BIT11(para->param_bits);
            break;
        case PWRALM_GPRS_CALL:
            SET_BIT10(para->param_bits);
            SET_BIT11(para->param_bits);
            break;
    }

    config_service_get(CFG_IS_STATIC_UPLOAD, TYPE_BOOL, &value_u8, sizeof(value_u8));
    if (value_u8)
    {
        CLR_BIT13(para->param_bits);
    }
    else
    {
        SET_BIT13(para->param_bits);
    }

    config_service_get(CFG_IS_LBS_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));
    if (value_u8)
    {
        SET_BIT14(para->param_bits);
    }
    else
    {
        CLR_BIT14(para->param_bits);
    }


    config_service_get(CFG_OPEN_APGS, TYPE_BOOL, &value_u8, sizeof(value_u8));
    if (value_u8)
    {
        SET_BIT20(para->param_bits);
    }
    else
    {
        CLR_BIT20(para->param_bits);
    }

	config_service_get(CFG_IS_90V_POWER, TYPE_BOOL, &value_u8, sizeof(value_u8));
    if (value_u8)
    {
        SET_BIT21(para->param_bits);
    }
    else
    {
        CLR_BIT21(para->param_bits);
    }

    config_service_get(CFG_IS_UART_IO_WHEN_SLEEP, TYPE_BOOL, &value_u8, sizeof(value_u8));
    if (value_u8)
    {
        SET_BIT22(para->param_bits);
    }
    else
    {
        CLR_BIT22(para->param_bits);
    }

    config_service_get(CFG_SMOOTH_TRACK, TYPE_BOOL, &value_u8, sizeof(value_u8));
    if (value_u8)
    {
        SET_BIT23(para->param_bits);
    }
    else
    {
        CLR_BIT23(para->param_bits);
    }

    config_service_get(CFG_BATTUPLOAD_DISABLE, TYPE_BYTE, &value_u8, sizeof(value_u8));
    if (value_u8)
    {
        SET_BIT24(para->param_bits);
    }
    else
    {
        CLR_BIT24(para->param_bits);
    }

	config_service_get(CFG_GPS_CLOSE, TYPE_BOOL, &value_u8, sizeof(value_u8));
    if (value_u8)
    {
        SET_BIT25(para->param_bits);
    }
    else
    {
        CLR_BIT25(para->param_bits);
    }

    config_service_get(CFG_IS_UART_9600, TYPE_BOOL, &value_u8, sizeof(value_u8));
    if (value_u8)
    {
        SET_BIT26(para->param_bits);
    }
    else
    {
        CLR_BIT26(para->param_bits);
    }

    config_service_get(CFG_IS_RELAY_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));
    if (value_u8)
    {
        SET_BIT27(para->param_bits);
    }
    else
    {
        CLR_BIT27(para->param_bits);
    }

	config_service_get(CFG_IS_MOVEALARM_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));
    if (value_u8)
    {
        SET_BIT29(para->param_bits);
    }
    else
    {
        CLR_BIT29(para->param_bits);
    }

	config_service_get(CFG_IS_ACLRALARM_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));
    if (value_u8)
    {
        SET_BIT30(para->param_bits);
    }
    else
    {
        CLR_BIT30(para->param_bits);
    }

	config_service_get(CFG_IS_BMS_MOS_CUT_OFF, TYPE_BOOL, &value_u8, sizeof(value_u8));
    if (value_u8)
    {
        SET_BIT31(para->param_bits);
    }
    else
    {
        CLR_BIT31(para->param_bits);
    }
	


    config_service_get(CFG_CENTER_NUMBER, TYPE_STRING, para->center_number, sizeof(para->center_number));
    config_service_get(CFG_USER1_NUMBER, TYPE_STRING, para->user_number_1, sizeof(para->user_number_1));
    config_service_get(CFG_USER2_NUMBER, TYPE_STRING, para->user_number_2, sizeof(para->user_number_2));
    config_service_get(CFG_USER3_NUMBER, TYPE_STRING, para->user_number_3, sizeof(para->user_number_3));
    config_service_get(CFG_USER4_NUMBER, TYPE_STRING, para->user_number_4, sizeof(para->user_number_4));


    para->reserved_u8_7  = 0;

    para->reserved_u8_8     = 12;

    // 低电阀值
    para->reserved_u8_9 = 10;
    
    // 断电阀值
    para->reserved_u8_10  = 5;

    config_service_get(CFG_POWER_CHECK_TIME, TYPE_SHORT, &value_u16, sizeof(value_u16));
    para->power_alarm_check_tim = value_u16;
    
    config_service_get(CFG_POWER_CHARGE_MIN_TIME, TYPE_SHORT, &value_u16, sizeof(value_u16));
    para->power_alarm_chr_tim = value_u16;
    
	config_service_get(CFG_TURN_ANGLE, TYPE_SHORT, &value_u16, sizeof(value_u16));
    para->turn_angle  = (u16)value_u16;

	config_service_get(CFG_GPS_TYPE, TYPE_BYTE, &value_u8, sizeof(value_u8));
    para->gps_type  = value_u8;

    config_service_get(CFG_POWER_ACC_CHECK_TIME, TYPE_SHORT, &value_u16, sizeof(value_u16));
    para->power_alarm_acc_tim = value_u16;

    config_service_get(CFG_SHAKE_THRESHOLD, TYPE_INT, &value_u32, sizeof(value_u32));
    para->shake_threshold = (u16)value_u32;


    config_service_get(CFG_JT_DEVICE_ID, TYPE_STRING, value_str, sizeof(value_str));
    GM_memcpy(para->login.device_id, value_str, sizeof(para->login.device_id));
    
    config_service_get(CFG_JT_OEM_ID, TYPE_STRING, value_str, sizeof(value_str));
    GM_memcpy(para->login.OEM_id, value_str, sizeof(para->login.OEM_id));
    
    config_service_get(CFG_JT_VEHICLE_NUMBER, TYPE_STRING, value_str, sizeof(value_str));
    GM_memcpy(para->login.vehicle_number, value_str, sizeof(para->login.vehicle_number));

    config_service_get(CFG_JT_PROVINCE, TYPE_SHORT, &value_u16, sizeof(value_u16));
    para->login.province_id = value_u16;
    
    config_service_get(CFG_JT_CITY, TYPE_SHORT, &value_u16, sizeof(value_u16));
    para->login.city_id = value_u16;

    config_service_get(CFG_JT_AT_TYPE, TYPE_BYTE, &value_u8, sizeof(value_u8));
    para->login.AT_protocol = value_u8;

    config_service_get(CFG_JT_VEHICLE_COLOR, TYPE_BYTE, &value_u8, sizeof(value_u8));
    para->login.vehicle_color = value_u8;

    config_service_get(CFG_JT_AUTH_CODE, TYPE_STRING, value_str, sizeof(value_str));
    GM_memcpy(para->jt_authority_code, value_str, sizeof(para->jt_authority_code));

    config_service_get(CFG_SHAKE_COUNT, TYPE_BYTE, &value_u8, sizeof(value_u8));
    para->shock_alarm_count = value_u8;

    config_service_get(CFG_MAP_URL, TYPE_STRING, value_str, sizeof(value_str));
    GM_strncpy((char *)para->map_url, (const char *)value_str, sizeof(para->map_url));

    config_service_get(CFG_LANGUAGE, TYPE_SHORT, &value_u16, sizeof(value_u16));
    para->language = value_u16;
    
    config_service_get(CFG_SEN_ANGLE_SHOW_TIME, TYPE_BYTE, &value_u8, sizeof(value_u8));
    para->angle_view_time = value_u8;

    para->reserved_u8_13        = 0; 

	config_service_get(CFG_SHAKE_LEVEL, TYPE_BYTE, &value_u8, sizeof(value_u8));
    para->shake_level = value_u8;

    config_service_get(CFG_AWAKE_CHECK_TIME, TYPE_BYTE, &value_u8, sizeof(value_u8));
    para->shake_time = value_u8;
    
    config_service_get(CFG_AWAKE_COUNT, TYPE_BYTE, &value_u8, sizeof(value_u8));
    para->shake_cnt = value_u8;

    config_service_get(CFG_CLOSE_DRIFT_DETECT, TYPE_BOOL, &value_u8, sizeof(value_u8));
    para->sf_switch = value_u8;

    para->shake_durn_tim = 1;

    config_service_get(CFG_SLEEP_TIME, TYPE_SHORT, &value_u16, sizeof(value_u16));
    para->sleep_tim = value_u16;

    config_service_get(CFG_SHAKE_CHECK_TIME, TYPE_BYTE, &value_u8, sizeof(value_u8));
    para->shake_interval = value_u8;

    config_service_get(CFG_PROTOCOL, TYPE_BYTE, &value_u8, sizeof(value_u8));
    config_service_get(CFG_PROTOCOL_VER, TYPE_BYTE, &value_u8_2, sizeof(value_u8_2));
    para->protocol_type = MERGEBCD(value_u8,value_u8_2);

    config_service_get(CFG_JT_ISREGISTERED, TYPE_BOOL, &value_u8, sizeof(value_u8));
    para->jt_registered = value_u8;

    config_service_get(CFG_CHARGE_DELAY, TYPE_SHORT, &value_u16, sizeof(value_u16));
    para->chr_delay_tim = value_u16;

    config_service_get(CFG_SERVERLOCK, TYPE_BYTE, &value_u8, sizeof(value_u8));
    para->ip_lock_mode = value_u8;

    config_service_get(CFG_MIN_SNR, TYPE_BYTE, &value_u8, sizeof(value_u8));
    para->min_snr = value_u8;

    config_service_get(CFG_SHAKE_ALARM_DELAY, TYPE_SHORT, &value_u16, sizeof(value_u16));
    para->shake_alarm_timeout = value_u16;
    
    config_service_get(CFG_SHAKE_ALARM_INTERVAL, TYPE_SHORT, &value_u16, sizeof(value_u16));
    para->shake_alarm_interval = value_u16;

    
    config_service_get(CFG_AUTO_DEFENCE_DELAY, TYPE_SHORT, &value_u16, sizeof(value_u16));
    para->defende_tim = value_u16;
    


    config_service_get(CFG_STATIC_CHECK_TIME, TYPE_SHORT, &value_u16, sizeof(value_u16));
    para->static_check_interval = value_u16;

    config_service_get(CFG_STATIC_CHECK_SPEED, TYPE_BYTE, &value_u8, sizeof(value_u8));
    para->static_check_speed= value_u8;

    config_service_get(CFG_STATIC_CHECK_COUNT, TYPE_BYTE, &value_u8, sizeof(value_u8));
    para->static_check_count= value_u8;

    config_service_get(CFG_LBS_INTERVAL, TYPE_SHORT, &value_u16, sizeof(value_u16));
    para->lbs_interval = value_u16;

    config_service_get(CFG_WAIT_GPS_TIME, TYPE_SHORT, &value_u16, sizeof(value_u16));
    para->gps_not_fix_time = value_u16;

    config_service_get(CFG_REOPEN_GSP_TIME, TYPE_BYTE, &value_u8, sizeof(value_u8));
    para->reopen_gps_time = value_u8;

    config_service_get(CFG_ACC_CHECK_TIME, TYPE_SHORT, &value_u16, sizeof(value_u16));
    para->acc_check_interval = value_u16;

	config_service_get(CFG_SEN_RAPID_ACLR, TYPE_FLOAT, &value_float, sizeof(value_float));
    para->sensor_threshold[THRESHOLD_INDEX_FOR_RAPID_ACLR] = value_float;

	config_service_get(CFG_SEN_EMERGENCY_BRAKE, TYPE_FLOAT, &value_float, sizeof(value_float));
    para->sensor_threshold[THRESHOLD_INDEX_FOR_EMERGENCY_BRAKE] = value_float;

	config_service_get(CFG_SEN_SLIGHT_COLLISION, TYPE_FLOAT, &value_float, sizeof(value_float));
    para->sensor_threshold[THRESHOLD_INDEX_FOR_SLIGHT_COLLISION] = value_float;

	config_service_get(CFG_SEN_NORMAL_COLLISION, TYPE_FLOAT, &value_float, sizeof(value_float));
    para->sensor_threshold[THRESHOLD_INDEX_FOR_NORMAL_COLLISION] = value_float;

	config_service_get(CFG_SEN_SERIOUS_COLLISION, TYPE_FLOAT, &value_float, sizeof(value_float));
    para->sensor_threshold[THRESHOLD_INDEX_FOR_SERIOUS_COLLISION] = value_float;

	config_service_get(CFG_SEN_SUDDEN_TURN, TYPE_FLOAT, &value_float, sizeof(value_float));
    para->sensor_threshold[THRESHOLD_INDEX_FOR_SUDDEN_TURN] = value_float;

	config_service_get(CFG_SEN_LOWVOlTAGE_ALARM, TYPE_FLOAT, &value_float, sizeof(value_float));
    para->sensor_threshold[THRESHOLD_INDEX_FOR_LOWVOlTAGE_ALARM] = value_float;

	config_service_get(CFG_SEN_QUAKE_ALARM, TYPE_FLOAT, &value_float, sizeof(value_float));
    para->sensor_threshold[THRESHOLD_INDEX_FOR_QUAKE_ALARM] = value_float;

	config_service_get(CFG_SEN_STATIC, TYPE_FLOAT, &value_float, sizeof(value_float));
    para->sensor_threshold[THRESHOLD_INDEX_FOR_STATIC] = value_float;

	config_service_get(CFG_SEN_RUN, TYPE_FLOAT, &value_float, sizeof(value_float));
    para->sensor_threshold[THRESHOLD_INDEX_FOR_RUN] = value_float;
}


static void convert_para_to_cfg(const GprsParaFileType *para)
{
    u8 value_u8,value_u8_2;
	s8 value_s8 = 0;
    u16 value_u16;
    u32 value_u32;
    float value_float;
    u8 value_str[CONFIG_STRING_MAX_LEN];
    
    value_u16 = para->device_type;
	config_service_set(CFG_DEVICETYPE, TYPE_SHORT, &value_u16, sizeof(value_u16));

    //lz mod for compatable T808 device config only ip.
    if(util_is_valid_dns(para->sock[SOCKET_INDEX_MAIN].src_dns.dns, GM_strlen((const char*)para->sock[SOCKET_INDEX_MAIN].src_dns.dns)))
    {
        GM_snprintf((char*)value_str,sizeof(value_str), "%s:%d",(char*)para->sock[SOCKET_INDEX_MAIN].src_dns.dns, para->sock[SOCKET_INDEX_MAIN].src_dns.port);
        config_service_set(CFG_SERVERADDR, TYPE_STRING, &value_str, GM_strlen((char*)value_str));
		LOG(INFO,"is domain:%s",value_str);
    }
    else if(util_is_valid_ip(para->sock[SOCKET_INDEX_MAIN].sock_ip[TCP_IP].IP, sizeof(para->sock[SOCKET_INDEX_MAIN].sock_ip[TCP_IP].IP)))
    {
        GM_snprintf((char*)value_str,sizeof(value_str), "%d.%d.%d.%d:%d",para->sock[SOCKET_INDEX_MAIN].sock_ip[TCP_IP].IP[0], para->sock[SOCKET_INDEX_MAIN].sock_ip[TCP_IP].IP[1],
            para->sock[SOCKET_INDEX_MAIN].sock_ip[TCP_IP].IP[2],para->sock[SOCKET_INDEX_MAIN].sock_ip[TCP_IP].IP[3],para->sock[SOCKET_INDEX_MAIN].sock_ip[TCP_IP].PORT);
        config_service_set(CFG_SERVERADDR, TYPE_STRING, &value_str, GM_strlen((char*)value_str));

		LOG(INFO,"is IP:%s",value_str);
    }

    //以下服务器用默认值，所以不用存储的内容更新
    GM_snprintf((char*)value_str,sizeof(value_str), "%s:%d",(char*)para->sock[SOCKET_INDEX_AGPS].src_dns.dns, para->sock[SOCKET_INDEX_AGPS].src_dns.port);
	//config_service_set(CFG_AGPSSERVERADDR, TYPE_STRING, &value_str, GM_strlen((char*)value_str));

    GM_snprintf((char*)value_str,sizeof(value_str), "%s:%d",(char*)para->sock[SOCKET_INDEX_LOG].src_dns.dns, para->sock[SOCKET_INDEX_LOG].src_dns.port);
	//config_service_set(CFG_LOGSERVERADDR, TYPE_STRING, &value_str, GM_strlen((char*)value_str));
    
    GM_snprintf((char*)value_str,sizeof(value_str), "%s:%d",(char*)para->sock[SOCKET_INDEX_UPDATE].src_dns.dns, para->sock[SOCKET_INDEX_UPDATE].src_dns.port);
	//config_service_set(CFG_UPDATESERVERADDR, TYPE_STRING, &value_str, GM_strlen((char*)value_str));

    GM_snprintf((char*)value_str,sizeof(value_str), "%s:%d",(char*)para->sock[SOCKET_INDEX_CONFIG].src_dns.dns, para->sock[SOCKET_INDEX_CONFIG].src_dns.port);
	//config_service_set(CFG_CFGSERVERADDR, TYPE_STRING, &value_str, GM_strlen((char*)value_str));
    

	config_service_set(CFG_APN_NAME, TYPE_STRING, &para->Apn, GM_strlen((const char*)para->Apn));
	config_service_set(CFG_APN_USER, TYPE_STRING, &para->para_user, GM_strlen((const char*)para->para_user));
	config_service_set(CFG_APN_PWD, TYPE_STRING, &para->para_pwd, GM_strlen((const char*)para->para_pwd));


    value_u16 = para->jt_location_heart_time;
    config_service_set(CFG_JT_HBINTERVAL, TYPE_SHORT, &value_u16, sizeof(value_u16));

    value_u16 = para->upload_time;
    config_service_set(CFG_UPLOADTIME, TYPE_SHORT, &value_u16, sizeof(value_u16));

    // 心跳间隔
    value_u16 = para->heart_interval;
    config_service_set(CFG_HEART_INTERVAL, TYPE_SHORT, &value_u16, sizeof(value_u16));
    
    value_u16 = para->distance_of_slow;
    config_service_set(CFG_DISTANCE_FOR_LOW_SPEED, TYPE_SHORT, &value_u16, sizeof(value_u16));

    value_u8 = para->gps_update_gap;
	config_service_set(CFG_GPS_UPDATE_TIME, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u8 = para->heart_protocol;
    config_service_set(CFG_HBPROTOCOL, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_s8 = para->local_time_hour;
    config_service_set(CFG_TIME_ZONE, TYPE_BYTE, &value_s8, sizeof(value_s8));

    value_u8 = para->speed_switch;
    config_service_set(CFG_SPEED_ALARM_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));

    value_u8 = para->speed_threshold;
    config_service_set(CFG_SPEEDTHR, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u8 = para->speed_detect_time;
    config_service_set(CFG_SPEED_CHECK_TIME, TYPE_BYTE, &value_u8, sizeof(value_u8));

    if(GET_BIT2(para->param_bits))
    {
        if(GET_BIT3(para->param_bits))
        {
            value_u8 = PWRALM_GPRS_CALL;
            config_service_set(CFG_SENSOR_ALARM_MODE, TYPE_BYTE, &value_u8, sizeof(value_u8));
        }
        else
        {
            value_u8 = PWRALM_GPRS_SMS;
            config_service_set(CFG_SENSOR_ALARM_MODE, TYPE_BYTE, &value_u8, sizeof(value_u8));
        }
    }
    else
    {
        if(GET_BIT3(para->param_bits))
        {
            value_u8 = PWRALM_GPRS_SMS_CALL;
            config_service_set(CFG_SENSOR_ALARM_MODE, TYPE_BYTE, &value_u8, sizeof(value_u8));
        }
        else
        {
            value_u8 = PWRALM_GPRS;
            config_service_set(CFG_SENSOR_ALARM_MODE, TYPE_BYTE, &value_u8, sizeof(value_u8));
        }
    }

    value_u8 = GET_BIT0(para->param_bits);
    config_service_set(CFG_IS_MANUAL_DEFENCE, TYPE_BOOL, &value_u8, sizeof(value_u8));

	value_u8 = GET_BIT1(para->param_bits);
    config_service_set(CFG_IS_SHAKE_ALARM_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));

    value_u8 = GET_BIT6(para->param_bits);
    config_service_set(CFG_ALARM_SMS_ONE, TYPE_BOOL, &value_u8, sizeof(value_u8));

    value_u8 = GET_BIT7(para->param_bits);
    config_service_set(CFG_LOWBATTALM_DISABLE, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u8 = GET_BIT9(para->param_bits);
    config_service_set(CFG_CUTOFFALM_DISABLE, TYPE_BYTE, &value_u8, sizeof(value_u8));
    
    if(GET_BIT10(para->param_bits))
    {
        if(GET_BIT11(para->param_bits))
        {
            value_u8 = PWRALM_GPRS_CALL;
            config_service_set(CFG_POWER_ALARM_MODE, TYPE_BYTE, &value_u8, sizeof(value_u8));
        }
        else
        {
            value_u8 = PWRALM_GPRS_SMS;
            config_service_set(CFG_POWER_ALARM_MODE, TYPE_BYTE, &value_u8, sizeof(value_u8));
        }
    }
    else
    {
        if(GET_BIT11(para->param_bits))
        {
            value_u8 = PWRALM_GPRS_SMS_CALL;
            config_service_set(CFG_POWER_ALARM_MODE, TYPE_BYTE, &value_u8, sizeof(value_u8));
        }
        else
        {
            value_u8 = PWRALM_GPRS;
            config_service_set(CFG_POWER_ALARM_MODE, TYPE_BYTE, &value_u8, sizeof(value_u8));
        }
    }


    value_u8 = GET_BIT13(para->param_bits);
    value_u8 = value_u8?false:true;
    config_service_set(CFG_IS_STATIC_UPLOAD, TYPE_BOOL, &value_u8, sizeof(value_u8));

    value_u8 = GET_BIT14(para->param_bits);
    config_service_set(CFG_IS_LBS_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));

    value_u8 = GET_BIT15(para->param_bits);
    config_service_set(CFG_APN_CHECK, TYPE_BOOL, &value_u8, sizeof(value_u8));

    value_u8 = GET_BIT19(para->param_bits);
    config_service_set(CFG_APP_BATTERT_MGR, TYPE_BOOL, &value_u8, sizeof(value_u8));

    value_u8 = GET_BIT20(para->param_bits);
    config_service_set(CFG_OPEN_APGS, TYPE_BOOL, &value_u8, sizeof(value_u8));

    value_u8 = GET_BIT21(para->param_bits);
	config_service_set(CFG_IS_90V_POWER, TYPE_BOOL, &value_u8, sizeof(value_u8));

    value_u8 = GET_BIT22(para->param_bits);
    config_service_set(CFG_IS_UART_IO_WHEN_SLEEP, TYPE_BOOL, &value_u8, sizeof(value_u8));

    value_u8 = GET_BIT23(para->param_bits);
    config_service_set(CFG_SMOOTH_TRACK, TYPE_BOOL, &value_u8, sizeof(value_u8));

    value_u8 = GET_BIT24(para->param_bits);
    config_service_set(CFG_BATTUPLOAD_DISABLE, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u8 = GET_BIT25(para->param_bits);
	config_service_set(CFG_GPS_CLOSE, TYPE_BOOL, &value_u8, sizeof(value_u8));

    value_u8 = GET_BIT26(para->param_bits);
    config_service_set(CFG_IS_UART_9600, TYPE_BOOL, &value_u8, sizeof(value_u8));

    value_u8 = GET_BIT27(para->param_bits);
    config_service_set(CFG_IS_RELAY_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));

	value_u8 = GET_BIT29(para->param_bits);
    config_service_set(CFG_IS_MOVEALARM_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));

	value_u8 = GET_BIT30(para->param_bits);
    config_service_set(CFG_IS_ACLRALARM_ENABLE, TYPE_BOOL, &value_u8, sizeof(value_u8));

	value_u8 = GET_BIT31(para->param_bits);
    config_service_set(CFG_IS_BMS_MOS_CUT_OFF, TYPE_BOOL, &value_u8, sizeof(value_u8));
	
    config_service_set(CFG_CENTER_NUMBER, TYPE_STRING, para->center_number, GM_strlen((char *)para->center_number));
    config_service_set(CFG_USER1_NUMBER, TYPE_STRING, para->user_number_1, GM_strlen((char *)para->user_number_1));
    config_service_set(CFG_USER2_NUMBER, TYPE_STRING, para->user_number_2, GM_strlen((char *)para->user_number_2));
    config_service_set(CFG_USER3_NUMBER, TYPE_STRING, para->user_number_3, GM_strlen((char *)para->user_number_3));
    config_service_set(CFG_USER4_NUMBER, TYPE_STRING, para->user_number_4, GM_strlen((char *)para->user_number_4));


    value_u16 = para->power_alarm_check_tim;
    config_service_set(CFG_POWER_CHECK_TIME, TYPE_SHORT, &value_u16, sizeof(value_u16));
    
    value_u16 = para->power_alarm_chr_tim;
    config_service_set(CFG_POWER_CHARGE_MIN_TIME, TYPE_SHORT, &value_u16, sizeof(value_u16));

    value_u16 = para->turn_angle;
	config_service_set(CFG_TURN_ANGLE, TYPE_SHORT, &value_u16, sizeof(value_u16));

    value_u8 = para->gps_type;
	config_service_set(CFG_GPS_TYPE, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u16 = para->power_alarm_acc_tim;
    config_service_set(CFG_POWER_ACC_CHECK_TIME, TYPE_SHORT, &value_u16, sizeof(value_u16));

    value_u32 = para->shake_threshold;
    config_service_set(CFG_SHAKE_THRESHOLD, TYPE_INT, &value_u32, sizeof(value_u32));


    //注意以下三处都是sizeof, 因为para指向的结构中,没有为结束符预留位置
    config_service_set(CFG_JT_DEVICE_ID, TYPE_STRING, para->login.device_id, sizeof(para->login.device_id));
    
    config_service_set(CFG_JT_OEM_ID, TYPE_STRING, para->login.OEM_id, sizeof(para->login.OEM_id));
    
    config_service_set(CFG_JT_VEHICLE_NUMBER, TYPE_STRING, para->login.vehicle_number, sizeof(para->login.vehicle_number));

    value_u16 = para->login.province_id;
    config_service_set(CFG_JT_PROVINCE, TYPE_SHORT, &value_u16, sizeof(value_u16));
    
    value_u16 = para->login.city_id;
    config_service_set(CFG_JT_CITY, TYPE_SHORT, &value_u16, sizeof(value_u16));

    value_u8 = para->login.AT_protocol;
    config_service_set(CFG_JT_AT_TYPE, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u8 = para->login.vehicle_color;
    config_service_set(CFG_JT_VEHICLE_COLOR, TYPE_BYTE, &value_u8, sizeof(value_u8));

    config_service_set(CFG_JT_AUTH_CODE, TYPE_STRING, para->jt_authority_code, GM_strlen((char *)para->jt_authority_code));

    value_u8 = para->shock_alarm_count;
    config_service_set(CFG_SHAKE_COUNT, TYPE_BYTE, &value_u8, sizeof(value_u8));

    config_service_set(CFG_MAP_URL, TYPE_STRING, para->map_url, GM_strlen((char *)para->map_url));

    value_u16 = para->language;
    config_service_set(CFG_LANGUAGE, TYPE_SHORT, &value_u16, sizeof(value_u16));
    
    value_u8 = para->angle_view_time;
    config_service_set(CFG_SEN_ANGLE_SHOW_TIME, TYPE_BYTE, &value_u8, sizeof(value_u8));


    value_u8 = para->shake_level;
	config_service_set(CFG_SHAKE_LEVEL, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u8 = para->shake_time;
    config_service_set(CFG_AWAKE_CHECK_TIME, TYPE_BYTE, &value_u8, sizeof(value_u8));
    
    value_u8 = para->shake_cnt;
    config_service_set(CFG_AWAKE_COUNT, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u8 = para->sf_switch;
    config_service_set(CFG_CLOSE_DRIFT_DETECT, TYPE_BOOL, &value_u8, sizeof(value_u8));


    value_u16 = para->sleep_tim;
    config_service_set(CFG_SLEEP_TIME, TYPE_SHORT, &value_u16, sizeof(value_u16));

    value_u8 = para->shake_interval;
    config_service_set(CFG_SHAKE_CHECK_TIME, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u8_2 = BCD_LOW(para->protocol_type);
    value_u8 = BCD_HIGH(para->protocol_type);
    config_service_set(CFG_PROTOCOL, TYPE_BYTE, &value_u8, sizeof(value_u8));
    config_service_set(CFG_PROTOCOL_VER, TYPE_BYTE, &value_u8_2, sizeof(value_u8_2));

    value_u8 = para->jt_registered;
    config_service_set(CFG_JT_ISREGISTERED, TYPE_BOOL, &value_u8, sizeof(value_u8));

    value_u16 = para->chr_delay_tim;
    config_service_set(CFG_CHARGE_DELAY, TYPE_SHORT, &value_u16, sizeof(value_u16));

    value_u8 = para->ip_lock_mode;
    config_service_set(CFG_SERVERLOCK, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u8 = para->min_snr;
    config_service_set(CFG_MIN_SNR, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u16 = para->shake_alarm_timeout;
    config_service_set(CFG_SHAKE_ALARM_DELAY, TYPE_SHORT, &value_u16, sizeof(value_u16));
    
    value_u16 = para->shake_alarm_interval;
    config_service_set(CFG_SHAKE_ALARM_INTERVAL, TYPE_SHORT, &value_u16, sizeof(value_u16));

    
    value_u16 = para->defende_tim;
    config_service_set(CFG_AUTO_DEFENCE_DELAY, TYPE_SHORT, &value_u16, sizeof(value_u16));
    


    value_u16 = para->static_check_interval;
    config_service_set(CFG_STATIC_CHECK_TIME, TYPE_SHORT, &value_u16, sizeof(value_u16));

    value_u8 = para->static_check_speed;
    config_service_set(CFG_STATIC_CHECK_SPEED, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u8 = para->static_check_count;
    config_service_set(CFG_STATIC_CHECK_COUNT, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u16 = para->lbs_interval;
    config_service_set(CFG_LBS_INTERVAL, TYPE_SHORT, &value_u16, sizeof(value_u16));

    value_u16 = para->gps_not_fix_time;
    config_service_set(CFG_WAIT_GPS_TIME, TYPE_SHORT, &value_u16, sizeof(value_u16));

    value_u8 = para->reopen_gps_time;
    config_service_set(CFG_REOPEN_GSP_TIME, TYPE_BYTE, &value_u8, sizeof(value_u8));

    value_u16 = para->acc_check_interval;
    config_service_set(CFG_ACC_CHECK_TIME, TYPE_SHORT, &value_u16, sizeof(value_u16));


    value_float = para->sensor_threshold[THRESHOLD_INDEX_FOR_RAPID_ACLR];
	config_service_set(CFG_SEN_RAPID_ACLR, TYPE_FLOAT, &value_float, sizeof(value_float));

    value_float = para->sensor_threshold[THRESHOLD_INDEX_FOR_EMERGENCY_BRAKE];
	config_service_set(CFG_SEN_EMERGENCY_BRAKE, TYPE_FLOAT, &value_float, sizeof(value_float));

    value_float = para->sensor_threshold[THRESHOLD_INDEX_FOR_SLIGHT_COLLISION];
	config_service_set(CFG_SEN_SLIGHT_COLLISION, TYPE_FLOAT, &value_float, sizeof(value_float));

    value_float = para->sensor_threshold[THRESHOLD_INDEX_FOR_NORMAL_COLLISION];
	config_service_set(CFG_SEN_NORMAL_COLLISION, TYPE_FLOAT, &value_float, sizeof(value_float));

    value_float = para->sensor_threshold[THRESHOLD_INDEX_FOR_SERIOUS_COLLISION];
	config_service_set(CFG_SEN_SERIOUS_COLLISION, TYPE_FLOAT, &value_float, sizeof(value_float));

    value_float = para->sensor_threshold[THRESHOLD_INDEX_FOR_SUDDEN_TURN];
	config_service_set(CFG_SEN_SUDDEN_TURN, TYPE_FLOAT, &value_float, sizeof(value_float));

    value_float = para->sensor_threshold[THRESHOLD_INDEX_FOR_LOWVOlTAGE_ALARM];
	config_service_set(CFG_SEN_LOWVOlTAGE_ALARM, TYPE_FLOAT, &value_float, sizeof(value_float));

    value_float = para->sensor_threshold[THRESHOLD_INDEX_FOR_QUAKE_ALARM];
	config_service_set(CFG_SEN_QUAKE_ALARM, TYPE_FLOAT, &value_float, sizeof(value_float));

    value_float = para->sensor_threshold[THRESHOLD_INDEX_FOR_STATIC];
	config_service_set(CFG_SEN_STATIC, TYPE_FLOAT, &value_float, sizeof(value_float));

    value_float = para->sensor_threshold[THRESHOLD_INDEX_FOR_RUN];
	config_service_set(CFG_SEN_RUN, TYPE_FLOAT, &value_float, sizeof(value_float));

}

static bool check_address_para_ok(u8 * param)
{
    u8 addr[2*GOOME_DNS_MAX_LENTH];
    u8 IP[4];
    u32 port = 0;
    u8 idx = 0;

    GM_memset(addr, 0x00, sizeof(addr));
    idx = GM_sscanf((const char*)param, "%[^:]:%d", addr, &port);
    if (idx != 2)
    {
        LOG(INFO,"check_address_para_ok assert(2 fields(%s)) failed.", param);
        return false;
    }
    
    if(GM_SUCCESS != GM_ConvertIpAddr(addr, IP))
    {
        if(! util_is_valid_dns(addr, GM_strlen((const char *)addr)))
        {
            LOG(WARN,"check_address_para_ok assert(dns(%s)) failed.", addr);
            return false;
        }
    }
	return true;
}


static bool check_para_ok(const GprsParaFileType *para)
{
    u8 value_u8 = 0;
	u8 value_u8_2 = 0;
    u16 value_u16 = 0;
    u8 value_str[CONFIG_STRING_MAX_LEN];

    
    value_u16 = para->device_type;
	if(value_u16 <= DEVICE_NONE || value_u16 >= DEVICE_MAX)
	{
		return false;
	}

	value_str[0] = 0;
    if(util_is_valid_dns(para->sock[SOCKET_INDEX_MAIN].src_dns.dns, GM_strlen((const char*)para->sock[SOCKET_INDEX_MAIN].src_dns.dns)))
    {
        GM_snprintf((char*)value_str,sizeof(value_str), "%s:%d",(char*)para->sock[SOCKET_INDEX_MAIN].src_dns.dns, para->sock[SOCKET_INDEX_MAIN].src_dns.port);
		LOG(INFO,"domain:%s",value_str);
    }
    else if(util_is_valid_ip(para->sock[SOCKET_INDEX_MAIN].sock_ip[TCP_IP].IP, sizeof(para->sock[SOCKET_INDEX_MAIN].sock_ip[TCP_IP].IP)))
    {
        GM_snprintf((char*)value_str,sizeof(value_str), "%d.%d.%d.%d:%d",para->sock[SOCKET_INDEX_MAIN].sock_ip[TCP_IP].IP[0], para->sock[SOCKET_INDEX_MAIN].sock_ip[TCP_IP].IP[1],
            para->sock[SOCKET_INDEX_MAIN].sock_ip[TCP_IP].IP[2],para->sock[SOCKET_INDEX_MAIN].sock_ip[TCP_IP].IP[3],para->sock[SOCKET_INDEX_MAIN].sock_ip[TCP_IP].PORT);
		LOG(INFO,"ip:%s",value_str);
    }
	
	if(!check_address_para_ok(value_str))
	{
		return false;
	}
	
    value_u8_2 = BCD_LOW(para->protocol_type);
    value_u8 = BCD_HIGH(para->protocol_type);
	if(value_u8 >= PROTOCOL_MAX)
	{
		return false;
	}
	if(value_u8_2 >= PROTOCOL_APP_MAX)
	{
		return false;
	}
	return true;
}


