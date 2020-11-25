#ifndef _SPI_LCD_H
#define _SPI_LCD_H

#ifndef SPI_12864LCD
#define EXT_SPI_LCD extern
#else
#define EXT_SPI_LCD
#endif




//控制LCD的复位
#define LCD_RESET()		GPIO_ResetBits(GPIOD,GPIO_Pin_12);	//PD12->L	
#define LCD_WORK()		GPIO_SetBits(GPIOD,GPIO_Pin_12);	//PD12->H

//数据/指令控制
#define LCD_ORDER_EN()	GPIO_ResetBits(GPIOD,GPIO_Pin_13);//指令使能PD13->L
#define LCD_DATA_EN()	GPIO_SetBits(GPIOD,GPIO_Pin_13);//数据使能PD13->H

//控制LCD的片选CS
#define LCD_CS_EN()		GPIO_ResetBits(GPIOD,GPIO_Pin_11);//指令使能PD11->L
#define LCD_CS_DN()		GPIO_SetBits(GPIOD,GPIO_Pin_11);//数据使能PD11->H

//控制背光
#define LCD_BKG_EN()		GPIO_ResetBits(GPIOB,GPIO_Pin_11);//指令使能PB12->L
#define LCD_BKG_DN()		GPIO_SetBits(GPIOB,GPIO_Pin_11);//数据使能PB12->H


//---------------------------------------------------------------------------------

#define  TCOLOR				uint8 
#define  NULL				_NULL

/* 定义LCM像素数宏 */
#define  GUI_LCM_XMAX		128							/* 定义液晶x轴的像素数 */
#define  GUI_LCM_YMAX		64							/* 定义液晶y轴的像素数 */

#define DEFAULT_REFURBISH	300

#define TURN_PLUS					0X00	//字符正显
#define TURN_MINUS					0X01	//字符反显


#define  GUI_CopyColor(color1, color2) 	*color1 = color2		//&0x0
#define  GUI_CmpColor(color1, color2)	( (color1&0x01) == (color2&0x01) )



EXT_SPI_LCD void  GUI_FillSCR(TCOLOR dat);
EXT_SPI_LCD void  GUI_Initialize(void);

EXT_SPI_LCD uint8 GUI_Point(uint8 x, uint8 y, TCOLOR color);
EXT_SPI_LCD uint8 GUI_ReadPoint(uint8 x, uint8 y, TCOLOR *ret);
EXT_SPI_LCD void  GUI_HLine(uint8 x0, uint8 y0, uint8 x1, TCOLOR color);
EXT_SPI_LCD void  GUI_RLine(uint8 x0, uint8 y0, uint8 y1, TCOLOR color);


EXT_SPI_LCD UINT8 lcd_need_refurbish;

EXT_SPI_LCD void LCD_DispRefurbish(void);

EXT_SPI_LCD void GUI_ClrScr(void);
EXT_SPI_LCD void GUI_DispReverse(UINT8 x, UINT8 y, UINT8 width,UINT8 height);

#define CLR_SCREEN()	GUI_FillSCR(0x00)


#endif

