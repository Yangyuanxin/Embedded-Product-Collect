/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        g_sensor.h
 * Author:           王志华       
 * Version:          1.0
 * Date:             2019-03-01
 * Description:      运动传感器数据处理,控制GPS休眠与唤醒、判断运动状态（用于控制GPS是否产生数据）、相关报警
 * Others:      
 * Function List:    
    1. 创建g_sensor模块
    2. 销毁g_sensor模块
    3. g_sensor模块定时处理入口
    4. 获取g_sensor阈值
    5. 设置g_sensor阈值
    6. 获取g-sensor状态
 * History: 
    1. Date:         2019-03-01
       Author:       王志华
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 
 */

#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <gm_timer.h>
#include <gm_eint.h>
#include <gm_iic.h>
#include <gm_stdlib.h>
#include "g_sensor.h"
#include "applied_math.h"
#include "circular_queue.h"
#include "gps.h"
#include "log_service.h"
#include "config_service.h"
#include "system_state.h"
#include "led.h"
#include "gps_service.h"
#include "utility.h"
#include "main.h"
#include "hard_ware.h"
#include "uart.h"
#include "bms.h"

	
//写控制bit
#define I2C_WR 0

//读控制bit
#define I2C_RD 1

#define I2C_SCL 43
#define I2C_SDA 44

#define HW_I2C 1
#define SW_I2C 0


#define GSEN_PIN GM_GPIO52

#define GSEN_EINT GM_EINT23

//SC7A20、BMA253适用
#define GOOME_I2C_ADDR 0x30

//DA213适用
#define DA213_I2C_ADDR 0x4E

#define SC7A20_REG_ADDR                      0x0F

#define BMA253_REG_ADDR                      0x00
#define BMA253_X_AXIS_LSB_REG                   0x02
#define BMA253_X_AXIS_MSB_REG                   0x03
#define BMA253_Y_AXIS_LSB_REG                   0x04
#define BMA253_Y_AXIS_MSB_REG                   0x05
#define BMA253_Z_AXIS_LSB_REG                   0x06
#define BMA253_Z_AXIS_MSB_REG                   0x07
#define BMA253_STATUS1_REG                      0x09
#define BMA253_STATUS2_REG                      0x0A
#define BMA253_STATUS_TAP_SLOPE_REG             0x0B
#define BMA253_STATUS_ORIENT_HIGH_REG           0x0C
#define BMA253_STATUS_FIFO_REG                  0x0E  //ADDED
#define BMA253_RANGE_SEL_REG                    0x0F
#define BMA253_BW_SEL_REG                       0x10
#define BMA253_MODE_CTRL_REG                    0x11
#define BMA253_LOW_NOISE_CTRL_REG               0x12
#define BMA253_DATA_CTRL_REG                    0x13
#define BMA253_RESET_REG                        0x14  // only write 0xB6 will reset
#define BMA253_INT_ENABLE1_REG                  0x16  //选择中断使能,0x07 为X,Y,Z向量使能
#define BMA253_INT_ENABLE2_REG                  0x17
#define BMA253_INT_SLO_NO_MOT_REG               0x18      //ADDED
#define BMA253_INT1_PAD_SEL_REG                 0x19  // 选择INT1产生中断方式,默认无,0x04 为 向量方式
#define BMA253_INT_DATA_SEL_REG                 0x1A
#define BMA253_INT2_PAD_SEL_REG                 0x1B
#define BMA253_INT_SRC_REG                      0x1E  //中断数据源是否经过过滤,默认为0,为经过过滤
#define BMA253_INT_SET_REG                      0x20  //设置中断输出方式,默认为INT1,INT2产生中断时输出高电平
#define BMA253_INT_CTRL_REG                     0x21
#define BMA253_LOW_DURN_REG                     0x22
#define BMA253_LOW_THRES_REG                    0x23
#define BMA253_LOW_HIGH_HYST_REG                0x24  //设置高中断滞后值,bit7-6  x * 125mg
#define BMA253_HIGH_DURN_REG                    0x25  //设置高中断持续时间,bit7-0  x * 2ms
#define BMA253_HIGH_THRES_REG                   0x26  //设置高中断震动阈值,bit7-0  x * 7.81mg(2g)
#define BMA253_SLOPE_DURN_REG                   0x27  //向量 震动持续时间,bit7-0  x * 3.91mg(2g)
#define BMA253_SLOPE_THRES_REG                  0x28  //向量 震动阈值,bit7-0  x * 3.91mg(2g)
#define BMA253_SLO_NO_MOT_THRES_REG             0x29  //ADDED
#define BMA253_TAP_PARAM_REG                    0x2A
#define BMA253_TAP_THRES_REG                    0x2B
#define BMA253_ORIENT_PARAM_REG                 0x2C
#define BMA253_THETA_BLOCK_REG                  0x2D
#define BMA253_THETA_FLAT_REG                   0x2E
#define BMA253_FLAT_HOLD_TIME_REG               0x2F
#define BMA253_FIFO_WML_TRIG                    0x30   //ADDED
#define BMA253_SELF_TEST_REG                    0x32
#define BMA253_EEPROM_CTRL_REG                  0x33
#define BMA253_SERIAL_CTRL_REG                  0x34
#define BMA253_OFFSET_CTRL_REG                  0x36
#define BMA253_OFFSET_PARAMS_REG                0x37
#define BMA253_OFFSET_X_AXIS_REG                0x38
#define BMA253_OFFSET_Y_AXIS_REG                0x39
#define BMA253_OFFSET_Z_AXIS_REG                0x3A

#define DA213_REG_ADDR                       0x01
#define DA213_X_AXIS_LSB_ADDR                   0x02
#define DA213_AXIS_NEW_DATA_ADDR                0x0A
#define DA213_RANGE_SELECT_ADDR                 0x0F
#define DA213_BW_SELECT_ADDR                    0x10
#define DA213_POWERMODE_BW_ADDR                 0x11
#define DA213_INTR_SET_ADDR                     0x20
#define DA213_INTR_CTRL_ADDR                    0x21
#define DA213_INTR_ENABLE1_ADDR                 0x16
#define DA213_INTMAP1_ADDR                      0x19
#define DA213_SLOPE_DURN_ADDR                   0x27
#define DA213_SLOPE_THRES_ADDR                  0x28
#define DA213_ANY_ACTIVE_ADDR                   0x0B
#define DA213_MOTION_FLAG_ADDR                  0x09
#define DA213_RESET_REG                         0x00


typedef struct
{
    S16 x;
    S16 y;
    S16 z;
} Aclr, *pAclr;

typedef struct
{
	U8 shake_level;
	U32 shake_threshold;
	float static_thr;        // 静止阈值
	float run_thr;           // 运动阈值
	float brake_thr;         // 急刹车阈值
	float rapid_aclr_thr;// 急加速阈值
	float sudden_turn_thr;   // 急转弯阈值
	float slight_crash_thr;  // 轻微碰撞阈值
	float normal_crash_thr;  // 一般碰撞阈值
	float serious_crash_thr; // 严重碰撞阈值	 
}GSensorThreshold;






typedef struct
{
	GSensorType type;

	//启动时间（毫秒）
	U64 start_time;

	//无振动时间毫秒
	U32 no_shake_time_ms;
	
	//唤醒时间（毫秒）
	U32 awake_time;

	//休眠时间（毫秒）
	U32 sleep_time;
	
	U32 rw_error_count;
	U32 read_data_count;

	//震动事件队列（存储的是震动时间）
	CircularQueue shake_event_time_queue_when_read;
	CircularQueue shake_event_time_queue_when_eint;
	U32 last_report_shake_alarm_time;
	U32 last_report_turnover_alarm_time;
	U32 last_report_remove_alarm_time;
	
	//存20秒传感器数据
	CircularQueue aclr_x_queue;
	CircularQueue aclr_y_queue;
	CircularQueue aclr_z_queue;
	
    CircularQueue sensor_angle_queue;

    Vector3D gravity;
    unsigned int cal_gravity_avg_len;
    Vector3D static_gravity;
    Vector2D vehicle_head_vector;
	
    //传感器加速度的滑动平均值,含重力加速度在内
    Vector3D senor_aclr_moving_avg;
    U32 cal_senor_aclr_avg_len;
    U32 cal_senor_aclr_avg_len_short_period;
    U64 study_aclr_count;
    U64 static_low_aclr_count;
    U64 super_low_aclr_count;

    COLLISION_LEVEL collision_level;
    U32 collision_aclr_count;
    GPSData last_collision_gps_info;
    Aclr last_collision_aclr;

	GSensorThreshold threshold;
}GSensor;

