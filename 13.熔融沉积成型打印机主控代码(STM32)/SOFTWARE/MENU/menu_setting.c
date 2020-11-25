#include "menu_setting.h"
//#include "rtc.h" 	   
//#include "calendar.h" 	
#include "menu_mainui.h" 
#include "adc.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//APP-设置 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/7/20
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
 
_system_setings systemset;	  
#define SYS_MENU_SIZE 			12		//条目个数	
	 
//系统设置主目录表
u8*const sysset_mmenu_tbl[GUI_LANGUAGE_NUM][SYS_MENU_SIZE]=//系统一级目录的个数
{
	{
	"1.时间设置",
	"2.日期设置",
	"3.闹钟时间设置",
	"4.闹钟开关设置",
	"5.闹钟提示设置",
	"6.语言设置",
	"7.数码相框设置",
 	"8.屏幕校准",
	"9.系统文件更新",
	"10.系统信息",
	"11.系统状态", 
	"12.关于"
	},
	{			   								
	"1.時間設置",
	"2.日期設置",
	"3.鬧鐘時間設置",
	"4.鬧鐘開關設置",
	"5.鬧鐘提示設置",
	"6.語言設置",
	"7.數碼相框設置",  
 	"8.屏幕校準",
	"9.系統文件更新", 
	"10.系統信息",
	"11.系統狀態", 
	"12.關於"
	}, 	 
	{
	"1.TIME SET",
	"2.DATE SET",
	"3.ALARM TIME SET",
	"4.ALARM ON/OFF SET",
	"5.ALARM RING SET",
	"6.LANGUAGE SET",
	"7.DIGITAL PHOTO FRAME SET",  
 	"8.TOUCH SCREEN ADJUST", 
	"9.SYSTEM FILE UPDATE", 
	"10.SYSTEM INFORMATION",
	"11.SYSTEM STATUS", 
	"12.ABOUT"
	},
};	    
//语言设置列表
u8*const sysset_language_tbl[GUI_LANGUAGE_NUM]=
{
	"简体中文","繁體中文","English",
};  	    
//铃声设置列表
u8*const sysset_ring_tbl[GUI_LANGUAGE_NUM][4]=
{
{"DS1闪烁1","DS1闪烁2","DS1闪烁3","DS1闪烁4",},
{"DS1閃爍1","DS1閃爍2","DS1閃爍3","DS1閃爍4",},
{"DS1 FLASH 1","DS1 FLASH 2","DS1 FLASH 3","DS1 FLASH 4",},	 
};
//音乐/图片播放模式设置列表
u8*const sysset_avplaymode_tbl[GUI_LANGUAGE_NUM][3]=
{
{"全部循环","随机播放","单曲循环"},
{"全部循環","隨機播放","單曲循環"},
{"LOOP PLAY","SHUFFLE PLAY","SINGLE PLAY"},
};  
//系统设置相关提示信息 
u8*const sysset_system_remaindmsg_tbl[3][GUI_LANGUAGE_NUM]=
{
{"您确认更新系统文件?","您確認更新系統文件?","Are you sure to update?"},
{"您确认恢复默认设置?","您確認恢復默認設置?","Are you sure to restore?"},
{"恢复默认设置中...","恢復默認設置中...","Default set restoring...",},
};
//系统更新复制提示信息
u8*const sysset_system_update_cpymsg_tbl[2][GUI_LANGUAGE_NUM]=
{
{"正在复制:","正在複製:"," Copying:",},
{"当前文件夹:","當前文件夾:","Cur Folder:",},
}; 
//系统更新提示
u8*const sysset_system_update_msg_tbl[GUI_LANGUAGE_NUM]=
{"系统正在更新...","系統正在更新...","SYSTEM Updating..."};
//系统更新结果提示
u8*const sysset_system_update_err_tbl[3][GUI_LANGUAGE_NUM]=
{
{"系统文件更新成功!","系統文件更新成功!","SYSTEM file lost!",},
{"系统文件丢失!","系統文件丟失!","SYSTEM file lost!",},
{"用户终止更新!","用戶終止更新!","User stop update!",},
};
//系统信息标注表
u8*const sysset_system_info_caption_tbl[9][GUI_LANGUAGE_NUM]=
{
{"处理器:","處理器:","CPU:"},
{"内存:","內存:","RAM:"},
{"SD卡:","SD卡:","SD Card:"},
{"FLASH盘:","FLASH盤:","FLASH Disk:"}, 
{"操作系统:","操作系統:","OS:"},
{"图形界面:","圖形界面:","GUI:"},
{"硬件平台:","硬件平臺:","Hardware:"},
{"版权信息:","版權信息:","Copyright:"},
{"技术支持:","技術支持:","Tech Support:"},
};
//系统提示信息表
u8*const sysset_system_info_msg_tbl[9]=
{
"STM32F103RCT6 @ 72Mhz",
"48KB",
"MB",
"KB", 
"STM32裸机 ",
"XIAOYUAN GUI ",
"STM32RCT6 ",
"oTouch->TopWill",
"www.otouch.com",
}; 
//系统状提示信息
u8*const sysset_sysstatus_tbl[3][GUI_LANGUAGE_NUM]=
{
{"CPU使用率:","CPU使用率:","CPU USAGE:",},
{"内部内存使用率:","内部内存使用率:","IN MEMORY USAGE:",}, 
{"温度:","溫度:","TEMP:"},
};

