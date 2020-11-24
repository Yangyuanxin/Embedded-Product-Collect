/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2017,BG0BGH
 *     All rights reserved.
 *
 *     除注明出处外，以下所有内容版权均属王志浩所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留作者的版权声明。
 *
 * @file			EC11.c
 * @brief			
 * @author		王志浩
 * @version		V2.2
 * @date			2017-8-30
 */
#include "config.h"


EC11_State_s	ec11_state;
unsigned char str_buff[20];//缓存
unsigned char ShutdownCounterFlag;//如果进菜单或者工厂模式，该位置1，停止休眠计数器计数
unsigned char BeepCounterFlag = 0;//蜂鸣器工作标志位

//调用此函数打开蜂鸣器，并打开定时器0，由定时器0来调整蜂鸣器响的时间进而调整音量，避免延时函数对主程序影响
void Beep(unsigned int beep)
{
	if(beep>0 && beep<11)
	{
		BeepCounterFlag = 1;
	}
	else
	{
		BeepCounterFlag = 0;
	}
}

//捕捉EC11按下和旋转信息
void EC11_Capture(void)
{
	static unsigned char Aold=0,Bold=0;
	static unsigned char RotatingFlag=0;
	static unsigned int KeyDown_Counter=0,KeyDown_Counter_Old=0;

	//如果同时为高电平，说明转了
	if(EC11_A && EC11_B)
	{
		RotatingFlag = 1;
	}
	//如果转了之后
	if(RotatingFlag)
	{
		//同时为低电平，判断上次的值，即可得出转的方向
		if(!EC11_A && !EC11_B)
		{
			if(Aold)
			{
				RotatingFlag = 0;
				CoderUseFlag = 1;
				if(!FAC_DATA_Table.Adjust_s.Encoder)
				{
					Beep(MENU_DATA_Table.Other_s.Beep_Volume);
					ec11_state.Coder = 2;
				}
				else
				{
					Beep(MENU_DATA_Table.Other_s.Beep_Volume);
					ec11_state.Coder = 1;
				}
			}
			if(Bold)
			{
				RotatingFlag = 0;
				CoderUseFlag = 1;
				if(!FAC_DATA_Table.Adjust_s.Encoder)
				{
					Beep(MENU_DATA_Table.Other_s.Beep_Volume);
					ec11_state.Coder = 1;
				}
				else
				{
					Beep(MENU_DATA_Table.Other_s.Beep_Volume);
					ec11_state.Coder = 2;
				}
			}
		}
	}
	//保存上次的值
	Aold = EC11_A;
	Bold = EC11_B;
	
	//记录按键按下时间
	if(!EC11_KEY)
	{
		_nop_();
		if(!EC11_KEY)
		{
			KeyDown_Counter++;
			if(KeyDown_Counter >= KEYHOLD_TIME)
			{
				BEEP_ON;
				Delay_ms(100);
				BEEP_OFF;
			}
		}
	}
	else
	{
		KeyDown_Counter_Old = KeyDown_Counter;
		KeyDown_Counter = 0;
	}

	//根据按下时间判断长按短按
	if((KeyDown_Counter_Old < KEYHOLD_TIME) && (KeyDown_Counter_Old > KEYDOWN_TIME))
	{
		Beep(MENU_DATA_Table.Other_s.Beep_Volume);
		ec11_state.Key = 1;
		CoderUseFlag = 1;
	}
	else if(KeyDown_Counter_Old >= KEYHOLD_TIME)
	{
		//Beep(MENU_DATA_Table.Other_s.Beep_Volume);
		ec11_state.Key = 2;
		CoderUseFlag = 1;
		KeyDown_Counter_Old = KEYHOLD_TIME;
	}

}


