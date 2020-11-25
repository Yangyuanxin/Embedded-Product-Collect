/****************************************************************************************
* 文件名：LOADBIT.C
* 功能：显示单色图形及汉字显示。先将图形转换为对应的点阵数组，然后即可调用此文件的函数输出驱动。
* 作者：黄绍斌
* 日期：2004.02.26
* 备注：使用GUI_SetBackColor()函数设置显示颜色及背景色。
****************************************************************************************/
#define _LOADBIT_C

#include  "head.h"


#if  (GUI_LoadPic_EN==1)|(GUI_MenuIco_EN==1)|(GUI_PutHZ_EN==1)
/****************************************************************************
* 名称：GUI_LoadLine()
* 功能：输出单色图形的一行数据。
* 入口参数： x		指定显示位置，x坐标
*           y		指定显示位置，y坐标
*           dat		要输出显示的数据。
*           no      要显示此行的点个数
* 出口参数：返回值为1时表示操作成功，为0时表示操作失败。
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
uint8  GUI_LoadLine(uint32 x, uint32 y, uint8 *dat, uint32 no)
{  uint8   bit_dat;
   uint8   i;
   TCOLOR  bakc;

   /* 参数过滤 */
   if(x>=GUI_LCM_XMAX) return(0);
   if(y>=GUI_LCM_YMAX) return(0);
   
   for(i=0; i<no; i++)
   {  /* 判断是否要读取点阵数据 */
      if( (i%8)==0 ) bit_dat = *dat++;
     
      /* 设置相应的点为color或为back_color */
      if( (bit_dat&DCB2HEX_TAB[i&0x07])==0 ) GUI_CopyColor(&bakc, back_color); 
         else  GUI_CopyColor(&bakc, disp_color);
      GUI_Point(x, y, bakc);     //gh_auto  
     
      if( (++x)>=GUI_LCM_XMAX ) return(0);
   }
   
   return(1);
}
#endif


#if  (GUI_LoadPic_EN==1)|(GUI_MenuIco_EN==1)
/****************************************************************************
* 名称：GUI_LoadPic()
* 功能：输出单色图形数据。
* 入口参数： x		指定显示位置，x坐标
*           y		指定显示位置，y坐标
*           dat		要输出显示的数据
*           hno     要显示此行的点个数
*           lno     要显示此列的点个数
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
void  GUI_LoadPic(uint32 x, uint32 y, uint8 *dat, uint32 hno, uint32 lno)
{  uint32  i;

   for(i=0; i<lno; i++)
   {  GUI_LoadLine(x, y, dat, hno);				// 输出一行数据
      y++;										// 显示下一行
      dat += (hno>>3);							// 计算下一行的数据
      if( (hno&0x07)!=0 ) dat++;
   }
}




/****************************************************************************
* 名称：GUI_LoadPic1()
* 功能：输出单色图形数据，反相显示。
* 入口参数： x		指定显示位置，x坐标
*           y		指定显示位置，y坐标
*           dat		要输出显示的数据。
*           hno     要显示此行的点个数
*           lno     要显示此列的点个数
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
void  GUI_LoadPic1(uint32 x, uint32 y, uint8 *dat, uint32 hno, uint32 lno)
{  uint32  i;
   
   GUI_ExchangeColor();									// 显示色与背景色交换
   for(i=0; i<lno; i++)
   {  GUI_LoadLine(x, y, dat, hno);						// 输出一行数据
      y++;												// 显示下一行
      dat += (hno>>3);									// 计算下一行的数据
      if( (hno&0x07)!=0 ) dat++;
   }
   GUI_ExchangeColor();
   
}
#endif


#if  GUI_PutHZ_EN==1
/****************************************************************************
* 名称：GUI_PutHZ()
* 功能：显示汉字。
* 入口参数： x		指定显示位置，x坐标
*           y		指定显示位置，y坐标
*           dat		要输出显示的汉字点阵数据。
*           hno     要显示此行的点个数
*           lno     要显示此列的点个数
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
void  GUI_PutHZ(uint32 x, uint32 y, uint8 *dat, uint8 hno, uint8 lno)
{  uint8  i;

   for(i=0; i<lno; i++)
   {  GUI_LoadLine(x, y, dat, hno);						// 输出一行数据
      y++;												// 显示下一行
      dat += (hno>>3);									// 计算下一行的数据
      if( (hno&0x07)!=0 ) dat++;
   }
}
#endif






