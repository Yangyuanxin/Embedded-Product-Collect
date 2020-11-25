/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        gps.h
 * Author:           王志华       
 * Version:          1.0
 * Date:             2019-03-19
 * Description:      使用agps_service、epo_service、config_service
 * Others:      
 * Function List:    
    1. 创建gps模块
    2. 销毁gps模块
    3. 模块定时处理入口
    4. 打开GPS模块
    5. 关闭GPS模块
    6. 获取最新GPS数据
 * History: 
    1. Date:         2019-03-19
       Author:       王志华
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#include <stdio.h>
#include <math.h>
#include <gm_type.h>
#include <gm_stdlib.h>
#include <gm_timer.h>
#include <gm_system.h>
#include "gps.h"
#include "applied_math.h"
#include "nmea_protocol.h"
#include "log_service.h"
#include "config_service.h"
#include "agps_service.h"
#include "uart.h"
#include "hard_ware.h"
#include "circular_queue.h"
#include "gps_service.h"
#include "g_sensor.h"
#include "system_state.h"
#include "led.h"
#include "gsm.h"
#include "utility.h"
#include "bms.h"


#define GPS_BUFF_LEN 50


typedef enum 
{	//未初始化
	GM_GPS_STATE_NONE = 0,
		
	//初始化OK
	GM_GPS_STATE_INITED,

	//发送了版本号查询,等待版本号响应
	GM_GPS_STATE_WAIT_VERSION,

	//发送了AGPS时间请求,等待AGPS时间响应
	GM_GPS_STATE_WAIT_APGS_TIME,
	
	//发送了AGPS数据请求,等待AGPS数据响应
	GM_GPS_STATE_WAIT_APGS_DATA,

	//工作中
	GM_GPS_STATE_WORKING,
	
}GPSInternalState;

typedef struct
{
	//使用的波特率
	U32 baud_rate;

	//GPS芯片型号
	GPSChipType gpsdev_type;

	//内部状态
	GPSInternalState internal_state;

	//整体状态
	GPSState state;

	//定位状态时间记录
    StateRecord state_record;

	//存放定位数据的环形队列
	CircularQueue gps_time_queue;
	CircularQueue gps_lat_queue;
	CircularQueue gps_lng_queue;
	CircularQueue gps_speed_queue;
	CircularQueue gps_course_queue;

	//启动时间
	time_t power_on_time;

	time_t mtk_start_time;

	time_t last_rcv_time;

	bool push_data_enbale;

	//AGPS完成注入时间
	U16 agps_time;

	//定位时间
	time_t fix_time;

	//内存中保存时间（每秒一条）
	time_t save_time;

	//上报平台时间
	time_t report_time;

	time_t sleep_time;

	//从上次上报点到当前点的距离（曲线距离）
	float distance_since_last_report;

	//是否已打开VTG语句（泰斗芯片需要打开VTG语句）
	bool has_opened_td_vtg;
	
    NMEASentenceVER frame_ver;
	NMEASentenceRMC frame_rmc;
	NMEASentenceGGA frame_gga;
	NMEASentenceGSA frame_gsa;
	NMEASentenceGST frame_gst;
	NMEASentenceGSV frame_gsv;
	NMEASentenceVTG frame_vtg;
	NMEASentenceZDA frame_zda;
	NMEASentenceGLL frame_gll;

	//辅助定位经纬度
	float ref_lng;
	float ref_lat;

	//定位状态
	//GPSState的第0位表示是否定位定位,0——未定位；1——已定位
	bool is_fix;
	
	//GPSState的第1位表示2D/3D定位,0——2D；1——3D
	bool is_3d;

	//GPSState的第2位表示是否差分定位定位,0——非差分定位；1——差分定位
	bool is_diff;

	//信号强度等级（0-100,100最高）
	U8 signal_intensity_grade;

	//水平方向定位精度[0.5,99.9]
	float hdop;

    //可见卫星数
	U8 satellites_tracked;

	//天线高度（相对于平均海平面,单位米）
    float altitude; 

	//最大信噪比(1分钟更新一次)
	U8 max_snr;

	//信号最好的5颗星
	SNRInfo snr_array[SNR_INFO_NUM];

	//可见卫星数(1秒更新一次)
	U8 satellites_inview;

	//snr大于35的卫星数,即我们认为可用的卫星数(1秒更新一次)
	U8 satellites_good;

	float aclr_avg_calculate_by_speed;
	U32 constant_speed_time;
	U32 static_speed_time;
	StateRecord over_speed_alarm_state;
}Gps,*PGps;

static Gps s_gps;
	
//GPS串口超过多少秒没有接收到数据关闭重新打开
#define GM_REOPEN_GPS_PORT_TIME 30

static void clear_data(void);

static void check_has_received_data(void);

static void set_device_type(const GPSChipType dev_type);

static void write_mtk_config(void);

//查询MTK版本号
static void query_mtk_version(void);

//知道9600波特率能收到数据,查询泰斗和中科微版本号
static void query_td_and_at_version(void);

static void query_td_version(void);

static void query_at_version(void);
	
static GM_ERRCODE write_mtk_epo_time(const ST_Time utc_time);

static void write_mtk_epo_pos(void);

static void open_td_vtg(void);

static GM_ERRCODE write_td_agps_time(const ST_Time utc_time,const U8 leap_sencond);

static void write_td_agps_pos(void);

static GM_ERRCODE write_at_agps_info(const ST_Time utc_time,const U8 leap_sencond,const float ref_lat,const float ref_lng);

static void on_received_gga(const NMEASentenceGGA gga);

static void on_received_gsa(const NMEASentenceGSA gsa);

static void on_received_fixed_state(const NMEAGSAFixType fix_type);

static on_received_gsv(const NMEASentenceGSV gsv);

static void on_received_rmc(const NMEASentenceRMC rmc);

static bool is_turn_point(const GPSData current_gps_data);

static GpsDataModeEnum upload_mode(const GPSData current_gps_data);

static void upload_gps_data(const GPSData current_gps_data);

static void check_fix_state(void);

static void check_fix_state_change(void);

static void calc_alcr_by_speed(GPSData gps_info);

static void check_over_speed_alarm(float speed);

static void reopen_gps(void);

static void request_write_time(void);





GM_ERRCODE gps_create(void)
{
	clear_data();
	s_gps.sleep_time = 0;
	circular_queue_create(&s_gps.gps_time_queue, GPS_BUFF_LEN, GM_QUEUE_TYPE_INT);
	circular_queue_create(&s_gps.gps_lat_queue, GPS_BUFF_LEN, GM_QUEUE_TYPE_FLOAT);
	circular_queue_create(&s_gps.gps_lng_queue, GPS_BUFF_LEN, GM_QUEUE_TYPE_FLOAT);
	circular_queue_create(&s_gps.gps_speed_queue, GPS_BUFF_LEN, GM_QUEUE_TYPE_FLOAT);
	circular_queue_create(&s_gps.gps_course_queue, GPS_BUFF_LEN, GM_QUEUE_TYPE_FLOAT);

	return GM_SUCCESS;
}

GM_ERRCODE gps_destroy(void)
{
	circular_queue_destroy(&s_gps.gps_time_queue, GM_QUEUE_TYPE_INT);
	circular_queue_destroy(&s_gps.gps_lat_queue, GM_QUEUE_TYPE_FLOAT);
	circular_queue_destroy(&s_gps.gps_lng_queue, GM_QUEUE_TYPE_FLOAT);
	circular_queue_destroy(&s_gps.gps_speed_queue, GM_QUEUE_TYPE_FLOAT);
	circular_queue_destroy(&s_gps.gps_course_queue, GM_QUEUE_TYPE_FLOAT);
	return GM_SUCCESS;
}


GM_ERRCODE gps_timer_proc(void)
{
	return GM_SUCCESS;
}

static void clear_data(void)
{
	s_gps.gpsdev_type = GM_GPS_TYPE_UNKNOWN;
	s_gps.baud_rate = BAUD_RATE_LOW;
	s_gps.internal_state = GM_GPS_STATE_INITED;
	s_gps.agps_time = 0;
	s_gps.state = GM_GPS_OFF;
	s_gps.state_record.state = false;
	s_gps.state_record.true_state_hold_seconds = 0;
	s_gps.state_record.false_state_hold_seconds = 0;
	s_gps.power_on_time = 0;
	s_gps.mtk_start_time = 0;
    s_gps.last_rcv_time = 0;
	s_gps.push_data_enbale = false;
	s_gps.fix_time = 0;
	s_gps.save_time = 0;
	s_gps.report_time = 0;
	s_gps.distance_since_last_report = 0;
	s_gps.has_opened_td_vtg = false;
	s_gps.ref_lng = 0;
	s_gps.ref_lat = 0;
	s_gps.is_fix = false;
	s_gps.is_3d = false;
	s_gps.is_diff = false;
	s_gps.signal_intensity_grade = 0;
	s_gps.hdop = 99.9;
	s_gps.satellites_tracked = 0;
	s_gps.max_snr = 0;
	GM_memset(s_gps.snr_array, 0, sizeof(s_gps.snr_array));
	s_gps.satellites_inview = 0;
	s_gps.satellites_good = 0;
	s_gps.altitude = 0;
	s_gps.aclr_avg_calculate_by_speed = 0;
	s_gps.constant_speed_time = 0;
	s_gps.static_speed_time = 0;
	s_gps.static_speed_time = 0;
	s_gps.over_speed_alarm_state.state = system_state_get_overspeed_alarm();
	s_gps.over_speed_alarm_state.true_state_hold_seconds = 0;
	s_gps.over_speed_alarm_state.false_state_hold_seconds = 0;
	
	GM_memset(&s_gps.frame_ver, 0, sizeof(NMEASentenceRMC));
	GM_memset(&s_gps.frame_rmc, 0, sizeof(NMEASentenceRMC));
	GM_memset(&s_gps.frame_gga, 0, sizeof(NMEASentenceGGA));
	GM_memset(&s_gps.frame_gst, 0, sizeof(NMEASentenceGST));
	GM_memset(&s_gps.frame_gsv, 0, sizeof(NMEASentenceGSV));
	GM_memset(&s_gps.frame_vtg, 0, sizeof(NMEASentenceVTG));
	GM_memset(&s_gps.frame_zda, 0, sizeof(NMEASentenceZDA));
	
}