static char Page_num = 0,Option_num = 80 ,option_var;//页号，选项号
/* 菜单数据 */
MENU_MSG_s MENU_MSG = 
{   
		{//页参数名
			{"温度设置"},
			{"时间设置"},
			{"其它设置"},
		},
		{//TEMP参数选项   
			{
				{"最高温度:%d"},   
				{"休眠温度:%d"},
				{"默认温度:%d"},
				{"默认通道:%d"},
			},
			{//TIME参数选项
				{"休眠时间:%d"}, 
				{"关屏时间:%d"},
				{"贷记时间:%d"},//待机时间
				{0},
			},
			{//OTHER参数选项
				{"工作模式:%u"},
				{"烙铁头选择:%u"},
				{"振动灵敏度:%u"},
				{"蜂鸣器音量:%u"},
			},
		}  
};
MENU_DATA_s MENU_DATA_Table = 
{ 
	//Max_Temp   	Slp_Temp   Default_Temp	Default_CH
	{ 410 ,     	0,       		0,					0				},//温度参数值
	//Slp_Time  	Scr_Time  	Shd_Time 
	{ 1 ,    			1,					1										},//时间参数值
	//Wrk_Mode  	Heater_Tip	Shk_Sens    Bep_Volm
	{ 0,        	1,       		0,					0				},//其他参数值
};
FAC_DATA_s					FAC_DATA_Table = 
{
	//工厂模式
	{    
		{//页参数名
			{"参书校准"},
			{"通道设置"},
			{"其它设置"}
		},
		{//TEMP参数选项   
			{
				{"烙铁头校准:%d"},   
				{"电压校准:%d"},
				{"编码器校准:%d"},
				{"恢复出厂:%d"},
			},
			{
				{"通道1:%d"},   
				{"通道2:%d"},
				{"通道3:%d"},
				{"通道4:%d"},
			},
			{
				{"温度步进:%d"},   
				{"保护电压:%d"},
				{"恢复出厂:%d"},
				{0},
			},
		},  
	},
		//以下为工厂模式参数
	//Iron				Voltage   	Encoder			
	{ 0,					244,     		0,									},//校准参数值
	//Ch1					Ch2   			Ch3					Ch4
	{ 200,				300,     		370,     		400			},//通道温度设定
	//Temp_Step		Lim_Voltage	Recovery
	{ 10,					20,				0,     								},//其他
	//烙铁头参数
	{
		{
			120,	173,	209,	245,	287,	329,	381,	432,	480,	565,	625,	685,	740,	800,
		},
		{
			118,	148,	184,	213,	264,	303,	350,	401,	480,	565,	625,	685,	740,	800,
		},
		{
			118,	148,	184,	213,	264,	303,	350,	401,	480,	565,	625,	685,	740,	800,
		},
		{
			120,	148,	186,	214,	255,	304,	340,	390,	480,	565,	625,	685,	740,	800,
		},
		{
			120,	148,	186,	214,	255,	304,	340,	390,	480,	565,	625,	685,	740,	800,
		},
		{
			120,	148,	186,	214,	255,	304,	340,	390,	480,	565,	625,	685,	740,	800,
		},
	},
};

/* OLED打印 */
void OLED_printf(unsigned char x,unsigned char y,char *str,float str_num)
{
	int num_temp = (int)(str_num );
	sprintf((char *)str_buff , str ,num_temp);//静态值
	OLED_Print(x,2*y,str_buff);     
}
unsigned char Select_flag = 0;
/* 光标 */
void Display_cursor(void)
{
	OLED_CLS();
	if(Option_num != 80)//页面光标
	{
		if(!Select_flag)
		{
			OLED_P8x16Str(104,2*Option_num,"< ");  
		}
		else
		{
			OLED_P8x16Str(104,2*Option_num,"<="); 
		}			
	}
	else								//选项光标
	{
		OLED_P8x16Str(112,2*Page_num+2,"< ");     
	}
}
/* 显示 */
void DisPlay(unsigned char num)
{
	unsigned char j;
	float var_temp;
	
	//显示一级菜单
	if(Option_num == 80)
	{
		OLED_P14x16Str(50,0,"菜单");
		for(j = 0;j < num;j++)//换行
		{
			OLED_printf(0,j+1,MENU_MSG.OPTINO_NAME[j],0);//显示值
		}
	}
	//显示二级菜单
	else
	{

		switch(Page_num)
		{
			case 0:
			{              
				var_temp = MENU_DATA_Table.Temp_s.Max_Temp;
				OLED_printf(0,0,MENU_MSG.ADS_str[Page_num][0],var_temp);//显示值
				var_temp = MENU_DATA_Table.Temp_s.Slp_Temp;
				OLED_printf(0,1,MENU_MSG.ADS_str[Page_num][1],var_temp);//显示值    
				var_temp = MENU_DATA_Table.Temp_s.Default_Temp;
				OLED_printf(0,2,MENU_MSG.ADS_str[Page_num][2],var_temp);//显示值
				var_temp = MENU_DATA_Table.Temp_s.Default_CH;
				OLED_printf(0,3,MENU_MSG.ADS_str[Page_num][3],var_temp);//显示值  				
			}break;
			case 1:
			{
				var_temp = MENU_DATA_Table.Time_s.Slp_Time;
				OLED_printf(0,0,MENU_MSG.ADS_str[Page_num][0],var_temp);//显示值 
				var_temp = MENU_DATA_Table.Time_s.ScrSaver_Time;
				OLED_printf(0,1,MENU_MSG.ADS_str[Page_num][1],var_temp);//显示值 
				var_temp = MENU_DATA_Table.Time_s.Shutdown_Time;
				OLED_printf(0,2,MENU_MSG.ADS_str[Page_num][2],var_temp);//显示值  				
			}break;
			case 2:
			{
				var_temp = MENU_DATA_Table.Other_s.Work_Mode;
				OLED_printf(0,0,MENU_MSG.ADS_str[Page_num][0],var_temp);//显示值 
				var_temp = MENU_DATA_Table.Other_s.Heater_Tip;
				OLED_printf(0,1,MENU_MSG.ADS_str[Page_num][1],var_temp);//显示值 
				var_temp = MENU_DATA_Table.Other_s.Shock_Sensor;
				OLED_printf(0,2,MENU_MSG.ADS_str[Page_num][2],var_temp);//显示值 
				var_temp = MENU_DATA_Table.Other_s.Beep_Volume;
				OLED_printf(0,3,MENU_MSG.ADS_str[Page_num][3],var_temp);//显示值 
				switch (MENU_DATA_Table.Other_s.Work_Mode)
				{
					case 0 :
					{
						OLED_P14x16Str(64,0,"整场");  
					}break;
					case 1 :
					{
						OLED_P14x16Str(64,0,"整场");  
					}break;
					case 2 :
					{
						OLED_P14x16Str(64,0,"工厂");  
					}break;
				}
			}break;
			default:;
		}     
	}   
}


