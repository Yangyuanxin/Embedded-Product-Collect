#include "filelistbox.h"
#include "ff.h"
#include "exfuns.h"
#include "picdecode.h"			 
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

		    
//创建filelistbox
//top,left,width,height:坐标.及尺寸. 
//height:高度必须为:gui_phy.listheight*N+gui_phy.listheight+gui_phy.listheight,这里默认为40+20的N倍 
//width:宽度必须大于等于gui_phy.listheight(20)
//type:bit7,用于是否显示滚动条(自动控制),其他位保留.
//font:字体类型 12/16/24 表示字体为12*12或者16*16或24*24字体.
_filelistbox_obj * filelistbox_creat(u16 left,u16 top,u16 width,u16 height,u8 type,u8 font)
{
 	_filelistbox_obj * filelistbox_crt;
	_scrollbar_obj * scb_crt;
	
	if(height<=gui_phy.listheight+gui_phy.listheight+gui_phy.listheight)return NULL;//高度小了	    
	if((height-gui_phy.listheight-gui_phy.listheight)%gui_phy.listheight)return NULL;//尺寸不合格 	 				    
	if(width<gui_phy.listheight)return NULL;			//尺寸不合格
	filelistbox_crt=(_filelistbox_obj*)gui_memin_malloc(sizeof(_filelistbox_obj));	//分配内存
	if(filelistbox_crt==NULL)return NULL;			//内存分配不够.
	gui_memset((u8*)filelistbox_crt,0,sizeof(_filelistbox_obj));//将filelistbox_crt的值全部设置为0    
	scb_crt=scrollbar_creat(left+width-FLBOX_SCB_WIDTH,top+gui_phy.listheight,FLBOX_SCB_WIDTH,height-gui_phy.listheight-gui_phy.listheight,0x80);//创建scrollbar.
	if(scb_crt==NULL)
	{
		gui_memin_free(filelistbox_crt);//释放之前申请的内存
		return NULL;					//内存分配不够.
 	}
 	scb_crt->itemsperpage=(height-gui_phy.listheight-gui_phy.listheight)/gui_phy.listheight;	//每页显示的条目数

 	filelistbox_crt->top=top;
	filelistbox_crt->left=left;
	filelistbox_crt->width=width;
	filelistbox_crt->height=height;
	filelistbox_crt->type=type;		//类型 
 	filelistbox_crt->sta=0;
	filelistbox_crt->id=0;
	filelistbox_crt->dbclick=0;		//没有双击
	filelistbox_crt->font=font;		//字体大小 
	filelistbox_crt->selindex=0;	//当前选中的索引
	if(font==12)filelistbox_crt->icosize=16;
	else if(font==16)filelistbox_crt->icosize=20;
	else if(font==24)filelistbox_crt->icosize=28;
	else filelistbox_crt->icosize=16;//不支持的字体,设置icosize为16
													
   	filelistbox_crt->lbkcolor=FLBOX_DFT_LBKCOLOR;			//内部背景色
 	filelistbox_crt->lnselcolor=FLBOX_DFT_LSELCOLOR;		//选中list后的字体颜色 
 	filelistbox_crt->lnselbkcolor=FLBOX_DFT_LSELBKCOLOR;	//选中list后的背景色
 	filelistbox_crt->lncolor=FLBOX_DFT_LNCOLOR;				//未选中的list字体颜色
   	filelistbox_crt->rimcolor=FLBOX_DFT_RIMCOLOR;			//边框颜色
	filelistbox_crt->pthinfbkcolor=FLBOX_DFT_PTHINFBKCOLOR;	//路径/信息栏 背景颜色
	
	
	filelistbox_crt->path=NULL;			    				//路径为空	 

	filelistbox_crt->fliter=0X0000;		    				//所有文件过滤(不包含文件夹)
	filelistbox_crt->fliterex=0XFFFF;	    				//不对小类进行过滤(所有小类均符合要求)

	filelistbox_crt->fname=NULL;			    	    	//名字为空
	filelistbox_crt->namelen=0;			    	    		//长度清零
	filelistbox_crt->curnamepos=0;			    	    	//当前位置清零
	filelistbox_crt->oldtime=0;			    	    		//时间清零
	 

	filelistbox_crt->findextbl=NULL;			    		//文件索引表为空

	filelistbox_crt->scbv=scb_crt;			    			//滚动条
 	filelistbox_crt->list=NULL;								//无链表
 	return filelistbox_crt;
}
//查找编号为index的flist
//flistx:当前的flist指针
//index:要寻找的flist编号 0~totalitems-1;
//返回值:list结构体
//如果找到,则返回list结构体,如果没找到,返回NULL.
_filelistbox_list * filelist_search(_filelistbox_list *filelistx,u16 index)
{
	u16 icnt=0;
 	_filelistbox_list * filelisttemp=filelistx;	
	if(filelistx==NULL)return NULL;
	while(filelisttemp->prevlist!=NULL)filelisttemp=filelisttemp->prevlist;//追溯到最开始的list
	while(1)//寻找编号为index的list
	{
		if(index==icnt)break;	 
		if(filelisttemp->nextlist!=NULL)
		{
			filelisttemp=filelisttemp->nextlist;    
			icnt++;//计数增加
		}else return NULL;//未找到编号为index的list
	} 
	return filelisttemp;//返回找到的list指针
}
//删除所有链表 
//filelistdel,要删除的链表中的某个成员
void filelist_delete(_filelistbox_obj * filelistbox)
{
 	_filelistbox_list * filelisttemp;
  	_filelistbox_list * filelistdel;
	if(filelistbox==NULL)return;//非法的地址,直接退出
 	if(filelistbox->list!=NULL)
	{
		filelisttemp=filelist_search(filelistbox->list,0);//得到第一个filelist的结构体
		do
		{
			filelistdel=filelisttemp;
			filelisttemp=filelisttemp->nextlist;
		    gui_memex_free(filelistdel->name);	//删除条目的名字
		    gui_memex_free(filelistdel);		//删除一个条目
		}while(filelisttemp!=NULL);		//一次删除所有的条目
	}
	filelistbox->list=NULL;//清空
 	filelistbox->scbv->totalitems=0;//总条目清零
	filelistbox->scbv->topitem=0;	//顶部条目清零
}
//删除filelistbox
//filelistbox_del:要删除的filelistbox
void filelistbox_delete(_filelistbox_obj *filelistbox_del)
{
 	filelist_delete(filelistbox_del);	 	//删除条目
	gui_memin_free(filelistbox_del->findextbl);//删除目标文件索引表
	gui_memin_free(filelistbox_del->path);	//删除路径
	gui_memin_free(filelistbox_del->scbv);	//删除滚动条
	gui_memin_free(filelistbox_del);	  	//删除filelistbox
}
//重建文件列表框
//当有效文件使用完退出后使用,重新扫描文件，并重画
//filelistbox:listbox指针
u8 filelistbox_rebuild_filelist(_filelistbox_obj * filelistbox)
{
	u8 res=0;
	if(filelistbox!=NULL)
	{
		res=filelistbox_scan_filelist(filelistbox);	//重新扫描列表	  
		if(filelistbox->scbv->totalitems>filelistbox->scbv->itemsperpage)filelistbox->scbv->topitem=filelistbox->selindex;
		filelistbox_draw_listbox(filelistbox);		//重画 
 	}else res=2;
	return res;
}	 
//检查	
//filelistbox:listbox指针
//in_key:输入按键指针
//返回值:未用到				   
u8 filelistbox_check(_filelistbox_obj * filelistbox,void * in_key)
{
	_in_obj *key=(_in_obj*)in_key;
	_filelistbox_list * filelistx;
	u16 endx;//条目显示区的尾端.	
	u16 tempindex;
	u16 i;	   
	u16 lastindex; 
	static u8 scrollbarflag=0;//scrollbar标记
	switch(key->intype)
	{
		case IN_TYPE_TOUCH:	//触摸屏按下了
 			if(filelistbox!=NULL)//listbox非空
			{
				if(filelistbox->dbclick==0x81||filelistbox->list==NULL)//目标文件发生了双击，或者列表为空
				{		 
					break;//不在执行原本的扫描			
				} 
				endx=filelistbox->left+filelistbox->width-1;		  
 				if(filelistbox->scbv->totalitems>filelistbox->scbv->itemsperpage)//有滚动条
				{ 
					endx-=FLBOX_SCB_WIDTH;
					if((filelistbox->top+gui_phy.listheight)<=key->y&&key->y<=(filelistbox->top+filelistbox->height-gui_phy.listheight)&&filelistbox->left<=key->x&&key->x<(endx+FLBOX_SCB_WIDTH))//在有效区域内(SCB+条目区域)
					{
						//在滚动条范围之内.
						if(((filelistbox->top+gui_phy.listheight)<=key->y&&key->y<=(filelistbox->top+filelistbox->height-gui_phy.listheight)&&endx<=key->x&&key->x<=(endx+FLBOX_SCB_WIDTH))||scrollbarflag==1)
						{
							if(filelistbox->sta&(1<<6))break;	//是从list区划过来的,直接不执行
							scrollbarflag=1;					//标记scrollbar模式
							tempindex=filelistbox->scbv->topitem;
							key->x=filelistbox->scbv->left+1;	//强制让x坐标在scrollbar里面
							scrollbar_check(filelistbox->scbv,in_key);//滚动条检测
							if(tempindex!=filelistbox->scbv->topitem)filelistbox_draw_list(filelistbox);
							filelistbox->sta|=1<<7;
							break;
						}						
					}else scrollbarflag=0;
					if((filelistbox->sta&0xc0)==(1<<7))//上次操作仅仅是在滚动条区域内
					{
						filelistbox->sta=0; 				
					}
 				}
				tempindex=filelistbox->sta&0x3f;//得到当前sta中的位置
				if((filelistbox->top+gui_phy.listheight)<=key->y&&key->y<=(filelistbox->top+filelistbox->height-gui_phy.listheight)&&filelistbox->left<=key->x&&key->x<(endx))//在items区域内
				{ 
					//itemperpage最大不能超过64!(按20一个index设计,这样64个list可以支持到64*20=1280垂直像素的屏)
					for(i=0;i<filelistbox->scbv->itemsperpage;i++)//找到当前触屏按下的坐标在listbox种的index
					{
						if(key->y<=(filelistbox->top+gui_phy.listheight+(i+1)*gui_phy.listheight)&&key->y>=(filelistbox->top+gui_phy.listheight+i*gui_phy.listheight))break;
					}
					if((filelistbox->sta&(1<<6))==0)//编号还无效
					{
						filelistbox->sta|=1<<6;	//标记有效
						filelistbox->sta|=i;	//记录编号
					}else if((filelistbox->sta&(1<<7))==0)//还不是滑动
					{
						if(filelistbox->scbv->totalitems<=filelistbox->scbv->itemsperpage)break;//没滚动条,滑动无效
						if(gui_disabs(i,(filelistbox->sta&0x3f))>1)	filelistbox->sta|=1<<7;//滑动距离大于1个条目的间隙 标记滑动
					}
					if((filelistbox->sta&0xc0)==0xc0)//是滑动
					{
						lastindex=filelistbox->scbv->topitem; 
						if(tempindex>i)//减少了
						{
							filelistbox->sta&=0xc0;//清空上次的
							filelistbox->sta|=i;//记录最近的index号	    
							filelistbox->scbv->topitem+=tempindex-i;    
							if(filelistbox->scbv->topitem>=(filelistbox->scbv->totalitems-filelistbox->scbv->itemsperpage))filelistbox->scbv->topitem=filelistbox->scbv->totalitems-filelistbox->scbv->itemsperpage;
						}else if(i>tempindex)//增加了
						{
							filelistbox->sta&=0xc0;//清空上次的
							filelistbox->sta|=i;//记录最近的index号
							i-=tempindex;				  
							if(filelistbox->scbv->topitem>=i)filelistbox->scbv->topitem-=i;
							else filelistbox->scbv->topitem=0;						
 						}else break;
						if(filelistbox->scbv->topitem!=lastindex)filelistbox_draw_listbox(filelistbox);//重画listbox
					}
					filelistbox->oldtime=GUI_TIMER_10MS;//记录当前时间
					filelistbox->curnamepos=0;//清零
				}else //按键松开了,或者滑出了
				{
					if(filelistbox->sta&(1<<7))//滑动
					{
						filelistbox->sta=0;										 
					}else if(filelistbox->sta&(1<<6))//单点按下
					{
						filelistbox->dbclick=0;	//双击标记清零
						if((filelistbox->scbv->topitem+tempindex)==filelistbox->selindex)
						{
							filelistbox->dbclick|=1<<7;							//标记双击了. 
							filelistbox_check_filelist(filelistbox);//文件夹
							filelistbox_2click_hook(filelistbox);//执行双击钩子函数
						}else if((filelistbox->scbv->topitem+tempindex)<filelistbox->scbv->totalitems) //重新选择新的选项
						{	  
							if((filelistbox->selindex<(filelistbox->scbv->topitem+filelistbox->scbv->itemsperpage))&&(filelistbox->selindex>=filelistbox->scbv->topitem))//老的selindex在屏幕范围内
							{
								i=filelistbox->selindex-filelistbox->scbv->topitem;//老编号
								filelistx=filelist_search(filelistbox->list,filelistbox->selindex);//得到listindex的名字
								gui_fill_rectangle(filelistbox->left,filelistbox->top+gui_phy.listheight+i*gui_phy.listheight,endx-filelistbox->left+1,gui_phy.listheight,filelistbox->lbkcolor);//恢复底色
 								gui_draw_icos(filelistbox->left,filelistbox->top+gui_phy.listheight+i*gui_phy.listheight+(gui_phy.listheight-filelistbox->icosize)/2,filelistbox->icosize,filelistx->type);//画ICO图标																				    
								gui_show_ptstr(filelistbox->left+filelistbox->icosize+2,filelistbox->top+gui_phy.listheight+i*gui_phy.listheight+(gui_phy.listheight-filelistbox->font)/2,endx,gui_phy.lcdheight,0,filelistbox->lncolor,filelistbox->font,filelistx->name,1);//显示名字
							}
							filelistbox->selindex=filelistbox->scbv->topitem+tempindex; 
							filelistx=filelist_search(filelistbox->list,filelistbox->selindex);//得到listindex的名字
							gui_fill_rectangle(filelistbox->left+filelistbox->icosize,filelistbox->top+gui_phy.listheight+tempindex*gui_phy.listheight,endx-filelistbox->left+1-filelistbox->icosize,gui_phy.listheight,filelistbox->lnselbkcolor);//填充选中后的底色
							gui_draw_icos(filelistbox->left,filelistbox->top+gui_phy.listheight+tempindex*gui_phy.listheight+(gui_phy.listheight-filelistbox->icosize)/2,filelistbox->icosize,filelistx->type);//画ICO图标																				    
							gui_show_ptstr(filelistbox->left+filelistbox->icosize+2,filelistbox->top+gui_phy.listheight+tempindex*gui_phy.listheight+(gui_phy.listheight-filelistbox->font)/2,endx,gui_phy.lcdheight,0,filelistbox->lnselcolor,filelistbox->font,filelistx->name,1);//显示名字
							
							filelistbox->fname=filelistx->name;//获得当前选中的条目的名字
							filelistbox->namelen=strlen((const char*)filelistbox->fname)*filelistbox->font/2;//名字的总长度
  						}
						filelistbox->sta=0;	  
						filelistbox_show_info(filelistbox);
						//双击目标文件了.
						////if(filelistbox->dbclick==0X81&&filelistbox->list!=NULL)filelist_delete(filelistbox);//删除之前的条目,释放内存
 						filelistbox->oldtime=GUI_TIMER_10MS;//记录当前时间
						filelistbox->curnamepos=0;
					}else //空闲状态,既没有滑动,也没单点按,执行走字显示.
					{
						if(gui_disabs(filelistbox->oldtime,GUI_TIMER_10MS)>=10)//超过100ms了.
						{	  
							if(filelistbox->selindex>=filelistbox->scbv->topitem&&filelistbox->selindex<(filelistbox->scbv->topitem+filelistbox->scbv->itemsperpage))//选中条目在当前的显示范围内
	 						{	 
								i=endx-filelistbox->left-filelistbox->icosize-2;//得到显示文字区域的长度
								if(i<filelistbox->namelen)//小于文字长度,就是不能够一次显示的
								{
 		 							tempindex=filelistbox->selindex-filelistbox->scbv->topitem;
									gui_fill_rectangle(filelistbox->left+filelistbox->icosize,filelistbox->top+gui_phy.listheight+tempindex*gui_phy.listheight,endx-filelistbox->left+1-filelistbox->icosize,gui_phy.listheight,filelistbox->lnselbkcolor);//填充选中后的底色
		 							gui_show_ptstr(filelistbox->left+filelistbox->icosize+2,filelistbox->top+gui_phy.listheight+tempindex*gui_phy.listheight+(gui_phy.listheight-filelistbox->font)/2,endx,gui_phy.lcdheight,filelistbox->curnamepos,filelistbox->lnselcolor,filelistbox->font,filelistbox->fname,1);//显示名字
									filelistbox->curnamepos++;
									if(filelistbox->curnamepos+i>filelistbox->namelen+i/4)filelistbox->curnamepos=0;//循环显示	
								}
							}
							if(filelistbox->path==NULL)//磁盘浏览界面,扫描磁盘在位状态.
							{
								if(filelistbox->memdevflag!=(gui_phy.memdevflag&0XFF))//磁盘状态有变化
								{
									filelist_delete(filelistbox);			//删除之前的条目
									filelistbox_add_disk(filelistbox);		//重新添加磁盘路径
									filelistbox_draw_listbox(filelistbox);	//重新显示
								}
							}
							filelistbox->oldtime=GUI_TIMER_10MS;//记录当前时间
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
//增加一条filelistbox的条目
//filelistbox:
//0,增加成功;
//1,增加失败
u8 filelistbox_addlist(_filelistbox_obj * filelistbox,u8 *name,u8 type)
{
	_filelistbox_list * filelistx;
	_filelistbox_list * filelisttemp;

	filelistx=(_filelistbox_list*)gui_memex_malloc(sizeof(_filelistbox_list));//分配内存(外部SRAM)
	if(filelistx==NULL)return 1;//内存分配不够.
	gui_memset(filelistx,0,sizeof(_filelistbox_list));//全部设置为0
	filelistx->name=gui_memex_malloc(strlen((const char*)name)+1);	//分配内存来存放name,外加一个结束符.
 	if(filelistx->name==NULL)
	{
		gui_memex_free(filelistx);//释放已经申请到的内存
		return 2;//内存分配不够.
	}
 	filelistx->name=(u8*)strcpy((char *)filelistx->name,(const char*)name);	//复制name的内容到
	filelistx->type=type;								//类型
 	filelistx->nextlist=NULL;
 	if(filelistbox->list==NULL)//还未创建链表
	{
		filelistx->prevlist=NULL;
		filelistbox->list=filelistx;
	}else //已经创建了
	{
		filelisttemp=filelistbox->list;
		while(filelisttemp->nextlist!=NULL)filelisttemp=filelisttemp->nextlist;//偏移到最后一个list	 
		filelistx->prevlist=filelisttemp;
		filelisttemp->nextlist=filelistx;
	}
	filelistbox->scbv->totalitems++;//总条目数增加1条
	if(filelistbox->scbv->totalitems>filelistbox->scbv->itemsperpage)filelistbox->type|=0x80;//需要显示滚条
 	else filelistbox->type&=~0x80;//不需要显示滚条
	return 0;
}
//画filelist
//filelistbox:filelistbox指针
void filelistbox_draw_list(_filelistbox_obj *filelistbox)
{
	u16 i;
	u16 endx=0;
	u16 tempcolor;	
  	_filelistbox_list * filelistx;		   
 	endx=filelistbox->left+filelistbox->width-1; //条目显示结束x坐标
	if(filelistbox->scbv->totalitems>filelistbox->scbv->itemsperpage)
	{
		endx-=FLBOX_SCB_WIDTH;//需要滚动条了.
	}else filelistbox->scbv->topitem=0;
 	gui_fill_rectangle(filelistbox->left,filelistbox->top+gui_phy.listheight,endx-filelistbox->left+1,filelistbox->height-gui_phy.listheight-gui_phy.listheight,filelistbox->lbkcolor);//清空为底色.	
 	filelistx=filelist_search(filelistbox->list,filelistbox->scbv->topitem);//得到顶部list信息(curitem在listbox中用于记录顶部index)
	if(filelistx==NULL)return ;//没有任何list
	for(i=0;i<filelistbox->scbv->itemsperpage;i++)//显示条目
	{	
		if((filelistbox->scbv->topitem+i)==filelistbox->selindex)//此项为选中项目
		{
			gui_fill_rectangle(filelistbox->left+filelistbox->icosize,filelistbox->top+gui_phy.listheight+i*gui_phy.listheight,endx-filelistbox->left+1-filelistbox->icosize,gui_phy.listheight,filelistbox->lnselbkcolor);//填充底色
			tempcolor=filelistbox->lnselcolor;
			filelistbox->fname=filelistx->name;//获得当前选中的条目的名字
			filelistbox->namelen=strlen((const char*)filelistbox->fname)*filelistbox->font/2;//名字的总长度
			filelistbox->curnamepos=0;//当前的位置为0. 
		}else tempcolor=filelistbox->lncolor;		
		gui_draw_icos(filelistbox->left,filelistbox->top+gui_phy.listheight+i*gui_phy.listheight+(gui_phy.listheight-filelistbox->icosize)/2,filelistbox->icosize,filelistx->type);//画ICO图标																				    
		gui_show_ptstr(filelistbox->left+filelistbox->icosize+2,filelistbox->top+gui_phy.listheight+i*gui_phy.listheight+(gui_phy.listheight-filelistbox->font)/2,endx,gui_phy.lcdheight,0,tempcolor,filelistbox->font,filelistx->name,1);//显示名字
		if(filelistx->nextlist==0)break;
		filelistx=filelistx->nextlist;
 	}
}
//显示filelistbox的路径 
//filelistbox:filelistbox指针
void filelistbox_show_path(_filelistbox_obj *filelistbox)
{
	u16 plen=0;
	u8 *path=filelistbox->path;
	u8 offset=0;    
	const u8 chgchar[2]={0X5C,0X00};//转义符 等效"\"
	if(path)//路径有效
	{
		plen=strlen((const char*)path);
		if(plen>4)offset=1;	//文件夹 
	} 
	gui_draw_icosalpha(filelistbox->left+5,filelistbox->top+(gui_phy.listheight-filelistbox->icosize)/2,filelistbox->icosize,offset);//偏移5个单位,显示图标
	if(plen)//需要显示
	{
		offset=0;
		if(plen*(gui_phy.listfsize/2)>(filelistbox->width-filelistbox->icosize-7))
		{
			offset=(gui_phy.listfsize/2)*3;
			while(plen*(gui_phy.listfsize/2)>(filelistbox->width-offset-filelistbox->icosize-7))
			{
 				path=(u8*)strstr((const char*)++path,(const char*)chgchar);	//去掉一截
				plen=strlen((const char*)path);					//得到新的长度
			}
 			gui_show_ptstr(filelistbox->left+filelistbox->icosize+7,filelistbox->top+(gui_phy.listheight-gui_phy.listfsize)/2,filelistbox->width+filelistbox->left,gui_phy.lcdheight,0,filelistbox->lncolor,gui_phy.listfsize,"...",1);//显示3点
		}   	
		gui_show_ptstr(filelistbox->left+offset+filelistbox->icosize+7,filelistbox->top+(gui_phy.listheight-gui_phy.listfsize)/2,filelistbox->width+filelistbox->left,gui_phy.lcdheight,0,filelistbox->lncolor,gui_phy.listfsize,path,1);//显示路径
	}
}	 

//磁盘容量字符串
u8*const FLBOX_DISK_SIZE_CAPTION_TBL[GUI_LANGUAGE_NUM]=
{
	"磁盘容量:","磁盤容量:","Capacity:",	
};
//磁盘可用空间字符串
u8*const FLBOX_DISK_FREE_CAPTION_TBL[GUI_LANGUAGE_NUM]=
{
	"可用空间:","可用空間:","Free:",	
};
//文件夹字符串
u8*const FLBOX_FOLDER_CAPTION_TBL[GUI_LANGUAGE_NUM]=
{
	"文件夹:","文件夾:","Folder:",	
};
//磁盘可用空间字符串
u8*const FLBOX_FILES_CAPTION_TBL[GUI_LANGUAGE_NUM]=
{
	"  文件:","  文件:","  Files:",	
}; 
//磁盘名字表
u8*const FLIST_DISK_NAME_TBL[3][GUI_LANGUAGE_NUM]=
{
	{"SD卡","SD卡","SD Card",},
 	{"FLASH磁盘","FLASH磁盤","FLASH Disk",},
 	{"U盘","U盤","U Disk",},
};
//filelist磁盘顺序表,即选择编号与磁盘编号对应关系
vu8 FLIST_DISK_ORDER_TBL[3];
//filelist磁盘路径表
u8*const FLIST_DISK_PATH_TBL[3]={"0:","1:","2:"};

//显示信息  
//filelistbox:filelistbox指针
//mode:0,非打开文件夹/磁盘的操作.1,打开文件夹或磁盘,需要更新全部信息
void filelistbox_show_info(_filelistbox_obj *filelistbox)
{
	_filelistbox_list *filelistx;
	FATFS *fs1;
	u8 res;
    u32 freclust=0,fresec=0,totsec=0;
 	u8 * temp;
	u8 * temp1;
	u8 * mark;
	temp=gui_memin_malloc(60);	//用于存放字符串
	if(temp==NULL)return;
 	temp1=gui_memin_malloc(30);	//用于存放数字串
	if(temp1==NULL)
	{
		gui_memin_free(temp);	
		return;  
	}
 	gui_memset((char *)temp,0,60);
  	gui_memset((char *)temp1,0,30);
	gui_fill_rectangle(filelistbox->left,filelistbox->top+filelistbox->height-gui_phy.listheight,filelistbox->width,gui_phy.listheight,filelistbox->pthinfbkcolor);//显示文件信息背景色
	gui_draw_hline(filelistbox->left,filelistbox->top+filelistbox->height-gui_phy.listheight,filelistbox->width,0X000);//显示黑色的横线		
	filelistx=filelist_search(filelistbox->list,filelistbox->selindex);//得到选中的list的详细信息
	if(filelistx->type==FICO_DISK)//当前选中的是磁盘
	{
		mark=(u8*)FLIST_DISK_PATH_TBL[FLIST_DISK_ORDER_TBL[filelistbox->selindex]]; 
		res=f_getfree((const TCHAR*)mark,(DWORD*)&freclust,&fs1);//获得剩余容量
	    if(res==0)
		{											   
		    totsec=(fs1->n_fatent-2)*fs1->csize;//得到总扇区数
		    fresec=freclust*fs1->csize;			//得到空闲扇区数	   
#if _MAX_SS!=512								//对于扇区区不是512字节大小的磁盘
			totsec*=fs1->ssize/512;
			fresec*=fs1->ssize/512;
#endif	  
		}
		if(totsec<20480)//总容量小于10M
		{
			totsec>>=1,fresec>>=1;
			mark="KB ";
		}else
		{
			totsec>>=11,fresec>>=11;
			mark="MB ";
 		}
		strcpy((char*)temp,(const char*)FLBOX_DISK_SIZE_CAPTION_TBL[gui_phy.language]);//磁盘容量	
		gui_num2str(temp1,totsec);				//总容量转为字符串
		strcat((char*)temp,(const char*)temp1);	
		strcat((char*)temp,(const char*)mark);	//添加单位
  		strcat((char*)temp,(const char*)FLBOX_DISK_FREE_CAPTION_TBL[gui_phy.language]);//可用空间		
 		gui_num2str(temp1,fresec);				//可用空间转为字符串
		strcat((char*)temp,(const char*)temp1);	
		strcat((char*)temp,(const char*)mark);	//添加单位
	}else	//选中的是文件/文件夹
	{
		if(filelistbox->selindex<filelistbox->foldercnt)fresec=filelistbox->selindex+1;	//在文件夹总数目以内.
		else freclust=filelistbox->selindex-filelistbox->foldercnt+1;		//在文件总数目以内
		strcpy((char*)temp,(const char*)FLBOX_FOLDER_CAPTION_TBL[gui_phy.language]);//文件夹	
		gui_num2str(temp1,fresec);					//当前文件夹编号转为字符串
		strcat((char*)temp,(const char*)temp1);	
 		strcat((char*)temp,(const char*)"/");		
		gui_num2str(temp1,filelistbox->foldercnt);	//文件夹总数转为字符串
 		strcat((char*)temp,(const char*)temp1);	
 		strcat((char*)temp,(const char*)FLBOX_FILES_CAPTION_TBL[gui_phy.language]);//文件		
		gui_num2str(temp1,freclust);				//当前文件编号转为字符串
		strcat((char*)temp,(const char*)temp1);	
 		strcat((char*)temp,(const char*)"/");		
		gui_num2str(temp1,filelistbox->filecnt);	//文件总数转为字符串
 		strcat((char*)temp,(const char*)temp1);	
	}
	gui_show_ptstr(filelistbox->left,filelistbox->top+filelistbox->height-gui_phy.listheight+(gui_phy.listheight-gui_phy.listfsize)/2,filelistbox->width+filelistbox->left,gui_phy.lcdheight,0,filelistbox->lncolor,gui_phy.listfsize,temp,1);//显示路径
	gui_memin_free(temp);
 	gui_memin_free(temp1);
}
//画filelistbox
//filelistbox:filelistbox指针
void filelistbox_draw_listbox(_filelistbox_obj *filelistbox)
{															  
	if(filelistbox==NULL)return;//无效,直接退出
	gui_fill_rectangle(filelistbox->left,filelistbox->top,filelistbox->width,gui_phy.listheight-1,filelistbox->pthinfbkcolor);//显示路径背景色
	gui_draw_hline(filelistbox->left,filelistbox->top+gui_phy.listheight-1,filelistbox->width,0X000);//显示黑色的横线		
 	filelistbox_show_path(filelistbox);		//显示路径
	if(filelistbox->scbv->totalitems>filelistbox->scbv->itemsperpage)scrollbar_draw_scrollbar(filelistbox->scbv);//必要时,画滚动条
    filelistbox_draw_list(filelistbox);	//画list
	filelistbox_show_info(filelistbox);	//显示信息
} 	   
//扫描当前路径下的所有文件
//filelistbox:filelistbox指针
//返回值:0,正常;
//    其他,操作磁盘出现错误
u8 filelistbox_scan_filelist(_filelistbox_obj *filelistbox)
{
 	DIR flboxdir;		//flbox专用	  
	FILINFO flboxfinfo;	 
	u8 res=0; 
	u16 type=0XFFFF;
	u16 typeex=0;
	u16 findex;	  
    u8 *fn;   //This function is assuming non-Unicode cfg. 
	u8 mark=0;//0,查找文件夹;1,查找目标文件;2

#if _USE_LFN						
 	flboxfinfo.lfsize=_MAX_LFN * 2 + 1;
	flboxfinfo.lfname=gui_memin_malloc(flboxfinfo.lfsize);
	if(flboxfinfo.lfname==NULL)return 0XFF;//申请内存失败 
   	gui_memset((char *)flboxfinfo.lfname,0,flboxfinfo.lfsize);
#endif	 
	 
 	if(filelistbox->path)	//路径有效
	{
		res=f_opendir(&flboxdir,(const TCHAR*)filelistbox->path); //打开一个目录
		if(res==FR_OK)		//打开目录成功了.
		{
			if(filelistbox->list!=NULL)filelist_delete(filelistbox);	//删除之前的条目
 			filelistbox->foldercnt=0;
			filelistbox->filecnt=0;
			gui_memin_free(filelistbox->findextbl);//释放之前申请的索引表内存
			filelistbox->findextbl=(u16*)gui_memin_malloc(FLBOX_MAX_DFFILE*2);//先默认申请FLBOX_MAX_DFFILE个目标文件索引内存
 		} 
		while(res==FR_OK)
		{
			findex=(u16)flboxdir.index;//记录下此文件的index值
			res=f_readdir(&flboxdir,&flboxfinfo);//读取文件信息
			if(res!=FR_OK||flboxfinfo.fname[0]==0)
			{
				if(mark==0)	//文件夹查找完了
				{
					mark=1;	//标记查找目标文件
					f_closedir(&flboxdir);
					res=f_opendir(&flboxdir,(const TCHAR*)filelistbox->path); //再次打开该目录
 					continue;
				}
				break;		//错误了/到末尾了,退出   
			}
			if (flboxfinfo.fname[0]=='.'&&flboxfinfo.fname[1]==0)continue;//忽略本级目录
			fn=(u8*)(*flboxfinfo.lfname?flboxfinfo.lfname:flboxfinfo.fname);
			if(flboxfinfo.fattrib&(1<<4)&&mark==0)//是一个文件夹且mark=0
			{
				type=FICO_FOLDER;	
				filelistbox->foldercnt++;
			}else if(flboxfinfo.fattrib&(1<<5)&&mark==1)//是一个归档文件且mark=1
			{
				type=f_typetell(fn);	//获得类型
				typeex=type&0XF;		//获得小类的编号
				type>>=4;				//获得大类的编号
 				if(((1<<type)&filelistbox->fliter)&&((1<<typeex)&filelistbox->fliterex))//大小类型都正确
				{
					if(type<10)type+=3;	//文件类型(3~8)
					else type=2;		//未知文件
					if(filelistbox->filecnt<FLBOX_MAX_DFFILE)filelistbox->findextbl[filelistbox->filecnt]=findex;//记录下文件索引
					filelistbox->filecnt++;//目标文件增加
				}else continue;	//不需要的类型	
			}else continue;		//继续找下一个 
			if(filelistbox_addlist(filelistbox,fn,type))break;//添加失败了         
		}
		if(mark==1&&filelistbox->filecnt)
		{
			fn=gui_memin_realloc(filelistbox->findextbl,filelistbox->filecnt*2);//释放多余的内存
			if(fn!=NULL)filelistbox->findextbl=(u16*)fn;//如果申请成功了,则使用新的地址,否则源地址不变
		}   
	}else//没有任何有效路径,说明回到选择磁盘的状态了
	{
		filelist_delete(filelistbox);		//删除之前的条目
		filelistbox_add_disk(filelistbox);	//重新添加磁盘路径
 	}
	gui_memin_free(flboxfinfo.lfname);
	return res;
}
//filelist添加磁盘(在filelist启动前,必须执行一次该函数)
//filelistbox:filelistbox指针
void filelistbox_add_disk(_filelistbox_obj *filelistbox)
{
	u8 i=0;
	u8 res;
	u8 tbuf[12];
	u32 sn;

	filelistbox->selindex=0;
	filelistbox->foldercnt=0;
	filelistbox->filecnt=0;
	filelistbox->memdevflag=gui_phy.memdevflag&0XFF;
	
	if(filelistbox->memdevflag&(1<<0)) 	//SD卡在位
	{
		FLIST_DISK_ORDER_TBL[i++]=0;
		res=f_getlabel ((TCHAR *)"0:",(TCHAR *)tbuf,(DWORD*)&sn);
		if(res==FR_OK&&strlen((const char *)tbuf))//读取成功，且磁盘名字不为空。
		{
			filelistbox_addlist(filelistbox,tbuf,0);//添加一个磁盘  
		}else //读取失败/磁盘盘符为空的时候，采用默认名字 
		{
			filelistbox_addlist(filelistbox,(u8*)FLIST_DISK_NAME_TBL[0][gui_phy.language],0);//添加一个磁盘
 		}  
	}
	if(filelistbox->memdevflag&(1<<1)) 	//FLASH盘在位
	{
		FLIST_DISK_ORDER_TBL[i++]=1;
		res=f_getlabel ((TCHAR *)"1:",(TCHAR *)tbuf,(DWORD*)&sn);
		if(res==FR_OK&&strlen((const char *)tbuf))//读取成功，且磁盘名字不为空。
		{
			filelistbox_addlist(filelistbox,tbuf,0);//添加一个磁盘  
		}else //读取失败/磁盘盘符为空的时候，采用默认名字 
		{
			filelistbox_addlist(filelistbox,(u8*)FLIST_DISK_NAME_TBL[1][gui_phy.language],0);//添加一个磁盘
 		}  
	}
	if(filelistbox->memdevflag&(1<<2)) 	//U盘在位
	{
		FLIST_DISK_ORDER_TBL[i++]=2;
		res=f_getlabel ((TCHAR *)"2:",(TCHAR *)tbuf,(DWORD*)&sn);
		if(res==FR_OK&&strlen((const char *)tbuf))//读取成功，且磁盘名字不为空。
		{
			filelistbox_addlist(filelistbox,tbuf,0);//添加一个磁盘  
		}else //读取失败/磁盘盘符为空的时候，采用默认名字 
		{
			filelistbox_addlist(filelistbox,(u8*)FLIST_DISK_NAME_TBL[2][gui_phy.language],0);//添加一个磁盘
 		}  
	}		 
}

//获得路径的目录深度
//path:路径
u8 filelistbox_get_pathdepth(u8* path)
{
	u8 depth=0;
	while(*path!=0)
	{
		if(*path==0X5C)//找到'\'了
		{
			depth++;
		}
		path++;
	}
	return depth;
	
}
//扫描路径下的文件
//filelistbox:filelist指针
void filelistbox_check_filelist(_filelistbox_obj *filelistbox)
{
	u8 *str;
	u8 *oldpath;
	u16 temp;
	u8 res=0;
	_filelistbox_list *filelistx;
	u16 ttlen;//总长度
	filelistx=filelist_search(filelistbox->list,filelistbox->selindex);//得到选中的list的详细信息
 	if(filelistx->type==0||filelistx->type==1)//属于磁盘/文件夹,则打开	
	{   
		temp=filelistbox_get_pathdepth(filelistbox->path);//得到当前目录的深度
		if(filelistx->type==0)//是磁盘
		{	
			str=(u8*)FLIST_DISK_PATH_TBL[FLIST_DISK_ORDER_TBL[filelistbox->selindex]];  
			gui_memin_free(filelistbox->path);//释放之前申请的内存
			filelistbox->path=(u8*)gui_memin_malloc(strlen((const char*)str)+1);//重新申请内存
			if(filelistbox->path==NULL)return;//申请内存失败.
 			strcpy((char *)filelistbox->path,(const char*)str);//拷贝str到path里面
		}else //是文件夹
		{
			if(strcmp((const char*)filelistx->name,".."))//不是..文件名
			{
				if(temp<FLBOX_PATH_DEPTH)filelistbox->seltbl[temp]=filelistbox->selindex;//记录下当前目录下选中的
			  	ttlen=strlen((const char*)filelistbox->path);//得到原来的路径的长度(不包含结束符)
				ttlen+=strlen((const char*)filelistx->name);	//得到名字的长度
				oldpath=(u8*)gui_memin_malloc(strlen((const char*)filelistbox->path)+1);//申请内存
				if(oldpath==NULL)return;//申请内存失败
 				oldpath=(u8*)strcpy((char *)oldpath,(const char*)filelistbox->path);//拷贝filelistbox->path到oldpath里面
	 			gui_memin_free(filelistbox->path);//释放filelistbox->path之前申请的内存
				filelistbox->path=(u8*)gui_memin_malloc(ttlen+2);//重新申请内存
				if(filelistbox->path==NULL)
				{
					gui_memin_free(oldpath);
					return;//申请内存失败.
				}
 				filelistbox->path=gui_path_name(filelistbox->path,oldpath,filelistx->name);//路径和文件名结合
				gui_memin_free(oldpath);//释放申请的内存.
				temp=0;//进入新目录,选择0位置.
			}else //..,进入上一层目录
			{
				filelistbox_back(filelistbox);//退回上一层目录
				return ;  
 			}
		} 
		res=filelistbox_scan_filelist(filelistbox);	//重新扫描列表
		filelistbox->selindex=temp;				//最后一次选中的条目
		if(filelistbox->scbv->totalitems>filelistbox->scbv->itemsperpage)filelistbox->scbv->topitem=temp;
		if(res==0&&filelistx->type==0&&filelistbox->foldercnt==0&&filelistbox->filecnt==0)	//磁盘根目录扫描正常,且跟目录下没有任何文件/文件夹
		{	    
 			filelistbox_addlist(filelistbox,"..",1);	//添加返回上一层伪文件夹 
		}
		filelistbox_draw_listbox(filelistbox);	//重画
 	}else 
	{
		filelistbox->dbclick|=1<<0;//标记是目标文件
	}	 
}

//向上一级操作.(返回操作)
//此函数可以单独调用,自动返回上一层.自带了扫描及重画功能.
//filelistbox:filelistbox指针
void filelistbox_back(_filelistbox_obj *filelistbox)
{
	u8 *oldpath;
	u16 temp=0; 
	oldpath=filelistbox->path;
	while(*oldpath!=0)//一直偏移到最后.
	{
		oldpath++;
		temp++;
	}
	if(temp<4)
	{
		if(filelistbox->path)gui_memin_free(filelistbox->path);//释放之前申请到的内存
		filelistbox->path=NULL;//退到最顶层
		temp=0;
	}else	   
	{	  
		while(*oldpath!=0x5c)oldpath--;	//追述到倒数第一个"\"处 
		*oldpath='\0';//用结束符代替"\".从而回到上一级目录
		temp=filelistbox_get_pathdepth(filelistbox->path);//得到新目录的深度
		if(temp<FLBOX_PATH_DEPTH)temp=filelistbox->seltbl[temp];
		else temp=0;
	}   
	filelistbox_scan_filelist(filelistbox);	//重新扫描列表
	filelistbox->selindex=temp;				//最后一次选中的条目
	if(filelistbox->scbv->totalitems>filelistbox->scbv->itemsperpage)filelistbox->scbv->topitem=temp;
	filelistbox_draw_listbox(filelistbox);	//重画
}

//滚动条条目双击钩子函数
//filelistbox:filelistbox指针
void filelistbox_2click_hook(_filelistbox_obj *filelistbox)
{	 	
//	u8 *pname; 
//	u8 t=0;
//	_filelistbox_list *filelistx;
//	filelistx=filelist_search(filelistbox->list,filelistbox->selindex);//得到选中的list的详细信息
// 	if(filelistx->type==FICO_PICTURE)//属于图片文件
//	{
//		pname=gui_memin_malloc(strlen((const char*)filelistx->name)+strlen((const char*)filelistbox->path)+2);//申请内存
//		if(pname==NULL)return;
//	    pname=gui_path_name(pname,filelistbox->path,filelistx->name);//文件名加入路径  
//		pic_viewer(pname);//打开这个文件.
//		filelistbox_draw_listbox(filelistbox);//重画filelistbox
//		gui_memin_free(pname);
//	}		  
// 	if(filelistx->type==FICO_MUSIC)//属于音乐文件
//	{
//		pname=gui_memin_malloc(strlen((const char*)filelistx->name)+strlen((const char*)filelistbox->path)+2);//申请内存
//		if(pname==NULL)return;
//	    pname=gui_path_name(pname,filelistbox->path,filelistx->name);//文件名加入路径  
//
//		mp3sta&=~(1<<0);//请求终止解码
//		while(mp3sta&0x80)delay_ms(10);//等待MP3停止播放
//		OSMboxPost(mp3mbox,pname);//发送邮箱
//		mp3sta|=1<<0;	//允许播放
//		while((mp3sta&0x80)==0&&t<200)//等待MP3开始播放
//		{
//			delay_ms(10);
//			t++;
//		}
//		filelistbox_draw_listbox(filelistbox);//重画filelistbox
//		gui_memin_free(pname);
//	}
//	printf("listbox%d index %d 2click!\r\n",filelistbox->id,filelistbox->selindex);
} 

///////////////////////////////////////测试序列//////////////////////////
_filelistbox_obj * tfilelistbox;
//创建
void tsflistbox_creat(u16 x,u16 y,u16 lenth,u16 height,u8 type,u8 fsize)
{
	tfilelistbox=filelistbox_creat(x,y,lenth,height,type,fsize);
}
//设置路径
void tsflistbox_setpath(u8 *path)
{	 
	gui_memin_free(tfilelistbox->path);//释放之前的.		   
	tfilelistbox->path=(u8*)gui_memin_malloc(strlen((const char*)path)+1);//分配内存
	if(tfilelistbox->path==NULL)return  ;//内存分配不够.	
	tfilelistbox->path=(u8*)strcpy((char *)tfilelistbox->path,(const char*)path);
}
//删除
void tsflistbox_del(void)
{
	filelistbox_delete(tfilelistbox);
	tfilelistbox=NULL;
	LCD_Clear(0XFFFF);
}	  
//添加1个listbox条目
u16 tsflistbox_addlist(u8 *name,u8 type)
{
	filelistbox_addlist(tfilelistbox,name,type);
	return tfilelistbox->scbv->totalitems; 
}
//画出listbox
void tsflistbox_draw(void)
{
	filelistbox_draw_listbox(tfilelistbox);	 
}