GM_ERRCODE gps_power_on(bool push_data_enbale)
{
	GM_ERRCODE ret = GM_SUCCESS;
	bool gps_close = false;

	config_service_get(CFG_GPS_CLOSE, TYPE_BOOL, &gps_close, sizeof(gps_close));
    
	if (gps_close)
	{
		return GM_SUCCESS;
	}

	
	if (GM_GPS_OFF != s_gps.state)
	{
        hard_ware_open_gps();
		return GM_SUCCESS;
	}
	else
	{
		clear_data();
	}

	s_gps.push_data_enbale = push_data_enbale;

	//先打开调试串口
	uart_open_port(GM_UART_DEBUG,BAUD_RATE_HIGH,0);
	GM_SysMsdelay(10);

	s_gps.state = GM_GPS_FIX_NONE;
	led_set_gps_state(GM_LED_FLASH);
	s_gps.power_on_time = util_clock();
	
	s_gps.gpsdev_type = config_service_get_gps_type();

	switch (s_gps.gpsdev_type)
	{
		
		//型号未知,先用9600波特率打开,1秒后检查,如果不成功再以115200波特率打开
		case GM_GPS_TYPE_UNKNOWN:
		{
			s_gps.baud_rate = BAUD_RATE_LOW;
			ret = uart_open_port(GM_UART_GPS, s_gps.baud_rate, GM_REOPEN_GPS_PORT_TIME);
			if(GM_SUCCESS != ret)
			{
				LOG(FATAL,"Failed to open GPS uart,ret=%d", ret);
			}
			
			GM_StartTimer(GM_TIMER_GPS_QUERY_TD_AT_VERSION, TIM_GEN_1SECOND, query_td_and_at_version);
	        s_gps.internal_state = GM_GPS_STATE_WAIT_VERSION;
		}
		break;

		//MTK内置GPS,115200波特率
		case GM_GPS_TYPE_MTK_EPO:
		{
			s_gps.baud_rate = BAUD_RATE_HIGH;
			ret = uart_open_port(GM_UART_GPS, s_gps.baud_rate, GM_REOPEN_GPS_PORT_TIME);
			if (GM_SUCCESS != ret)
			{
				LOG(FATAL,"Failed to open GPS uart,ret=%d", ret);
			}
			
			GM_StartTimer(GM_TIMER_GPS_QUERY_MTK_VERSION, TIM_GEN_1SECOND, query_mtk_version);
			s_gps.internal_state = GM_GPS_STATE_WAIT_VERSION;
		}
		break;

		//泰斗,9600波特率
		case GM_GPS_TYPE_TD_AGPS:
		{
			s_gps.baud_rate = BAUD_RATE_LOW;
			ret = uart_open_port(GM_UART_GPS, s_gps.baud_rate, GM_REOPEN_GPS_PORT_TIME);
			if (GM_SUCCESS != ret)
			{
				LOG(FATAL,"Failed to open GPS uart,ret=%d", ret);
			}
			
			GM_StartTimer(GM_TIMER_GPS_QUERY_TD_VERSION, TIM_GEN_1SECOND, query_td_version);
			s_gps.internal_state = GM_GPS_STATE_WAIT_VERSION;
		}
		break;

		//中科微,9600波特率
		case GM_GPS_TYPE_AT_AGPS:
		{
			s_gps.baud_rate = BAUD_RATE_LOW;
			ret = uart_open_port(GM_UART_GPS, s_gps.baud_rate, GM_REOPEN_GPS_PORT_TIME);
			if (GM_SUCCESS != ret)
			{
				LOG(FATAL,"Failed to open GPS uart,ret=%d", ret);
			}
			
			GM_StartTimer(GM_TIMER_GPS_QUERY_AT_VERSION, TIM_GEN_1SECOND, query_at_version);
			s_gps.internal_state = GM_GPS_STATE_WAIT_VERSION;
		}
		break;

		default:
		{
			
		}
		break;
		
	}

    hard_ware_awake();
	
	GM_StartTimer(GM_TIMER_GPS_CHECK_RECEIVED, TIM_GEN_1SECOND*10, check_has_received_data);
	GM_StartTimer(GM_TIMER_GPS_CHECK_STATE, TIM_GEN_1SECOND*60, check_fix_state);
	return hard_ware_open_gps();
}

static void check_fix_state(void)
{
	JsonObject* p_log_root = NULL;
	char snr_str[128] = {0};
	if (s_gps.state < GM_GPS_FIX_3D)
	{
		if (false == system_state_has_reported_lbs_since_boot())
		{
			gps_service_push_lbs();
			system_state_set_has_reported_lbs_since_boot(true);
		}
		p_log_root = json_create();
		json_add_string(p_log_root, "event", "unfixed");
		json_add_int(p_log_root, "AGPS time", s_gps.agps_time);
		json_add_int(p_log_root, "satellites_in_view", s_gps.satellites_inview);
		json_add_int(p_log_root, "satellites_good", s_gps.satellites_good);
		json_add_int(p_log_root, "satellites_tracked", s_gps.satellites_tracked);

		GM_snprintf(snr_str,128,"%03d:%02d,%03d:%02d,%03d:%02d,%03d:%02d,%03d:%02d,",
			s_gps.snr_array[0].prn,s_gps.snr_array[0].snr,
			s_gps.snr_array[1].prn,s_gps.snr_array[1].snr,
			s_gps.snr_array[2].prn,s_gps.snr_array[2].snr, 
			s_gps.snr_array[3].prn,s_gps.snr_array[3].snr,
			s_gps.snr_array[4].prn,s_gps.snr_array[4].snr);

		json_add_string(p_log_root, "snr", snr_str);
		json_add_int(p_log_root, "csq", gsm_get_csq());

		//泰斗可能会出现AGPS数据无效的情况，导致发送几个数据包以后收不到芯片确认，需要重新下载AGPS数据
		if (GM_GPS_TYPE_TD_AGPS == s_gps.gpsdev_type && 0 == s_gps.agps_time && GM_GPS_STATE_WAIT_APGS_DATA == s_gps.internal_state)
		{
			//s_gps.internal_state = GM_GPS_STATE_WAIT_APGS_DATA;
			//agps_service_require_to_gps(AGPS_TO_GPS_DATA,true);
			json_add_string(p_log_root, "AGPS data", "invalid");
		}
		log_service_upload(INFO,p_log_root);
	}
	
}

static void reopen_gps(void)
{
	bool push_data_enbale = s_gps.push_data_enbale;
	JsonObject* p_log_root = json_create();

	LOG(INFO,"reopen_gps");
	
	hard_ware_close_gps();
	clear_data();
	s_gps.push_data_enbale = push_data_enbale;
	GM_SysMsdelay(100);
	hard_ware_open_gps();
	s_gps.internal_state = GM_GPS_STATE_WAIT_VERSION;
	s_gps.power_on_time = util_clock();
	GM_StartTimer(GM_TIMER_GPS_CHECK_RECEIVED, TIM_GEN_1SECOND*1, check_has_received_data);
	GM_StartTimer(GM_TIMER_GPS_CHECK_STATE, TIM_GEN_1SECOND*60, check_fix_state);
	
	
	json_add_string(p_log_root, "event", "reopen_gps");
	json_add_int(p_log_root, "AGPS time late", util_clock() - s_gps.mtk_start_time);
	log_service_upload(INFO,p_log_root);
	return;
}

GM_ERRCODE gps_write_agps_info(const float lng,const float lat,const U8 leap_sencond,const U32 data_start_time)
{
	ST_Time utc_time;
	GM_ERRCODE ret = GM_SUCCESS;
    U8 reopen_gps_time = 0;

	//如果GPS芯片启动超过10秒了，AGPS才准备好，那么需要重启GPS芯片AGPS才有用
    config_service_get(CFG_REOPEN_GSP_TIME, TYPE_BYTE, &reopen_gps_time, sizeof(reopen_gps_time));
    //如果读取的配置值很小，恢复默认值
    if(reopen_gps_time < 5 )
    {
        reopen_gps_time = 9;
        config_service_set(CFG_REOPEN_GSP_TIME, TYPE_BYTE, &reopen_gps_time, sizeof(reopen_gps_time));
    }

	if((util_clock() - s_gps.mtk_start_time) > reopen_gps_time && GM_GPS_OFF != s_gps.state && s_gps.gpsdev_type == GM_GPS_TYPE_MTK_EPO)
	{
		LOG(INFO,"reopen_gps,clock=%d,mtk_start_time=%d,reopen_gps_time=%d,state=%d",util_clock(),s_gps.mtk_start_time,reopen_gps_time,s_gps.state);
		reopen_gps();
		return GM_ERROR_STATUS;
	}
	else
	{
		LOG(INFO,"clock=%d,power_on_time=%d,reopen_gps_time=%d,state=%d",util_clock(),s_gps.power_on_time,reopen_gps_time,s_gps.state);
	}

	//获取UTC时间
    GM_GetLocalTime(&utc_time, 0);
    
	//经纬度先保存,MTK和TD要等会再用
	s_gps.ref_lng = lng;
	s_gps.ref_lat = lat;
	
	switch (s_gps.gpsdev_type)
	{
		case GM_GPS_TYPE_MTK_EPO:
		{
			LOG(INFO,"gps_write_agps_info,write_mtk_epo_time");
			ret = write_mtk_epo_time(utc_time);
		}
			break;
		case GM_GPS_TYPE_TD_AGPS:
		{
			LOG(INFO,"gps_write_agps_info,write_td_agps_time");
			ret = write_td_agps_time(utc_time, leap_sencond);
		}
			break;
		case GM_GPS_TYPE_AT_AGPS:
		{	
			LOG(INFO,"gps_write_agps_info,write_at_agps_info");
			ret = write_at_agps_info(utc_time, leap_sencond, lat, lng);
		}
			break;
		default:
		{
			LOG(ERROR,"gps_write_agps_info,dev_type=%d,",s_gps.gpsdev_type);
			return GM_ERROR_STATUS;
		}
	}
	return ret;
}

