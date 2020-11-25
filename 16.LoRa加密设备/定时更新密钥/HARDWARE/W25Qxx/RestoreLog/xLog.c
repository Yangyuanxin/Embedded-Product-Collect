#include "xLog.h"

void 	MixOpreateLogData(u8 *_OperateLogBuf,u16 _RestoreCode,u8 AdminCode,u8 _result);
void 	MixSystemLogData(u8 *_SystemLogBuf,u16 _RestoreCode,u8 _result);
void 	MixCommunicateLogData(u8 *_CommunicateLogBuf,u8 _result);
void 	MixEncryptLogData(u8 *_EncryptLogBuf,u8 _result);

/****************************************************************************************
										变量区
*****************************************************************************************/
	extern tLoRaSettings LoRaSettings;
	//-------------------------------------------------------------
	u8 	OperateLogBuf[16] = {0,};			//操作日志缓冲区
	u8 	SystemLogBuf[16] = {0,};			//系统日志缓冲区
	u8 	CommunicateLogBuf[1100] = {0,};		//通信日志缓冲区
	u8  EncryptLogBuf[1100] = {0,};			//加密日志缓冲区
	//-------------------------------------------------------------
	u32 OperateWriteAddr = 0x00;			//写入操作日志偏移地址
	u32	OperateReadAddr = 0x00;				//读取操作日志偏移地址
	u32 SystemWriteAddr = 0x00;				//写入系统日志偏移地址
	u32	SystemReadAddr = 0x00;				//读取系统日志偏移地址
	u32 CommunicateWriteAddr = 0x00;		//写入通信日志偏移地址
	u32	CommunicateReadAddr = 0x00;			//读取通信日志偏移地址
	u32 EncryptWriteAddr = 0x00;			//写入加密日志偏移地址
	u32	EncryptReadAddr = 0x00;				//读取加密日志偏移地址
/****************************************************************************************/

	
/*********************************************************
	* 功能描述：存储日志
	* 入口参数：LogType		--	日志类型
				SourceBuf	--	源数据缓冲区地址
				RestoreCode	--	存储类型代码
				AdminCode	-- 	管理员代码
				Result		-- 	结果
	* 返回值：	无
	* 备注：	
**********************************************************/
void RestoreWhichLog(u8 LogType,u8 *SourceBuf,u16 RestoreCode,u8 AdminCode,u8 Result )
{
	switch(LogType)
	{
		//---->存储操作日志(主体为操作人员)
		case OperateLogFlag:
		//判断是否超过存储地址
		if( (OPERATE_LOG_BASE_ADDR+OperateWriteAddr) < OPERATE_LOG_SZIE )
		{
			//合成操作日志
			MixOpreateLogData(SourceBuf,RestoreCode,AdminCode,Result);
			//将操作日志写入存芯片
			W25QXX_Write(SourceBuf,OPERATE_LOG_BASE_ADDR+OperateWriteAddr,MIN_LOG_PACK_SIZE);
			//地址偏移
			OperateWriteAddr += MIN_LOG_PACK_SIZE;
			memset(SourceBuf,0,MIN_LOG_PACK_SIZE);		

			//保存操作日志写入地址
			RestoreWriteAddr(LogWriteAddr_Operate);
		}
		else
			//将偏移地址重置
			OperateWriteAddr = 0x00;				
		break;
		
		//---->存储系统日志(主体为设备)
		case SystemLogFlag:
			//判断是否超过存储地址
			if( (SYSTEM_LOG_BASE_ADDR+SystemWriteAddr) < SYSTEM_LOG_SZIE )
			{
				//合成系统日志
				MixSystemLogData(SourceBuf,RestoreCode,Result);
				//将系统日志写入存储芯片
				W25QXX_Write(SourceBuf,SYSTEM_LOG_BASE_ADDR+SystemWriteAddr,MIN_LOG_PACK_SIZE);
				//地址偏移
				SystemWriteAddr += MIN_LOG_PACK_SIZE;	
				memset(SourceBuf,0,MIN_LOG_PACK_SIZE);

				//存储系统日志写入地址
				RestoreWriteAddr(LogWriteAddr_System);
			}
			else
				//将偏移地址重置
				SystemWriteAddr = 0x00;
		break;
		
		//---->存储通信日志
		case CommunicateLogFlag:
			//判断是否超过存储地址
			if( (COMMUNICATE_LOG_BASE_ADDR+CommunicateWriteAddr) < COMMUNICATE_LOG_SIZE )
			{
				//合成通信日志
				MixCommunicateLogData(SourceBuf,Result);
				//将通信日志写入存储芯片
				W25QXX_Write(SourceBuf,COMMUNICATE_LOG_BASE_ADDR+CommunicateWriteAddr,MAX_LOG_PACK_SIZE);
				//地址偏移
				CommunicateWriteAddr += MAX_LOG_PACK_SIZE;	
				memset(SourceBuf,0,MAX_LOG_PACK_SIZE);

				//保存通信日志写入地址
				RestoreWriteAddr(LogWriteAddr_Communicate);
			}
			else
				//将偏移地址重置
				CommunicateWriteAddr = 0x00;				
		break;
		
		//---->存储加密日志
		case EncryptFlag:
			//判断是否超过存储地址
			if( (ENCRYPT_LOG_BASE_ADDR+EncryptWriteAddr) < ENCRYPT_LOG_SZIE )
			{
				//合成加密日志
				MixEncryptLogData(SourceBuf,Result);	
				//将加密日志写入存储芯片
				W25QXX_Write(SourceBuf,ENCRYPT_LOG_BASE_ADDR+EncryptWriteAddr,MAX_LOG_PACK_SIZE);
				//地址偏移
				EncryptWriteAddr += MAX_LOG_PACK_SIZE;	
				memset(SourceBuf,0,MAX_LOG_PACK_SIZE);

				//保存加密日志写入地址
				RestoreWriteAddr(LogWriteAddr_Encrypt);	
			}
			else
				//将偏移地址重置
				EncryptWriteAddr = 0x00;	
		break;
		
		default:
		break;
	}
}