static GSensor s_gsensor;

static void reset_sensor_data(void);
static GM_ERRCODE get_config(void);
static void init_gsensor_chip(void);
static void check_awake_sleep(void);
static bool is_known_type(const GSensorType type);
static void g_sensor_eint_callback(void* v_arg);
static void read_data_from_chip(void);
static void check_shake_event(Aclr current_aclr);
static void check_shake_alarm(void);
static void report_shake_alarm(void);
static void check_acc_and_report_shake_alarm(void);
static U8 get_data_addr(const GSensorType type);
static void write_config_to_chip(const GSensorType type);
static void check_vehicle_state(Aclr a);
static void check_collision(Vector3D sensor_aclr);
static bool check_emergency_brake(float vehicle_horizontal_aclr_magnitude);
static void report_emergency_brake_alarm(void);
static bool check_rapid_acceleration(float vehicle_horizontal_aclr_magnitude);
static void report_rapid_aclr_alarm(void);
static bool check_sudden_turn_acceleration(float vehicle_horizontal_aclr_magnitude);
static void report_sudden_turn_alarm(void);
static void study_gravity(Aclr a);
static GM_ERRCODE get_vehicle_acceleration(Vector3D g, Vector3D sensor_aclr, Vector3D *vehicle_horizontal_aclr, Vector3D *vehicle_vertical_aclr);
static void calculate_moving_avg(Vector3D sensor_aclr, int num, PVector3D p_aclr_moving_avg, unsigned int *p_len);
static void check_static_or_run(float vehicle_horizontal_aclr_magnitude);
static void calc_sensor_angle(void);
static void check_angle_alarm(void);


/**
 * Function:   创建g_sensor模块
 * Description:创建g_sensor模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   使用前必须调用,否则调用其它接口返回失败错误码
 */
GM_ERRCODE g_sensor_create(void)
{
	GM_ERRCODE ret = GM_SUCCESS;
	s_gsensor.type = GSENSOR_TYPE_UNKNOWN;
	s_gsensor.start_time = 0;
	s_gsensor.no_shake_time_ms = 0;
	s_gsensor.awake_time = 0;
	s_gsensor.sleep_time = 0;
	s_gsensor.rw_error_count = 0;
	s_gsensor.read_data_count = 0;
	s_gsensor.last_report_shake_alarm_time = 0;
	s_gsensor.last_report_turnover_alarm_time = 0;
	s_gsensor.last_report_remove_alarm_time = 0;


	reset_sensor_data();

	ret = get_config();
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR, "Failed to get_config()!");
		return ret;
	}

	ret = circular_queue_create(&s_gsensor.shake_event_time_queue_when_read,G_SHAKE_BUF_LEN,GM_QUEUE_TYPE_INT);
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR, "Failed to circular_queue_create()!");
		return ret;
	}

	ret = circular_queue_create(&s_gsensor.shake_event_time_queue_when_eint,G_SHAKE_BUF_LEN,GM_QUEUE_TYPE_INT);
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR, "Failed to circular_queue_create()!");
		return ret;
	}
	
	ret = circular_queue_create(&s_gsensor.aclr_x_queue,G_SENSOR_BUF_LEN,GM_QUEUE_TYPE_INT);
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR, "Failed to circular_queue_create()!");
		return ret;
	}
	
	ret = circular_queue_create(&s_gsensor.aclr_y_queue,G_SENSOR_BUF_LEN,GM_QUEUE_TYPE_INT);
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR, "Failed to circular_queue_create()!");
		return ret;
	}
	
	ret = circular_queue_create(&s_gsensor.aclr_z_queue,G_SENSOR_BUF_LEN,GM_QUEUE_TYPE_INT);
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR, "Failed to circular_queue_create()!");
		return ret;
	}
	
	ret = circular_queue_create(&s_gsensor.sensor_angle_queue, ANGLE_RECORD_NUM,GM_QUEUE_TYPE_INT);
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR, "Failed to circular_queue_create()!");
		return ret;
	}
	return ret;
}

static void reset_sensor_data(void)
{
    s_gsensor.gravity.x = 0;
    s_gsensor.gravity.y = 0;
    s_gsensor.gravity.z = 0;
    s_gsensor.cal_gravity_avg_len = 0;
    s_gsensor.cal_senor_aclr_avg_len = 0;
    s_gsensor.cal_senor_aclr_avg_len_short_period = 0;
    s_gsensor.static_gravity.x = 0;
    s_gsensor.static_gravity.y = 0;
    s_gsensor.static_gravity.z = 0;
    s_gsensor.vehicle_head_vector.x = 0;
    s_gsensor.vehicle_head_vector.y = 0;
    s_gsensor.study_aclr_count = 0;
    s_gsensor.static_low_aclr_count = 0;
    s_gsensor.super_low_aclr_count = 0;
}

static GM_ERRCODE get_config(void)
{
	GM_ERRCODE ret = GM_SUCCESS;
	ret = config_service_get(CFG_SHAKE_LEVEL, TYPE_BYTE, &s_gsensor.threshold.shake_level, sizeof(s_gsensor.threshold.shake_level));
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR, "Failed to config_service_get(CFG_SHAKE_LEVEL),ret=%d", ret);
	}

	ret = config_service_get(CFG_SHAKE_THRESHOLD, TYPE_INT, &s_gsensor.threshold.shake_threshold, sizeof(s_gsensor.threshold.shake_threshold));
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR, "Failed to config_service_get(CFG_SHAKE_THRESHOLD),ret=%d", ret);
	}

	ret = config_service_get(CFG_SEN_STATIC, TYPE_FLOAT, &s_gsensor.threshold.static_thr, sizeof(s_gsensor.threshold.static_thr));
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR, "Failed to config_service_get(CFG_SEN_STATIC),ret=%d", ret);
	}

	ret = config_service_get(CFG_SEN_RUN, TYPE_FLOAT, &s_gsensor.threshold.run_thr, sizeof(s_gsensor.threshold.run_thr));
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR, "Failed to config_service_get(CFG_SEN_RUN),ret=%d", ret);
	}

	ret = config_service_get(CFG_SEN_EMERGENCY_BRAKE, TYPE_FLOAT, &s_gsensor.threshold.brake_thr, sizeof(s_gsensor.threshold.brake_thr));
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR, "Failed to config_service_get(CFG_SEN_EMERGENCY_BRAKE),ret=%d", ret);
	}

	ret = config_service_get(CFG_SEN_RAPID_ACLR, TYPE_FLOAT, &s_gsensor.threshold.rapid_aclr_thr, sizeof(s_gsensor.threshold.rapid_aclr_thr));
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR, "Failed to config_service_get(CFG_SEN_RAPID_ACLR),ret=%d", ret);
	}

	ret = config_service_get(CFG_SEN_SUDDEN_TURN, TYPE_FLOAT, &s_gsensor.threshold.sudden_turn_thr, sizeof(s_gsensor.threshold.sudden_turn_thr));
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR, "Failed to config_service_get(CFG_SEN_SUDDEN_TURN),ret=%d", ret);
	}

	ret = config_service_get(CFG_SEN_SLIGHT_COLLISION, TYPE_FLOAT, &s_gsensor.threshold.slight_crash_thr, sizeof(s_gsensor.threshold.slight_crash_thr));
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR, "Failed to config_service_get(CFG_SEN_SLIGHT_COLLISION),ret=%d", ret);
	}

	ret = config_service_get(CFG_SEN_NORMAL_COLLISION, TYPE_FLOAT, &s_gsensor.threshold.normal_crash_thr, sizeof(s_gsensor.threshold.normal_crash_thr));
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR, "Failed to config_service_get(CFG_SEN_NORMAL_COLLISION),ret=%d", ret);
	}

	ret = config_service_get(CFG_SEN_SERIOUS_COLLISION, TYPE_FLOAT, &s_gsensor.threshold.serious_crash_thr, sizeof(s_gsensor.threshold.serious_crash_thr));
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR, "Failed to config_service_get(CFG_SEN_SERIOUS_COLLISION),ret=%d", ret);
	}
	

	return GM_SUCCESS;
}