/**
 * Function:   写入agps（epo）数据
 * Description:
 * Input:	   seg_index:分片ID；p_data:数据指针；len:数据长度
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE gps_write_agps_data(const U16 seg_index, const U8* p_data, const U16 len)
{
	GM_ERRCODE ret = GM_SUCCESS;
	if (s_gps.internal_state != GM_GPS_STATE_WAIT_APGS_DATA)
	{
		LOG(ERROR,"Could not write APGS data,current state=%d",s_gps.internal_state);
		return GM_ERROR_STATUS;
	}
	else
	{
		LOG(INFO,"gps_write_agps_data,seg_index=%d,len=%d",seg_index,len);
		//LOG_HEX((const char *)p_data,len);
	}
	
	switch (s_gps.gpsdev_type)
	{
		case GM_GPS_TYPE_MTK_EPO:
		{	
			U8 epo_sentence[300] = {0};
			U16 sentence_len = 300;
			
			nmea_creat_mtk_epo_sentence(seg_index,p_data,len,epo_sentence,&sentence_len);
			ret = uart_write(GM_UART_GPS, epo_sentence, sentence_len);
			//LOG(INFO,(char*)epo_sentence);
			if (GM_SUCCESS != ret)
			{
				LOG(ERROR, "Failed to write EPO data,ret=%d", ret);
			}		
		}
		break;
		
		case GM_GPS_TYPE_TD_AGPS:
		{
			ret = uart_write(GM_UART_GPS, p_data, len);
			if (GM_SUCCESS != ret)
			{
				LOG(ERROR, "Failed to write AGPS data,ret=%d,len=%d", ret,len);
			}
		}
		break;
		
		case GM_GPS_TYPE_AT_AGPS:
		{
			ret = uart_write(GM_UART_GPS, p_data, len);
			if (GM_SUCCESS != ret)
			{
				LOG(ERROR, "Failed to write AGPS data,ret=%d,len=%d", ret,len);
			}
		}
		break;
		
		default:
		{
			LOG(ERROR, "Unknown device type!");
		}
	}
	return GM_SUCCESS;
}

GM_ERRCODE gps_power_off(void)
{
	s_gps.state = GM_GPS_OFF;
	GM_StopTimer(GM_TIMER_GPS_CHECK_STATE);
	GM_StopTimer(GM_TIMER_GPS_CHECK_RECEIVED);
	uart_close_port(GM_UART_GPS);
	led_set_gps_state(GM_LED_OFF);
	
	uart_close_port(GM_UART_DEBUG);
	hard_ware_close_gps();
	hard_ware_sleep();
	s_gps.sleep_time = util_clock();

	bms_destroy();
	return GM_SUCCESS;
}

GPSState gps_get_state(void)
{
	return s_gps.state;
}

bool gps_is_fixed(void)
{
	if (GM_GPS_FIX_3D > gps_get_state() || GM_GPS_OFF == gps_get_state())
	{
		return false;
	}
	else
	{
		return true;
	}
}

U16 gps_get_fix_time(void)
{
	return s_gps.fix_time;
}

U8 gps_get_max_snr(void)
{
	return s_gps.max_snr;
}

U8 gps_get_satellites_tracked(void)
{
	return s_gps.satellites_tracked;
}

U8 gps_get_satellites_inview(void)
{
	return s_gps.satellites_inview;
}

U8 gps_get_satellites_good(void)
{
	return s_gps.satellites_good;
}
const SNRInfo* gps_get_snr_array(void)
{
	return s_gps.snr_array;
}

bool gps_get_last_data(GPSData* p_data)
{
	return gps_get_last_n_senconds_data(0,p_data);
}

/**
 * Function:   获取最近n秒的GPS数据
 * Description:
 * Input:	   seconds:第几秒,从0开始
 * Output:	   p_data:指向定位数据的指针
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
bool gps_get_last_n_senconds_data(U8 seconds,GPSData* p_data)
{
	if (NULL == p_data)
	{
		return false;
	}
	else
	{
		if(!circular_queue_get_by_index_i(&s_gps.gps_time_queue, seconds, (S32 *)&p_data->gps_time))
		{
				return false;
		}
		if(!circular_queue_get_by_index_f(&s_gps.gps_lat_queue, seconds, &p_data->lat))
		{
				return false;
		}
		if(!circular_queue_get_by_index_f(&s_gps.gps_lng_queue, seconds, &p_data->lng))
		{
				return false;
		}
		if(!circular_queue_get_by_index_f(&s_gps.gps_speed_queue, seconds, &p_data->speed))
		{
				return false;
		}
		if(!circular_queue_get_by_index_f(&s_gps.gps_course_queue, seconds, &p_data->course))
		{
				return false;
		}
        p_data->satellites = s_gps.satellites_tracked;
		p_data->precision = s_gps.hdop;
		p_data->signal_intensity_grade = s_gps.signal_intensity_grade;
		return true;
	}
}


static void check_has_received_data(void)
{
    //如果串口还没收到数据
	if (s_gps.internal_state <= GM_GPS_STATE_WAIT_VERSION)
	{
        JsonObject* p_log = json_create();
        json_add_string(p_log, "event", "Open GPS failed!");
        json_add_int(p_log, "power_on_time", s_gps.power_on_time);
        json_add_int(p_log, "last_rcv_time", s_gps.last_rcv_time);
        log_service_upload(ERROR, p_log);
        
		hard_ware_close_gps();
		uart_close_port(GM_UART_GPS);
		GM_SysMsdelay(100);

		//如果型号未知，更换波特率
		if(GM_GPS_TYPE_UNKNOWN == s_gps.gpsdev_type)
		{
			if (BAUD_RATE_HIGH == s_gps.baud_rate)
			{
				s_gps.baud_rate = BAUD_RATE_LOW;
			}
			else
			{
				s_gps.baud_rate = BAUD_RATE_HIGH;
			}
		}

		//连续10分钟未收到数据重启,否则重新打开GPS和串口继续检查是否收到数据
		if((util_clock() - s_gps.last_rcv_time) >= 10*SECONDS_PER_MIN && (util_clock() - s_gps.power_on_time) > 10*SECONDS_PER_MIN)
		{
			hard_ware_reboot(GM_REBOOT_GPS,1);
			return;
		}
		else
		{
			GM_ERRCODE ret = uart_open_port(GM_UART_GPS, s_gps.baud_rate, GM_REOPEN_GPS_PORT_TIME);
			if(GM_SUCCESS != ret)
			{
				LOG(FATAL,"Failed to open GPS uart,ret=%d", ret);
				return;
			}
			else
			{	
				LOG(INFO,"Reopen UART with %d.",s_gps.baud_rate);
				query_mtk_version();
			}
			hard_ware_open_gps();
			GM_StartTimer(GM_TIMER_GPS_CHECK_RECEIVED, TIM_GEN_1SECOND*5, check_has_received_data);
		}
		
	}
	else
	{
		LOG(DEBUG,"Internal state:%d",s_gps.internal_state);
	}
}

static void set_device_type(const GPSChipType dev_type)
{
	if (dev_type != s_gps.gpsdev_type)
	{
		s_gps.gpsdev_type = dev_type;
		config_service_set(CFG_GPS_TYPE, TYPE_BYTE, &dev_type, sizeof(U8));
		config_service_save_to_local();
		LOG(INFO,"Save GPS type to local:type=%d",dev_type);
	}
}

static void write_mtk_config(void)
{
	U8 min_snr = 0;
	char mtk_cmd[30] = {0};
	config_service_get(CFG_MIN_SNR, TYPE_BYTE, &min_snr, sizeof(min_snr));
	if(system_state_is_cold_boot())
	{
		gps_write_mtk_cmd("PMTK104");
		system_state_set_cold_boot(false);
	}

	//GM_SysMsdelay(10);
	
	//Enable fast TTFF or high accuracy function when out of the tunnel or garage. (Default enabled high accuracy function). 
    //'0' = Enable fast TTFF when out of the tunnel or garage 
    //'1' = Enable high accuracy when out of the tunnel or garage 
	//gps_write_mtk_cmd("PMTK,257,0");

	//GM_SysMsdelay(10);

	//HACC， Range from 30m to 200m or -1. GPS will get fix only when hacc value < mask.
	//gps_write_mtk_cmd("PMTK,328,-1");

	//GM_SysMsdelay(10);

	//HDOP_THRESHOLD: If the HDOP value is larger than this threshold value, the position will not be fixed.  
	//"0": Disable this function.
    //Other value: Enable set the HDOP threshold
	//gps_write_mtk_cmd("PMTK356,0");

	//GM_SysMsdelay(10);

	//Enable SBAS
	//gps_write_mtk_cmd("PMTK313,1");

	GM_SysMsdelay(10);

	GM_snprintf(mtk_cmd, 30, "PMTK306,%d",min_snr);
	gps_write_mtk_cmd(mtk_cmd);

	//GM_SysMsdelay(10);

	//$PMTK353,1,0,0,0,1*2B : Search GPS and BEIDOU satellites.
	//gps_write_mtk_cmd("PMTK353,1,0,0,0,1");

	LOG(INFO,"write_mtk_config");
}

static void request_write_time(void)
{
	s_gps.internal_state = GM_GPS_STATE_WAIT_APGS_TIME;
	agps_service_require_to_gps(AGPS_TO_GPS_LNGLAT_TIME,false);
	LOG(INFO,"request_mtk_time");
}
void gps_on_rcv_uart_data(char* p_data, const U16 len)
{
	bool has_agps_data_left = false;
	s_gps.last_rcv_time = util_clock();
	
	switch (nmea_sentence_id(p_data,len, false)) 
	{
		case NMEA_SENTENCE_TXT:
		{
			
			if (nmea_parse_txt(&s_gps.frame_ver, p_data)) 
			{
	        	LOG(INFO,"AT GPS version:%s",s_gps.frame_ver.ver);
				set_device_type(GM_GPS_TYPE_AT_AGPS);
				if (s_gps.internal_state < GM_GPS_STATE_WAIT_APGS_TIME)
				{
					request_write_time();
					LOG(DEBUG,"NMEA_SENTENCE_TXT");
				}	
	        }
	        else
			{
	            //LOG(DEBUG,"$TXT sentence is not parsed:%s",p_data);
	        }	
		}
			break;
		case NMEA_SENTENCE_INF:
		{
			if (nmea_parse_inf(&s_gps.frame_ver, p_data)) 
			{
				LOG(INFO,"TD GPS version:%s",s_gps.frame_ver.ver);
				set_device_type(GM_GPS_TYPE_TD_AGPS);
				if (s_gps.internal_state < GM_GPS_STATE_WAIT_APGS_TIME)
				{
					request_write_time();
					LOG(DEBUG,"NMEA_SENTENCE_INF");
				}	   
            }
            else
			{
                //LOG(DEBUG,"$INF sentence is not parsed");
            }
		}
			break;
		
		// MTK AGPS ACK,确认包
		case NMEA_SENTENCE_MTK_ACK:    
		{
			NMEASentenceMTKACK mtk_ack;
			if (nmea_parse_mtk_ack(&mtk_ack, p_data)) 
			{
				if(MTK_TIME_ACK == mtk_ack.ack_type)
				{
					write_mtk_epo_pos();
					LOG(INFO,"Received MTK_TIME_ACK");
					
					/*if(MTK_ACTION_SUCCEED == mtk_ack.ack_result)
					{
						write_mtk_epo_pos();
						LOG(INFO,"Received MTK_TIME_ACK");
					}
					else if(MTK_ACTION_FAILED == mtk_ack.ack_result)
					{
						LOG(INFO,"Received MTK_TIME_ACTION_FAILED");
						reopen_gps();
					}
					else
					{
						LOG(WARN,"Failed to action MTK command,send cmd again.");
						GM_StartTimer(GM_TIMER_GPS_RESEND_TIME, TIM_GEN_1SECOND, request_write_time);
					}*/
				}
				else if(MTK_POS_ACK == mtk_ack.ack_type)
				{
					s_gps.internal_state = GM_GPS_STATE_WAIT_APGS_DATA;
					agps_service_require_to_gps(AGPS_TO_GPS_DATA,false);
					LOG(INFO,"Received MTK_POS_ACK");
				}
				else if(MTK_DATA_ACK == mtk_ack.ack_type)
				{
					LOG(INFO,"Received MTK_DATA_ACK");
					if (GM_GPS_STATE_WAIT_APGS_DATA == s_gps.internal_state)
					{
						has_agps_data_left = agps_service_require_to_gps(AGPS_TO_GPS_DATA,false);
						if (false == has_agps_data_left)
						{
							s_gps.internal_state = GM_GPS_STATE_WORKING;
							s_gps.agps_time = util_clock() - s_gps.power_on_time;
							LOG(INFO,"MTK Start working.........................");
						}
					}
				}
				else if(MTK_TTFF_ACK == mtk_ack.ack_type)
				{
					LOG(INFO,"Enable TIFF");
				}
				else
				{	
					LOG(INFO,"MTK ack type:%d,%s",mtk_ack.ack_type,p_data);
				}
            }
            else
			{
                LOG(DEBUG,"sentence is not parsed:%s",p_data);
            }
		}
			break;
		
		// MTK GPS work OK,可以发送EPO
    	case NMEA_SENTENCE_MTK_START:  
    	{
    		NMEASentenceStart frame;
    		if(nmea_parse_mtk_start(&frame, p_data) && frame.system_message_type == 2)
			{
				s_gps.mtk_start_time = util_clock();
				if (s_gps.internal_state < GM_GPS_STATE_WAIT_APGS_TIME)
				{
					write_mtk_config();
					set_device_type(GM_GPS_TYPE_MTK_EPO);
					request_write_time();
					LOG(INFO,"NMEA_SENTENCE_MTK_START");
				}	
			}
			else
			{
                LOG(DEBUG,"sentence is not parsed:%s",p_data);
            }
		}
			break;
		
		// MTK GPS version,版本号
    	case NMEA_SENTENCE_MTK_VER:   
    	{
			if (nmea_parse_mtk_ver(&s_gps.frame_ver,p_data))
			{
				LOG(INFO,"Received MTK version:%s",s_gps.frame_ver.ver);
			}
			else
			{
                LOG(DEBUG,"sentence is not parsed:%s",p_data);
            }
		}
			break;
		
    	case NMEA_SENTENCE_TD_ACK:
		{
			U16 ack_cmd = 0;
			
			if (nmea_parse_td_ack(&ack_cmd, p_data,len)) 
			{
				if(TD_AID_TIME_CMD == ack_cmd)
				{
					write_td_agps_pos();
				}
				else if(TD_AID_POS_CMD == ack_cmd)
				{
					s_gps.internal_state = GM_GPS_STATE_WAIT_APGS_DATA;
					agps_service_require_to_gps(AGPS_TO_GPS_DATA,false);
				}
				else if ( TD_AID_BDSEPH_CMD == ack_cmd || TD_AID_BDSALM_CMD == ack_cmd || TD_AID_GPSEPH_CMD == ack_cmd || TD_AID_GPSALM_CMD == ack_cmd)
				{
					if (GM_GPS_STATE_WAIT_APGS_DATA == s_gps.internal_state)
					{
						has_agps_data_left = agps_service_require_to_gps(AGPS_TO_GPS_DATA,false);
						if (false == has_agps_data_left)
						{
							s_gps.internal_state = GM_GPS_STATE_WORKING;
							s_gps.agps_time = util_clock() - s_gps.power_on_time;
							LOG(DEBUG,"TD Start working.........................");
						}
					}	
				}
				else if(TD_OPEN_FUNC_CMD == ack_cmd)
				{
					s_gps.has_opened_td_vtg = true;
				}
				else
				{	
					LOG(ERROR,"Unknown TD ack type:%d",ack_cmd);
				}
            }
            else
			{
                LOG(WARN,"sentence is not parsed!");
            }
		}
			break;
		
		// TD GPS version,版本号
    	case NMEA_SENTENCE_TD_VER:   
    	{
    		
			if (nmea_parse_td_ver(&s_gps.frame_ver, p_data,len)) 
			{
                //LOG(DEBUG,"$TDVER: %s",s_gps.frame_ver.ver);
				set_device_type(GM_GPS_TYPE_TD_AGPS);
				if (s_gps.internal_state < GM_GPS_STATE_WAIT_APGS_TIME)
				{
					LOG(DEBUG,"NMEA_SENTENCE_TD_VER");
					request_write_time();
					open_td_vtg();
				}	
            }
            else
			{
                LOG(WARN,"$TDVER sentence is not parsed");
            }
		}
			break;
		
    	case NMEA_SENTENCE_AT_ACK:
		{
			U16 ack_cmd = 0;
			
			if (nmea_parse_at_ack(&ack_cmd, p_data,len)) 
			{
				if(AT_AID_TIMEPOS_CMD == ack_cmd)
				{
					s_gps.internal_state = GM_GPS_STATE_WAIT_APGS_DATA;
					agps_service_require_to_gps(AGPS_TO_GPS_DATA,false);
				}
				else if(AT_AID_BDSEPH_CMD == ack_cmd || AT_AID_GPSEPH_CMD == ack_cmd || AT_AID_GLNEPH_CMD == ack_cmd)
				{
					if (GM_GPS_STATE_WAIT_APGS_DATA == s_gps.internal_state)
					{
						s_gps.internal_state = GM_GPS_STATE_WORKING;
						s_gps.agps_time = util_clock() - s_gps.power_on_time;
						LOG(DEBUG,"AT Start working.........................");
					}
				}
				else
				{	
					LOG(WARN,"Unknown AT ack cmd:%d",ack_cmd);
				}
            }
            else
			{
                LOG(WARN,"AT_ACK sentence is not parsed!");
            }
		}
			break;
		case NMEA_SENTENCE_AT_NACK:
		{
			U16 ack_cmd = 0;
    		
			if (nmea_parse_at_ack(&ack_cmd, p_data,len)) 
			{
				if(AT_AID_TIMEPOS_CMD == ack_cmd)
				{
					LOG(ERROR,"The time and position packet is wrong!");
				}
				else if(AT_AID_BDSEPH_CMD == ack_cmd || AT_AID_GPSEPH_CMD == ack_cmd || AT_AID_GLNEPH_CMD == ack_cmd)
				{
					LOG(WARN,"The AGPS data packet is wrong!");
				}
				else
				{	
					LOG(WARN,"Unknown AT nack cmd:%d",ack_cmd);
				}
            }
            else
			{
                LOG(WARN,"AT_NACK sentence is not parsed!");
            }
		}
			break;
		
		// AT GPS version,版本号
    	case NMEA_SENTENCE_AT_VER:   
    	{
			if (nmea_parse_at_ver(&s_gps.frame_ver, p_data,len)) 
			{
				set_device_type(GM_GPS_TYPE_AT_AGPS);
				if (s_gps.internal_state < GM_GPS_STATE_WAIT_APGS_TIME)
				{
					LOG(DEBUG,"NMEA_SENTENCE_AT_VER");
					request_write_time();
				}	
            }
            else
			{
                LOG(WARN,"$ATVER sentence is not parsed");
            }
		}
			break;
			
        case NMEA_SENTENCE_RMC: 
		{
    		
            if (nmea_parse_rmc(&s_gps.frame_rmc, p_data)) 
			{
                //LOG(DEBUG,"$RMC: raw coordinates,speed and course: (%d/%d,%d/%d), %d/%d,%d/%d",
                        //s_gps.frame_rmc.latitude.value, s_gps.frame_rmc.latitude.scale,
                        //s_gps.frame_rmc.longitude.value, s_gps.frame_rmc.longitude.scale,
                        //s_gps.frame_rmc.speed.value, s_gps.frame_rmc.speed.scale,
                        //s_gps.frame_rmc.course.value, s_gps.frame_rmc.course.scale);
 
				
				on_received_rmc(s_gps.frame_rmc);
            }
            else
			{
                LOG(WARN,"$RMC sentence is not parsed");
            }
        } 
			break;

		case NMEA_SENTENCE_GSA:
		{
    		
            if (nmea_parse_gsa(&s_gps.frame_gsa, p_data)) 
			{
                //LOG(DEBUG,"$GSA: %c,%d,%f,%f,%f", 
					//s_gps.frame_gsa.mode,
    				//s_gps.frame_gsa.fix_type,
   					//nmea_tofloat(&s_gps.frame_gsa.pdop),
   					//nmea_tofloat(&s_gps.frame_gsa.hdop),
   					//nmea_tofloat(&s_gps.frame_gsa.vdop));
				on_received_gsa(s_gps.frame_gsa);
            }
            else 
			{
                LOG(WARN,"$GSA sentence is not parsed");
            }
        } 
			break;
		
        case NMEA_SENTENCE_GGA: 
		{
    		
            if (nmea_parse_gga(&s_gps.frame_gga, p_data)) 
			{
                //LOG(DEBUG,"$GGA: fix quality: %d", s_gps.frame_gga.fix_quality);
				on_received_gga(s_gps.frame_gga);
            }
            else 
			{
                LOG(WARN,"$GGA sentence is not parsed");
            }
        } 
			break;

        
		case NMEA_SENTENCE_GSV: 
		{	
            if (nmea_parse_gsv(&s_gps.frame_gsv, p_data))
			{ 
                on_received_gsv(s_gps.frame_gsv);
            }
            else 
			{
                LOG(WARN,"$XXGSV sentence is not parsed");
            }
        } 
        	break;

		case NMEA_SENTENCE_BDGSV:
		{	
			U8 index_sate = 0;
            if (nmea_parse_gsv(&s_gps.frame_gsv, p_data))
            {
				LOG(DEBUG,"msg_number:%d,total_msgs:%d",s_gps.frame_gsv.msg_number,s_gps.frame_gsv.total_msgs);
				for (index_sate = 0; index_sate < 4; index_sate++)
				{
					LOG(DEBUG,"$BDGSV: sat nr %d, elevation: %d, azimuth: %d, snr: %d dbm",
						s_gps.frame_gsv.satellites[index_sate].nr,
						s_gps.frame_gsv.satellites[index_sate].elevation,
						s_gps.frame_gsv.satellites[index_sate].azimuth,
						s_gps.frame_gsv.satellites[index_sate].snr);
	            }
            }
		}
			break;
        /*
        case NMEA_SENTENCE_GST: 
		{
    		
            if (nmea_parse_gst(&s_gps.frame_gst, p_data)) 
			{
                //LOG(DEBUG,"$GST: raw latitude,longitude and altitude error deviation: (%d/%d,%d/%d,%d/%d)",
                        //s_gps.frame_gst.latitude_error_deviation.value, s_gps.frame_gst.latitude_error_deviation.scale,
                        //s_gps.frame_gst.longitude_error_deviation.value, s_gps.frame_gst.longitude_error_deviation.scale,
                        //s_gps.frame_gst.altitude_error_deviation.value, s_gps.frame_gst.altitude_error_deviation.scale);
                //LOG(DEBUG,"$GST fixed point latitude,longitude and altitude error deviation scaled to one decimal place: (%d,%d,%d)",
                        //nmea_rescale(&s_gps.frame_gst.latitude_error_deviation, 10),
                        //nmea_rescale(&s_gps.frame_gst.longitude_error_deviation, 10),
                        //nmea_rescale(&s_gps.frame_gst.altitude_error_deviation, 10));
                //LOG(DEBUG,"$GST floating point degree latitude, longitude and altitude error deviation: (%f,%f,%f)",
                        //nmea_tofloat(&s_gps.frame_gst.latitude_error_deviation),
                        //nmea_tofloat(&s_gps.frame_gst.longitude_error_deviation),
                        //nmea_tofloat(&s_gps.frame_gst.altitude_error_deviation));
            }
            else 
			{
                LOG(WARN,"$GST sentence is not parsed");
            }
        } 
			break;

        

        case NMEA_SENTENCE_VTG: 
		{
    		
           if (nmea_parse_vtg(&s_gps.frame_vtg, p_data)) 
		   {
                //LOG(DEBUG,"$VTG: true track degrees = %f",nmea_tofloat(&s_gps.frame_vtg.true_track_degrees));
                //LOG(DEBUG,"magnetic track degrees = %f",nmea_tofloat(&s_gps.frame_vtg.magnetic_track_degrees));
                //LOG(DEBUG,"speed knots = %f",nmea_tofloat(&s_gps.frame_vtg.speed_knots));
                //LOG(DEBUG,"speed kph = %f",nmea_tofloat(&s_gps.frame_vtg.speed_kph));
           }
           else 
		   {
                //LOG(WARN,"$VTG sentence is not parsed");
           }
        } 
		   break;
		
		
        case NMEA_SENTENCE_ZDA:
		{
    		
            if (nmea_parse_zda(&s_gps.frame_zda, p_data)) 
			{
                //LOG(DEBUG,"$ZDA: %d:%d:%d %02d.%02d.%d UTC%+03d:%02d",
                       //s_gps.frame_zda.time.hours,
                       //s_gps.frame_zda.time.minutes,
                       //s_gps.frame_zda.time.seconds,
                       //s_gps.frame_zda.date.day,
                       //s_gps.frame_zda.date.month,
                       //s_gps.frame_zda.date.year,
                       //s_gps.frame_zda.hour_offset,
                       //s_gps.frame_zda.minute_offset);
            }
            else 
			{
                LOG(WARN,"$ZDA sentence is not parsed");
            }
        } 
			break;
		
		case NMEA_SENTENCE_GLL:
		{
    		
            if (nmea_parse_gll(&s_gps.frame_gll, p_data)) 
			{
                //LOG(DEBUG,"$GLL: %d:%d:%d latitude:%f,longitude:%f,status:%d,mode:%d",
                       //s_gps.frame_gll.time.hours,
                       //s_gps.frame_gll.time.minutes,
                       //s_gps.frame_gll.time.seconds,
                       //nmea_tofloat(&s_gps.frame_gll.latitude),
                       //nmea_tofloat(&s_gps.frame_gll.longitude),
                       //s_gps.frame_gll.status,
                       //s_gps.frame_gll.mode);
            }
            else 
			{
                LOG(WARN,"$GLL sentence is not parsed");
            }
        } 
			break;
		*/

        case NMEA_INVALID: 
		{
            LOG(WARN,"Sentence is not valid:len=%d",len);
			LOG_HEX(p_data,len);
        } 
			break;

        default:
		{
			//LOG(DEBUG,"Unknown data:%s",p_data);
            //LOG_HEX(p_data,len);
        } 
			break;
    }
}

