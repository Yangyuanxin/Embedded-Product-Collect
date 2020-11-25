#ifndef __FILELISTBOX_H
#define __FILELISTBOX_H 
#include "gui.h"  	 			  
#include "scrollbar.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//GUI-文件列表框 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2012/10/4
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//********************************************************************************
//升级说明 
//V1.1 20140715
//1,修改滚动条检测机制,使用更方便
//2,去掉FLBOX_PATH_HEIGHT等宏定义,改用变量方式控制(在gui_phy结构体里面设置)
//////////////////////////////////////////////////////////////////////////////////

//文件图标定义 //对应于下面结构体中定义的type
#define FICO_DISK				0  			//磁盘
#define FICO_FOLDER		 		1			//文件夹
#define FICO_UNKNOW				2			//未知文件
#define FICO_BIN				3			//BIN文件
#define FICO_LRC				4			//LRC文件
#define FICO_NES				5			//NES文件
#define FICO_TEXT				6			//文本文件
#define FICO_MUSIC				7			//音乐文件
#define FICO_PICTURE			8			//图片文件
#define FICO_VIDEO				9			//视频文件
#define FICO_GCODE				10			//GCODE文件

//文件过滤器个位功能定义 //这些宏定义的配置一定要与exfuns.c文件中的“文件类型列表”数组相一致。
#define FLBOX_FLT_BIN 			1<<0		//BIN文件
#define FLBOX_FLT_LRC	 		1<<1		//LRC文件
#define FLBOX_FLT_NES	 		1<<2		//NES文件
#define FLBOX_FLT_TEXT	 		1<<3		//文本文件
#define FLBOX_FLT_MUSIC	 		1<<4		//音乐文件
#define FLBOX_FLT_PICTURE 		1<<5		//图片文件
#define FLBOX_FLT_VIDEO			1<<6		//视频文件
#define FLBOX_FLT_GCODE 		1<<7		//GCODE文件
#define FLBOX_FLT_UNKNOW		1<<15		//无法识别的一类文件

//////filelistbox上的符号的默认颜色 
#define FLBOX_DFT_LBKCOLOR		0XFFFF		//内部背景色
#define FLBOX_DFT_LSELCOLOR		0X0000//0XFFFF		//选中list后的字体颜色
#define FLBOX_DFT_LSELBKCOLOR	0XAF7D//0X001F		//0X07E0		//选中list后的背景色
#define FLBOX_DFT_LNCOLOR		0X0000		//未选中的list字体颜色
#define FLBOX_DFT_RIMCOLOR		0X0000//0XF800		//边框颜色
#define FLBOX_DFT_PTHINFBKCOLOR	0XFFFF//0XCE7E		//路径/信息栏的背景颜色


//filelistbox文件存储深度定义,如果超过这个深度了,将不再记忆上一层所选项目的位置.
#define FLBOX_PATH_DEPTH		0X0A		//文件路径深度
//filelistbox单个文件夹下面的最大目标文件索引数目
#define FLBOX_MAX_DFFILE 		2000		//使用12*12的字体 2K个,在搜索文件夹的时候,需要占用2K*2的内存

//固定宽高度定义
#define FLBOX_SCB_WIDTH 		0X14//0X0E		//滚动条宽度为15个像素 
//filelist结构体.链表结构
__packed typedef struct 
{
	void * prevlist;
	void * nextlist;
	u8 type;		//文件类型
	u8 *name;		//文件名字
}_filelistbox_list;

