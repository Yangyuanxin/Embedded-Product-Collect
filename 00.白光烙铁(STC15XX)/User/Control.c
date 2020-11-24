/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2017,BG0BGH
 *     All rights reserved.
 *
 *     除注明出处外，以下所有内容版权均属王志浩所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留作者的版权声明。
 *
 * @file			Control.c
 * @brief			
 * @author		王志浩
 * @version		V2.2
 * @date			2017-8-30
 */
#include "config.h"

unsigned int IronTemp_Now = 0;//当前温度
unsigned int IronTemp_AM = 375;//目标温度
unsigned char SetTempFlag = 0;//设定温度标志位，该位被置后由定时器清零，可大字显示设定温度

//NTC温度表，依次为-10℃~50℃对应的ADC值
 unsigned int code NTCTAB[61] =
{
	324, 318, 312, 306, 300, 294, 288, 282, 276, 269, 263, 257, 251, 245, 238, 232, 226, 220, 214, 209, 
	203, 197, 192, 186, 181, 175, 170, 165, 160, 155, 150, 146, 141, 137, 132, 128, 124, 120, 116, 112, 
	108, 105, 101,  98,  95,  92,  89,  86,  83,  80,  77,  75,  72,  70,  67,  65,  63,  61,  59,  57,
	55 
};


//BEAT的中值滤波程序，去掉最大值最小值求平均
static u16 mid_filter(unsigned char channel)
{
	u16 var_buf[NUM] = 0 ;            						//缓冲数据
	u16	sum = 0,var_max = 0, var_min = 1023; 			//最值初值装入最小值装满值 最大值装空
	u8 temp;
	for(temp = 0; temp < NUM; temp++)
	{
	 var_buf[temp] = Get_ADC10bitResult(channel); //下面代替延时
	 if(var_buf[temp] > var_max)   							  //最大
		 {
			 var_max = var_buf[temp];
		 }
	 else if(var_buf[temp] < var_min)  					  //最小
		 {
			 var_min = var_buf[temp];
		 }
		sum +=var_buf[temp];  
	}
	sum -= var_max;
	sum -= var_min;																//去除最值
	return(sum >> DIV);  													//右移代替除法
}
//限幅滤波程序，温度在限定幅度范围内波动时，return值保持上一次的值不变
unsigned int lim_filter(unsigned int data_new,unsigned int lim)
{
	static unsigned int data_old;
	int deta;
	deta = (int)(data_new - data_old);
	if(deta > lim || deta <-lim)
	{
		data_old = data_new;
		return data_new;
	}
	else
	{
		data_old = data_new;
		return data_old;
	}
}

//ADC读电压
float GetVoltage(void)
{
	return ((float)Get_ADC10bitResult(3) * (float)FAC_DATA_Table.Adjust_s.Voltage) / 6801.0f;
}

//查表法读室温，二分法查表
float GetRoomTemp(void)
{
	u16 temp=0;
	unsigned char det = 60,head = 0,tail = 60,mid = 0;
	
	temp = Get_ADC10bitResult(7);

	//小于-10℃直接等于
	if(temp >= NTCTAB[head])
	{
		return -10.0f;
	}
	//大于50℃直接等于
	else if(temp <= NTCTAB[tail])
	{
		return 50.0f;
	}
	//否则，二分法查表
	else
	{
		//头-尾 ！= 1 就循环
		while(det > 1)
		{
			//中值
			mid = (head + tail)/2;
			//如果恰好为中值 直接return
			if(temp == (NTCTAB[mid]))
			{
				return (float)(mid - 10);
			}
			else if(temp > (NTCTAB[mid]))
			{
				tail = mid;
			}
			else
			{
				head = mid;
			}
			det = tail - head;
		}
		return ((float)head-10.0f+1.0f/(float)(NTCTAB[head]-NTCTAB[tail]));
	}
}

