/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        hard_ware.h
 * Author:           王志华       
 * Version:          1.0
 * Date:             2019-03-01
 * Description:      简单硬件（开关）读写操作封装,包括读取ADC的数值
 * Others:      
 * Function List:    
    1. 创建hard_ware模块
    2. 销毁hard_ware模块
    3. 定时处理入口
    4. 获取ACC输入状态
    5. 设置GPS LED状态
    6. 设置GSM LED状态
    7. 设置断油电IO状态
    8. 获取供电电源电压值
    9. 设置内置电池充电状态（GS05）
 * History: 
    1. Date:         2019-03-01
       Author:       王志华
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#include <gm_time.h>
#include <gm_timer.h>
#include <gm_gpio.h>
#include <gm_adc.h>
#include <gm_power.h>
#include <gm_memory.h>
#include <gm_system.h>
#include <gm_stdlib.h>
#include "g_sensor.h"
#include "hard_ware.h"
#include "gps.h"
#include "uart.h"
#include "system_state.h"
#include "log_service.h"
#include "config_service.h"
#include "utility.h"
#include "gps_service.h"
#include "applied_math.h"
#include "auto_test.h"
#include "gsm.h"
#include "gps_service.h"

//输入:电池是否充满
#define GM_GPS_CHARGE_LEVLE_GPIO_PIN GM_GPIO4

//输出:充电控制
#define GM_GPS_CHARGE_CTROL_GPIO_PIN GM_GPIO5

//输出:GPS开关
#define GM_GPS_POWER_CTROL_GPIO_PIN  GM_GPIO19

//输出:断油电
#define GM_RELAY_CTROL_GPIO_PIN  GM_GPIO25

//输出:看门狗
#define GM_WDT_CTROL_GPIO_PIN  GM_GPIO28

//输入:ACC高低
#define GM_ACC_LEVEL_GPIO_PIN  GM_GPIO20

//输入:电源电压,ADC,通道ID
#define GM_POWER_VOLTAGE_ADC_CHANNEL 4

//计算电压平均值的次数
#define GM_POWER_VOLTAGE_CALC_NUM 10

#define GM_HIGH_VOLTAGE_ALARM_THRESHHOLD_90V 96

#define GM_HIGH_VOLTAGE_ALARM_THRESHHOLD_36V 36

#define GM_BATTERY_HIGHEST_VOLTAGE 7.0

typedef struct
{
	bool inited;
	//设备型号
	ConfigDeviceTypeEnum dev_type;

	//电源电压
	float power_voltage;

	//外部电池电量百分比（根据供电电压计算）
	float extern_battery_percent;

	//电池电压
	float battery_voltage;

	bool battery_is_charging;

	bool battery_is_full;

	//电池充电电流
	float battery_charge_current;

	S32 battery_charge_time;
	
	U16 timer_ms;

	StateRecord power_off_alarm_record;
	
	StateRecord low_voltage_alarm_record;

	StateRecord high_voltage_alarm_record;
	
	StateRecord acc_record;
	bool acc_line_is_valid;
	U32 acc_low_but_run_seconds;

	StateRecord battery_is_full_record;

	bool gps_led_is_on;
	bool gsm_led_is_on;
	
}HardWare, *PHardWare;

static HardWare s_hard_ware;

static GM_ERRCODE init_gpio(void);

static void hard_ware_read_voltage(void);

static U8 hard_ware_calc_extern_voltage_grade(float voltage);

static float hard_ware_calc_extern_battery_percent(float voltage, U8 voltage_grade);

static void hard_ware_check_high_voltage_alarm(void);

static void hard_ware_check_power_off_alarm(void);

static void hard_ware_check_battery(void);

static void hard_ware_check_acc(void);

static void hard_ware_set_auto_defence(void);

static void hard_ware_reboot_atonce(void);

static bool hard_ware_has_acc_line(const ConfigDeviceTypeEnum dev_type);

static bool hard_ware_has_battery(const ConfigDeviceTypeEnum dev_type);

/**
 * Function:   创建hard_ware模块
 * Description:创建hard_ware模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   管脚(参看电路图及管脚功能表)
 */
