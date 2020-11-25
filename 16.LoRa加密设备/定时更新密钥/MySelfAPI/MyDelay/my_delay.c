#include "my_delay.h"


static u16 Counter = 0x00;
u16 ExportLogTimer = 0;		//导出日志计时器(时间间隔为1s，为了与上位机协调)

void TimerDelay_Ms(u16 counter)
{
	Counter = 0x00;
	while( Counter<counter );
}


void RunDelayCounter(void)
{
	if( Counter<DELAY_MAX_VALUE )
		Counter ++;
	
	if(ExportLogTimer< 10000)
		ExportLogTimer++;
}


//获取导出日志计时器的值
u16 GetExportLogTimerValue(void)
{
	return ExportLogTimer;
}


//清空导出日志计时器的值
void ClearExportLogTimer(void)
{
	ExportLogTimer = 0;
}





