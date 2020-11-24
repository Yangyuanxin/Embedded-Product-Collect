#ifndef __LQ12864_H__
#define __LQ12864_H__

#include "config.h"

#define X_WIDTH 128
#define Y_WIDTH 64

extern unsigned char code Logo[];

extern void OLED_Init(void);
extern void OLED_CLS(void);
extern void OLED_P6x8Str(unsigned char x,unsigned char y,unsigned char ch[]);
extern void OLED_P8x16Str(unsigned char x,unsigned char y,unsigned char ch[]);
extern void OLED_P14x16Str(unsigned char x,unsigned char y,unsigned char ch[]);
extern void OLED_P16x32Temp(unsigned char x,unsigned char y,unsigned char ch[]);
//extern void OLED_PrintU16(unsigned char x,unsigned char y,unsigned int num);
extern void OLED_Print(unsigned char x, unsigned char y, unsigned char ch[]);
//extern void OLED_PutPixel(unsigned char x,unsigned char y);
//extern void OLED_Rectangle(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2,unsigned char gif);
extern void Draw_BMP(unsigned char * bmp); 

#endif

