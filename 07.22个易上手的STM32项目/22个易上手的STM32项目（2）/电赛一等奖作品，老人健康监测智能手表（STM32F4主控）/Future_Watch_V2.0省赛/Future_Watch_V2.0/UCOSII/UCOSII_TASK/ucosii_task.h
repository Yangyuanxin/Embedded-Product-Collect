#ifndef _UCOSII_H_
#define _UCOSII_H_
#include "main.h"
#include "display.h"
/**************************外部变量声明************************************/
extern TouchPointRefTypeDef TPR_Structure;
extern  Clock My_Clock;
extern SYSTEM_STA System_Sta;	
extern u8  read_flag;  
extern SYSTEM_DATA System_Data;
extern u8 H_Average_Buf[12];
extern u8 T_Average_Buf[12];
extern u8 SEND_AT_FLAG;
extern u16 DATA_RX_STA;
extern SYSTEM_SET System_Set;
extern u8 Data_All[];
extern u8 Data_Ok[];
extern u8 Help_hg[];
extern u8 Help_ht[];
extern u8 Help_hh[];
extern u8 Help_ho[];
extern u16 AdcDma_Buf;
/**************************外部函数声明************************************/
extern u16 Get_Adc_Average(u16 adc[],u8 times);
extern WM_HWIN CreateWindow(void);
/**************************全局变量****************************************/

/****************************UCOSII信号量********************************/


//OS_EVENT * sem_smartlink;		//WIFI信号量指针
//OS_EVENT * sem_getdata;		//get天气数据的信号量指针
//OS_EVENT * sem_weather;		//开始进入天气显示
//OS_EVENT * sem_wifiover;
//OS_EVENT * sem_wifilink;
//OS_EVENT * sem_dataover;
/**************************UCOSII互斥信号量******************************/
//OS_EVENT * Data_SendMute;
/*************************UCOSII任务设置********************************/
//START任务
//设置任务的优先级
#define START_TASK_PRIO				3
//任务堆栈大小 
#define START_STK_SIZE			  64
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);

//Wifi_Task任务
//设置任务优先级
#define WIFI_TASK_PRIO			7
//任务堆栈大小
#define WIFI_STK_SIZE			128
//任务控制块
OS_TCB WifiTaskTCB;
//任务堆栈
OS_STK WIFI_TASK_STK[WIFI_STK_SIZE];
//Display_Task任务
void Wifi_Task(void *pdata);

//电池任务
//设置任务优先级
#define BATTERY_TASK_PRIO 				10
//任务堆栈大小
#define BATTERY_STK_SIZE				128
//任务控制块
OS_TCB BATTERYTaskTCB;
//任务堆栈
OS_STK BATTERY_TASK_STK[BATTERY_STK_SIZE];
//电池任务
void Battery_Task(void *pdata);

//Display_Task任务
//设置任务优先级
#define DISPLAY_TASK_PRIO			9
//任务堆栈大小
#define DISPLAY_STK_SIZE			512
//任务控制块
OS_TCB DisplayTaskTCB;
//任务堆栈
OS_STK DISPLAY_TASK_STK[DISPLAY_STK_SIZE];
//Display_Task任务
void Display_Task(void *pdata);

//Touch_Task
//设置任务优先级
#define TOUCH_TASK_PRIO 	4	
//任务堆栈大小
#define TOUCH_STK_SIZE		128
//任务控制块
OS_TCB TouchTaskTCB;
//任务堆栈
OS_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];
//Touch_Task任务
void Touch_Task(void *pdata);

//UpDate_Task
//设置任务优先级
#define UPDATE_TASK_PRIO 	8
//任务堆栈大小
#define UPDATE_STK_SIZE		128
//任务控制块
OS_TCB UpDateTaskTCB;
//任务堆栈
OS_STK UPDATE_TASK_STK[UPDATE_STK_SIZE];
//Touch_Task任务
void UpDate_Task(void *pdata);