GM_ERRCODE hard_ware_create(void)
{
	GM_ERRCODE ret = GM_SUCCESS;

	if (s_hard_ware.inited)
	{
		return GM_SUCCESS;
	}

	ret = init_gpio();
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR,"Failed to init GPIO!");
	}
	
	s_hard_ware.inited = false;
	s_hard_ware.dev_type = DEVICE_NONE;
	s_hard_ware.power_voltage = 0.0;
	s_hard_ware.extern_battery_percent = 0.0;
	s_hard_ware.battery_is_charging = false;
	s_hard_ware.battery_is_full = false;
	s_hard_ware.battery_charge_current = 0;
	s_hard_ware.battery_charge_time = -1;
	s_hard_ware.timer_ms = 0;
	s_hard_ware.gps_led_is_on = false;
	s_hard_ware.gsm_led_is_on = false;

	LOG(DEBUG, "hard_ware_create");

	s_hard_ware.power_off_alarm_record.state = system_state_get_power_off_alarm();
	s_hard_ware.power_off_alarm_record.true_state_hold_seconds = 0;
	s_hard_ware.power_off_alarm_record.false_state_hold_seconds = 0;

	s_hard_ware.low_voltage_alarm_record.state = system_state_get_battery_low_voltage_alarm();
	s_hard_ware.low_voltage_alarm_record.true_state_hold_seconds = 0;
	s_hard_ware.low_voltage_alarm_record.false_state_hold_seconds = 0;

	s_hard_ware.high_voltage_alarm_record.state = system_state_get_high_voltage_alarm();
	s_hard_ware.high_voltage_alarm_record.true_state_hold_seconds = 0;
	s_hard_ware.high_voltage_alarm_record.false_state_hold_seconds = 0;

	s_hard_ware.acc_record.state = false;
	s_hard_ware.acc_record.true_state_hold_seconds = 0;
	s_hard_ware.acc_record.false_state_hold_seconds = 0;
	s_hard_ware.acc_line_is_valid = true;
	s_hard_ware.acc_low_but_run_seconds = 0;

	s_hard_ware.battery_is_full_record.state = true;
	s_hard_ware.battery_is_full_record.true_state_hold_seconds = 0;
	s_hard_ware.battery_is_full_record.false_state_hold_seconds = 0;
	

	s_hard_ware.inited = true;
	
	return GM_SUCCESS;
}

static GM_ERRCODE init_gpio(void)
{
	S32 ret = GM_SUCCESS;
	ret = GM_GpioInit(GM_GPS_CHARGE_LEVLE_GPIO_PIN, PINDIRECTION_IN, PINLEVEL_LOW, PINPULLSEL_DISABLE);
	if (GM_SUCCESS != ret)
	{
	   LOG(FATAL,"Failed to init GM_GPS_CHARGE_LEVLE_GPIO_PIN!");
	   return GM_HARD_WARE_ERROR;
	}

	ret = GM_GpioInit(GM_GPS_CHARGE_CTROL_GPIO_PIN, PINDIRECTION_OUT, PINLEVEL_HIGH, PINPULLSEL_DISABLE);
	if (GM_SUCCESS != ret)
	{
	   LOG(FATAL,"Failed to init GM_GPS_CHARGE_CTROL_GPIO_PIN!");
	   return GM_HARD_WARE_ERROR;
	}

	ret = GM_GpioInit(GM_GPS_POWER_CTROL_GPIO_PIN, PINDIRECTION_OUT, PINLEVEL_HIGH, PINPULLSEL_DISABLE);
	if (GM_SUCCESS != ret)
	{
	   LOG(FATAL,"Failed to init GM_GPS_POWER_CTROL_GPIO_PIN!");
	   return GM_HARD_WARE_ERROR;
	}

	ret = GM_GpioInit(GM_RELAY_CTROL_GPIO_PIN, PINDIRECTION_OUT, PINLEVEL_LOW, PINPULLSEL_PULLDOWN);
	if (GM_SUCCESS != ret)
	{
	   LOG(FATAL,"Failed to init GM_RELAY_CTROL_GPIO_PIN!");
	   return GM_HARD_WARE_ERROR;
	}

	ret = GM_GpioInit(GM_WDT_CTROL_GPIO_PIN, PINDIRECTION_OUT, PINLEVEL_LOW, PINPULLSEL_DISABLE);
	if (GM_SUCCESS != ret)
	{
	   LOG(FATAL,"Failed to init GM_WDT_CTROL_GPIO_PIN!");
	   return GM_HARD_WARE_ERROR;
	}

	ret = GM_GpioInit(GM_ACC_LEVEL_GPIO_PIN, PINDIRECTION_IN, PINLEVEL_HIGH, PINPULLSEL_PULLUP);
	if (GM_SUCCESS != ret)
	{
	   LOG(FATAL,"Failed to init GM_ACC_LEVEL_GPIO_PIN!");
	   return GM_HARD_WARE_ERROR;
	}
	return GM_SUCCESS;
}

