#include "get_data.h"
#include "usr_wifi.h"
/***************外部变量声明****************/
extern  Clock My_Clock;		//时钟信息
extern u8  read_flag;  	//心率读标志位
extern u8 H_Average_Buf[12];	
extern u8 T_Average_Buf[12];
extern SYSTEM_STA System_Sta;			//系统标志位
extern SYSTEM_DATA	System_Data;		//系统数据
extern SYSTEM_SET	System_Set;			//系统设置

//像服务器发数据
const u8  Data_Num[]={0x31,0x30,0x33,0x32,0x35,0x34,0x37,0x36,0x39,0x38};	//0~9
u8 Data_Ok[]={0x81,0x88,0x01,0x01,0x01,0x01,0x41,0x60,0x71,0x71,0x7A,0x4e,0x4a,0x7c,'\0'};	//ok
u8 Data_All[]={0x81,0xA4,0x01,0x01,0x01,0x01,0x41,0x60,0x71,0x71,0x72,0x64,0x6f,0x65,0x45,0x60,0x75,0x60,0x75,0x31,0x31,0x31,0x27,0x69,0x31,0x31,0x31,0x27,0x63,0x71,0x31,0x31,0x31,0x27,0x63,0x65,0x31,0x31,0x31,0x27,0x6f,0x74,'\0'};
const u8 Help_hg[]={0x69,0x66};//摔倒求救
const u8 Help_ht[]={0x69,0x75};//体温求救
const u8 Help_hh[]={0x69,0x69,};//心率求救
const u8 Help_ho[]={0x69,0x6e};//其他求救
const u8 Help_nu[]={0x6f,0x74};//无求救
u8 	Time_AdJust[]={0x81,0x8B,0x01,0x01,0x01,0x01,0x41,0x60,0x71,0x71,0x66,0x64,0x75,0x55,0x68,0x6c,0x64,'\0'};//gettime

//获取心率数据
//这里面也包括了求平均值
void Data_GetHeart(void)
{
	u8 i=0;
	u16 buf=0;
	System_Sta.Get_Heart |= (1<<15);	//数据开始测量标志位
	if((System_Sta.Get_Heart&0x7fff)<320)		//等待2s
	{
		read_flag &= ~(1<<0);		//清楚数据读取到选择位
	}
	else
	{
		i = Get_Haer_Rate();
		if((read_flag&(1<<1)) !=0 )		//测量完成
		{
			System_Data.heart = i;
			if(H_Average_Buf[10]==10)		//平均值计数位
			{
				H_Average_Buf[10] = 0;
				H_Average_Buf[11] = 1;
			}
			H_Average_Buf[H_Average_Buf[10]]=i;//存入平均值
			H_Average_Buf[10] = (H_Average_Buf[10])+1;//平均值加1
			//计算平均值
			if(H_Average_Buf[11] == 1)		//已经拿了10个数据
			{
				for(i=0;i<10;i++)
				{
					buf = buf+H_Average_Buf[i];
				}
				System_Data.heart_average = buf/10;		//计算平均心率

			}
			else		//还未拿到10个数据
			{
				for(i=0;i<H_Average_Buf[10];i++)
				{
					buf = buf+H_Average_Buf[i];
				}
				System_Data.heart_average = buf/H_Average_Buf[10];		//计算平均心率
			}	
			System_Sta.Get_Heart = 0;		//计数位清0
			System_Sta.Get_Heart |= (1<<14);	//清除测量成功标志位
		}
		else
		{
			if((System_Sta.Get_Heart&0x7fff)>600)	//30s内都都没有测量成功
			{								
				System_Sta.Get_Heart = 0;		//计数位清0
				System_Sta.Get_Heart |= (1<<13);		//测量失败
			}
		}
	}
}
//获取体温数据
//这里面也包括了求平均值
void Data_GetTemp(void)
{
	u16 buf=0;
	u8 i=0;
	System_Sta.Get_Temp |= (1<<15);	//数据开始测量标志位
	if((System_Sta.Get_Temp&0x7fff)<400)		//等待5s
	{
		System_Data.temperature=SMBus_ReadTemp();		//提取体温							
	}
	else
	{	
		//计算平均值
		if(T_Average_Buf[10]==10)		//平均值计数位溢出清0
		{
			T_Average_Buf[10] = 0;
			T_Average_Buf[11] = 1;			//数据已经取到10标志位
		}
		T_Average_Buf[T_Average_Buf[10]]=System_Data.temperature;//存入平均值
		T_Average_Buf[10] = (T_Average_Buf[10])+1;//平均值加1
		if(T_Average_Buf[11] == 1)		//已经拿了10个数据
		{
			for(i=0;i<10;i++)
			{
				buf = buf+T_Average_Buf[i];
			}
			System_Data.temp_average = (buf+0.1)/10;		//计算平均心率
		}
		else		//还未拿到10个数据
		{
			for(i=0;i<T_Average_Buf[10];i++)
			{
				buf = buf+T_Average_Buf[i];
			}
			System_Data.temp_average = (buf+0.1)/(float)(T_Average_Buf[10]);		//计算平均心率
		}						
		System_Sta.Get_Temp = 0;		//计数位清0
		System_Sta.Get_Temp |= (1<<14);	//清除测量成功标志位
	}	
}
//获取RTC时间数据
void Data_GetTime(void)
{
	RTC_Get_Time(&My_Clock.hour,&My_Clock.min,&My_Clock.sec,&My_Clock.week);
	RTC_Get_Date(&My_Clock.year,&My_Clock.month,&My_Clock.date,&My_Clock.week);
	System_Sta.Get_Time = 0;		//清0
}
//获取血压数据
u8 Data_GetBP(void)
{
	u8 i=0,j=0;
	u8 bp_buf[11];
	if(Wifi_LinkBP())
	{

		System_Sta.Measure &= ~(1<<7);	//血压测量标志位清除
		return 1;
	}					
	else
	{
		delay_ms(1000);
		if(Wifi_StartBp(bp_buf))
		{
			System_Sta.Measure &= ~(1<<7);	//血压测量标志位清除
			return 2;
		}
		else		//测量成功，数据开始处理
		{					
			for(i=0;i<7;i++)	//寻找信息中的逗号
			{
				if(bp_buf[i] == ',')
				{
					j = i;		//记录逗号的位置
					break;
				}	 
			}
			if(j == 0)		//如果没有逗号说明数据错误
			{
				GUI_DispStringAt("data err",60,120);
				GUI_Exec();
				System_Sta.Measure &= ~(1<<7);	//血压测量标志位清除
			}
			else			//逗号不0，开始提取数据
			{
				//收缩压三位
				System_Data.boolpressure_up = ((bp_buf[2]-'0')*100)+((bp_buf[3]-'0')*10)+((bp_buf[4]-'0'));
				//舒展压三位
				System_Data.boolpressure_down = ((bp_buf[6]-'0')*100)+((bp_buf[7]-'0')*10)+((bp_buf[8]-'0'));
				System_Sta.Measure &= ~(1<<7);	//血压测量标志位清除
				System_Sta.Measure |= (1<<6);	//置位成功标志位
			}
		}
	}
	return 0;
}

