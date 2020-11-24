#include "usr_wifi.h"
#include "GUI.h"
#include "get_data.h"
extern u8 SEND_AT_FLAG;
extern u16 DATA_RX_STA;
extern u8 Rxd_DataBuf[70];
/******************USR_WIFI命令*********************/
//WIFI属性有关命令
const u8 Wifi_ENTM[]={"AT+ENTM"};		//进入透传模式
const u8 Wifi_OutTM[]={"+++"};		//退出透传模式
const u8 Wifi_SendA[]={"a"};
const u8 Wifi_SendZ[]={"AT+Z"};		//重启
const u8 Wifi_KeyCheck[]={"AT+WSKEY"};//获取账号
const u8 Wifi_IDCheck[]={"AT+WSSSID"};//获取密码
const u8 Wifi_SMTLK[]={"AT+SMTLK"};//smartlink
const u8 Wifi_KeySet[]={"AT+WSKEY="};//设置账号
const u8 Wifi_IdSet[]={"AT+WSSSID="};//设置密码
const u8 Wifi_Enter[]={"\r\n"};
//局域网有关命令
const u8 Wifi_WSSSID[]={"BloodTest"};		//血压端账号
const u8 Wifi_WSKEY[]={"WPA2PSK,AES,12345678"};		//血压密码
const u8 Wifi_BPTCP[]={"AT+NETP=TCP,Client,5000,192.168.4.1"};	//链接协议，模式，端口，IP地址
const u8 Wifi_ServerTCP[]={"AT+NETP=TCP,Client,80,183.61.166.98"};	
const u8 Wifi_CheckLink[]={"AT+TCPLK"};				//检查局域网链接是否建立
const u8 Wifi_SetTcpLink[]={"AT+TCPDIS=ON"};		//建立TCP链接
//与网络有关的命令
const u8 Wifi_HTPChcek[]={"AT+HTPEN"};
const u8 Wifi_HTPOFF[]={"AT+HTPEN=OFF"};			//关闭HTP
//websocket握手
const u8 Wifi_Get[]={"GET /websocket?xx2 HTTP/1.1"};
const u8 Wifi_Host[]={"Host: myxtc910839.java.jspee.cn"};
const u8 Wifi_Upgrade[]={"Upgrade: websocket"};
const u8 Wifi_Connection[]={"Connection: keep-alive"};
const u8 Wifi_WBKey[]={"Sec-WebSocket-Key: CMZruIgSKwXQgWonViF6Fg=="};
const u8 Wifi_WBPro[]={"Sec-WebSocket-Protocol: char,superchat"};
const u8 Wifi_WBVer[]={"Sec-WebSocket-Version: 13"};

/**********全局变量定义*******************/
_Wifi_Information My_Wifi;
//u8 Key_Test[]={"WPA2PSK,AES,123456789"};
//u8 Id_Test[]={"Future"};

/*
* 函数名称：Wifi_SendAt
* 功能说明：发送WifiAt指令
* 入口参数：At命令 
* 返回值  ：0:成功
* 备注    ：无；
*/

int Wifi_SendAt(const u8 Commond[]){
	Wifi_Send_Str(Commond);
	return 0;
}

/*
* 函数名称：Wifi_Send_Str；
* 功能说明：发送字符串；
* 入口参数：无； 
* 返回值  ：无；
* 备注    ：无；
*/
int Wifi_Send_Str(const u8* p){
	while(*p!='\0')
	{
		Usart_1_Send(*p);
		p++;
	}
	return 0;
}

/*
* 函数名称：Wait_AtRxd
* 功能说明：等待接收OK
* 入口参数：延时等待时间 time*100ms  time最大值65536
* 返回值  ：1:超时未接收 0：接收成功
* 备注    ：无；
*/
int Wait_AtRxd(u16 time){
	u16 i = 0;
	while(i<time)
	{
		if((AT_RX_STA&0x8000) != 0)		//接收成功
		{
			AT_RX_STA = 0 ;		//标志位清0
			return 0;
		}
		i++;
		delay_ms(10);
	}
	return 1;
}