//Warning_Task
//设置任务优先级
#define WARNING_TASK_PRIO 	5
//任务堆栈大小
#define WARNING_STK_SIZE		128
//任务控制块
OS_TCB Warning_TaskTCB;
//任务堆栈
OS_STK WARNING_TASK_STK[WARNING_STK_SIZE];
//Touch_Task任务
void Warning_Task(void *pdata);

/********************************系统任务函数****************************/
//报警&提醒任务
void Warning_Task(void *pdata)
{
	while(1)
	{	
		Check_Fall();		//扫描摔倒		
		Action_Task();		//手势扫描
		if(System_Set.warning_fall == 1)
		{
			if(System_Sta.Tumble_Flag != 0)		//摔倒发送
			{
				if((System_Sta.Warning&0x60) !=0)	//其他报警情况正在发生，马上终止，摔倒拥有最高优先级
				{
					System_Sta.Help_Time = 0;		//报警等待清0
					System_Sta.Warning |= (1<<3);
					System_Sta.Tumble_Flag = 0;													
					System_Sta.Disp_Sta |= (1<<1);//显示主界面												
					System_Sta.Disp_Sta &= ~(1<<2);
					System_Sta.Disp_Sta &= ~(1<<3);
					System_Sta.Motor |= (1<<5);	//马达停止工作
				}
				System_Sta.Warning |= (1<<7) ;		//报警状态
				System_Sta.Help_Time = 0;		//报警等待清0
			}
		}
		if((System_Set.warning_eat&0xe0)!=0)
		{
			if((System_Sta.Warning&(0xe0))==0)		//未有报警情况
			{
				if((System_Set.warning_eat&0x80) != 0)
				{
					if((System_Set.eat_time1 == My_Clock.hour) &&(System_Set.eat_min1 == My_Clock.min))
					{
							System_Sta.Remind |= (1<<7);		//提醒吃药界面
					}
					else
					{
						System_Sta.Remind &= ~(1<<6);	//清除提醒过标志位
						System_Sta.Remind &= ~(1<<7);
					}
				}
				if((System_Set.warning_eat&0x40) != 0)
				{
					if((System_Set.eat_time2 == My_Clock.hour) &&(System_Set.eat_min2 == My_Clock.min))
					{
						System_Sta.Remind1 |= (1<<7);		//提醒吃药界面
					}
					else
					{
						System_Sta.Remind1 &= ~(1<<6);	//清除提醒过标志位
						System_Sta.Remind1 &= ~(1<<7);
					}
				}
				if((System_Set.warning_eat&0x20) != 0)
				{	
					if((System_Set.eat_time3 == My_Clock.hour) &&(System_Set.eat_min3 == My_Clock.min))
					{
						System_Sta.Remind2 |= (1<<7);		//提醒吃药界面
					}
					else
					{
						System_Sta.Remind2 &= ~(1<<6);	//清除提醒过标志位
						System_Sta.Remind2 &= ~(1<<7);
					}
				}
			}
		}
		if(System_Sta.Check_Health > 600)	//1分钟检查一次
		{
			if(System_Set.warning_heart == 1)		//心率报警
			{
				if(((System_Sta.Warning&(1<<7))==0)&&((System_Sta.Warning&(1<<5))==0))//其他报警未开
				{
					if((System_Data.heart_average >100) || (System_Data.heart_average<50))
					{
						System_Sta.Warning |= (1<<6) ;		//报警状态
						System_Sta.Help_Time = 0;		//报警等待清0
					}
				}
			}
			if(System_Set.warning_temp == 1)		//体温报警
			{
				if(((System_Sta.Warning&(1<<7))==0)&&((System_Sta.Warning&(1<<6))==0))//其他报警未开
				{
					if((System_Data.temp_average>(float)37.5) || (System_Data.temp_average<36))
					{
						System_Sta.Warning |= (1<<5) ;		//报警状态
						System_Sta.Help_Time = 0;		//报警等待清0
					}
				}
			}
			System_Sta.Check_Health = 0;
		}
		if(System_Sta.unlock == 1)
		{
			System_Sta.Disp_Sta &= ~(1<<5);		//开屏
			System_Sta.Get_backlit &= ~(1<<7);		//开启背光控制
			TIM3->CCR3 = 500;
			System_Sta.unlock = 0;
			System_Sta.Motor |= (1<<7);
		}		

		if((System_Sta.Motor&(1<<7))!=0)		//震动
		{
			Motor_Work();
		}
		delay_ms(10);
	}
}

