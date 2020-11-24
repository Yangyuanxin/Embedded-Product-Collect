//V1.1 更新电量显示级别,增加按键抗干扰
//V1.2 改善手机信号干扰
//V1.3 增加边充边放功能
#include "define.h"

//--IO初始化-------------------------------------
void IO_Setting(void)
{		
  P0M = 0x0E;	//P0 Input-Output value   //P01，02，03输出模式。P00,04输入模式。
  P0UR = 0x01;	//P0 pull up value        //打开P00按键弱上拉
  P0 = P0&0X01; //P0 IO register-SET  ?

  P4M = 0x1A;	//P4 Input-Output value   //P41,43为输出模式，P4.2,P4.4,4.0为输入模式。
  P4UR = 0x04;  //P4 pull up value        //关闭所有若上拉。
  P4CON = 0x01;	//P4 Analog-Digital value //P4.0只做模拟输入
  P4 = 0X00;	//P4 register value       //P4口赋值0
                                                                                                                                                                                                                   
  P5M = 0x18;	//P5 Input-Output value   //P5.3,P5.4为输出模式.
  P5UR = 0x00;	//P5 pull up value        //关闭所有弱上拉  
  P5 = 0X00;	//P5 register value       //P5口赋值0
}

//--用户寄存器初始化-------------------------------
void UserInit(void)
{
	if (FirstPowerOn)
	{
		VoltageOffset=0;
		BatteryLevel=4;	
		BatteryEmptyFlag=0;
	}
	BatteryFullFlag=0;	
	KeyPressFlag=0;
	KeyTime=0;	
	BatteryCapUpdateTimeFlag=0;
	Counter1S=0;
	SleepWaitTime=0;
	KeyOutputSW=0;
	BatteryVchargeFlag=0;
	LEDTime=0;
	CurrentDetTime=0;	
	CurrentSmallTime=0;	
	CurrentMaxTime=0;
	KeyReleaseFlag=1;
	BatteryLowerCounter=0;
	BatteryFulltemp=0;
}

//--休眠处理函数-------------------------------
void EnterSleepMode(void)
{
	if (!(USBDET||OUTPUTSW))	
	{
		if (SleepWaitTime>=SleepWaitTimeSet)//if USB not insert and not 5V output
		{
			FGIE = 0;
			INTRQ=0;
			DCOUTOFF;
			LEDOFF;
			DisableADC;
			ADM=0;           //降低静态功耗
			DelayUs(100);
			SLEEPMODE_MY;
			DelayUs(10);
			ADC_Setting();	
			UserInit();	
	       
            OUTPUTSW=1;      //打开输出
            LED2ON;
			delayS(250);
			LED2OFF;
            LED3ON;
            delayS(250);
			LED3OFF;
			LED4ON;
            delayS(250);
			LED4OFF;
			LED5ON;
			delayS(250);
			LED5OFF;

			KeyOutputSW=1; //输出打开

			FGIE = 1;  //开总中断
		 }
	   }	
     }



//--电池电量更新程序-----------------------------
void UpdateBatteryCap(void)
{	
	uint8 batteryleveltemp;	  
	uint8 Offset;
	if (USBDET)     //充电时加入补偿
	{
		Offset=0xC7;//offset the battery level 4 to 4.1V  0.136V
	}
	else            //放电时没有补偿
	{
		Offset=0;
	}

	if (BatteryCapUpdateTimeFlag||FirstPowerOn)//1S update battery cap 1秒更新一次电池电量，或者第一次开机更新一次
	{	
		BatteryCapUpdateTimeFlag=0;                         //电量更新标志位清零

	  if (BatteryVoltage>=(BatteryVoltage4+Offset))        //如果电池电压>=3.8V  //充电大于3.936
		{
			batteryleveltemp=4;       //LED5亮
		//	BatteryFullFlag=1;        //满电标志位值1
		 }
		else
		{
		if (BatteryVoltage>=(BatteryVoltage3+Offset))        //如果电池电压>=3.8V  //充电大于3.936
		{
			batteryleveltemp=3;       //LED4亮

		}
		else
		{ 
			if (BatteryVoltage>=(BatteryVoltage2+Offset))    //如果电池电压>=3.6V  //充电大于3.736
			{
				batteryleveltemp=2;		 //LED3亮
			}
			else
			{
				if (BatteryVoltage>=(BatteryVoltage1+Offset))//如果电池电压>=3.45V  //充电大于3.586
				{
					batteryleveltemp=1;    //LED2亮 电池电压>=3.45V
				}
				else
				{
					if (BatteryVoltage>=BatteryVoltage0)      //如果电池电压>=2.9
					{
                       batteryleveltemp=0;   //LED1亮 电池电压>=2.9									
					}
                    else
					{
                     	BatteryEmptyFlag=1;	 //空电标志位值1																		 	  
					}
				  }
			    }
		      }
		    }

		if (USBDET)// if usb insert, battery level can up     如果在充电
		{
			if ((batteryleveltemp>=BatteryLevel)||FirstPowerOn) //如果电池电量大于显示电量||第一次开机，有一个为真
			{
				BatteryLevel=batteryleveltemp;	                //更新电量显示
			}
		}
		else	//if usb not insert, battery level can down     //否则没有在充电，正在放电
		{
			if (batteryleveltemp<BatteryLevel)                  //如果电池电量小于显示电量
			{
				BatteryLevel=batteryleveltemp;	                //更新显示电量
			}
		  }
	    }
      }