//系统关于提示信息
u8*const sysset_system_about_caption_tbl[2][GUI_LANGUAGE_NUM]=
{
{"3D 打印机","ALIENTEK 迷你","ALENTEK Mini",},
{"产品ID号","產品序列號","Serial Number",},
}; 
////////////////////////////////////////////////////////////////////////////////////////////

#define SYSSET_BTN1_WIDTH			 50			//一类按键宽度(加减按钮)
#define SYSSET_BTN1_HEIGHT		 40			//一类按键高度(加减按钮)
#define SYSSET_BTN2_WIDTH			 60			//二类按键宽度(确认/取消按钮)
#define SYSSET_BTN2_HEIGHT		 30			//二类按键高度(确认/取消按钮)
#define SYSSET_BTN3_WIDTH			100			//三类按键宽度(单个按钮的宽度)
#define SYSSET_EDIT_WIDTH			 50			//文字编辑处的宽度
#define SYSSET_EDIT_HEIGHT		 25			//文字编辑处的高度
 
#define SYSSET_INWIN_FONT_COLOR		0X736C		//0XAD53		//窗体内嵌字颜色
#define SYSSET_EDIT_BACK_COLOR		0XE79E		//显示内容区背景色
 
/////////////////////////////////////////////////////////////////////////////////////////////