static void on_received_gga(const NMEASentenceGGA gga)
{
	if (gga.fix_quality == 0)
	{
		s_gps.is_fix = false;
		s_gps.is_3d = false;
		s_gps.is_diff = false;
	}
	//中科微的航迹推算为6
	else if(gga.fix_quality == 1 || gga.fix_quality == 6)
	{
		s_gps.is_fix = true;
		s_gps.is_diff = false;
	}
	else if(gga.fix_quality == 2)
	{
		s_gps.is_fix = true;
		s_gps.is_diff = true;
	}
	else
	{
		LOG(ERROR, "Unknown fix quality:%d", gga.fix_quality);
	}
	
	s_gps.satellites_tracked = gga.satellites_tracked;
	s_gps.altitude = nmea_tofloat(&gga.altitude);
	s_gps.hdop = nmea_tofloat(&gga.hdop);
	
	check_fix_state_change();
	LOG(DEBUG, "fix_state=%d,tracked=%d,hdop=%f", s_gps.state,s_gps.satellites_tracked,s_gps.hdop);
}

static void on_received_gsa(const NMEASentenceGSA gsa)
{
	if (s_gps.gpsdev_type != GM_GPS_TYPE_MTK_EPO)
	{
		on_received_fixed_state((NMEAGSAFixType)gsa.fix_type);
	}
	s_gps.hdop = nmea_tofloat(&gsa.hdop);
}

