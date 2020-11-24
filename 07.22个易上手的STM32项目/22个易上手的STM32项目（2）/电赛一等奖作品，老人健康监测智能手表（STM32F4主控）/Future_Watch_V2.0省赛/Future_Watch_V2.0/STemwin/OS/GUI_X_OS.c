#include "GUI.h"
#include "includes.h"
    
/*********************************************************************
*
* Global data
*/
static OS_EVENT 	*DispSem;  	//显示的信号量
static OS_EVENT	    *EventMbox;  

static OS_EVENT	*KeySem;  	//按键信号量
static int		KeyPressed;
static char		KeyIsInited;


//时间获取函数
int GUI_X_GetTime(void)
{
  return ((int) OSTimeGet());
}

//emWin延时函数
//最终的延时时间是period*1000
void GUI_X_Delay(int period)
{
  INT32U ticks;
  ticks = (period*1000)/OS_TICKS_PER_SEC;
  OSTimeDly(ticks);
}

static void CheckInit(void)
{
	if(KeyIsInited == OS_FALSE)
	{
		KeyIsInited = OS_TRUE;
		GUI_X_Init();
	}
}

void GUI_X_Init(void)
{
	KeySem = OSSemCreate(0);	//创建初始值为0的信号量
}


int GUI_X_GetKey(void)
{
	int r;
	r = KeyPressed;
	CheckInit();
	KeyPressed = 0;
	return (r);
}

int GUI_X_WaitKey(void)
{
	int r;
	INT8U err;
	CheckInit();
	if(KeyPressed == 0)
	{
		OSSemPend(KeySem,0,&err);	//请求信号量
	}
	r = KeyPressed;
	KeyPressed = 0;
	return (r);
}

void GUI_X_StoreKey(int k)
{
	KeyPressed = k;
	OSSemPost(KeySem);
}
void GUI_X_ExecIdle(void) {}


//OS初始化
void GUI_X_InitOS(void)
{
	DispSem = OSSemCreate(1);			//创建初始值为1的信号量
	EventMbox = OSMboxCreate((void*)0);	//创建消息邮箱
	
}

//任务解锁
void GUI_X_Unlock(void)
{ 
  OSSemPost(DispSem);		//发送信号量
}

//任务上锁
void GUI_X_Lock(void)
{
  INT8U err;
  OSSemPend(DispSem,0,&err);//请求信号量
}

/* Get Task handle */
U32 GUI_X_GetTaskId(void) 
{ 
//  return ((U32) osThreadGetId());
	return 0;
}


//OS等待事件
void GUI_X_WaitEvent (void) 
{
	INT8U err;
	(void)OSMboxPend(EventMbox,0,&err);	//请求消息邮箱 
}

//OS发送事件
void GUI_X_SignalEvent (void) 
{
  (void)OSMboxPost(EventMbox,(void*)1);	//发送消息邮箱
}



void GUI_X_Log (const char *s) { }
void GUI_X_Warn (const char *s) { }
void GUI_X_ErrorOut(const char *s) { }

/*************************** End of file ****************************/

