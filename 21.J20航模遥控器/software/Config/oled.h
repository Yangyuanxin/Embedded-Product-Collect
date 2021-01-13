#ifndef __OLED_H
#define __OLED_H
#include "sys.h"
#include "stdlib.h"
#define OLED_MODE 0
#define SIZE 8
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF
#define X_WIDTH 	128
#define Y_WIDTH 	64
//-----------------OLED IIC端口定义----------------

#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_8)//SCL
#define OLED_SCLK_Set() GPIO_SetBits(GPIOB,GPIO_Pin_8)

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_9)//SDA
#define OLED_SDIN_Set() GPIO_SetBits(GPIOB,GPIO_Pin_9)


#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据


//OLED控制用函数
void OLED_WR_Byte(unsigned dat, unsigned cmd);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_Clear_GRAM(void);
void OLED_Refresh_Gram(void);
void OLED_DrawPoint(u8 x, u8 y, u8 t);
void OLED_DrawPlusSign(u8 x, u8 y);
void OLED_Fill(u8 x1, u8 y1, u8 x2, u8 y2, u8 dot);
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size, u8 mode);
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size, u8 mode);
void OLED_ShowString(u8 x, u8 y, u8 *p, u8 size, u8 mode);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowChinese(u16 x, u16 y, u8 index, u8 size, u8 mode);
void OLED_ShowChineseWords(u16 x, u16 y, u8 hzIndex[], u8 len, u8 mode);
void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[]);
void OLED_DrawPointBMP(u8 x, u8 y, unsigned char BMP[], u8 length, u8 height, u8 mode);
void fill_picture(unsigned char fill_Data);
void IIC_Start(void);
void IIC_Stop(void);
void Write_IIC_Command(unsigned char IIC_Command);
void Write_IIC_Data(unsigned char IIC_Data);
void Write_IIC_Byte(unsigned char IIC_Byte);
void IIC_Wait_Ack(void);
u8 *itoa(int num, u8 *str, int radix);
#endif

