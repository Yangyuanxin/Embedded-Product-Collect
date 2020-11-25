#include "com_status_key_init.h"


extern _ComWorkMode		com_work_mode;
extern _AT_TimerStruct	at_timer_struct;

static u8 ComStatusKeyPressFlag = DISABLE;
u16 ComStatusChangeTimer = 0x00;
u8	ComStatusChange = KEY_NONE;	


//串口状态切换按键(工作模式切换)
void ComStatusChangeKeyInit(void)
{
	GPIO_InitTypeDef 	GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOC, ENABLE );	//使能时钟
				
	//配置状态切换按键
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;		//选择PIN4引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//设置成上拉输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);			//初始化GPIOC4
}


//按键扫描检测
void ScanComStatusKey(void)
{
	if(COM_STATUS_KEY == 0)
	{
		ComStatusKeyPressFlag = ENABLE;
	}
	else
	{
		ComStatusKeyPressFlag = DISABLE;
	}
}


/**************************************************
	* 功能描述：运行串口状态切换计时器
	* 入口参数：无
	* 返回值：	无
	* 备注：	
***************************************************/
void RunComStatusChangeTimer(void)
{
	if(ComStatusKeyPressFlag == ENABLE)
	{
		ComStatusChangeTimer ++;
	}
	else
	{
		if( ComStatusChangeTimer >1500 )
		{
			ComStatusChange = KEY_AT;
			ComStatusChangeTimer = 0x00;
			
		}
		else if(ComStatusChangeTimer < 1000)
		{
			ComStatusChange = KEY_NONE;
			ComStatusChangeTimer = 0x00;
		}
	}
}	


/************************************************
	* 功能描述：判断是否更变串口工作状态
	* 入口参数：无
	* 返回值：	无
	* 备注：	无
*************************************************/
void IsComChangeToAT_WorkMode(void)
{
	if(ComStatusChange == KEY_AT)
	{
		com_work_mode = ComMode_AT;
		at_timer_struct.SetAtTimer_5s = 0x00;
	}
}





