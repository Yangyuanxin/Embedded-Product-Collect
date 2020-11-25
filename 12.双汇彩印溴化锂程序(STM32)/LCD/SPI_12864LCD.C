/*
该文件为金升阳SMG12864K9的驱动程序。
该模块使用了一个SPI口作为数据或命令输入。
*/


#define SPI_12864LCD


#include "head.h"

TCOLOR  gui_disp_buf[GUI_LCM_YMAX/8][GUI_LCM_XMAX];


void LcdInit_Gpio(void);
void SPI_SendChar(UINT8 data);


void delay_lcd(UINT16 value)
{
    UINT32 abc;
    
    while(value--)
    {
        abc = 0xff;
        
        while (--abc);
    }
}

/*
Init lcd
*/
void LcdInit(void)
{
	
	LcdInit_Gpio();
	
	LCD_CS_EN();
	
	//复位
	LCD_RESET();
	OSTimeDly(OS_TICKS_PER_SEC / 50);
	//delay
	LCD_WORK();
    OSTimeDly(OS_TICKS_PER_SEC / 50);
    LCD_BKG_EN();
	OSTimeDly(OS_TICKS_PER_SEC / 50);
	//切换到发送指令状态
	LCD_ORDER_EN();		
	OSTimeDly(OS_TICKS_PER_SEC / 50);
	SPI_SendChar(0xA2);		//设置LCD bias
	OSTimeDly(OS_TICKS_PER_SEC / 50);
	SPI_SendChar(0xA0);		//设置ADC select
	OSTimeDly(OS_TICKS_PER_SEC / 50);
	SPI_SendChar(0xc8);		//设置COM反向
	OSTimeDly(OS_TICKS_PER_SEC / 50);
	SPI_SendChar(0x2f);		//设置电源控制模式
	OSTimeDly(OS_TICKS_PER_SEC / 50);
	SPI_SendChar(0x60);		//设置显示初始行
	OSTimeDly(OS_TICKS_PER_SEC / 50);
	SPI_SendChar(0xaf);		//开显示 
    OSTimeDly(OS_TICKS_PER_SEC / 50);
    
    //while (1)   ;
    
	
}


/*
初始化LCD
1、占用的IO口为输出状态
2、占用的SPI口正确配置
*/
void LcdInit_Gpio(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	
	
	//GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	//
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//SCL MOSI
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	//此处需增加相应代码
	/* SPI2 configuration */ 
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);
	
	/* Enable SPI2*/  
	SPI_Cmd(SPI2, ENABLE);
	
}

/*
通过SPI口发送一个字符
*/
void SPI_SendChar(UINT8 data)
{
	//此处需增加相应代码
	/* Loop while DR register in not emplty */
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	//NotSelect_Flash();  while(1);
	/* Send byte through the SPI2 peripheral */
	SPI_I2S_SendData(SPI2, data);
	
	
	/* Wait to receive a byte */
	//  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	
	/* Return the byte read from the SPI bus */
	//  return SPI_I2S_ReceiveData(SPI1);
}



/*
清屏
*/
void LCD_DispFill(uint8 filldata)
{
	//LCD_DispRefurbish();
}