/**
 * Function:   销毁hard_ware模块
 * Description:销毁hard_ware模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE hard_ware_destroy(void)
{
	s_hard_ware.inited = false;
	return GM_SUCCESS;
}

GM_ERRCODE hard_ware_timer_proc(void)
{
	U8 extern_battery_voltage_grade = 0;
    u16 device_type;

	
	if (false == s_hard_ware.inited)
	{
		return GM_NOT_INIT;
	}

	if (GM_SYSTEM_STATE_WORK == system_state_get_work_state())
	{
		s_hard_ware.timer_ms += TIM_GEN_10MS;
	}
	else
	{
		s_hard_ware.timer_ms += TIM_GEN_1SECOND;
	}
    
	//100ms读一次数据
	if (s_hard_ware.timer_ms % TIM_GEN_100MS == 0)
	{
		hard_ware_read_voltage();
	}

	//1秒以内不做其它处理
	if (s_hard_ware.timer_ms < TIM_GEN_1SECOND)
	{
		return GM_SUCCESS;
	}

	config_service_get(CFG_DEVICETYPE, TYPE_SHORT, &device_type , sizeof(device_type));
	
    s_hard_ware.dev_type = (ConfigDeviceTypeEnum)device_type;

	if (hard_ware_has_acc_line(s_hard_ware.dev_type))
	{
		hard_ware_check_acc();
	}
	else
	{
		system_state_set_acc_is_line_mode(false);
	}

	//设备类型未知的时候也要能充电
	if (hard_ware_has_battery(s_hard_ware.dev_type) || DEVICE_NONE == s_hard_ware.dev_type)
	{
		hard_ware_check_battery();
	}
	
	//检查电源电压过高报警
	hard_ware_check_high_voltage_alarm();

	//检查断电报警
	hard_ware_check_power_off_alarm();

	//计算外部电压等级并保存到系统状态中
	extern_battery_voltage_grade = hard_ware_calc_extern_voltage_grade(s_hard_ware.power_voltage);
	system_state_set_extern_battery_voltage_grade(extern_battery_voltage_grade);

	//计算外部电池百分比（根据电源电压估算）
	s_hard_ware.extern_battery_percent = hard_ware_calc_extern_battery_percent(s_hard_ware.power_voltage, extern_battery_voltage_grade);

	//毫秒计时器清零
	s_hard_ware.timer_ms = 0;
	return GM_SUCCESS;
}

static ConfigDeviceTypeEnum four_line_devtypes[] = 
{
    DEVICE_GS03A,
	DEVICE_AS03A,
    DEVICE_GS07A,
    DEVICE_AS07A,
    DEVICE_GS03H,
    DEVICE_GS05A, 
    DEVICE_GS05H,
    DEVICE_GM06E
};
	
static bool hard_ware_has_acc_line(const ConfigDeviceTypeEnum dev_type)
{
	U8 index = 0;
	for (index = 0; index < sizeof(four_line_devtypes)/sizeof(ConfigDeviceTypeEnum); ++index)
	{
		if (dev_type == four_line_devtypes[index])
		{
			return true;
		}
	}

	return false;
}

static ConfigDeviceTypeEnum battery_devtypes[] = 
{
    DEVICE_GS03A, //4线单sensor 有电池
    DEVICE_AS03A, //4线单sensor 有电池
    DEVICE_GS03F, //2线单sensor 有电池
    DEVICE_AS03F, //2线单sensor 有电池
    DEVICE_GS07B, //2线单sensor 有电池90V
    DEVICE_AS07B, //2线单sensor 有电池90V
    DEVICE_GS03H, //4线双sensor,同03A
    DEVICE_GS05A, //4线单sensor 90V,有电池
    DEVICE_GS05F, //2线单sensor 90V,有电池
    DEVICE_GS05H, //4线双sensor 90V,有电池
    DEVICE_GM06E, //同GS03A,客户定制
};

static bool hard_ware_has_battery(const ConfigDeviceTypeEnum dev_type)
{
	U8 index = 0;
	for (index = 0; index < sizeof(battery_devtypes)/sizeof(ConfigDeviceTypeEnum); ++index)
	{
		if (dev_type == battery_devtypes[index])
		{
			return true;
		}
	}
	return false;
}

GM_ERRCODE hard_ware_get_acc_level(bool* p_state)
{
	if (false == s_hard_ware.inited || NULL == p_state)
	{
		return GM_NOT_INIT;
	}
	if(system_state_get_acc_is_line_mode())
	{
		*p_state = s_hard_ware.acc_record.state;
		LOG(DEBUG,"ACC from line is %d",*p_state);
	}
	else
	{
		*p_state = (GM_SYSTEM_STATE_WORK == system_state_get_work_state());
		LOG(DEBUG,"ACC from sensor is %d",*p_state);
	}
	return GM_SUCCESS;
}

GM_ERRCODE hard_ware_get_acc_line_level(bool* p_state)
{
	if (false == s_hard_ware.inited || NULL == p_state)
	{
		return GM_NOT_INIT;
	}
	if(system_state_get_acc_is_line_mode())
	{
		*p_state = s_hard_ware.acc_record.state;
		LOG(DEBUG,"ACC from line is %d",*p_state);
	}
	else
	{
		*p_state = false;
		LOG(DEBUG,"ACC line is invalid,return false");
	}
	return GM_SUCCESS;
}

/**
 * Function:   设置GPS LED状态
 * Description:
 * Input:	   state:true——亮灯；false——灭灯
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE hard_ware_set_gps_led(bool is_on)
{
	S32 ret = 0;


	if (false == s_hard_ware.inited)
	{
		return GM_NOT_INIT;
	}

	if (s_hard_ware.gps_led_is_on == is_on)
	{
		return GM_SUCCESS;
	}
	else
	{
		s_hard_ware.gps_led_is_on = is_on;
			
		ret = GM_IsinkBacklightCtrl(is_on,is_on);
		if (0 == ret)
		{
			return GM_SUCCESS;
		}
		else
		{
			LOG(ERROR,"Failed to GM_IsinkBacklightCtrl,ret=%d!",ret);
			return GM_HARD_WARE_ERROR;
		}
	}
}

/**
 * Function:   设置GSM LED状态
 * Description:
 * Input:	   state:true——亮灯；false——灭灯
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE hard_ware_set_gsm_led(bool is_on)
{
	S32 ret = 0;
	if (false == s_hard_ware.inited)
	{
		return GM_NOT_INIT;
	}

	if (s_hard_ware.gsm_led_is_on == is_on)
	{
		return GM_SUCCESS;
	}
	else
	{
		s_hard_ware.gsm_led_is_on = is_on;

		ret = GM_KpledLevelCtrl(is_on,is_on);
		if (0 == ret)
		{
			return GM_SUCCESS;
		}
		else
		{
			LOG(ERROR,"Failed to GM_KpledLevelCtrl,ret=%d!",ret);
			return GM_HARD_WARE_ERROR;
		}
	}
}

/**
 * Function:   设置断油电IO状态
 * Description:
 * Input:	   state:true——断油电；false——恢复油电
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE hard_ware_set_relay(bool state)
{
	if (false == s_hard_ware.inited)
	{
		return GM_NOT_INIT;
	}
	GM_GpioSetLevel(GM_RELAY_CTROL_GPIO_PIN,(Enum_PinLevel)state);
	return GM_SUCCESS;
}

/**
 * Function:   设置看门狗IO电平
 * Description:
 * Input:	   state:true——高电平；false——低电平
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE hard_ware_set_watchdog(bool state)
{
	if (false == s_hard_ware.inited)
	{
		return GM_NOT_INIT;
	}
	//总是返回0,不需要检查是否执行成功
	GM_GpioSetLevel(GM_WDT_CTROL_GPIO_PIN, (Enum_PinLevel)state);
	return GM_SUCCESS;
}


/**
 * Function:   获取供电电源电压值
 * Description:单位伏特（v）
 * Input:	   无
 * Output:	   p_voltage:电压值指针
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE hard_ware_get_power_voltage(float* p_voltage)
{
	if (false == s_hard_ware.inited || NULL == p_voltage)
	{
		return GM_NOT_INIT;
	}
	*p_voltage = s_hard_ware.power_voltage;
	return GM_SUCCESS;
}

GM_ERRCODE hard_ware_get_internal_battery_voltage(float* p_voltage)
{
	if (false == s_hard_ware.inited || NULL == p_voltage)
	{
		LOG(ERROR,"Has not inited hardware!");
		return GM_NOT_INIT;
	}
	*p_voltage = s_hard_ware.battery_voltage;
	return GM_SUCCESS;
}

bool hard_ware_battery_is_charging(void)
{
	return s_hard_ware.battery_is_charging;
}

bool hard_ware_battery_is_full(void)
{
	return s_hard_ware.battery_is_full;
}


GM_ERRCODE hard_ware_get_extern_battery_percent(U8* p_percent)
{
	if (false == s_hard_ware.inited || NULL == p_percent)
	{
		return GM_NOT_INIT;
	}
	*p_percent = applied_math_round(s_hard_ware.extern_battery_percent);
	return GM_SUCCESS;
}

GM_ERRCODE hard_ware_get_internal_battery_percent(U8* p_percent)
{
	if (!s_hard_ware.inited)
	{
		return GM_NOT_INIT;
	}

	if (NULL == p_percent)
	{
		return GM_PARAM_ERROR;
	}
	
	if (s_hard_ware.battery_voltage >= 4.15)
	{
	    *p_percent = 100;
	}
	else if (s_hard_ware.battery_voltage <= 3.50)
	{
	    *p_percent = 0;
	}
	else
	{
	    *p_percent = applied_math_round((s_hard_ware.battery_voltage - 3.50) * 100 / 0.65);
	}
	return GM_SUCCESS;

}

GM_ERRCODE hard_ware_get_internal_battery_level(U8* p_level)
{
	if (!s_hard_ware.inited)
	{
		return GM_NOT_INIT;
	}

	if (NULL == p_level)
	{
		return GM_PARAM_ERROR;
	}
	
	if (s_hard_ware.battery_voltage <= 3.45f)
    {
        *p_level = 0;
    }
    else if (s_hard_ware.battery_voltage <= 3.5f)
    {
        *p_level = 1;
    }
    else if (s_hard_ware.battery_voltage <= 3.6f)
    {
        *p_level = 2;
    }
    else if (s_hard_ware.battery_voltage <= 3.8f)
    {
        *p_level = 3;
    }
    else if (s_hard_ware.battery_voltage <= 3.95f)
    {
        *p_level = 4;
    }
    else if (s_hard_ware.battery_voltage <= 4.1f)
    {
        *p_level = 5;
    }
    else
    {
        *p_level = 6;
    }
	return GM_SUCCESS;
}


static void hard_ware_read_voltage(void)
{
    u32 realtime_voltage = 0;
	float power_voltage = 0;
    if(0xFFFF == GM_AdcRead(GM_POWER_VOLTAGE_ADC_CHANNEL,&realtime_voltage))
    {
    	LOG(ERROR,"Failed to GM_AdcRead!");
    	return;
    }
	
    power_voltage = realtime_voltage * 145.6 /1024.0;
	
	//补偿二极管压降
    if (power_voltage > 0.5)
    {
        power_voltage += (0.7 + 0.35);   
    } 
	if(power_voltage < 7)
	{
		s_hard_ware.power_voltage =  0;
	}
	else
	{
		if (s_hard_ware.power_voltage > 7)
		{
			s_hard_ware.power_voltage = applied_math_lowpass_filter(s_hard_ware.power_voltage, power_voltage, 0.5);
		}
		else
		{			
    		s_hard_ware.power_voltage = power_voltage;
		}
	}
	//LOG(DEBUG,"power_voltage=%f",s_hard_ware.power_voltage);
}

//电压等级标准
static U8 voltage_grade_standard[]={1,12,24,36,48,60,72,84,96};

/**
 * Function:   判断电压等级
 * Description:
 * Input:      voltage:电压
 * Output:     
 * Return:     电压等级:0——未知,其它情况返回对应12,24,36,48,60,72,84,96
 * Others:      
 */
