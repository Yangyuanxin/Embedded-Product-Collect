/*
*********************************************************************************************************
*
*	模块名称 : MODSBUS通信程序 （主机）
*	文件名称 : modbus_host.c
*	版    本 : V1.0
*	说    明 : 
*	修改记录 :
*		版本号  日期        作者    说明
*       V1.0    2020-12-15  armfly  首版，用于H7-TOOL. 只做了框架，还未应用. 用到时再完善.
*
*	Copyright (C), 2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "param.h"
#include "main.h"
#include "modbus_host.h"


MODH_T g_tModH;
uint8_t g_modh_timeout = 0;

static void MODH_Read_01H(void);
static void MODH_Read_02H(void);
static void MODH_Read_03H(void);
static void MODH_Read_04H(void);
static void MODH_Read_05H(void);
static void MODH_Read_06H(void);
static void MODH_Read_10H(void);

/*
*********************************************************************************************************
*	函 数 名: MODH_SendWithCRC
*	功能说明: 发送应答,自动加CRC.  
*	形    参: 无。发送数据在 g_tModH.TxBuf[], [g_tModH.TxCount
*	返 回 值: 无
*********************************************************************************************************
*/
void MODH_SendWithCRC(void)
{
	uint16_t crc;
	
	crc = CRC16_Modbus(g_tModH.TxBuf, g_tModH.TxCount);
	g_tModH.TxBuf[g_tModH.TxCount++] = crc >> 8;
	g_tModH.TxBuf[g_tModH.TxCount++] = crc;	
}

/*
*********************************************************************************************************
*	函 数 名: MODH_AnalyzeApp
*	功能说明: 分析应用层协议。处理应答。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t MODH_Poll(uint8_t *_buf, uint16_t _len)
{
    g_tModH.RxBuf = _buf;
    g_tModH.RxCount = _len;

	switch (g_tModH.RxBuf[1])			/* 第2个字节 功能码 */
	{
		case 0x01:	/* 读取线圈状态（对应远程开关D01/D02/D03） */
			MODH_Read_01H();
			break;

		case 0x02:	/* 读取输入状态（对应X0～X7） */
			MODH_Read_02H();
			break;

		case 0x03:	/* 读取保持寄存器 在一个或多个保持寄存器中取得当前的二进制值 */
			MODH_Read_03H();
			break;

		case 0x04:	/* 读取输入寄存器（对应A01/A02） ） */
			MODH_Read_04H();
			break;

		case 0x05:	/* 强制单线圈（对应Y1/Y2） */
			MODH_Read_05H();
			break;

		case 0x06:	/* 写单个寄存器  */
			MODH_Read_06H();
			break;		

		case 0x10:	/* 写多个寄存器  */
			MODH_Read_10H();
			break;
		
		default:
			break;
	}
    
    return 1;
}

/*
*********************************************************************************************************
*	函 数 名: MODH_Send10H
*	功能说明: 发送10H指令，连续写多个保持寄存器. 最多一次支持23个寄存器。
*	形    参: _addr : 从站地址
*			  _reg : 寄存器编号
*			  _num : 寄存器个数n (每个寄存器2个字节) 值域
*			  _buf : n个寄存器的数据。长度 = 2 * n
*	返 回 值: 无
*********************************************************************************************************
*/
void MODH_Send10H(uint8_t _addr, uint16_t _reg, uint8_t _num, uint8_t *_buf)
{
	uint16_t i;
	
	g_tModH.TxCount = 0;
	g_tModH.TxBuf[g_tModH.TxCount++] = _addr;		/* 从站地址 */
	g_tModH.TxBuf[g_tModH.TxCount++] = 0x10;		/* 从站地址 */	
	g_tModH.TxBuf[g_tModH.TxCount++] = _reg >> 8;	/* 寄存器编号 高字节 */
	g_tModH.TxBuf[g_tModH.TxCount++] = _reg;		/* 寄存器编号 低字节 */
	g_tModH.TxBuf[g_tModH.TxCount++] = _num >> 8;	/* 寄存器个数 高字节 */
	g_tModH.TxBuf[g_tModH.TxCount++] = _num;		/* 寄存器个数 低字节 */
	g_tModH.TxBuf[g_tModH.TxCount++] = 2 * _num;	/* 数据字节数 */
	
	for (i = 0; i < 2 * _num; i++)
	{
		if (g_tModH.TxCount > H_RX_BUF_SIZE - 3)
		{
			return;		/* 数据超过缓冲区超度，直接丢弃不发送 */
		}
		g_tModH.TxBuf[g_tModH.TxCount++] = _buf[i];		/* 后面的数据长度 */
	}
	
	MODH_SendWithCRC();	/* 发送数据，自动加CRC */
}