//系统数据更新任务
void UpDate_Task(void *pdata)
{
	u8 i;
	while(1)
	{
		if(System_Sta.Get_Time > 5)		//500ms更新一次
		{
			Data_GetTime();				//时间获取
		}		
		if((System_Sta.Get_Heart&0x0fff)>300)		//30s更新一次心率
		{	
			Data_GetHeart();			//心率获取
		}
		if((System_Sta.Get_Temp&0x0fff)>300)		//30s更新一次体温
		{	
			Data_GetTemp();		//温度获取
		}
		if(((System_Sta.Get_backlit&0x7f)>10)&&((System_Sta.Get_backlit&(1<<7))==0))		//1s测一下背光
		{
			i = Lsens_Get_Val();
			TIM3->CCR3 = (i*10);
			System_Sta.Get_backlit &= 0x80;	//清楚计数位
		}
		else
		{
			if((System_Sta.Get_backlit&0x7f)>0x0f)
			{
				System_Sta.Get_backlit &= 0x80;	//清楚计数位
			}
		}
		if(((System_Sta.Warning&0xe0)!=0)&&(System_Sta.Help_Time>300))		//有报警情况发生
		{
			if(System_Sta.App_Sta == 0)
			{
				System_Sta.Tumble_Flag = 0;
				System_Sta.Warning = 0;
				System_Sta.Disp_Sta |= (1<<1);//显示主界面												
				System_Sta.Disp_Sta &= ~(1<<2);
				System_Sta.Disp_Sta &= ~(1<<3);
				System_Sta.Motor |= (1<<5);//马达停止工作
			}
			else
			{
				System_Sta.Warning |= (1<<3);
				System_Sta.Tumble_Flag = 0;													
				System_Sta.Disp_Sta |= (1<<1);//显示主界面												
				System_Sta.Disp_Sta &= ~(1<<2);
				System_Sta.Disp_Sta &= ~(1<<3);
				System_Sta.Motor |= (1<<5);	//马达停止工作
			}
		}
		if(System_Sta.Help_Time>30000)
		{
			System_Sta.Help_Time = 0;		//计数清0
		}
		delay_ms(10);
	}
}

