#ifndef __GIF_H__
#define __GIF_H__
#include "sys.h"    
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//图片解码 驱动代码-gif解码部分
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/3/14
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//升级说明 
//无
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////用户配置区//////////////////////////////////
#define GIF_USE_MALLOC		1 	//定义是否使用malloc,这里我们选择使用malloc	     
//////////////////////////////////////////////END/////////////////////////////////////


#define LCD_MAX_LOG_COLORS  256 
#define MAX_NUM_LWZ_BITS 	12


#define GIF_INTRO_TERMINATOR ';'	//0X3B   GIF文件结束符
#define GIF_INTRO_EXTENSION  '!'    //0X21
#define GIF_INTRO_IMAGE      ','	//0X2C

#define GIF_COMMENT     	0xFE
#define GIF_APPLICATION 	0xFF
#define GIF_PLAINTEXT   	0x01
#define GIF_GRAPHICCTL  	0xF9

typedef struct
{
	u8    aBuffer[258];                     // Input buffer for data block 
	short aCode  [(1 << MAX_NUM_LWZ_BITS)]; // This array stores the LZW codes for the compressed strings 
	u8    aPrefix[(1 << MAX_NUM_LWZ_BITS)]; // Prefix character of the LZW code.
	u8    aDecompBuffer[3000];              // Decompression buffer. The higher the compression, the more bytes are needed in the buffer.
	u8 *  sp;                               // Pointer into the decompression buffer 
	int   CurBit;
	int   LastBit;
	int   GetDone;
	int   LastByte;
	int   ReturnClear;
	int   CodeSize;
	int   SetCodeSize;
	int   MaxCode;
	int   MaxCodeSize;
	int   ClearCode;
	int   EndCode;
	int   FirstCode;
	int   OldCode;
}LZW_INFO;

//逻辑屏幕描述块
__packed typedef struct
{
	u16 width;		//GIF宽度
	u16 height;		//GIF高度
	u8 flag;		//标识符  1:3:1:3=全局颜色表标志(1):颜色深度(3):分类标志(1):全局颜色表大小(3)
	u8 bkcindex;	//背景色在全局颜色表中的索引(仅当存在全局颜色表时有效)
	u8 pixratio;	//像素宽高比
}LogicalScreenDescriptor;


//图像描述块
__packed typedef struct
{
	u16 xoff;		//x方向偏移
	u16 yoff;		//y方向偏移
	u16 width;		//宽度
	u16 height;		//高度
	u8 flag;		//标识符  1:1:1:2:3=局部颜色表标志(1):交织标志(1):保留(2):局部颜色表大小(3)
}ImageScreenDescriptor;

//图像描述
__packed typedef struct
{
	LogicalScreenDescriptor gifLSD;	//逻辑屏幕描述块
	ImageScreenDescriptor gifISD;	//图像描述快
	u16 colortbl[256];				//当前使用颜色表
	u16 bkpcolortbl[256];			//备份颜色表.当存在局部颜色表时使用
	u16 numcolors;					//颜色表大小
	u16 delay;					    //延迟时间
	LZW_INFO *lzw;					//LZW信息
}gif89a;

extern u8 gifdecoding;	//GIF正在解码标记.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
u8 gif_check_head(FIL *file);														    //检测GIF头
u16 gif_getrgb565(u8 *ctb);																//将RGB888转为RGB565
u8 gif_readcolortbl(FIL *file,gif89a * gif,u16 num);									//读取颜色表
u8 gif_getinfo(FIL *file,gif89a * gif);												    //得到逻辑屏幕描述,图像尺寸等
void gif_savegctbl(gif89a* gif);														//保存全局颜色表
void gif_recovergctbl(gif89a* gif);														//恢复全局颜色表
void gif_initlzw(gif89a* gif,u8 codesize);												//初始化LZW相关参数
u16 gif_getdatablock(FIL *gfile,u8 *buf,u16 maxnum);								   	//读取一个数据块
u8 gif_readextension(FIL *gfile,gif89a* gif, int *pTransIndex,u8 *pDisposal);		   	//读取扩展部分
int gif_getnextcode(FIL *gfile,gif89a* gif);										   	//从LZW缓存中得到下一个LZW码,每个码包含12位
int gif_getnextbyte(FIL *gfile,gif89a* gif);											//得到LZW的下一个码
u8 gif_dispimage(FIL *gfile,gif89a* gif,u16 x0,u16 y0,int Transparency, u8 Disposal);	//显示图片
void gif_clear2bkcolor(u16 x,u16 y,gif89a* gif,ImageScreenDescriptor pimge);		   	//恢复成背景色
u8 gif_drawimage(FIL *gfile,gif89a* gif,u16 x0,u16 y0);									//画GIF图像的一帧

u8 gif_decode(const u8 *filename,u16 x,u16 y,u16 width,u16 height);//在指定区域解码一个GIF文件.
void gif_quit(void);									//退出当前解码.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