/*
* 函数名称：Wifi_Init
* 功能说明：WIFI初始化
* 入口参数：无;
* 返回值  ：无;
* 备注    ：初始化过程中默认将HTTP打开
*/
int Wifi_Init(void){
	
	//u8 AT1231[]={"AT+CHKTIME=60\r\n"};
	u8  i =0;
	GPIO_Clock_Set(IOPCEN);
	GPIO_Init(GPIOC,6,IO_MODE_OUT,IO_SPEED_100M,IO_OTYPER_PP,IO_KEEP,IO_H);		//REST
	GPIO_Init(GPIOC,7,IO_MODE_IN,IO_SPEED_50M,IO_OTYPER_PP,IO_KEEP,IO_H);		//LINK
	GPIO_Init(GPIOC,8,IO_MODE_IN,IO_SPEED_50M,IO_OTYPER_PP,IO_KEEP,IO_H);		//READY
	
	My_Wifi.Wifi_Sta = 0;
	
	while(!((GPIOC->IDR&(1<<8)) == 0))		//这里检查WIFI是否启动
	{
		delay_ms(100);
		if(i>100){break;}		//等待10s
		i++;
	}
	i = 0;
	while(!((GPIOC->IDR&(1<<7)) == 0))		//等待WIFI建立连接
	{
		delay_ms(100);
		if(i>50){break;}		//等待5s
		i++;
	}
	if(Wifi_OutUDP())	//进入指令模式,这里保证一定成功
	{
		Wifi_OutUDP();
	}
//	Wifi_SendAt(AT1231);
	if((GPIOC->IDR&(1<<7)) == 0)		//已经成功链接
	{
		//首先检查是不是连上血压端
		Wifi_GetId();
		if(!((My_Wifi.Wifi_Id[0] == 'B')&(My_Wifi.Wifi_Id[5] == 'T')))		
		{
			My_Wifi.Wifi_Sta |= (1<<7);		//WIFI已链接
			Wifi_GetKey();					//获取已链接的密码
			delay_ms(1000);
			Wifi_GetId();					//获取已链接的账号
			delay_ms(1000);
			if(Wifi_LinkServer())			//建立WEBSOCKET连接
			{
				Wifi_LinkServer();			
			}
		}
	}
	Wifi_InUDP();		//只有在数据模式才能从服务器接数据
	
	return 0;
}

/*
* 函数名称：Wifi_DataWait
* 功能说明：等待数据接收完成
* 入口参数：等待的时间times*100ms
* 返回值  ：0 成功 1失败
* 备注    ：无；
*/
int Wifi_DataWait(u16 times)
{
	u16 i= 0;
	while((DATA_RX_STA&0x4000) == 0)		//等待数据接收成功
	{
		delay_ms(10);
		i++;
		if(i>times)				//等待times*100ms
		{
			return 1;			//出错
		}
	}
	DATA_RX_STA = 0;			//标志位清0
	return 0;
}

/*
* 函数名称：Wifi_CheckSta
* 功能说明：判断接收到的是ON还是OFF
* 入口参数：无
* 返回值  ：1 ON 2 OFF
* 备注    ：无；
*/
int Wifi_CheckSta(u16 times)
{
	u8 i=0;
	while(i<times)
	{
		if((DATA_RX_STA&0x4000) != 0)		//接收成功
		{
			if(((Rxd_DataBuf[0]=='O')&(Rxd_DataBuf[1]=='N'))||((Rxd_DataBuf[0]=='o')&(Rxd_DataBuf[1]=='n')))	//大小写兼容
			{
				DATA_RX_STA = 0 ;	
				return 0;		//提取到ON
			}
			else
			{
				DATA_RX_STA = 0 ;	
				return 1;		//提取到OFF
			}	
		}
		i++;
		delay_ms(100);
	}
		AT_RX_STA = 0 ;	
		return 2;	
}

