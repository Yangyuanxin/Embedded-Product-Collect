/**
  *******************  ***********************************************************
  * @file    lcd12864.c
  * @author  xiaoyuan
  * @version V1.0
  * @date    2016-04-16
  * @brief   This file provides all the lcd12864 functions.
  ******************************************************************************
  * @attention
  * 该代码是从xs128例程中移植过来的。
	* 串行模式
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "lcd12864.h" 
#include "delay.h"

unsigned char adress_table[]=                 //定义液晶点阵的坐标
{ 
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,      //第一行汉字位置 
0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,      //第二行汉字位置 
0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,      //第三行汉字位置 
0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F       //第四行汉字位置 
};


/*************************************************************/
/*                      初始化液晶接口                       */
/*************************************************************/
void LCD12864_Init(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);  //使能PA时钟
	
	GPIO_InitStructure.GPIO_Pin = PIN_CS|PIN_SID|PIN_CLK|PIN_PSB|PIN_RST;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出
	GPIO_Init(GPIOE,&GPIO_InitStructure);             //按上面设定初始化GPIOA端口
	RESET_PSB;	   //PSB拉低定义为串行数据模式

}

/*************************************************************/
/*                   写一个字节的数据                     */
/*************************************************************/
void LCD12864_WriteByte(unsigned char byte) 
{ 
unsigned char j; 
for(j=0;j<8;j++)        
{ 
	if((byte<<j)&0x80)
		SET_SID; 
	else 
		RESET_SID; 
	SET_SCLK; 
	RESET_SCLK; 
} 
} 

/*************************************************************/
/*                     向液晶发送数据                        */
/*************************************************************/
void LCD12864_WriteData(uint8_t data) 
{
  SET_CS; 
  RESET_SCLK; 
  LCD12864_WriteByte(LCD12864_MODE_DTAE); 
  LCD12864_WriteByte(data&0xF0);          //写高四位数据 
  LCD12864_WriteByte(0xf0&(data<<4));     //写低四位数据 
  RESET_CS; 
}

/*************************************************************/
/*                      向液晶发送命令                       */
/*************************************************************/
void LCD12864_WriteCmd(uint8_t cmd) 
{
  SET_CS; 
  RESET_SCLK; 
  LCD12864_WriteByte(LCD12864_MODE_CMD); 
  LCD12864_WriteByte(cmd&0xF0);        //写高四位数据 
  LCD12864_WriteByte(0xf0&(cmd<<4));        //写低四位数据 
  RESET_CS;   
}

/***************************************************************************/
/*                            清屏子程序                                   */
/***************************************************************************/
void LCD12864_Clear(void)
{
    LCD12864_WriteCmd(0x30);//0011,0000 功能设置，一次送8位数据，基本指令集 
    delay_us(80);       //延时80us
    LCD12864_WriteCmd(0x03);//AC归0,不改变DDRAM内容 
    delay_ms(5);        //延时5ms
    LCD12864_WriteCmd(0x01);//0000,0001 清DDRAM 
    delay_ms(5);        //延时5ms
    LCD12864_WriteCmd(0x06);//写入时,游标右移动 
    delay_us(80);       //延时80us
    LCD12864_WriteCmd(0x0C);//0000,1100  整体显示，游标off，游标位置off
    delay_us(80);       //延时80us
}

