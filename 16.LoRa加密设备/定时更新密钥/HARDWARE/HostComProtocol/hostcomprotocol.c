#include "hostcomprotocol.h"


//声明函数
extern void SX1276LoRaSetSpreadingFactor( u8 factor );
extern void SX1276LoRaSetSignalBandwidth( u8 bw );

extern tLoRaSettings LoRaSettings;
extern u8 SM4_KEY[16];
extern u8 SelfID_Buf[16];
extern _calendar_obj calendar;		
extern u8 PrintTimeFlag;

void HostComSetATCmdACK(u8 *buf,u8 datasize);
void HostComConfirmUserCmdInfoACK(u8 *buf,u8 datasize);
void HostComSetUserInfoCmdACK(u8 *buf,u8 datasize);
void HostComConfirmUserCmdInfoERRACK(u8 *buf,u8 datasize);
void HostComSetLoRaParaCmdACK(u8 *buf,u8 datasize);
void HostComInquireSm4KeyCmdACK(u8 *buf,u8 datasize);
void HostComInquireHSC32IDCmdACK(u8 *buf,u8 datasize);
void HostComSetSysTimeCmdACK(u8 *buf,u8 datasize);
void HostComInquireSysTimeCmdACK(u8 *buf,u8 datasize);
void HostComSetDestDeviceIDCmdACK(u8 *buf,u8 datasize);
void HostComSetDestDeviceIDCmdErrorACK(u8 *buf,u8 datasize);
void HostComExportOperateLogCmdACK(u8 *buf,u16 datasize);
void HostComExportSystemLogCmdACK(u8 *buf,u16 datasize);
void HostComExportEncryptLogCmdACK(u8 *buf,u16 datasize);
void HostComExportCommunicateLogCmdACK(u8 *buf,u16 datasize);
void HostComInquireUartParaCmdACK(u8 *buf,u16 datasize);
void HostComInquireLoRaParaCmdACK(u8 *buf,u16 datasize);
void HostComInquireDestIdCmdACK(u8 *buf,u16 datasize);
void HostComLogoutCmdACK(u8 *buf,u16 datasize);
void UserInfoInitPowerOnDevice(void);
u8 	JudgeDataIseFfective(u8 *buf);
void SaveUartParaToMemChip(void);
void HandleLogoutCmd(u8 NowLogType);

_ComWorkMode		com_work_mode = ComMode_AT;	//串口的工作模式
_HostComStructure 	hostcom_struct;				//定义上位机结构体
_AT_TimerStruct		at_timer_struct;			//at设置模式计时器

u16 SysYear = 0;
u8 	SysMonth = 0;
u8 	SysDate = 0;
u8 	SysHour = 0;
u8 	SysMin = 0;
u8 	SysSec = 0;
static u8  ExportLogEnable = DISABLE;
_LogTypes log_type = NoneLogFlag;


/****************************************
	* 功能描述：串口1中断服务程序
	* 入口参数：无
	* 返回值：	无
	* 备注：	无
*****************************************/
void USART1_IRQHandler(void)
{
	u16 Temp = 0x00;
	
	//判断是否真的空闲帧来了
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  
	{
		Temp = USART1->SR;												//先读SR,然后DR才能清除
		Temp = USART1->DR;												//清USART_IT_IDLE标志

		DMA_Cmd(DMA1_Channel5, DISABLE);								//关闭DMA接受，防止期间还有数据过来
		Temp = HOST_RX_SIZE - DMA_GetCurrDataCounter(DMA1_Channel5);	//计算接收的字节数
		hostcom_struct.Comx_RxCnt = Temp;								//将接收的字节长度赋值
		hostcom_struct.Comx_RevCompleted = ENABLE;						//接收完成标志位	
	}	
}


/****************************************
	* 功能描述：串口1中断服务程序
	* 入口参数：无
	* 返回值：	无
	* 备注：	无
*****************************************/
void USART3_IRQHandler(void)
{
	u16 Temp = 0x00;

	//判断是否真的空闲帧来了
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)  
	{
		Temp = USART3->SR;												//先读SR,然后DR才能清除
		Temp = USART3->DR;												//清USART_IT_IDLE标志
		
		DMA_Cmd(DMA1_Channel3, DISABLE);								//关闭DMA接受，防止期间还有数据过来
		Temp = HOST_RX_SIZE - DMA_GetCurrDataCounter(DMA1_Channel3);	//计算接收的字节数
		hostcom_struct.Comx_RxCnt = Temp;								//将接收的字节长度赋值
		hostcom_struct.Comx_RevCompleted = ENABLE;						//接收完成标志位	
	}	
}


/***************************************************
	* 功能描述：上位机软件参数初始化
	* 入口参数：无
	* 返回值：	无
	* 备注：	使用printf时容易导致程序崩溃，宏
				定义需修改，使用哪个串口为未知数！
****************************************************/
void HostComSoftWareInit(void)
{
	ConfirmUartx();				//获取串口编号
	SetUartBound();				//设置串口波特率
		
	SaveUartParaToMemChip();	//****保存日志(设置串口参数)

	//判断是否更改密码
	UserInfoInitPowerOnDevice();		
	
	hostcom_struct.host_status = None;
	hostcom_struct.Comx_RevCompleted = DISABLE;
	hostcom_struct.Comx_RxCnt = 0x00;
	hostcom_struct.Comx_TxCnt = 0x00;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		Usart_X_Init(COM1);
		//------------------------------------------------------------------
		DMA_RX_Init(COM1,hostcom_struct.Comx_RxBuf);	//DMA1串口1初始化
		DMA_TX_Init(COM1,hostcom_struct.Comx_TxBuf);	//DMA1串口1初始化
		
		RS485Uart1SendData(hostcom_struct.WhichUart,(u8*)"UART IS READY!\r\n",strlen("UART IS READY!\r\n"));
		//****保存系统日志(启动上位机管理功能)
		RestoreWhichLog(SystemLogFlag,SystemLogBuf,START_HOST_MANGE_CODE,0,Log_Successful);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		Usart_X_Init(COM3);
		//------------------------------------------------------------------
		DMA_RX_Init(COM3,hostcom_struct.Comx_RxBuf);	//DMA1串口3初始化
		DMA_TX_Init(COM3,hostcom_struct.Comx_TxBuf);	//DMA1串口3初始化
		
		RS485Uart1SendData(hostcom_struct.WhichUart,(u8*)"UART IS READY!\r\n",strlen("UART IS READY!\r\n"));
		
		//****保存系统日志(启动上位机管理功能)
		RestoreWhichLog(SystemLogFlag,SystemLogBuf,START_HOST_MANGE_CODE,0,Log_Successful);
	}
}