/*
* 函数名称：Wifi_DataGet
* 功能说明：提取WIFI接收到的数据
* 入口参数：存放数据的数组
* 返回值  ：无
* 备注    ：无；
*/
int Wifi_DataGet(u8 *pstr)
{
	u8 i;
	for(i=0;i<200;i++)
	{
		if((Rxd_DataBuf[i] == '\r')||(Rxd_DataBuf[i] == '}'))		//提取ID
		{
			DATA_RX_STA = 0;			//标志位清0
			*pstr = '\0';
			return 0;
		}
		*pstr = Rxd_DataBuf[i];
		pstr++;
	}
	return 1;
}

/*
* 函数名称：Wifi_Send2Sever
* 功能说明：WIFI发数据到服务器
* 入口参数：存放的字符数组地址
* 返回值  ：0成功，1建立服务器连接失败
* 备注    ：无；
*/
int Wifi_Send2Sever(u8 *pstr)
{
	
	if(Wifi_LinkServer())
	{
		return 1;
	}

	if((My_Wifi.Wifi_Sta&(1<<4)) != 0)			//判断是命令模式，进入数据传输模式
	{
		Wifi_InUDP();
	}
	SEND_AT_FLAG = 0;
	Wifi_SendAt(pstr);			//发送数据
	if(Wait_AtRxd(1000))	//发送数据失败
	{
		Wifi_ReStart();
		delay_ms(5000);
		Set_WebSocket();			//重新建立WEB链接
		delay_ms(1000);
		Wifi_SendAt(pstr);			//再尝试发一次
		if(Wait_AtRxd(1000)){SEND_AT_FLAG = 1;return 2;}		//第二次失败直接退出
	}
	SEND_AT_FLAG = 1;
	return 0;
}

/*
* 函数名称：Wifi_ReStart
* 功能说明：Wifi重启
* 入口参数：无
* 返回值  ：无
* 备注    ：无；
*/
//void Wifi_Reload(void)
//{
//	u8 i = 0;
//	GPIOD->ODR &= ~(1<<4);		//PD4拉低 启动恢复出厂设置
//	delay_ms(5000);				//大于3s才有用
//	GPIOD->ODR |= (1<<4);		//PD4拉高
//	delay_ms(2000);				//等待稳定
//	while((GPIOD->IDR&(1<<5)) != 0)		//P5为高，模块为启动
//	{
//		delay_ms(1000);
//		if(i>50){break;}		//等待5s
//		i++;
//	}
//	delay_ms(2000);				//等待稳定
//	while((GPIOD->IDR&(1<<5)) != 0)		//P5为高，模块为启动
//	{
//		delay_ms(1000);
//		if(i>50){break;}		//等待5s
//		i++;
//	}
//	My_Wifi.Wifi_Sta &= ((1<<5)) |((1<<6))|((1<<3))|((1<<2));		//账号密码位不清0
//}

/*
* 函数名称：Wifi_OutTM
* 功能说明：Wifi进入指令模式
* 入口参数：无
* 返回值  ：无
* 备注    ：无；
*/
int Wifi_OutUDP(void)
{
	if((My_Wifi.Wifi_Sta&(1<<4)) == 0)			//当前为透传模式
	{
		SEND_AT_FLAG = 0;
		Wifi_SendAt(Wifi_OutTM);	//Wifi进入命令模式
		delay_ms(1500);				//3s内需要回应a
		Wifi_SendAt(Wifi_SendA);		
		if(Wait_AtRxd(500))
		{
			Wifi_ReStart();
			SEND_AT_FLAG = 1;
			return 1;
		}//失败重启
		else{My_Wifi.Wifi_Sta |= (1<<4);}	//等待5s,如果置位成功命令模式
		SEND_AT_FLAG = 1;
		return 0;
	}
	return 0;
}

/*
* 函数名称：Wifi_InUDP
* 功能说明：Wifi进入透传
* 入口参数：无
* 返回值  ：无
* 备注    ：无；
*/
int Wifi_InUDP(void)			
{	
	if((My_Wifi.Wifi_Sta&(1<<4)) != 0)			//当前为命令模式
	{		
		SEND_AT_FLAG = 0;	
		Wifi_SendAt(Wifi_ENTM);			//进入透传模式
		Wifi_SendAt(Wifi_Enter);		
		if(Wait_AtRxd(500)){Wifi_ReStart();SEND_AT_FLAG = 1;return 1;}//等待命令发送成功
		else{My_Wifi.Wifi_Sta &= ~(1<<4);}		//置位透传模式标志位
		SEND_AT_FLAG = 1;
		return 0;
	}
	return 0;
}