static U8 hard_ware_calc_extern_voltage_grade(float voltage)
{
	U8 index = 0;
	
	U8 array_size = sizeof(voltage_grade_standard)/sizeof(U8);
	
	if (voltage > voltage_grade_standard[array_size -1])
	{
		return voltage_grade_standard[array_size -1];
	}
	for (index = 0; index < array_size - 1; ++index)
	{
		if (voltage > voltage_grade_standard[index] + 6 && voltage <= voltage_grade_standard[index + 1] + 6)
		{
			return voltage_grade_standard[array_size -1];
		}
	}
	return 0;
}

//根据电压值和电压等级计算电量百分比,返回[0,100.0]
static float hard_ware_calc_extern_battery_percent(float voltage, U8 voltage_grade)
{
	float extern_battery_percent = 0;
	
	switch(voltage_grade)
	{
	case 0:
		extern_battery_percent = 0;
		break;

	case 12:
		if (voltage >= 12)
		{
			extern_battery_percent = 100;
		}
		else
		{
			extern_battery_percent = 0;
		}
		break;

	case 24:
		if (voltage >= 24)
		{
			extern_battery_percent = 100;
		}
		else
		{
			extern_battery_percent = 0;
		}
		break;

	case 36:
		if (voltage > 33)
		{
			extern_battery_percent = (voltage - 33) * 100 /9;
		}
		else
		{
			extern_battery_percent = 0;
		}
		break;

	case 48:
		if (voltage > 42)
		{
			extern_battery_percent = (voltage - 42) * 100 /14;
		}
		else
		{
			extern_battery_percent = 0;
		}
		break;

	case 60:
		if (voltage > 56)
		{
			extern_battery_percent = (voltage - 56) * 100 /14;
		}
		else
		{
			extern_battery_percent = 0;
		}
		break;

	case 72:
		if (voltage > 70)
		{
			extern_battery_percent = (voltage - 70) * 100 /14;
		}
		else
		{
			extern_battery_percent = 0;
		}
		break;

	case 84:
		if (voltage > 83)
		{
			extern_battery_percent = (voltage - 83) * 100 /13;
		}
		else
		{
			extern_battery_percent = 0;
		}
		break;

	default:
		extern_battery_percent = 100;
		break;
	}
	
	extern_battery_percent = (extern_battery_percent > 100) ? 100 : extern_battery_percent;
	
	return extern_battery_percent;

}