//设备上电前用户信息初始化
void UserInfoInitPowerOnDevice(void)
{
	u8 InfoBuf[16] = {0,};
	
	//第1步
	W25QXX_Read(InfoBuf,SYS_INFO_ADDR,12);
	if( InfoBuf[11] == SETED_USER_INFO)
		memcpy(hostcom_struct.SysPwd,InfoBuf,11);
	else
		memcpy(hostcom_struct.SysPwd,"Bon$on@2018", strlen("Bon$on@2018"));
	memset(InfoBuf,0,16);
	delay_ms(10);
	
	//第2步
	W25QXX_Read(InfoBuf,AUDIT_INFO_ADDR,12);
	if( InfoBuf[11] == SETED_USER_INFO)
		memcpy(hostcom_struct.AuditPwd,InfoBuf,11);
	else
		memcpy(hostcom_struct.AuditPwd,"Bon$on@2018", strlen("Bon$on@2018"));
	memset(InfoBuf,0,16);
	delay_ms(10);
	
	//第3步
	W25QXX_Read(InfoBuf,SEC_INFO_ADDR,12);
	if( InfoBuf[11] == SETED_USER_INFO)
		memcpy(hostcom_struct.SecPwd,InfoBuf,11);
	else
		memcpy(hostcom_struct.SecPwd,"Bon$on@2018", strlen("Bon$on@2018"));
	
	memcpy( hostcom_struct.AuditUser, "audit",strlen("audit") );
	memcpy( hostcom_struct.SysUser,   "sys",  strlen("sys")   );
	memcpy( hostcom_struct.SecUser,   "sec",  strlen("sec")   );
}


/**************************************************
	* 功能描述：保存串口设置参数信息到存储芯片
	* 入口参数：无
	* 返回值：	无
	* 备注：	
***************************************************/
void SaveUartParaToMemChip(void)
{
	switch(hostcom_struct.WhichUart)
	{
		//485接口
		case UART1:
			switch(hostcom_struct.Bound)
			{
				case 9600:
					RestoreWhichLog(SystemLogFlag,SystemLogBuf,7,0,Log_Successful);
				break;
				
				case 19200:
					RestoreWhichLog(SystemLogFlag,SystemLogBuf,8,0,Log_Successful);
				break;
				
				case 38400:
					RestoreWhichLog(SystemLogFlag,SystemLogBuf,9,0,Log_Successful);
				break;
				
				case 115200:
					RestoreWhichLog(SystemLogFlag,SystemLogBuf,10,0,Log_Successful);
				break;
				
				default:
				break;
			}
		break;
		
		//232接口
		case UART3:
			switch(hostcom_struct.Bound)
			{
				case 9600:
					RestoreWhichLog(SystemLogFlag,SystemLogBuf,11,0,Log_Successful);
				break;
				
				case 19200:
					RestoreWhichLog(SystemLogFlag,SystemLogBuf,12,0,Log_Successful);
				break;
				
				case 38400:
					RestoreWhichLog(SystemLogFlag,SystemLogBuf,13,0,Log_Successful);
				break;
				
				case 115200:
					RestoreWhichLog(SystemLogFlag,SystemLogBuf,14,0,Log_Successful);
				break;
				
				default:
				break;
			}
		break;
		
		default:
		break;
	}
}


//判断数据有效性
u8 JudgeDataIseFfective(u8 *buf)
{
	u16 Temp= (u16)buf[0] <<8 | buf[1];
	
	//判断年月日是否有效
	if(  ( (Temp< 2018)||(Temp>2100) ) || ( buf[0]==0xFF&&buf[1] == 0xFF ) )	
	{
		return False;
	}
	else
	{
		return True;
	}
}


//判断数据有效性
void JudgeTheHostComWorkMode()
{
	if( com_work_mode == ComMode_Trans )
	{
		NET_LED_ON();
	}
	else if( com_work_mode == ComMode_AT )
	{
		NET_LED_OFF();
	}
		
}