/**
 * Function:   销毁g_sensor模块
 * Description:销毁g_sensor模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE g_sensor_destroy(void)
{
	circular_queue_destroy(&s_gsensor.shake_event_time_queue_when_read,GM_QUEUE_TYPE_INT);

	circular_queue_destroy(&s_gsensor.shake_event_time_queue_when_eint,GM_QUEUE_TYPE_INT);
	
	circular_queue_destroy(&s_gsensor.aclr_x_queue,GM_QUEUE_TYPE_INT);

	circular_queue_destroy(&s_gsensor.aclr_y_queue,GM_QUEUE_TYPE_INT);
	
	circular_queue_destroy(&s_gsensor.aclr_z_queue,GM_QUEUE_TYPE_INT);
	
	circular_queue_destroy(&s_gsensor.sensor_angle_queue,GM_QUEUE_TYPE_INT);
	
	return GM_SUCCESS;
}

void g_sensor_reset_noshake_time(void)
{
	s_gsensor.no_shake_time_ms = 0;
}

void g_sensor_timer_proc(void)
{
	if (!is_known_type(s_gsensor.type))
	{
		init_gsensor_chip();
		return;
	}
	if (GM_SYSTEM_STATE_WORK == system_state_get_work_state())
	{
		s_gsensor.start_time += TIM_GEN_10MS;
	}
	else
	{
		s_gsensor.start_time += TIM_GEN_1SECOND;
	}
	
    get_config();
	read_data_from_chip();
}

U8 g_sensor_get_angle(void)
{
	S32 last_angle = 0;
	if(false == circular_queue_get_tail_i(&(s_gsensor.sensor_angle_queue),&last_angle))
	{
		return 0;
	}
	return (U8)last_angle;
}

#define SC7A20_STR "SC7A20"
#define BMA253_STR "BMA253"
#define DA213_STR "DA213"
#define UNKNOWN_STR "unknown"

GM_ERRCODE g_sensor_get_typestr(GSensorType type,char* p_type_str,const U8 max_len)
{
	if (GSENSOR_TYPE_SC7A20 == type)
	{
		GM_strncpy(p_type_str, SC7A20_STR, GM_strlen(SC7A20_STR));
		return GM_SUCCESS;
	}
	else if (GSENSOR_TYPE_BMA253 == type)
	{
		GM_strncpy(p_type_str, BMA253_STR, GM_strlen(BMA253_STR));
		return GM_SUCCESS;
	}
	else if (GSENSOR_TYPE_DA213 == type)
	{
		GM_strncpy(p_type_str, DA213_STR, GM_strlen(DA213_STR));
		return GM_SUCCESS;
	}
	else
	{	
		GM_strncpy(p_type_str, UNKNOWN_STR, GM_strlen(UNKNOWN_STR));
		return GM_HARD_WARE_ERROR;
	}
}

U32 g_sensor_get_error_count(void)
{
	return s_gsensor.rw_error_count;
}

U8 g_sensor_get_shake_event_count_when_read(void)
{
	return circular_queue_get_len(&s_gsensor.shake_event_time_queue_when_read);
}

U8 g_sensor_get_shake_event_count_when_eint(void)
{
	return circular_queue_get_len(&s_gsensor.shake_event_time_queue_when_eint);
}
static void check_awake_sleep(void)
{		
	U32 now =  util_clock();
	U8 index = 0;
	U32 shake_time = 0;
	U8 shake_count_when_read_data = 0;
	U8 shake_count_when_eint = 0;
	U16 sleep_time_minute_threshold = 0;
	U8 shake_time_threshold = 0;
	U8 shake_count_threshold = 0;
	
	config_service_get(CFG_SLEEP_TIME, TYPE_SHORT, &sleep_time_minute_threshold, sizeof(sleep_time_minute_threshold));
	config_service_get(CFG_AWAKE_CHECK_TIME, TYPE_BYTE, &shake_time_threshold, sizeof(shake_time_threshold));
	config_service_get(CFG_AWAKE_COUNT, TYPE_BYTE, &shake_count_threshold, sizeof(shake_count_threshold));
	if (shake_count_threshold > G_SHAKE_BUF_LEN)
	{
		shake_count_threshold = G_SHAKE_BUF_LEN;
	}

	for (index = 0; index < shake_count_threshold; ++index)
	{
		if(!circular_queue_get_by_index_i(&s_gsensor.shake_event_time_queue_when_read, index, (S32*)&shake_time))
		{
			break;
		}

		if ((now - shake_time) <= shake_time_threshold)
		{
			shake_count_when_read_data++;
		}
	}

	for (index = 0; index <  shake_count_threshold; ++index)
	{
		if(!circular_queue_get_by_index_i(&s_gsensor.shake_event_time_queue_when_eint, index, (S32*)&shake_time))
		{
			break;
		}

		if ((now - shake_time) <= shake_time_threshold)
		{
			shake_count_when_eint++;
		}
	}

	if (shake_count_when_read_data >= shake_count_threshold || shake_count_when_eint >= shake_count_threshold) 
	{
		s_gsensor.no_shake_time_ms = 0;
		if(GM_SYSTEM_STATE_SLEEP == system_state_get_work_state())
		{
			if(GM_SUCCESS == gps_power_on(true))
			{
				system_state_set_work_state(GM_SYSTEM_STATE_WORK);
				led_set_gsm_state(GM_LED_FLASH);
				
				GM_StartTimer(GM_TIMER_10MS_MAIN, TIM_GEN_10MS, timer_10ms_proc);
				GM_StartTimer(GM_TIMER_1S_MAIN, TIM_GEN_1SECOND, timer_1s_proc);
				LOG(INFO,"awake,shake_count_when_read_data=%d",shake_count_when_read_data);
			}
		}
	}
	else
	{
	    if (GM_SYSTEM_STATE_SLEEP == system_state_get_work_state())
		{
			s_gsensor.no_shake_time_ms += TIM_GEN_1SECOND;
		}
		else
		{
			s_gsensor.no_shake_time_ms += TIM_GEN_10MS;
		}
		
	}

	//sleep_time_minute_threshold为0不休眠
	if (s_gsensor.no_shake_time_ms >= sleep_time_minute_threshold * SECONDS_PER_MIN * MSEC_PER_SECONDS && 0 != sleep_time_minute_threshold)
	{
		if(GM_SYSTEM_STATE_WORK == system_state_get_work_state())
		{
			if(GM_SUCCESS == gps_power_off())
			{
				system_state_set_work_state(GM_SYSTEM_STATE_SLEEP);
				led_set_gsm_state(GM_LED_OFF);

			}
			s_gsensor.sleep_time = s_gsensor.no_shake_time_ms;
		}
	}

	if (GM_SYSTEM_STATE_SLEEP == system_state_get_work_state())
	{
		s_gsensor.sleep_time += TIM_GEN_1SECOND;
		s_gsensor.awake_time = 0;
	}
	else
	{
		s_gsensor.awake_time += TIM_GEN_10MS;
		s_gsensor.sleep_time = 0;
	}
}

// 震动报警和恢复,两次报警之间间隔30分钟
// 1、有震动:10秒内5次以上传感器中断（用震动时间队列解决）
// 2、自动设防（GS03,ACC OFF 5分钟后设防）:      有震动就延迟5秒后检查ACC状态再报
// 3、手动设防(GS05):有震动并且已设防就立即报警
// 修改了上一个版本的默认参数
static void check_shake_alarm(void)
{
	
	U8 index = 0;
	U32 shake_time = 0;
	U8 shake_count_when_read = 0;
	U8 shake_check_interval = 10;
	U8 shake_count_threshold = 5;
	bool is_defence_by_hand = false;
	U16 shake_alarm_delay = 30;
	U32 now = util_clock();
	
	config_service_get(CFG_SHAKE_CHECK_TIME, TYPE_BYTE, &shake_check_interval, sizeof(shake_check_interval));
	config_service_get(CFG_SHAKE_COUNT, TYPE_BYTE, &shake_count_threshold, sizeof(shake_count_threshold));
	config_service_get(CFG_IS_MANUAL_DEFENCE,TYPE_BOOL, &is_defence_by_hand, sizeof(is_defence_by_hand));
	config_service_get(CFG_SHAKE_ALARM_DELAY, TYPE_SHORT, &shake_alarm_delay, sizeof(shake_alarm_delay));
	if (shake_count_threshold > G_SHAKE_BUF_LEN)
	{
		shake_count_threshold = G_SHAKE_BUF_LEN;
	}
	
	config_service_get(CFG_IS_MANUAL_DEFENCE,TYPE_BOOL, &is_defence_by_hand, sizeof(is_defence_by_hand));

	
	for (index = 0; index <  shake_count_threshold; ++index)
	{
		circular_queue_get_by_index_i(&s_gsensor.shake_event_time_queue_when_read, index, (S32*)&shake_time);

		if ((now - shake_time) <= shake_check_interval)
		{
			shake_count_when_read++;
		}
	}

	//没有震动
	if (shake_count_when_read < shake_count_threshold) 
	{
		return;
	}
	//没设防
	if (!system_state_get_defence())
	{
		return;
	}
	
	//手动设防,立即上报
	if (is_defence_by_hand)
	{
		report_shake_alarm();
	}
	else
	{
		//自动设防模式,延迟30秒后检查ACC状态后再报,如果ACC状态为OFF就不报了
		GM_StartTimer(GM_TIMER_GSENSOR_CHECK_ACC_AFTER_SHAKE, shake_alarm_delay*TIM_GEN_1SECOND, check_acc_and_report_shake_alarm);
		LOG(INFO,"After %d seconds check_acc_and_report_shake_alarm",shake_alarm_delay);
	}
	
		
}

static void report_shake_alarm(void)
{
	U32 now = util_clock();
	U16 shake_alarm_interval = 30 * SECONDS_PER_MIN; 
	AlarmInfo alarm_info;
	
	config_service_get(CFG_SHAKE_ALARM_INTERVAL,TYPE_SHORT, &shake_alarm_interval, sizeof(shake_alarm_interval));

	//刚刚启动不报震动报警
	if(now <= 10)
	{
		return;
	}

	if ((now - s_gsensor.last_report_shake_alarm_time >= shake_alarm_interval) || 0 == s_gsensor.last_report_shake_alarm_time)
	{
		alarm_info.type = ALARM_SHOCK;
		gps_service_push_alarm(&alarm_info);
		system_state_set_shake_alarm(true);
		s_gsensor.last_report_shake_alarm_time = now;
		LOG(INFO,"report_shake_alarm now(clock:%d)",now);
	}
	else
	{
		LOG(DEBUG,"do not report_shake_alarm,last_report_shake_alarm_time=%d",s_gsensor.last_report_shake_alarm_time);
	}
}

static void check_acc_and_report_shake_alarm(void)
{  
	bool acc_is_on = false;
	if (hard_ware_get_acc_line_level(&acc_is_on))
	{
		return;
	}
	if(acc_is_on)
	{
		return;
	}
	report_shake_alarm();
}

static bool is_known_type(const GSensorType type)
{
	if (GSENSOR_TYPE_SC7A20 == type || GSENSOR_TYPE_BMA253 == type || GSENSOR_TYPE_DA213 == type)
	{
		return true;
	}
	else
	{
		return false;
	}
}

static void g_sensor_eint_callback(void* v_arg)
{
	U32 now = util_clock();
	bool shake_alarm_enable = false;

	U32 last_shake_time = 0;
	circular_queue_get_tail_i(&s_gsensor.shake_event_time_queue_when_eint, (S32*)&last_shake_time);
	//不是同一秒的才入对列
	if (now > last_shake_time)
	{
		circular_queue_en_queue_i(&s_gsensor.shake_event_time_queue_when_eint, now);
		LOG(INFO, "Shake once in EINT(now=%d)",now);
	}
	
	if(config_service_get(CFG_IS_SHAKE_ALARM_ENABLE, TYPE_BOOL, &shake_alarm_enable, sizeof(shake_alarm_enable)))
	{
		return;
	}
	
	if (shake_alarm_enable)
	{
		check_shake_alarm();
	}
}
static void init_gsensor_chip(void)
{
	// GSEN_PIN 中断脚  拉高(KAL_TRUE)就触发g_sensor_eint_callback 
	// EDGE_SENSITIVE true:边缘触发 fase:电平触发
    GM_EintPinInit(GSEN_PIN);
    GM_GpioSetPullSelection(GSEN_PIN, PINPULLSEL_PULLUP);
    GM_EintSetPolarity(GSEN_EINT, KAL_TRUE);
    GM_EintRegister(GSEN_EINT, g_sensor_eint_callback, KAL_TRUE, EDGE_SENSITIVE);
	
	GM_I2cInit(HW_I2C, GOOME_I2C_ADDR|I2C_RD,  GOOME_I2C_ADDR|I2C_WR, I2C_SCL, I2C_SDA);
    GM_I2cConfig(GOOME_I2C_ADDR, G_SENSOR_SAMPLE_FREQ);
	GM_I2cSetMode(HW_I2C);

	GM_I2cReadBytes(SC7A20_REG_ADDR, &s_gsensor.type, 1);
	if (GSENSOR_TYPE_SC7A20 == s_gsensor.type)
	{
		LOG(INFO, "gsensor type SC7A20");
		system_state_set_gsensor_type(s_gsensor.type);
		write_config_to_chip(GSENSOR_TYPE_SC7A20);
		return;
	}
	
	//BMA253
	GM_I2cReadBytes(BMA253_REG_ADDR, &s_gsensor.type, 1);
    if (GSENSOR_TYPE_BMA253 == s_gsensor.type)
	{
	    LOG(INFO, "gsensor type BMA253");
		system_state_set_gsensor_type(s_gsensor.type);
		write_config_to_chip(GSENSOR_TYPE_BMA253);
		return;
	}
	
	//DA213与其他2种芯片初始化不同
	GM_I2cInit(HW_I2C, DA213_I2C_ADDR|I2C_RD,  DA213_I2C_ADDR|I2C_WR, I2C_SCL, I2C_SDA);
    GM_I2cConfig(GOOME_I2C_ADDR, G_SENSOR_SAMPLE_FREQ);

	GM_I2cReadBytes(DA213_REG_ADDR, &s_gsensor.type, 1);
	if (GSENSOR_TYPE_DA213 == s_gsensor.type)
	{
		LOG(INFO, "gsensor type DA213");
		system_state_set_gsensor_type(s_gsensor.type);
		write_config_to_chip(GSENSOR_TYPE_DA213);
		return;
	}

	//走到这里的时候一定是出错了
	LOG(INFO, "gsensor init error!");
	s_gsensor.rw_error_count++;
	
}

static void read_data_from_chip(void)
{
	Aclr aclr = {0};
	if(!GM_I2cReadBytes(get_data_addr(s_gsensor.type), (U8*)&aclr, sizeof(aclr)))
	{
        LOG(WARN, "Failed to read data from gsensor!");
        s_gsensor.rw_error_count++;
        return;
    }
	s_gsensor.rw_error_count = 0;
	aclr.x = aclr.x >> 4;
	aclr.y = aclr.y >> 4;
	aclr.z = aclr.z >> 4;

	s_gsensor.read_data_count++;
	check_shake_event(aclr);
	check_awake_sleep();

	if(GM_SYSTEM_STATE_WORK == system_state_get_work_state())
	{
		check_vehicle_state(aclr);
	}
	
}

static void check_shake_event(Aclr current_aclr)
{
	static Aclr last_aclr = {0,0,0};
	U32 now = util_clock();
	
	float x_diff = abs(last_aclr.x - current_aclr.x) * G_SENSOR_RANGE / G_SENSOR_MAX_VALUE * 1000;
	float y_diff = abs(last_aclr.y - current_aclr.y) * G_SENSOR_RANGE / G_SENSOR_MAX_VALUE * 1000;
	float z_diff = abs(last_aclr.z - current_aclr.z) * G_SENSOR_RANGE / G_SENSOR_MAX_VALUE * 1000;
	if (x_diff >= s_gsensor.threshold.shake_threshold || y_diff >= s_gsensor.threshold.shake_threshold || z_diff >= s_gsensor.threshold.shake_threshold)
	{
		U32 last_shake_time = 0;
		circular_queue_get_tail_i(&s_gsensor.shake_event_time_queue_when_read, (S32*)&last_shake_time);
		//不是同一秒的才入对列
		if (now > last_shake_time)
		{
			circular_queue_en_queue_i(&s_gsensor.shake_event_time_queue_when_read, now);
			LOG(INFO, "Shake once(%f,%f,%f,now=%d)",x_diff,y_diff,z_diff,now);
		}
	}
	last_aclr.x = current_aclr.x;
	last_aclr.y = current_aclr.y;
	last_aclr.z = current_aclr.z;
}


static U8 get_data_addr(const GSensorType type)
{
	U8 reg_addr = 0;
	switch(type)
    {
		case GSENSOR_TYPE_SC7A20:
        {
			reg_addr = 0xA8;
        }
			break;
		
        case GSENSOR_TYPE_BMA253:
        {
			reg_addr = BMA253_X_AXIS_LSB_REG;
        }
            break;

        case GSENSOR_TYPE_DA213:
        {
            reg_addr = DA213_AXIS_NEW_DATA_ADDR;
        }
            break;

        default:
			reg_addr = 0;
            break;
    }
	return reg_addr;
}

static void write_config_to_chip(const GSensorType type)
{
    switch(type)
    {
		case GSENSOR_TYPE_SC7A20:
        {
			//  100HZ,0x67=103
            GM_I2cWriteByte(0x20,  0x67);  

			//  高精度模式 //2G
            GM_I2cWriteByte(0x23,  0x88);   

			//  高精度模式
            GM_I2cWriteByte(0x21,  0x31);   
            GM_I2cWriteByte(0x22,  0x40); 
            GM_I2cWriteByte(0x25,  0x00);
            GM_I2cWriteByte(0x24,  0x00);
            GM_I2cWriteByte(0x30,  0x2A);
			if (s_gsensor.threshold.shake_level >= 2)
			{
				GM_I2cWriteByte(0x32, s_gsensor.threshold.shake_level - 1);
			}
			else
			{
				GM_I2cWriteByte(0x32, 1);
			}
            
            GM_I2cWriteByte(0x33,  0x00);   //持续时间
        }
            break;
		 
        case GSENSOR_TYPE_BMA253:
        {
			//0x0B 62.5 hz
			//0x0C 125hz
            GM_I2cWriteByte(BMA253_BW_SEL_REG,  0x0C);

			//+- 2g
            GM_I2cWriteByte(BMA253_RANGE_SEL_REG, 0x03);    
            
            GM_I2cWriteByte(BMA253_INT_CTRL_REG, 0x88);
            GM_I2cWriteByte(BMA253_INT_ENABLE1_REG, 0x07);
            GM_I2cWriteByte(BMA253_INT1_PAD_SEL_REG, 0x04);
            GM_I2cWriteByte(BMA253_INT_SRC_REG, 0x00);
            GM_I2cWriteByte(BMA253_INT_CTRL_REG, 0x80);
            GM_I2cWriteByte(BMA253_SLOPE_DURN_REG, 0x00);
            GM_I2cWriteByte(BMA253_SLOPE_THRES_REG, s_gsensor.threshold.shake_level);
        }
            break;

        case GSENSOR_TYPE_DA213:
        {
            GM_I2cWriteByte(DA213_POWERMODE_BW_ADDR, 0x90);
            GM_I2cWriteByte(DA213_MOTION_FLAG_ADDR, 0x04);   //使能中断
            GM_I2cWriteByte(DA213_ANY_ACTIVE_ADDR, 0x07);    //使能XYZ中断
            GM_I2cWriteByte(DA213_RANGE_SELECT_ADDR,  0x03); //  高精度模式 //14bit 2G
            GM_I2cWriteByte(DA213_BW_SELECT_ADDR, 0x07);     //125hz
            GM_I2cWriteByte(DA213_INTR_ENABLE1_ADDR, 0x07);  
            GM_I2cWriteByte(DA213_INTMAP1_ADDR, 0x04);
            GM_I2cWriteByte(DA213_INTR_SET_ADDR, 0x01);
            GM_I2cWriteByte(DA213_INTR_CTRL_ADDR, 0x00);
            GM_I2cWriteByte(DA213_SLOPE_DURN_ADDR, 0x00);    //持续时间
            GM_I2cWriteByte(DA213_SLOPE_THRES_ADDR, s_gsensor.threshold.shake_level - 1);
            GM_I2cWriteByte(DA213_POWERMODE_BW_ADDR, 0x10);
        }

            break;

        default:
            break;
    }
}

//每10ms调用一次
void check_vehicle_state(Aclr a)
{
    //实时加速度（含重力）
    Vector3D sensor_aclr = {0, 0, 0};
    //水平方向加速度（不含重力）
    Vector3D vehicle_horizontal_aclr = {0, 0, 0};
    //垂直方向加速度（不含重力）
    Vector3D vehicle_vertical_aclr = {0, 0, 0};
    float vehicle_horizontal_aclr_magnitude = 0;
    float vehicle_vertical_aclr_magnitude = 0;
	bool aclr_alarm_enable = false;
	
    
    circular_queue_en_queue_i(&(s_gsensor.aclr_x_queue), a.x);
    circular_queue_en_queue_i(&(s_gsensor.aclr_y_queue), a.y);
    circular_queue_en_queue_i(&(s_gsensor.aclr_z_queue), a.z);

    sensor_aclr.x = a.x;
    sensor_aclr.y = a.y;
    sensor_aclr.z = a.z;

	config_service_get(CFG_IS_ACLRALARM_ENABLE, TYPE_BOOL, &aclr_alarm_enable, sizeof(aclr_alarm_enable));

	if (aclr_alarm_enable)
	{
		check_collision(sensor_aclr);
	}
    calculate_moving_avg(sensor_aclr, G_SENSOR_SAMPLE_FREQ, &(s_gsensor.senor_aclr_moving_avg), &(s_gsensor.cal_senor_aclr_avg_len));

    //还没学好
    if (s_gsensor.study_aclr_count < MIN_STUDY_GRAVITY_TIMES)
    {
		//保持不变
		//system_state_set_vehicle_state(VEHICLE_STATE_RUN);
    }
    //学习完毕
    else
    {
    	get_vehicle_acceleration(s_gsensor.gravity, s_gsensor.senor_aclr_moving_avg, &vehicle_horizontal_aclr, &vehicle_vertical_aclr);
   
        //检测是否由静止转为运动
        vehicle_horizontal_aclr_magnitude = applied_math_get_magnitude_3d(vehicle_horizontal_aclr) * G_SENSOR_RANGE / G_SENSOR_MAX_VALUE;
        check_static_or_run(vehicle_horizontal_aclr_magnitude);

		if (aclr_alarm_enable)
		{
			check_rapid_acceleration(vehicle_horizontal_aclr_magnitude);

	        check_emergency_brake(vehicle_horizontal_aclr_magnitude);

	        check_sudden_turn_acceleration(vehicle_vertical_aclr_magnitude);
		}     
    }
	
    study_gravity(a);
	
	if (s_gsensor.start_time % TIM_GEN_1SECOND == 0)
	{
		calc_sensor_angle();
		if (aclr_alarm_enable)
		{
			check_angle_alarm();
		}
	}
}

static void calc_sensor_angle(void)
{
    U8 sensor_angle = 0;
	Vector3D z_axis = {0, 0, -1};
    sensor_angle = applied_math_get_angle_3d(s_gsensor.senor_aclr_moving_avg, z_axis);  
    circular_queue_en_queue_i(&s_gsensor.sensor_angle_queue, sensor_angle);
}

/**
 * Function:   检查角度变化引起的报警（翻车报警、拆动报警）
 * Description:1、默认关闭，由固件管理平台配置
 *             2、启动2分钟内不报警（包括重启）
 *             3、保存最近1分钟角度记录（每秒1条，共60条）
 *             4、当前角度与历史记录差大于TURN_OVER_ANGLE并且当前状态为静止，判定为翻车报警
 *             5、当前角度与历史记录差大于REMOVE_ANGLE并且当前状态为运动，判定为拆动报警
 *             6、以上两种报警在1分钟记录内只报一条（通过限定上报间隔大于ANGLE_RECORD_NUM实现）,防止一次大的动作上报多次报警
 * Input:	   无
 * Output:	   无
 * Return:	   无
 * 测试方法:	   1、拆动：启动2分钟后，将定位器变换超过30°（由于误差存在，操作时角度要大于判断角度20°以上），要触发拆动报警
 *             2、翻车：启动2分钟后，将定位器变换超过45°（由于误差存在，操作时角度要大于判断角度20°以上），然后保持完全静止1分钟以上，要触发翻车报警
 *             3、不误报：
 *                1）无论什么安装角度，启动后不动不能报警
 *                2）水平晃动和垂直震动不能报警（车辆行驶）
 *                3）激烈驾驶（急加速、急减速）不能报警
 */

