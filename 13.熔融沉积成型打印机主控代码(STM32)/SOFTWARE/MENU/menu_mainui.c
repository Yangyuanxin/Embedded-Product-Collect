//#include "includes.h" 
//#include "dma.h"
#include "gui_conf.h"
#include "picdecode.h"
#include "touch.h"
#include "menu_mainui.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//主界面控制 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/2/22
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//主界面 控制器
m_mui_dev muidev;


//icos的路径表
u8*const mui_icos_path_tbl[6]=
{ 
	"1:/SYSTEM/SYSICO/disk.bmp",
	"1:/SYSTEM/SYSICO/piano.bmp",	    
	"1:/SYSTEM/SYSICO/calculator.bmp",	    
	"1:/SYSTEM/SYSICO/console.bmp",	    
	"1:/SYSTEM/SYSICO/movie.bmp",	    
	"1:/SYSTEM/SYSICO/binoculars.bmp",	    
	//"1:/SYSTEM/SYSICO/paint.bmp",	    
	//"1:/SYSTEM/SYSICO/wireless.bmp",	    
	//"1:/SYSTEM/SYSICO/notebook.bmp", 	    
};  

//各个ICOS的对应功能名字
u8*const icos_name_tbl[GUI_LANGUAGE_NUM][6]=
{ 
	{
		"磁盘","钢琴","计算器",
		"调试","电影","望远镜",
		//"画板","无线传输","记事本",	   
	}, 
	{
		"電子圖書","數碼相框","USB連接",
		"應用中心","時鐘","系統設置",
		//"觸摸畫板","無線傳輸","記事本",	   
	}, 
	{
		"EBOOK","PHOTOS","USB",
		"APP","TIME","SETTINGS",
		//"PAINT","WIRELESS","NOTEPAD",	   
	},  
};						  