/*************************************************************
	* 功能描述：上位机协议解析程序
	* 入口参数：HostStruct -- 上位机结构体地址
	* 返回值：	无
	* 备注：
**************************************************************/
void HostComProtocol(_HostComStructure *HostStruct)
{
	u16 CalcCrc = 0x00;
	u16 tempCrc = 0x00;
	
	//工作在设置模式
	if(com_work_mode!=ComMode_AT)
		return;
	
	//接收到数据
	if( hostcom_struct.Comx_RevCompleted == ENABLE )
	{
		//处理下一次的数据
		HostStruct->Comx_RevCompleted = DISABLE;
		
		//判断CRC校验是否通过
		CalcCrc = CalCrc16(HostStruct->Comx_RxBuf,HostStruct->Comx_RxCnt-2,0xFFFF);	
		tempCrc = (HostStruct->Comx_RxBuf[HostStruct->Comx_RxCnt - 2]<<8) | 
				  (HostStruct->Comx_RxBuf[HostStruct->Comx_RxCnt - 1]);	
		
		if(tempCrc == CalcCrc)
		{
			//解析数据帧
			switch( HostStruct->Comx_RxBuf[0] )
			{
				//设置AT指令
				case 0x4c:
					//将超时计时器清零
					if(	HostStruct->Comx_RxBuf[3] == 'A'&&
						HostStruct->Comx_RxBuf[4] == 'T')
					{
						at_timer_struct.SetAtTimer_5s = 0;
						HostComSetATCmdACK(HostStruct->Comx_TxBuf,0);	//指令应答
					}
				break;
				
				//验证用户名、密码
				case 0x4b:
					//将超时计时器清零
					at_timer_struct.SetAtTimer_5s = 0;
									
					//匹配特定的字符串
					if(	( (strstr(&HostStruct->Comx_RxBuf[3],hostcom_struct.AuditUser))&&
						  (strstr(&HostStruct->Comx_RxBuf[8],hostcom_struct.AuditPwd)) ) )
					{
						HostStruct->host_status = User_Audit;					//切换到用户模式
						HostComConfirmUserCmdInfoACK(HostStruct->Comx_TxBuf,0);	//应答
						
						//****保存操作日志(验证audit用户)
						RestoreWhichLog(OperateLogFlag,OperateLogBuf,USER_CONFIRM_CODE,AUDIT_CODE,Log_Successful);
					}
					else if( (strstr(&HostStruct->Comx_RxBuf[3],hostcom_struct.SecUser))&&
							 (strstr(&HostStruct->Comx_RxBuf[8],hostcom_struct.SecPwd)) )
					{
						HostStruct->host_status = User_Sec;						//切换到固件升级模式
						HostComConfirmUserCmdInfoACK(HostStruct->Comx_TxBuf,0);	//应答
						
						//****保存操作日志(验证sec用户)
						RestoreWhichLog(OperateLogFlag,OperateLogBuf,USER_CONFIRM_CODE,SEC_CODE,Log_Successful);
					}
					else if( (strstr(&HostStruct->Comx_RxBuf[3],hostcom_struct.SysUser))&&
							 (strstr(&HostStruct->Comx_RxBuf[8],hostcom_struct.SysPwd)) )
					{
						HostStruct->host_status = User_Sys;						//切换到固件升级模式
						HostComConfirmUserCmdInfoACK(HostStruct->Comx_TxBuf,0);	//应答
						
						//****保存操作日志(验证sys用户)
						RestoreWhichLog(OperateLogFlag,OperateLogBuf,USER_CONFIRM_CODE,SYS_CODE,Log_Successful);
					}
					//验证失败应答
					else
						HostComConfirmUserCmdInfoERRACK(HostStruct->Comx_TxBuf,0);
				break;
				
				//修改对应用户的密码
				case 0x4a:
					//将超时计时器清零
					at_timer_struct.SetAtTimer_5s = 0;
					
					//设置用户密码						
					if( strstr(&HostStruct->Comx_RxBuf[3],hostcom_struct.AuditUser) )
					{
						memcpy(HostStruct->AuditPwd, &HostStruct->Comx_RxBuf[9],11);
						
						//保存安全员登录密码
						RestoreUserPassword(hostcom_struct.AuditPwd,11,AUDIT_INFO_ADDR);
						
						//****保存操作日志(更改audit密码)
						RestoreWhichLog(OperateLogFlag,OperateLogBuf,CHANGE_PWD_CODE,AUDIT_CODE,Log_Successful);
					}
					else if( strstr(&HostStruct->Comx_RxBuf[3],hostcom_struct.SecUser) )
					{
						memcpy(HostStruct->SecPwd,  &HostStruct->Comx_RxBuf[9],11);
						
						//保存审计员登录密码
						RestoreUserPassword(hostcom_struct.SecPwd,11,SEC_INFO_ADDR);
						
						//****保存操作日志(更改sec密码)
						RestoreWhichLog(OperateLogFlag,OperateLogBuf,CHANGE_PWD_CODE,SEC_CODE,Log_Successful);
					}	
					else if( strstr(&HostStruct->Comx_RxBuf[3],hostcom_struct.SysUser) )
					{
						memcpy(HostStruct->SysPwd,  &HostStruct->Comx_RxBuf[9],11);
						
						//保存系统员登录密码
						RestoreUserPassword(hostcom_struct.SysPwd,11,SYS_INFO_ADDR);
						
						//****保存操作日志(更改sys密码)
						RestoreWhichLog(OperateLogFlag,OperateLogBuf,CHANGE_PWD_CODE,SYS_CODE,Log_Successful);
					}
					HostComSetUserInfoCmdACK(HostStruct->Comx_TxBuf,0);
				break;
		
				//设置系统时间
				case 0x49:
					//将超时计时器清零
					at_timer_struct.SetAtTimer_5s = 0;
					if(HostStruct->host_status == User_Sec)
					{
						SysYear  = HostStruct->Comx_RxBuf[3]<<8|	//年
								   HostStruct->Comx_RxBuf[4];	
						SysMonth = HostStruct->Comx_RxBuf[5];		//月		
						SysDate  = HostStruct->Comx_RxBuf[6];		//日
						SysHour  = HostStruct->Comx_RxBuf[7];		//小时
						SysMin   = HostStruct->Comx_RxBuf[8];		//分钟
						SysSec   = HostStruct->Comx_RxBuf[9];		//秒
						
						//与从机进行时间同步
						if(LoRaDeviceIs == MASTER)
							LoRaTimeSynchronizationCmd(LoRaPact.TxBuf,0);
						
						RTC_Set(SysYear,SysMonth,SysDate,SysHour,SysMin,SysSec);
						HostComSetSysTimeCmdACK(HostStruct->Comx_TxBuf,0);
						
							
						
						//****保存操作日志(设置系统时间)
						RestoreWhichLog(OperateLogFlag,OperateLogBuf,SET_SYS_TIME_CODE,SEC_CODE,Log_Successful);
					}
				break;
				
				//读取系统时间
				case 0x48:
					//将超时计时器清零
					at_timer_struct.SetAtTimer_5s = 0;
					if(hostcom_struct.host_status == User_Sec)
					{
						HostComInquireSysTimeCmdACK(HostStruct->Comx_TxBuf,0);
						
						//****保存操作日志(读取系统时间)
						RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_SYS_TIME_CODE,SEC_CODE,Log_Successful);
					}
				break;
				
				//设置LoRa参数(信号带宽、扩频因子)
				case 0x47:
					//将超时计时器清零
					at_timer_struct.SetAtTimer_5s = 0;
					if(HostStruct->host_status == User_Sec)
					{
						SX1276LoRaSetSignalBandwidth(HostStruct->Comx_RxBuf[3]);	//设置信号带宽
						SX1276LoRaSetSpreadingFactor(HostStruct->Comx_RxBuf[4]);	//设置扩频因子
						HostComSetLoRaParaCmdACK(HostStruct->Comx_TxBuf,0);			//应答
						
						//****保存操作日志(设置LoRa参数)
						RestoreWhichLog(OperateLogFlag,OperateLogBuf,SET_LORA_PARA_CODE,SEC_CODE,Log_Successful);
					}
				break;
				
				//读取SM4密钥
				case 0x46:
					//将超时计时器清零
					at_timer_struct.SetAtTimer_5s = 0;
					if(HostStruct->host_status == User_Sec)
					{
						if( (HostStruct->Comx_RxBuf[3]=='S')&&
							(HostStruct->Comx_RxBuf[4]=='M')&&
							(HostStruct->Comx_RxBuf[5]=='4'))
						{
							HostComInquireSm4KeyCmdACK(HostStruct->Comx_TxBuf,0);
							
							//****保存操作日志(读取SM4密钥)
							RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_SM4_KEY_CODE,SEC_CODE,Log_Successful);
						}
					}
				break;
				
				//读取芯片ID(加密芯片随机数)
				case 0x45:
					//将超时计时器清零
					at_timer_struct.SetAtTimer_5s = 0;
					if(HostStruct->host_status == User_Sec)
					{
						if( (HostStruct->Comx_RxBuf[3]=='I')&&
							(HostStruct->Comx_RxBuf[4]=='D')  )
						{
							HostComInquireHSC32IDCmdACK(HostStruct->Comx_TxBuf,0);
							
							//****保存操作日志(读取随机数)
							RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_RANGDOM_CODE,SEC_CODE,Log_Successful);
						}
					}
				break;
					
				//读取操作日志
				case 0x44:
					//将超时计时器清零
					at_timer_struct.SetAtTimer_5s = 0;
					if(HostStruct->host_status == User_Audit)
					{
						if( (HostStruct->Comx_RxBuf[3] == 'O')&&
							(HostStruct->Comx_RxBuf[4] == 'L') )
						{
							if(ExportLogEnable == DISABLE)
							{
								ExportLogEnable = ENABLE;
								log_type = OperateLogFlag;
							}
							
						}
					}
				break;
					
				//读取系统日志
				case 0x43:
					//将超时计时器清零
					at_timer_struct.SetAtTimer_5s = 0;
					if(HostStruct->host_status == User_Audit)
					{
						if( (HostStruct->Comx_RxBuf[3] == 'S')&&
							(HostStruct->Comx_RxBuf[4] == 'L') )
						{
							if(ExportLogEnable == DISABLE)
							{
								ExportLogEnable = ENABLE;
								log_type = SystemLogFlag;
							}
							
						}
					}
				break;
				
				//读取通信日志
				case 0x42:
					//将超时计时器清零
					at_timer_struct.SetAtTimer_5s = 0;
					if(HostStruct->host_status == User_Audit)
					{
						if( (HostStruct->Comx_RxBuf[3] == 'C')&&
							(HostStruct->Comx_RxBuf[4] == 'L') )
						{
							if(ExportLogEnable == DISABLE)
							{
								ExportLogEnable = ENABLE;
								log_type = CommunicateLogFlag;
							}
						}
					}
				break;
					
				//读取加密日志
				case 0x41:
					//将超时计时器清零
					at_timer_struct.SetAtTimer_5s = 0;
					if(HostStruct->host_status == User_Audit)
					{
						if( (HostStruct->Comx_RxBuf[3] == 'E')&&
							(HostStruct->Comx_RxBuf[4] == 'D')&&
							(HostStruct->Comx_RxBuf[5] == 'L') )
						{
							if(ExportLogEnable == DISABLE)
							{
								ExportLogEnable = ENABLE;
								log_type = EncryptFlag;
							}
						}
					}
				break;
					
				//设置目标设备的ID
				case 0x40:
					//将超时计时器清零
					at_timer_struct.SetAtTimer_5s = 0;
					if(HostStruct->host_status == User_Sec)
					{
						//判断设备地址是否越界
						if( HostStruct->Comx_RxBuf[3]<DEST_DEVICE_MAX_ADDR )
						{
							LoRaPact.DstDeviceAddr = HostStruct->Comx_RxBuf[3];
							HostComSetDestDeviceIDCmdACK(HostStruct->Comx_TxBuf,0);
							
							//如果为主机则发起密钥协商
							if(LoRaDeviceIs == MASTER)
								GetSlaveID();
							
							//****保存操作日志(设置目标设备的ID地址)
							RestoreWhichLog(OperateLogFlag,OperateLogBuf,SET_DEST_ID_CODE,SEC_CODE,Log_Successful);
						}
						else
							HostComSetDestDeviceIDCmdErrorACK(HostStruct->Comx_TxBuf,0);
					}
				break;
				
				//查看串口配置
				case 0x3F:
					//将超时计时器清零
					at_timer_struct.SetAtTimer_5s = 0;
					if(HostStruct->host_status == User_Sec)
					{
						if( (HostStruct->Comx_RxBuf[3] == 'U')&&
							(HostStruct->Comx_RxBuf[4] == 'T') )
						{
							HostComInquireUartParaCmdACK(HostStruct->Comx_TxBuf,0);
							
							//****保存操作日志(读取串口参数)
							RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_UART_PARA_CODE,SEC_CODE,Log_Successful);
						}
					}
				break;
				
				//恢复出厂设置
				case 0x3E:
					//将超时计时器清零
					at_timer_struct.SetAtTimer_5s = 0;
					if( (HostStruct->Comx_RxBuf[3] == 'R')&&
						(HostStruct->Comx_RxBuf[4] == 'E')&&
						(HostStruct->Comx_RxBuf[5] == 'S')&&
						(HostStruct->Comx_RxBuf[6] == 'T') )
					{
						//重置账号信息
						memcpy(hostcom_struct.SysUser,SYS_USER,4);
						memcpy(hostcom_struct.SysPwd,PASSWORD,12);	
						memcpy(hostcom_struct.AuditUser,AUDIT_USER,6);
						memcpy(hostcom_struct.AuditPwd,PASSWORD,12);
						memcpy(hostcom_struct.SecUser,SEC_USER,4);
						memcpy(hostcom_struct.SecPwd,PASSWORD,12);
						
						//****保存操作日志(恢复出厂设置)
						RestoreWhichLog(OperateLogFlag,OperateLogBuf,USER_REST_CODE,SEC_CODE,Log_Successful);
						
						//****保存系统日志(恢复出厂设置)
						RestoreWhichLog(SystemLogFlag,SystemLogBuf,DEVICE_RESET_CODE,0,Log_Successful);
					}
				break;
				
				//查询LoRa参数
				case 0X3D:
					//将超时计时器清零
					at_timer_struct.SetAtTimer_5s = 0;
					if(hostcom_struct.host_status == User_Sec)
					{
						if( (HostStruct->Comx_RxBuf[3] == 'L')&&
							(HostStruct->Comx_RxBuf[4] == 'o')&&
							(HostStruct->Comx_RxBuf[5] == 'R')&&
							(HostStruct->Comx_RxBuf[6] == 'a') )
						{
							HostComInquireLoRaParaCmdACK(HostStruct->Comx_TxBuf,0);
							
							//****保存操作日志(读取LoRa参数)
							RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_LORA_PARA_CODE,SEC_CODE,Log_Successful);
						}
					}
				break;
					
					
				//查询目标设备ID
				case 0X3C:
					//将超时计时器清零
					at_timer_struct.SetAtTimer_5s = 0;
					if(hostcom_struct.host_status == User_Sec)
					{
						if( (HostStruct->Comx_RxBuf[3] == 'D')&&
							(HostStruct->Comx_RxBuf[4] == 'E')&&
							(HostStruct->Comx_RxBuf[5] == 'I')&&
							(HostStruct->Comx_RxBuf[6] == 'D') )
						{
							HostComInquireDestIdCmdACK(HostStruct->Comx_TxBuf,0);
							
							//****保存操作日志(读取目标设备的ID)
							RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_DEST_ID_CODE,SEC_CODE,Log_Successful);
						}
					}
				break;
					
					
				//终止读取日志操作
				case 0X3B:
					//将超时计时器清零
					at_timer_struct.SetAtTimer_5s = 0;
					if(hostcom_struct.host_status == User_Audit)
					{
						if( (HostStruct->Comx_RxBuf[3] == 'L')&&
							(HostStruct->Comx_RxBuf[4] == 'g')&&
							(HostStruct->Comx_RxBuf[5] == 'o')  )
						{
							//停止日志导出指令处理
							HandleLogoutCmd(log_type);
							
							//****保存操作日志(终止读取日志)
							RestoreWhichLog(OperateLogFlag,OperateLogBuf,STOP_READ_LOG_CODE,AUDIT_CODE,Log_Successful);
						}
					}
				break;
					
				//固件升级
				//保存日志
				
				//其他
				default:
				break;
			}
		}
		
		//--------------------------------------------------------------------------------------------------------------------
		HostStruct->Comx_RxCnt = 0;							//将接收计数器清零
		memset(HostStruct->Comx_RxBuf,0,HOST_RX_SIZE);		//将接收缓冲区清空
		memset(HostStruct->Comx_TxBuf,0,HOST_RX_SIZE);		//将接收缓冲区清空
		//判断串口编号
		if(hostcom_struct.WhichUart == UART1)		
		{
			//设置数据接收长度，重新装填，并让数据接收地址偏移地址从0开始
			DMA_SetCurrDataCounter(DMA1_Channel5,HOST_RX_SIZE);	
			//打开DMA接收			
			DMA_Cmd(DMA1_Channel5,ENABLE);  					
		}
		else if(hostcom_struct.WhichUart == UART3)
		{
			//设置数据接收长度，重新装填，并让数据接收地址偏移地址从0开始
			DMA_SetCurrDataCounter(DMA1_Channel3,HOST_RX_SIZE);	
			//打开DMA接收			
			DMA_Cmd(DMA1_Channel3,ENABLE);  					
		}
	}
}


