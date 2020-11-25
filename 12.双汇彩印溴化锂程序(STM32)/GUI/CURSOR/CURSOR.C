#define _CURSOR_C

#include "head.h"

void CurTurnMinus(void);

UINT8 cur_x,cur_y,cur_mode,cur_width,cur_height;

//隐藏光标
void CurHideCur(void)
{
	if (cur_s != 0x00)//若当前操作区域有光标，则清除
	{
		CurTurnMinus();
	}
}

//显示光标
void CurShowCur(void)
{
	if (cur_mode != C_CUR_HIDE)
	{
		CurTurnMinus();
	}
}

//禁能光标
void CurDisCur(void)
{
	CurHideCur();
	cur_mode = C_CUR_HIDE;
}

//刷新光标,此函数是被周期调用的	API函数
void CurRefCur(void)
{
	if (cur_mode == C_CUR_HIDE)
	{
		return;
	}
	//
	if (cur_mode == C_CUR_NO_FLASH)
	{
		return;
	}
	////
	CurTurnMinus();
}

//更新光标位置函数				API函数
void CurDisplayCur(UINT8 x,UINT8 y,UINT8 dx, UINT8 dy, UINT8 mode)
{
	cur_x = x;
	cur_y = y;
	cur_width = dx;
	cur_height = dy;
	cur_mode = mode;
	
	CurShowCur();
}

//对显示缓冲区进行操作，显示光标
void CurTurnMinus(void)
{
    #if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
    #endif
  
	GUI_DispReverse(cur_x,
					cur_y,
					cur_width,
					cur_height
					);
					
	cur_s ^= 0x01;		
	
	//
	REFURBISH_LCD();		
}