/* 参数值更改值*/
unsigned char set_var()
{ 
	unsigned char var_flag = 1;
	//微增量
	float div_num = 1.0f; //

  /**************************/
  while(var_flag)//检测 按键
  {
		if((ec11_state.Key+ec11_state.Coder) > 0)//如果编码器有动作
		{
			if(ec11_state.Key == 1)
			{
				Select_flag = 0;
				ec11_state.Key = 0;
				return(0);
			}
			else if(ec11_state.Key == 2)
			{
				Select_flag = 0;				
				ec11_state.Key = 0;				
				var_flag = 0; 
				Option_num = 80;
				return(0);
			}
			else if(ec11_state.Coder == 1)//顺时针
			{
				ec11_state.Coder = 0;
				switch(Page_num)//页号
				{
					case 0:
					{
						switch(Option_num)
						{
							case 0:
							{
								MENU_DATA_Table.Temp_s.Max_Temp += FAC_DATA_Table.Other_s.Temp_Step;
								if(MENU_DATA_Table.Temp_s.Max_Temp >= 700)
									MENU_DATA_Table.Temp_s.Max_Temp = 700;
							}break;
							case 1:
							{
								MENU_DATA_Table.Temp_s.Slp_Temp += FAC_DATA_Table.Other_s.Temp_Step;
								if(MENU_DATA_Table.Temp_s.Slp_Temp >= MENU_DATA_Table.Temp_s.Max_Temp)
									MENU_DATA_Table.Temp_s.Slp_Temp = MENU_DATA_Table.Temp_s.Max_Temp;
							}break;
							case 2:
							{
								MENU_DATA_Table.Temp_s.Default_Temp += FAC_DATA_Table.Other_s.Temp_Step;
								if(MENU_DATA_Table.Temp_s.Default_Temp >= MENU_DATA_Table.Temp_s.Max_Temp)
									MENU_DATA_Table.Temp_s.Default_Temp = MENU_DATA_Table.Temp_s.Max_Temp;
							}break;
							case 3:
							{
								MENU_DATA_Table.Temp_s.Default_CH += div_num;
								if(MENU_DATA_Table.Temp_s.Default_CH >= 5)
									MENU_DATA_Table.Temp_s.Default_CH = 5;
							}break;
						default:break;
						}
					}break;
					case 1: 
					{
						switch(Option_num)
						{
							case 0:
							{
								MENU_DATA_Table.Time_s.Slp_Time += div_num;
							} break;
							case 1:
							{
								MENU_DATA_Table.Time_s.ScrSaver_Time += div_num;
							}break;
							case 2:
							{
								MENU_DATA_Table.Time_s.Shutdown_Time += div_num;
							}break;
						default:break;
						}
					}break;
					case 2:
					{
						switch(Option_num)
						{
							case 0:
							{
								MENU_DATA_Table.Other_s.Work_Mode += 2;
								if(MENU_DATA_Table.Other_s.Work_Mode>2)
								{
									MENU_DATA_Table.Other_s.Work_Mode = 0;
								}
							}break;
							case 1:
							{
								MENU_DATA_Table.Other_s.Heater_Tip += div_num;
								if(MENU_DATA_Table.Other_s.Heater_Tip>8)
								{
									MENU_DATA_Table.Other_s.Heater_Tip = 1;
								}
							}break;
							case 2:
							{
								MENU_DATA_Table.Other_s.Shock_Sensor += div_num;
								if(MENU_DATA_Table.Other_s.Shock_Sensor>10)
								{
									MENU_DATA_Table.Other_s.Shock_Sensor = 0;
								}
							}break;
							case 3:
							{
								MENU_DATA_Table.Other_s.Beep_Volume += div_num;
								if(MENU_DATA_Table.Other_s.Beep_Volume>10)
								{
									MENU_DATA_Table.Other_s.Beep_Volume = 0;
									BEEP_OFF;
								}
							}break;
						default :break;
						}
					}break;
				default :break;
				}//页号检查    
			} //顺时针
			else if(ec11_state.Coder == 2)//逆时针
			{
				ec11_state.Coder = 0;
				switch(Page_num)//页号
				{
					case 0:
					{
						switch(Option_num)
						{
							case 0:
							{
								MENU_DATA_Table.Temp_s.Max_Temp -= FAC_DATA_Table.Other_s.Temp_Step;
								if(MENU_DATA_Table.Temp_s.Max_Temp > 1000)
									MENU_DATA_Table.Temp_s.Max_Temp = 0;
							}break;
							case 1:
							{
								MENU_DATA_Table.Temp_s.Slp_Temp -= FAC_DATA_Table.Other_s.Temp_Step;
								if(MENU_DATA_Table.Temp_s.Slp_Temp > 1000)
									MENU_DATA_Table.Temp_s.Slp_Temp = 0;
							}break;
							case 2:
							{
								MENU_DATA_Table.Temp_s.Default_Temp -= FAC_DATA_Table.Other_s.Temp_Step;
								if(MENU_DATA_Table.Temp_s.Default_Temp > 1000)
									MENU_DATA_Table.Temp_s.Default_Temp = 0;
							}break;
							case 3:
							{
								MENU_DATA_Table.Temp_s.Default_CH -= div_num;
								if(MENU_DATA_Table.Temp_s.Default_CH > 100)
									MENU_DATA_Table.Temp_s.Default_CH = 0;
							}break;
						default:break;
						}
					}break;
					case 1: 
					{
						switch(Option_num)
						{
							case 0:
							{
								MENU_DATA_Table.Time_s.Slp_Time -= div_num;
								if(MENU_DATA_Table.Time_s.Slp_Time > 100)
									MENU_DATA_Table.Time_s.Slp_Time = 0;
							} break;
							case 1:
							{
								MENU_DATA_Table.Time_s.ScrSaver_Time -= div_num;
								if(MENU_DATA_Table.Time_s.ScrSaver_Time > 100)
									MENU_DATA_Table.Time_s.ScrSaver_Time = 0;
							}break;
							case 2:
							{
								MENU_DATA_Table.Time_s.Shutdown_Time -= div_num;
								if(MENU_DATA_Table.Time_s.Shutdown_Time > 100)
									MENU_DATA_Table.Time_s.Shutdown_Time = 0;
							}break;
						default:break;
						}
					}break;
					case 2:
					{
						switch(Option_num)
						{
							case 0:
							{
								MENU_DATA_Table.Other_s.Work_Mode -= 2;
								if(MENU_DATA_Table.Other_s.Work_Mode>100)
								{
									MENU_DATA_Table.Other_s.Work_Mode = 2;
								}
							}break;
							case 1:
							{
								MENU_DATA_Table.Other_s.Heater_Tip -= div_num;
								if(MENU_DATA_Table.Other_s.Heater_Tip==0)
								{
									MENU_DATA_Table.Other_s.Heater_Tip = 8;
								}
							}break;
							case 2:
							{
								MENU_DATA_Table.Other_s.Shock_Sensor -= div_num;
								if(MENU_DATA_Table.Other_s.Shock_Sensor>100)
								{
									MENU_DATA_Table.Other_s.Shock_Sensor = 10;
								}
							}break;
							case 3:
							{
								MENU_DATA_Table.Other_s.Beep_Volume -= div_num;
								if(MENU_DATA_Table.Other_s.Beep_Volume>100)
								{
									MENU_DATA_Table.Other_s.Beep_Volume = 10;
								}
							}break;
						default :;
						}
					}break;
				default :break;
				}//页号检查    
			} //逆时针
			else
			{
				return(0);
			}
			
			/* 显示函数 */
			Display_cursor();
			DisPlay(option_var);
			/* 显示函数 */
			
		}//如果编码器有动作
  /**************************/
		return(1);
	}
	return(0);
}