/*********************************************************
	* 功能描述：导出日志
	* 入口参数：LogType -- 日志类型
				DestBuf -- 导出的数据缓冲区地址
	* 返回值：	无
	* 备注：	
**********************************************************/
void ExportWhichLog(u8 LogType,u8 *DestBuf)
{
	switch(LogType)
	{
		//---->操作日志
		case OperateLogFlag:
			//读取W25Qxx数据
			W25QXX_Read(DestBuf,OPERATE_LOG_BASE_ADDR+OperateReadAddr,MIN_LOG_PACK_SIZE);
			//偏移地址		
			OperateReadAddr += MIN_LOG_PACK_SIZE; 											
		break;
		
		//---->系统日志
		case SystemLogFlag:
			//读取W25Qxx数据
			W25QXX_Read(DestBuf,SYSTEM_LOG_BASE_ADDR+SystemReadAddr,MIN_LOG_PACK_SIZE);	
			//偏移地址
			SystemReadAddr += MIN_LOG_PACK_SIZE; 										
		break;
		
		//---->通信日志
		case CommunicateLogFlag:
			//读取W25Qxx数据
			W25QXX_Read(DestBuf,COMMUNICATE_LOG_BASE_ADDR+CommunicateReadAddr,MAX_LOG_PACK_SIZE);	
			//地址偏移
			CommunicateReadAddr += MAX_LOG_PACK_SIZE;
		break;
		
		//---->加密日志
		case EncryptFlag:
			//读取W25Qxx数据
			W25QXX_Read(DestBuf,ENCRYPT_LOG_BASE_ADDR+EncryptReadAddr,MAX_LOG_PACK_SIZE);	
			//地址偏移
			EncryptReadAddr += MAX_LOG_PACK_SIZE;
		break;
		
		default:
			
		break;
	}
}


/**********************************************************
	* 功能描述：合成操作日志数据
	* 入口参数：OperateLogBuf 	-- 存储操作日志缓冲区
				_result 		-- 操作结果
	* 返回值：	无
	* 备注：	
***********************************************************/
void MixOpreateLogData(u8 *_OperateLogBuf,u16 _RestoreCode,u8 AdminCode,u8 _result)
{
	u16 my_year = 0x00;
	
	my_year = calendar.w_year;

	_OperateLogBuf[0]  = my_year>>8;		//时间
	_OperateLogBuf[1]  = my_year;
	_OperateLogBuf[2]  = calendar.w_month;
	_OperateLogBuf[3]  = calendar.w_date;
	_OperateLogBuf[4]  = calendar.hour;
	_OperateLogBuf[5]  = calendar.min;
	_OperateLogBuf[6]  = calendar.sec;
	_OperateLogBuf[7]  = AdminCode;			//管理员代码
	_OperateLogBuf[8]  = _RestoreCode >>8;	//操作码高8位
	_OperateLogBuf[9]  = _RestoreCode;		//操作码低8位
	_OperateLogBuf[10] = _result;			//操作结果
}


/**********************************************************
	* 功能描述：合成系统日志数据
	* 入口参数：OperateLogBuf 	-- 存储系统作日志缓冲区
				_result 		-- 操作结果
	* 返回值：	无
	* 备注：	
***********************************************************/
void MixSystemLogData(u8 *_SystemLogBuf,u16 _RestoreCode,u8 _result)
{
	_SystemLogBuf[0]  = calendar.w_year>>8;
	_SystemLogBuf[1]  = calendar.w_year;
	_SystemLogBuf[2]  = calendar.w_month;
	_SystemLogBuf[3]  = calendar.w_date;
	_SystemLogBuf[4]  = calendar.hour;
	_SystemLogBuf[5]  = calendar.min;
	_SystemLogBuf[6]  = calendar.sec;
	_SystemLogBuf[7]  = _RestoreCode>>8;
	_SystemLogBuf[8]  = _RestoreCode; 
	_SystemLogBuf[9]  = _result;
}