/*
*********************************************************************************************************
*	函 数 名: MODH_Send05H
*	功能说明: 发送05H指令，写强置单线圈
*	形    参: _addr : 从站地址
*			  _reg : 寄存器编号
*			  _value : 寄存器值,2字节
*	返 回 值: 无
*********************************************************************************************************
*/
void MODH_Send05H(uint8_t _addr, uint16_t _reg, uint16_t _value)
{
	g_tModH.TxCount = 0;
	g_tModH.TxBuf[g_tModH.TxCount++] = _addr;			/* 从站地址 */
	g_tModH.TxBuf[g_tModH.TxCount++] = 0x05;			/* 功能码 */	
	g_tModH.TxBuf[g_tModH.TxCount++] = _reg >> 8;		/* 寄存器编号 高字节 */
	g_tModH.TxBuf[g_tModH.TxCount++] = _reg;			/* 寄存器编号 低字节 */
	g_tModH.TxBuf[g_tModH.TxCount++] = _value >> 8;	/* 寄存器值 高字节 */
	g_tModH.TxBuf[g_tModH.TxCount++] = _value;		/* 寄存器值 低字节 */
	
	MODH_SendWithCRC();	/* 发送数据，自动加CRC */

	g_tModH.fAck05H = 0;		/* 如果收到从机的应答，则这个标志会设为1 */
}

/*
*********************************************************************************************************
*	函 数 名: MODH_Send06H
*	功能说明: 发送06H指令，写1个保持寄存器
*	形    参: _addr : 从站地址
*			  _reg : 寄存器编号
*			  _value : 寄存器值,2字节
*	返 回 值: 无
*********************************************************************************************************
*/
void MODH_Send06H(uint8_t _addr, uint16_t _reg, uint16_t _value)
{
	g_tModH.TxCount = 0;
	g_tModH.TxBuf[g_tModH.TxCount++] = _addr;			/* 从站地址 */
	g_tModH.TxBuf[g_tModH.TxCount++] = 0x06;			/* 功能码 */	
	g_tModH.TxBuf[g_tModH.TxCount++] = _reg >> 8;		/* 寄存器编号 高字节 */
	g_tModH.TxBuf[g_tModH.TxCount++] = _reg;			/* 寄存器编号 低字节 */
	g_tModH.TxBuf[g_tModH.TxCount++] = _value >> 8;	/* 寄存器值 高字节 */
	g_tModH.TxBuf[g_tModH.TxCount++] = _value;		/* 寄存器值 低字节 */
	
	MODH_SendWithCRC();	/* 发送数据，自动加CRC */
	
	g_tModH.fAck06H = 0;		/* 如果收到从机的应答，则这个标志会设为1 */
}

/*
*********************************************************************************************************
*	函 数 名: MODH_Send03H
*	功能说明: 发送03H指令，查询1个或多个保持寄存器
*	形    参: _addr : 从站地址
*			  _reg : 寄存器编号
*			  _num : 寄存器个数
*	返 回 值: 无
*********************************************************************************************************
*/
void MODH_Send03H(uint8_t _addr, uint16_t _reg, uint16_t _num)
{
	g_tModH.TxCount = 0;
	g_tModH.TxBuf[g_tModH.TxCount++] = _addr;		/* 从站地址 */
	g_tModH.TxBuf[g_tModH.TxCount++] = 0x03;		/* 功能码 */	
	g_tModH.TxBuf[g_tModH.TxCount++] = _reg >> 8;	/* 寄存器编号 高字节 */
	g_tModH.TxBuf[g_tModH.TxCount++] = _reg;		/* 寄存器编号 低字节 */
	g_tModH.TxBuf[g_tModH.TxCount++] = _num >> 8;	/* 寄存器个数 高字节 */
	g_tModH.TxBuf[g_tModH.TxCount++] = _num;		/* 寄存器个数 低字节 */
	
	MODH_SendWithCRC();	/* 发送数据，自动加CRC */
	g_tModH.fAck03H = 0;	/* 清接收标志 */
	g_tModH.RegNum = _num;	/* 寄存器个数 */
	g_tModH.Reg03H = _reg;		/* 保存03H指令中的寄存器地址，方便对应答数据进行分类 */	
}

