#ifndef _SURSOR_H
#define _SURSOR_H

#ifndef _SURSOR_C
#define EXT_SURSOR extern
#else
#define EXT_SURSOR 
#endif

//光标定义
#define CUR_LARGE     	0x00        //大光标（整个高度字符）
#define CUR_HALF      	0x01        //中光标（半个高度字符）
#define CUR_SMALL     	0x02        //小光标（下划线）
#define CUR_BAND      	0x03        //光带
//光标显示模式定义
#define C_CUR_HIDE  	0x00
#define C_CUR_FLASH     0x01
#define C_CUR_NO_FLASH  0x02

EXT_SURSOR void CurHideCur(void);
EXT_SURSOR void CurShowCur(void);
EXT_SURSOR void CurDisplayCur(UINT8 x,UINT8 y,UINT8 dx, UINT8 dy, UINT8 mode);
EXT_SURSOR void CurRefCur(void);
EXT_SURSOR void CurDisCur(void);

#endif