/**********************************************************
	* 功能描述：合成通信日志数据
	* 入口参数：OperateLogBuf 	-- 存储通信日志缓冲区
				_result 		-- 操作结果
	* 返回值：	无
	* 备注：	
***********************************************************/
void MixCommunicateLogData(u8 *_CommunicateLogBuf,u8 _result)
{
	_CommunicateLogBuf[0]  = calendar.w_year>>8;				//时间
	_CommunicateLogBuf[1]  = calendar.w_year;
	_CommunicateLogBuf[2]  = calendar.w_month;
	_CommunicateLogBuf[3]  = calendar.w_date;
	_CommunicateLogBuf[4]  = calendar.hour;
	_CommunicateLogBuf[5]  = calendar.min;
	_CommunicateLogBuf[6]  = calendar.sec;
	
	_CommunicateLogBuf[7]  = PackSumSize>>8;					//数据包长度
	_CommunicateLogBuf[8]  = PackSumSize;
	
	memcpy(&_CommunicateLogBuf[9],EncryptedData,PackSumSize);	//通信内容
	
	_CommunicateLogBuf[PackSumSize+9]  = _result;				//通信结果
}


/**********************************************************
	* 功能描述：合成加密日志数据
	* 入口参数：OperateLogBuf 	-- 存储加密日志缓冲区
				_result 		-- 操作结果
	* 返回值：	无
	* 备注：	
***********************************************************/
void MixEncryptLogData(u8 *_EncryptLogBuf,u8 _result)
{
	u16 _year = 0x00;
	
	_year = calendar.w_year;								//时间
	_EncryptLogBuf[0]  = _year>>8;
	_EncryptLogBuf[1]  = _year;
	_EncryptLogBuf[2]  = calendar.w_month;
	_EncryptLogBuf[3]  = calendar.w_date;
	_EncryptLogBuf[4]  = calendar.hour;
	_EncryptLogBuf[5]  = calendar.min;
	_EncryptLogBuf[6]  = calendar.sec;
	
	_EncryptLogBuf[7] = PackSumSize>>8;						//数据包长度			
	_EncryptLogBuf[8] = PackSumSize;
	
	_EncryptLogBuf[9]  = ENCRYPT_DATA;						//加密标志
	
	memcpy(&_EncryptLogBuf[10],EncryptedData,PackSumSize);	//加密内容
	
	_EncryptLogBuf[PackSumSize+10]  = _result;				//加密结果
	
}

/**************************************************************
	* 功能描述：保存用户密码
	* 入口参数：
***************************************************************/
void RestoreUserPassword(u8 *PwdBuf, u8 PwdSize,u32 where_addr)
{
	u8 temp_buf[12] = {0,};
	
	memcpy(temp_buf,PwdBuf,PwdSize);
	temp_buf[11] = SETED_USER_INFO;
	W25QXX_Write(temp_buf,where_addr,12);
}


/**************************************************
	* 功能描述：动态存储写入日志地址
	* 入口参数：写入日志地址类型
	* 返回值：	无
	* 备注：	无
***************************************************/
void RestoreWriteAddr(u8 WhichWriteAddr)
{
	switch(WhichWriteAddr)
	{
		case LogWriteAddr_Operate:
			BKP_WriteBackupRegister(BKP_DR3,OperateWriteAddr>>16);
			BKP_WriteBackupRegister(BKP_DR4,OperateWriteAddr);
		break;
		
		case LogWriteAddr_System:
			BKP_WriteBackupRegister(BKP_DR5,SystemWriteAddr>>16);
			BKP_WriteBackupRegister(BKP_DR6,SystemWriteAddr);
		break;
		
		case LogWriteAddr_Communicate:
			BKP_WriteBackupRegister(BKP_DR7,CommunicateWriteAddr>>16);
			BKP_WriteBackupRegister(BKP_DR8,CommunicateWriteAddr);
		break;
		
		case LogWriteAddr_Encrypt:
			BKP_WriteBackupRegister(BKP_DR9, EncryptWriteAddr>>16);
			BKP_WriteBackupRegister(BKP_DR10,EncryptWriteAddr);
		break;
		
		default:
		break;
	}
}


/******************************************************
	* 功能描述：设备上电加载上一次的日志写入地址
	* 入口参数：无
	* 返回值：	无
	* 备注：	
*******************************************************/
void LoadWriteAddrPowerOnDevice(void)
{
	//加载上一次地址
	OperateWriteAddr 	 = BKP_ReadBackupRegister(BKP_DR3) <<16 | BKP_ReadBackupRegister(BKP_DR4);
	SystemWriteAddr 	 = BKP_ReadBackupRegister(BKP_DR5) <<16 | BKP_ReadBackupRegister(BKP_DR6);
	CommunicateWriteAddr = BKP_ReadBackupRegister(BKP_DR7) <<16 | BKP_ReadBackupRegister(BKP_DR8);
	EncryptWriteAddr 	 = BKP_ReadBackupRegister(BKP_DR9) <<16 | BKP_ReadBackupRegister(BKP_DR10);
}




