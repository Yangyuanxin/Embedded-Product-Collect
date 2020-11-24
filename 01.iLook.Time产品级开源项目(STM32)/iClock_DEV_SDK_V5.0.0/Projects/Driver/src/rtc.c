//------------------------------------------------------------------------------
//
// Project:     image recorder(图像记录盒)
// Designer:    PaulWang,***
// File:        rtc.c
// Date:        2008/6/4 create,...
//              2008/6/22 加入ds1302驱动
// Description: 实时时钟驱动 for DS1302
//              
//
// Copyright (c) 2008 U-Sky Ltd.  All rights reserved.
//
//------------------------------------------------------------------------------
#include "sys_includes.h"

/*--------------------------------------------------------------------------------------*/
/*积存器表
READ  WRITE     BIT7  BIT6  BIT5  BIT4  BIT3  BIT2  BIT1  BIT0    RANGE
81H   80H        CH   ---10senconds--   ------ seconds ------
83H   82H             ---10minutes---   ------ minutes ------
85H   84H       12/24   0   AM/PM Hour  ------- hour --------
87H   86H         0     0   -10 Date-   ------- date --------
89H   88H         0     0     0  month  ------- month -------   
8BH   8AH         0     0     0     0     0     0   -- day --
8DH   8CH       ------  10 year -----   ------- year --------
8FH   8EH         WP    0     0     0     0     0     0     0
91H   90H         TCS   TCS  TCS  TCS     DS    DS    DS    DS
C1H	  C0H
:
FDH   FCH    31 bytes RAM
*/
/*--------------------------------------------------------------------------------------*/
//define
#define RTC_RST_HI()        GPIO_SetBits(RTC_RST, RTC_RST_PIN)
#define RTC_RST_LOW()       GPIO_ResetBits(RTC_RST, RTC_RST_PIN)

#define RTC_CLOCK_HI()      GPIO_SetBits(RTC_CLK, RTC_CLK_PIN)
#define RTC_CLOCK_LOW()     GPIO_ResetBits(RTC_CLK, RTC_CLK_PIN)

#define RTC_DATA_HI()       GPIO_SetBits(RTC_DAT, RTC_DAT_PIN)
#define RTC_DATA_LOW()      GPIO_ResetBits(RTC_DAT, RTC_DAT_PIN)

#define RTC_DATA_RD()       GPIO_ReadInputDataBit(RTC_DAT,RTC_DAT_PIN)


tm rtc_tm;

//extern
void SetRegVal(uint8_t par,uint8_t val );
//public

void rtc_initial(void);
void GetTime(void);
//private

/*--------------------------------------------------------------------------------------*/
/*                              DS1302基础通讯函数                                      */
/*--------------------------------------------------------------------------------------*/

__inline void bus_delay()
{
  int i=4;
  
  while(i--);
}


static void DS1302_DataIn(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin   = RTC_DAT_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(RTC_DAT, &GPIO_InitStructure);
}

static void DS1302_DataOut(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin   = RTC_DAT_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(RTC_DAT, &GPIO_InitStructure);
}

static void DS1302_BusOpen(void)
{
  DS1302_DataOut();
  RTC_DATA_HI();
  //
  RTC_CLOCK_LOW();
  RTC_RST_LOW();
  bus_delay();
  RTC_RST_HI(); 
  bus_delay();
}

static void DS1302_BusClose(void)
{
  DS1302_DataOut();
  RTC_DATA_LOW();
  //
  RTC_CLOCK_LOW();
  RTC_RST_LOW();
  bus_delay();
}

static void DS1302_GetBytes(uint8_t cmd, uint8_t cnt, uint8_t *buff)
{ 
    unsigned char i,j;
    unsigned char tmp=cmd;
    
    DS1302_BusOpen();
    DS1302_DataOut();
    for(i=0; i<8; i++){
        if(tmp&0x01)  RTC_DATA_HI();
        else          RTC_DATA_LOW();
        tmp = tmp>>1;
        RTC_CLOCK_HI(); 
        bus_delay();
        RTC_CLOCK_LOW();
        bus_delay();
    }
    DS1302_DataIn();
    for(j=0; j<cnt; j++) {
        tmp = 0x0;
        for(i=0; i<8; i++) {
          tmp = tmp>>1;
          if(RTC_DATA_RD())
            tmp |= 0x80;
          RTC_CLOCK_HI();
          bus_delay();
          RTC_CLOCK_LOW();
          bus_delay();
        }
        *buff++ = tmp;
    }
    DS1302_BusClose();
}