static void check_angle_alarm(void)
{
	U8 index = 0;
	S32 new_angle = 0;
	S32 last_angle = 0;
	U8 angle_diff = 0;
	U8 max_diff = 0;
	AlarmInfo alarm_info;
	U32 now = util_clock();

	//启动4分钟内角度不准确不检查角度报警
	if (now < 2*ANGLE_RECORD_NUM)
	{
		return;
	}

	if(false == circular_queue_get_tail_i(&(s_gsensor.sensor_angle_queue),&new_angle))
	{
		return;
	}

	LOG(DEBUG,"now(%d) angle:%d",util_clock(),new_angle);

	for(index = 1;index < ANGLE_RECORD_NUM;index++)
	{
		//如果找不到历史记录退出循环
		if(false == circular_queue_get_by_index_i(&(s_gsensor.sensor_angle_queue),index,&last_angle))
		{
			break;
		}
		angle_diff = applied_math_get_angle_diff(new_angle,last_angle);
		if(angle_diff > max_diff)
		{
			max_diff = angle_diff;
		}
	}

	if (max_diff > TURN_OVER_ANGLE && VEHICLE_STATE_STATIC == system_state_get_vehicle_state() 
		&& (now - s_gsensor.last_report_turnover_alarm_time) > ANGLE_RECORD_NUM)
	{
		LOG(FATAL,"TURN_OVER_ANGLE:%d",max_diff);
		alarm_info.type = ALARM_TURN_OVER;
		alarm_info.info = max_diff;
		gps_service_push_alarm(&alarm_info);
		system_state_set_turn_over_alarm(true);
		s_gsensor.last_report_turnover_alarm_time = now;
	}
	else if(max_diff > REMOVE_ANGLE && VEHICLE_STATE_RUN == system_state_get_vehicle_state() 
		&& (now - s_gsensor.last_report_remove_alarm_time) > ANGLE_RECORD_NUM)
	{
		LOG(FATAL,"ALARM_REMOVE:%d",max_diff);
		alarm_info.type = ALARM_REMOVE;
		alarm_info.info = max_diff;
		gps_service_push_alarm(&alarm_info);
		system_state_set_remove_alarm(true);
		s_gsensor.last_report_remove_alarm_time = now;
	}
	else
	{
	}

}
static void check_collision(Vector3D sensor_aclr)
{
	GM_ERRCODE ret = GM_SUCCESS;
	AlarmInfo alarm_info;
    float sensor_alcr_mag = applied_math_get_magnitude_3d(sensor_aclr) * G_SENSOR_RANGE / G_SENSOR_MAX_VALUE;
    COLLISION_LEVEL level = NO_COLLISION;
    if (sensor_alcr_mag >= s_gsensor.threshold.serious_crash_thr)
    {
        s_gsensor.collision_aclr_count++;
        level = SERIOUS_COLLISION;
    }
    else if(sensor_alcr_mag >= s_gsensor.threshold.normal_crash_thr)
    {
        s_gsensor.collision_aclr_count++;
        level = NORMAL_COLLISION;
    }
    else if(sensor_alcr_mag >= s_gsensor.threshold.slight_crash_thr)
    {
        s_gsensor.collision_aclr_count++;
        level = SLIGHT_COLLISION;
    }
    else
    {
        s_gsensor.collision_aclr_count = 0;
        level = NO_COLLISION;
    }

	//记录最严重的一次碰撞
    if(level >= s_gsensor.collision_level)
    {
        s_gsensor.collision_level = level;
        gps_get_last_data(&(s_gsensor.last_collision_gps_info));
        s_gsensor.last_collision_aclr.x = sensor_aclr.x;
        s_gsensor.last_collision_aclr.y = sensor_aclr.y;
        s_gsensor.last_collision_aclr.z = sensor_aclr.z;
    }

	//3次（30ms)以上的高加速度才算碰撞,防止传感器突发错误数据误报
	if(s_gsensor.collision_aclr_count >= 3)
	{
		alarm_info.type = ALARM_COLLISION;
		alarm_info.info = s_gsensor.collision_level;
		s_gsensor.collision_level = NO_COLLISION;
		s_gsensor.collision_aclr_count = 0;
		ret = gps_service_push_alarm(&alarm_info);
		system_state_set_collision_alarm(true);
		if (GM_SUCCESS != ret)
		{
			LOG(ERROR, "Failed to gps_service_push_alarm(ALARM_COLLISION),ret=%d", ret);
		}
	}
}