//filelistbox结构体定义		  
__packed typedef struct 
{
	u16 top; 				  		//filelistbox顶端坐标
	u16 left;                       //filelistbox左端坐标
	u16 width; 				  		//宽度
	u16 height;						//高度 必须为12/16的倍数,如果有边框,在倍数基础上加2

	u8 type;						//类型标记字
									//[bit7]:1,需要画滚动条出来(条件是totalitems>itemsperpage);0,不需要画出来.(此位由软件自动控制)
									//[bit6:0]:文件类型					 
								
	u8 sta;							//filelistbox状态,[bit7]:滑动标志;[bit6]:编号有效的标志;[bit5:0]:第一次按下的编号.	 
	u8 id;							//filelistbox 的id
	u8 dbclick;					   	//双击,
									//[7]:0,没有双击.1,有双击.
									//[0]:0,非目标文件(是文件夹/磁盘).1,目标文件(非文件夹/磁盘)

	u8 font;						//文字字体 12/16
	u16 selindex;					//选中的索引 0<selindex<foldercnt+filecnt;

	u16 foldercnt;					//文件夹数目
	u16 filecnt;					//文件数目
	
	u8 icosize;						//ico图标大小 16/20/28等
 
	u16 lbkcolor;					//内部背景颜色
 	u16 lnselcolor; 				//list name 选中后的颜色
  	u16 lnselbkcolor; 				//list name 选中后的背景颜色
	u16 lncolor; 					//list name 未选中的颜色
	u16 rimcolor;					//边框颜色
	u16 pthinfbkcolor;			    //路径/信息栏的背景颜色

	u8* path;						//父目录路径
	//u8  seltblindex;				//当前的目录层数
	u16 seltbl[FLBOX_PATH_DEPTH];	//选中的条目列表,FLBOX_PATH_DEPTH为深度
	u16 fliter;						//文件过滤器,大类过滤
									//刷选表见上面的定义
	u16 fliterex;					//文件扩展的过滤器
									//用来选择大类里面的小类,最多支持16种不同的文件.默认0XFFFF,即不对小类进行刷选.

	u8 memdevflag;					//磁盘在位状态(最多可以表示8个磁盘)
									//bit7:0,存储设备7~0在位状态;0,不在位;1,在位;
									//本例中,存储设备0表示:SD卡
									//              1表示:SPI FLASH
									//              2表示:U盘
//////////////////////
	u8 *fname;						//当前选中的index的名字
	u16 namelen;					//name所占的点数.
	u16 curnamepos;					//当前的偏移
	u32 oldtime;					//上一次更新时间 

    //以下只与目标文件相关(不含文件夹)
 	u16 *findextbl;    				//目标文件的索引表	

	_scrollbar_obj * scbv;			//垂直滚动条
	_filelistbox_list *list; 		//链表	    
}_filelistbox_obj;


_filelistbox_obj * filelistbox_creat(u16 left,u16 top,u16 width,u16 height,u8 type,u8 font);//创建filelistbox
_filelistbox_list * filelist_search(_filelistbox_list *filelistx,u16 index);		//查找编号为index的list,并取得详细信息
void filelist_delete(_filelistbox_obj * filelistbox);						//删除filelist链表
void filelistbox_delete(_filelistbox_obj *filelistbox_del);	 				//删除filelistbox
u8 filelistbox_rebuild_filelist(_filelistbox_obj * filelistbox);			//重建filelistbox
u8 filelistbox_check(_filelistbox_obj * filelistbox,void * in_key);			//检查filelistbox的按下状态
u8 filelistbox_addlist(_filelistbox_obj * filelistbox,u8 *name,u8 type);	//增加一个filelist
void filelistbox_draw_list(_filelistbox_obj *filelistbox);					//画filelist
void filelistbox_draw_listbox(_filelistbox_obj *filelistbox);				//画filelistbox
void filelistbox_show_path(_filelistbox_obj *filelistbox); 					//显示路径
void filelistbox_show_info(_filelistbox_obj *filelistbox);
u8   filelistbox_scan_filelist(_filelistbox_obj *filelistbox);
void filelistbox_add_disk(_filelistbox_obj *filelistbox);
void filelistbox_check_filelist(_filelistbox_obj *filelistbox);
void filelistbox_back(_filelistbox_obj *filelistbox);
void filelistbox_2click_hook(_filelistbox_obj *filelistbox);				//双击钩子函数
#endif