/* 选项查询 */
unsigned char option_check_data()
{
	unsigned char Option_flag = 1;  //Option_flag 选项标志
	unsigned char first_enter = 1;		//首次进入标志
	switch(Page_num)       //option_var  选项元素个数
	{
		case 0:		//TEMP 元素个数
			option_var = 3;
		break;
		case 1:		//TIME 元素个数
			option_var = 2;
		break;
		case 2: 	//OTHER 元素个数 
			option_var = 3;
		break;
		default:break;
	}
	Option_num = 0;
	while(Option_flag)//进入循环等待 按键信息
	{
		if(first_enter)//第一次进入时直接显示界面，以后如果有操作才更新界面
		{
			Display_cursor();
			DisPlay(Option_num);
			first_enter = 0;
		}
		if((ec11_state.Key+ec11_state.Coder) > 0)//编码器有动作
		{
			if(ec11_state.Key == 1)
			{
				Select_flag = 1;
				Display_cursor();
				DisPlay(Option_num);
				ec11_state.Key = 0;
				while(set_var());
				while((ec11_state.Key+ec11_state.Coder) > 0)//按键确定退出
				{
					if(ec11_state.Key == 2)
					{
						return(0);
					}
				}	
			}
			else if(ec11_state.Key == 2)
			{
				ec11_state.Key = 0;
				Option_flag = 0;//选项标志清零 退出
				Option_num = 80;
				return(0);
			}
			else if(ec11_state.Coder == 1)
			{
				ec11_state.Coder = 0;
				++Option_num;
			}
			else if(ec11_state.Coder == 2)
			{
				ec11_state.Coder = 0;
				--Option_num;
			}
			else
			{
			}

			/* 循环选项号 */
			if(Option_num  > option_var)//选项循环
				Option_num = 0;//选项号循环
			if(Option_num < 0)//选项循环
				Option_num = option_var;
			 /* 选项号循环 */
					/* 显示函数 */
			Display_cursor();
			DisPlay(Option_num);
		}//编码器有动作
		
	}//进入循环等待 按键信息
  return(1);
}
	unsigned char Page_flag = 1;//页面标志