static bool check_emergency_brake(float vehicle_horizontal_aclr_magnitude)
{
    if (vehicle_horizontal_aclr_magnitude >= s_gsensor.threshold.brake_thr)
    {
		GM_StartTimer(GM_TIMER_GSENSOR_CHECK_SPEED_AFTER_EMERGENCY_BRAKE, GM_TICKS_1_SEC * 5, report_emergency_brake_alarm);
        return true;
    }
    else
    {
        return false;
    }
}

//上报急刹车报警
static void report_emergency_brake_alarm(void)
{
    GPSData new_gpsinfo = {0};
    GPSData last_five_second_gpsinfo = {0};
	AlarmInfo alarm_info;
	GM_ERRCODE ret = GM_SUCCESS;
	
    if(!gps_get_last_data(&new_gpsinfo))
    {
        return;
    }
    if(!gps_get_last_n_senconds_data(5,&last_five_second_gpsinfo))
    {
        return;
    }
	
    //是急转弯,角度变化大于2倍MIN_ANGLE_RANGE
    if (last_five_second_gpsinfo.speed > MIN_CRUISE_SPEED && applied_math_get_angle_diff(new_gpsinfo.course,last_five_second_gpsinfo.course) > 2 * MIN_ANGLE_RANGE)
    {
        return;
    }
	
    //最近1秒的速度比5秒前的速度低20km/h以上,才判定为急减速
    if(new_gpsinfo.speed - last_five_second_gpsinfo.speed < -20)
    {
        alarm_info.type = ALARM_SPEED_DOWN;
		ret = gps_service_push_alarm(&alarm_info);
		system_state_set_speed_down_alarm(true);
		if (GM_SUCCESS != ret)
		{
			LOG(ERROR, "Failed to gps_service_push_alarm(ALARM_SPEED_DOWN),ret=%d", ret);
		}
    }
}