static void DS1302_SendBytes(uint8_t cmd, uint8_t cnt, uint8_t *buff)
{
    uint8_t tmp = cmd;
    uint8_t i,j;
    
    DS1302_BusOpen();
    DS1302_DataOut();
    for(i=0; i<8; i++) {
        if(tmp&0x01)  RTC_DATA_HI();
        else    RTC_DATA_LOW();
        RTC_CLOCK_HI();
        bus_delay();
        RTC_CLOCK_LOW();
        bus_delay();
        tmp = tmp>>1;
    }
    for(j=0; j<cnt; j++) {
        tmp = *buff++;
        for(i=0; i<8; i++) {
            if(tmp&0x01)  RTC_DATA_HI();
            else    RTC_DATA_LOW();
            RTC_CLOCK_HI();
            bus_delay();
            RTC_CLOCK_LOW();
            bus_delay();
            tmp = tmp>>1;
        }
    }
    DS1302_BusClose();
}

/*-----------------------------读写DS1302的命令或控制积存器------------------------------*/
//Address/Command Bytes(bit7~bit0): 1,RAM/#CK,A4,A3,A2,A1,A0,RD/#W 
static void DS1302_WriteReg(unsigned char ucAddr, unsigned char ucDa)  //ucAddr: DS1302地址, ucData: 要写的数据
{
    uint8_t tmp = ucDa;
    
  DS1302_SendBytes(ucAddr,1,&tmp);
} 
 
static unsigned char DS1302_ReadReg(unsigned char ucAddr)  //读取DS1302某地址的数据
{
    unsigned char tmp;

    DS1302_GetBytes(ucAddr|0x01,1,&tmp);
    return(tmp);
}
/*--------------------------------------------------------------------------------------*/
uint8_t BcdToInt(uint8_t bcd)
{
    return (0xff & (bcd>>4))*10 +(0xf & bcd);
}

uint8_t IntToVal(uint8_t k)
{
    uint8_t tmp;
    
    tmp = ((k/10)<<4) | (k%10);
  return tmp;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
char DrvInit_rtc(void)
{
  char rslt=1;

  GetTime();
  if( (DS1302_ReadReg(0xc0)!=0xaa)||(rtc_tm.sec>60)||(rtc_tm.min>60)||(rtc_tm.hour>24)||(rtc_tm.mday>31)||(rtc_tm.month>12) ){
    //没有初始化过DS1302，或者曾经断过电
    DS1302_WriteReg(0x8E,0x00);            //关闭写保护
    DS1302_WriteReg(0x90,0x00);            //禁止充电
    DS1302_WriteReg(0x8E,0x80);            //打开写保护
    //
    rtc_tm.sec = 0,
    rtc_tm.min = 0,
    rtc_tm.hour = 0,
    rtc_tm.mday = 25,
    rtc_tm.month = 12,
    rtc_tm.year = 14,
    SetTime(&rtc_tm);
    rslt = 0;
  }
  GetTime();
  
  return rslt;
}

void SetTime(tm *t)
{
  uint8_t buff[8];
  
  buff[0] = IntToVal(t->sec);
  buff[1] = IntToVal(t->min);
  buff[2] = IntToVal(t->hour);
  buff[3] = IntToVal(t->mday);
  buff[4] = IntToVal(t->month);
  buff[6] = IntToVal(t->year-2000);
  buff[7] = 0x80;   //写保护寄存器
  DS1302_WriteReg(0x8E,0x00);      //关闭写保护
  DS1302_WriteReg(0xc0,0xaa);      //置时间更新标志位
  DS1302_SendBytes(0xBE,8,buff);    //连续写日历/时钟寄存器
  DS1302_WriteReg(0x8E,0x80);      //关闭写保护
}

void GetTime(void)
{
    uint8_t buff[8];
    int c,y,m,d,iWeek;
    
    DS1302_GetBytes(0xBF,8,buff);   //连续读日历/时钟寄存器
    
    rtc_tm.sec = BcdToInt(buff[0]);
    rtc_tm.min = BcdToInt(buff[1]);
    rtc_tm.hour = BcdToInt(buff[2]);
    rtc_tm.mday = BcdToInt(buff[3]);
    rtc_tm.month = BcdToInt(buff[4]);
    rtc_tm.year = BcdToInt(buff[6])+2000;
    //星期几
    
    if ( rtc_tm.month == 1 || rtc_tm.month == 2 )  
    {  
        c = ( rtc_tm.year - 1 ) / 100;  
        y = ( rtc_tm.year - 1 ) % 100;  
        m = rtc_tm.month + 12;  
        d = rtc_tm.mday;  
    }  
    else  
    {  
        c = rtc_tm.year / 100;  
        y = rtc_tm.year % 100;  
        m = rtc_tm.month;  
        d = rtc_tm.mday;  
    }  
      
    iWeek = y + y / 4 + c / 4 - 2 * c + 26 * ( m + 1 ) / 10 + d - 1;    //蔡勒公式  
    iWeek = iWeek >= 0 ? ( iWeek % 7 ) : ( iWeek % 7 + 7 );    //iWeek为负时取模  
    if ( iWeek == 0 )    //星期日不作为一周的第一天  
    {  
        iWeek = 7;  
    }
    rtc_tm.wday = iWeek;
    //
    rtc_tm.tick = rtc_tm.hour*3600+rtc_tm.min*60+rtc_tm.sec;
}