/*
LCD刷屏显示
每固定的时间段或者是有更新数据时，开始整屏刷新
*/
void LCD_DispRefurbish(void)
{
	u8 i,j;    
	
	LCD_CS_EN();
    OSTimeDly(OS_TICKS_PER_SEC / 50);
    //切换到发送指令状态
	LCD_ORDER_EN();		
	OSTimeDly(OS_TICKS_PER_SEC / 50);
	//	SPI_SendChar(0xA2);		//设置LCD bias
	OSTimeDly(OS_TICKS_PER_SEC / 50);
	//	SPI_SendChar(0xA0);		//设置ADC select
	OSTimeDly(OS_TICKS_PER_SEC / 50);
	//SPI_SendChar(0xc8);		//设置COM反向
	OSTimeDly(OS_TICKS_PER_SEC / 50);
	//SPI_SendChar(0x2f);		//设置电源控制模式
	OSTimeDly(OS_TICKS_PER_SEC / 50);
	SPI_SendChar(0x60);		//设置显示初始行
	OSTimeDly(OS_TICKS_PER_SEC / 50);
	SPI_SendChar(0xaf);		//开显示 
    OSTimeDly(OS_TICKS_PER_SEC / 50);
	
	
	for (j=0; j<8; j++)
	{
		//OSTimeDly(OS_TICKS_PER_SEC / 50);
		delay_lcd(10);
		LCD_ORDER_EN();
		delay_lcd(10);
		SPI_SendChar(0xB0+j);		//设置显示初始行
		//OSTimeDly(OS_TICKS_PER_SEC / 50);
		delay_lcd(10);
		
		SPI_SendChar(0x10);		//设置显示初始行
		delay_lcd(10);
		//OSTimeDly(OS_TICKS_PER_SEC / 50);
		SPI_SendChar(0x01);		//设置显示初始行
		//OSTimeDly(OS_TICKS_PER_SEC / 50);
		delay_lcd(10);
		
		LCD_DATA_EN();
		//OSTimeDly(OS_TICKS_PER_SEC / 50);
		delay_lcd(50);
		for (i=0;i<128;i++)
		{
			SPI_SendChar(gui_disp_buf[j][i]);		//开显示 
			//SPI_SendChar(tk++);		//开显示 
			//OSTimeDly(OS_TICKS_PER_SEC / 50);
			//delay_lcd(10);
		}
	}
	
    
	
}


void  GUI_FillSCR(TCOLOR dat)
{  
	uint32 i,j;
    
    #if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
    #endif
	
	// 填充缓冲区
	for(i=0; i<(GUI_LCM_YMAX/8); i++)
	{  
		for(j=0; j<GUI_LCM_XMAX; j++)
		{  
			gui_disp_buf[i][j] = dat;
		}
	}
	
	// 填充LCM
	//LCD_DispFill(dat);
    REFURBISH_LCD();
}


void GUI_ClrScr(void)
{
	GUI_FillSCR(0);
}

/****************************************************************************
* 名称：GUI_Initialize()
* 功能：初始化GUI，包括初始化显示缓冲区，初始化LCM并清屏。
* 入口参数：无
* 出口参数：无
* 说明：用户根据LCM的实际情况编写此函数。
****************************************************************************/
void  GUI_Initialize(void)
{  
	LcdInit();						// 初始化LCM模块工作模式，纯图形模式
	GUI_FillSCR(0);				// 初始化缓冲区为0x00，并输出屏幕(清屏)
}

uint8  LCD_ReadByte(uint8 x, uint8 y)
{ //x = x&0x7f;				// 参数过滤
	y = y&0x3f;
	
	y = y>>3;
	return(gui_disp_buf[y][x]);
}

void GUI_UpdataDispBuf(UINT8 x, UINT8 y, UINT8 data)
{
	y = y>>3;
	gui_disp_buf[y][x] = data;
}