/********************************************
	* 功能描述：导出日志任务
	* 入口参数：无
	* 返回值：	无
	* 备注：	无
*********************************************/
void ExportLogHandle(void)
{
	//判断导出日志标志位
	if(ExportLogEnable != ENABLE)
		return;
	
	//等待1s延时到来
	if( GetExportLogTimerValue() < 1000 )
		return;
	
	//将计时器清零
	at_timer_struct.SetAtTimer_5s = 0x00;
	
	//匹配导出日志类型
	switch(log_type)
	{
		//导出操作日志
		case OperateLogFlag:
			//判断偏移地址是否越界
			if( (OPERATE_LOG_BASE_ADDR+OperateReadAddr) < OPERATE_LOG_SZIE )
			{
				ExportWhichLog(OperateLogFlag,OperateLogBuf);
				
				//加上判断数据是否有效
				if( JudgeDataIseFfective(OperateLogBuf) )
					HostComExportOperateLogCmdACK(hostcom_struct.Comx_TxBuf,0x0B);
				
				//如果数据无效，导出日志结束
				else
				{
					ExportLogEnable = DISABLE;					//结束导出
					OperateReadAddr = 0x00;						//将偏移地址重置
					memset(OperateLogBuf,0,MIN_LOG_PACK_SIZE);	//将日志缓冲区清零
					
					//****保存操作日志(查看日志)
					RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_OL_CODE,AUDIT_CODE,Log_Successful);
				}
			}
			else
			{
				ExportLogEnable = DISABLE;						//结束导出
				OperateReadAddr = 0x00;							//将偏移地址重置
				memset(OperateLogBuf,0,MIN_LOG_PACK_SIZE);		//将日志缓冲区清零
					
				//****保存加密日志(查看日志)
				RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_OL_CODE,AUDIT_CODE,Log_Successful);
			}
		break;
		
		//系统日志
		case SystemLogFlag:
			//判断偏移地址是否越界
			if( (SYSTEM_LOG_BASE_ADDR+SystemReadAddr) < SYSTEM_LOG_SZIE )
			{
				ExportWhichLog(SystemLogFlag,SystemLogBuf);
				
				//加上判断数据是否有效
				if( JudgeDataIseFfective(SystemLogBuf) )
					HostComExportSystemLogCmdACK(hostcom_struct.Comx_TxBuf,0x0A);
				
				//如果数据无效，导出日志结束
				else
				{
					ExportLogEnable = DISABLE;					//结束导出
					SystemReadAddr  = 0x00;						//将偏移地址重置
					memset(SystemLogBuf,0,MIN_LOG_PACK_SIZE);	//将日志缓冲区清零
					
					//****保存操作日志(读取系统日志)
					RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_SL_CODE,AUDIT_CODE,Log_Successful);
				}
			}
			else
			{
				ExportLogEnable = DISABLE;						//结束导出
				SystemReadAddr = 0x00;							//将偏移地址重置
				memset(SystemLogBuf,0,MIN_LOG_PACK_SIZE);		//将日志缓冲区清零
					
				//****1保存操作日志(读取系统日志)
				RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_SL_CODE,AUDIT_CODE,Log_Successful);
			}
		break;
		
		//通信日志
		case CommunicateLogFlag:
			//判断偏移地址是否越界
			if( (COMMUNICATE_LOG_BASE_ADDR+CommunicateReadAddr) < COMMUNICATE_LOG_SIZE )
			{
				ExportWhichLog(CommunicateLogFlag,CommunicateLogBuf);
				
				//加上判断数据是否有效
				if( JudgeDataIseFfective(CommunicateLogBuf) )
					HostComExportCommunicateLogCmdACK(hostcom_struct.Comx_TxBuf,MAX_LOG_PACK_SIZE);
				
				//如果数据无效，导出日志结束
				else
				{
					ExportLogEnable = DISABLE;						//结束导出
					CommunicateReadAddr = 0x00;						//将偏移地址重置
					memset(CommunicateLogBuf,0,MAX_LOG_PACK_SIZE);	//将日志缓冲区清零
					
					//****1保存操作日志(读取通信日志)
					RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_CL_CODE,AUDIT_CODE,Log_Successful);
				}
			}
			else
			{
				ExportLogEnable = DISABLE;						//结束导出
				CommunicateReadAddr = 0x00;						//将偏移地址重置
				memset(CommunicateLogBuf,0,MAX_LOG_PACK_SIZE);	//将日志缓冲区清零
					
				//****1保存加密日志(查看日志)
				RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_CL_CODE,AUDIT_CODE,Log_Successful);
			}
		break;
		
		//加密日志
		case EncryptFlag:
			//判断偏移地址是否越界
			if( (ENCRYPT_LOG_BASE_ADDR+EncryptReadAddr) < ENCRYPT_LOG_SZIE )
			{
				ExportWhichLog(EncryptFlag,EncryptLogBuf);
				
				//加上判断数据是否有效
				if( JudgeDataIseFfective(EncryptLogBuf) )
					HostComExportEncryptLogCmdACK(hostcom_struct.Comx_TxBuf,MAX_LOG_PACK_SIZE);
				
				//如果数据无效，导出日志结束
				else
				{
					ExportLogEnable = DISABLE;					//结束导出
					EncryptReadAddr = 0x00;						//将偏移地址重置
					memset(EncryptLogBuf,0,MAX_LOG_PACK_SIZE);	//将日志缓冲区清零
					
					//****保存操作日志(查看加密日志)
					RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_EL_CODE,AUDIT_CODE,Log_Successful);
				}
			}
			else
			{
				ExportLogEnable = DISABLE;					//结束导出
				EncryptReadAddr = 0x00;						//将偏移地址重置
				memset(EncryptLogBuf,0,MAX_LOG_PACK_SIZE);	//将日志缓冲区清零
					
				//****保存操作日志(查看加密日志)
				RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_EL_CODE,AUDIT_CODE,Log_Successful);
			}
		break;
		
		//其他
		default:	
		break;
	}
	ClearExportLogTimer();
}


