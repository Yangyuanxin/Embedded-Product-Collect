#include "window.h"
#include "touch.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//GUI-窗体 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2012/10/4
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//********************************************************************************
//升级说明 
//V1.1 20140815
//1,修正window_creat函数和window_delete函数的小bug
//2,修改window_msg_box函数,新增时间参数
//////////////////////////////////////////////////////////////////////////////////

//读取背景色
//btnx:按钮指针
void window_read_backcolor(_window_obj * windosx)
{
	u16 x,y,ccnt;
	ccnt=0;
	for(y=windosx->top;y<windosx->top+windosx->height;y++)
	{
		for(x=windosx->left;x<windosx->left+windosx->width;x++)
		{
			windosx->bkctbl[ccnt]=gui_phy.read_point(x,y);//读取颜色
			ccnt++;
		}
	}

}
//恢复背景色
//btnx:按钮指针
void window_recover_backcolor(_window_obj * windosx)
{
	u16 x,y,ccnt;
	ccnt=0;	    
	for(y=windosx->top;y<windosx->top+windosx->height;y++)
	{
		for(x=windosx->left;x<windosx->left+windosx->width;x++)
		{
			gui_phy.draw_point(x,y,windosx->bkctbl[ccnt]);//画点	
			ccnt++;
		}
	}  
}
//创建窗口
//top,left,width,height:坐标及尺寸.
//id:window的ID
//type:window类型
//[7]:0,没有关闭按钮.1,有关闭按钮			   
//[6]:0,不读取背景色.1,读取背景色.					 
//[5]:0,标题靠左.1,标题居中.					 
//[4:2]:保留					 
//[1:0]:0,标准的窗口(仿XP);1,圆边窗口(仿Android)
//font:窗体标题文字字体大小
_window_obj * window_creat(u16 left,u16 top,u16 width,u16 height,u8 id,u8 type,u8 font)
{
 	_window_obj * window_crt;
	if(width<(WIN_BTN_SIZE+20)||height<WIN_CAPTION_HEIGHT)return NULL;//尺寸不能太小		  
	window_crt=(_window_obj*)gui_memin_malloc(sizeof(_window_obj));//分配内存
	if(window_crt==NULL)return NULL;//内存分配不够.
	window_crt->top=top;
	window_crt->left=left;
	window_crt->width=width;
	window_crt->height=height;
	window_crt->id=id;
	window_crt->type=type;
	window_crt->sta=0;
	window_crt->caption=0;
	window_crt->captionheight=WIN_CAPTION_HEIGHT;	//默认高度
	window_crt->font=font;							//设置字体大小
	window_crt->arcwinr=6;							//默认圆角的半径
	window_crt->bkctbl=0;							//默认地址为0

	window_crt->captionbkcu=WIN_CAPTION_UPC;	//默认caption上部分背景色
	window_crt->captionbkcd=WIN_CAPTION_DOWNC;	//默认caption下部分背景色
	window_crt->captioncolor=WIN_CAPTION_COLOR;	//默认caption的颜色
	window_crt->windowbkc=WIN_BODY_BKC;			//默认背景色

	if(type&(1<<7))//需要关闭按钮(仅适用仿XP窗口)
	{
		window_crt->closebtn=btn_creat(left+width-WIN_BTN_SIZE-WIN_BTN_OFFSIDE,top+(window_crt->captionheight-WIN_BTN_SIZE)/2,WIN_BTN_SIZE,WIN_BTN_SIZE,0,2);//创建边角按钮
		if(window_crt->closebtn==NULL)
		{
			window_delete(window_crt);		//释放之前申请的内存
			return NULL;					//内存分配不够.
		}
		window_crt->closebtn->caption="×";				//关闭图标
 		window_crt->closebtn->bcfucolor=WIN_BTN_FUPC;	//默认松开的颜色
		window_crt->closebtn->bcfdcolor=WIN_BTN_FDOWNC;	//默认按下的颜色
		window_crt->closebtn->bkctbl[0]=WIN_BTN_RIMC;	//圆角按钮边框颜色
		window_crt->closebtn->bkctbl[1]=WIN_BTN_TP1C;	//第一行的颜色
		window_crt->closebtn->bkctbl[2]=WIN_BTN_UPHC;	//上半部分颜色
		window_crt->closebtn->bkctbl[3]=WIN_BTN_DNHC;	//下半部分颜色

	}else window_crt->closebtn=NULL;


	if(type&(1<<6))//需要读取背景色
	{
		window_crt->bkctbl=(u16*)gui_memex_malloc(width*height*2);//要分配完整的背景色表
		if(window_crt->bkctbl==NULL)
		{
			window_crt->type&=~(1<<6);		//取消读背景状态(因为都还没读背景色)
			window_delete(window_crt);		//释放之前申请的内存
			return NULL;					//内存分配不够.
		}
		window_read_backcolor(window_crt);	//读取背景色	   
	}
    return window_crt;
}
//删除窗口
//windowx:窗口指针
void window_delete(_window_obj * window_del)
{
	if(window_del->type&(1<<6))//需要读取背景色
	{
		window_recover_backcolor(window_del);//保存了背景色,此时恢复
		gui_memex_free(window_del->bkctbl);
	}
	btn_delete(window_del->closebtn);//删除关闭按钮
	gui_memin_free(window_del);
};
//画出窗口
//windowx:窗口指针
void window_draw(_window_obj * windowx)
{
	u8 winclosebtnsize=0;	//关闭按钮的尺寸
	if(windowx==NULL)return;//无效,直接退出
	if(windowx->type&(1<<0))//圆角窗,仿android的时候,不支持按钮(放了难看)
	{
		gui_draw_arcrectangle(windowx->left,windowx->top,windowx->width,windowx->captionheight,windowx->arcwinr,1,windowx->captionbkcu,windowx->captionbkcd);									//画出标题栏
 		gui_fill_rectangle(windowx->left,windowx->top+windowx->captionheight-windowx->arcwinr,windowx->arcwinr,windowx->arcwinr,windowx->captionbkcd);											//把下部分不需要的半圆形的部分填充掉1
 		gui_fill_rectangle(windowx->left+windowx->width-windowx->arcwinr,windowx->top+windowx->captionheight-windowx->arcwinr,windowx->arcwinr,windowx->arcwinr,windowx->captionbkcd);			//把下部分不需要的半圆形的部分填充掉2
	
		gui_draw_arcrectangle(windowx->left,windowx->top+windowx->captionheight,windowx->width,windowx->height-windowx->captionheight,windowx->arcwinr,1,windowx->windowbkc,windowx->windowbkc);//填充空白部分
  		gui_fill_rectangle(windowx->left,windowx->top+windowx->captionheight,windowx->arcwinr,windowx->arcwinr,windowx->windowbkc);																//把上部分不需要的半圆形的部分填充掉1
 		gui_fill_rectangle(windowx->left+windowx->width-windowx->arcwinr,windowx->top+windowx->captionheight,windowx->arcwinr,windowx->arcwinr,windowx->windowbkc);								//把上部分不需要的半圆形的部分填充掉2
		gui_draw_arcrectangle(windowx->left,windowx->top,windowx->width,windowx->height,windowx->arcwinr,0,ARC_WIN_RIM_COLOR,ARC_WIN_RIM_COLOR);//画边框										//画出标题栏
 	}else	//方角窗
	{
		//填充内部
 		gui_fill_rectangle(windowx->left,windowx->top,windowx->width,windowx->captionheight/2,windowx->captionbkcu);//填充上半部分
		gui_fill_rectangle(windowx->left,windowx->top+windowx->captionheight/2,windowx->width,windowx->captionheight/2,windowx->captionbkcd);//填充下半部分
		gui_fill_rectangle(windowx->left,windowx->top+windowx->captionheight-1,windowx->width,windowx->height-windowx->captionheight,windowx->windowbkc);//填充内部
		//画边框
		gui_draw_rectangle(windowx->left,windowx->top,windowx->width,windowx->height,STD_WIN_RIM_OUTC);//画外框
		gui_draw_rectangle(windowx->left+1,windowx->top+1,windowx->width-2,windowx->height-2,STD_WIN_RIM_MIDC);//画中间框
		gui_draw_rectangle(windowx->left+2,windowx->top+windowx->captionheight-1,windowx->width-4,windowx->height-windowx->captionheight-1,STD_WIN_RIM_INC); //画内框
		if(windowx->type&(1<<7))
		{
			btn_draw(windowx->closebtn);//有按钮的时候需要画按钮.	
			winclosebtnsize=WIN_BTN_SIZE;
		}
	}
	if(windowx->type&(1<<5))gui_show_strmid(windowx->left,windowx->top,windowx->width,windowx->captionheight,windowx->captioncolor,windowx->font,windowx->caption);//显示标题
	else gui_show_ptstr(windowx->left+WIN_BTN_OFFSIDE,windowx->top+(windowx->captionheight-windowx->font)/2,windowx->left+windowx->width-2*WIN_BTN_OFFSIDE-winclosebtnsize,windowx->top+windowx->captionheight,0,windowx->captioncolor,windowx->font,windowx->caption,1);//靠左显示caption
}

