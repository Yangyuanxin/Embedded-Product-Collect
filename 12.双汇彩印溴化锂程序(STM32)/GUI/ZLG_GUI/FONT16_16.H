#ifndef  _FONT16_16_H
#define  _FONT16_16_H

#ifndef _FONT16_16_C
#define EXT_FONT16_16 extern
#else 
#define EXT_FONT16_16
#endif

#define REFURBISH_LCD()	OS_ENTER_CRITICAL();lcd_need_refurbish = 1;	OS_EXIT_CRITICAL();

EXT_FONT16_16  uint8 GUI_PutChar8_16(uint32 x, uint32 y, uint8 ch ,UINT8 type);
EXT_FONT16_16  void GUI_PutStringTier(UINT32 x, UINT32 y, char *str, UINT8 type);
EXT_FONT16_16  void  GUI_PutString16_16(uint32 x, uint32 y, char *str, UINT8 type);
EXT_FONT16_16  void  GUI_PutNoStr16_16(uint32 x, uint32 y, char *str, uint8 no, UINT8 type);
EXT_FONT16_16  void  GUI_PutString8_16(uint32 x, uint32 y, char *str);

#endif