//WIFI任务函数
void Wifi_Task(void *pdata)
{
	while(1)
	{
		if((System_Sta.Wifi_Connection&(1<<7))!=0)		//启动smartlink;
		{
			System_Sta.Wifi_Connection |= (1<<3);
			if(Wifi_SmartLink()){System_Sta.App_Sta = 0;System_Sta.Wifi_Connection |= (1<<6);}//失败
			else//成功
			{	
				Wifi_LinkServer();//连入服务器
				delay_ms(200);
				if(Watch_Get_Time()){/*校准失败*/}//时间校准
				System_Sta.Wifi_Connection |= (1<<5);
			}
			System_Sta.Wifi_Connection &= ~(1<<7);	//清除smartlink标志位
			Wifi_InUDP();		//进入数据透传模式
		}
		if((DATA_RX_STA&0x4000)!=0)		//接收到数据
		{
			if(Data_GetServer()){GUI_DispStringAt("server err",120,120);GUI_Exec();}//数据处理
			else{/*Wifi_Send2Sever(Data_Ok);*/}//返回OK给服务器
			Wifi_InUDP();		//进入数据透传模式
		}
		if(System_Sta.Send_Start == 1)
		{
			//数据编码
			Data_Convert(System_Data.temp_average,System_Data.heart_average,System_Data.boolpressure_up,System_Data.boolpressure_down,0);
			//发送数据
			if(Wifi_Send2Sever(Data_All)){GUI_DispStringAt("server err",60,120);GUI_Exec();}	
			System_Sta.Send_Start = 0;		//清空数据发送到服务器标志位
			DATA_RX_STA = 0;
			Wifi_InUDP();		//进入数据透传模式
		}
		if((System_Sta.Measure&(1<<7))!=0)		//开始测量
		{							
			if(Data_GetBP()){Wifi_LinkServer();/*获取失败*/}
			Wifi_InUDP();		//进入数据透传模式
		}
		if((System_Sta.Warning&(1<<3)) != 0 )
		{
			
			switch(System_Sta.Warning&0xf0)
			{
				//摔倒求救
				case 0x80:System_Sta.Warning = 0;
						  Data_Convert(System_Data.temp_average,System_Data.heart_average,System_Data.boolpressure_up,System_Data.boolpressure_down,1);
						  Wifi_Send2Sever(Data_All);
						  break;
				//心率求救
				case 0x40:System_Sta.Warning = 0;
						  Data_Convert(System_Data.temp_average,System_Data.heart_average,System_Data.boolpressure_up,System_Data.boolpressure_down,2);
						  Wifi_Send2Sever(Data_All);
						  break;	
				//温度求救
				case 0x20:System_Sta.Warning = 0;
						  Data_Convert(System_Data.temp_average,System_Data.heart_average,System_Data.boolpressure_up,System_Data.boolpressure_down,3);
						  Wifi_Send2Sever(Data_All);
						  break;
				//其他求救
				case 0x10:System_Sta.Warning = 0;
						  Data_Convert(System_Data.temp_average,System_Data.heart_average,System_Data.boolpressure_up,System_Data.boolpressure_down,4);
						  Wifi_Send2Sever(Data_All);
						  break;
				default:break;
			}
			Wifi_InUDP();		//进入数据透传模式
		}
		delay_ms(50);
	}
	
}