//此定义在main.c函数里面,如果不需要tpad返回,直接去掉.
//extern volatile u8 system_task_return;		//任务强制返回标志.


#define MSG_BOX_BTN1_WIDTH 			60
#define MSG_BOX_BTN2_WIDTH 			100
#define MSG_BOX_BTN_HEIGHT 			30
//在制定位置显示一个msg box
//x,y,width,height:坐标尺寸
//str:字符串
//caption:消息窗口名字
//font:字体大小
//color:颜色
//mode:
//[7]:0,没有关闭按钮.1,有关闭按钮			   
//[6]:0,不读取背景色.1,读取背景色.					 
//[5]:0,标题靠左.1,标题居中.					 
//[4:2]:保留
//[1]:0,不显示取消按键;1,显示取消按键.
//[0]:0,不显示OK按键;1,显示OK按键.
//time:延时时间,单位:ms(仅在没有按键且需要读取背景色的时候,有效,最大65535)
//返回值:
//0,没有任何按键按下/产生了错误.
//1,确认按钮按下了.
//2,取消按钮按下了.	   
u8 window_msg_box(u16 x,u16 y,u16 width,u16 height,u8 *str,u8 *caption,u8 font,u16 color,u8 mode,u16 time)
{
	u8 rval=0,res;				 
	u16 offx=0,offy=0;	
	u16 temp,strheight=0;

 	_window_obj* twin=0;	//窗体
 	_btn_obj * okbtn=0;		//确定按钮
 	_btn_obj * cancelbtn=0; //取消按钮
														  
 	if(width<150||height<(WIN_CAPTION_HEIGHT+font+5))return 0;//尺寸错误
  	twin=window_creat(x,y,width,height,0,1|((7<<5)&mode),16);//创建窗口
	if((mode&0X03)==0x03)	//有两个按钮
	{
		offy=MSG_BOX_BTN1_WIDTH;
		offx=(width-MSG_BOX_BTN1_WIDTH*2)/3;
	}else 		   			//只有一个按钮
	{
		offy=MSG_BOX_BTN2_WIDTH;
		offx=(width-MSG_BOX_BTN2_WIDTH)/2; 
	}
	if(mode&(1<<0))//需要显示OK按键
	{
 		okbtn=btn_creat(x+offx,y+height-MSG_BOX_BTN_HEIGHT-10,offy,MSG_BOX_BTN_HEIGHT,0,0x02);//创建OK按钮
		if(okbtn==NULL)rval=1;
		else
		{
	 		okbtn->caption=(u8*)GUI_OK_CAPTION_TBL[gui_phy.language];//确认
			okbtn->bkctbl[0]=0X8452;//边框颜色
			okbtn->bkctbl[1]=0XAD97;//第一行的颜色				
			okbtn->bkctbl[2]=0XAD97;//上半部分颜色
			okbtn->bkctbl[3]=0XAD97;//0X8452;//下半部分颜色
		}
	}
  	if(mode&(1<<1))//需要显示取消按键
	{
		if(mode&(1<<0))cancelbtn=btn_creat(x+offx*2+MSG_BOX_BTN1_WIDTH,y+height-MSG_BOX_BTN_HEIGHT-10,offy,MSG_BOX_BTN_HEIGHT,0,0x02);//创建cancel按钮
		else cancelbtn=btn_creat(x+offx,y+height-MSG_BOX_BTN_HEIGHT-10,offy,MSG_BOX_BTN_HEIGHT,0,0x02);//创建cancel按钮
		if(cancelbtn==NULL)rval=1;
		else
		{
	 		cancelbtn->caption=(u8*)GUI_CANCEL_CAPTION_TBL[gui_phy.language];//确认
			cancelbtn->bkctbl[0]=0X8452;//边框颜色
			cancelbtn->bkctbl[1]=0XAD97;//第一行的颜色				
			cancelbtn->bkctbl[2]=0XAD97;//上半部分颜色
			cancelbtn->bkctbl[3]=0XAD97;//0X8452;//下半部分颜色
		}
	}	
 	if(twin==NULL)rval=1;
	else
	{
		twin->caption=caption;	
 		window_draw(twin);			//画出窗体
		btn_draw(okbtn);			//画按钮
		btn_draw(cancelbtn);		//画按钮
 		if((mode&0X03)==0)
		{
			rval=1;//不需要进入while	
			strheight=height-(WIN_CAPTION_HEIGHT+5);
		}else strheight=height-(WIN_CAPTION_HEIGHT+50+5);
 	
   		temp=strlen((const char*)str)*(font/2);	//得到字符串长度
		if(temp>=(width-10))offx=5;				//得到x的偏移
		else offx=(width-temp)/2;

		temp=gui_get_stringline(str,width-offx*2,font)*font;//得到字符串要占用的行数(像素)
 		if(temp>=strheight)offy=5;				//得到y的偏移
		else offy=(strheight-temp)/2;
		gui_show_string(str,x+offx,y+WIN_CAPTION_HEIGHT+offy,width-offx*2,strheight,font,color);	//显示要显示的文字
	}
//	system_task_return=0;//取消TPAD
	while(rval==0)
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
 		//在其他系统里面,用户可以自行去掉此句
// 		if(system_task_return)//TPAD返回
//		{	
//			rval=0XFE;//视为取消的情况,退出
// 			break;			
//		}
		delay_ms(10);							//延时10ms
		if(okbtn)
		{
			res=btn_check(okbtn,&in_obj);		//确认按钮检测
			if(res)
			{
				if((okbtn->sta&0X80)==0)		//有有效操作
				{
					rval=0XFF;
					break;//退出
				}
			}
		}	
 		if(cancelbtn)
		{
			res=btn_check(cancelbtn,&in_obj);	//返回按钮检测
			if(res)
			{
				if((cancelbtn->sta&0X80)==0)	//有有效操作
				{
					rval=0XFE;
					break;//退出
				}
			}
		}
	}
	if(((mode&0X03)==0)&&(mode&(1<<6)))delay_ms(time);//没有按钮,且需要读取背景色,则调用延时 
 	btn_delete(cancelbtn);	//删除按钮	
  	btn_delete(okbtn);		//删除按钮
	window_delete(twin);	//删除窗体
	if(rval==0XFF)return 1;	//确认
	if(rval==0XFE)return 2;	//取消
	return 0;	    		//错误或者无操作
}					    



//////////////////////////////////////////////////////////////////////////////////////////
//测试	  
void win_test(u16 x,u16 y,u16 width,u16 height,u8 type,u16 cup,u16 cdown,u8 *caption)
{
	_window_obj* twin;
	twin=window_creat(x,y,width,height,0,type,16);//创建窗口
	if(twin==NULL)return;//创建失败.
	twin->caption=caption;
 	{
		twin->windowbkc=cup; 
 	}
	window_draw(twin);
	window_delete(twin);

}