/*
* 函数名称：Wifi_Link
* 功能说明：WIFI的账号和密码
* 入口参数：无
* 返回值  ：无
* 备注    ：无；
*/
int Wifi_Link(u8 *IdStr,u8 *KeyStr)
{
	if((My_Wifi.Wifi_Sta&(1<<4)) == 0)		//如果是透传模式
	{
		if((Wifi_OutUDP()))		//进入指令模式
		{
			Wifi_OutUDP();
		}
	}
	SEND_AT_FLAG = 0;
	delay_ms(1000);
	Wifi_SendAt(Wifi_KeySet);//设置密码
	Wifi_SendAt(KeyStr);
	Wifi_SendAt(Wifi_Enter);
	if(Wait_AtRxd(500)){SEND_AT_FLAG = 1;return 1;}//等待返回OK
	delay_ms(2000);
	Wifi_SendAt(Wifi_IdSet);//设置账号
	Wifi_SendAt(IdStr);
	Wifi_SendAt(Wifi_Enter);
	if(Wait_AtRxd(500)){SEND_AT_FLAG = 1;return 2;}		//等待返回OK
	delay_ms(500);	
	if(Wifi_ReStart()){Wifi_ReStart();SEND_AT_FLAG = 1;return 3;} //Wifi重启
	SEND_AT_FLAG = 1;
	return 0;
}

/*
* 函数名称：Wifi_ReStart
* 功能说明：重启
* 入口参数：无
* 返回值  ：无
* 备注    ：无；
*/
int Wifi_ReStart(void)
{
	u8 i=0;
	u8 i_am_test[]={"123\r\n"};
	Wifi_InUDP();
	Wifi_SendAt(i_am_test);
	GPIOC->ODR &= ~(1<<6); //拉低
	delay_ms(100);
	GPIOC->ODR |= (1<<6);//拉高重启
	My_Wifi.Wifi_Sta &= ((1<<5))|((1<<6))|((1<<2))|((1<<7))|((1<<1));	/*这里的7位需要去掉*/	//账号密码标志位 HTTP标志位
	delay_ms(2000); //2ms等待模块稳定
	while(!((GPIOC->IDR&(1<<8)) == 0))			//等待模块启动完成
	{
		delay_ms(100);
		if(i>100){return 1;}		//等待10s
		i++;
	}
	delay_ms(2000);			//等待模块稳定
	return 0;
}


/*
* 函数名称：Wifi_SmartLink
* 功能说明：启动智能连接模式
* 入口参数：无
* 返回值  ：无
* 备注    ：无；
*/
int Wifi_SmartLink(void)
{
	u8 times=0;
	u8 i_am_test[]={"123"};
	Wifi_InUDP();
	Wifi_SendAt(i_am_test);
	//Wifi_ReStart();
	if((My_Wifi.Wifi_Sta&(1<<4)) == 0)			//判断是数据模式
	{
		if(Wifi_OutUDP())
		{
			Wifi_OutUDP();
		}
	}
	
	SEND_AT_FLAG = 0;
//	Wifi_SendAt(Wifi_BPTCP);			//TCP设置
//	Wifi_SendAt(Wifi_Enter);
//	Wait_AtRxd(300);
//	

	Wifi_SendAt(Wifi_SMTLK);//启动smart link
	Wifi_SendAt(Wifi_Enter);
	SEND_AT_FLAG = 1;
	delay_ms(2000);		//等待稳定
	while((GPIOC->IDR&(1<<8)) != 0)			//等待模块启动完成
	{
		delay_ms(1000);
		times ++;
		if(times>90)
		{
			return 1;
		}
	}
	delay_ms(10000);			//10s等待WIFI稳定
	My_Wifi.Wifi_Sta &= ((1<<5))|((1<<6))|((1<<2))|((1<<7))|((1<<1)); //WIFI在这里重启过
	if((GPIOC->IDR&(1<<7)) == 0)		//链接成功
	{
		My_Wifi.Wifi_Sta |= (1<<7);		//WIFI已链接
		My_Wifi.Wifi_Sta &= ~(1<<4);
		Wifi_GetKey();
		delay_ms(2000);
		Wifi_GetId();
		delay_ms(1000);
		return 0;
	}
	else
	{
		return 1;
	}
}
/*
* 函数名称：Wifi_GetKey
* 功能说明：提取WIFI的密码
* 入口参数：无
* 返回值  ：无
* 备注    ：无；
*/
int Wifi_GetKey(void)
{	
	if((My_Wifi.Wifi_Sta&(1<<4)) == 0)			//判断是数据模式
	{
		if(Wifi_OutUDP())
		{
			Wifi_OutUDP();
		}
	}
	delay_ms(1000);
	Wifi_SendAt(Wifi_KeyCheck);		//WIFI密码查询
	Wifi_SendAt(Wifi_Enter);		
	if(!(Wifi_DataWait(1000)))//等待数据接收成功
	{			
		Wifi_DataGet(My_Wifi.Wifi_Key);//保存密码
		My_Wifi.Wifi_Sta |= (1<<6);
	}
	else{return 2;}
	return 0;
}