u16 Get_EatTime(u8 i,u8 j,u8 k,u8 l)
{
	u8 buf1=0;
	u8 buf2=0;
	u16 buf3 = 0;
	if(i!=0)
	{
		buf1 = ((i-'0')*10)+(j-'0');
	}
	else
	{
		buf1 = (j-'0');
	}
	if(k!=0)
	{
		buf2 = ((k-'0')*10)+(l-'0');
	}
	else
	{
		buf2 = (l-'0');
	}
	
	buf3 = ((buf1<<8)|(buf2));
	return  buf3;
 }
//处理判断来自服务器的命令
int Data_GetServer(void)
{
	u8 buf[35];
	u16 timebuf;
	if(Wifi_DataGet(buf)){return 1;}//提取数据
	if(buf[0] == 'S')	//设置
	{
		//体温设置
		if(buf[2] == 'O'){System_Set.warning_temp = 1;}		//体温提醒开
		else
		{
			if(buf[2] == 'C')
			{
				System_Set.warning_temp = 0;//体温提醒关
			}
			else{return 1;}
		}
		//摔倒设置
		if(buf[6] == 'O'){System_Set.warning_fall  = 1;}	//摔倒报警开
		else
		{
			if(buf[6] == 'C')
			{
				System_Set.warning_fall  = 0;	//摔倒报警关
			}
			else{return 1;}
		}
		//心率设置
		if(buf[10] == 'O')	{System_Set.warning_heart = 1;}//心率提醒开
		else
		{
			if(buf[10] == 'C')
			{
				System_Set.warning_heart  = 0;		//心率提醒关
			}
			else{return 1;}
		}
		//吃药三个时间段设置设置	
		//第一个时间点
		if(buf[14] != '6')			
		{
			System_Set.warning_eat |= (1<<7);	//第一个时间点开
		}
		else
		{
			if(buf[14] == '6')
			{
				System_Set.warning_eat &= ~(1<<7);	//第一个时间点关
			}
			else{return 1;}
		}
		//第二个时间点
		if(buf[20] != '6')			
		{
			System_Set.warning_eat |= (1<<6);	//第一个时间点开
		}
		else
		{
			if(buf[20] == '6')
			{
				System_Set.warning_eat &= ~(1<<6);	//第一个时间点开
			}
			else{return 1;}
		}
		//第三个时间点
		if(buf[26] != '6')			
		{
			System_Set.warning_eat |= (1<<5);	//第一个时间点开
		}
		else
		{
			if(buf[26] == '6')
			{
				System_Set.warning_eat &= ~(1<<5);	//第一个时间点开
			}
			else{return 1;}
		}
		//开始提取时间数据
		if((System_Set.warning_eat&(1<<7))!=0)		//第一个时间
		{
			timebuf = Get_EatTime(buf[14],buf[15],buf[17],buf[18]);
			System_Set.eat_time1 = (u8)(timebuf>>8);
			System_Set.eat_min1 = (u8)(timebuf&0x00ff);
		}
		if((System_Set.warning_eat&(1<<6))!=0)		//第二个时间
		{
			timebuf = Get_EatTime(buf[20],buf[21],buf[23],buf[24]);
			System_Set.eat_time2 = (u8)(timebuf>>8);
			System_Set.eat_min2 = (u8)(timebuf&0x00ff);
		}
		if((System_Set.warning_eat&(1<<5))!=0)		//第三个时间
		{
			timebuf = Get_EatTime(buf[26],buf[27],buf[29],buf[30]);
			System_Set.eat_time3 = (u8)(timebuf>>8);
			System_Set.eat_min3 = (u8)(timebuf&0x00ff);
		}
		Wifi_Send2Sever(Data_Ok);
		return 0;	
	}
	else 
	{
		if((buf[0] == 'g')&&(buf[3]=='D'))		//获取数据，这个命令是用来GET手表的数据
		{
			System_Sta.Send_Start = 1;
			return 0;
		}
		else 
		{
			if((buf[0] == 'c')&&(buf[1]=='o'))
			{
				System_Sta.App_Sta = 1 ;		//APP已经连接
				return 0;
			}
			else
			{
				if((buf[0] == 'u')&&(buf[1]=='n'))
				{
					System_Sta.App_Sta = 0 ;		//APP断开连接
					return 0;
				}
			}
		}
	}
	return 1;//无效的数据
}