static void on_received_fixed_state(const NMEAGSAFixType fix_type)
{
	static U8 is_not_3d_seconds = 0;
	if (NMEA_GPGSA_FIX_NONE == fix_type)
	{
		is_not_3d_seconds++;
		if(is_not_3d_seconds >= SECONDS_PER_MIN)
		{
			s_gps.is_fix = false;
			s_gps.is_3d = false;
			s_gps.is_diff = false;
		}
	}
	else if (NMEA_GPGSA_FIX_2D == fix_type)
	{
		is_not_3d_seconds++;
		if(is_not_3d_seconds >= SECONDS_PER_MIN)
		{
			s_gps.is_fix = true;
			s_gps.is_3d = false;
		}
	}
	else if (NMEA_GPGSA_FIX_3D == fix_type)
	{
		s_gps.is_fix = true;
		s_gps.is_3d = true;
		is_not_3d_seconds = 0;
	}
	else 
	{
		LOG(WARN, "Unknown fix_type:%d", fix_type);
	}
	check_fix_state_change();
}

static on_received_gsv(const NMEASentenceGSV gsv)
{
	U8 index_sate = 0;
	U8 index_snr = 0;
	static U8 satellites_good = 0;
	static U8 max_snr = 0;
	//信号最好的5颗星
	static SNRInfo snr_array[5] = {{0,0},{0,0},{0,0},{0,0},{0,0}};
	s_gps.satellites_inview = gsv.total_satellites;


    for (index_sate = 0; index_sate < 4; index_sate++)
    {
    	//找出最差的那颗星的位置
		U8 min_snr_index = 0;
		U8 min_snr = 0xFF;
		
		LOG(DEBUG,"$GPGSV: sat nr %d, elevation: %d, azimuth: %d, snr: %d dbm\n",
			gsv.satellites[index_sate].nr,
			gsv.satellites[index_sate].elevation,
			gsv.satellites[index_sate].azimuth,
			gsv.satellites[index_sate].snr);
		
		for(index_snr = 0;index_snr < SNR_INFO_NUM;index_snr++)
		{
			if (snr_array[index_snr].snr < min_snr)
			{
				min_snr = snr_array[index_snr].snr;
				min_snr_index = index_snr;
			}
		}

		//LOG(DEBUG,"min_snr:%d,min_snr_index:%d",min_snr,min_snr_index);
	
		//如果当前这颗星比之前保存的5颗中最差的一个要好，那么替换掉最差的那个
		if (gsv.satellites[index_sate].snr > min_snr)
		{
			snr_array[min_snr_index].prn = gsv.satellites[index_sate].nr;
			snr_array[min_snr_index].snr = gsv.satellites[index_sate].snr;
		}

		if (gsv.satellites[index_sate].snr > 35)
		{
			satellites_good++;
		}
		
		if (gsv.satellites[index_sate].snr > max_snr)
		{
			max_snr = gsv.satellites[index_sate].snr;
		}
    }

	//LOG(DEBUG,"msg_number:%d,total_msgs:%d",gsv.msg_number, gsv.total_msgs);
	//最后一条消息，更新可用卫星数和最大信噪比等成员变量,中间变量清零
	if (gsv.msg_number == gsv.total_msgs)
	{
		s_gps.satellites_good = satellites_good;
		satellites_good = 0;
		
		s_gps.max_snr = max_snr;
		max_snr = 0;
		//LOG(DEBUG,"$GSV: sattelites in view: %d,satellites_good: %d,max_snr %d", gsv.total_satellites,s_gps.satellites_good,s_gps.max_snr);
			
		//上个版本用的水平精度因子（和卫星分布有关）
		s_gps.signal_intensity_grade = s_gps.max_snr/10;
		if (s_gps.signal_intensity_grade > 4)
		{
			s_gps.signal_intensity_grade = 4;
		}
		GM_memcpy(s_gps.snr_array, snr_array, sizeof(snr_array));
		GM_memset(snr_array, 0, sizeof(snr_array));
	}

	//如果有AGPS，但是启动20秒后仍然没有一颗星可见，重启GPS
	//if (((util_clock() - s_gps.power_on_time) > 20 + s_gps.agps_time) && s_gps.agps_time != 0 && s_gps.max_snr < 8 && !gps_is_fixed())
	//{
		//LOG(INFO,"reopen GPS because of no signal");
		//reopen_gps();
	//}
}

