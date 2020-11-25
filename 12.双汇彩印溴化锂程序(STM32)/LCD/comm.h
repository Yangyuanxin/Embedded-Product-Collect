#ifndef _COMM_H
#define _COMM_H

#ifndef _COMM_C
#define EXT_COMM_C extern
#else
#define EXT_COMM_C
#endif

/*
*******************************************************************************
函 数 名：	UINT8 CheckXor(UINT8 *source,UINT8 length)
功    能：	异或校验
入口参数：	UINT16 *source	:源数据位置
			UINT8 length	:数据长度
出口参数：
编写日期：	2005.11.20
作    者：	郭辉
*******************************************************************************
*/
EXT_COMM_C UINT8 CheckXor(UINT8 *source,UINT8 length);

/*
**************************************************************************
* 函 数 名：UINT8 CheckSum(UINT8 *source,UINT16 len)
* 功    能：计算校验和
* 说    明：

* 入口参数：数据源(source),数据长度(len)
* 出口参数：返回校验和
* 编写日期：2006.2
* 作    者： 
**************************************************************************
*/
EXT_COMM_C UINT8 CheckSum(UINT8 *source,UINT16 len);

/***********************************************************
* 函数名：static void pop_one_byte(UINT8 scr,UINT8 length)
* 功能：挤出一个字节
***********************************************************/


/*
***********************************************************
* 函 数 名：UINT16 CheckCrc(UINT8 *source,UINT8 length)
* 功    能：完成CRC校验
* 说    明：

* 入口参数：UINT8 *source	源数据位置
			UINT8 length	数据长度
* 出口参数：返回校验值
* 编写日期：2006.3
* 作    者：SUNYUAN
***********************************************************
*/

 
 
EXT_COMM_C void PrintfFormat(INT8 *p_des,INT32 value, UINT8 length);

EXT_COMM_C UINT8 GetListCount(INT8 **scr_list);
#endif