//体征数据编码
//sta 0:无求救信息 1:摔倒 2:心率 3:体温 4:其他
int Data_Convert(float Temp,u8 Heart,u8 BP,u8 BD,u8 sta)
{
	u16 buf = (u16)(Temp*10);
	Data_All[19]= Data_Num[buf/100];		//嵌入温度数据
	Data_All[20]= Data_Num[buf%100/10];
	Data_All[21]= Data_Num[buf%10];
	
	Data_All[24]= Data_Num[Heart/100];		//嵌入心率数据
	Data_All[25]= Data_Num[Heart%100/10];
	Data_All[26]= Data_Num[Heart%10];
	
	Data_All[30]= Data_Num[BP/100];			//嵌入收缩压
	Data_All[31]= Data_Num[BP%100/10];
	Data_All[32]= Data_Num[BP%10];
	
	Data_All[36]= Data_Num[BD/100];			//嵌入收舒张压
	Data_All[37]= Data_Num[BD%100/10];
	Data_All[38]= Data_Num[BD%10];
	
	switch(sta)
	{
		case 0:Data_All[40]=Help_nu[0];		//没有求救
			   Data_All[41]=Help_nu[1];
			   break;	
		case 1:Data_All[40]=Help_hg[0];		//摔倒求救
			   Data_All[41]=Help_hg[1];
			   break;		
		case 2:Data_All[40]=Help_hh[0];		//心率求救
			   Data_All[41]=Help_hh[1];
			   break;		
		case 3:Data_All[40]=Help_ht[0];		//体温求救
			   Data_All[41]=Help_ht[1];
				break;		
		case 4:Data_All[40]=Help_ho[0];		//其他求救
			   Data_All[41]=Help_ho[1];
			   break;
		default:break;
	}
	return 0;
}

//网络时钟校准函数
//服务器返回的数据格式为YXXRXXHXXMXX
int Watch_Get_Time(void)
{
	u8 buf[15],i=0;
	u8 mon=0,date=0,hour = 0,min = 0;
	if(Wifi_Send2Sever(Time_AdJust)){return 1;}
	if(Wifi_DataWait(1000)){return 1;}//10s
	if(Wifi_DataGet(buf)){return 1;}
	if(buf[0]=='Y')		//月份
	{
		mon = (buf[1]-'0')*10+(buf[2]-'0');
	}
	if(buf[3]=='R')		//日期
	{
		date = (buf[4]-'0')*10+(buf[5]-'0');
	}
	if(buf[6]=='H')		//小时
	{
		hour = (buf[7]-'0')*10+(buf[8]-'0');
	}
	if(buf[9]=='M')		//分
	{
		min = (buf[10]-'0')*10+(buf[11]-'0');
	}
	if(hour>12)
	{
		i = 1;
	}
	RTC_Set_Time(hour,min,0,1);		//设置时间
	i =RTC_Get_Week(2016,mon,date);	//获取星期信息
	RTC_Set_Date(16,mon,date,i);	//设置年月份
	return 0;
}