static void check_fix_state_change(void)
{
	U8 fix_time_min = (GM_GPS_TYPE_MTK_EPO == s_gps.gpsdev_type ? 2 : 10);
	GM_CHANGE_ENUM change = util_check_state_change(s_gps.is_3d,&s_gps.state_record,fix_time_min,20);
	
	if (GM_CHANGE_TRUE == change)
	{
		JsonObject* p_log_root = json_create();
		char snr_str[128] = {0};
		
		s_gps.internal_state = GM_GPS_STATE_WORKING;

		if(0 == s_gps.fix_time)
		{
			s_gps.fix_time = util_clock() - s_gps.power_on_time;
			json_add_string(p_log_root, "event", "fixed");
			json_add_int(p_log_root, "AGPS time", s_gps.agps_time);
			//距离上次休眠到现在超过6小时或者没有休眠过是冷启动，否则是热启动
			if((util_clock() - s_gps.sleep_time) > 6*SECONDS_PER_HOUR || 0 == s_gps.sleep_time)
			{
				json_add_int(p_log_root, "cold fix_time", s_gps.fix_time);
				LOG(INFO,"cold fix_time:%d",s_gps.fix_time);
			}
			else
			{
				json_add_int(p_log_root, "hot fix_time", s_gps.fix_time);
				LOG(INFO,"hot fix_time:%d",s_gps.fix_time);
			}
		}
		else
		{
			json_add_string(p_log_root, "event", "become to fixed");
		}
				
		json_add_int(p_log_root, "satellites_in_view", s_gps.satellites_inview);
		json_add_int(p_log_root, "satellites_good", s_gps.satellites_good);
		json_add_int(p_log_root, "satellites_tracked", s_gps.satellites_tracked);
		
		GM_snprintf(snr_str,128,"%03d:%02d,%03d:%02d,%03d:%02d,%03d:%02d,%03d:%02d,",
			s_gps.snr_array[0].prn,s_gps.snr_array[0].snr,
			s_gps.snr_array[1].prn,s_gps.snr_array[1].snr,
			s_gps.snr_array[2].prn,s_gps.snr_array[2].snr, 
			s_gps.snr_array[3].prn,s_gps.snr_array[3].snr,
			s_gps.snr_array[4].prn,s_gps.snr_array[4].snr);

		json_add_string(p_log_root, "snr", snr_str);
		json_add_int(p_log_root, "csq", gsm_get_csq());
		log_service_upload(INFO, p_log_root);

		
		
		led_set_gps_state(GM_LED_ON);
		s_gps.state = (GPSState)(s_gps.is_fix << 2 | (s_gps.is_3d << 1) | (s_gps.is_diff));
	}
	else if(GM_CHANGE_FALSE == change)
	{
		JsonObject* p_log_root = json_create();
		char snr_str[128] = {0};
		json_add_string(p_log_root, "event", "become to unfixed");
		json_add_int(p_log_root, "satellites_in_view", s_gps.satellites_inview);
		json_add_int(p_log_root, "satellites_good", s_gps.satellites_good);
		json_add_int(p_log_root, "satellites_tracked", s_gps.satellites_tracked);
		
		GM_snprintf(snr_str,128,"%03d:%02d,%03d:%02d,%03d:%02d,%03d:%02d,%03d:%02d,",
			s_gps.snr_array[0].prn,s_gps.snr_array[0].snr,
			s_gps.snr_array[1].prn,s_gps.snr_array[1].snr,
			s_gps.snr_array[2].prn,s_gps.snr_array[2].snr, 
			s_gps.snr_array[3].prn,s_gps.snr_array[3].snr,
			s_gps.snr_array[4].prn,s_gps.snr_array[4].snr);

		json_add_string(p_log_root, "snr", snr_str);
		json_add_int(p_log_root, "csq", gsm_get_csq());
		log_service_upload(INFO, p_log_root);
		
		led_set_gps_state(GM_LED_FLASH);
		s_gps.state = (GPSState)(s_gps.is_fix << 2 | (s_gps.is_3d << 1) | (s_gps.is_diff));
	}
	else
	{
		
	}
	
}
static void on_received_rmc(const NMEASentenceRMC rmc)
{
	GPSData gps_data = {0};
	U8 index = 0;
	float lat_avg = 0;
	float lng_avg = 0;
	float speed_avg = 0;
	time_t seconds_from_reboot = util_clock();

	if (s_gps.gpsdev_type == GM_GPS_TYPE_MTK_EPO)
	{
		on_received_fixed_state(rmc.valid?NMEA_GPGSA_FIX_3D:NMEA_GPGSA_FIX_NONE);
	}


	if (s_gps.state < GM_GPS_FIX_3D)
	{
		return;
	}

	if (seconds_from_reboot - s_gps.save_time >= 1)
	{
		bool if_smooth_track = false;
		gps_data.gps_time = nmea_get_utc_time(&rmc.date,&rmc.time);
		gps_data.lat = nmea_tocoord(&rmc.latitude);
		gps_data.lng = nmea_tocoord(&rmc.longitude);
		gps_data.speed = util_mile_to_km(nmea_tofloat(&rmc.speed));
		//判断角度是否有效,如果无效取上一个点的角度
		if (0 == rmc.course.value && 0 == rmc.course.scale)
		{	
			GPSData last_gps_data = {0};
			if(gps_get_last_data(&last_gps_data))
			{
				gps_data.course = last_gps_data.course;
			}
			else
			{
				gps_data.course = 0;
			}
		}
		else
		{
			gps_data.course = nmea_tofloat(&rmc.course);
		}
		gps_data.satellites = s_gps.satellites_tracked;
		gps_data.precision = s_gps.hdop;
		gps_data.signal_intensity_grade = s_gps.signal_intensity_grade;

		//检查异常数据
		if (0 == gps_data.gps_time || false == rmc.valid)
		{
			return;
		}
		
		if (fabs(gps_data.lat) < 0.1 &&  fabs(gps_data.lng)  < 0.1)
		{
			return;
		}
		
		calc_alcr_by_speed(gps_data);
		check_over_speed_alarm(gps_data.speed);

		
		s_gps.save_time = seconds_from_reboot;
		
		circular_queue_en_queue_i(&s_gps.gps_time_queue,gps_data.gps_time);
		circular_queue_en_queue_f(&s_gps.gps_lat_queue,gps_data.lat);
		circular_queue_en_queue_f(&s_gps.gps_lng_queue, gps_data.lng);
		circular_queue_en_queue_f(&s_gps.gps_speed_queue,gps_data.speed);
		circular_queue_en_queue_f(&s_gps.gps_course_queue,gps_data.course);
		
		config_service_get(CFG_SMOOTH_TRACK, TYPE_BOOL, &if_smooth_track, sizeof(if_smooth_track));
		if(if_smooth_track)
		{
			LOG(DEBUG,"smooth track");
			//取前10秒（包括当前时间点）平均值
			for(index = 0;index < 10;index++)
			{
				time_t last_n_time = 0;
				float last_n_lat = 0;
				float last_n_lng = 0;
				float last_n_speed = 0;
			
				if(false == circular_queue_get_by_index_i(&s_gps.gps_time_queue,index,(S32*)&last_n_time))
				{
					break;
				}
				if (gps_data.gps_time - last_n_time >= 10)
				{
					break;
				}
				circular_queue_get_by_index_f(&s_gps.gps_lat_queue,index,&last_n_lat);
				circular_queue_get_by_index_f(&s_gps.gps_lng_queue, index,&last_n_lng);
				circular_queue_get_by_index_f(&s_gps.gps_speed_queue,index,&last_n_speed);
			
				lat_avg = (lat_avg*index + last_n_lat)/(index + 1);
				lng_avg = (lng_avg*index + last_n_lng)/(index + 1);
				speed_avg = (speed_avg*index + last_n_speed)/(index + 1);
			}
			gps_data.lat = lat_avg;
			gps_data.lng = lng_avg;
			gps_data.speed = speed_avg;
		}
		upload_gps_data(gps_data);
	}
}

