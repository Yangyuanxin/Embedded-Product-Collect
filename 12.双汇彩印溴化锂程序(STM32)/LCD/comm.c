#define _COMM_C

#include "head.h"

/*
*******************************************************************************
函 数 名：	UINT8 CheckXor(UINT8 *source,UINT8 length)
功    能：	异或校验
入口参数：	UINT8 *source	:源数据位置
			UINT8 length	:数据长度
出口参数：
编写日期：	2005.11.20
作    者：	郭辉
*******************************************************************************
*/
UINT8 CheckXor(UINT8 *source,UINT8 length)
{

	UINT8 value = 0,i;
	
	for(i=0; i<length; i++)
	{
		value ^= (*source);
		source++;
	}
	
	return(value);
}

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
UINT8 CheckSum(UINT8 *source,UINT16 len)
{
	UINT8 sum;
	UINT16 i;
	sum = 0;

	for (i=0; i<len; i++)
	{
		sum += *(source + i);
	}

	return sum;
}



/*

*/
void PrintfFormat(INT8 *p_des,INT32 value, UINT8 length)
{
	if (length == 0)
	{
		*p_des = '\0';
	}
	//
	switch(length)
   	{
   		case 1:
   			sprintf(p_des,"%01d",value);
   			break;
   		case 2:
   			sprintf(p_des,"%02d",value);
   			break;
   		case 3:
   			sprintf(p_des,"%03d",value);
   			break;
   		case 4:
   			sprintf(p_des,"%04d",value);
  			break;
   		case 5:
   			sprintf(p_des,"%05d",value);
   			break;
   		case 6:
   			sprintf(p_des,"%06d",value);
   			break;
   		case 7:
   			sprintf(p_des,"%07d",value);
   			break;
   		case 8:
   			sprintf(p_des,"%08d",value);
   			break;
   		case 9:
   			sprintf(p_des,"%09d",value);
   			break;
   		case 10:
   			sprintf(p_des,"%010d",value);
   			break;
   		case 11:
   			sprintf(p_des,"%011d",value);
   			break;
   		case 12:
   			sprintf(p_des,"%012d",value);
   			break;   
   		default:   			
   			sprintf(p_des,"打印长度超限");
   			break; 				
   	}
   	
}

/*
得到列表单的表单项数
*/

UINT8 GetListCount(INT8 **scr_list)
{
	UINT8 i;
	i = 0;
	while (*scr_list++)
	{
		i++;
	}
	return i;
}