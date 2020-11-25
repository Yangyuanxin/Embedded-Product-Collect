#define _LCDDRV_C

#include "head.h"
const u8 databit[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
const u8 databit1[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
/* 定义显示缓冲区 */
TCOLOR  gui_disp_buf[GUI_LCM_YMAX/8][GUI_LCM_XMAX];
static u8 reverce_data(u8 data);
static u8 reverce_data(u8 data)
{
	u8  variable;
	u8 i;
	variable = 0;
	for(i=0; i<8; i++)
	{
		if((data&databit[i]) == databit[i])
		{
			variable |= databit1[i];
		}
	}
	return variable;
}


void gpio_LcdControl(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

/*RS RW E*/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
/*CS1 CS2 BKG RST*/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

        BKG_OFF();
        GPIO_ResetBits(GPIOB,GPIO_Pin_12);

}

void  data_in(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}
void data_out(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void LcdDelay(void)
{
	UINT32 i;
	
	i = 0xf;
	
	while (--i) ;
}

void LcdWrCommand(UINT8 cmd)
{
	u16 temp;
        u8 temp_cmd;
        temp_cmd=reverce_data(cmd);
	data_in(); 
	
	RW_H();
	RS_L();
	E_H();   
	  
	while(BUSY_FLAG()); 
    
	data_out();
	
	temp=GPIO_ReadOutputData(GPIOD);
	temp=temp&0xff;
	
	RW_L();
    
	temp = (temp_cmd<<8)|temp;
	
	GPIO_Write(GPIOD, temp);

//	while(1);
	
	E_H();
    LcdDelay();
	E_L();
}


void LcdWrData(UINT8 data)
{
	u16 temp;
        u8 temp_data;
        temp_data=reverce_data(data);
	data_in();
	do
	{
		RW_H();//高电平读数据
		RS_L();	 //读指令代码
		E_H();	 //读使能
	}
	while(BUSY_FLAG());
	data_out();
	
	temp=GPIO_ReadOutputData(GPIOD);
	temp=temp&0xff;
	E_L(); //禁止信号
    
	RS_H();//写的为显示数据
    
	RW_L();	 //低电平写操作
    
	temp = (temp_data<<8)|temp;
	
	GPIO_Write(GPIOD, temp);
	
	E_H();	//
    
	E_L();	//由高到低的下降沿写数据 
}


/***********************************************************************
* 名称：LCM_WriteByte()
* 功能：向指定点写数据(一字节)。
* 入口参数：x 		x坐标值(0-127)
*	    	y       y坐标值(0-63)
*           wrdata	所要写的数据
* 说明：会重新设置CS1/CS2，及其内部指针
***********************************************************************/
void LCD_WriteByte(uint8 x, uint8 y, uint8 wrdata) 
{	
	y = y>>3;
 //   gui_disp_buf[y][x] = wrdata;
	if (x>127)
	{
		C1_EN();
	}
	else if (x>63)
	{
		C2_EN();
	}
	else 
	{

	}
	if (x>63)x-=64;
	if (x>63)x-=64;
	
	LcdWrCommand(y+C_TOP_BASE);
	LcdWrCommand(x+C_LEFT_BASE);	
	
	LcdWrData(wrdata);
    
    
    C1_DN();
    C2_DN();	
}


/***********************************************************************
* 名称：LCM_ReadByte()
* 功能：读取指定点上的数据。
* 入口参数：x 	  x坐标值(0-127)
*	   	    y     y坐标值(0-63)
* 出口参数：返回该点上的字节数据。
***********************************************************************/
uint8  LCD_ReadByte(uint8 x, uint8 y)
{ //x = x&0x7f;				// 参数过滤
  y = y&0x3f;

  y = y>>3;
  return(gui_disp_buf[y][x]);
}



/////////////////////////////////////////////////////////////////////////


/***********************************************************************
* 名称：LCM_DispFill()
* 功能：向显示缓冲区填充数据
* 入口参数：filldata  	要写入LCM的填充数据
* 注：此函数会设置显示起始行为0，且会自动选中CS1有效
***********************************************************************/
void LCD_DispFill(uint8 filldata)
{ 
	uint16 page;
	uint16 i;
	
	C1_EN();
	for(page=0;page<8;page++)
	{
		LcdWrCommand(C_LEFT_BASE+0);  
        
		LcdWrCommand(C_TOP_BASE+page);     
        
		for(i=0;i<64;i++)
		{
			LcdWrData(filldata);
		}
	}
	C1_DN();
	C2_DN();

    
    C2_EN();
	for(page=0;page<8;page++)
	{
		LcdWrCommand(C_LEFT_BASE+0);  
        
		LcdWrCommand(C_TOP_BASE+page);     
        
		for(i=0;i<64;i++)
		{
			LcdWrData(filldata);
		}
	}
	C1_DN();
	C2_DN();
	
    
    
	
}

    
/***********************************************************************
* 名称：LCM_DispIni()
* 功能：LCM显示初始化
* 入口参数：无
* 出口参数：无
* 注：初化显示后，清屏并设置显示起始行为0
*     会复位LCM_DISPCX，LCM_DISPCY.(并会只选中CS1)
***********************************************************************/
void LCD_DispIni(void)
{ 
	data_out();	//连接液晶D0-D7和控制线的数据方向位输出
	
    C1_DN();	
    C2_DN();	
    
    //增加片选信号
    C1_EN();
	LcdWrCommand(C_STARTLINE+0);
	LcdWrCommand(0x3f);		//打开显示器开关
    
	C1_DN();
    
    
    C2_EN();  
	LcdWrCommand(C_STARTLINE+0);
    LcdWrCommand(0x3f);		//打开显示器开关    
	C2_DN();
	
}



void LCD_DispRefurbish(void)
{
	INT32 i,page;	
        u8 temp_data,temp_page;
        temp_page=0;
 
    C1_DN();	
    C2_DN();	
    
	C2_EN();	
	for(page=8;page>=1;page--)
	{
		LcdWrCommand(C_LEFT_BASE+0);
		LcdWrCommand(C_TOP_BASE+temp_page++);
		for(i=63;i>=0;i--)
		{
                         temp_data=reverce_data(gui_disp_buf[page-1][i+64]);
			LcdWrData(temp_data);
		}
	}
	C2_DN();	
        temp_page=0;
	C1_EN();	
	for(page=8;page>=1;page--)
	{
		LcdWrCommand(C_LEFT_BASE+0);
		LcdWrCommand(C_TOP_BASE+temp_page++);
		for(i=63;i>=0;i--)
		{
                    temp_data=reverce_data(gui_disp_buf[page-1][i]);
			LcdWrData(temp_data);
		}
	}	
	C1_DN();
}
/********************
void LCD_DispRefurbish(void)
{
	INT32 i,page;	
 
    C1_DN();	
    C2_DN();	
    
	C1_EN();	
	for(page=0;page<8;page++)
	{
		LcdWrCommand(C_LEFT_BASE+0);
		LcdWrCommand(C_TOP_BASE+page);
		for(i=0;i<64;i++)
		{
			LcdWrData(gui_disp_buf[page][i]);
		}
	}
	C1_DN();	
    
	C2_EN();	
	for(page=0;page<8;page++)
	{
		LcdWrCommand(C_LEFT_BASE+0);
		LcdWrCommand(C_TOP_BASE+page);
		for(i=0;i<64;i++)
		{
			LcdWrData(gui_disp_buf[page][i+64]);
		}
	}	
	C2_DN();
}
********************************************/
/////////////////////////////////////////////////////////////////////////////

void GUI_UpdataDispBuf(UINT8 x, UINT8 y, UINT8 data)
{
	y = y>>3;
	gui_disp_buf[y][x] = data;
}

/****************************************************************************
* 名称：GUI_FillSCR()
* 功能：全屏填充。直接使用数据填充显示缓冲区。
* 入口参数：dat		填充的数据
* 出口参数：无
* 说明：用户根据LCM的实际情况编写此函数。
****************************************************************************/
void  GUI_FillSCR(TCOLOR dat)
{  uint32 i,j;
  
   // 填充缓冲区
   for(i=0; i<(GUI_LCM_YMAX/8); i++)
   {  for(j=0; j<GUI_LCM_XMAX; j++)
      {  gui_disp_buf[i][j] = dat;
      }
   }
   
   // 填充LCM
   LCD_DispFill(dat);
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
{  LCD_DispIni();					// 初始化LCM模块工作模式，纯图形模式
   GUI_FillSCR(0x00);				// 初始化缓冲区为0x00，并输出屏幕(清屏)
}


const UINT8   DEC_HEX_TAB[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
/****************************************************************************
* 名称：GUI_Point()
* 功能：在指定位置上画点。
* 入口参数：x		指定点所在列的位置
*           y		指定点所在行的位置
*           color	显示颜色(对于黑白色LCM，为0时灭，为1时显示)
* 出口参数：返回值为1时表示操作成功，为0时表示操作失败。
* 说明：操作失败原因是指定地址超出缓冲区范围。
****************************************************************************/
uint8  GUI_Point(uint8 x, uint8 y, TCOLOR color)
{  uint8   bak;
   
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


/****************************************************************************
* 名称：GUI_ReadPoint()
* 功能：读取指定点的颜色。
* 入口参数：x		指定点所在列的位置
*           y		指定点所在行的位置
*           ret     保存颜色值的指针
* 出口参数：返回0表示指定地址超出缓冲区范围
* 说明：对于单色，设置ret的d0位为1或0，4级灰度则为d0、d1有效，8位RGB则d0--d7有效，
*      RGB结构则R、G、B变量有效。
****************************************************************************/
uint8  GUI_ReadPoint(uint8 x, uint8 y, TCOLOR *ret)
{  uint8  bak;

   // 参数过滤
   if(x>=GUI_LCM_XMAX) return(0);
   if(y>=GUI_LCM_YMAX) return(0);
  
   bak = LCD_ReadByte(x,y);
   if( (bak & (DEC_HEX_TAB[y&0x07])) == 0 ) *ret = 0x00;
     else  *ret = 0x01;
   
   return(1);
}


/****************************************************************************
* 名称：GUI_HLine()
* 功能：画水平线。
* 入口参数：x0		水平线起点所在列的位置
*           y0		水平线起点所在行的位置
*           x1      水平线终点所在列的位置
*           color	显示颜色(对于黑白色LCM，为0时灭，为1时显示)
* 出口参数：无
* 说明：操作失败原因是指定地址超出缓冲区范围。
****************************************************************************/
void  GUI_HLine(uint8 x0, uint8 y0, uint8 x1, TCOLOR color) 
{  uint8  bak;

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


/***********************************************************************
* 名称：GUI_RLine()
* 功能：画竖直线。根据硬件特点，实现加速。
* 入口参数：x0		垂直线起点所在列的位置
*           y0		垂直线起点所在行的位置
*           y1      垂直线终点所在行的位置
*           color	显示颜色(对于黑白色LCM，为0时灭，为1时显示)
* 出口参数：	无
* 说明：操作失败原因是指定地址超出缓冲区范围。
***********************************************************************/
void  GUI_RLine(uint8 x0, uint8 y0, uint8 y1, TCOLOR color) 
{  uint8  bak;
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

/*
液晶矩形区反显示函数,即相当于光标显示
*/
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
			flag = 0;
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
