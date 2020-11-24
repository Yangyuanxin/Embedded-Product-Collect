#include "RTL.h"
#include "stm32f10x.h"
#include "dbg.h"
#include "bms.h"

#if 1
#define SHK_MSG dbg_msg
#else
#define SHK_MSG (void)
#endif

#if 1
#define SHK_ERROR dbg_error
#else
#define SHK_ERROR (void)
#endif



static u8 hsk_ok;   //标志是否检测到充电枪插入信号

/*
 * @brief          获取充电枪状态
 * @retval         1 已经插入   0 未插入
 */
u8 shk_isok()
{
	return hsk_ok;
}



/*
 * 检测充电枪是否插入  当充电枪CC1信号为4V时表示已经插入
 */
void task_shk()
{
	u8  cnt, seach_cnt;
	u16 ic1;
	u32 fre;

	os_dly_wait(1000);

	while(1)
	{
		cnt = 0;
		seach_cnt = 5;

		while(seach_cnt--)
		{
			if(TIM_GetFlagStatus(TIM3,TIM_FLAG_CC1))  //捕获到脉冲
			{
				TIM_ClearFlag(TIM3, TIM_FLAG_CC1);
				ic1 = TIM_GetCapture1(TIM3);
				if(ic1 != 0)
				{
					fre = SystemCoreClock / (ic1 * 72);

					if((fre > 70000) && (fre < 100000))  //充电枪已经插入
					//if((fre > 5500) && (fre < 8500))  //充电枪已经插入
					{
						cnt++;
					}
					else
					{
						cnt = 0;
					}
					//SHK_MSG("fre %d\n", fre);
				}
			}
			os_dly_wait(10);
		}

		if(cnt >= 3)  //连续3次检测到
		{
			hsk_ok = 1;
			//SHK_MSG("shk handshake ok\n");
		}
		else
		{
			//SHK_MSG("shk handshake no\n");
			hsk_ok = 0;
		}
		os_dly_wait(200);
	}
}




