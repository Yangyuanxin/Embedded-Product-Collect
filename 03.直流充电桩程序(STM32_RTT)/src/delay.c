#include "RTL.h"
#include "stm32f10x.h"


/*
 * @brief  延时复位
 * @param  delay_base 定时基准时间
 */
void delay_reset(u32 *delay_base)
{
	*delay_base = os_time_get();  //获取系统时间
}


/*
 * @brief  判断是否到达定时时间  当超时后应该调用server_timer_reset函数复位
 * @param  delay_base 定时基准时间
 * @param  ms 定时时间  单位毫秒
 * @retval 0:超时  >0:剩余时间
 */
u32 delay_timeout(u32 *delay_base, u32 ms)
{
	u32 temp = os_time_get();  //获取系统时间
	if(*delay_base <= temp)
	{
		u32 d = temp - *delay_base;
		if(d >= ms)  //定时时间到
		{
			return 0;
		}
		else
		{
			return (ms - d);  //剩余时间
		}
	}
	else
	{
		//0xffffffff - server_time_base + temp
		u32 d = ~(*delay_base) + temp;
		if(d >= ms)  //定时时间到
		{
			return 0;
		}
		else
		{
			return (ms - d);  //剩余时间
		}
	}
}

/*
 * @brief  判断是否到达定时时间  当超时后自动重载
 * @param  delay_base 定时基准时间
 * @param  ms 定时时间  单位毫秒
 * @retval 0:超时  >0:剩余时间
 */
u32 delay_timeout_reload(u32 *delay_base, u32 ms)
{
	u32 temp = delay_timeout(delay_base, ms);
	if(!temp)
	{
		delay_reset(delay_base);  //复位 重新计时
	}
	return temp;
}