static bool check_rapid_acceleration(float vehicle_horizontal_aclr_magnitude)
{
    if (vehicle_horizontal_aclr_magnitude >= s_gsensor.threshold.rapid_aclr_thr)
    {
		GM_StartTimer(GM_TIMER_GSENSOR_CHECK_SPEED_AFTER_RAPID_ACCERATION, GM_TICKS_1_SEC * 5, report_rapid_aclr_alarm);
        return true;
    }
    else
    {
        return false;
    }
}


//获取急加速记录
static void report_rapid_aclr_alarm(void)
{
    GPSData new_gpsinfo = {0};
    GPSData last_five_second_gpsinfo = {0};
	AlarmInfo alarm_info;
	GM_ERRCODE ret = GM_SUCCESS;
	
    if(!gps_get_last_data(&new_gpsinfo))
    {
        return;
    }
    if(!gps_get_last_n_senconds_data(5,&last_five_second_gpsinfo))
    {
        return;
    }
	
    //是急转弯,角度变化大于2倍MIN_ANGLE_RANGE
    if (last_five_second_gpsinfo.speed > MIN_CRUISE_SPEED && applied_math_get_angle_diff(new_gpsinfo.course,last_five_second_gpsinfo.course) > 2*MIN_ANGLE_RANGE)
    {
        return;
    }

    //最近1秒的速度比5秒前的速度大10km/h以上,才判定为急加速
    if(new_gpsinfo.speed - last_five_second_gpsinfo.speed > 10)
    {
        alarm_info.type = ALARM_SPEED_UP;
		ret = gps_service_push_alarm(&alarm_info);
		system_state_set_speed_up_alarm(true);
		if (GM_SUCCESS != ret)
		{
			LOG(ERROR, "Failed to gps_service_push_alarm(ALARM_SPEED_UP),ret=%d", ret);
		}
    }
}