//--ADC初始化-------------------------------------
void ADC_Setting(void)
{
  FADENB = 1;
  DelayUs(200); //User support DELAY function
//  ADR = 0x50;	//ADC clock: 2.000000MHZ,ADC resolution: -1
  ADR = ADR&0x0f;     //设定ADC频率，FCPU/16分频，250000HZ。/1ms  ?
  VREFH = 0x03;	//Internal VREFH: VDD
  FEVHENB = 0;	//Internal VREFH disable
}

//--充电处理-------------------------------------
void Charge(void)
{
	if (USBDET)
	{
//	    KeyOutputSW=0;
		if (BatteryVoltage>=BatteryFullVoltage) //如果电池电压>=4.16V
		{
		   if(BatteryFullDelayFlag==1)
		   {
		    BatteryFullDelayFlag=0;     //1S到清零，
            BatteryFulltemp++;          //电池满电判断寄存器
		   if(BatteryFulltemp>=200)      //连续200S保持满电以上。
		   {
            BatteryFulltemp=0;
			BatteryFullFlag=1;          //电池满电标志位
			}
		  }
		}
		else
		{
		    BatteryFulltemp=0;     //清零满电延时判断
			BatteryFullFlag=0;	   //否则满电标志位，为0.
		}
		if (!OUTPUTSW)             //如果输出是关闭状态
		{
			BatteryEmptyFlag=0;	   //电池空电标志位清零。		
		}
//		BatteryWarningFlag=0;      //只要检测到充电，电池警告标志位就清零。
	    }

}

//--放电处理函数----------------------------------------------
void Discharge(void)
{	
	if (BatteryEmptyFlag)    //如果空电标志位为真
	{
		KeyOutputSW=OFF;     //输出关闭
	}
	if (KeyOutputSW)         //开关为真
	{
		BatteryFullFlag=0;	 //电池满电标志位清零	
		DCOUTON;             //升压使能开
		DischargeCurrent=ADFilter(ChannelI);  //采集放电电流数据赋给DischargeCurrent 变量
		if (DischargeCurrent>=DischargeCurrentPro)//if current larger protect current//如果放电电流大于过流点。
		{
			CurrentMaxTime++;  //过流计时器自增。
			if (CurrentMaxTime==CurrentMaxTimeSet) //如果过流时间==设定的最大过流时间
			{
				KeyOutputSW=0;     //关闭输出
				CurrentMaxTime=0;  //过流计时器清零
			}
		}
		else  //否则放电电流没有过流
		{
			CurrentMaxTime=0; //过流计时器清零
		}

		if (CurrentDetTime)	  //1S detect 1 cycle //1S时间检测一次负载
		{
			CurrentDetTime=0; //清零负载检测标志位
			if (DischargeCurrent<DischargeCurrentMin)//if discharge current is low 2.8/4096=0.00068359375×5=0.00341796875/0.05=0.068A
			{
				CurrentSmallTime++;  //无负载计时器++
				dischargeFlag=0;     //无负载时标志位为0
				if (CurrentSmallTime>=CurrentSmallTimeSet)  //如果计时器大于设定时间
				{
					KeyOutputSW=0;      //关闭输出
					CurrentSmallTime=0; //清零计时器
					
				}
			}
			else    //否则放电电流正常
			{
				CurrentSmallTime=0;	 //清零计时器
			}
		}
      if(DischargeCurrent>5) //负载检测  68ma
	   {
	    dischargeFlag=1;  //有负载时标志位为1
					   
	    }
     
	 }
	else
	{
		DCOUTOFF;
		KeyOutputSW=0;
		CurrentMaxTime=0;
		CurrentSmallTime=0;
	}
}

//--更新电池电压处理---------------------------------
void UpdateBatteryVoltage(void)   //16次平均滤波处理
{
	uint8 loop=0;
	uint16 temp,result=0;
	for (loop=0;loop<16;loop++)
	{
	  temp=ADFilter(ChannelVbat);
	  result+=temp;
	   }
	result=result>>4;	
    BatteryVoltage=result;   //电池电压
}