static void hard_ware_check_high_voltage_alarm(void)
{
	GM_ERRCODE ret = GM_SUCCESS;
	U16 voltage_alarm_hold_time_sec = 5;
	bool is_90v_power = false;
	U8 high_voltage_alarm_threshhold = GM_HIGH_VOLTAGE_ALARM_THRESHHOLD_36V;
	JsonObject* p_log_root = NULL;
	GM_CHANGE_ENUM alarm_state_change = GM_NO_CHANGE;
	AlarmInfo alarm_info;
	
	ret = config_service_get(CFG_POWER_CHECK_TIME, TYPE_SHORT, &voltage_alarm_hold_time_sec, sizeof(voltage_alarm_hold_time_sec));
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR,"Failed to config_service_get,ret=%d",ret);
		return;
	}
	
	ret = config_service_get(CFG_IS_90V_POWER, TYPE_BOOL, &is_90v_power, sizeof(bool));
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR,"Failed to config_service_get,ret=%d",ret);
		return;
	}

	high_voltage_alarm_threshhold = is_90v_power ? GM_HIGH_VOLTAGE_ALARM_THRESHHOLD_90V : GM_HIGH_VOLTAGE_ALARM_THRESHHOLD_36V;
	alarm_state_change  = util_check_state_change(s_hard_ware.power_voltage > high_voltage_alarm_threshhold, &s_hard_ware.high_voltage_alarm_record,voltage_alarm_hold_time_sec,voltage_alarm_hold_time_sec);

	if (GM_CHANGE_TRUE == alarm_state_change)
	{
		//报电压过高报警
		alarm_info.type = ALARM_POWER_HIGH;
		alarm_info.info = (u16)s_hard_ware.power_voltage;
		ret = gps_service_push_alarm(&alarm_info);
		if (GM_SUCCESS != ret)
		{
			LOG(ERROR, "Failed to gps_service_push_alarm(ALARM_POWER_HIGH),ret=%d", ret);
		}
		
		system_state_set_high_voltage_alarm(true);

		//上传日志
		p_log_root = json_create();
		json_add_string(p_log_root, "event", ("high_voltage_alarm"));
		json_add_double(p_log_root, "voltage", s_hard_ware.power_voltage);
		log_service_upload(FATAL,p_log_root);
	}
    else if(GM_CHANGE_FALSE == alarm_state_change)
    {
    	
    	system_state_set_high_voltage_alarm(false);
		
		//上传日志
		p_log_root = json_create();
		json_add_string(p_log_root, "event", ("high_voltage_alarm recover"));
		json_add_double(p_log_root, "voltage", s_hard_ware.power_voltage);
		log_service_upload(FATAL,p_log_root);
    }
	else
	{
	}
		
}

