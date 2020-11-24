#ifndef __LCD12864_H__
#define __LCD12864_H__
#include"sys.h"
#include"delay.h"


#define ___SetWhite__

#define LCD_SDA  PAout(1)
#define LCD_SCL  PAout(0)
#define LCD_CS  PAout(2)


//------------初始化函数----------//
void Lcd12864_Init(void);
#ifdef ___SetWhite__
void Lcd12864_SetWhite_Init(void);
#endif


//--------------基本操作函数--------//
void Lcd12864_WriCmdDat(u8 start,u8 dataa);    /********写命令/写数据*********/
	//[start] -----> 0 写命令   1 写数据
void Lcd12864_WriAdd(u8 x,u8 y);
	/*  ----->> x 写数据所在行，取值为：0 1 2 3
		----->> y 写数据所在列，取值为：0-7
	*/
void Lcd12864_PriAsc(u8 ascii);
	/* ------>>ascii 需打印的ascii字符*/
void Lcd12864_PriStr(char *p);
	/* ------>>ascii 需打印的字符串*/
	
#ifdef ___SetWhite__
void Lcd12864_SetWhite(u8 y,u8 x,u8 endx);
	/*----->  y   需反白的行号 【0，1，2，3】
	  ----->  x   需反白的起始列地址【0---7】
	  ----->  endx需反白的终止列地址【0---7】 */
	 
void Lcd12864_ClrWhite(u8  y,u8 x,u8 endx);
	/*----->  y   需清楚反白的行号 【0，1，2，3】
	  ----->  x   需清除反白的起始列地址【0---7】
	  ----->  endx需清除反白的终止列地址【0---7】 */
#endif	
	
	
//--------------辅助函数----------//
void Lcd12864_SendByte(u8 byte);    //写位操作
void LCD12864_PortInit(void);  //端口初始化函数
#endif
