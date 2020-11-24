#include "display.h"
#include "main.h"
extern Clock My_Clock;
extern SYSTEM_STA System_Sta;
extern GUI_CONST_STORAGE GUI_FONT GUI_Fontmyfont;
extern SYSTEM_DATA System_Data;
extern u8 H_Average_Buf[12];
extern u8 T_Average_Buf[12];
extern GUI_CONST_STORAGE GUI_BITMAP bmwifiok;
extern GUI_CONST_STORAGE GUI_BITMAP bmwifino;

void Main_Window(WM_HWIN *head)
{
	if((System_Sta.Disp_Sta&(1<<6))!= 0)	//如何是从WIFI设置界面转过来则删除WIFI界面
	{
		System_Sta.Disp_Sta &= ~(1<<6);
		WM_DeleteWindow(*head);
		GUI_Clear();
	}
	if((System_Sta.Disp_Sta&(1<<4))!= 0)
	{
		System_Sta.Disp_Sta &= ~(1<<4);
		WM_DeleteWindow(*head);
		GUI_Clear();
	}
	if((System_Sta.Disp_Sta&(1<<0))!= 0)
	{
		System_Sta.Disp_Sta &= ~(1<<0);//标志位清0
		WM_DeleteWindow(*head);
		GUI_Clear();
	}
	Disp_Watch();
}

void Wifi_Window(WM_HWIN *head)
{
	*head = Create_WifiWindow();
	GUI_Exec();
	WM_SelectWindow(*head);
	WM_EnableMemdev(*head);
	System_Sta.Disp_Sta &= ~(1<<7);			//这个标志位用来避免重复显示
	System_Sta.Disp_Sta |= (1<<6);			//标志已显示标志位
}

void Data_Window(WM_HWIN *head)
{
	GUI_SetBkColor(GUI_RED);
	*head = Create_DataWindow();
	WM_SelectWindow(*head);
	Disp_Data(head);
	GUI_Exec();
	WM_EnableMemdev(*head);
	System_Sta.Disp_Sta &= ~(1<<7);	//这个标志位用来避免重复显示
	System_Sta.Disp_Sta |= (1<<4);	//标志已显示标志位
}

void Help_Window(WM_HWIN *head)
{
	if((System_Sta.Disp_Sta&(1<<1)) == 0)		//当前不是主界面
	{
		System_Sta.Disp_Sta &= ~(1<<6);		//这两个标志位用来避免重复删除和刷新
		System_Sta.Disp_Sta &= ~(1<<4);
		System_Sta.Disp_Sta &= ~(14<<1);	//清楚已显示标志位
		WM_DeleteWindow(*head);					//不是主界面就删除之前显示的窗口
		*head = Create_HelpWindow();			
		WM_EnableMemdev(*head);				//这里使报警的界面不会闪动
		System_Sta.Disp_Sta |= (1<<0);		//置位标志位，这里为了防止重复刷屏，以及关闭手势		
	}
	else	//是主界面
	{
		*head = Create_HelpWindow();	//直接显示不用删除
		GUI_Exec();
		WM_EnableMemdev(*head);	
		System_Sta.Disp_Sta |= (1<<0);		//置位标志位
		System_Sta.Disp_Sta &= ~(1<<1);		//清楚主界面标志位	
	}
}

void Remind_Window(WM_HWIN *head)
{
	if((System_Sta.Disp_Sta&(1<<1)) == 0)		//当前不是主界面
	{
		System_Sta.Disp_Sta &= ~(1<<6);		//这两个标志位用来避免重复删除和刷新
		System_Sta.Disp_Sta &= ~(1<<4);
		System_Sta.Disp_Sta &= ~(14<<1);	//清楚已显示标志位
		WM_DeleteWindow(*head);					//不是主界面就删除之前显示的窗口
		*head = Create_MedicineWindow();			
		WM_EnableMemdev(*head);				//这里使报警的界面不会闪动
		System_Sta.Disp_Sta |= (1<<0);		//置位标志位，这里为了防止重复刷屏，以及关闭手势		
	}	
	else	//是主界面
	{
		*head =  Create_MedicineWindow();	//直接显示不用删除
		GUI_Exec();
		WM_EnableMemdev(*head);	
		System_Sta.Disp_Sta |= (1<<0);		//置位标志位
		System_Sta.Disp_Sta &= ~(1<<1);		//清楚主界面标志位	
	}
}