//显示任务
void Display_Task(void *pdata)
{	
	WM_HWIN head;		//当前显示的句柄
	while(1)	
	{			
		if((System_Sta.Disp_Sta&(1<<5)) == 0)		//不是锁屏
		{			
			if(((System_Sta.Warning&0xf0)!= 0) && ((System_Sta.Disp_Sta&(1<<0))==0))//报警发生
			{
					System_Sta.Motor = 0 ;
					System_Sta.Motor |= ((1<<6)|(1<<7));		//震动马达开启
					System_Sta.Help_Time = 0;		//报警等待清0
					Help_Window(&head);		//求助界面
			}
			else
			{
				if((((System_Sta.Remind&(1<<7))!=0) &&((System_Sta.Remind&(1<<6))==0)) || (((System_Sta.Remind1&(1<<7))!=0) &&((System_Sta.Remind1&(1<<6))==0))||(((System_Sta.Remind2&(1<<7))!=0) &&((System_Sta.Remind2&(1<<6))==0)))//提醒界面
				{
					if((System_Sta.Disp_Sta&(1<<0))==0)
					{
						System_Sta.Motor = 0 ;
						System_Sta.Motor |= ((1<<6)|(1<<7));	//震动马达开启
						Remind_Window(&head);		//提醒界面
					}
				}
				else
				{ 
					if((System_Sta.Disp_Sta&(1<<1))!=0)	 //主界面
					{	
						System_Sta.Wifi_Connection = 0;
						Main_Window(&head);
					}
					else
					{
						if(((System_Sta.Disp_Sta&(1<<2))!=0) &&((System_Sta.Disp_Sta&(1<<7))!=0))//Wifi设置界面
						{
							Wifi_Window(&head);		//WIFI界面
						}
						else
						{
							if(((System_Sta.Disp_Sta&(1<<3))!=0) &&((System_Sta.Disp_Sta&(1<<7))!=0))//数据显示界面
							{		
								Data_Window(&head);	//数据界面
							}
						}
					}
				}
			}
		}
		else		//锁屏状态
		{		
			Disp_lockWindow(&head);		//锁屏界面处理
		}		
		if(((System_Sta.Disp_Sta&(1<<3))!=0)&&((System_Sta.Disp_Sta&(1<<7))==0))//当前数据界面
		{	
			Disp_Update(&head);		//数据界面信息显示
		}
		else
		{
			if(((System_Sta.Disp_Sta&(1<<2))!=0) &&((System_Sta.Disp_Sta&(1<<7))==0))	//当前为Wifi界面
			{
				if((System_Sta.Wifi_Connection&(1<<3))!=0)		//开始测量
				{
					Disp_Connection(&head);
				}
			}
		}
		GUI_Exec();		
		delay_ms(10);
	}
	
}
//电池电量检测任务
void Battery_Task(void *pdata)
{
	u8 i,j;
	u16 adc_buf;
	u16 adc[20];
	u16 adc_new[10];
	float temp;
	while(1)
	{	
		for(j=0;j<6;j++)	//10s取一次AD值
		{
			for(i=0;i<20;i++)		
			{
				adc[i] = AdcDma_Buf;	//从DMA读取数据
			}
			adc_new[j]=Get_Adc_Average(adc,20);		//冒泡排序滤波
			delay_ms(1000);		//1s取样时间
		}
		adc_buf=Get_Adc_Average(adc_new,6);		//冒泡排序滤波
		temp=((float)adc_buf)*(3.300/4096); 	//转换后的电压
		temp -= (float)0.05;		//误差 这里需要再测试
		temp *= 2;			//两个电阻分压，*2=真实电压 		
		if(temp>(float)3.9)		//满电
		{
			System_Sta.battery &= ~(1<<6);
			System_Sta.battery &= ~(1<<5);
			System_Sta.battery |= (1<<7);		//满电标志位
		}
		else
		{
			if(temp<(float)3.9 && temp>(float)3.7)		//中间电量
			{
				System_Sta.battery &= ~(1<<7);
				System_Sta.battery &= ~(1<<5);
				System_Sta.battery |= (1<<6);			//中间电量
				
			}
			else				//没电
			{
				System_Sta.battery &= ~(1<<7);
				System_Sta.battery &= ~(1<<6);
				System_Sta.battery |= (1<<5);			//没电量
			}
		}
	}

}