//读烙铁温度，查表法
unsigned int GetIronTemp(unsigned char num)
{
	u16 temp=0;
	unsigned char det = 13,head = 0,tail = 13,mid = 0,i=0;
	
	//关烙铁，避免读到24V电平，将来有调用他的函数再决定打不打开烙铁
	Iron_Off();
	
	//延时并先空读几次避免误差
	//Delay_us(20);
	Get_ADC10bitResult(5); 
	//Get_ADC10bitResult(5);
	temp = lim_filter(mid_filter(5),20);
	//temp = mid_filter(5);
	
	//小于50℃返回0
	if(temp <= FAC_DATA_Table.Heater_Tip[num-1][head])
	{
		return 0;
	}
	//大于700℃直接等于
	else if(temp < 900 && temp>=FAC_DATA_Table.Heater_Tip[num-1][tail])
	{
		return 700;
	}
	else if(temp > 900)
	{
		return 800;
	}
	//否则，二分法查表
	else
	{
		//头-尾 ！= 1 就循环
		while(det > 1)
		{
			//中值
			mid = (head + tail) / 2;
			//如果恰好为中值 直接return
			if(temp == FAC_DATA_Table.Heater_Tip[num-1][mid])
			{
				return mid * 50 + 50;
			}
			else if(temp < (FAC_DATA_Table.Heater_Tip[num-1][mid]))
			{
				tail = mid;
			}
			else
			{
				head = mid;
			}
			det = tail - head;
		}
		//线性插值计算
		return 50 * (head + 1) + (unsigned int)((float)(temp - FAC_DATA_Table.Heater_Tip[num-1][head]) * 50.0f / (float)(FAC_DATA_Table.Heater_Tip[num-1][tail] - FAC_DATA_Table.Heater_Tip[num-1][head]));
	}
}
//PD控制烙铁温度，P参数分段
void IronTempControl(unsigned int temp)
{
	static float fDelta[2]; //0是当前误差,1是上次误差；
	float fP=0,fD=0;
	float Control_P = 1.0f,Control_D = 0.01f;
	float ControlOut = 0;
	//反馈温度
	if(FtyModeFlag)
	{
		IronTemp_Now = GetIronTemp(FAC_DATA_Table.Adjust_s.Iron);
	}
	else
	{
		IronTemp_Now = GetIronTemp(MENU_DATA_Table.Other_s.Heater_Tip);
	}
	
  //误差计算
  fDelta[0] = (float)temp - (float)IronTemp_Now; 
  
	//P参数分段
	if(fDelta[0] > 40)
	{
		Control_P = 6.5f;
	}
	else if(fDelta[0] > 30)
	{
		Control_P = 5.5f;
	}
	else if(fDelta[0] > 20)
	{
		Control_P = 4.0f;
	}
	else if(fDelta[0] > 10)
	{
		Control_P = 2.5f;
	}
	else if(fDelta[0] > 5)
	{
		Control_P = 1.0f;
	}
  //PD计算
  fP = Control_P * fDelta[0];
  fD = Control_D * (fDelta[0] - fDelta[1]);
	
  ControlOut = fP + fD;
  
	//设定温度大于50且小于最高温度才开烙铁
	if(temp > 50 && temp <= MENU_DATA_Table.Temp_s.Max_Temp && ((unsigned char)GetVoltage()>=FAC_DATA_Table.Other_s.Lim_Voltage))
	{
		//大于255
		if(ControlOut > 255.0f)
		{
			Iron_Run();
			ControlOut = 255.0f;
			UpdatePwm((unsigned char)ControlOut);
		}
		//正常
		else if(ControlOut > 0.001f)
		{
			Iron_Run();
			UpdatePwm((unsigned char)ControlOut);
		}
		//负数
		else
		{
			ControlOut = 0.0f;
			Iron_Off();
		}
	}
	else
	{
		ControlOut = 0.0f;
		Iron_Off();
	}

	//传递
  fDelta[1] = fDelta[0];
	
}