//时间/闹钟设置
//x,y:窗口坐标(窗口尺寸已经固定了的)	 
//hour,min:时分
//caption:窗口名字				  
//返回值:0,ok;其他,取消或者错误.
u8 sysset_time_set(u16 x,u16 y,u8 *hour,u8 *min,u8*caption) 
{
	u8 rval=0,res;
	u8 i;
	_window_obj* twin=0;	//窗体
 	_btn_obj * tbtn[6];		//总共六个按钮:0,时钟加按钮;1,时钟减按钮;2,分钟加按钮;3,分钟减按钮;4,确认按钮;5,取消按钮.		  
 	twin=window_creat(x,y,150,199,0,1|1<<5,16);//创建窗口
 	tbtn[0]=btn_creat(x+20,y+42,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);			//创建按钮
	tbtn[1]=btn_creat(x+20,y+42+67,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//创建按钮
	tbtn[2]=btn_creat(x+20+60,y+42,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//创建按钮
	tbtn[3]=btn_creat(x+20+60,y+42+67,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//创建按钮
	tbtn[4]=btn_creat(x+10,y+42+117,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);		//创建按钮
	tbtn[5]=btn_creat(x+10+70,y+42+117,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);	//创建按钮
	for(i=0;i<6;i++)
	{
		if(tbtn[i]==NULL)
		{
			rval=1;
			break;
		}
		if(i<4)//加减按键
		{
			tbtn[i]->bcfucolor=BLACK;//松开时为黑色
			tbtn[i]->bcfdcolor=WHITE;//按下时为白色			
			tbtn[i]->bkctbl[0]=0X453A;//边框颜色
			tbtn[i]->bkctbl[1]=0X5DDC;//第一行的颜色				
			tbtn[i]->bkctbl[2]=0X5DDC;//上半部分颜色
			tbtn[i]->bkctbl[3]=0X453A;//下半部分颜色
		}else//确认和取消按键
		{
			tbtn[i]->bkctbl[0]=0X8452;//边框颜色
			tbtn[i]->bkctbl[1]=0XAD97;//第一行的颜色				
			tbtn[i]->bkctbl[2]=0XAD97;//上半部分颜色
			tbtn[i]->bkctbl[3]=0X8452;//下半部分颜色
		}
		if(i==0||i==2)tbtn[i]->caption="＋";
		if(i==1||i==3)tbtn[i]->caption="－";
		if(i==4)tbtn[i]->caption=(u8*)GUI_OK_CAPTION_TBL[gui_phy.language];
		if(i==5)tbtn[i]->caption=(u8*)GUI_CANCEL_CAPTION_TBL[gui_phy.language];				
	}
    if(twin==NULL)rval=1;
	else
	{
		twin->caption=caption;
		twin->windowbkc=APP_WIN_BACK_COLOR;	     
	}
	if(rval==0)//无错误
	{
		window_draw(twin);					//画出窗体
		for(i=0;i<6;i++)btn_draw(tbtn[i]);	//画按钮
		gui_fill_rectangle(x+20+1,y+42+41,SYSSET_EDIT_WIDTH-2,SYSSET_EDIT_HEIGHT,SYSSET_EDIT_BACK_COLOR);  		//填充时钟背景
		gui_fill_rectangle(x+20+60+1,y+42+41,SYSSET_EDIT_WIDTH-2,SYSSET_EDIT_HEIGHT,SYSSET_EDIT_BACK_COLOR);	//填充分钟背景
 		app_show_nummid(x+20,y+42+41,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*hour,2,16,BLACK,SYSSET_EDIT_BACK_COLOR);
		app_show_nummid(x+20+60,y+42+41,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*min,2,16,BLACK,SYSSET_EDIT_BACK_COLOR);  
		while(rval==0)
		{
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
			delay_ms(1);		//延时一个时钟节拍
 			if(system_task_return)
			{
				rval=1;			//取消设置
				break;			//TPAD返回	
			}
			for(i=0;i<6;i++)
			{
				res=btn_check(tbtn[i],&in_obj);//确认按钮检测
				if(res)
				{
					if((tbtn[i]->sta&0X80)==0)//有有效操作
					{
						switch(i)
						{
							case 0://时钟增加按钮按下了
								(*hour)++;
								if(*hour>23)*hour=0;
								break;
							case 1://时钟减少按钮按下了	  
								if(*hour)(*hour)--;
								else *hour=23;
								break;
							case 2://分钟增加按钮按下了
								(*min)++;
								if(*min>59)(*min)=0;
								break;
							case 3://分钟减少按钮按下了	  
								if(*min)(*min)--;
								else *min=59;
								break;
							case 4://确认按钮按下  
								rval=0XFF;
								break;	  
							case 5://取消按钮按下	  
								rval=1;
								break;
 						}
					}
					app_show_nummid(x+20,y+42+41,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*hour,2,16,BLACK,SYSSET_EDIT_BACK_COLOR);
					app_show_nummid(x+20+60,y+42+41,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*min,2,16,BLACK,SYSSET_EDIT_BACK_COLOR);  

				}
			}
			
		}
 	}
	window_delete(twin);				//删除窗口
	for(i=0;i<6;i++)btn_delete(tbtn[i]);//删除按钮					   
	system_task_return=0;				//清除退出标志
	if(rval==0XFF)return 0;
	return rval;
}
//判断是否是闰年函数
//月份   1  2  3  4  5  6  7  8  9  10 11 12
//闰年   31 29 31 30 31 30 31 31 30 31 30 31
//非闰年 31 28 31 30 31 30 31 31 30 31 30 31
//year:年份
//返回值:该年份是不是闰年.1,是.0,不是
u8 sysset_is_leap_year(u16 year)
{			  
	if(year%4==0) //必须能被4整除
	{ 
		if(year%100==0) 
		{ 
			if(year%400==0)return 1;//如果以00结尾,还要能被400整除 	   
			else return 0;   
		}else return 1;   
	}else return 0;	
}	
//日期设置
//x,y:窗口坐标(窗口尺寸已经固定了的)
//year,month,date:年月日
//caption:窗口名字
//返回值:0,ok;其他,取消或者错误.
u8 sysset_date_set(u16 x,u16 y,u16 *year,u8 *month,u8 *date,u8*caption) 
{
	u8 rval=0,res;
	u8 i;
	u8 maxdate=31;			//2月份最大的天数
	_window_obj* twin=0;	//窗体
 	_btn_obj * tbtn[8];		//总共八个按钮:0,年份加按钮;1,年份减按钮;2,月份加按钮;3月份减按钮;4,日期加按钮;5,日期减按钮;6,确认按钮;7,取消按钮		  
 	twin=window_creat(x,y,190,199,0,1|1<<5,16);//创建窗口
 	tbtn[0]=btn_creat(x+10,y+42,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);			//创建按钮
	tbtn[1]=btn_creat(x+10,y+42+67,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//创建按钮
	tbtn[2]=btn_creat(x+10+60,y+42,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//创建按钮
	tbtn[3]=btn_creat(x+10+60,y+42+67,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//创建按钮
	tbtn[4]=btn_creat(x+10+120,y+42,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//创建按钮
	tbtn[5]=btn_creat(x+10+120,y+42+67,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);	//创建按钮

	tbtn[6]=btn_creat(x+20,y+42+117,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);		//创建按钮
	tbtn[7]=btn_creat(x+20+90,y+42+117,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);	//创建按钮
 	for(i=0;i<8;i++)
	{
		if(tbtn[i]==NULL)
		{
			rval=1;
			break;
		}
		if(i<6)//加减按键
		{
			tbtn[i]->bcfucolor=BLACK;//松开时为黑色
			tbtn[i]->bcfdcolor=WHITE;//按下时为白色			
			tbtn[i]->bkctbl[0]=0X453A;//边框颜色
			tbtn[i]->bkctbl[1]=0X5DDC;//第一行的颜色				
			tbtn[i]->bkctbl[2]=0X5DDC;//上半部分颜色
			tbtn[i]->bkctbl[3]=0X453A;//下半部分颜色
		}else//确认和取消按键
		{
			tbtn[i]->bkctbl[0]=0X8452;//边框颜色
			tbtn[i]->bkctbl[1]=0XAD97;//第一行的颜色				
			tbtn[i]->bkctbl[2]=0XAD97;//上半部分颜色
			tbtn[i]->bkctbl[3]=0X8452;//下半部分颜色
		}
		if(i==0||i==2||i==4)tbtn[i]->caption="＋";
		if(i==1||i==3||i==5)tbtn[i]->caption="－";
		if(i==6)tbtn[i]->caption=(u8*)GUI_OK_CAPTION_TBL[gui_phy.language];;
		if(i==7)tbtn[i]->caption=(u8*)GUI_CANCEL_CAPTION_TBL[gui_phy.language];;				
	}
    if(twin==NULL)rval=1;
	else
	{
		twin->caption=caption;
		twin->windowbkc=APP_WIN_BACK_COLOR;	     
	}
	if(rval==0)//无错误
	{
		window_draw(twin);					//画出窗体
		for(i=0;i<8;i++)btn_draw(tbtn[i]);	//画按钮
		gui_fill_rectangle(x+10+1,y+42+41,SYSSET_EDIT_WIDTH-2,SYSSET_EDIT_HEIGHT,SYSSET_EDIT_BACK_COLOR);  		//填充年份背景
		gui_fill_rectangle(x+10+60+1,y+42+41,SYSSET_EDIT_WIDTH-2,SYSSET_EDIT_HEIGHT,SYSSET_EDIT_BACK_COLOR);	//填充月份背景
 		gui_fill_rectangle(x+10+120+1,y+42+41,SYSSET_EDIT_WIDTH-2,SYSSET_EDIT_HEIGHT,SYSSET_EDIT_BACK_COLOR);	//填充日期背景

 		app_show_nummid(x+10,y+42+41,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*year,4,16,BLACK,SYSSET_EDIT_BACK_COLOR);
		app_show_nummid(x+10+60,y+42+41,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*month,2,16,BLACK,SYSSET_EDIT_BACK_COLOR);  
		app_show_nummid(x+10+120,y+42+41,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*date,2,16,BLACK,SYSSET_EDIT_BACK_COLOR);  
		while(rval==0)
		{
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
			delay_ms(1);		//延时一个时钟节拍
 			if(system_task_return)
			{
				rval=1;			//取消设置
				break;			//TPAD返回	
			}
			for(i=0;i<8;i++)
			{
				res=btn_check(tbtn[i],&in_obj);//确认按钮检测
				if(res)
				{
					if((tbtn[i]->sta&0X80)==0)//有有效操作
					{
						if(*month==2)
						{
							if(sysset_is_leap_year(*year))maxdate=29;//是闰年的2月份
	 						else maxdate=28;	  
						}else maxdate=31;  
						switch(i)
						{
							case 0://年份增加按钮按下了
								(*year)++;
								if(*year>2100)*year=2000;
								break;
							case 1://年份减少按钮按下了	  
								if(*year>2000)(*year)--;
								else *year=2100;
								break;
							case 2://月份增加按钮按下了
								(*month)++;
								if(*month>12)(*month)=1;
								break;
							case 3://月份减少按钮按下了	  
								if(*month>1)(*month)--;
								else *month=12;
								break;
							case 4://日期增加按钮按下了
								(*date)++;
								if(*date>maxdate)(*date)=1;
								break;
							case 5://日期减少按钮按下了	  
								if(*date>1)(*date)--;
								else *date=maxdate;
								break;
							case 6://确认按钮按下  
								rval=0XFF;
								break;	  
							case 7://取消按钮按下  
								rval=1;
								break;
 						}
					}   
			 		app_show_nummid(x+10,y+42+41,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*year,4,16,BLACK,SYSSET_EDIT_BACK_COLOR);
					app_show_nummid(x+10+60,y+42+41,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*month,2,16,BLACK,SYSSET_EDIT_BACK_COLOR);  
					app_show_nummid(x+10+120,y+42+41,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*date,2,16,BLACK,SYSSET_EDIT_BACK_COLOR);  
				}
			}
			
		}
 	}
	window_delete(twin);				//删除窗口
	for(i=0;i<8;i++)btn_delete(tbtn[i]);//删除按钮					   
	system_task_return=0;				//清除退出标志
	if(rval==0XFF)return 0;
	return rval;
}	  
//用于外部设置坐标
u16 cpymsg_x;
u16 cpymsg_y;
 //系统启动的时候,用于显示更新进度
//*pname:更新文件名字
//pct:百分比
//mode:模式
//[0]:更新文件名
//[1]:更新百分比pct
//[2]:更新文件夹
//[3~7]:保留
//返回值:0,正常;
//       1,结束复制			  
u8 sysset_system_update_cpymsg(u8*pname,u8 pct,u8 mode)
{		
	u16 filled; 
	if(system_task_return)return 1;//TPAD返回	
	if(mode&0X01)//显示文件名
	{  
 		gui_phy.back_color=APP_WIN_BACK_COLOR;
		gui_fill_rectangle(cpymsg_x+10+9*6,cpymsg_y+32+15,126,12,APP_WIN_BACK_COLOR);		//填充底色  0XE73C 
 		gui_show_string(pname,cpymsg_x+10+9*6,cpymsg_y+32+15,126,12,12,BLACK);				//显示新的文件名	
   		gui_show_string((u8*)sysset_system_update_cpymsg_tbl[1][gui_phy.language],cpymsg_x+10,cpymsg_y+32+15+21,66,12,12,BLACK);	
 	}
	if(mode&0X04)//显示文件夹名
	{
 		gui_phy.back_color=APP_WIN_BACK_COLOR;
		gui_fill_rectangle(cpymsg_x+10+11*6,cpymsg_y+32+15+21,114,12,APP_WIN_BACK_COLOR);	//填充底色  0XE73C 
 		gui_show_string(pname,cpymsg_x+10+11*6,cpymsg_y+32+15+21,114,12,12,BLACK);			//显示新的文件夹名	
	}
	if(mode&0X02)//更新百分比
	{
		if(pct>100)pct=100;
	    filled=pct;
		filled=(filled*9)/5;
		gui_fill_rectangle(cpymsg_x+10,cpymsg_y+32+15+21*2,filled,16,0X071F);				//填充占用了的百分比部分     
		gui_fill_rectangle(cpymsg_x+10+filled,cpymsg_y+32+15+21*2,180-filled,16,WHITE);		//填充未占用的百分比部分  
		gui_show_num(cpymsg_x+10+72,cpymsg_y+32+15+21*2+2,3,BLACK,12,pct,1);				//显示更新进度
		gui_show_string("%",cpymsg_x+10+72+18,cpymsg_y+32+15+21*2+2,6,12,12,BLACK);			//显示百分号
  	}
	delay_ms(1);//延时一个时钟节拍
	return 0;	
}
//更新系统文件夹
//caption:标题
void sysset_system_update(u8*caption,u16 sx,u16 sy)
{
  	_window_obj* twin=0;			//窗体
	u8 res;
	u8 *p;  
	res=app_system_file_check("0");
  	if(res)
	{
		p=(u8*)sysset_system_update_err_tbl[1][gui_phy.language];	//系统文件丢失
		window_msg_box(sx,sy+20,200,80,p,APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
		delay_ms(1500);
		system_task_return=0;//取消TPAD
		return;
	}
	twin=window_creat(sx,sy,200,120,0,1,16);//创建窗口
  	if(twin!=NULL)
	{
		//窗口的名字和背景色
		twin->caption=caption;
		twin->windowbkc=APP_WIN_BACK_COLOR;	
 		window_draw(twin);						//画出窗体
		gui_phy.back_color=APP_WIN_BACK_COLOR;
  		gui_show_string((u8*)sysset_system_update_cpymsg_tbl[0][gui_phy.language],sx+10,sy+32+15,54,12,12,BLACK);	//正在复制:
  		gui_show_string((u8*)sysset_system_update_cpymsg_tbl[1][gui_phy.language],sx+10,sy+32+15+21,66,12,12,BLACK);//当前文件夹:
 		gui_draw_rectangle(sx+10-1,sy+32+15+21*2-1,182,18,0X4A49);		//画边框
		gui_fill_rectangle(sx+10,sy+32+15+21*2,180,16,WHITE);			//填充底色  0XE73C 
		gui_show_num(sx+10+72,sy+32+15+21*2+2,3,BLACK,12,0,1);			//显示更新进度
		gui_show_string("%",sx+10+72+18,sy+32+15+21*2+2,6,12,12,BLACK);	//显示百分号
		cpymsg_x=sx;
		cpymsg_y=sy;
		res=app_system_update(sysset_system_update_cpymsg);				//从SD卡更新
	   	p=(u8*)sysset_system_update_err_tbl[0][gui_phy.language];		//更新成功提示
 		if(res==0XFF)p=(u8*)sysset_system_update_err_tbl[2][gui_phy.language];	//强制退出
		else if(res)p=(u8*)sysset_system_update_err_tbl[1][gui_phy.language];	//系统文件丢失
  		window_msg_box(sx,sy+20,200,80,p,APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
		delay_ms(1500); 
	}
	window_delete(twin);
	system_task_return=0;//取消TPAD
}  
//显示系统信息
//x,y:起始坐标
//宽度:200,高度:272.
//caption:标题
void sysset_system_info(u16 x,u16 y,u8*caption)
{
  	_window_obj* twin=0;			//窗体
 	_btn_obj * rbtn=0;				//返回按钮		  
	u8 rval=0;
	u8 *msgbuf;
	u8 numstrbuf[6];
 	u8 i;
	u32 dtsize,dfsize; 
	msgbuf=mymalloc(31);			//申请31个字节
  	twin=window_creat(x,y,260,210,0,1|1<<5,16);//创建窗口
 	rbtn=btn_creat(x+80,y+32+10+15*9,SYSSET_BTN3_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);//创建按钮
  	if(twin!=NULL&&rbtn!=NULL)
	{
		//窗口的名字和背景色
		twin->caption=caption;
		twin->windowbkc=APP_WIN_BACK_COLOR;	
 		//返回按钮的颜色    
		rbtn->bkctbl[0]=0X8452;//边框颜色
		rbtn->bkctbl[1]=0XAD55;//0XAD97;//第一行的颜色				
		rbtn->bkctbl[2]=0XAD55;//0XAD97;//上半部分颜色
		rbtn->bkctbl[3]=0XAD55;//0X8452;//下半部分颜色
 		rbtn->caption=(u8*)GUI_BACK_CAPTION_TBL[gui_phy.language];//标题为返回

 		window_draw(twin);	//画出窗体	
		btn_draw(rbtn);	  	//画按键
 		gui_draw_rectangle(x+10-4,y+32+6,260-12,15*9+5,0X4A49);		//画边框
 		gui_phy.back_color=APP_WIN_BACK_COLOR;
 		rval=strlen((const char*)sysset_system_info_caption_tbl[0][gui_phy.language]);
		//while(1); //调试节点
		for(i=0;i<9;i++)
		{
	 		strcpy((char*)msgbuf,(const char*)sysset_system_info_caption_tbl[i][gui_phy.language]);
	 		if(i==2||i==3)//SD卡/FLASH盘  容量
			{
				dtsize=0;
				dfsize=0;
				if(i==2)
				{
					if(i==2)exf_getfree("0:",&dtsize,&dfsize);	//得到SD卡剩余容量和总容量
					else exf_getfree("2:",&dtsize,&dfsize);		//得到SD卡剩余容量和总容量 
					dtsize>>=10;
					dfsize>>=10;
				}else exf_getfree("1:",&dtsize,&dfsize);//得到FLASH盘剩余容量和总容量  
  				gui_num2str(numstrbuf,dfsize);
				strcat((char*)msgbuf,(const char*)numstrbuf);//增加空闲容量
				if(i==2)strcat((char*)msgbuf,(const char*)"MB/");
				else strcat((char*)msgbuf,(const char*)"KB/"); 
				gui_num2str(numstrbuf,dtsize);
				strcat((char*)msgbuf,(const char*)numstrbuf);//增加总容量	  
			}
			strcat((char*)msgbuf,(const char*)sysset_system_info_msg_tbl[i]);
			if(i==4||i==5||i==6)
			{									   
				if(i==4)app_get_version(numstrbuf,100,3);	  	//OS版本
				else if(i==5)app_get_version(numstrbuf,GUI_VERSION,3);	//GUI版本
				else app_get_version(numstrbuf,HARDWARE_VERSION,2);		//硬件版本
				strcat((char*)msgbuf,(const char*)numstrbuf);		 
 			}     
	   		gui_show_string(msgbuf,x+15,y+32+10+15*i,180,12,12,BLACK);//显示信息
		} 
 		while(1)
		{
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
			delay_ms(1);		//延时一个时钟节拍
	 		if(system_task_return)break;			//TPAD返回	
			rval=btn_check(rbtn,&in_obj);			//返回按钮检测
			if(rval&&(rbtn->sta&0X80)==0)break;	 	//有有效操作
	 
		}
 	}
	myfree(msgbuf);
	window_delete(twin);
	btn_delete(rbtn);
	system_task_return=0;//取消TPAD
}	  

//显示系统状态
//x,y:显示的坐标
//宽度:200,高度:190.
//caption:名称
void sysset_system_status(u16 x,u16 y,u8* caption)
{
	u8 rval=0,res;
	u8 i=0;
	u16 temperate=0;
 	_window_obj* twin=0;			//窗体
 	_btn_obj * rbtn=0;				//返回按钮		  
	_progressbar_obj * cpuprgb=0;	//CPU使用率进度条
	_progressbar_obj * meminprgb=0;	//内部内存使用进度条 

 	twin=window_creat(x,y,200,190,0,1|1<<5,16);					//创建窗口
	cpuprgb=progressbar_creat(x+10,y+32+22,180,18,0X61);		//创建进度条
	meminprgb=progressbar_creat(x+10,y+32+22+45,180,18,0X61);	//创建进度条
	rbtn=btn_creat(x+50,y+32+22+45+38,SYSSET_BTN3_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);//创建按钮
 	if(twin==NULL||cpuprgb==NULL||meminprgb==NULL||rbtn==NULL)rval=1;
	else
	{
		//窗口的名字和背景色
		twin->caption=caption;
		twin->windowbkc=APP_WIN_BACK_COLOR;	
		//返回按钮的颜色    
		rbtn->bkctbl[0]=0X8452;	//边框颜色
		rbtn->bkctbl[1]=0XAD97;	//第一行的颜色				
		rbtn->bkctbl[2]=0XAD97;	//上半部分颜色
		rbtn->bkctbl[3]=0XAD97;//0X8452;	//下半部分颜色

		rbtn->caption=(u8*)GUI_BACK_CAPTION_TBL[gui_phy.language];//标题为返回

		cpuprgb->totallen=100; 			//最大是100
		meminprgb->totallen=100;    	//最大是100
														 
 		window_draw(twin);						//画出窗体
		btn_draw(rbtn);							//画按钮

		gui_show_string((u8*)sysset_sysstatus_tbl[0][gui_phy.language],x+10,y+32+5,190,12,12,SYSSET_INWIN_FONT_COLOR);		//显示CPU使用率
 		gui_show_string((u8*)sysset_sysstatus_tbl[1][gui_phy.language],x+10,y+32+5+45,190,12,12,SYSSET_INWIN_FONT_COLOR);	//显示内部内存使用率
 		gui_show_string((u8*)sysset_sysstatus_tbl[2][gui_phy.language],x+200-10-36-36,y+32+5,190,12,12,SYSSET_INWIN_FONT_COLOR);//显示温度
 		gui_show_string("℃",x+200-10-12,y+32+5,190,12,12,SYSSET_INWIN_FONT_COLOR);	//显示CPU当前温度
 	}
 	while(rval==0)
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
		delay_ms(1);		//延时一个时钟节拍
 		if(system_task_return)break;			//需要返回	
		res=btn_check(rbtn,&in_obj);			//返回按钮检测
		if(res)
		{
			if((rbtn->sta&0X80)==0)//有有效操作
			{
				break;//退出
			}
		}	
		if(i==0)//时间到了,更新内存,CPU等的状态
		{
//			cpuprgb->curpos=OSCPUUsage; //由于未挂操作系统，不能得到CPU使用率。
			meminprgb->curpos=mem_perused();//内部内存使用率

			progressbar_draw_progressbar(cpuprgb);	//显示进度条
			progressbar_draw_progressbar(meminprgb);//显示进度条
			//显示温度值
//			temperate=Get_Temp();//得到内部温度
			app_show_float(x+200-10-12,y+32+5,temperate,1,5,12,SYSSET_INWIN_FONT_COLOR,twin->windowbkc);//显示温度
  			i=100;
		}
		i--;
	}
	window_delete(twin);			//删除窗口
	btn_delete(rbtn);				//删除按钮	
	progressbar_delete(cpuprgb);	//删除进度条
	progressbar_delete(meminprgb);	//删除进度条
	system_task_return=0;		 	//清除退出标志
}
//关于系统
//caption:标题
void sysset_system_about(u16 x,u16 y,u8*caption)
{
  	_window_obj* twin=0;			//窗体
 	_btn_obj * rbtn=0;				//返回按钮		  
	u8 rval=0;
 	u32 sn0,sn1,sn2;
	u8 *tbuf;		  
	u8 verbuf[6];

 	tbuf=mymalloc(31);					//申请31个字节
  	twin=window_creat(x,y,260,200,0,1|1<<5,16);	//创建窗口
 	rbtn=btn_creat(x+80,y+32+120+10,SYSSET_BTN3_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);//创建按钮
  	if(twin!=NULL&&rbtn!=NULL)
	{
		//窗口的名字和背景色
		twin->caption=caption;
		twin->windowbkc=APP_WIN_BACK_COLOR;	
 		//返回按钮的颜色    
		rbtn->bkctbl[0]=0X8452;	//边框颜色
		rbtn->bkctbl[1]=0XAD97;	//第一行的颜色				
		rbtn->bkctbl[2]=0XAD97;	//上半部分颜色
		rbtn->bkctbl[3]=0XAD97;//0X8452;	//下半部分颜色
 		rbtn->caption=(u8*)GUI_BACK_CAPTION_TBL[gui_phy.language];//标题为返回

 		window_draw(twin);		//画出窗体	
		btn_draw(rbtn);	  		//画按键
 		gui_draw_rectangle(x+10-4,y+32+8,260-12,70,0X4A49);		//画边框
 		gui_draw_rectangle(x+10-4,y+32+80+10,260-12,35,0X4A49);		//画边框
 
  		gui_phy.back_color=APP_WIN_BACK_COLOR;
		gui_show_ptstr(x+10,y+32+0,x+10+180,y+32+10+16,0,BLACK,16,(u8*)sysset_system_about_caption_tbl[0][gui_phy.language],0);//ALIENTEK Mini
 	 	strcpy((char*)tbuf,"HARDWARE:");
		app_get_version(verbuf,HARDWARE_VERSION,2);
		strcat((char*)tbuf,(const char*)verbuf);
		strcat((char*)tbuf,", SOFTWARE:");						   
		app_get_version(verbuf,SOFTWARE_VERSION,3);
		strcat((char*)tbuf,(const char*)verbuf);
  		gui_show_string(tbuf,x+10,y+32+20,180,12,12,BLACK);//显示信息
 		gui_show_string("All rights reserved. Copyright(C) 2016",x+10,y+32+20+20*1,230,12,12,BLACK);//显示信息
 		gui_show_string("团队:oTouch->TopWill",x+10,y+32+20+20*2,160,12,12,BLACK);//显示信息
 		gui_show_string("网址:www.otouch.com",x+10+100+25,y+32+20+20*2,180,12,12,BLACK);//显示信息
 
  		gui_show_ptstr(x+10,y+32+80+5,x+10+180,y+32+120+10+16,0,BLACK,16,(u8*)sysset_system_about_caption_tbl[1][gui_phy.language],0);//产品序列号
		app_getstm32_sn(&sn0,&sn1,&sn2);//得到序列号
		sprintf((char*)tbuf,"ID:%X%X%X",sn0,sn1,sn2);
		gui_show_string(tbuf,x+10,y+32+80+25,180,12,12,BLACK);//显示信息
  		while(1)
		{
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
			delay_ms(1);		//延时一个时钟节拍
	 		if(system_task_return)break;			//TPAD返回	
			rval=btn_check(rbtn,&in_obj);			//返回按钮检测
			if(rval&&(rbtn->sta&0X80)==0)break;	 	//有有效操作
	 
		}
 	}
	myfree(tbuf);
	window_delete(twin);
	btn_delete(rbtn);
	system_task_return=0;//取消TPAD
} 

//查找条目名字
//mcaption:主目录下的目录名(一定要有'.'字符在里面)
u8 * set_search_caption(const u8 *mcaption)
{
	while(*mcaption!='.')mcaption++;
	return (u8*)(++mcaption);
}
//系统设置数据保存在:SYSTEM_PARA_SAVE_BASE
//读取系统设置信息
//sysset:系统信息 
void sysset_read_para(_system_setings * sysset)
{
	AT24CXX_Read(SYSTEM_PARA_SAVE_BASE,(u8*)sysset,sizeof(_system_setings));
}
//写入系统设置信息
//sysset:系统信息 
void sysset_save_para(_system_setings * sysset)
{
//  	OS_CPU_SR cpu_sr=0;
//	OS_ENTER_CRITICAL();//进入临界区(无法被中断打断) 
	AT24CXX_Write(SYSTEM_PARA_SAVE_BASE,(u8*)sysset,sizeof(_system_setings));
//	OS_EXIT_CRITICAL();	//退出临界区(可以被中断打断)
}

//系统设置 
u8 sysset_play(void)
{	
	u8 tlanguage;			//进入时语言的类型					  	   
	u8 res;		 
	u8 selitem=0;
	u8 topitem=0;  	 
	u8 **items;	     					   	   
 	u8 *caption;			//标题列表 
	u8 *scaption;			//子标题
	u16 temp1;
	u8 temp2,temp3;			//时间/日期暂存
	u8 savemask=0;		    //0,不需要保存任何数据;
							//[0]:systemset数据改动标记
							//[1]:vsset数据改动标记
							//[2]:alarm数据改动标记	 
  
    caption=(u8*)APP_MFUNS_CAPTION_TBL[5][gui_phy.language];	 
	items=(u8**)sysset_mmenu_tbl[gui_phy.language];
	tlanguage=gui_phy.language;							//保存刚进入是语言的种类
	while(1)
	{   						   
   		res=app_listbox_select(&selitem,&topitem,caption,items,SYS_MENU_SIZE);
 		if(system_task_return)
		{	
			break;					//TPAD返回
		}
			if(res&0X80)
		{	  
			scaption=set_search_caption(items[selitem]); 	 						   
			switch(selitem)//判断选择的条目
			{
				case 0://时间设置 			    
//					RTC_Get();//更新到最新的时间 
//					temp1=calendar.hour;
//					temp2=calendar.min;
//					res=sysset_time_set((lcddev.width-150)/2,(lcddev.height-200)/2,(u8*)&temp1,(u8*)&temp2,scaption);
//					if(res==0)//成功
//					{
//						RTC_Set(calendar.w_year,calendar.w_month,calendar.w_date,temp1,temp2,0);//设置时间
//					} 
  					break;	  
				case 1://日期设置  	    
//					RTC_Get();//更新到最新的时间 
//					temp1=calendar.w_year;
//					temp2=calendar.w_month;	 
//					temp3=calendar.w_date;
//					res=sysset_date_set((lcddev.width-190)/2,(lcddev.height-200)/2,&temp1,(u8*)&temp2,(u8*)&temp3,scaption);
//					if(res==0)//成功
//					{
//						RTC_Set(temp1,temp2,temp3,calendar.hour,calendar.min,calendar.sec);//设置时间
//					} 																	  
					break;
				case 2://闹钟时间设置
//					temp1=alarm.hour;
//					temp2=alarm.min;
//					res=sysset_time_set((lcddev.width-150)/2,(lcddev.height-200)/2,(u8*)&temp1,(u8*)&temp2,scaption);
//					if(res==0)//成功,点击了确认按钮
//					{
//						alarm.hour=temp1;
//						alarm.min=temp2;
//						calendar_alarm_init(&alarm,&calendar);//重新初始化闹钟
//						savemask|=1<<2;//标记闹钟数据改动了
//					} 
  					break;
				case 3://闹钟开关设置
//					temp1=alarm.weekmask;
//			 		res=app_items_sel((lcddev.width-200)/2,(lcddev.height-302)/2,200,302,(u8**)calendar_week_table[gui_phy.language],7,(u8*)&temp1,0XB0,scaption);//多选
// 					if(res==0)//设置成功
//					{
//						alarm.weekmask=temp1;
//						calendar_alarm_init(&alarm,&calendar);//重新初始化闹钟	
//						savemask|=1<<2;//标记闹钟数据改动了
//					}   
					break;
				case 4://闹钟铃声类型设置
//					temp1=alarm.ringsta&0X03;
//			 		res=app_items_sel((lcddev.width-180)/2,(lcddev.height-232)/2,180,72+40*4,(u8**)sysset_ring_tbl[gui_phy.language],4,(u8*)&temp1,0X90,scaption);//单选
//					if(res==0)//设置成功
//					{
//						alarm.ringsta&=0XFC;//取消最后两位原来的设置
//						alarm.ringsta|=temp1;//重新设置铃声						    
//						savemask|=1<<2;//标记闹钟数据改动了
//					}   
					break;
				case 5://语言设置
					temp1=gui_phy.language;//得到之前的设置
			 		res=app_items_sel((lcddev.width-180)/2,(lcddev.height-192)/2,180,72+40*3,(u8**)sysset_language_tbl,3,(u8*)&temp1,0X90,scaption);//单选
					if(res==0)//设置成功
					{												   
						gui_phy.language=temp1;		//重新设置语言
						systemset.syslanguage=temp1;//重新设置默认语言
					    caption=(u8*)APP_MFUNS_CAPTION_TBL[5][gui_phy.language];	 
						items=(u8**)sysset_mmenu_tbl[gui_phy.language];			 
						savemask|=1<<0;//标记系统设置数据改动了
					} 
					break; 
				case 6://图片浏览模式设置				 
					temp1=systemset.picmode;//得到之前的设置
			 		res=app_items_sel((lcddev.width-180)/2,(lcddev.height-152)/2,180,72+40*2,(u8**)sysset_avplaymode_tbl[gui_phy.language],2,(u8*)&temp1,0X90,scaption);//2个模式
					if(res==0)//设置成功
					{												   
						systemset.picmode=temp1;//重设图片浏览模式			   		 
						savemask|=1<<0;//标记系统设置数据改动了
					} 
					break; 	 
				case 7://屏幕校准  
					if((tp_dev.touchtype&0X80)==0)TP_Adjust(); //电阻屏才需要校准
					system_task_return=0;	//清除退出标志
					break; 
				case 8://系统更新
					res=window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,"",(u8*)sysset_system_remaindmsg_tbl[0][gui_phy.language],12,0,0X03,0);
					if(res==1)//需要更新
					{
				    	sysset_system_update((u8*)sysset_system_update_msg_tbl[gui_phy.language],(lcddev.width-200)/2,(lcddev.height-120)/2-10);
					}
					system_task_return=0;//取消TPAD 
					break;
				case 9://系统信息	
					sysset_system_info((lcddev.width-260)/2,(lcddev.height-200)/2,scaption);
					break;
				case 10://系统状态 显示CPU使用率/内存状态等
					sysset_system_status((lcddev.width-200)/2,(lcddev.height-190)/2,scaption);
					break;
				case 11://关于
					sysset_system_about((lcddev.width-260)/2,(lcddev.height-200)/2,scaption);
					break;	  				 				
			}
  	}
		else 
		{
			mui_init(1); //返回主界面
			break;//有错误发生了	 			   
		}
	}
	if(savemask&1<<0)//系统设置数据改动了
	{
		printf("sysset save!\r\n");
		sysset_save_para(&systemset);	//保存系统设置信息
		mui_init(0); 					//Main UI 初始化		
	} 
//	if(savemask&1<<2)//闹钟数据改动了
//	{
//		printf("alarm save!\r\n");
//		calendar_save_para(&alarm);		//保存闹钟设置信息 
//	}   
	if(tlanguage!=gui_phy.language)		//语言发生了改变
	{
		LCD_Clear(BLACK); 
	}
	return res;			   							  
}






