static void hard_ware_check_power_off_alarm(void)
{
	GM_ERRCODE ret = GM_SUCCESS;
	U16 voltage_alarm_hold_time_sec = 0;
	JsonObject* p_log_root = NULL;
	GM_CHANGE_ENUM alarm_state_change = GM_NO_CHANGE;
	AlarmInfo alarm_info;
	U8 power_off_alarm_disable = 0;
	
	config_service_get(CFG_POWER_CHECK_TIME, TYPE_SHORT, &voltage_alarm_hold_time_sec, sizeof(voltage_alarm_hold_time_sec));
	config_service_get(CFG_CUTOFFALM_DISABLE, TYPE_BYTE, &power_off_alarm_disable, sizeof(power_off_alarm_disable));

	if (power_off_alarm_disable)
	{
		return;
	}
	
	alarm_state_change	= util_check_state_change(s_hard_ware.power_voltage < GM_BATTERY_HIGHEST_VOLTAGE ,&s_hard_ware.power_off_alarm_record,voltage_alarm_hold_time_sec,10);

    if (GM_CHANGE_TRUE == alarm_state_change)
	{
		//报断电报警
		alarm_info.type = ALARM_POWER_OFF;
		alarm_info.info = (u16)s_hard_ware.power_voltage;
		ret = gps_service_push_alarm(&alarm_info);
		if (GM_SUCCESS != ret)
		{
			LOG(ERROR, "Failed to gps_service_push_alarm(ALARM_POWER_OFF),ret=%d", ret);
		}
		
		system_state_set_power_off_alarm(true);

		//上传日志
		p_log_root = json_create();
		json_add_string(p_log_root, "event", ("power_off_alarm"));
		json_add_double(p_log_root, "voltage", s_hard_ware.power_voltage);
		log_service_upload(DEBUG,p_log_root);
	}
	else if(GM_CHANGE_FALSE == alarm_state_change)
    {
    	system_state_set_power_off_alarm(false);
	
		//上传日志
		p_log_root = json_create();
		json_add_string(p_log_root, "event", ("power_off_alarm recover"));
		json_add_double(p_log_root, "voltage", s_hard_ware.power_voltage);
		log_service_upload(DEBUG,p_log_root);
		
    }
	else
	{
		//do nothing
	}
		
}