//每1秒调用一次
static void calc_alcr_by_speed(GPSData gps_info)
{
    float last_second_speed = 0;
    float aclr_calculate_by_speed = 0;

    if (gps_info.speed >= MIN_CRUISE_SPEED)
    {
        s_gps.constant_speed_time++;
        s_gps.static_speed_time = 0;
        if (circular_queue_get_tail_f(&(s_gps.gps_speed_queue), &last_second_speed))
        {
            aclr_calculate_by_speed = (gps_info.speed - last_second_speed) * 1000.0 / SECONDS_PER_HOUR;
            s_gps.aclr_avg_calculate_by_speed = (s_gps.aclr_avg_calculate_by_speed * (s_gps.constant_speed_time - 1) + aclr_calculate_by_speed) / (s_gps.constant_speed_time);
        }
    }
    else
    {
        s_gps.constant_speed_time = 0;
        s_gps.static_speed_time++;
        s_gps.aclr_avg_calculate_by_speed = 0;
    }

}

static void check_over_speed_alarm(float speed)
{
	bool speed_alarm_enable = false;
	U8 speed_threshold = 0;
	U8 speed_check_time = 0;
	GM_CHANGE_ENUM state_change = GM_NO_CHANGE;
	AlarmInfo alarm_info;
	alarm_info.type = ALARM_SPEED;
	alarm_info.info = speed;
	
	config_service_get(CFG_SPEED_ALARM_ENABLE, TYPE_BOOL, &speed_alarm_enable, sizeof(speed_alarm_enable));
	config_service_get(CFG_SPEEDTHR, TYPE_BYTE, &speed_threshold, sizeof(speed_threshold));
	config_service_get(CFG_SPEED_CHECK_TIME, TYPE_BYTE, &speed_check_time, sizeof(speed_check_time));
	
	if(!speed_alarm_enable)
	{
		return;
	}
	
	state_change = util_check_state_change(speed > speed_threshold, &s_gps.over_speed_alarm_state, speed_check_time, speed_check_time);
	if(GM_CHANGE_TRUE == state_change)
	{
		system_state_set_overspeed_alarm(true);
		gps_service_push_alarm(&alarm_info);
	}
	else if(GM_CHANGE_FALSE == state_change)
	{
		system_state_set_overspeed_alarm(false);
	}
	else
	{
		
	}
}

time_t gps_get_constant_speed_time(void)
{
	return s_gps.constant_speed_time;
}

float gps_get_aclr(void)
{
	return s_gps.aclr_avg_calculate_by_speed;
}

static GpsDataModeEnum upload_mode(const GPSData current_gps_data)
{
	GM_ERRCODE err_code = GM_SUCCESS;
	U32 seconds_from_reboot = util_clock();
    U16 upload_time_interval = 0;
	bool static_upload_enable = false;
	
	err_code = config_service_get(CFG_UPLOADTIME, TYPE_SHORT, &upload_time_interval, sizeof(upload_time_interval));
	if (GM_SUCCESS != err_code)
	{
		LOG(ERROR, "Failed to config_service_get(CFG_UPLOADTIME),err_code=%d", err_code);
		return GPS_MODE_NONE;
	}

	if(false == system_state_has_reported_gps_since_boot())
	{
		system_state_set_has_reported_gps_since_boot(true);
		s_gps.distance_since_last_report = 0;
		s_gps.report_time = seconds_from_reboot;
		return GPS_MODE_POWER_UP;
	}
	
	if(false == system_state_has_reported_gps_since_modify_ip())
	{
		system_state_set_reported_gps_since_modify_ip(true);
		s_gps.distance_since_last_report = 0;
		s_gps.report_time = seconds_from_reboot;
		return GPS_MODE_POWER_UP;
	}
	
    config_service_get(CFG_IS_STATIC_UPLOAD, TYPE_BOOL, &static_upload_enable, sizeof(static_upload_enable));
	if (VEHICLE_STATE_STATIC == system_state_get_vehicle_state()  && false == static_upload_enable && false == config_service_is_test_mode())
	{
		LOG(DEBUG, "Need not upload data because of static.");
		return GPS_MODE_NONE;
	}
	else 
	{
		if ((seconds_from_reboot - s_gps.report_time) >= upload_time_interval)
		{
			s_gps.distance_since_last_report = 0;
			s_gps.report_time = seconds_from_reboot;
			if(VEHICLE_STATE_STATIC == system_state_get_vehicle_state())
			{
				return GPS_MODE_STATIC_POSITION;
			}
			else
			{
				return GPS_MODE_FIX_TIME;
			}
			
		}
		else if (is_turn_point(current_gps_data)) 
		{
			s_gps.distance_since_last_report = 0;
			s_gps.report_time = seconds_from_reboot;
			LOG(DEBUG, "GPS_MODE_TURN_POINT");
			return GPS_MODE_TURN_POINT;
		}
		else
		{
			return GPS_MODE_NONE;
		}
	}
}

static void upload_gps_data(const GPSData current_gps_data)
{
	GM_ERRCODE ret = GM_SUCCESS;
	GpsDataModeEnum mode = upload_mode(current_gps_data);
	ST_Time st_time = {0};
	struct tm tm_time = {0};

	static GPSData last_report_gps = {0};

	if(false == s_gps.push_data_enbale)
	{
		LOG(WARN,"do not push data!");
		return;
	}
	
	if (0 == current_gps_data.gps_time)
	{
		LOG(WARN,"Invalid GPS data!");
		return;
	}

	if (GPS_MODE_NONE == mode)
	{
		return;
	}

	if (GPS_MODE_POWER_UP == mode)
	{	
		tm_time = util_gmtime(current_gps_data.gps_time);
		util_tm_to_mtktime(&tm_time,&st_time);
		GM_SetLocalTime(&st_time);
		LOG(INFO,"Set local time:(%d-%02d-%02d %02d:%02d:%02d)).",st_time.year,st_time.month,st_time.day,st_time.hour,st_time.minute,st_time.second);
	}

	ret = gps_service_push_gps(mode,&current_gps_data);

	if (last_report_gps.gps_time != 0)
	{
		s_gps.distance_since_last_report = applied_math_get_distance(current_gps_data.lng, current_gps_data.lat, last_report_gps.lng, last_report_gps.lat);
		system_state_set_mileage(system_state_get_mileage() + s_gps.distance_since_last_report);
	}

	last_report_gps = current_gps_data;
	
	
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR, "Failed to gps_service_push_gps,ret=%d", ret);
		return;
	}
}

static bool is_turn_point(const GPSData current_gps_data)
{
	U16 course_change_threshhold = 0;
    float course_change = 0;
	float current_course = current_gps_data.course;
	float last_second_course = 0;
	float last_speed = 0;
	GM_ERRCODE ret = GM_SUCCESS;
	U8 index = 0;

	ret = config_service_get(CFG_TURN_ANGLE, TYPE_SHORT, &course_change_threshhold, sizeof(course_change_threshhold));
	if(GM_SUCCESS != ret)
	{
		LOG(ERROR, "Failed to config_service_get(CFG_TURN_ANGLE),ret=%d", ret);
		return false;
	}
	
	if(!circular_queue_get_by_index_f(&s_gps.gps_course_queue, 1, &last_second_course))
	{
		return false;
	}

	course_change = applied_math_get_angle_diff(current_course,last_second_course);	
	LOG(DEBUG,"[%s]:current course:%f,last second course:%f,course change:%f",__FUNCTION__,current_course,last_second_course,course_change);
	
    if (current_gps_data.speed >= 80.0f)
    {
        if (course_change >= 6)
        {   
            return true;
        }
        else if (course_change >= course_change_threshhold)
        {
            return true;
        }
		else
		{
			return false;
		}
    }
    else if (current_gps_data.speed >= 40.0f)
    {
        if (course_change >= 7)
        {
            return true;
        }
        else if (course_change >= course_change_threshhold)
        {
            return true;
        }
		else
		{
			return false;
		}
    }
    else if (current_gps_data.speed >= 25.0f)
    {
        if (course_change >= 8)
        {
            return true;
        }
        else if (course_change >= course_change_threshhold)
        {
            return true;
        }
		else
		{
			return false;
		}
    }
    else if (current_gps_data.speed >= 15.0f)
    {
        if (course_change >= (course_change_threshhold - 6))
        {
            return true;
        }
		else
		{
			return false;
		}
    }
    else if ((current_gps_data.speed >= 5.0f) && course_change >= 16 && s_gps.distance_since_last_report > 20.0f)
    {
        // 连续5秒速度大于5KMH,角度大于20度,里程超过20米,上传拐点
        for (index =0; index < 5; index++)
        {
			if(!circular_queue_get_by_index_f(&s_gps.gps_speed_queue, index, &last_speed))
			{
				return false;
			}
            if (last_speed <= 5.0f)
            {
                return false;
            }
        }
		return true;
    }
	else
	{
		return false;
	}
}