const UINT8   DEC_HEX_TAB[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

uint8 GUI_Point(uint8 x, uint8 y, TCOLOR color)
{
    uint8   bak;
	
	// 参数过滤 
	if(x>=GUI_LCM_XMAX) return(0);
	if(y>=GUI_LCM_YMAX) return(0);
	
	// 设置相应的点为1或0 
	bak = LCD_ReadByte(x,y);
	if(0==color)
	{  bak &= (~DEC_HEX_TAB[y&0x07]);
	}
	else
	{  bak |= DEC_HEX_TAB[y&0x07];
	}
	
	// 刷新显示 
	GUI_UpdataDispBuf(x, y, bak);//gh_auto
   return(1);
}
uint8 GUI_ReadPoint(uint8 x, uint8 y, TCOLOR *ret)
{
	uint8  bak;
	
	// 参数过滤
	if(x>=GUI_LCM_XMAX) return(0);
	if(y>=GUI_LCM_YMAX) return(0);
	
	bak = LCD_ReadByte(x,y);
	if( (bak & (DEC_HEX_TAB[y&0x07])) == 0 ) *ret = 0x00;
	else  *ret = 0x01;
	
   return(1);
	
}
void  GUI_HLine(uint8 x0, uint8 y0, uint8 x1, TCOLOR color)
{
	uint8  bak;
	
	if(x0>x1) 						// 对x0、x1大小进行排列，以便画图
	{  bak = x1;
	x1 = x0;
	x0 = bak;
	}
	
	do
	{  GUI_Point(x0, y0, color);		// 逐点显示，描出垂直线
	x0++;
   }while(x1>=x0);
}
void  GUI_RLine(uint8 x0, uint8 y0, uint8 y1, TCOLOR color)
{
	uint8  bak;
	uint8  wr_dat;
	
	if(y0>y1) 		// 对y0、y1大小进行排列，以便画图
	{  bak = y1;
	y1 = y0;
	y0 = bak;
	}
	
	do
	{  // 先读取当前点的字节数据
		bak = LCD_ReadByte(x0,y0);	
		
		// 进行'与'/'或'操作后，将正确的数据写回LCM
		// 若y0和y1不是同一字节，则y0--当前字节结束，即(y0+8)&0x38，全写1，或者0。
		// 若y0和y1是同一字节，则y0--y1，要全写1，或者0。
		// 方法：dat=0xff，然后按y0清零dat低位，按y1清零高位。
		if((y0>>3) != (y1>>3))		// 竖直线是否跨越两个字节(或以上)
		{  wr_dat = 0xFF << (y0&0x07);// 清0低位
		
		if(color)					
		{  wr_dat = bak | wr_dat;	// 若color不为0，则显示
		}
		else
		{  wr_dat = ~wr_dat;		// 若color为0，则清除显示
		wr_dat = bak & wr_dat;
		}
		GUI_UpdataDispBuf(x0,y0, wr_dat);//gh_auto
		y0 = (y0+8)&0x38;
		}
		else
		{  wr_dat = 0xFF << (y0&0x07);
		wr_dat = wr_dat &  ( 0xFF >> (7-(y1&0x07)) );
		
		if(color)					
		{  wr_dat = bak | wr_dat;	// 若color不为0，则显示
		}
		else
		{  wr_dat = ~wr_dat;		// 若color为0，则清除显示
		wr_dat = bak & wr_dat;
		}
		GUI_UpdataDispBuf(x0,y0, wr_dat);//gh_auto 
		
		
		return;
		} // end of if((y0>>3) != (y1>>3))... else...
   }while(y1>=y0);
}
/*******
功能：对选定的区域反选
*****************/
void GUI_DispReverse(UINT8 x, UINT8 y, UINT8 width,UINT8 height)
{
	UINT8 dx,dy;
	UINT8 temp;
	UINT8 flag;
	//参数过滤
	if(x>=GUI_LCM_XMAX) return;
   	if(y>=GUI_LCM_YMAX) return;
   	if (height>=GUI_LCM_YMAX)	return;
   	if (width>=GUI_LCM_XMAX) 	return;
	//
	for (dy=0; dy<height; dy++)
	{		
		for (dx=0; dx<width; dx++)
		{
			if ((x+dx) >= GUI_LCM_XMAX)
			{
				break;
			}
			//
			flag = 0;//flag好像始终都是0
			if ((y%8) != 0)
			{
				flag = 0;
			}
			//			
			temp = gui_disp_buf[(y>>3)+flag][x+dx];
			temp ^= DCB2HEX_TAB[7-(y&0x07)];
			gui_disp_buf[(y>>3)+flag][x+dx] = temp;
			//			
		}
		y++;
	}
}