//--按键处理-----------------------------------------
void KeyScan(void)
{
	if (!KEY)           //如果检测到按键被按下
	{	
	  DelayUs(20);	    //延时	
		if (!KEY)       //确认按键被按下
		{
			 KeyPressFlag=1;      //按键标志位置1
             SleepWaitTime=0;
			if (DoubleKeyFlag)   //双击处理程序
			{
				if ((DoubleKeyTime>=50)&&(DoubleKeyTime<100))  //双击判断
				{
					DoubleKeyFlag=0;
					DoubleKeyTime=0;
//					LED=~LED;         //双击打开手电筒
				}
			   }			
		     }		
	       }
	else       //如果没有检测到按键按下
	{
      	if (KeyPressFlag)
		{
		 	DoubleKeyFlag=1;   //双击按键标志位置1	
			if ((KeyTime>=ShortPressTime)&&(KeyTime<LongPressTime))
			{	
				KeyOutputSW=~KeyOutputSW;							
			}
		   }
		if (DoubleKeyTime>=100)  
		{
			DoubleKeyFlag=0;
			DoubleKeyTime=0;	
		}
		KeyPressFlag=0;
		KeyTime=0;
	    KeyPressFlag=0;	
		KeyReleaseFlag=1;	
	  }
    }

//--AD采集处理----------------------------
uint16 CheckAD(uint8 AD_Channel)
{
	uint16 temp=0;
	uint8 loop;
	ADM=0x80+AD_Channel;
	FGCHS=1;			
	FADS=1;//AD start
	loop=250;
	while((!FEOC)&&loop)
	{
		loop--;
	}
	temp=ADB;
	temp=temp<<4;
	temp=temp|(ADR&0x0F);
	return temp;	
}
//--AD数字滤波处理-------------------------
uint16 ADFilter(uint8 AD_Channel)
{
	uint8 loop;
	uint16 temp,max=0,min=0xFFF,result=0;
	FGIE=OFF;//disable interrupt
	for (loop=0;loop<18;loop++)
	{
		temp=CheckAD(AD_Channel);	
		if (temp>max)
		{
			max=temp;
		}
		if (temp<min)
		{
			min=temp;
		}
		result+=temp;
	}
	FGIE=ON;//Enable interrupt
	result=(result-max-min)>>4;	
	return result;
  }

//--定时器初始化--------------------------------
void Timer_Setting(void)
{
   TC0M=0x74;      //timer 0 SET OSC  16.000000hz
   FTC0X8=1;       //interior osc frequency/1
   TC0C=0X00;      //starter value
   TC0R=0X00;
   FTC0ENB=1;      //enabled T0 timer
   
   TC1M=0X04;      //timer 1 SET Fcpu 
   FTC1X8=0;       //interior Fcpu frequency/256
   TC1C = 0xB2;	   //TC1C register value, time: 5.000000ms(200.000000HZ)
   TC1R = 0xB2;	   //TC1R register value
   FTC1IRQ = 0;
   FTC1IEN = 1;	   //TC1 Interrupt enable
   FTC1ENB = 1;	   //TC1 enable
   FGIE=1;         //Enable Whole Interrupt 
}

//--延时函数US-----------------------------
void DelayUs(uint8 time)
{
	while (time--);
}

void delayS(uint8 temp)
 {
     uint8 aa;
	for(aa=temp;aa>0;aa--)
	  {
	     DelayUs(255);
		 _ClearWatchDogTimer(); 	
	     DelayUs(255);
		  _ClearWatchDogTimer(); 
		 DelayUs(255);	   
	   }    
    }
		 