//锁屏时的界面处理
int Disp_lockWindow(WM_HWIN *head)
{
	if((System_Sta.Warning&0xe0)!= 0)	//报警发生
	{
		System_Sta.Get_backlit &= ~(1<<7);		//开启背光控制
		TIM3->CCR3 = 500;
		delay_ms(20);
		System_Sta.Disp_Sta &= ~(1<<5);		//清除锁屏标志位 
	}
	else
	{
		if((((System_Sta.Remind&(1<<7))!=0) &&((System_Sta.Remind&(1<<6))==0)) || (((System_Sta.Remind1&(1<<7))!=0) &&((System_Sta.Remind1&(1<<6))==0))||(((System_Sta.Remind2&(1<<7))!=0) &&((System_Sta.Remind2&(1<<6))==0)))
		{
			System_Sta.Get_backlit &= ~(1<<7);		//开启背光控制
			TIM3->CCR3 = 500;
			delay_ms(20);
			System_Sta.Disp_Sta &= ~(1<<5);		//清除锁屏标志位 
		}
	}
	return 0 ;
}
//整个数据界面的更新
void Disp_Update(WM_HWIN *head)
{
	//有数据正在测量，进入更新
	if(((System_Sta.Get_Heart&(1<<15))!=0)||((System_Sta.Get_Temp&(1<<15))!=0)||((System_Sta.Measure&(1<<7))!=0))
	{
		Disp_Circle(head);//正在测量显示标志
	}
	else 
	{
		if(((System_Sta.Get_Heart&(1<<14))!=0)||((System_Sta.Get_Temp&(1<<14))!=0)||((System_Sta.Measure&(1<<6))!=0))		//测量成功
		{
			Disp_Data(head); //更新页面数据
			System_Sta.Get_Heart &= ~(1<<14);	//清除成功标志位
			System_Sta.Get_Temp &= ~(1<<14);
			if((System_Sta.Measure&(1<<6))!=0)
			{
				System_Sta.Measure &= ~(1<<6);		
			}
		}
		else
		{
			if(((System_Sta.Get_Heart&(1<<13))!=0)||((System_Sta.Get_Temp&(1<<13))!=0))
			{
					GUI_SetColor(GUI_WHITE);
					GUI_FillCircle(5, 5, 5);
					GUI_FillCircle(3, 86, 5);
				//GUI_FillCircle(5, 170, 5);   
				//GUI_DispStringAt("")
				/*测量失败请重试*/
			}
		}
	}
}


//显示测量数据
void Disp_Data(WM_HWIN *head)
{
	u16 buf;
//	u8 i = 0;
	//GUI_Clear();
	GUI_SetBkColor(GUI_WHITE);
	GUI_ClearRect(137,83,240,158);		//心率数值区域
	GUI_ClearRect(137,3,240,78);		//体温数值区域
	GUI_ClearRect(164,163,240,240);		//血压区域
	GUI_SetColor(GUI_WHITE);
	GUI_FillCircle(5, 5, 5);
	GUI_FillCircle(3, 86, 5);
	GUI_FillCircle(5, 170, 5);         
	GUI_SetColor(GUI_RED);
	GUI_SetFont(&GUI_Font32_ASCII);
	//WM_SelectWindow(*head);
	//体温
	buf = ((u16)(System_Data.temperature*10))%10;//提取小数点后一位
	GUI_DispDecAt((u8)System_Data.temperature,137,3,2);	
	GUI_DispStringAt(".",166,3);
	GUI_DispDecAt(buf,172,3,1);
	//显示体温平均值
	buf = ((u16)(System_Data.temp_average*10))%10;//提取小数点后一位
	GUI_DispDecAt((u8)System_Data.temp_average,137,43,2);	
	GUI_DispStringAt(".",166,43);
	GUI_DispDecAt(buf,172,43,1);
		
	//心率
	//显示心率平均值
	if((System_Data.heart_average/100) != 0)		//均值3位数
	{
		GUI_DispDecAt(System_Data.heart_average,137,123,3);		//显示心率
	}
	else
	{
		GUI_DispDecAt(System_Data.heart_average,137,123,2);
	}
	//显示测量值
	if((System_Data.heart/100) != 0)	//三位数
	{
		
		GUI_DispDecAt(System_Data.heart,137,83,3);		//显示心率
	}
	else
	{
		GUI_ClearRect(137,83,175,113);
		GUI_DispDecAt(System_Data.heart,137,83,2);		//显示心率
	}
	//血压
	GUI_DispDecAt((System_Data.boolpressure_up),164,163,3);		//收缩压
	GUI_DispDecAt((System_Data.boolpressure_down),164,203,2);	//舒张压
	/*显示文字*/
	
	//体温文字
	GUI_SetFont(&GUI_Fontmyfont);
	GUI_DispStringAt("\xe5\xba\xa6",187,3);		//度
	if(System_Data.temperature>37)		
	{
		GUI_DispStringAt("\xe9\xab\x98",215,20);//显示高
	}
	else
	{
		if(System_Data.temperature<36)
		{
			GUI_DispStringAt("\xe4\xbd\x8e",215,20);//显示低
		}
	}
	
	//心率文字
	if(System_Data.heart>101)		
	{
		GUI_DispStringAt("\xe9\xab\x98",215,105);//显示高
	}
	else
	{
		if(System_Data.heart<55)
		{
			GUI_DispStringAt("\xe4\xbd\x8e",215,108);//显示低
		}
	}
	GUI_SetFont(&GUI_Font24_ASCII);
	GUI_DispStringAt("/bpm",185,83);
	GUI_DispStringAt("/bpm",185,137);
	GUI_Exec();
}