static void MODH_Read_01H(void)
{
	;
}

static void MODH_Read_02H(void)
{
	;
}

static void MODH_Read_04H(void)
{
	;
}

static void MODH_Read_05H(void)
{

}

/*
*********************************************************************************************************
*	函 数 名: MODH_Read_06H
*	功能说明: 分析06H指令的应答数据
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODH_Read_06H(void)
{

}

/*
*********************************************************************************************************
*	函 数 名: MODH_Read_03H
*	功能说明: 分析03H指令的应答数据
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void MODH_Read_03H(void)
{

}

/*
*********************************************************************************************************
*	函 数 名: MODH_Read_10H
*	功能说明: 分析10H指令的应答数据
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void MODH_Read_10H(void)
{

}

/*
*********************************************************************************************************
*	函 数 名: MODH_WriteParam_06H
*	功能说明: 单个参数. 通过发送06H指令实现，发送之后，等待从机应答。循环10次写命令
*	形    参: 无
*	返 回 值: 1 表示成功。0 表示失败（通信超时或被拒绝）
*********************************************************************************************************
*/
uint8_t MODH_WriteParam_06H(uint16_t _reg, uint16_t _value)
{
	int32_t time1;
	uint8_t i;
	
	for (i = 0; i < 10; i++)
	{	
		MODH_Send06H (SlaveAddr, _reg, _value);
		time1 = bsp_GetRunTime();	/* 记录命令发送的时刻 */
		while (1)
		{
			bsp_Idle();
			
			/* 超时大于 150ms ，则认为异常,  100ms有时会超时，数据没有接收到，PC软件应答时间间隔几十到一百多浮动 */
			if (bsp_CheckRunTime(time1) > 150)		//100
			{
				break;
			}
			
			if (g_tModH.fAck06H > 0)
			{
				break;
			}
		}
		
		if (g_tModH.fAck06H > 0)
		{
			break;
		}
	}
	
	if (g_tModH.fAck06H == 0)
	{
		return 0;	/* 通信超时了 */
	}
	else
	{
		return 1;	/* 写入06H参数成功 */
	}
}

/*
*********************************************************************************************************
*	函 数 名: MODH_WriteParam_05H
*	功能说明: 单个参数. 通过发送05H指令实现，发送之后，等待从机应答。
*	形    参: 无
*	返 回 值: 1 表示成功。0 表示失败（通信超时或被拒绝）
*********************************************************************************************************
*/
uint8_t MODH_WriteParam_05H(uint16_t _reg, uint16_t _value)
{
	int32_t time1;
	uint8_t i;

	for (i = 0; i < 10; i++)
	{
		MODH_Send05H (SlaveAddr, _reg, _value);
		time1 = bsp_GetRunTime();	/* 记录命令发送的时刻 */
		while (1)
		{
			bsp_Idle();
			
			/* 超时大于 150ms ，则认为异常 */
			if (bsp_CheckRunTime(time1) > 150)		//100
			{
				break;	/* 通信超时了 */
			}
			
			if (g_tModH.fAck05H > 0)
			{
				break;
			}
		}
		
		if (g_tModH.fAck05H > 0)
		{
			break;
		}
	}
	
	if (g_tModH.fAck05H == 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/* 读一个寄存器 */
uint8_t MODH_ReadParam_03H(uint16_t _reg, uint16_t _num)
{
	int32_t time1;
	uint8_t i;
	
	for (i = 0; i < 10; i++)
	{
		MODH_Send03H (SlaveAddr, _reg, _num);
		time1 = bsp_GetRunTime();	/* 记录命令发送的时刻 */

		while (1)
		{
			bsp_Idle();
			
			/* 超时大于 150ms ，则认为异常 */
			if (bsp_CheckRunTime(time1) > 150)		//100
			{
				break;	/* 通信超时了 */
			}
			
			if (g_tModH.fAck03H > 0)
			{
				break;
			}
		}
		
		if (g_tModH.fAck03H > 0)
		{
			break;
		}
	}
	
	if (g_tModH.fAck03H == 0)
	{
		return 0;
	}
	else 
	{
		return 1;	/* 写入03H参数成功 */
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