//--中断处理函数---------------------------
__interrupt ISR_TC1(void)
{
  if(FTC1IRQ)//4ms              //定时器1中断，4MS中断一次
  {
  	INTRQ=0;                    //软件清除所有中断标志位
	Counter1S++;                //1S计时器++
	if (Counter1S==250)         //如果1S计时器加到250×4MS=1000MS=1S时间到
	{
    Counter1S=0;                //清零1S计时器
	BatteryFullDelayFlag=1;     //电池满点延时标志位
	BatteryCapUpdateTimeFlag=1; //电池电量更新时间标志位值1，更新电池电量,1S更新一次。
	CurrentDetTime=1;           //Current detect flag bit  输出电流检测时间标志位值1，		
	if (SleepWaitTime<0xFF)     //休眠计时器
	{
    SleepWaitTime++;	        //休眠计时器自增
	  }
      
	}

	if (KeyPressFlag)//key time det//按键长按判断
	{
		KeyTime++;                 //按键长按计时器自增  
	}

    if (DoubleKeyFlag)    //双击计数器自增
	{
		DoubleKeyTime++;
	}


//--LED1处理-----------------------------------------------
	switch(LEDTime++)     //LED循环显示程序。
	  {
	  	case 0:
		{
		    if(BatteryLevel==0)
			{
			LED1OFF;//改善亮度不一
			if (USBDET||dischargeFlag) //如果正在充电或者放电
			{
				if (BatteryLevel==0)   //如果电量==0 /电池电压>=2.9 /判断为第一个LED亮。 
				{
					if (Counter1S<125) //如果1S延时小于0.5S
					{
						LED1ON;        //LED1亮
					}		
					else               //否则
					{
						LED1OFF;        //LED关闭 ，闪烁用
					}
				}
				else                   //如果电池电量不=0 判断为电压不在2.9V-3.45V之间 
				{
					LED1ON;            //LED1长亮
				}	
			}
			else                  //如果没有在充电或放电
			{       
			  LED1ON;             //LED1常亮
			  }
			  }
			  else
			  {
			   LED1OFF;           //LED关闭 ，闪烁用
			  }
			break;                //返回
		}

//--LED2处理---------------------------------------------
		case 1:
		{
			LED2OFF;
			if (BatteryLevel>0)            //如果电池电量>0  如果电池电压>=3.45V
			{
				if (USBDET) //如果正在充电，或者在放电
				{
					if (BatteryLevel==1)   //如果电池电量==1  如果电池电压>=3.45V
					{
						if (Counter1S<125) //如果Counter1S小于0.5S
						{
							LED2ON;        //LED2亮
						}		
						else               //否则大于0.5S
						{
							LED2OFF;        //LED不亮，闪烁用
						}
					}
					else        //否则不==1
					{
						LED2ON; //LED2常亮
					}	
				}
			   else   //否则没有在充电，或放电
				{
					LED2ON;  //LED2开
				}	
			}
			break;   //返回
		}

//--LED3处理----------------------------------------
		case 2:
		{
			LED3OFF;
			if (BatteryLevel>1)        //如果电池电量>1  如果电池电压>3.6V
			{
				if (USBDET)  //如果正在充电或放电
				{
					if (BatteryLevel==2)    //如果电量等于2 如果电池电压>=3.6V
					{
						if (Counter1S<125)  //如果Counter1S小于0.5S
						{
							LED3ON;   //LED3开
						}		
						else          //否则
						{
							LED3OFF;   //LED3关，闪烁用
						}
					}
					else     //否则电量不等于2
					{
						LED3ON;  //LED3开
					}	
				}
				else   //否则没有在充电，或放电
				{
					LED3ON;  //LED3开
				}				
			}
			break;   //返回
		}

//--LED4处理----------------------------------------
		case 3:
		{
			LED4OFF;
			if (BatteryLevel>2)        //如果电池电量>1  如果电池电压>3.6V
			{
				if (USBDET)  //如果正在充电或放电
				{
					if (BatteryLevel==3)    //如果电量等于2 如果电池电压>=3.6V
					{
						if (Counter1S<125)  //如果Counter1S小于0.5S
						{
							LED4ON;   //LED3开
						}		
						else          //否则
						{
							LED4OFF;   //LED3关，闪烁用
						}
					}
					else     //否则电量不等于2
					{
						LED4ON;  //LED3开
					}	
				}
				else   //否则没有在充电，或放电
				{
					LED4ON;  //LED3开
				}				
			}
			break;   //返回
		}

//--LED5处理------------------------------------------
		case 4:
		{
			LED5OFF;
			if (BatteryLevel>3)              //如果电量大于2  如果电池电压>3.6V
			{
				if (USBDET)   //如果正在充或者放电
				{
                  if (!BatteryFullFlag)      //如果满电标志位为零
					{
						if (Counter1S<125)   //如果Counter1S小于0.5S
						{
							LED5ON;         //LED4开
						}		
						else
						{
							LED5OFF;        //LED4关。闪烁用
						}
					}
					else          //否则就是满电
					{
						LED5ON;   //LED4常亮
					}	
				}
				else   //否则就是没有在充电，或放电
				{
					LED5ON;   //LED4亮
				}				
			}
			LEDTime=0;   //LED扫描循环计时器清零	
			break;       //返回
		}
	  }			 
   } 
  INTRQ=0; 
}
//--------------------------------------------------------------
void main(void)
{
  NORMALMODE_MY;
  FirstPowerOn=1;
  IO_Setting();
  Timer_Setting();
  ADC_Setting();
  UserInit();
  FGIE =1;
   
  while(!CurrentDetTime)
  {_ClearWatchDogTimer();}//wait 1S for battery voltage
  while(1)
  {
  	_ClearWatchDogTimer();
  	EnterSleepMode();
  	KeyScan();
  	UpdateBatteryVoltage();
	Charge();
	Discharge();	
	UpdateBatteryCap();
	FirstPowerOn=0;
  }
}