static bool check_sudden_turn_acceleration(float vehicle_horizontal_aclr_magnitude)
{
    if (vehicle_horizontal_aclr_magnitude >= s_gsensor.threshold.sudden_turn_thr)
    {
		GM_StartTimer(GM_TIMER_GSENSOR_CHECK_ANGLE_AFTER_SUDDEN_TURN, GM_TICKS_1_SEC * 5, report_sudden_turn_alarm);
        return true;
    }
    else
    {
        return false;
    }
}

static void report_sudden_turn_alarm(void)
{
    GPSData new_gpsinfo = {0};
    GPSData last_five_second_gpsinfo = {0};
    unsigned int index = 1;
    unsigned int direction_diff = 0;
	AlarmInfo alarm_info;
	GM_ERRCODE ret = GM_SUCCESS;

    if(!gps_get_last_data(&new_gpsinfo))
    {
        return;
    }
	
    for(index = 1;index <= 5;index++)
    {
        if (!gps_get_last_n_senconds_data(index, &last_five_second_gpsinfo))
        {
            return;
        }
        direction_diff = applied_math_get_angle_diff(new_gpsinfo.course,last_five_second_gpsinfo.course);
        //角度变化是在转弯的时候发生,本来加速度就会加大,角度变化超过10度/秒
        if (last_five_second_gpsinfo.speed > MIN_CRUISE_SPEED && direction_diff > MIN_ANGLE_RANGE/3.0 * index)
        {
            alarm_info.type = ALARM_SHARP_TURN;
			ret = gps_service_push_alarm(&alarm_info);
			system_state_set_sharp_turn_alarm(true);
			if (GM_SUCCESS != ret)
			{
				LOG(ERROR, "Failed to gps_service_push_alarm(ALARM_SHARP_TURN),ret=%d", ret);
			}
            break;
        }
        else
        {
            continue;
        }
    }
}


static void study_gravity(Aclr a)
{
    Vector3D sensor_aclr = {0,0,0};
    sensor_aclr.x = a.x;
    sensor_aclr.y = a.y;
    sensor_aclr.z = a.z;
    //1、首次执行
    //2、运行1年以上（计算滑动平均值有累积误差）
    if (s_gsensor.study_aclr_count == 0 || s_gsensor.study_aclr_count % ((U64)G_SENSOR_SAMPLE_FREQ * (U64)SECONDS_PER_YEAR) == 0)
    {
        //重置各项参数
        reset_sensor_data();
        //重置重力加速度初值
        s_gsensor.gravity.x = a.x;
        s_gsensor.gravity.y = a.y;
        s_gsensor.gravity.z = a.z;
        s_gsensor.senor_aclr_moving_avg.x = a.x;
        s_gsensor.senor_aclr_moving_avg.y = a.y;
        s_gsensor.senor_aclr_moving_avg.z = a.z;

        //清空计算滑动平均值的队列
        circular_queue_empty(&(s_gsensor.aclr_x_queue));
        circular_queue_empty(&(s_gsensor.aclr_y_queue));
        circular_queue_empty(&(s_gsensor.aclr_z_queue));

        //最新的加速度值入队列
        circular_queue_en_queue_i(&(s_gsensor.aclr_x_queue), a.x);
        circular_queue_en_queue_i(&(s_gsensor.aclr_y_queue), a.y);
        circular_queue_en_queue_i(&(s_gsensor.aclr_z_queue), a.z);
    }
    else
    {
        //长距离滑动平均得到重力和固定误差
        calculate_moving_avg(sensor_aclr, G_SENSOR_BUF_LEN - 1, &(s_gsensor.gravity), &(s_gsensor.cal_gravity_avg_len));
        if (VEHICLE_STATE_STATIC == system_state_get_vehicle_state())
        {
            s_gsensor.static_gravity = s_gsensor.gravity;
        }
    }
    s_gsensor.study_aclr_count++;
}