//运行串口AT模式超时计时器
void RunComAtModeTimer(void)
{
	if( at_timer_struct.SetAtTimer_5s<45000 )
	{
		at_timer_struct.SetAtTimer_5s ++;
	}
}


/************************************************
	* 功能描述：上位机串口超时处理
	* 入口参数：无
	* 返回值：
	* 备注：
*************************************************/
void HostComTimeOutHandle(void)
{
	//超时等待，串口则退出AT设置工作模式
	if(at_timer_struct.SetAtTimer_5s > WAIT_HOST_MAX_TIME)
	{
		com_work_mode = ComMode_Trans;
		return;
	}
}


//进入设置模式
void HostComSetATCmdACK(u8 *buf,u8 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x4C;						//功能码
	
	buf[1] = 0x07>>8;					//数据包长度
	buf[2] = 0x07;
	
	buf[3] = 'O';						//回应数据
	buf[4] = 'K';
	
	crc_value = CalCrc16(buf,5,0xFFFF);	//CRC校验值
	buf[5] = crc_value>>8;
	buf[6] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//串口1DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel4,0x07);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//串口3DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel2,0x07);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//确认用户信息(用户名、密码)
void HostComConfirmUserCmdInfoACK(u8 *buf,u8 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x4B;
	
	buf[1] = 0x07>>8;
	buf[2] = 0x07;
	
	buf[3] = 'O';
	buf[4] = 'K';
	
	crc_value = CalCrc16(buf,5,0xFFFF);
	buf[5] = crc_value>>8;
	buf[6] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//串口1DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel4,0x07);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//串口3DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel2,0x07);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//确认用户信息(用户名、密码)
void HostComConfirmUserCmdInfoERRACK(u8 *buf,u8 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x4B;
	
	buf[1] = 0x08>>8;
	buf[2] = 0x08;
	
	buf[3] = 'E';
	buf[4] = 'R';
	buf[5] = 'R';
	
	crc_value = CalCrc16(buf,6,0xFFFF);
	buf[6] = crc_value>>8;
	buf[7] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//串口1DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel4,0x08);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//串口3DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel2,0x08);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//设置用户信息(用户名、密码)
void HostComSetUserInfoCmdACK(u8 *buf,u8 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x4A;
	
	buf[1] = 0x07>>8;
	buf[2] = 0x07;
	
	buf[3] = 'O';
	buf[4] = 'K';
	
	crc_value = CalCrc16(buf,5,0xFFFF);
	buf[5] = crc_value>>8;
	buf[6] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//串口1DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel4,0x07);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//串口3DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel2,0x07);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//设置LoRa通信参数
void HostComSetLoRaParaCmdACK(u8 *buf,u8 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x47;
	
	buf[1] = 0x07>>8;
	buf[2] = 0x07;
	
	buf[3] = 'O';
	buf[4] = 'K';
	
	crc_value = CalCrc16(buf,5,0xFFFF);
	buf[5] = crc_value>>8;
	buf[6] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//串口1DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel4,0x07);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//串口3DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel2,0x07);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//读取SM4密钥
void HostComInquireSm4KeyCmdACK(u8 *buf,u8 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x46;
	
	buf[1] = 0x15>>8;
	buf[2] = 0x15;
	
	memcpy(&buf[3],SM4_KEY,16);
	
	crc_value = CalCrc16(buf,3+16,0xFFFF);
	buf[19] = crc_value>>8;
	buf[20] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//串口1DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel4,0x15);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//串口3DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel2,0x15);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//读取加密芯片的ID标识符
void HostComInquireHSC32IDCmdACK(u8 *buf,u8 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x45;
	
	buf[1] = 0x15>>8;
	buf[2] = 0x15;
	
	memcpy(&buf[3],SelfID_Buf,16);
	
	crc_value = CalCrc16(buf,3+16,0xFFFF);
	buf[19] = crc_value>>8;
	buf[20] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//串口1DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel4,0x15);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//串口3DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel2,0x15);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//设置系统时间应答
void HostComSetSysTimeCmdACK(u8 *buf,u8 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x49;
	
	buf[1] = 0x07>>8;
	buf[2] = 0x07;
	
	buf[3] = 'O';
	buf[4] = 'K';
	
	crc_value = CalCrc16(buf,5,0xFFFF);
	buf[5] = crc_value>>8;
	buf[6] = crc_value;

	if(hostcom_struct.WhichUart == UART1)
	{
		//串口1DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel4,0x07);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//串口3DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel2,0x07);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//设置系统时间应答
void HostComInquireSysTimeCmdACK(u8 *buf,u8 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x48;
	
	buf[1] = 0x0C>>8;
	buf[2] = 0x0C;
	
	buf[3] = calendar.w_year>>8;
	buf[4] = calendar.w_year;
	buf[5] = calendar.w_month;
	buf[6] = calendar.w_date;
	buf[7] = calendar.hour;
	buf[8] = calendar.min;
	buf[9] = calendar.sec;

	crc_value = CalCrc16(buf,3+7,0xFFFF);
	buf[10] = crc_value>>8;
	buf[11] = crc_value;

	if(hostcom_struct.WhichUart == UART1)
	{
		//串口1DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel4,0x0C);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//串口3DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel2,0x0C);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}



//设置目标设备的地址成功应答
void HostComSetDestDeviceIDCmdACK(u8 *buf,u8 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x40;
	
	buf[1] = 0x07>>8;
	buf[2] = 0x07;
	
	buf[3] = 'O';
	buf[4] = 'K';
	
	crc_value = CalCrc16(buf,5,0xFFFF);
	buf[5] = crc_value>>8;
	buf[6] = crc_value;

	if(hostcom_struct.WhichUart == UART1)
	{
		//串口1DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel4,0x07);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//串口3DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel2,0x07);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//设置目标设备的地址错误应答
void HostComSetDestDeviceIDCmdErrorACK(u8 *buf,u8 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x40;
	
	buf[1] = 0x07>>8;
	buf[2] = 0x07;
	
	
	buf[3] = 'E';
	buf[4] = 'R';
	
	crc_value = CalCrc16(buf,5,0xFFFF);
	buf[5] = crc_value>>8;
	buf[6] = crc_value;

	if(hostcom_struct.WhichUart == UART1)
	{
		//串口1DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel4,0x07);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//串口3DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel2,0x07);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//导出操作日志应答(0X0B)
void HostComExportOperateLogCmdACK(u8 *buf,u16 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x44;
	
	buf[1] = (datasize+5) >> 8;
	buf[2] = (datasize+5);
	
	memcpy(&buf[3],OperateLogBuf,datasize);
	
	crc_value = CalCrc16(buf,datasize+3,0xFFFF);
	buf[datasize+3] = crc_value>>8;
	buf[datasize+4] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//串口1DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel4,datasize+5);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//串口3DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel2,datasize+5);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//导出系统日志应答
void HostComExportSystemLogCmdACK(u8 *buf,u16 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x43;
	
	buf[1] = (datasize+5)>>8;
	buf[2] = (datasize+5);
	
	memcpy(&buf[3],SystemLogBuf,datasize);
	
	crc_value = CalCrc16(buf,datasize+3,0xFFFF);
	buf[datasize+3] = crc_value>>8;
	buf[datasize+4] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//串口1DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel4,datasize+5);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//串口3DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel2,datasize+5);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//导出通信日志应答
void HostComExportCommunicateLogCmdACK(u8 *buf,u16 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x42;
	
	buf[1] = (datasize+5)>>8;	//高字节
	buf[2] = (datasize+5);		//低字节
	
	memcpy(&buf[3],CommunicateLogBuf,datasize);
	
	crc_value = CalCrc16(buf,datasize+3,0xFFFF);
	buf[datasize+3] = crc_value>>8;
	buf[datasize+4] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//串口1DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel4,datasize+5);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//串口3DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel2,datasize+5);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//导出加密日志应答
void HostComExportEncryptLogCmdACK(u8 *buf,u16 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x41;
	
	buf[1] = (datasize+5)>>8;
	buf[2] = (datasize+5);
	
	memcpy(&buf[3],EncryptLogBuf,datasize);
	
	crc_value = CalCrc16(buf,datasize+3,0xFFFF);
	buf[datasize+3] = crc_value>>8;
	buf[datasize+4] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//串口1DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel4,datasize+5);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//串口3DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel2,datasize+5);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//查询串口配置参数应答
void HostComInquireUartParaCmdACK(u8 *buf,u16 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x3F;
	
	buf[1] = 0x08>>8;
	buf[2] = 0x08;
	
	buf[3] = ID1;					//接口类型
	buf[4] = LoRaPact.DeviceAddr;	//设备地址
	buf[5] = ID2 | ID3<<1;			//波特率
	
 	crc_value = CalCrc16(buf,6,0xFFFF);
	buf[6] = crc_value>>8;
	buf[7] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//串口1DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel4,8);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//串口3DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel2,8);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//查询LoRa配置参数应答
void HostComInquireLoRaParaCmdACK(u8 *buf,u16 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x3D;
	
	buf[1] = 0x07>>8;
	buf[2] = 0x07;
	
	buf[3] = LoRaSettings.SignalBw;			//信号带宽
	buf[4] = LoRaSettings.SpreadingFactor;	//扩频因子
	
 	crc_value = CalCrc16(buf,5,0xFFFF);
	buf[5] = crc_value>>8;
	buf[6] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//串口1DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel4,7);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//串口3DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel2,7);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//查询目标设备ID应答
void HostComInquireDestIdCmdACK(u8 *buf,u16 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x3C;
	
	buf[1] = 0x06>>8;
	buf[2] = 0x06;
	
	buf[3] = LoRaPact.DstDeviceAddr;	//接口类型

 	crc_value = CalCrc16(buf,4,0xFFFF);
	buf[4] = crc_value>>8;
	buf[5] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//串口1DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel4,6);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//串口3DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel2,6);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//注销指令应答
void HostComLogoutCmdACK(u8 *buf,u16 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x3B;
	
	buf[1] = 0x07>>8;
	buf[2] = 0x07;
	
	buf[3] = 'O';
	buf[4] = 'K';
	
 	crc_value = CalCrc16(buf,5,0xFFFF);
	buf[5] = crc_value>>8;
	buf[6] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//串口1DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel4,7);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//串口3DMA1发送
		DMA_SetCurrDataCounter(DMA1_Channel2,7);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


/***************************************************
	* 功能描述：注销指令处理
	* 入口参数：NowLogType -- 现在导出日志类型
	* 返回值：	无
	* 备注：	无
****************************************************/
void HandleLogoutCmd(u8 NowLogType)
{
	//结束导出日志
	ExportLogEnable = DISABLE;
	
	switch(NowLogType)
	{
		//当前导出操作日志
		case OperateLogFlag:
			OperateReadAddr = 0x00;
			memset(OperateLogBuf,0,MIN_LOG_PACK_SIZE);	//将日志缓冲区清零
		break;
		
		//当前导出系统日志
		case SystemLogFlag:
			SystemReadAddr = 0x00;
			memset(SystemLogBuf,0,MIN_LOG_PACK_SIZE);	//将日志缓冲区清零
		break;		
		
		//当前导出通信日志
		case CommunicateLogFlag:
			CommunicateReadAddr = 0x00;
			memset(CommunicateLogBuf,0,MAX_LOG_PACK_SIZE);	//将日志缓冲区清零
		break;
		
		//当前导出加密日志
		case EncryptFlag:
			EncryptReadAddr = 0x00;
			memset(EncryptLogBuf,0,MAX_LOG_PACK_SIZE);	//将日志缓冲区清零
		break;
		
		default:
		break;
	}
	HostComLogoutCmdACK(hostcom_struct.Comx_TxBuf,0);
}



