/****************************************************************************************
* 文件名：GUI_STOCKC.C
* 功能：设置前景色及背景色变量，用于ASCII码、汉字、窗口、单色位图显示。
* 作者：黄绍斌
* 日期：2004.02.28
* 备注：
****************************************************************************************/
#define _GUI_STOCKC_C


#include  "head.h"

#if  (GUI_WINDOW_EN==1)|(GUI_MenuIco_EN==1)|(GUI_MenuDown_EN==1)|(GUI_LoadPic_EN==1)|(GUI_PutHZ_EN==1)|(FONT5x7_EN==1)|(FONT8x8_EN==1)|(FONT24x32_EN==1)

/* 定义十进制(0-7)==>十六进制位转换表，由于显示点数据是由左到右，所以十六进制位顺序是倒的 */
uint8 const  DCB2HEX_TAB[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};


/* 定义前景色及背景色变量，用于ASCII码、汉字、窗口、单色位图显示 */
TCOLOR  disp_color;
TCOLOR	back_color;

/****************************************************************************
* 名称：GUI_SetColor()
* 功能：设置显示色及背景色。用于ASCII字符显示及汉字显示。
* 入口参数：color1	显示色的值
*          color2	背景色的值
* 出口参数：无
* 说明：
****************************************************************************/
void  GUI_SetColor(TCOLOR color1, TCOLOR color2)
{  GUI_CopyColor(&disp_color, color1);
   GUI_CopyColor(&back_color, color2);  
}


/****************************************************************************
* 名称：GUI_GetBackColor()
* 功能：最得当前背景色。
* 入口参数：bakc		保存颜色的变量地址
* 出口参数：无
* 说明：
****************************************************************************/
void  GUI_GetBackColor(TCOLOR  *bakc)
{  GUI_CopyColor(bakc, back_color);  
}


/****************************************************************************
* 名称：GUI_GetDispColor()
* 功能：最得当前前景色。
* 入口参数：bakc		保存颜色的变量地址
* 出口参数：无
* 说明：
****************************************************************************/
void  GUI_GetDispColor(TCOLOR  *bakc)
{  GUI_CopyColor(bakc, disp_color);  
}


/****************************************************************************
* 名称：GUI_ExchangeColor()
* 功能：交换前景色与背景色。用于反相显示。
* 入口参数：无
* 出口参数：无
* 说明：
****************************************************************************/
void  GUI_ExchangeColor(void)
{  TCOLOR  bakc;

   GUI_CopyColor(&bakc, disp_color);
   GUI_CopyColor(&disp_color, back_color);
   GUI_CopyColor(&back_color, bakc);
}

#endif