/*****************************************************************
功能:分解加速度到竖直方向和水平方向（传感器坐标系）
输入参数:g——滤波后重力加速度
输入参数:sensor_aclr——滤波后传感器加速度
输出参数:vehicle_horizontal_aclr——指向汽车水平方向加速度的指针
输出参数:vehicle_vertical_aclr——指向汽车垂直方向加速度的指针
返回值:0——成功；其它值错误
*****************************************************************/
static GM_ERRCODE get_vehicle_acceleration(Vector3D g, Vector3D sensor_aclr, Vector3D *vehicle_horizontal_aclr, Vector3D *vehicle_vertical_aclr)
{
    Vector3D B1 = {0, 0, 0};
    Vector3D B2 = {0, 0, 0};
    Vector3D B3 = {0, 0, 0};

    Vector3D B11 = {0, 0, 0};
    Vector3D B12 = {0, 0, 0};
    Vector3D B21 = {0, 0, 0};
    Vector3D B22 = {0, 0, 0};
    Vector3D B31 = {0, 0, 0};
    Vector3D B32 = {0, 0, 0};

    float s = 0;
    float t = 0;

    //判断参数合法性
    if (NULL == vehicle_horizontal_aclr || NULL == vehicle_vertical_aclr)
    {
        return GM_PARAM_ERROR;
    }
    //传感器加速度减去重力和固有误差
    sensor_aclr.x -= g.x;
    sensor_aclr.y -= g.y;
    sensor_aclr.z -= g.z;

    //传感器加速度x分量在水平面的投影
    s = g.x * g.x + g.y * g.y + g.z * g.z;
    t = (g.x * sensor_aclr.x) / s;
    B11.x = sensor_aclr.x - g.x * t;
    B11.y = 0 - g.y * t;
    B11.z = 0 - g.z * t;
    B12.x = B1.x - B11.x;
    B12.y = B1.y - B11.y;
    B12.z = B1.z - B11.z;

    //传感器加速度y分量在水平面的投影
    t = (g.y * sensor_aclr.y) / s;
    B21.x = 0 - g.x * t;
    B21.y = sensor_aclr.y - g.y * t;
    B21.z = 0 - g.z * t;
    B22.x = B2.x - B21.x;
    B22.y = B2.y - B21.y;
    B22.z = B2.z - B21.z;

    //传感器加速度z分量在水平面的投影
    t = (g.z * sensor_aclr.z) / s;
    B31.x = 0 - g.x * t;
    B31.y = 0 - g.y * t;
    B31.z = sensor_aclr.z - g.z * t;
    B32.x = B3.x - B31.x;
    B32.y = B3.y - B31.y;
    B32.z = B3.z - B31.z;

    //车辆加速度的水平分量
    vehicle_horizontal_aclr->x = B11.x + B21.x + B31.x;
    vehicle_horizontal_aclr->y = B11.y + B21.y + B31.y;
    vehicle_horizontal_aclr->z = B11.z + B21.z + B31.z;

    //车辆加速度的垂直分量
    vehicle_vertical_aclr->x = B12.x + B22.x + B32.x;
    vehicle_vertical_aclr->y = B12.y + B22.y + B32.y;
    vehicle_vertical_aclr->z = B12.z + B22.z + B32.z;

    return GM_SUCCESS;
}


//滑动平均
static void calculate_moving_avg(Vector3D sensor_aclr, int num, PVector3D p_aclr_moving_avg, unsigned int *p_len)
{
    int head_x = 0;
    int head_y = 0;
    int head_z = 0;
    int len = circular_queue_get_len(&(s_gsensor.aclr_x_queue));
    if (NULL == p_aclr_moving_avg || NULL == p_len || num <= 0 || num > circular_queue_get_capacity(&(s_gsensor.aclr_x_queue)))
    {
        return;
    }
    if (circular_queue_is_empty(&(s_gsensor.aclr_x_queue)))
    {
        *p_aclr_moving_avg = sensor_aclr;
        *p_len = 1;
    }
    //不满
    else if (num >= len)
    {
        p_aclr_moving_avg->x = (sensor_aclr.x + (len - 1) * p_aclr_moving_avg->x) / (len);
        p_aclr_moving_avg->y = (sensor_aclr.y + (len - 1) * p_aclr_moving_avg->y) / (len);
        p_aclr_moving_avg->z = (sensor_aclr.z + (len - 1) * p_aclr_moving_avg->z) / (len);
        *p_len = len;
        //printf("len = %d,x = %f,y=%f,z=%f\n",*p_len,p_aclr_moving_avg->x,p_aclr_moving_avg->y,p_aclr_moving_avg->z);
    }
    //满了
    else if (num < len)
    {
        circular_queue_get_by_index_i(&(s_gsensor.aclr_x_queue), num, &head_x);
        circular_queue_get_by_index_i(&(s_gsensor.aclr_y_queue), num, &head_y);
        circular_queue_get_by_index_i(&(s_gsensor.aclr_z_queue), num, &head_z);
        p_aclr_moving_avg->x = (sensor_aclr.x + (*p_len) * p_aclr_moving_avg->x - head_x) / num;
        p_aclr_moving_avg->y = (sensor_aclr.y + (*p_len) * p_aclr_moving_avg->y - head_y) / num;
        p_aclr_moving_avg->z = (sensor_aclr.z + (*p_len) * p_aclr_moving_avg->z - head_z) / num;
        *p_len = num;
        //printf("len = %d, head.x = %f, head.y = %f, head.z = %f,x = %f,y=%f,z=%f\n",*p_len,head_x,head_y,head_z,p_aclr_moving_avg->x,p_aclr_moving_avg->y,p_aclr_moving_avg->z);
    }
    else
    {
        //不存在这种情况
    }
}


static void check_static_or_run(float vehicle_horizontal_aclr_magnitude)
{
	//不过不休眠，也不判断静止	
	U16 sleep_time_minute_threshold = 0;
	config_service_get(CFG_SLEEP_TIME, TYPE_SHORT, &sleep_time_minute_threshold, sizeof(sleep_time_minute_threshold));
	if(0 == sleep_time_minute_threshold)
	{
		system_state_set_vehicle_state(VEHICLE_STATE_RUN);
		return;
	}
	
    //1、正常行驶:加速度（传感器）超过阈值
    if (vehicle_horizontal_aclr_magnitude >= s_gsensor.threshold.run_thr)
    {
        s_gsensor.static_low_aclr_count = 0;
        s_gsensor.super_low_aclr_count = 0;
		if (VEHICLE_STATE_STATIC == system_state_get_vehicle_state())
		{
			bool move_alarm_enable = false;
			config_service_get(CFG_IS_MOVEALARM_ENABLE, TYPE_BOOL, &move_alarm_enable, sizeof(move_alarm_enable));
			//自从休眠唤醒以后还没有上报过移动报警
			if (move_alarm_enable && !system_state_get_move_alarm())
			{
				AlarmInfo alarm_info;
				alarm_info.type = ALARM_MOVE;
	  			gps_service_push_alarm(&alarm_info);
	  			system_state_set_move_alarm(true);
				LOG(WARN,"MOVE ALARM");
			}
			system_state_set_vehicle_state(VEHICLE_STATE_RUN);
			GM_StartTimer(GM_TIMER_BMS_TRANSPRENT, 1000, bms_transprent_callback);
		}
		LOG(DEBUG,"RUN:aclr=%f,thr=%f",vehicle_horizontal_aclr_magnitude,s_gsensor.threshold.run_thr);
  	}
    //2、运动转为静止
    else if (vehicle_horizontal_aclr_magnitude <= s_gsensor.threshold.static_thr)
    {
        s_gsensor.static_low_aclr_count++;
        //MIN_LOW_ACLR_NUM次加速度小于阈值才判定为静止了
        if (s_gsensor.static_low_aclr_count >= MIN_LOW_ACLR_NUM)
        {
			system_state_set_vehicle_state(VEHICLE_STATE_STATIC);
        }
        //如果加速度特别低,连续10秒就变静止
        if (vehicle_horizontal_aclr_magnitude <= (s_gsensor.threshold.static_thr / 5))
        {
            s_gsensor.super_low_aclr_count++;
            if (s_gsensor.super_low_aclr_count >= MIN_LOW_ACLR_NUM / 2)
            {
				system_state_set_vehicle_state(VEHICLE_STATE_STATIC);
            }
        }
        else
        {
            s_gsensor.super_low_aclr_count = 0;
        }
    }
    //3、中间态
    else
    {
    }

    //4、匀速行驶:速度超过阈值,并且加速度（通过速度计算）小
    if ((gps_get_constant_speed_time() >= MIN_CRUISE_SPEED_TIME && gps_get_aclr() < s_gsensor.threshold.rapid_aclr_thr * GRAVITY_CONSTANT))
    {
        s_gsensor.static_low_aclr_count = 0;
        s_gsensor.super_low_aclr_count = 0;
		if (VEHICLE_STATE_STATIC == system_state_get_vehicle_state())
		{
			LOG(INFO,"RUN because of speed,time:%d,gps aclr:%f",gps_get_constant_speed_time(),gps_get_aclr());
			system_state_set_vehicle_state(VEHICLE_STATE_RUN);
			GM_StartTimer(GM_TIMER_BMS_TRANSPRENT, 1000, bms_transprent_callback);
		}
    }
}


