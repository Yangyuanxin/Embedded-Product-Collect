/*
完整的一组参数包格式：

word:		leng 数据总长度
//每种参数的结构，长度按各自的结构定义
byte,byte:	参数类别，备用
WORD:		leng 本组参数数据长度,包括参数类型及备用的2人字节
the leng byte:   本组参数

  ...
*/

#define _ALL_PARA_C

#include "head.h"

//----------------------------------------------------------------------
//线路A型参数
#define XL_A_PARA_LENG		XL_A_PARA_LENG_COMMPORT \
							+XL_A_PARA_LENG_DISPYX  \
							+XL_A_PARA_LENG_SOE  \
							+XL_A_PARA_LENG_YC  \
							+XL_A_PARA_LENG_KWH  \
							+XL_A_PARA_LENG_FIXED  \
							+XL_A_PARA_LENG_YK  \
							+XL_A_PARA_LENG_WORK  \
							+XL_A_PARA_LENG_PARA_MODULPARA  \
							+XL_A_PARA_LENG_PARA_YX \
							+XL_A_PARA_LENG_JIANKONG \
							+XL_A_PARA_LENG_GPRS  \
							+XL_A_PARA_LENG_SENSOR
//
#define XL_A_PARA_LENG_COMMPORT				0	
////YX参数块长度	
#define XL_A_PARA_LENG_DISPYX				4  \
                                            +7*4

#define XL_A_PARA_LENG_SOE					0
//YC参数块长度
#define XL_A_PARA_LENG_YC					4   \
                                            +10*4

#define XL_A_PARA_LENG_KWH					0
//定值块长度	
#define XL_A_PARA_LENG_FIXED				4 	  \
											+2+8  \
											+2+8  \
                                            +2+8  \
                                            +2+8+2+8  \
											+2+8  \
                                            +2+8+2+8     
                                                
                                                
                                                
                                                
#define XL_A_PARA_LENG_YK					0
#define XL_A_PARA_LENG_WORK				    4 \
                                            +2+8\
                                            +2+8\
                                            +2+8\
                                            +2+8\
                                            +2+8\
                                            +2+8\
                                            +2+8
                                                
//系数长度                                                
#define XL_A_PARA_LENG_PARA_MODULPARA		4 	  \
											+2+8  \
											+2+8  \
                                            +2+8 

#define XL_A_PARA_LENG_JIANKONG				4   \
                                            +2+3 \
											+2+3 \
											+2+3 \
											+2+3 
#define XL_A_PARA_LENG_GPRS                4   \
                                            +2+3 \
											+2+3 \
											+2+3 \
											+2+3

#define XL_A_PARA_LENG_SENSOR                4   \
                                            +2+3 \
											+2+3 \
											+2+3 \
											+2+3 \
                                            +2+3
							

#define XL_A_PARA_LENG_PARA_YX				0