//初始化spb各个参数, 并加载主界面	
//mode:0,仅仅更新icos的名字
//     1,更新名字，并加载图片
void mui_init(uint8_t mode)
{
	u16 i,j;
	u16 offx=0,offy=0;
	//if(lcddev.width<240||lcddev.height<320)return ;//屏幕尺寸不能小于320*240;
	offx=(lcddev.width-320)/2;
	offy=(lcddev.height-240)/2;
	muidev.status=0x0F;
	for(i=0;i<2;i++)  //循环为每个图标文件赋属性
	{
		for(j=0;j<3;j++)
		{
			muidev.icos[i*3+j].x=10+j*105+offx;
			muidev.icos[i*3+j].y=10+i*115+offy;
			muidev.icos[i*3+j].width=95;   //图标像素宽度90+5
			muidev.icos[i*3+j].height=105; //图标像素高度90+文字高度12+3
			muidev.icos[i*3+j].path=(u8*)mui_icos_path_tbl[i*3+j];
			muidev.icos[i*3+j].name=(u8*)icos_name_tbl[gui_phy.language][i*3+j]; 
		}
	} 
	if(mode==0)return;
	mui_load_icos();
	//指向lcd
	gui_phy.read_point=LCD_ReadPoint;
	gui_phy.draw_point=LCD_Fast_DrawPoint;
	gui_phy.lcdwidth=lcddev.width;
	gui_phy.lcdheight=lcddev.height; 
	pic_phy.read_point=LCD_ReadPoint;
	pic_phy.draw_point=LCD_Fast_DrawPoint; 
}
//设置主界面语言种类
//language:0,简体中文;1,繁体中文;2,英文
void mui_language_set(u8 language)
{
	u8 i;
	if(language>2)return;
	for(i=0;i<6;i++)muidev.icos[i].name=(u8*)icos_name_tbl[language][i]; 
}
//装载ICOS
void mui_load_icos(void)
{
	u8 i,j; 
	LCD_Fill(0,0,lcddev.width,8,BLACK);
	LCD_Fill(0,0,8,lcddev.height,BLACK);
	LCD_Fill(0,lcddev.height-8,lcddev.width,lcddev.height,BLACK);
	LCD_Fill(lcddev.width-8,0,lcddev.width,lcddev.height,BLACK);  
	gui_draw_arcrectangle(2,2,lcddev.width-4,lcddev.height-4,6,1,MUI_IN_BACKCOLOR,MUI_IN_BACKCOLOR);
	for(i=0;i<2;i++)
	{
		for(j=0;j<3;j++)
		{
			minibmp_decode(muidev.icos[i*3+j].path,muidev.icos[i*3+j].x+5,muidev.icos[i*3+j].y,muidev.icos[i*3+j].width,muidev.icos[i*3+j].height,0,0);
			gui_show_strmid(muidev.icos[i*3+j].x,muidev.icos[i*3+j].y+92,muidev.icos[i*3+j].width,12,MUI_FONT_COLOR,12,muidev.icos[i*3+j].name);//显示名字  
		}
	}  	
} 
//设置选中哪个图标
//sel:0~8代表当前页的选中ico
void mui_set_sel(u8 sel)
{
	u8 oldsel=muidev.status&0X0F;
	if(oldsel<6) //清除图标的选中状态
	{
		LCD_Fill(muidev.icos[oldsel].x-5,muidev.icos[oldsel].y-5,muidev.icos[oldsel].x+muidev.icos[oldsel].width,muidev.icos[oldsel].y+muidev.icos[oldsel].height,MUI_IN_BACKCOLOR);
		minibmp_decode(muidev.icos[oldsel].path,muidev.icos[oldsel].x+0,muidev.icos[oldsel].y,muidev.icos[oldsel].width,muidev.icos[oldsel].height,0,0);		
		gui_show_strmid(muidev.icos[oldsel].x,muidev.icos[oldsel].y+92,muidev.icos[oldsel].width,12,MUI_FONT_COLOR,12,muidev.icos[oldsel].name);//显示名字
	}
	muidev.status=(muidev.status&0XF0)+sel;  

	LCD_Fill(muidev.icos[sel].x-5,muidev.icos[sel].y-5,muidev.icos[sel].x+muidev.icos[sel].width,muidev.icos[sel].y+muidev.icos[sel].height,MUI_BACK_COLOR);
	minibmp_decode(muidev.icos[sel].path,muidev.icos[sel].x+0,muidev.icos[sel].y,muidev.icos[sel].width,muidev.icos[sel].height,0,0);		

  	gui_show_strmid(muidev.icos[sel].x,muidev.icos[sel].y+92,muidev.icos[sel].width,12,MUI_FONT_COLOR,12,muidev.icos[sel].name);//显示名字
} 

//屏幕触摸检测
//返回值:0~8,被双击的图标编号.		    
//       0xff,没有任何图标被双击或者按下
u8 mui_touch_chk(void)
{		 
	u8 i=0xff;
	tp_dev.scan(0);//扫描																		 
	if(tp_dev.sta&TP_PRES_DOWN)//有按键被按下
	{
		muidev.status|=0X80;	//标记有有效触摸
		muidev.tpx=tp_dev.x[0];
		muidev.tpy=tp_dev.y[0];
	}else if(muidev.status&0X80)//按键松开了,并且有有效触摸
	{
		
		for(i=0;i<6;i++)
		{
			if((muidev.tpx>muidev.icos[i].x)&&(muidev.tpx<muidev.icos[i].x+muidev.icos[i].width)&&(muidev.tpx>muidev.icos[i].x)&&
			   (muidev.tpy<muidev.icos[i].y+muidev.icos[i].height))
			{
				break;//得到选中的编号	
			}
		}
		if(i<6)
		{
			if(i!=(muidev.status&0X0F))	//选中了不同的图标,切换图标
			{
				mui_set_sel(i);
				i=0xff;
			}else 
			{ 
				muidev.status|=0X0F;//清零选中的图标
			}
		}else i=0xff;//无效的点按.
		muidev.status&=0X7F;//清除按键有效标志
	} 	 
	return i; 
}














