/* 页面查询 */
unsigned char Menu_check_data() 
{              

	unsigned char first_enter = 1;//首次进入
	
	while(Page_flag)//等待 扫描页面
	{
		if(first_enter)
		{
			Display_cursor();
			DisPlay(3);//页面 
			first_enter = 0;
			ec11_state.Key = 0;
			ec11_state.Coder = 0;
		}
		if((ec11_state.Key+ec11_state.Coder) > 0)//是否接收值 Page
		{
			/* 确定页面 */
			if(ec11_state.Key == 1)
			{    /*刷新*/
				ec11_state.Key = 0;
				while(option_check_data());//进入选项查询
			}                         
			/* 退出页面 */   
			else if(ec11_state.Key == 2)
			{
				ec11_state.Key = 0;
				Page_flag = 0;
				return(0);	
			} 
			/* 页号加减 */
			else if(ec11_state.Coder == 1)
			{
				ec11_state.Coder = 0;
				++Page_num;
			}
			else if(ec11_state.Coder == 2)
			{
				ec11_state.Coder = 0;
				--Page_num;
			}
			else
			{}
				
			if(Page_num > 2) //页面号循环 
				Page_num = 0;
			if(Page_num < 0)
				Page_num = 2; 

				Display_cursor();
				DisPlay(3);//页面 			
		}//if() 是否接收值 Page
	}//while()
	return(1);
}

unsigned int ParaSet()
{
	unsigned char KEY_flag = 1;
	//如果不是在休眠状态下的按键
	if(ec11_state.Key == 1)
	{
		ec11_state.Key = 0;
		if(MENU_DATA_Table.Other_s.Work_Mode == 1)
			MENU_DATA_Table.Other_s.Work_Mode = 0;
		else if(MENU_DATA_Table.Other_s.Work_Mode == 0)
			MENU_DATA_Table.Other_s.Work_Mode = 1;
		//存储
		EEPROM_SectorErase(0x0);
		EEPROM_write_n(0x0,(unsigned char *)&MENU_DATA_Table,sizeof(MENU_DATA_Table));
	}
	if(ec11_state.Key == 2)
	{
		ShutdownCounterFlag = 1;
		OLED_CLS();//清屏
		Page_flag = 1;
		while(KEY_flag)
		{
			if(Menu_check_data());
			while(Page_flag == 1);
			OLED_CLS();//清屏
			//存储
			EEPROM_SectorErase(0x0);
			EEPROM_write_n(0x0,(unsigned char *)&MENU_DATA_Table,sizeof(MENU_DATA_Table));
			
			KEY_flag = 0;//退出循环
		}
		if(MENU_DATA_Table.Other_s.Work_Mode == 2)
		{
			//软关机
			IAP_CONTR = 0x20;
		}
	}
	ShutdownCounterFlag = 0;
	return 0;
}