//显示正在测量的标识位
void Disp_Circle(WM_HWIN *head)
{
	static u8 times ;
	GUI_SetColor(GUI_RED);
	times++;
	if((System_Sta.Get_Temp&(1<<15))!=0)		//心率在测量
	{
		if(times < 25)
		{
			GUI_SetColor(GUI_RED);
			GUI_FillCircle(5, 5, 5);
		}
		else
		{
			if(times <50)
			{
				GUI_SetColor(GUI_WHITE);
				GUI_FillCircle(5, 5, 5);
			}
			else
			{
				times = 0;
			}
		}
	}
	if((System_Sta.Get_Heart&(1<<15))!=0)		//体温在测量
	{	
		if(times < 25)
		{
			GUI_SetColor(GUI_RED);
			GUI_FillCircle(3, 86, 5);
		}
		else
		{
			if(times < 50)
			{
				GUI_SetColor(GUI_WHITE);
				GUI_FillCircle(3, 86, 5);	
			}
			else
			{
				times = 0;
			}
		}
	}
	if((System_Sta.Measure&(1<<7))!=0 )		//血压在测量
	{
		if(times < 25)
		{
			GUI_SetColor(GUI_RED);
			GUI_FillCircle(5, 170, 5);
		}
		else
		{
			if(times < 50)
			{
				GUI_SetColor(GUI_WHITE);
				GUI_FillCircle(5, 170, 5);
			}
			else
			{
				times = 0;
			}
		}
	}
	GUI_Exec();
}



void Disp_Connection(WM_HWIN *head)
{
	static u8 times = 0;
	WM_HWIN hItem;
	if((System_Sta.Wifi_Connection&(1<<4))==0)	//第一次进来
	{
		times = 0;
		System_Sta.Wifi_Connection |= (1<<4);		//这里表示已经显示了文字
		GUI_SetFont(&GUI_Fontmyfont);
		GUI_SetColor(GUI_RED);
		GUI_DispStringAt("Wifi\xe9\x93\xbe\xe6\x8e\xa5\xe4\xb8\xad",50,200);
	}
	GUI_SetFont(&GUI_Font32_ASCII);
	switch(times/50)
	{
		case 0: GUI_DispStringAt(".",180,203);break;
		case 1:	GUI_DispStringAt(".",190,203);break;
		case 2: GUI_DispStringAt(".",200,203);break;
		case 3:	GUI_DispStringAt(".",210,203);break;
		case 4: GUI_DispStringAt(".",220,203);break;
		default:times=1;
				GUI_SetColor(GUI_WHITE);
				GUI_FillRect(180,190,240,240);
				GUI_SetColor(GUI_RED);
				break;
	}
	times++;
	if((System_Sta.Wifi_Connection&(1<<6)) != 0)
	{
		System_Sta.Wifi_Connection = 0;
		hItem = WM_GetDialogItem(*head, 0x800);
		BUTTON_SetBitmapEx(hItem,0,&bmwifino,0,0);
		Disp_No();
	}
	else
	{
		if((System_Sta.Wifi_Connection&(1<<5)) != 0)
		{
			System_Sta.Wifi_Connection = 0;
			hItem = WM_GetDialogItem(*head, 0x800);
			BUTTON_SetBitmapEx(hItem,0,&bmwifiok,0,0);
			Disp_Ok();
		}
	}
		
}

void Disp_Ok(void)
{
	GUI_SetColor(GUI_WHITE);
	GUI_FillRect(50,200,240,240);
	GUI_SetFont(&GUI_Fontmyfont);
	GUI_SetColor(GUI_RED);
	GUI_DispStringAt("\xe9\x93\xbe\xe6\x8e\xa5\xe6\x88\x90\xe5\x8a\x9f!",60,200);
}

void Disp_No(void)
{
	GUI_SetColor(GUI_WHITE);
	GUI_FillRect(50,200,240,240);
	GUI_SetFont(&GUI_Fontmyfont);
	GUI_SetColor(GUI_RED);
	GUI_DispStringAt("\xe9\x93\xbe\xe6\x8e\xa5\xe5\xa4\xb1\xe8\xb4\xa5!",60,200);
}

void Motor_Work(void)
{
	static u8 times;
	if((System_Sta.Motor&(1<<6))!= 0)//开启长时间震动
	{
		if(times<80)
		{
			MOTOR_START;
		}
		else
		{
			if(times<120)
			{
				MOTOR_STOP;
			}
			else
			{
				times = 0;
			}
			if((System_Sta.Motor&(1<<5))!=0)
			{
				System_Sta.Motor = 0;
				MOTOR_STOP;
				times = 0;
			}
		}
	}
	else
	{		
		if(times<10)
		{
			MOTOR_START;
		}
		else 
		{
			MOTOR_STOP;
			times = 0;
			System_Sta.Motor = 0;
		}
	}
	times++;
}
