#ifndef __GPIO_H__
#define __GPIO_H__

#include "delay.h"
#include "sys.h"

//#define RESET (1<<4)
//#define SCL   (1<<5)
//#define MOSI  (1<<6)
//#define WR    (1<<7)
//#define CS    (1<<8)


#define P0  0                      //引脚编号宏定义；
#define P1  1
#define P2  2
#define P3  3
#define P4  4
#define P5  5
#define P6  6
#define P7  7
#define P8  8
#define P9  9
#define P10 10
#define P11 11
#define P12 12
#define P13 13
#define P14 14
#define P15 15

//IO时钟使能位
#define IOPAEN 0                      //端口时钟位宏定义；
#define IOPBEN 1
#define IOPCEN 2
#define IOPDEN 3
#define IOPEEN 4
#define IOPFEN 5
#define IOPGEN 6
#define IOPHEN 7
#define IOPIEN 8


//IO模式
#define IO_MODE_OUT  0x01		//输出模式
#define IO_MODE_IN   0x00		//输入模式
#define IO_MODE_TWO  0x02		//服用功能
#define IO_MODE_ANA	 0x03		//模拟输入

//IO输出速度
#define IO_SPEED_2M 	0x00				//2MHz
#define IO_SPEED_25M    0x1                //25M输出；
#define IO_SPEED_50M    0x2                //50M输出；
#define IO_SPEED_100M   0x3                //100M输出；
//IO推挽或者开漏
#define IO_OTYPER_PP 	0			//推挽
#define IO_OTYPER_OD 	1			//开漏

//IO状态
#define IO_NOT		0x00		//无上/下拉
#define IO_PULL		0X01		//上拉
#define IO_DOWN		0X02		//下拉
#define IO_KEEP		0X03		//保留

//IO电平
#define IO_H      1                   //输出高；
#define IO_L      0                   //输出低；


/*
* 函数名称：GPIO_Init；
* 功能说明：GPIO初始化；
* 入口参数：IO：哪个端口
*           PORT_NUM：端口位子
*           MODE：端口模式
*           SPEED：端口速度
*			PUPD：上拉或者下拉
*           IO_DR：端口电平
* 返回值  ：无；
* 备注    ：无；
*/
void GPIO_Init(GPIO_TypeDef * IO,u8 PORT_NUM,u8 MODE,u8 SPEED,u8 OTY,u8 PUPD,u8 IO_DR);

/*
* 函数名称：GPIO_Clock_Set；
* 功能说明：GPIO时钟使能；
* 入口参数：IOPxEN->IOPAEN--IOPEEN,端口时钟；
*           EN_PULL_OR_OUT-->ODR设置；
* 返回值  ：无；
* 备注    ：无；
*/
void GPIO_Clock_Set(u8 IOPxEN);



#endif