/*
* 函数名称：Wifi_GetId
* 功能说明：提取WIFI的账号
* 入口参数：无
* 返回值  ：无
* 备注    ：无；
*/
int Wifi_GetId(void)
{	
	if((My_Wifi.Wifi_Sta&(1<<4)) == 0)			//判断是数据模式
	{
		if(Wifi_OutUDP())
		{
			Wifi_OutUDP();
		}
	}
	delay_ms(1000);
	Wifi_SendAt(Wifi_IDCheck);			//WIFI账号检测
	Wifi_SendAt(Wifi_Enter);		
	if(!(Wifi_DataWait(1000)))			//等待数据接收成功
	{	
		Wifi_DataGet(My_Wifi.Wifi_Id);			//保存账号
		My_Wifi.Wifi_Sta |= (1<<5);
	}		
	else{return 2;}
	return 0;
}



int Wifi_SendCheck(const u8 *Str,u8 times)
{	
	u8 i=0;
	if((My_Wifi.Wifi_Sta&(1<<4)) == 0)			//判断是数据模式
	{
		if(Wifi_OutUDP())
		{
			Wifi_OutUDP();
		}
	}
	delay_ms(1000);
	Wifi_SendAt(Str);			//检测命令
	Wifi_SendAt(Wifi_Enter);		
	i = Wifi_CheckSta(times);
	if(i == 1)
	{
		return 1;		//关闭
	}
	else
	{
		if(i == 0){return 0;}//打开
		 else{return 2;}	//获取失败
	}
	
//	return 0;	
}

