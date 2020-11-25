#include "listbox.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//GUI-列表框 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2012/10/4
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//********************************************************************************
//升级说明 
//V1.1 20140810
//1,新增对长条目的滚动显示功能
//2,修正原来存在的一些小bug
//3,修改滚动条检测机制,使用更方便
//4,去掉LBOX_ITEM_HEIGHT宏定义,改用变量方式控制(在gui_phy结构体里面设置)
//////////////////////////////////////////////////////////////////////////////////

//创建listbox
//top,left,width,height:坐标.及尺寸.
//width的范围:必须大于等于LBOX_ITEM_HEIGHT
//height的范围:必须是LBOX_ITEM_HEIGHT的整数倍.		 
//type:bit7,用于是否显示滚动条,其他位保留.
//font:字体类型 12/16表示字体为12*12或者16*16.
_listbox_obj * listbox_creat(u16 left,u16 top,u16 width,u16 height,u8 type,u8 font)
{
 	_listbox_obj * listbox_crt;
	_scrollbar_obj * scb_crt;	    
	if(height%gui_phy.listheight)return NULL;//尺寸不合格 	 				    
	if(height<gui_phy.listheight||width<gui_phy.listheight)return NULL;	//尺寸不合格
	listbox_crt=(_listbox_obj*)gui_memin_malloc(sizeof(_listbox_obj));//分配内存
	if(listbox_crt==NULL)return NULL;			//内存分配不够.
	scb_crt=scrollbar_creat(left+width-LBOX_SCB_WIDTH,top,LBOX_SCB_WIDTH,height,0x80);//创建scrollbar.
	if(scb_crt==NULL)//内存分配不够.
	{
		gui_memin_free(listbox_crt);
		return NULL;				
	}
 	scb_crt->itemsperpage=height/gui_phy.listheight;	//每页显示的条目数

 	listbox_crt->top=top;
	listbox_crt->left=left;
	listbox_crt->width=width;
	listbox_crt->height=height;
	listbox_crt->type=type;		//类型 
 	listbox_crt->sta=0;
	listbox_crt->id=0;
	listbox_crt->dbclick=0;		//双击标志清零
	listbox_crt->font=font;		//字体大小 
	listbox_crt->selindex=0;	//当前选中的索引
													
   	listbox_crt->lbkcolor=LBOX_DFT_LBKCOLOR;			//内部背景色
 	listbox_crt->lnselcolor=LBOX_DFT_LSELCOLOR;			//选中list后的字体颜色 
 	listbox_crt->lnselbkcolor=LBOX_DFT_LSELBKCOLOR;		//选中list后的背景色
 	listbox_crt->lncolor=LBOX_DFT_LNCOLOR;				//未选中的list字体颜色
   	listbox_crt->rimcolor=LBOX_DFT_RIMCOLOR;			//边框颜色
	
 	listbox_crt->fname=NULL;			    	    	//名字为空
	listbox_crt->namelen=0;			    	    		//长度清零
	listbox_crt->curnamepos=0;			    	    	//当前位置清零
	listbox_crt->oldtime=0;			    	    		//时间清零

	listbox_crt->scbv=scb_crt;			    			//滚动条
 	listbox_crt->list=NULL;								//无链表
 	return listbox_crt;
}
//查找编号为index的list
//listx:当前的list
//index:要寻找的list编号 0~totalitems-1;
//返回值:list结构体
//如果找到,则返回list结构体,如果没找到,返回NULL.
_listbox_list * list_search(_listbox_list *listx,u16 index)
{
	u16 icnt=0;
 	_listbox_list * listtemp=listx;	
	while(listtemp->prevlist!=NULL)listtemp=listtemp->prevlist;//追溯到最开始的list
	while(1)//寻找编号为index的list
	{
		if(index==icnt)break;	 
		if(listtemp->nextlist!=NULL)
		{
			listtemp=listtemp->nextlist;    
			icnt++;//计数增加
		}else return NULL;//未找到编号为index的list
	} 
	return listtemp;//返回找到的list指针
} 
//删除listbox
//btn_del:要删除的按键
void listbox_delete(_listbox_obj *listbox_del)
{
 	_listbox_list * listtemp;
 	_listbox_list * listdel;
	if(listbox_del->list!=NULL)
	{				    	  
		listtemp=list_search(listbox_del->list,0);//得到第一个list的结构体
 		do
		{
			listdel=listtemp;
			listtemp=listtemp->nextlist;
		    gui_memin_free(listdel);	//删除一个条目
		}while(listtemp!=NULL);	//一次删除所有的条目
	}
	gui_memin_free(listbox_del->scbv);//删除滚动条
 	gui_memin_free(listbox_del);
}
//检查	
//listbox:listbox指针
//in_key:输入按键指针
//返回值:未用到				   
u8 listbox_check(_listbox_obj * listbox,void * in_key)
{
	_in_obj *key=(_in_obj*)in_key;
	_listbox_list * listx;
	u16 endx;//条目显示区的尾端.	
	u16 tempindex;
	u16 i;	   
	u16 lastindex; 
	static u8 scrollbarflag=0;//scrollbar标记
	switch(key->intype)
	{
		case IN_TYPE_TOUCH:	//触摸屏按下了
 			if(listbox!=NULL)//listbox非空
			{
				endx=listbox->left+listbox->width-1;		  
				if(listbox->scbv->totalitems>listbox->scbv->itemsperpage)//有滚动条
				{ 
					endx-=LBOX_SCB_WIDTH;
					if(listbox->top<=key->y&&key->y<=(listbox->top+listbox->height)&&listbox->left<=key->x&&key->x<=(endx+LBOX_SCB_WIDTH))//在有效区域内(SCB+条目区域)
					{
						//在滚动条范围之内.
						if((listbox->top<=key->y&&key->y<=(listbox->top+listbox->height)&&endx<=key->x&&key->x<=(endx+LBOX_SCB_WIDTH))||scrollbarflag==1)
						{
							if(listbox->sta&(1<<6))break;		//是从list区划过来的,直接不执行
							scrollbarflag=1;					//标记scrollbar模式
							tempindex=listbox->scbv->topitem;
							key->x=listbox->scbv->left+1;		//强制让x坐标在scrollbar里面
							scrollbar_check(listbox->scbv,in_key);//滚动条检测
							if(tempindex!=listbox->scbv->topitem)listbox_draw_list(listbox);
							listbox->sta|=1<<7;
							break;
						}
					}else scrollbarflag=0;
					if((listbox->sta&0xc0)==(1<<7))//上次操作仅仅是在滚动条区域内
					{
						listbox->sta=0;
						scrollbar_check(listbox->scbv,in_key);//滚动条检测					
					} 
				}
				tempindex=listbox->sta&0x3f;//得到当前sta种的位置
				if(listbox->top<=key->y&&key->y<=(listbox->top+listbox->height)&&listbox->left<=key->x&&key->x<(endx))//在items区域内
				{
					//itemperpage最大不能超过64!(按20一个index设计,这样64个list可以支持到64*20=1280垂直像素的屏)
					for(i=0;i<listbox->scbv->itemsperpage;i++)//找到当前触屏按下的坐标在listbox种的index
					{
						if(key->y<=(listbox->top+(i+1)*gui_phy.listheight)&&key->y>=(listbox->top+i*gui_phy.listheight))break;
					}
					if((listbox->sta&(1<<6))==0)//编号还无效
					{
						listbox->sta|=1<<6;	//标记有效
						listbox->sta|=i;	//记录编号
					}else if((listbox->sta&(1<<7))==0)//还不是滑动
					{
						if(listbox->scbv->totalitems<=listbox->scbv->itemsperpage)break;//没滚动条,滑动无效
						if(gui_disabs(i,(listbox->sta&0x3f))>1)	listbox->sta|=1<<7;//滑动距离大于1个条目的间隙 标记滑动
					}
					if((listbox->sta&0xc0)==0xc0)//是滑动
					{
						lastindex=listbox->scbv->topitem; 
						if(tempindex>i)//减少了
						{
							listbox->sta&=0xc0;//清空上次的
							listbox->sta|=i;//记录最近的index号	    
							listbox->scbv->topitem+=tempindex-i;    
							if(listbox->scbv->topitem>=(listbox->scbv->totalitems-listbox->scbv->itemsperpage))listbox->scbv->topitem=listbox->scbv->totalitems-listbox->scbv->itemsperpage;
						}else if(i>tempindex)//增加了
						{
							listbox->sta&=0xc0;//清空上次的
							listbox->sta|=i;//记录最近的index号
							i-=tempindex;				  
							if(listbox->scbv->topitem>=i)listbox->scbv->topitem-=i;
							else listbox->scbv->topitem=0;						
 						}else break;
						if(listbox->scbv->topitem!=lastindex)listbox_draw_listbox(listbox);//重画listbox
					}
				}else //按键松开了,或者滑出了
				{
					if(listbox->sta&(1<<7))//滑动
					{
						listbox->sta=0;										 
						listbox->oldtime=GUI_TIMER_10MS;//记录当前时间
						listbox->curnamepos=0;//清零
					}else if(listbox->sta&(1<<6))//单点按下
					{
						listbox->dbclick=0;			//双击标记清零
						if((listbox->scbv->topitem+tempindex)==listbox->selindex)
						{
							listbox->dbclick|=1<<7;	//标记双击了
							listbox_2click_hook(listbox);//执行双击钩子函数
						}else if((listbox->scbv->topitem+tempindex)<listbox->scbv->totalitems) //重新选择新的选项
						{		 
							if((listbox->selindex<(listbox->scbv->topitem+listbox->scbv->itemsperpage))&&(listbox->selindex>=listbox->scbv->topitem))//老的selindex在屏幕范围内
							{
								i=listbox->selindex-listbox->scbv->topitem;//老编号
								listx=list_search(listbox->list,listbox->selindex);//得到listindex的名字
								gui_fill_rectangle(listbox->left,listbox->top+i*gui_phy.listheight,endx-listbox->left+1,gui_phy.listheight,listbox->lbkcolor);//恢复底色
								gui_show_ptstr(listbox->left,listbox->top+i*gui_phy.listheight+(gui_phy.listheight-listbox->font)/2,endx,gui_phy.lcdheight,0,listbox->lncolor,listbox->font,listx->name,1);//显示名字
							}
							listbox->selindex=listbox->scbv->topitem+tempindex; 
							listx=list_search(listbox->list,listbox->selindex);//得到listindex的名字
							gui_fill_rectangle(listbox->left,listbox->top+tempindex*gui_phy.listheight,endx-listbox->left+1,gui_phy.listheight,listbox->lnselbkcolor);//填充选中后的底色
							gui_show_ptstr(listbox->left,listbox->top+tempindex*gui_phy.listheight+(gui_phy.listheight-listbox->font)/2,endx,gui_phy.lcdheight,0,listbox->lnselcolor,listbox->font,listx->name,1);//显示名字
							listbox->fname=listx->name;//获得当前选中的条目的名字
							listbox->namelen=strlen((const char*)listbox->fname)*listbox->font/2;//名字的总长度
						}
						listbox->sta=0;	  
 						listbox->oldtime=GUI_TIMER_10MS;//记录当前时间
						listbox->curnamepos=0;
					}else
					{
						if(gui_disabs(listbox->oldtime,GUI_TIMER_10MS)>=5)//超过50ms了.
						{	  
							if(listbox->selindex>=listbox->scbv->topitem&&listbox->selindex<(listbox->scbv->topitem+listbox->scbv->itemsperpage))//选中条目在当前的显示范围内
	 						{	 
								i=endx-listbox->left;//得到显示文字区域的长度
								if(i<listbox->namelen)//小于文字长度,就是不能够一次显示的
								{
 		 							tempindex=listbox->selindex-listbox->scbv->topitem;
									gui_fill_rectangle(listbox->left,listbox->top+tempindex*gui_phy.listheight,endx-listbox->left+1,gui_phy.listheight,listbox->lnselbkcolor);//填充选中后的底色
		 							gui_show_ptstr(listbox->left,listbox->top+tempindex*gui_phy.listheight+(gui_phy.listheight-listbox->font)/2,endx,gui_phy.lcdheight,listbox->curnamepos,listbox->lnselcolor,listbox->font,listbox->fname,1);//显示名字
									listbox->curnamepos++;
									if(listbox->curnamepos+i>listbox->namelen+i/4)listbox->curnamepos=0;//循环显示	
								}
							} 
							listbox->oldtime=GUI_TIMER_10MS;//记录当前时间
						}
					}
				}
			}
 			break;
		case IN_TYPE_KEY:	//按键数据
			break;
		case IN_TYPE_JOYPAD://手柄数据
			break;
		case IN_TYPE_MOUSE:	//鼠标数据
			break;
		default:
			break;
	}
	return 0;
}
//增加一条listbox的条目
//0,增加成功;
//1,增加失败
u8 listbox_addlist(_listbox_obj * listbox,u8 *name)
{
	_listbox_list * listx;
	_listbox_list * listtemp;

	listx=(_listbox_list*)gui_memin_malloc(sizeof(_listbox_list));//分配内存
	if(listx==NULL)return 1;//内存分配不够.
	listx->name=name;//得到名字
	listx->nextlist=NULL;
 	if(listbox->list==NULL)//还未创建链表
	{
		listx->prevlist=NULL;
		listbox->list=listx;
	}else //已经创建了
	{
		listtemp=listbox->list;
		while(listtemp->nextlist!=NULL)listtemp=listtemp->nextlist;	 
		listx->prevlist=listtemp;
		listtemp->nextlist=listx;
	}
	listbox->scbv->totalitems++;//总条目数增加1条
	if(listbox->scbv->totalitems>listbox->scbv->itemsperpage)listbox->type|=0x80;//需要显示滚条
 	else listbox->type&=~0x80;//不需要显示滚条
	return 0;
}
//画list
//listbox:listbox指针
void listbox_draw_list(_listbox_obj *listbox)
{
	u16 i;
	u16 endx=0;
	u16 tempcolor;	  
 	_listbox_list * listx;		   
 	endx=listbox->left+listbox->width-1; //条目显示结束x坐标
	if(listbox->scbv->totalitems>listbox->scbv->itemsperpage)endx-=LBOX_SCB_WIDTH;//需要滚动条了.
	gui_fill_rectangle(listbox->left,listbox->top,endx-listbox->left+1,listbox->height,listbox->lbkcolor);//清空为底色.	
	listx=list_search(listbox->list,listbox->scbv->topitem);//得到顶部list信息(curitem在listbox中用于记录顶部index)
	for(i=0;i<listbox->scbv->itemsperpage;i++)//显示条目
	{	
		if((listbox->scbv->topitem+i)==listbox->selindex)//此项为选中项目
		{
			gui_fill_rectangle(listbox->left,listbox->top+i*gui_phy.listheight,endx-listbox->left+1,gui_phy.listheight,listbox->lnselbkcolor);//填充底色
			listbox->fname=listx->name;//获得当前选中的条目的名字
			listbox->namelen=strlen((const char*)listbox->fname)*listbox->font/2;//名字的总长度
			listbox->curnamepos=0;
			tempcolor=listbox->lnselcolor;
		}else tempcolor=listbox->lncolor;																						    
		gui_show_ptstr(listbox->left,listbox->top+i*gui_phy.listheight+(gui_phy.listheight-listbox->font)/2,endx,gui_phy.lcdheight,0,tempcolor,listbox->font,listx->name,1);
		if(listx->nextlist==0)break;
		listx=listx->nextlist;
 	}
}
//画listbox
//listbox:listbox指针
void listbox_draw_listbox(_listbox_obj *listbox)
{															  
	if(listbox==NULL)return;//无效,直接退出
	if(listbox->scbv->totalitems>listbox->scbv->itemsperpage)scrollbar_draw_scrollbar(listbox->scbv);//必要时,画滚动条
    listbox_draw_list(listbox);//画list
}
//滚动条条目双击钩子函数
//listbox:listbox指针
void listbox_2click_hook(_listbox_obj *listbox)
{
	//printf("listbox%d index %d 2click!\r\n",listbox->id,listbox->selindex);
}
 
















