//以下为工厂模式菜单
/* 显示 */
void DisPlay_Fac(unsigned char num)
{
	unsigned char j;
	float var_temp;
	
	//显示一级菜单
	if(Option_num == 80)
	{
		OLED_P14x16Str(32,0,"工厂模式");     
		for(j = 0;j < num;j++)//换行
		{
			OLED_printf(0,j+1,FAC_DATA_Table.FAC_MSG_s.OPTINO_NAME[j],0);//显示值
		}
	}
	//显示二级菜单
	else
	{

		switch(Page_num)
		{
			case 0:
			{              
				var_temp = FAC_DATA_Table.Adjust_s.Iron;
				OLED_printf(0,0,FAC_DATA_Table.FAC_MSG_s.ADS_str[Page_num][0],var_temp);//显示值
				var_temp = FAC_DATA_Table.Adjust_s.Voltage;
				OLED_printf(0,1,FAC_DATA_Table.FAC_MSG_s.ADS_str[Page_num][1],var_temp);//显示值
				var_temp = FAC_DATA_Table.Adjust_s.Encoder;
				OLED_printf(0,2,FAC_DATA_Table.FAC_MSG_s.ADS_str[Page_num][2],var_temp);//显示值  				
			}break;
			case 1:
			{              
				var_temp = FAC_DATA_Table.Channel_s.Ch1;
				OLED_printf(0,0,FAC_DATA_Table.FAC_MSG_s.ADS_str[Page_num][0],var_temp);//显示值
				var_temp = FAC_DATA_Table.Channel_s.Ch2;
				OLED_printf(0,1,FAC_DATA_Table.FAC_MSG_s.ADS_str[Page_num][1],var_temp);//显示值
				var_temp = FAC_DATA_Table.Channel_s.Ch3;
				OLED_printf(0,2,FAC_DATA_Table.FAC_MSG_s.ADS_str[Page_num][2],var_temp);//显示值    
				var_temp = FAC_DATA_Table.Channel_s.Ch4;
				OLED_printf(0,3,FAC_DATA_Table.FAC_MSG_s.ADS_str[Page_num][3],var_temp);//显示值  				
			}break;
			case 2:
			{              
				var_temp = FAC_DATA_Table.Other_s.Temp_Step;
				OLED_printf(0,0,FAC_DATA_Table.FAC_MSG_s.ADS_str[Page_num][0],var_temp);//显示值 
				var_temp = FAC_DATA_Table.Other_s.Lim_Voltage;
				OLED_printf(0,1,FAC_DATA_Table.FAC_MSG_s.ADS_str[Page_num][1],var_temp);//显示值 					
				var_temp = FAC_DATA_Table.Other_s.Recovery;
				OLED_printf(0,2,FAC_DATA_Table.FAC_MSG_s.ADS_str[Page_num][2],var_temp);//显示值 					
			}break;
			default:break;
		}     
	}   
}