/*
* 函数名称：Wifi_LinkBP
* 功能说明：与血压端建立连接
* 入口参数：无
* 返回值  ：无
* 备注    ：无；
*/
int Wifi_LinkBP(void)
{
	u8 Bp_LK[]={"LK"};
	u8 i;
	if((My_Wifi.Wifi_Sta&(1<<4)) == 0)			//判断是数据模式
	{
		if(Wifi_OutUDP())			//这里保证一定成功
		{
			Wifi_OutUDP();				
		}
	}
	SEND_AT_FLAG = 0;
	delay_ms(1000);
	Wifi_SendAt(Wifi_KeySet);//设置密码
	Wifi_SendAt(Wifi_WSKEY);
	Wifi_SendAt(Wifi_Enter);
	if(Wait_AtRxd(500)){SEND_AT_FLAG = 1;
	return 1;}//等待返回OK
	delay_ms(2000);
	Wifi_SendAt(Wifi_IdSet);//设置账号
	Wifi_SendAt(Wifi_WSSSID);
	Wifi_SendAt(Wifi_Enter);
	if(Wait_AtRxd(500)){SEND_AT_FLAG = 1;
	return 2;}		//等待返回OK
	delay_ms(500);
	Wifi_SendAt(Wifi_BPTCP);			//TCP设置
	Wifi_SendAt(Wifi_Enter);
	if(Wait_AtRxd(500)){SEND_AT_FLAG = 1;
	return 2;}
	My_Wifi.Wifi_Sta |= (1<<2);			//TCP标志位
	My_Wifi.Wifi_Sta &= ~(1<<1);
	delay_ms(2000);
	Wifi_SendAt(Wifi_SetTcpLink);//建立TCP连接
	Wifi_SendAt(Wifi_Enter);
	if(Wait_AtRxd(500)){SEND_AT_FLAG = 1;
	return 3;}	
	delay_ms(500);
	SEND_AT_FLAG = 1;
	My_Wifi.Wifi_Sta &= ~(1<<7);		//网络连接已断开
	Wifi_ReStart();//重启上面的设置
	delay_ms(1000);			//1s
	while(1)
	{
		if(Wifi_SendCheck(Wifi_CheckLink,30))	//检查TCP连接否是建立
		{
				
			if(i==6)	//
			{
				return 1;
			}
			if(i == 3)
			{
				Wifi_ReStart();		//重启试试
			}					
			i++;
			delay_ms(5000);		//5s
		}
		else
		{
				My_Wifi.Wifi_Sta |= (1<<2);	//BPTCP建立成功标志位
				break;
		}
	}
	Wifi_InUDP();
	SEND_AT_FLAG = 0;
	Wifi_SendAt(Bp_LK);
	if(Wait_AtRxd(6000)){SEND_AT_FLAG = 1;return 5;}
	SEND_AT_FLAG = 1;
	Wifi_InUDP();
	return 0;
}
/*
* 函数名称：Wifi_StartBp
* 功能说明：开始测量
* 入口参数：无
* 返回值  ：无
* 备注    ：无；
*/
int Wifi_StartBp(u8 *Bpdata)
{
	u8 Bp_ST[]={"ST"};
	u8 Bp_NL[]={"NL"};
	Wifi_InUDP();
	SEND_AT_FLAG = 0;
	Wifi_SendAt(Bp_ST);
	if(Wait_AtRxd(2000)){SEND_AT_FLAG = 1;return 1;}	//等待响应ok
	SEND_AT_FLAG = 1;
	//delay_ms(60000);
	if(!(Wifi_DataWait(12000)))
	{
		Wifi_DataGet(Bpdata);			//获取血压数据
	}
	else{return 2;}
	SEND_AT_FLAG = 0;
	Wifi_SendAt(Bp_NL);
	if(Wait_AtRxd(500)){SEND_AT_FLAG = 1;}
	SEND_AT_FLAG =1;
	Wifi_LinkServer();
	return 0;
}
/*
* 函数名称：Wifi_LinkServer
* 功能说明：连接至服务器
* 入口参数：无
* 返回值  ：无
* 备注    ：无；
*/
int Wifi_LinkServer(void)
{	
	u8 i = 0;
	if((My_Wifi.Wifi_Sta&(1<<4)) == 0)			//判断是数据模式
	{
		if(Wifi_OutUDP())			//这里保证一定成功
		{
			Wifi_OutUDP();				
		}
	}
	//TCP设置
	if(!(((My_Wifi.Wifi_Sta & (1<<1)) !=0)&&((My_Wifi.Wifi_Sta & (1<<2)) ==0)))
	{
		SEND_AT_FLAG = 0;
		delay_ms(500);
		Wifi_SendAt(Wifi_ServerTCP);		//发送TCP设置指令
		Wifi_SendAt(Wifi_Enter);
		if(Wait_AtRxd(300)){SEND_AT_FLAG = 1;return 1;}
		delay_ms(1000);
		Wifi_SendAt(Wifi_SetTcpLink);//建立TCP连接
		Wifi_SendAt(Wifi_Enter);
		My_Wifi.Wifi_Sta |= (1<<1);		//webTCP设置
		My_Wifi.Wifi_Sta &= ~(1<<2);	//BPTCP清０
		delay_ms(500);
		SEND_AT_FLAG = 1;
		delay_ms(1000);
		i = 1;
	}
	//WIFI链接
	if((My_Wifi.Wifi_Sta &(1<<7)) == 0)
	{
		if(((My_Wifi.Wifi_Sta &(1<<6)) != 0)&((My_Wifi.Wifi_Sta &(1<<5)) != 0))		//账号密码已保存
		{
			if(Wifi_Link(My_Wifi.Wifi_Id,My_Wifi.Wifi_Key)){return 3;}		//链接WIFI,失败返回3
			My_Wifi.Wifi_Sta |= (1<<7);			//WIFI链接标志位
		}
		else	//BUG,没有账号密码的时候
		{
			if(i == 1){Wifi_ReStart();}	
		}
	}
	else
	{
		if(i == 1){Wifi_ReStart();}		//重启生效TCP设置
	}
	
	//TCP链接检查
	if((My_Wifi.Wifi_Sta & (1<<0)) == 0)
	{	
		if((Wifi_OutUDP()))		//进入指令模式，这里保证成功
		{
			Wifi_OutUDP();		
		}
		i = 0;
		delay_ms(1000);
		while(1)
		{
			if(Wifi_SendCheck(Wifi_CheckLink,30))	//检查TCP连接否是建立
			{
				
				if(i==6)	//
				{
					return 1;
				}
				if(i == 3)
				{
					Wifi_ReStart();		//重启试试
				}					
				i++;
				delay_ms(5000);		//5s
			}
			else
			{
				My_Wifi.Wifi_Sta |= (1<<0);		//TCP建立成功标志位
				break;
			}
		}
	}		
	
	delay_ms(1000);
	if((My_Wifi.Wifi_Sta & (1<<3)) == 0)
	{
		if(Set_WebSocket())		//这里尝试建立两次链接
		{
			Wifi_ReStart();		//重启后重新链接一次
			delay_ms(15000);		//等待15s
			if(Set_WebSocket()){return 1;}
		}
	}
	if(!(Wifi_DataWait(100)))
	{
		if(Data_GetServer()){GUI_DispStringAt("server err",120,120);GUI_Exec();}//数据处理
		else{/*Wifi_Send2Sever(Data_Ok);*/}//返回OK给服务器
		Wifi_InUDP();		//进入数据透传模式
	}
//	else
//	{
//		GUI_DispStringAt("GG",60,120);
//		GUI_Exec();
//	}
	return 0;
}