//普通模式下的监控界面
void Normal_Monitor(void)
{
	int deta = 0,tmp;
	static unsigned int IronTemp_Display = 0,sum = 0,counter = 0;
	//对显示的温度再次滤波保持显示值稳定
	if(counter<50)
	{
		sum += lim_filter(IronTemp_Now,20);
		//sum += IronTemp_Now;
		counter ++;
	}
	else
	{
		IronTemp_Display = sum / (counter-2);
		counter = 0;
		sum = 0;
	}
	deta = (int)IronTemp_Display - (int)IronTemp_AM;
	if(deta<10 && deta>-10)
	{
		IronTemp_Display = IronTemp_AM;
	}
	
	
	if(counter == 1)
	{
		//显示电压
		sprintf((char *)str_buff , "%3.1fV" ,GetVoltage());
		OLED_P8x16Str(0,6,str_buff); 
		//室温
		sprintf((char *)str_buff , "%3.0f`C" ,GetRoomTemp());
		OLED_P8x16Str(48,6,str_buff); 
		//烙铁头
		tmp = MENU_DATA_Table.Other_s.Heater_Tip;
		sprintf((char *)str_buff , "%1d" ,tmp);
		OLED_P8x16Str(112,6,str_buff); 
	}


	if(P01)
	{
		OLED_P8x16Str(96,6,"."); 
	}
	else
	{
		OLED_P8x16Str(96,6," "); 
	}
	//编码器调整温度
	if(ec11_state.Coder == 1)
	{
		IronTemp_AM +=FAC_DATA_Table.Other_s.Temp_Step;
		SetTempFlag = 1;
		ec11_state.Coder = 0;
		MENU_DATA_Table.Temp_s.Last_Temp = IronTemp_AM;
	}
	else if(ec11_state.Coder == 2)
	{
		IronTemp_AM -=FAC_DATA_Table.Other_s.Temp_Step;
		SetTempFlag = 1;
		ec11_state.Coder = 0;
		MENU_DATA_Table.Temp_s.Last_Temp = IronTemp_AM;
	}
	//限制设定温度范围
	if(IronTemp_AM > 700)
	{
		IronTemp_AM = 0;
	}
	else if(IronTemp_AM > MENU_DATA_Table.Temp_s.Max_Temp)
	{
		IronTemp_AM = MENU_DATA_Table.Temp_s.Max_Temp;
	}
	else if(IronTemp_AM < 0)
	{
		IronTemp_AM = 0;
	}
	//显示设定温度
	sprintf((char *)str_buff , "设定:%3u`C      " ,IronTemp_AM);
	OLED_printf(0,0,str_buff,0); 
	
	
	//设定温度时大字显示设定的温度
	if(SetTempFlag)
	{
		if(IronTemp_Display<690)
		{
			sprintf((char *)str_buff , " %3u `C " ,IronTemp_AM);
			OLED_P16x32Temp(0,2,str_buff);	
		}
		else
		{
			OLED_P16x32Temp(0,2," --- `C ");	
		}
	}
	else
	{
		if(IronTemp_Display<690)
		{
			sprintf((char *)str_buff , " %3u `C " ,IronTemp_Display);
			OLED_P16x32Temp(0,2,str_buff);	
		}
		else
		{
			OLED_P16x32Temp(0,2," --- `C ");	
		}
	}
}
unsigned int chn=0;
void Channel_Monitor(void)
{
	int deta = 0,tmp;
	static unsigned int IronTemp_Display = 0,sum = 0,counter = 0;
	//显示温度滤波
	if(counter<50)
	{
		sum += lim_filter(IronTemp_Now,25);
		counter ++;
	}
	else
	{
		IronTemp_Display = sum / (counter - 2);
		counter = 0;
		sum = 0;
	}
	deta = (int)IronTemp_Display - (int)IronTemp_AM;
	if(deta<10 && deta>-10)
	{
		IronTemp_Display = IronTemp_AM;
	}
	
	if(counter == 1)
	{
		//显示电压
		sprintf((char *)str_buff , "%3.1fV" ,GetVoltage());
		OLED_P8x16Str(0,6,str_buff); 
		//室温
		sprintf((char *)str_buff , "%3.0f`C" ,GetRoomTemp());
		OLED_P8x16Str(48,6,str_buff); 
		//烙铁头
		tmp = MENU_DATA_Table.Other_s.Heater_Tip;
		sprintf((char *)str_buff , "%1d" ,tmp);
		OLED_P8x16Str(112,6,str_buff); 
	}
	
	//显示烙铁工作状态
	if(IronUseFlag)
	{
		OLED_P8x16Str(96,6,"."); 
	}
	else
	{
		OLED_P8x16Str(96,6," "); 
	}
	
	//编码器调整通道
	if(ec11_state.Coder == 1)
	{
		chn ++;
		SetTempFlag = 1;
		ec11_state.Coder = 0;
		MENU_DATA_Table.Temp_s.Last_CH = chn;
	}
	else if(ec11_state.Coder == 2)
	{
		chn --;
		SetTempFlag = 1;
		ec11_state.Coder = 0;
		MENU_DATA_Table.Temp_s.Last_CH = chn;
	}
	//限制通道范围
	if(chn > 100)
	{
		chn = 0;
	}
	if(chn > 5)
	{
		chn = 5;
	}
	else if(chn < 0)
	{
		chn = 0;
	}
	
	//设定目标温度
	switch (chn)
	{
		case 0:
		{
			chn = 0;
			IronTemp_AM = 0;
		}break;
		case 1:
		{
			IronTemp_AM = FAC_DATA_Table.Channel_s.Ch1;
		}break;
		case 2:
		{
			IronTemp_AM = FAC_DATA_Table.Channel_s.Ch2;
		}break;
		case 3:
		{
			IronTemp_AM = FAC_DATA_Table.Channel_s.Ch3;
		}break;
		case 4:
		{
			IronTemp_AM = FAC_DATA_Table.Channel_s.Ch4;
		}break;
		case 5:
		{
			IronTemp_AM = MENU_DATA_Table.Temp_s.Max_Temp;
		}break;
	}
	sprintf((char *)str_buff , "设定:%3u`C" ,IronTemp_AM);
	OLED_printf(0,0,str_buff,0); 	
	sprintf((char *)str_buff , "通道:%1u" ,chn);
	OLED_printf(78,0,str_buff,0); 

	//设定温度时大字显示设定的温度	
	if(SetTempFlag)
	{
		if(IronTemp_Display<690)
		{
			sprintf((char *)str_buff , " %3u `C " ,IronTemp_AM);
			OLED_P16x32Temp(0,2,str_buff);	
		}
		else
		{
			OLED_P16x32Temp(0,2," --- `C ");	
		}
	}
	else
	{
		if(IronTemp_Display<690)
		{
			sprintf((char *)str_buff , " %3u `C " ,IronTemp_Display);
			OLED_P16x32Temp(0,2,str_buff);	
		}
		else
		{
			OLED_P16x32Temp(0,2," --- `C ");	
		}
	}
}