//查询MTK版本号
static void query_mtk_version(void)
{
	bool create_ret = false;
	GM_ERRCODE write_ret = GM_SUCCESS;

	U8 sentence[SENTENCE_MAX_LENGTH] = {0};
	U8 len = SENTENCE_MAX_LENGTH;
	
	//已经查到了版本号
	if (s_gps.internal_state > GM_GPS_STATE_WAIT_VERSION)
	{
		return;
	}
	else
	{
		GM_StartTimer(GM_TIMER_GPS_QUERY_MTK_VERSION, TIM_GEN_1SECOND*1, query_mtk_version);
	}
	
	create_ret = nmea_creat_mtk_qeury_version_sentence(sentence, &len);
	if (false == create_ret || 0 == len)
	{
		LOG(ERROR, "Failed to nmea_creat_mtk_qeury_version_sentence!");
		return;
	}
	write_ret = uart_write(GM_UART_GPS, sentence, len);
	if (GM_SUCCESS != write_ret)
	{
		LOG(ERROR, "Failed to write to GPS UART,ret=%d!",write_ret);
	}
	else
	{
		LOG(DEBUG,"Send query_mtk_version cmd:%s",sentence);
	}
}

//知道9600波特率能收到数据,查询泰斗和中科微版本号
static void query_td_and_at_version(void)
{
	bool create_ret = false;
	GM_ERRCODE write_ret = GM_SUCCESS;
	U8 sentence[SENTENCE_MAX_LENGTH] = {0};
	U8 len = SENTENCE_MAX_LENGTH;

	 //已经查到了版本号
	if (s_gps.internal_state > GM_GPS_STATE_WAIT_VERSION)
	{
		return;
	}
	else
	{
		GM_StartTimer(GM_TIMER_GPS_QUERY_TD_AT_VERSION, TIM_GEN_1SECOND*1, query_td_and_at_version);
	}

	create_ret = nmea_creat_td_qeury_version_sentence(sentence, &len);
	if (false == create_ret || 0 == len)
	{
		LOG(ERROR, "Failed to nmea_creat_td_qeury_version_sentence!");
		return;
	}
	write_ret = uart_write(GM_UART_GPS, sentence, len);
	if (GM_SUCCESS != write_ret)
	{
		LOG(ERROR, "Failed to write to GPS UART,ret=%d!",write_ret);
	}
	
	create_ret = false;
	write_ret = GM_SUCCESS;

	GM_memset(sentence, 0, SENTENCE_MAX_LENGTH);
	len = SENTENCE_MAX_LENGTH;
	create_ret = nmea_creat_at_qeury_version_sentence(sentence, &len);
	if (false == create_ret || 0 == len)
	{
		LOG(ERROR, "Failed to nmea_creat_at_qeury_version_sentence!");
		return;
	}
	write_ret = uart_write(GM_UART_GPS, sentence, len);
	if (GM_SUCCESS != write_ret)
	{
		LOG(ERROR, "Failed to write to GPS UART,ret=%d!",write_ret);
	}
}

static void query_td_version(void)
{
	bool create_ret = false;
	GM_ERRCODE write_ret = GM_SUCCESS;

	U8 sentence[SENTENCE_MAX_LENGTH] = {0};
	U8 len = SENTENCE_MAX_LENGTH;

    //已经查到了版本号
	if (s_gps.internal_state > GM_GPS_STATE_WAIT_VERSION)
	{
		return;
	}
	else
	{
		GM_StartTimer(GM_TIMER_GPS_QUERY_TD_VERSION, TIM_GEN_1SECOND*1, query_td_version);
	}
	
	create_ret = nmea_creat_td_qeury_version_sentence(sentence, &len);
	if (false == create_ret || 0 == len)
	{
		LOG(ERROR, "Failed to nmea_creat_td_qeury_version_sentence!");
		return;
	}
	write_ret = uart_write(GM_UART_GPS, sentence, len);
	if (GM_SUCCESS != write_ret)
	{
		LOG(ERROR, "Failed to write to GPS UART,ret=%d!",write_ret);
	}
}

static void query_at_version(void)
{
	bool create_ret = false;
	GM_ERRCODE write_ret = GM_SUCCESS;

	U8 sentence[SENTENCE_MAX_LENGTH] = {0};
	U8 len = SENTENCE_MAX_LENGTH;

    //已经查到了版本号
	if (s_gps.internal_state > GM_GPS_STATE_WAIT_VERSION)
	{
		return;
	}
	else
	{
		GM_StartTimer(GM_TIMER_GPS_QUERY_AT_VERSION, TIM_GEN_1SECOND*1, query_at_version);
	}
	
	create_ret = nmea_creat_at_qeury_version_sentence(sentence, &len);
	if (false == create_ret || 0 == len)
	{
		LOG(ERROR, "Failed to nmea_creat_at_qeury_version_sentence!");
		return;
	}
	write_ret = uart_write(GM_UART_GPS, sentence, len);
	if (GM_SUCCESS != write_ret)
	{
		LOG(ERROR, "Failed to write to GPS UART,ret=%d!",write_ret);
	}
}

static GM_ERRCODE write_mtk_epo_time(const ST_Time utc_time)
{
	U8 sentence[SENTENCE_MAX_LENGTH] = {0};
	U8 len = SENTENCE_MAX_LENGTH;
	nmea_creat_mtk_aid_time_sentence(utc_time,sentence,&len);
	LOG(DEBUG,"write_mtk_epo_time:%s",sentence);
	return uart_write(GM_UART_GPS, sentence, len);
}

static void write_mtk_epo_pos(void)
{
	GM_ERRCODE ret = GM_SUCCESS;
	U8 sentence[SENTENCE_MAX_LENGTH] = {0};
	U8 len = SENTENCE_MAX_LENGTH;
	nmea_creat_mtk_aid_pos_sentence(s_gps.ref_lat,s_gps.ref_lng,sentence,&len);
	LOG(DEBUG,"write_mtk_epo_pos:%s",sentence);
	ret = uart_write(GM_UART_GPS, sentence, len);
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR,"Failed to write data to GM_UART_GPS,ret=%d",ret);
	}
}

void gps_write_mtk_cmd(const char* cmd)
{
	GM_ERRCODE ret = GM_SUCCESS;
	U8 sentence[SENTENCE_MAX_LENGTH] = {0};
	nmea_create_common_mtk_sentence(cmd,sentence);
	LOG(DEBUG,"gps_write_mtk_cmd:%s",sentence);
	ret = uart_write(GM_UART_GPS, sentence, GM_strlen((char*)sentence));
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR,"Failed to write data to GM_UART_GPS,ret=%d",ret);
	}
}

static void open_td_vtg(void)
{
	GM_ERRCODE ret = GM_SUCCESS;
	U8 sentence[SENTENCE_MAX_LENGTH] = {0};
	U8 len = SENTENCE_MAX_LENGTH;

    //已经打开
	if (true == s_gps.has_opened_td_vtg)
	{
		return;
	}
	else
	{
		GM_StartTimer(GM_TIMER_GPS_OPEN_TD_VTG, TIM_GEN_1SECOND*1, open_td_vtg);
	}

	nmea_creat_td_open_vtg_sentence(sentence,&len);
	ret = uart_write(GM_UART_GPS, sentence, len);
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR,"Failed to write data to GM_UART_GPS,ret=%d",ret);
	}
}

static GM_ERRCODE write_td_agps_time(const ST_Time utc_time,const U8 leap_sencond)
{
	U8 sentence[SENTENCE_MAX_LENGTH] = {0};
	U8 len = SENTENCE_MAX_LENGTH;
	nmea_creat_td_aid_time_sentence(utc_time,leap_sencond,sentence,&len);
	LOG(INFO,"write_td_agps_time:len=%d",len);
	LOG_HEX((char*)sentence,len);
	return uart_write(GM_UART_GPS, sentence, len);
}

static void write_td_agps_pos(void)
{
	GM_ERRCODE ret = GM_SUCCESS;
	U8 sentence[SENTENCE_MAX_LENGTH] = {0};
	U8 len = SENTENCE_MAX_LENGTH;
	nmea_creat_td_aid_pos_sentence(s_gps.ref_lat,s_gps.ref_lng,sentence,&len);
	ret = uart_write(GM_UART_GPS, sentence, len);
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR,"Failed to write data to GM_UART_GPS,ret=%d",ret);
	}
}

static GM_ERRCODE write_at_agps_info(const ST_Time utc_time,const U8 leap_sencond,const float ref_lat,const float ref_lng)
{
	U8 sentence[SENTENCE_MAX_LENGTH] = {0};
	U8 len = SENTENCE_MAX_LENGTH;
	nmea_creat_at_aid_info_sentence(utc_time, leap_sencond, ref_lat, ref_lng,sentence,&len);
	return uart_write(GM_UART_GPS, sentence, len);
}