int Set_WebSocket(void)
{
	if((My_Wifi.Wifi_Sta&(1<<4)) != 0)			//判断是命令模式
	{
		Wifi_InUDP();				
	}
	SEND_AT_FLAG = 0;
	delay_ms(1000);
	Wifi_SendAt(Wifi_Get);			//Get
	Wifi_SendAt(Wifi_Enter);
	delay_ms(1000);
	Wifi_SendAt(Wifi_Host);			//Host
	Wifi_SendAt(Wifi_Enter);
	delay_ms(1000);
	Wifi_SendAt(Wifi_Upgrade);		//Upgrade
	Wifi_SendAt(Wifi_Enter);
	delay_ms(1000);
	Wifi_SendAt(Wifi_Connection);	//Connection
	Wifi_SendAt(Wifi_Enter);
	delay_ms(1000);
	Wifi_SendAt(Wifi_WBKey);		//Sec-WebSocket-Key
	Wifi_SendAt(Wifi_Enter);
	delay_ms(1000);
	Wifi_SendAt(Wifi_WBPro);		//Sec-WebSocket-Protocol
	Wifi_SendAt(Wifi_Enter);
	delay_ms(1000);
	Wifi_SendAt(Wifi_WBVer);		//Version
	Wifi_SendAt(Wifi_Enter);
	delay_ms(1000);
	Wifi_SendAt(Wifi_Enter);		//数据最后以\r\n结束
//	delay_ms(1000);
	if(Wait_AtRxd(3000)){My_Wifi.Wifi_Sta &=~(1<<3);SEND_AT_FLAG = 1;return 1;}		//等待服务器返回OK
	else{My_Wifi.Wifi_Sta |= (1<<3);}			//WEBSOCKET建立成功
	SEND_AT_FLAG = 1;
	//GUI_DispStringAt("Server OK",120,120);
	return 0;
}