//
const UINT8 all_para_xl_a[] = 
{
	XL_A_PARA_LENG,XL_A_PARA_LENG>>8,//总长度
	//SEGMENT------------------------------------
	//定值参数段
	PARA_TYPE_FIXED,0,
	XL_A_PARA_LENG_FIXED,XL_A_PARA_LENG_FIXED>>8,
	0x00,			//PH高限
	0x01,			// “PH高限”  定值组的定值个数
  //      名字  存放位置 单位(小数点)   最小值      最大值
	0x10, 0x00, 0x00, 0x01, 0x00, 0x00, 0x10, 0x27, 	//定值

	0x01,			//PH低限
	0x01,			// “PH低限”  定值组的定值个数
	0x10, 0x00, 0x01, 0x01, 0x00, 0x00, 0x10, 0x27, 	//定值

	0x02,			//ORP高限
	0x01,			// “ORP高限”  定值组的定值个数
	0x10, 0x00, 0x02, 0x00, 0x00, 0x00, 0x10, 0x27, 	//定值

	0x03,			//ORP低限
	0x01,			// “ORP低限”  定值组的定值个数
	0x10, 0x00, 0x03, 0x00, 0x00, 0x00, 0x88, 0x13, 	//定值

	0x04,			//温度高限
	0x01,//0x05,			// “温度高限”  定值组的定值个数
	0x10, 0x00, 0x04, 0x01, 0x00, 0x00, 0x88, 0x13, 	//定值


	0x05,			//温度低限
	0x01,//0x05,			// “温度低限”  定值组的定值个数
	0x10, 0x00, 0x05, 0x01, 0x00, 0x00, 0x88, 0x13, 	//定值

	0x06,			//电导高限
	0x01,			// “电导高限”  定值组的定值个数
	0x10, 0x00, 0x06, 0x01, 0xF8, 0x11, 0x18, 0x15, 	//定值
    
    0x07,			//电导低限
	0x01,			// “电导低限”  定值组的定值个数
	0x10, 0x00, 0x07, 0x01, 0xF8, 0x11, 0x18, 0x15, 	//定值
	


    //SEGMENT------------------------------------
    //系数参数
    PARA_TYPE_MODUL,0,
    XL_A_PARA_LENG_PARA_MODULPARA, XL_A_PARA_LENG_PARA_MODULPARA>>8,
    0x00,			//ORP系数
	0x01,			// “ORP系数”  定值组的定值个数
  //      名字  存放位置 单位(小数点)   最小值      最大值
	0x10, 0x00, 0x00, 0x01, 0x00, 0x00, 0x10, 0x27, 	//定值

	0x01,			//流量1系数
	0x01,			// “流量1系数”  定值组的定值个数
	0x10, 0x00, 0x01, 0x01, 0x00, 0x00, 0x10, 0x27, 	//定值

	0x02,			//流量2系数
	0x01,			// “流量1系数”  定值组的定值个数
	0x10, 0x00, 0x02, 0x01, 0x00, 0x00, 0x10, 0x27, 	//定值


	//SEGMENT------------------------------------
	//YC参数段
	PARA_TYPE_YC,0,
	XL_A_PARA_LENG_YC,XL_A_PARA_LENG_YC>>8,
	//遥测0的参数
	0x00,         //遥测的类型：Ua
	0x00,          //遥测的路数
	0x01,          //遥测的数据格式
	0x01,          //
	//遥测1的参数
	0x01,         //遥测的类型：Ub
	0x00,          //遥测的路数
	0x01,          //遥测的数据格式
	0x01,          //
    //遥测2的参数
	0x02,         //遥测的类型：Ua
	0x00,          //遥测的路数
	0x01,          //遥测的数据格式
	0x01,          //
	//遥测3的参数
	0x03,         //遥测的类型：Ub
	0x00,          //遥测的路数
	0x01,          //遥测的数据格式
	0x01,          //
    //遥测4的参数
	0x04,         //遥测的类型：Ua
	0x00,          //遥测的路数
	0x01,          //遥测的数据格式
	0x01,          //
	//遥测5的参数
	0x05,         //遥测的类型：Ub
	0x00,          //遥测的路数
	0x01,          //遥测的数据格式
	0x01,          //
    //遥测6的参数
	0x06,         //遥测的类型：Ua
	0x00,          //遥测的路数
	0x01,          //遥测的数据格式
	0x01,          //
	//遥测7的参数
	0x07,         //遥测的类型：Ub
	0x00,          //遥测的路数
	0x01,          //遥测的数据格式
	0x01,          //
    //遥测8的参数
	0x08,         //遥测的类型：Ua
	0x00,          //遥测的路数
	0x01,          //遥测的数据格式
	0x01,          //
	//遥测9的参数
	0x09,         //遥测的类型：Ub
	0x00,          //遥测的路数
	0x01,          //遥测的数据格式
	0x01,          //
        //

	//
	//SEGMENT------------------------------------
	//YX参数段
	PARA_TYPE_YX,0,
	XL_A_PARA_LENG_DISPYX,XL_A_PARA_LENG_DISPYX>>8,
	//遥信0的参数
	0x02,           //遥信0的遥信位置：0
	0x01,         //遥信的显示方式：合/分
	0x00,          //遥信的显示位置
	0x00,       //遥信的名称
	//遥信1的参数
	0x04,           //遥信1的遥信位置：1
	0x01,         //遥信的显示方式：合/分
	0x00,          //遥信的显示位置
	0x01,       //遥信的名称
	//遥信2的参数
	0x05,           //遥信0的遥信位置：0
	0x01,         //遥信的显示方式：合/分
	0x00,          //遥信的显示位置
	0x02,       //遥信的名称
	//遥信3的参数
	0x06,           //遥信1的遥信位置：1
	0x01,         //遥信的显示方式：合/分
	0x00,          //遥信的显示位置
	0x03,       //遥信的名称
	//遥信4的参数
	0x07,           //遥信0的遥信位置：0
	0x01,         //遥信的显示方式：合/分
	0x00,          //遥信的显示位置
	0x04,       //遥信的名称
	//遥信5的参数
	0x08,           //遥信1的遥信位置：1
	0x01,         //遥信的显示方式：合/分
	0x00,          //遥信的显示位置
	0x05,       //遥信的名称
	//遥信6的参数
	0x09,           //遥信0的遥信位置：0
	0x01,         //遥信的显示方式：合/分
	0x00,          //遥信的显示位置
	0x06,       //遥信的名称


    //SEGMENT------------------------------------
	//工作参数段
	PARA_TYPE_WORK,0,
	XL_A_PARA_LENG_WORK,XL_A_PARA_LENG_WORK>>8,
   	0x00,			//1#加药脉冲设置
	0x01,			// “1#加药脉冲设置”  定值组的定值个数
	0x10, 0x00, 0x00, 0x11, 0x00, 0x00, 0x10, 0x27, 	//定值
	
	0x01,			//2#加药脉冲设置
	0x01,			// “2#加药脉冲设置”  定值组的定值个数
	0x10, 0x00, 0x01, 0x11, 0x00, 0x00, 0x10, 0x27, 	//定值
	
	0x02,			//3#加药脉冲设置
	0x01,			// “3#加药脉冲设置”  定值组的定值个数
	0x10, 0x00, 0x02, 0x11, 0x00, 0x00, 0x10, 0x27, 	//定值
	
	0x03,			//4#加药脉冲设置
	0x01,			// “4#加药脉冲设置”  定值组的定值个数
	0x10, 0x00, 0x03, 0x11, 0x00, 0x00, 0x88, 0x13,  	//定值
    
    0x04,			//4#加药脉冲设置
	0x01,			// “4#加药脉冲设置”  定值组的定值个数
	0x10, 0x00, 0x04, 0x11, 0x00, 0x00, 0x88, 0x13,  	//定值
    
    0x05,			//4#加药脉冲设置
	0x01,			// “4#加药脉冲设置”  定值组的定值个数
	0x10, 0x00, 0x05, 0x11, 0x00, 0x00, 0x88, 0x13,  	//定值
    
    0x06,			//4#加药脉冲设置
	0x01,			// “4#加药脉冲设置”  定值组的定值个数
	0x10, 0x00, 0x06, 0x11, 0x00, 0x00, 0x88, 0x13,  	//定值

	//SEGMENT------------------------------------
	//监控参数段
	PARA_TYPE_JIANKONG,0,
	XL_A_PARA_LENG_JIANKONG,XL_A_PARA_LENG_JIANKONG>>8,
	//监控端口号
	0x00,         //端口名称
	0x01,          //遥测的路数
	0x10,0x00, 0x00,         //
	//监控端口波特率
	0x01,         //端口名称
	0x01,          //遥测的路数
	0x10,0x01, 0x01 ,        //
	//监控端口停止位
	0x02,         //端口名称
	0x01,          //遥测的路数
	0x10,0x02, 0x02 ,        //
	//监控端口校验
	0x03,         //端口名称
	0x01,          //遥测的路数
	0x10,0x03, 0x03 ,        //
	//SEGMENT------------------------------------
	//监控参数段
	PARA_TYPE_GPRS,0,
	XL_A_PARA_LENG_GPRS,XL_A_PARA_LENG_GPRS>>8,
	//监控端口号
	0x00,         //端口名称
	0x01,          //遥测的路数
	0x10,0x00, 0x00,         //
	//监控端口波特率
	0x01,         //端口名称
	0x01,          //遥测的路数
	0x10,0x01, 0x01 ,        //
	//监控端口停止位
	0x02,         //端口名称
	0x01,          //遥测的路数
	0x10,0x02, 0x02 ,        //
	//监控端口校验
	0x03,         //端口名称
	0x01,          //遥测的路数
	0x10,0x03, 0x03 ,        //
	//SEGMENT------------------------------------
	//传感器参数段
	PARA_TYPE_SENSOR,0,
	XL_A_PARA_LENG_SENSOR,XL_A_PARA_LENG_SENSOR>>8,
	//监控端口号
	0x00,         //端口名称
	0x01,          //遥测的路数
	0x10,0x00, 0x00,         //
	//监控端口波特率
	0x01,         //端口名称
	0x01,          //遥测的路数
	0x10,0x01, 0x01 ,        //
	//监控端口停止位
	0x02,         //端口名称
	0x01,          //遥测的路数
	0x10,0x02, 0x02 ,        //
	//监控端口校验
	0x03,         //端口名称
	0x01,          //遥测的路数
	0x10,0x03, 0x03 ,        //
	//监控端口校验
	0x04,         //端口名称
	0x01,          //遥测的路数
	0x10,0x04, 0x03 ,        //
/******
	//SEGMENT------------------------------------
	//通讯口参数段
    **********/
};
//----------------------------------------------------------------------
const UINT8 ctrl_para_xl_a[] = 
{
  //       YX        名称号  
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	//定值
    
	0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 	//定值
    
    0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 	//定值
    
	0x00, 0x00, 0x01, 0x03, 0x00, 0x00, 	//定值
    
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 	//定值
    
	0x00, 0x00, 0x01, 0x05, 0x00, 0x00, 	//定值
    
    0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 	//定值
    
	0x00, 0x00, 0x01, 0x07, 0x00, 0x00, 	//定值
    
    0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 	//定值
    
	0x00, 0x00, 0x01, 0x09, 0x00, 0x00, 	//定值
};