//触摸扫描任务
void Touch_Task(void *pdata)
{	
	while(1)
	{
		GUI_TOUCH_Exec();
		if(((TPR_Structure.Gesture_Sta &(1<<6)) != 0)&((TPR_Structure.Gesture_Sta &(1<<7)) != 0))	//手势发生 进行手势判断
		{		
			if((System_Sta.Warning == 0)&&((System_Sta.Remind&(1<<7)) == 0))
			{
				TPR_Structure.Gesture_Sta |= (1<<(Gesture_Scan()));		//标志手势位
				switch(TPR_Structure.Gesture_Sta&0x0f)
				{	
					//向左滑
					case 0x01:	if((System_Sta.Disp_Sta&(1<<5)) == 0)
								{
									if((System_Sta.Disp_Sta&(1<<1)) != 0)		//主界面/*这里再加一个标志位判断是否在锁屏状态*/
									{
										System_Sta.Disp_Sta &= ~(1<<1);
										System_Sta.Disp_Sta |= (1<<2);		//向左滑动，进入WIfi链接界面
										System_Sta.Disp_Sta |= (1<<7);
									}
									else
									{
										if((System_Sta.Disp_Sta&(1<<3)) != 0)	//数据显示界面
										{
											System_Sta.Disp_Sta &= ~(1<<3);
											System_Sta.Disp_Sta |= 1<<1;		//进入主界面
											System_Sta.Disp_Sta |= (1<<7);
										}
									}
								}
								TPR_Structure.Gesture_Sta &= ~(1<<0);
								break;
					//向上解锁		
					case 0x02:	if((System_Sta.Disp_Sta&(1<<5)) != 0)
								{
									System_Sta.Motor |= (1<<7);//触发一次震动
									System_Sta.Disp_Sta &= ~(1<<5);		//清楚锁屏标志位
//									System_Sta.Disp_Sta |= (1<<1);
//									System_Sta.Disp_Sta &= ~((1<<2)|(1<<3));
									System_Sta.Get_backlit &= ~(1<<7);	//开启背光控制
									TIM3->CCR3 = 500;
									//GPIOB->BSRRL |=(1<<0);	
								}
								TPR_Structure.Gesture_Sta &= ~(1<<1);
								break;
					//向右
					case 0x04:	if((System_Sta.Disp_Sta&(1<<5)) == 0)
								{
									if((System_Sta.Disp_Sta&(1<<1))!=0)		//主界面
									{
										System_Sta.Disp_Sta &= ~(1<<1);
										System_Sta.Disp_Sta |= (1<<3);		//进入显示界面
										System_Sta.Disp_Sta |= (1<<7);
									}
									else
									{
										if((System_Sta.Disp_Sta&(1<<2))!=0)		//WIFI设置界面
										{
											System_Sta.Disp_Sta &= ~(1<<2);
											System_Sta.Disp_Sta |= (1<<1);		//进入主界面
											System_Sta.Disp_Sta |= (1<<7);
										}
									}
								}
								TPR_Structure.Gesture_Sta &= ~(1<<2);
								break;
					//向下锁屏
					case 0x08:	if((System_Sta.Disp_Sta &(1<<5)) == 0)
								{
									System_Sta.Motor |= (1<<7);//触发一次震动
									//System_Sta.Disp_Sta &= 0xe0;
									System_Sta.Disp_Sta |= (1<<5);
									System_Sta.Get_backlit |= (1<<7);		//关闭背光控制
									TIM3->CCR3 = 0;
									//GPIOB->BSRRH |=(1<<0);
								}
								TPR_Structure.Gesture_Sta &= ~(1<<3);
								break;
					default : break;	
				}
					TPR_Structure.Gesture_Sta &= ~(1<<7);			//清除手势发生标志位
					TPR_Structure.Gesture_Sta &= ~(1<<6);			//清除手势发生标志位
			}
			else
			{
				TPR_Structure.Gesture_Sta &= ~(1<<7);			//清除手势发生标志位
				TPR_Structure.Gesture_Sta &= ~(1<<6);			//清除手势发生标志位
			}
		
		}
		//GUI_Exec();	
		delay_ms(5);
	}										 
}	  


//开始任务
void start_task(void *pdata)
{	
//	u8 err;
    OS_CPU_SR cpu_sr=0;
	pdata = pdata; 	
//	sem_smartlink = OSSemCreate(0);		//smart link 信号量
	OSStatInit();					//初始化统计任务.这里会延时1秒钟左右	
 	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
	OSTaskCreate(Display_Task,(void *)0,(OS_STK *)&DISPLAY_TASK_STK[DISPLAY_STK_SIZE-1],DISPLAY_TASK_PRIO);
	OSTaskCreate(Wifi_Task,(void *)0,(OS_STK *)&WIFI_TASK_STK[WIFI_STK_SIZE-1],WIFI_TASK_PRIO);	
	OSTaskCreate(Touch_Task,(void *)0,(OS_STK *)&TOUCH_TASK_STK[TOUCH_STK_SIZE-1],TOUCH_TASK_PRIO);		
	OSTaskCreate(UpDate_Task,(void *)0,(OS_STK *)&UPDATE_TASK_STK[UPDATE_STK_SIZE-1],UPDATE_TASK_PRIO);	
 	OSTaskCreate(Battery_Task,(void *)0,(OS_STK *)&BATTERY_TASK_STK[BATTERY_STK_SIZE-1],BATTERY_TASK_PRIO);
	OSTaskCreate(Warning_Task,(void *)0,(OS_STK *)&WARNING_TASK_STK[WARNING_STK_SIZE-1],WARNING_TASK_PRIO);	
	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
}

#endif
