/******************************************************************************
 * CPU.H - CPU相关宏定义等
 *
 *
 * DESCRIPTION: -
 *
 * modification history
 * --------------------
 * 01a, 04/sep/2007, 梁炎昌 written
 * --------------------
 ******************************************************************************/
#include <stdio.h>
#include <REGX52.H>
#include <intrins.h>
#include <absacc.h>

//-----------------------------
#define _CONST_  code

//-----------------------------

#define U8  unsigned char
#define U16 unsigned int
#define U32 unsigned long
#define Bool bit

#define true 1
#define false 0
//STC AUXR Reg
sfr  AUXR = 0x8E;
//STC P4口定义
sfr P4   =  0xE8;

sbit P4_0 = 0xE8;
sbit P4_1 = 0xE9;
sbit P4_2 = 0xEA;
sbit P4_3 = 0xEB;
//-----------------------

#define OSC_48MHZ   true
//#define OSC_24MHZ   true

//2个LED端口定义   0驱动
sbit LED0   = P4^3;
sbit LED1   = P1^7;
//液晶控制线
sbit LCD_E     = P2^1;//P3^2;
sbit LCD_RW    = P2^2;//P2^5;
sbit LCD_RS    = P2^0;//P2^6;
#define LCD_Bus   P1//P0
//74HC138的片选
sbit CSA       = P2^3;
sbit CSB       = P2^4;
//74HC138数据
sbit LatchaData0   = P2^0;
sbit LatchaData1   = P2^1;
sbit LatchaData2   = P2^2;

//键盘
sbit Key0   =P3^1; //P1^3;
sbit Key1   =P3^2; //P1^4;
sbit Key2   =P3^3; //P1^5;
sbit Key3   =P3^4; //P1^6;

#define Key_Down     0x11
#define Key_Up       0x12
#define Key_Left     0x13
#define Key_Right    0x14

//------------------------------------
//3个LCD片选信号
//LCD CS A   (CSA = 0 CSB = 1) LatchData = 101

#define LCD_CSA();    {CSA = 1; CSB = 0;}

        
            

//------------------------------------
//LCD CS B   (CSA = 0 CSB = 1) LatchData = 110
#define LCD_CSB();	  {CSA = 0; CSB = 1;} 


//------------------------------------
//LCD CS C   (CSA = 0 CSB = 1) LatchData = 111
#define LCD_CSC();    {CSA = 1; CSB = 1;}


//LCD CS A   (CSA = 0 CSB = 0)
#define LCD_CS_None();    {\
    CSA = 0;          \
    CSB = 0;          \
}

/*#define LCD_CSA();    {\
    P2 = 0x0D;         \
}
#define LCD_CSB();    {\
    P2 = 0x0E;         \
}
#define LCD_CSC();    {\
    P2 = 0x0F;         \
}
#define LCD_CS_None();    {\
} */
//------------------------------------