static void hard_ware_check_battery(void)
{
	GM_ERRCODE ret = GM_SUCCESS;
	U16 voltage_alarm_hold_time_sec = 0;
	JsonObject* p_log_root = NULL;
    gm_chr_status_struct charge_status = {0};
	bool app_battery_mgr = true;
	bool current_alarm_state = false;
	GM_CHANGE_ENUM alarm_state_change = GM_NO_CHANGE;
	GM_CHANGE_ENUM battery_is_full_change = GM_NO_CHANGE;
	AlarmInfo alarm_info;
	bool voltage_alarm_disable = false;

	U16 min_charge_time = 0;

	ret = config_service_get(CFG_POWER_CHECK_TIME, TYPE_SHORT, &voltage_alarm_hold_time_sec, sizeof(voltage_alarm_hold_time_sec));
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR,"Failed to config_service_get(CFG_POWER_CHECK_TIME),ret=%d",ret);
		return;
	}

	ret = config_service_get(CFG_APP_BATTERT_MGR, TYPE_BOOL, &app_battery_mgr, sizeof(app_battery_mgr));
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR,"Failed to config_service_get(CFG_APP_BATTERT_MGR),ret=%d", ret);
		return;
	}

	config_service_get(CFG_POWER_CHARGE_MIN_TIME, TYPE_SHORT, &min_charge_time, sizeof(min_charge_time));
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR,"Failed to config_service_get(CFG_APP_BATTERT_MGR),ret=%d", ret);
		return;
	}

	//GS05系列没有使用MTK内部充电管理,这些数据对05无效
	ret = (GM_ERRCODE)GM_GetChrStatus((U8* )&charge_status);
	if (GM_SUCCESS != ret)
	{
		LOG(ERROR,"Failed to GM_GetChrStatus(),ret=%d", ret);
		return;
	}
	
	s_hard_ware.battery_voltage = charge_status.VBAT/1000000.0;
	s_hard_ware.battery_is_full = GM_GpioGetLevel(GM_GPS_CHARGE_LEVLE_GPIO_PIN);

	LOG(DEBUG,"battery voltage:%f,charge_current:%f,is_charging:%d,is_full:%d",s_hard_ware.battery_voltage,s_hard_ware.battery_charge_current,s_hard_ware.battery_is_charging,s_hard_ware.battery_is_full);

	//即使型号未知也一直充电
	GM_GpioSetLevel(GM_GPS_CHARGE_CTROL_GPIO_PIN,PINLEVEL_LOW);
	//GS05系列,应用层控制
	if (app_battery_mgr)
	{
		battery_is_full_change = util_check_state_change(s_hard_ware.battery_is_full, &s_hard_ware.battery_is_full_record, voltage_alarm_hold_time_sec,voltage_alarm_hold_time_sec);
		if (GM_CHANGE_TRUE == battery_is_full_change || s_hard_ware.power_voltage < GM_BATTERY_HIGHEST_VOLTAGE)
		{
			s_hard_ware.battery_is_charging = false;
		    s_hard_ware.battery_charge_current = 0.02;
		    //GM_GpioSetLevel(GM_GPS_CHARGE_CTROL_GPIO_PIN,PINLEVEL_HIGH);
		    LOG(DEBUG,"is full");
		}
		else if(GM_CHANGE_FALSE == battery_is_full_change)
		{
			s_hard_ware.battery_is_charging = true;
		    s_hard_ware.battery_charge_current = 0.06;
		    //GM_GpioSetLevel(GM_GPS_CHARGE_CTROL_GPIO_PIN,PINLEVEL_LOW);
		    LOG(DEBUG,"is not full.");		
		}
		else
		{
			LOG(DEBUG,"Dont change charge state.");
		}
	}
	else
	{
		LOG(DEBUG,"MTK charge.");
		s_hard_ware.battery_charge_current = charge_status.ICHARGE > 0 ? charge_status.ICHARGE/1000000.0 : 0.0;
		s_hard_ware.battery_is_charging = (charge_status.chr_plug == CHARGER_PLUG_IN);
	}

	system_state_set_has_started_charge(s_hard_ware.battery_is_charging);
	config_service_get(CFG_LOWBATTALM_DISABLE, TYPE_BYTE, &voltage_alarm_disable, sizeof(voltage_alarm_disable));
	
	//正在充电或者报警关闭
	if (s_hard_ware.battery_is_charging || voltage_alarm_disable)
	{
		return;
	}
	
    current_alarm_state = s_hard_ware.low_voltage_alarm_record.state;
	if (s_hard_ware.battery_voltage > 3.80)
	{
		current_alarm_state = false;
	}
	else if (s_hard_ware.battery_voltage < 3.65)
	{
		current_alarm_state = true;
	}
	else
	{
	}
	
	alarm_state_change	= util_check_state_change(current_alarm_state,&s_hard_ware.low_voltage_alarm_record,voltage_alarm_hold_time_sec,min_charge_time);
	if (GM_CHANGE_TRUE == alarm_state_change)
	{
		//报低电压报警
		alarm_info.type = ALARM_BATTERY_LOW;
		alarm_info.info = (u16)s_hard_ware.battery_voltage;
		ret = gps_service_push_alarm(&alarm_info);
		if (GM_SUCCESS != ret)
		{
			LOG(ERROR, "Failed to gps_service_push_alarm(ALARM_BATTERY_LOW),ret=%d", ret);
		}
		system_state_set_battery_low_voltage_alarm(true);
		

		//上传日志
		p_log_root = json_create();
		json_add_string(p_log_root, "event", ("low_voltage_alarm"));
		log_service_upload(DEBUG,p_log_root);
	}
	else if(GM_CHANGE_FALSE == alarm_state_change)
    {
    	system_state_set_battery_low_voltage_alarm(false);
	
		//上传日志
		p_log_root = json_create();
		json_add_string(p_log_root, "event", ("low_voltage_alarm recover"));
		json_add_double(p_log_root, "voltage", s_hard_ware.power_voltage);
		log_service_upload(DEBUG,p_log_root);
    }
	else
	{
		//do nothing
	}
	
	return;
}