/* 参数值更改值*/
unsigned char set_var_fac()
{ 
	unsigned char var_flag = 1;
	//微增量
	float div_num = 1.0f; //

  /**************************/
  while(var_flag)//检测 按键
  {
		if((ec11_state.Key+ec11_state.Coder) > 0)//如果编码器有动作
		{
			if(ec11_state.Key == 1)
			{
				Select_flag = 0;
				ec11_state.Key = 0;
				return(0);
			}
			else if(ec11_state.Key == 2)
			{  
				Select_flag = 0;
				ec11_state.Key = 0;				
				var_flag = 0; 
				Option_num = 80;
				return(0);
			}
			else if(ec11_state.Coder == 1)//顺时针
			{
				ec11_state.Coder = 0;
				switch(Page_num)//页号
				{
					case 0:
					{
						switch(Option_num)
						{
							case 0:
							{
								FAC_DATA_Table.Adjust_s.Iron += div_num;
								if(FAC_DATA_Table.Adjust_s.Iron >= 8)
									FAC_DATA_Table.Adjust_s.Iron = 8;
							}break;
							case 1:
							{
								FAC_DATA_Table.Adjust_s.Voltage += div_num;
							}break;
							case 2:
							{
								FAC_DATA_Table.Adjust_s.Encoder += div_num;
								if(FAC_DATA_Table.Adjust_s.Encoder >= 1)
									FAC_DATA_Table.Adjust_s.Encoder = 1;
							}break;
						default:break;
						}
					}break;
					case 1:
					{
						switch(Option_num)
						{
							case 0:
							{
								FAC_DATA_Table.Channel_s.Ch1 += FAC_DATA_Table.Other_s.Temp_Step;
								if(FAC_DATA_Table.Channel_s.Ch1 >= MENU_DATA_Table.Temp_s.Max_Temp)
									FAC_DATA_Table.Channel_s.Ch1 = MENU_DATA_Table.Temp_s.Max_Temp;
							}break;
							case 1:
							{
								FAC_DATA_Table.Channel_s.Ch2 += FAC_DATA_Table.Other_s.Temp_Step;
								if(FAC_DATA_Table.Channel_s.Ch2 >= MENU_DATA_Table.Temp_s.Max_Temp)
									FAC_DATA_Table.Channel_s.Ch2 = MENU_DATA_Table.Temp_s.Max_Temp;
							}break;
							case 2:
							{
								FAC_DATA_Table.Channel_s.Ch3 += FAC_DATA_Table.Other_s.Temp_Step;
								if(FAC_DATA_Table.Channel_s.Ch3 >= MENU_DATA_Table.Temp_s.Max_Temp)
									FAC_DATA_Table.Channel_s.Ch3 = MENU_DATA_Table.Temp_s.Max_Temp;
							}break;
							case 3:
							{
								FAC_DATA_Table.Channel_s.Ch4 += FAC_DATA_Table.Other_s.Temp_Step;
								if(FAC_DATA_Table.Channel_s.Ch4 >= MENU_DATA_Table.Temp_s.Max_Temp)
									FAC_DATA_Table.Channel_s.Ch4 = MENU_DATA_Table.Temp_s.Max_Temp;
							}break;
						default:break;
						}
					}break;
					case 2:
					{
						switch(Option_num)
						{
							case 0:
							{
								FAC_DATA_Table.Other_s.Temp_Step += div_num;
								if(FAC_DATA_Table.Other_s.Temp_Step > 50)
									FAC_DATA_Table.Other_s.Temp_Step = 50;
							}break;
							case 1:
							{
								FAC_DATA_Table.Other_s.Lim_Voltage += div_num;
								if(FAC_DATA_Table.Other_s.Lim_Voltage > 30)
									FAC_DATA_Table.Other_s.Lim_Voltage = 30;
							}break;
							case 2:
							{
								FAC_DATA_Table.Other_s.Recovery += div_num;
								if(FAC_DATA_Table.Other_s.Recovery >= 1)
									FAC_DATA_Table.Other_s.Recovery = 1;
							}break;
						default:break;
						}
					}break;
				default :break;
				}//页号检查    
			} //顺时针
			else if(ec11_state.Coder == 2)//逆时针
			{
				ec11_state.Coder = 0;
				switch(Page_num)//页号
				{
					case 0:
					{
						switch(Option_num)
						{
							case 0:
							{
								FAC_DATA_Table.Adjust_s.Iron -= div_num;
								if(FAC_DATA_Table.Adjust_s.Iron > 100)
									FAC_DATA_Table.Adjust_s.Iron = 0;
							}break;
							case 1:
							{
								FAC_DATA_Table.Adjust_s.Voltage -= div_num;
								if(FAC_DATA_Table.Adjust_s.Voltage >= 1000)
									FAC_DATA_Table.Adjust_s.Voltage = 0;
							}break;
							case 2:
							{
								FAC_DATA_Table.Adjust_s.Encoder -= div_num;
								if(FAC_DATA_Table.Adjust_s.Encoder >= 100)
									FAC_DATA_Table.Adjust_s.Encoder = 0;
							}break;
						default:break;
						}
					}break;
					case 1:
					{
						switch(Option_num)
						{
							case 0:
							{
								FAC_DATA_Table.Channel_s.Ch1 -= FAC_DATA_Table.Other_s.Temp_Step;
								if(FAC_DATA_Table.Channel_s.Ch1 >= 1000)
									FAC_DATA_Table.Channel_s.Ch1 = 0;
							}break;
							case 1:
							{
								FAC_DATA_Table.Channel_s.Ch2 -= FAC_DATA_Table.Other_s.Temp_Step;
								if(FAC_DATA_Table.Channel_s.Ch2 >= 1000)
									FAC_DATA_Table.Channel_s.Ch2 = 0;
							}break;
							case 2:
							{
								FAC_DATA_Table.Channel_s.Ch3 -= FAC_DATA_Table.Other_s.Temp_Step;
								if(FAC_DATA_Table.Channel_s.Ch3 >= 1000)
									FAC_DATA_Table.Channel_s.Ch3 = 0;
							}break;
							case 3:
							{
								FAC_DATA_Table.Channel_s.Ch4 -= FAC_DATA_Table.Other_s.Temp_Step;
								if(FAC_DATA_Table.Channel_s.Ch4 >= 1000)
									FAC_DATA_Table.Channel_s.Ch4 = 0;
							}break;
						default:break;
						}
					}break;
					case 2:
					{
						switch(Option_num)
						{
							case 0:
							{
								FAC_DATA_Table.Other_s.Temp_Step -= div_num;
								if(FAC_DATA_Table.Other_s.Temp_Step == 0)
									FAC_DATA_Table.Other_s.Temp_Step = 1;
							}break;
							case 1:
							{
								FAC_DATA_Table.Other_s.Lim_Voltage -= div_num;
								if(FAC_DATA_Table.Other_s.Lim_Voltage >= 100)
									FAC_DATA_Table.Other_s.Lim_Voltage = 0;
							}break;
							case 2:
							{
								FAC_DATA_Table.Other_s.Recovery -= div_num;
								if(FAC_DATA_Table.Other_s.Recovery >= 100)
									FAC_DATA_Table.Other_s.Recovery = 0;
							}break;
						default:break;
						}
					}break;
				default :break;
				}//页号检查    
			} //逆时针
			else
			{
				return(0);
			}
			
			/* 显示函数 */
			Display_cursor();
			DisPlay_Fac(option_var);
			/* 显示函数 */
			
		}//如果编码器有动作
  /**************************/
		return(1);
	}
	return(0);
}

