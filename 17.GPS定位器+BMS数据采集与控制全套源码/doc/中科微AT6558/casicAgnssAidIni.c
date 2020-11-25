#include <string.h>
#include <stdio.h>
#include <casicAgnssAidIni.h>

/************************************************************
函数名称：isLeapYear
函数功能：闰年判断。判断规则：四年一闰，百年不闰，四百年再闰。
函数输入：year，待判断年份
函数输出：1, 闰年，0，非闰年（平年）
************************************************************/
int isLeapYear(int year)
{	
	if ((year & 0x3) != 0)
	{						// 如果year不是4的倍数，一定是平年
		return 0;
	}	
	else if ((year % 400) == 0)
	{						// year是400的倍数
		return 1;
	}	
	else if ((year % 100) == 0)
	{						// year是100的倍数
		return 0;
	}	
	else
	{						// year是4的倍数
		return 1;
	}
}
/*************************************************************************
函数名称：	gregorian2SvTime
函数功能：	时间格式转换, 需要考虑UTC跳秒修正
			输入的时间格式是常规的年月日时分秒格式的时间；
			转换后的时间格式是GPS时间格式，用周数和周内时表示，GPS的时间起点是1980.1.6
			GPS时间没有闰秒修正，是连续的时间，而常规时间是经过闰秒修正的
			2016年的闰秒修正值是17秒
函数输入：	pDateTime,	结构体指针，年月日时分秒格式的时间
函数输出：	pAidIni,	结构体指针，周数和周内时（或者天数和天内时）时间格式
*************************************************************************/
void gregorian2SvTime(DATETIME_STR *pDateTime, AID_INI_STR *pAidIni)
{	
	int DayMonthTable[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	int i, dn, wn;
	double tod, tow;

	// 天内时间
	tod = pDateTime->hour * 3600 + pDateTime->minute * 60 + pDateTime->second + pDateTime->ms / 1000.0;
	
	// 参考时间: 1980.1.6
	dn = pDateTime->day;
	// 年->天
	for (i = 1980; i < (pDateTime->year); i++)
	{
		if (isLeapYear(i))
		{
			dn += 366;
		}			
		else
		{
			dn += 365;
		}
	}
	dn -= 6;
	// 月->天
	if (isLeapYear(pDateTime->year))
	{
		DayMonthTable[1] = 29;
	}		
	for (i = 1; i < pDateTime->month; i++)
	{
		dn += DayMonthTable[i-1];
	}		

	// 周数+周内时间
	wn	= (dn / 7);						// 周数
	tow = (dn % 7) * 86400 + tod + 17;	// 周内时间，闰秒修正

	if (tow >= 604800)
	{
		wn++;
		tow -= 604800;
	}

	pAidIni->wn 	= wn;
	pAidIni->tow 	= tow;
}
/*************************************************************************
函数名称：casicAgnssAidIni
函数功能：把辅助位置和辅助时间打包成专用的数据格式。二进制信息格式化，并输出
函数输入：dateTime，日期与时间，包括有效标志（1有效）
		  lla, 经纬度标志，包括有效标志（1有效）
函数输出：aidIniMsg[66]，字符数组，辅助信息数据包，长度固定
*************************************************************************/
void casicAgnssAidIni(DATETIME_STR dateTime, POS_LLA_STR lla, char aidIniMsg[66])
{
	AID_INI_STR aidIni;
	int ckSum, i;
	int *pDataBuff = (int*)&aidIni;

	gregorian2SvTime(&dateTime, &aidIni);
	
	aidIni.df			= 0;
	aidIni.xOrLat		= lla.lat;
	aidIni.yOrLon		= lla.lon;
	aidIni.zOrAlt		= lla.alt;
	aidIni.fAcc			= 0;
	aidIni.posAcc		= 0;
	aidIni.tAcc			= 0;
	aidIni.timeSource	= 0;
	
	aidIni.flags 		= 0x20;											// 位置格式是LLA格式，高度无效，频率和位置精度估计无效
	aidIni.flags 		= aidIni.flags | ((lla.valid 	  == 1) << 0); 	// BIT0：位置有效标志
	aidIni.flags 		= aidIni.flags | ((dateTime.valid == 1) << 1); 	// BIT1：时间有效标志

	// 辅助数据打包
	ckSum = 0x010B0038;//低字节保存在内存的低地址中，即小端模式
	for (i = 0; i < 14; i++)
	{
		ckSum += pDataBuff[i];
	}
	
	aidIniMsg[0] = 0xBA;
	aidIniMsg[1] = 0xCE;
	aidIniMsg[2] = 0x38;		// LENGTH
	aidIniMsg[3] = 0x00;
	aidIniMsg[4] = 0x0B;		// CLASS	ID
	aidIniMsg[5] = 0x01;		// MESSAGE	ID
	
	memcpy(&aidIniMsg[6],  (char*)(&aidIni), 56);
	memcpy(&aidIniMsg[62], (char*)(&ckSum),  4);
	
	return;
}