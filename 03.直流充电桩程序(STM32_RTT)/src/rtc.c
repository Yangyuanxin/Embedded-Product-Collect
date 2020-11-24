#include "RTL.h"
#include "stm32f10x.h"
#include "rtc.h"
#include "dbg.h"

#define YEAR_BASE 2015

//月份      1  2  3  4  5  6  7  8  9  10 11 12
//闰年     31 29 31 30 31 30 31 31 30 31 30 31
//非闰年 31 28 31 30 31 30 31 31 30 31 30 31

/**
  * @brief  判断是否是闰年
  * @param  year: 输入年份
  * @retval 1 是   0 不是
  */
u8 Is_Leap_Year(u16 year)
{
	if (year % 4 == 0) //必须能被4整除
	{
		if (year % 100 == 0)
		{
			if (year % 400 == 0)
				return 1; //如果以00结尾,还要能被400整除
			else
				return 0;
		}
		else
			return 1;
	}
	else
		return 0;
}



//月份数据表
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表
//平年的月份日期表
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};

/**
  * @brief  设置时间  把输入的时时转换为秒钟 以1970.1.1为基准  1970~2099年为合法年份
  * @param  syear: 年份
  * @param  smon:  月份
  * @param  sday:  日
  * @param  hour:  时
  * @param  min:   分
  * @param  sec:   秒
  * @retval 0 成功   其它 错误
  */
u8 TIME_Set(time_t *time)
{
	u16 t;
	u32 seccount = 0;
	u8 is_leap_year = Is_Leap_Year(time->year);

	if (time->year < 2000 || time->year > 2099)
		return 1; //syear范围1970-2099，此处设置范围为2000-2099

	for (t = YEAR_BASE; t < time->year; t++) //把所有年份的秒钟相加
	{
		if (Is_Leap_Year(t))
			seccount += 31622400; //闰年的秒钟数
		else
			seccount += 31536000; //平年的秒钟数
	}

	time->mon -= 1;

	for (t = 0; t < time->mon; t++) //把前面月份的秒钟数相加
	{
		seccount += (u32) mon_table[t] * 86400; //月份秒钟数相加
		if (is_leap_year && t == 1)
			seccount += 86400; //闰年2月份增加一天的秒钟数
	}

	seccount += (u32) (time->day - 1) * 86400; //把前面日期的秒钟数相加
	seccount += (u32) time->hour * 3600; //小时秒钟数
	seccount += (u32) time->min * 60; //分钟秒钟数
	seccount += time->sec; //最后的秒钟加上去

	//设置时钟
	/*  RCC->APB1ENR|=1<<28;//使能电源时钟
	 RCC->APB1ENR|=1<<27;//使能备份时钟
	 PWR->CR|=1<<8;    //取消备份区写保护
	 //上面三步是必须的!*/

	RTC_WaitForLastTask();
	RTC_SetCounter(seccount);
	RTC_WaitForLastTask();
	return 0;
}




u8 TIME_Get_Week(u16 year, u8 month, u8 day);
/**
  * @brief  获取时间
  * @param  year: 输入年份
  * @retval 0 成功   其它 错误
  */
u8 TIME_Get(time_t *time)
{
	//static u16 daycnt = 0;
	u32 timecount = 0;
	u32 temp = 0;
	u16 temp1 = 0;
	timecount = RTC_GetCounter();  //得到计数器中的值(秒钟数)
	temp = timecount / 86400; //得到天数(秒钟数对应的)

	//dbg_msg("cnt  %d\n", timecount);

	//if (daycnt != temp) //超过一天了
	//{
		//daycnt = temp;
		temp1 = YEAR_BASE; //从1970年开始
		while (temp >= 365)
		{
			if (Is_Leap_Year(temp1)) //是闰年
			{
				if (temp >= 366)
					temp -= 366; //闰年的秒钟数
				else
				{
					temp1++;
					break;
				}
			}
			else
				temp -= 365; //平年
			temp1++;
		}
		time->year = temp1; //得到年份
		temp1 = 0;
		while (temp >= 28) //超过了一个月
		{
			if (Is_Leap_Year(time->year) && temp1 == 1) //当年是不是闰年/2月份
			{
				if (temp >= 29)
					temp -= 29; //闰年的秒钟数
				else
					break;
			}
			else
			{
				if (temp >= mon_table[temp1])
					temp -= mon_table[temp1]; //平年
				else
					break;
			}
			temp1++;
		}
		time->mon = temp1 + 1; //得到月份
		time->day = temp + 1; //得到日期
	//}
	temp = timecount % 86400; //得到秒钟数
	time->hour = temp / 3600; //小时
	time->min = (temp % 3600) / 60; //分钟
	time->sec = (temp % 3600) % 60; //秒钟
	time->week = TIME_Get_Week(time->year, time->mon, time->day); //获取星期

	return 0;
}


/**
  * @brief  计算星期
  * @param  year: 年份
  * @param  mon:  月份
  * @param  day:  日
  * @retval 星期
  */
u8 TIME_Get_Week(u16 year, u8 month, u8 day)
{
	u16 temp2;
	u8 yearH, yearL;
	yearH = year / 100;
	yearL = year % 100;

	// 如果为21世纪,年份数加100

	if (yearH > 19)
		yearL += 100;
	// 所过闰年数只算1900年之后的
	temp2 = yearL + yearL / 4;
	temp2 = temp2 % 7;
	temp2 = temp2 + day + table_week[month - 1];
	if (yearL % 4 == 0 && month < 3)
		temp2--;
	return (temp2 % 7);
}





static void RTC_Configuration()
{
	u32 timeout = 0x50000;

	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	//PWR_BackupAccessCmd(ENABLE);

	//BKP_DeInit();
	RCC_LSEConfig(RCC_LSE_ON);

	while (!RCC_GetFlagStatus(RCC_FLAG_LSERDY) && (--timeout));  //设置后需要等待启动

	if(timeout == 0)
	{
		dbg_msg("LSE ON timeout\n");
		return;
	}

	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);  //选择LSE为RTC设备的时钟
	RCC_RTCCLKCmd(ENABLE);  //使能
	RTC_WaitForSynchro();  //等待同步

	RTC_WaitForLastTask();
	RTC_SetPrescaler(32767);
	RTC_WaitForLastTask();

	//RTC_ITConfig(RTC_IT_SEC,ENABLE);
	//RTC_WaitForLastTask();
}


void RTC_Init()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_BackupAccessCmd(ENABLE);

	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
		time_t time;
		time.year = 2015;
		time.mon  = 1;
		time.day  = 1;
		time.hour = 0;
		time.min  = 0;
		time.sec  = 0;

		RTC_Configuration();

		TIME_Set(&time);

		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
	}
	else
	{
		RTC_WaitForSynchro();
	}

	RCC_ClearFlag();
}
