static void hard_ware_check_acc(void)
{	
	//硬件设计上，高电平是没有接ACC，低电平是接了ACC,所以这里要取反
	bool acc_level = !GM_GpioGetLevel(GM_ACC_LEVEL_GPIO_PIN);
	GM_CHANGE_ENUM change = GM_NO_CHANGE;
	U16 auto_defence_delay = 0;
	bool is_defence_by_hand = false;
	
	config_service_get(CFG_IS_MANUAL_DEFENCE,TYPE_BOOL, &is_defence_by_hand, sizeof(is_defence_by_hand));
	config_service_get(CFG_AUTO_DEFENCE_DELAY, TYPE_SHORT, &auto_defence_delay, sizeof(auto_defence_delay));
	
	change = util_check_state_change(acc_level,&s_hard_ware.acc_record,5,5);
	if (GM_CHANGE_TRUE == change)
	{
		auto_test_acc_on();
		if(!is_defence_by_hand)
		{
			GM_StopTimer(GM_TIMER_HARDWARE_AUTODEFENCE);
			system_state_set_defence(false);
		}
	}
	else if(GM_CHANGE_FALSE == change)
	{
		auto_test_acc_off();
		if(!is_defence_by_hand)
		{
			GM_StartTimer(GM_TIMER_HARDWARE_AUTODEFENCE, auto_defence_delay, hard_ware_set_auto_defence);
		}
	}
	else
	{
		//do nothing
	}

	if (s_hard_ware.acc_record.state == false && system_state_get_vehicle_state() == VEHICLE_STATE_RUN)
	{
		s_hard_ware.acc_low_but_run_seconds++;
	}

	if(s_hard_ware.acc_record.state)
	{
		s_hard_ware.acc_low_but_run_seconds = 0;
	}

	//连续24小时ACC高            ————>接常电
	//连续1分钟运动状态ACC低————>没接ACC线
	if (s_hard_ware.acc_record.true_state_hold_seconds > SECONDS_PER_DAY || s_hard_ware.acc_low_but_run_seconds > SECONDS_PER_HOUR)
	{
		//ACC无效，取消自动设防
		system_state_set_acc_is_line_mode(false);
		if(!is_defence_by_hand)
		{
			GM_StopTimer(GM_TIMER_HARDWARE_AUTODEFENCE);
			system_state_set_defence(false);
		}
	}
	else
	{
		system_state_set_acc_is_line_mode(true);
	}
	
	//如果ACC状态发生变化，立即触发一次心跳
	if (change != GM_NO_CHANGE)
	{
		gps_service_heart_atonce();
	}
}

static void hard_ware_set_auto_defence(void)
{
	system_state_set_defence(true);
}

static void hard_ware_reboot_atonce(void)
{
    gps_service_save_to_history_file();
	if (0 == GM_SystemReboot())
	{
		LOG(FATAL,"Failed to GM_SystemReboot!");
	}
}

/**
 * Function:   重启
 * Description:重启整个系统软件
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE hard_ware_reboot(const BootReason reason,U16 delay_seconds)
{
	//快速重复调用这个函数启动定时器会取消之前设置的定时器，导致无法重启，因此要用个静态变量记录下来
	static bool has_reboot = false;
	system_state_set_boot_reason(reason);

	if (0 == delay_seconds)
	{
		hard_ware_reboot_atonce();
		has_reboot = true;
	}
	else
	{
		if(!has_reboot)
		{
			GM_StartTimer(GM_TIMER_HARDWARE_REBOOT,delay_seconds*TIM_GEN_1SECOND,hard_ware_reboot_atonce);
			has_reboot = true;
		}
	}
	return GM_SUCCESS;
}

/**
 * Function:   休眠
 * Description:使系统休眠
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE hard_ware_sleep(void)
{
	if (0 == GM_SleepEnable())
	{
		LOG(ERROR,"Failed to GM_SleepEnable.");
		return GM_HARD_WARE_ERROR;
	}
	else
	{
		//上传日志
		JsonObject* p_log_root = json_create();
		json_add_string(p_log_root, "event", ("sleep"));
		json_add_int(p_log_root, "csq", gsm_get_csq());
		log_service_upload(INFO,p_log_root);
		return GM_SUCCESS;
	}
}

GM_ERRCODE hard_ware_close_gps(void)
{
	GM_GpioSetLevel(GM_GPS_POWER_CTROL_GPIO_PIN, PINLEVEL_LOW);
	return GM_SUCCESS;
}


/**
 * Function:   唤醒
 * Description:使系统唤醒
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE hard_ware_awake(void)
{
	if (0 == GM_SleepDisable())
	{
		LOG(ERROR,"Failed to hard_ware_awake.");
		return GM_HARD_WARE_ERROR;
	}
	else
	{
		//上传日志
		JsonObject* p_log_root = json_create();
		json_add_string(p_log_root, "event", ("awake"));
		json_add_int(p_log_root, "csq", gsm_get_csq());
		json_add_int(p_log_root, "run time", system_state_get_start_time());
		log_service_upload(INFO,p_log_root);
		return GM_SUCCESS;
	}
}

GM_ERRCODE hard_ware_open_gps(void)
{
	GM_GpioSetLevel(GM_GPS_POWER_CTROL_GPIO_PIN, PINLEVEL_HIGH);
    GM_SysMsdelay(10);
    GM_GpioSetLevel(GM_GPS_POWER_CTROL_GPIO_PIN, PINLEVEL_HIGH);
    GM_SysMsdelay(10);
    GM_GpioSetLevel(GM_GPS_POWER_CTROL_GPIO_PIN, PINLEVEL_HIGH);
	return GM_SUCCESS;
}