/*
参数初始化函数，主程序在召唤到装置类型后，要调用此函数进行参数指针初始化。
*/
void InitAllPara(void)
{
	INT32 data_length;
	INT32 segment_length;
	UINT8 *p;

	//先把指针置为NULL
	p_fix_para = NULL;			//定值参数指针	
	p_dispyx_para = NULL;
	p_soe_para = NULL;
	p_yc_para = NULL;
	p_kwh_para = NULL;
	p_jiayao_para = NULL;	
	p_adj_xs_para = NULL;
	p_yx_para = NULL;
	p_jiankong_para = NULL;
	p_gprs_para =NULL;
	p_sensor_para= NULL;	
	
	
	
	//开始指针对位

	p = (UINT8*)all_para_xl_a;
	
	//
	data_length = *(p+1);
	data_length <<= 8;
	data_length += *p;
	//
	segment_length = 0;
	p += 2;			//指向段首
	//
	while (((UINT32)p) < (((UINT32)all_para_xl_a)+data_length)-4)
	{
		p += segment_length;
	//	printf("peng\r\n");
		//
		switch (*p)
		{
			case PARA_TYPE_YX:
				p_yx_para = p;
				break;
			case PARA_TYPE_YC:
				p_yc_para = p;
				break;
			case PARA_TYPE_FIXED:
				p_fix_para = p;
				break;
			case PARA_TYPE_MODUL:
				p_adj_xs_para = p;
				break;
            case PARA_TYPE_WORK:
				p_jiayao_para = p;
				break;
			case PARA_TYPE_JIANKONG:
				p_jiankong_para = p;
				break;
			case PARA_TYPE_GPRS:
				p_gprs_para = p;
				break;
			case PARA_TYPE_SENSOR:
				p_sensor_para =p;
				break;				
		}
		//调整到下一段
		segment_length = *(p+3);
		segment_length <<= 8;
		segment_length += *(p+2);
	}
	
	
	
}