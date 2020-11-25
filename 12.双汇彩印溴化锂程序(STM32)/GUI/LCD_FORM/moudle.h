#ifndef _LCD_MOUDLE_H
#define _LCD_MOUDLE_H
#ifndef _LCD_MOUDLE_C
#define EXT_LCD_MOUDLE extern
#else
#define EXT_LCD_MOUDLE
#endif

//--------------------------------------------------
EXT_LCD_MOUDLE UINT8 ExeCallForm(UINT8 *str, UINT8 mode);
EXT_LCD_MOUDLE UINT8 ExePasswordComp(UINT8 *str, UINT8 mode);


//--------------------------------------------------
#endif