/* 选项查询 */
unsigned char option_check_data_fac()
{
	unsigned char Option_flag = 1;  //Option_flag 选项标志
	unsigned char first_enter = 1;		//首次进入标志
	switch(Page_num)       //option_var  选项元素个数
	{
		case 0:		//调整 元素个数
			option_var = 2;
		break;
		case 1:		//通道 元素个数
			option_var = 3;
		break;
		case 2:		//通道 元素个数
			option_var = 2;
		break;
		default:break;
	}
	Option_num = 0;
	while(Option_flag)//进入循环等待 按键信息
	{
		if(first_enter)//第一次进入时直接显示界面，以后如果有操作才更新界面
		{
			Display_cursor();
			DisPlay_Fac(Option_num);
			first_enter = 0;
		}
		if((ec11_state.Key+ec11_state.Coder) > 0)//编码器有动作
		{
			if(ec11_state.Key == 1)
			{
				ec11_state.Key = 0;
				Select_flag = 1;
				Display_cursor();
				DisPlay_Fac(option_var);
				while(set_var_fac());
				while((ec11_state.Key+ec11_state.Coder) > 0)//按键确定退出
				{
					if(ec11_state.Key == 2)
					{
						return(0);
					}
				}	
			}
			else if(ec11_state.Key == 2)
			{
				ec11_state.Key = 0;
				Option_flag = 0;//选项标志清零 退出
				Option_num = 80;
				return(0);
			}
			else if(ec11_state.Coder == 1)
			{
				ec11_state.Coder = 0;
				++Option_num;
			}
			else if(ec11_state.Coder == 2)
			{
				ec11_state.Coder = 0;
				--Option_num;
			}
			else
			{
			}

			/* 循环选项号 */
			if(Option_num  > option_var)//选项循环
				Option_num = 0;//选项号循环
			if(Option_num < 0)//选项循环
				Option_num = option_var;
			 /* 选项号循环 */
					/* 显示函数 */
			Display_cursor();
			DisPlay_Fac(Option_num);
		}//编码器有动作
		
	}//进入循环等待 按键信息
  return(1);
}
/* 页面查询 */
unsigned char Fac_check_data() 
{              

	unsigned char first_enter = 1;//首次进入
	
	while(Page_flag)//等待 扫描页面
	{
		if(first_enter)
		{
			Display_cursor();
			DisPlay_Fac(3);//页面 
			first_enter = 0;
			ec11_state.Key = 0;
			ec11_state.Coder = 0;
		}
		if((ec11_state.Key+ec11_state.Coder) > 0)//是否接收值 Page
		{
			/* 确定页面 */
			if(ec11_state.Key == 1)
			{    /*刷新*/
				ec11_state.Key = 0;
				while(option_check_data_fac());//进入选项查询
			}                         
			/* 退出页面 */   
			else if(ec11_state.Key == 2)
			{
				ec11_state.Key = 0;
				Page_flag = 0;
				return(0);	
			} 
			/* 页号加减 */
			else if(ec11_state.Coder == 1)
			{
				ec11_state.Coder = 0;
				++Page_num;
			}
			else if(ec11_state.Coder == 2)
			{
				ec11_state.Coder = 0;
				--Page_num;
			}
			else
			{}
				
			if(Page_num > 2) //页面号循环 
				Page_num = 0;
			if(Page_num < 0)
				Page_num = 2; 

				Display_cursor();
				DisPlay_Fac(3);//页面 			
		}//if() 是否接收值 Page
	}//while()
	return(1);
}

unsigned int ParaSet_Fac()
{
	unsigned char KEY_flag = 1;
	
	OLED_CLS();//清屏
	Page_flag = 1;
	while(KEY_flag)
	{
		if(Fac_check_data());
		while(Page_flag == 1);
		OLED_CLS();//清屏
		
		//存储
		EEPROM_SectorErase(0x200);
		EEPROM_write_n(0x200,(unsigned char *)&FAC_DATA_Table,sizeof(FAC_DATA_Table));	
		
		KEY_flag = 0;//退出循环
	}

	return 0;
}