#include "stm32f10x.h"

/*
 * 模块上电(交流接触器闭合)
 */
void io_evc_poweron()
{
	GPIO_SetBits(GPIOE, GPIO_Pin_13);
}

/*
 * 模块下电
 */
void io_evc_poweroff()
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_13);
}

/*
 * 读取交流接触器反馈  0:闭合   >0断开
 */
u8 io_ac_status()
{
	return GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_13);
}


/*
 * 运行灯亮
 */
void io_runled_on()
{
	GPIO_SetBits(GPIOE, GPIO_Pin_12);
}

/*
 * 运行灯灭
 */
void io_runled_off()
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_12);
}



/*
 * 故障灯亮
 */
void io_errled_on()
{
	GPIO_SetBits(GPIOE, GPIO_Pin_11);
}

/*
 * 故障灯灭
 */
void io_errled_off()
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_11);
}


/*
 * 开启风扇
 */
void io_fun_on()
{
	GPIO_SetBits(GPIOE, GPIO_Pin_8);
}

/*
 * 关闭风扇
 */
void io_fun_off()
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_8);
}


/************************用于双充电枪***************************/
/*
 * @brief  吸合充电枪1继电器
 */
void io_bms_sng1_en()
{
	GPIO_SetBits(GPIOE, GPIO_Pin_6);
}

/*
 * @brief  断开充电枪1继电器
 */
void io_bms_sng1_dis()
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_6);
}


/*
 * @brief  吸合充电枪2继电器
 */
void io_bms_sng2_en()
{
	GPIO_SetBits(GPIOE, GPIO_Pin_5);
}

/*
 * @brief  断开充电枪2继电器
 */
void io_bms_sng2_dis()
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_5);
}
/***************************************************/

/*
 * 读取DC输出继电器1反馈  0:闭合   >0断开
 */
u8 io_dc1_status()
{
	return GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_14);
}

/*
 * 读取DC输出继电器2反馈  0:闭合   >0断开
 */
u8 io_dc2_status()
{
	return GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_15);
}

/*
 * @brief  吸合DC输出继电器
 */
void io_bms_dc_out_en()
{
	GPIO_SetBits(GPIOE, GPIO_Pin_6);  //DC输出继电器吸合
}

/*
 * @brief  释放DC输出继电器
 */
void io_bms_dc_out_dis()
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_6);  //DC输出继电器释放
}


/*
 * @brief  24V继电器吸合
 */
void io_bms_24v_en()
{
	GPIO_SetBits(GPIOE, GPIO_Pin_9);  //24V继电器吸合
}

/*
 * @brief  24V继电器释放
 */
void io_bms_24v_dis()
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_9);  //24V继电器释放
}


/*
 * @brief  12V继电器吸合
 */
void io_bms_12v_en()
{
	GPIO_SetBits(GPIOE, GPIO_Pin_10);  //12V继电器吸合
}

/*
 * @brief  12V继电器释放
 */
void io_bms_12v_dis()
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_10);  //12V继电器释放
}










