 #include "menu_picview.h"
#include "picdecode.h"
#include "menucontrol.h"
#include "cardreader.h"
#include "menu_setting.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//APP-数码相框实现 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/2/16
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   



//图片浏览
u8 picviewer_play(void)
{				 	 
	DIR picdir;			//picdir专用	  
	FILINFO picinfo;   		   
	u8 rval=0;			//返回值	  
	u8 *pname=0;
	u8 *fn=0;
	u8 picsta=0;		//ebook状态
						//0,属于文件浏览状态    
						//1,顺序播放图片
						//2,暂停状态    
	
	u16 curindex=0;	//当前浏览的图片文件的索引号
	u8  endecode=0;	//使能解码
	u8 key; 
	u16 dtime=0;	//延时时间
	u8 keyup=1;		//松开标记
	u8 pictype=0;	//图片类型 

  _filelistbox_obj * flistbox;   //文件夹
	_filelistbox_list * filelistx; 	//文件
 	_btn_obj* rbtn;		//返回按钮控件

	app_filebrower((u8*)APP_MFUNS_CAPTION_TBL[1][gui_phy.language],0X07);//选择目标文件,并得到目标数量
   	flistbox=filelistbox_creat(0,gui_phy.tbheight,lcddev.width,lcddev.height-gui_phy.tbheight*2,1,gui_phy.listfsize);//创建一个filelistbox
 	if(flistbox==NULL)rval=1;							//申请内存失败.
	else  
	{
		flistbox->fliter=FLBOX_FLT_PICTURE;				//图片文件
		filelistbox_add_disk(flistbox);					//添加磁盘路径
 		filelistbox_draw_listbox(flistbox);
	} 	  
	//为长文件名申请缓存区
 	picinfo.lfsize = _MAX_LFN * 2 + 1;
	picinfo.lfname = gui_memin_malloc(picinfo.lfsize);
	if(picinfo.lfname==NULL)rval=1;//申请内存失败 
   	else gui_memset((u8*)picinfo.lfname,0,picinfo.lfsize);
 	rbtn=btn_creat(lcddev.width-2*gui_phy.tbfsize-8-1,lcddev.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//创建文字按钮
 	if(rbtn==NULL)rval=1;	//没有足够内存够分配
	else
	{																				
	 	rbtn->caption=(u8*)GUI_BACK_CAPTION_TBL[gui_phy.language];//返回 
	 	rbtn->font=gui_phy.tbfsize;//设置新的字体大小	 	 
		rbtn->bcfdcolor=WHITE;	//按下时的颜色
		rbtn->bcfucolor=WHITE;	//松开时的颜色
		btn_draw(rbtn);//画按钮
	}
	LED1=1;//关闭LED1
	while(rval==0)//主循环
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
		delay_ms(5);
		//if(system_task_return)break;			//TPAD返回   	
		if(picsta==0)
		{
			filelistbox_check(flistbox,&in_obj);	//扫描文件
	 		if(flistbox->dbclick==0X81)				//双击文件了
			{	
				curindex=flistbox->selindex-flistbox->foldercnt;//得到当前图片索引号
				picsta=1;	//图片播放状态		 
				endecode=1;	//第一张图片自动播放 
				LCD_Clear(0x0);//黑屏 	 
			}
		}
		key=btn_check(rbtn,&in_obj);
  		if(key&&((rbtn->sta&0X80)==0))
		{		  
			if(flistbox->dbclick!=0X81)//在文件浏览的时候按了返回按钮,则返回上一层目录
			{
				filelistx=filelist_search(flistbox->list,flistbox->selindex);//得到此时选中的list的信息
				if(filelistx->type==FICO_DISK)//已经不能再往上了,则退出图片浏览
				{				 
					currentMenu = main_interface; //将当前界面指定到主界面
					break;
				}else filelistbox_back(flistbox);//退回上一层目录	 
			} 
		} 
		if(endecode)
		{
     	rval=f_opendir(&picdir,(const TCHAR*)flistbox->path); //打开选中的目录
			if(rval)break;	 			   
			dir_sdi(&picdir,flistbox->findextbl[curindex]);
	 		rval=f_readdir(&picdir,&picinfo);//读取文件信息
			if(rval)break;//打开成功    
			fn=(u8*)(*picinfo.lfname?picinfo.lfname:picinfo.fname);
			pname=gui_memin_malloc(strlen((const char*)fn)+strlen((const char*)flistbox->path)+2);//申请内存
			if(pname==NULL)break;	//申请失败
			pname=gui_path_name(pname,flistbox->path,fn);	//文件名加入路径 
			pictype=f_typetell(pname); 
			if(pictype==T_GIF)gui_show_string(fn,5,5,lcddev.width-5,gui_phy.tbfsize,gui_phy.tbfsize,RED);	//显示GIF名字
			ai_load_picfile(pname,0,0,lcddev.width,lcddev.height,1);			//播放这个图片
			if(pictype!=T_GIF)gui_show_string(fn,5,5,lcddev.width-5,gui_phy.tbfsize,gui_phy.tbfsize,RED);	//显示图片名字						  
 			gui_memin_free(pname);			//释放内存 
			pname=NULL;
			endecode=0;		    
		}
		keyup=0;
		dtime=0;
		while(picsta)//按键扫描循环
		{     
			key=pic_tp_scan();   
			dtime++;
			if(dtime>400&&(picsta==1))key=3;//顺序播放状态下时间溢出,自动播放下一张图片    
			if(key)//有按键按下
			{ 
				dtime=0;
				if(keyup)
				{   
					keyup=0;
					if(key==1)//上一幅图片
					{
						picsta=0;
//						if(curindex)curindex--;
//						else curindex=flistbox->filecnt-1;
//						endecode=1;
//						LCD_Clear(0x0);//黑屏 	 
						break;
					}else if(key==2)//在暂停和非暂停之间切换
					{ 
						picsta=0;
//						if(picsta==1)
//						{
//							picsta=2;
//							LED1=0;	 	//表示暂停
//						}else 
//						{
//							picsta=1;
//							LED1=1;	   	//暂停结束
//						} 
					}else if(key==3)
					{
						picsta=0;
//						if(systemset.picmode==0)//顺序播放
//						{
//							if(curindex<(flistbox->filecnt-1))curindex++;
//							else curindex=0;
//						}else	//随机播放
//						{   
//							curindex=app_get_rand(flistbox->filecnt);//随机得到下一张图片的编号   	 
//						}
						endecode=1;
						LCD_Clear(0x0);//黑屏 	 
						break;	 
					}  
				}
			}else keyup=1;//标记按键松开
			delay_ms(10);  
//			if(system_task_return)picsta=0;//TPAD返回
			if(picsta==0)//回到文件浏览状态之前的处理
			{
				LED1=1;	   				//关闭LED1
				flistbox->dbclick=0;	//设置非文件浏览状态
				app_filebrower((u8*)APP_MFUNS_CAPTION_TBL[1][gui_phy.language],0X07);//选择目标文件,并得到目标数量
 				btn_draw(rbtn);			//画按钮
				filelistbox_rebuild_filelist(flistbox);//重建flistbox
// 				system_task_return=0;	//还不能退出图片浏览
				break;
			}
		}
	}	
	LED1=1;//关闭LED1
	filelistbox_delete(flistbox);	//删除filelist 	  
	btn_delete(rbtn);				//删除按钮	 	  
 	gui_memin_free(pname);			//释放内存		  
 	gui_memin_free(picinfo.lfname);
 	return rval; 
}

//图片浏览触摸屏检测touch panel
//返回按键结果：1，按了上面；2，按了中间；3按了下面
u8 pic_tp_scan(void)
{
	u8 res=0;
	tp_dev.scan(0);    
	if(tp_dev.sta&TP_PRES_DOWN)//有按键按下
	{ 
		if(tp_dev.y[0]<lcddev.height/3)res=1;			//按了上面
		else if(tp_dev.y[0]<lcddev.height*2/3)res=2;	//按了中间
		else if(tp_dev.y[0]<lcddev.height)res=3;		//按了下面 
	} 
	while(res)//等待按键松开
	{
		tp_dev.scan(0);  
		if((tp_dev.sta&TP_PRES_DOWN)==0)break;
		delay_ms(5);
	} 
	return res;
}






