unsigned int	LastVotage=0;
unsigned char FtyModeFlag = 0;
void Factory_Model(void)
{
	unsigned int temp[9]={0},str=0;
	unsigned char i;
	unsigned char code recoverycode[5] = {0xff,0xff,0xff,0xff,0xff};
	
	IronTemp_AM = 0;
	ShutdownCounterFlag = 1;
	FtyModeFlag = 1;
	
	
	//先记录下这个值，将来如果调整了，我们就计算
	LastVotage = FAC_DATA_Table.Adjust_s.Voltage;


	ParaSet_Fac();


	ec11_state.Key = 0;
	ec11_state.Coder = 0;

	//烙铁校准
	if(FAC_DATA_Table.Adjust_s.Iron > 0)
	{
		OLED_printf(0,0,"烙铁头 %d 校准",FAC_DATA_Table.Adjust_s.Iron);
		OLED_printf(0,1,"每50摄氏度校准",0);
		OLED_printf(0,2,"烙铁头一次!",0);
		while(ec11_state.Key != 1);
		ec11_state.Key = 0;
		OLED_CLS();
		Iron_Run();
		for(i=0;i<=13;i++)
		{
			OLED_printf(0,0,"%d `C时",50+i*50);
			OLED_printf(0,1,"AD = :%d",FAC_DATA_Table.Heater_Tip[FAC_DATA_Table.Adjust_s.Iron][i]);
			OLED_printf(0,3,"短按编码器保存",0);
			
			if(i<=8)
			{
				IronTemp_AM = 50+i*50;
			}
			else
			{
				IronTemp_AM = 0;
			}
			
			while(ec11_state.Key != 1)
			{
				if(ec11_state.Coder == 1)
				{
					FAC_DATA_Table.Heater_Tip[FAC_DATA_Table.Adjust_s.Iron-1][i] ++;
					OLED_printf(0,1,"AD = :%d",FAC_DATA_Table.Heater_Tip[FAC_DATA_Table.Adjust_s.Iron-1][i]);
					ec11_state.Coder = 0;
				}
				if(ec11_state.Coder == 2)
				{
					FAC_DATA_Table.Heater_Tip[FAC_DATA_Table.Adjust_s.Iron-1][i] --;
					OLED_printf(0,1,"AD = :%d",FAC_DATA_Table.Heater_Tip[FAC_DATA_Table.Adjust_s.Iron-1][i]);
					ec11_state.Coder = 0;
				}
			}
			ec11_state.Key = 0;
			
		}

		OLED_CLS();
		OLED_printf(0,2,"(已校准烙铁头%d)",FAC_DATA_Table.Adjust_s.Iron);
		FAC_DATA_Table.Adjust_s.Iron = 0;
		//存储
		EEPROM_SectorErase(0x200);
		EEPROM_write_n(0x200,(unsigned char *)&FAC_DATA_Table,sizeof(FAC_DATA_Table));	
		

	}
	if(FAC_DATA_Table.Other_s.Recovery == 1)
	{
		OLED_printf(0,3,"(已恢复出厂设置)",0);
		FAC_DATA_Table.Other_s.Recovery = 0;
		//存储
		EEPROM_SectorErase(0x200);
		EEPROM_write_n(0x200,(unsigned char *)&FAC_DATA_Table,sizeof(FAC_DATA_Table));	
		
		EEPROM_SectorErase(0x0);
		EEPROM_write_n(0x0,(unsigned char *)&recoverycode,sizeof(recoverycode));
	}
	
	
	OLED_printf(0,0,"短按继续校准",0);
	OLED_printf(0,1,"长按退出工厂模式",0);
	while(ec11_state.Key == 0);
	if(ec11_state.Key == 2)
	{
		ec11_state.Key = 0;
		MENU_DATA_Table.Other_s.Work_Mode = 0;
		//存储
		EEPROM_SectorErase(0x0);
		EEPROM_write_n(0x0,(unsigned char *)&MENU_DATA_Table,sizeof(MENU_DATA_Table));
		ShutdownCounterFlag = 0;
		IAP_CONTR = 0x20;
	}
}
unsigned char First_Sleep = 1;
void Sys_Monitor()
{
	//如果不是在休眠或关屏，则显示监控信息
	if((IronSleepFlag||IronScrSaverFlag)!=1)
	{
		switch (MENU_DATA_Table.Other_s.Work_Mode)
		{
			case 0:
			{
				Normal_Monitor();
			}break;
			case 1:
			{
				Channel_Monitor();
			}break;
			case 2:
			{
				
				Factory_Model();
				
			}break;
			default:break;
		}
	}
	//如果是休眠
	else if(IronSleepFlag)
	{
		OLED_P8x16Str(0,0,"                ");
		OLED_P8x16Str(0,2,"   Sleeping...  ");
		OLED_P8x16Str(0,4,"                ");
		OLED_P8x16Str(0,6,"                ");
	}
	//关屏
	else if(IronScrSaverFlag)
	{
		OLED_P8x16Str(0,0,"                ");
		OLED_P8x16Str(0,2,"                ");
		OLED_P8x16Str(0,4,"                ");
		OLED_P8x16Str(0,6,"                ");
	}
}