/***************************************************************************/
/*                           清除绘图存储区                               */
/***************************************************************************/
void LCD12864_ClearGraphMemory(void)//在反白之前先清绘图存储区,将绘图存储区的参数全设为不反白0x00.
{
	unsigned char i,j;
	LCD12864_WriteCmd(0x36);//lcm_w_test(0,0x36); //图形方式
	delay_us(80);
	for(i=0;i<32;i++)
	{
		LCD12864_WriteCmd(0x80+i);//lcm_w_test(0,0x80+i);
		delay_us(20);
		LCD12864_WriteCmd(0x80);//lcm_w_test(0,0x80);
		delay_us(20);
		for(j=0;j<16;j++) 
		{
		    LCD12864_WriteData(0x00);//lcm_w_test(1,0x00);
			delay_us(20);
		}
	}
	for(i=0;i<32;i++)
	{
		LCD12864_WriteCmd(0x80+i);//lcm_w_test(0,0x80+i);
		delay_us(20);
		LCD12864_WriteCmd(0x88);//lcm_w_test(0,0x88);
		delay_us(20);
		for(j=0;j<16;j++) 
		{
			LCD12864_WriteData(0x00);//lcm_w_test(1,0x00);
			delay_us(20);
		}
	}
}
/***************************************************************************/
/*                           反白显示                               */
/***************************************************************************/
void LCD12864_HightlightShow (unsigned char CX, unsigned char CY, unsigned char width,unsigned char f) //CX（0-3）反白显示的行，CY（0-7）开始反白显示的列，width（0-16）反白显示的宽度
{
	unsigned char halfLineCnt, basicBlock,lcdPosX,lcdPosY;
	/*反白显示之前先清除图形存储区，*/
	if(f==1)
	LCD12864_ClearGraphMemory(); //清绘图区
	lcdPosY = 0x80;
	if (CX == 0)
	{
		CX = 0x80;
		halfLineCnt = 16;
	}
	else if (CX == 1)
	{
		CX = 0x80;
		halfLineCnt = 32;
	}
	else if (CX == 2)
	{
		CX = 0x88;
		halfLineCnt = 16;
	}
	else if (CX == 3)
	{
		CX = 0x88;
		halfLineCnt = 32;
	}
	lcdPosX = CX + CY;

	for (; halfLineCnt != 0; halfLineCnt--)
	{
		basicBlock = width;
		LCD12864_WriteCmd(0x34);//write_com(0x34);
		delay_us(20);
		LCD12864_WriteCmd(lcdPosY);
		delay_us(20);
		LCD12864_WriteCmd(lcdPosX);
		delay_us(20);
		LCD12864_WriteCmd(0x30);
		delay_us(20);

		for (;basicBlock != 0; basicBlock--)
		{
			if (halfLineCnt > 16)
			{
				LCD12864_WriteData(0x00);
				delay_us(20);
			}
			else
			{
				LCD12864_WriteData(0xff);
//				if(YN==1) 
//					LCD12864_WriteData(0xff); //高亮
//				else 
//					LCD12864_WriteData(0x00); //清除高亮
			}
			delay_us(20);
		}
		lcdPosY++;
	}
	LCD12864_WriteCmd(0x36);
	delay_us(80);
	LCD12864_WriteCmd(0x30);
	delay_us(80);
}


/***************************************************************************/
/*                           向LCD12864发送字符串                               */
/***************************************************************************/
void LCD12864_ShowString(uint8_t row,uint8_t col,uint8_t *data1)   //row为写入数据所在的行数,col为写入数据所在的列数，*data1为写入的数据
{
  for(;row<4&&(*data1)!=0;row++)
  { 
      for(;col<8&&(*data1)!=0;col++)
      { 
          LCD12864_WriteCmd(adress_table[row*8+col]);
          delay_us(80);       //延时80us
           
          LCD12864_WriteData(*data1++); 
          delay_us(80);       //延时80us
          LCD12864_WriteData(*data1++); 
          delay_us(80);       //延时80us
      } 
      col=0; 
  }
}
/***************************************************************************/
/*                           向LCD12864发送整数                               */
/***************************************************************************/
void LCD12864_ShowNum(uint8_t row,uint8_t col,uint16_t num)  //显示整型变量的函数,最多显示16位的整数。只能显示正数。
{
	uint8_t temp[17];
	uint8_t str[17];
	int i=0,j=0;
	while(num != 0)	  //这里不能用num%10 != 0，如果num是10的整数倍，
	                  //例如，100，这样就会出错，根本就不能进入循环体。
	{
		temp[i] = (num%10)+0x30;
		num/=10;
		i++;
	}
	i--;           //因为i在退出循环之前还自加了一次，此时，
	                //指向最后一个存储有用值的元素的后一个位置。
	while(i != -1)	 //因为i=0时，temp[0]还是有用值。
	{
		str[j] = temp[i];
		j++;
		i--;	
	}
	str[j]='\0';  //因为i在退出循环之前还自加了一次，此时，
	              //指向最后一个存储有用值的元素的后一个位置。
	LCD12864_ShowString(row,col,str);	
}
/***************************************************************************/
/*                           向LCD12864发送小数                              */
/***************************************************************************/
void LCD12864_ShowFloat(uint8_t row,uint8_t col,float fnum) //显示有4位小数的浮点数，总位数不超过16位。
{
	long int num = fnum*10000;
	u8 temp[17];
	u8 str[17];
	int i=0,j=0;
	while(num != 0)	                  
	{
		temp[i] = (num%10)+0x30;
		num/=10;
		i++;
		if(i == 4)	 //4位小数处理完后，加入小数点。
		{
			temp[i] = '.';
			i++;
		}
	}
	i--;   
	while(i != -1)	
	{
		str[j] = temp[i];
		j++;
		i--;	
	}
	str[j]='\0';  
	LCD12864_ShowString(row,col,str);				
}












