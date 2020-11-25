#include "edit.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//GUI-编辑框 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2012/10/4
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									   
//********************************************************************************
//升级说明 
//V1.1 20140825
//修改edit_check函数加入模糊检测 
//////////////////////////////////////////////////////////////////////////////////

//创建编辑框
//top,left,width,height:坐标.
//id:编辑框ID
//type:编辑框类型
//[7:3]:保留
//[2]:0,带边框;1,不带边框(边框采用背景色)
//[1]:0,不显示光标;1,显示光标;
//[0]:
//0,标准edit,满之不能再写入.
//1,自动清除edit,满之后自动清除,回到原位.
//font:编辑框字体
_edit_obj * edit_creat(u16 left,u16 top,u16 width,u16 height,u8 id,u8 type,u8 font)
{
 	_edit_obj * edit_crt;
	u16 temp;
	if(width<6+6||height<12+6)return NULL;//尺寸不能太小		  
	edit_crt=(_edit_obj*)gui_memin_malloc(sizeof(_edit_obj));//分配内存
	if(edit_crt==NULL)return NULL;//内存分配不够.
	edit_crt->top=top;
	edit_crt->left=left;
	edit_crt->width=width;
	edit_crt->height=height;
	edit_crt->id=id;
	edit_crt->type=type;
	edit_crt->sta=0;	 
	edit_crt->text=NULL;  
 	edit_crt->font=font;			//字体
	temp=(edit_crt->width-6)/(edit_crt->font/2)+1;		
	edit_crt->text=gui_memin_malloc(temp);//为edit申请字符串存储空间
	if(edit_crt->text==NULL)		//申请失败
	{
		edit_delete(edit_crt);
		return NULL;
	}
	gui_memset(edit_crt->text,0,temp);			//将该内存块清零
	edit_crt->cursorpos=0;					//光标位置为原点
	edit_crt->edit_timer_old=0;			 	//时间为0	 
	edit_crt->textcolor=EDIT_DFT_TXTC;
	edit_crt->textbkcolor=EDIT_DFT_TBKC;	 
    return edit_crt;
}
//删除编辑框
//edit_del:要删除的编辑框
void edit_delete(_edit_obj * edit_del)
{
	gui_memin_free(edit_del->text);
	gui_memin_free(edit_del);
}	  
//得到光标的当前x坐标
//editx:编辑框
u16 edit_get_cursorxpos(_edit_obj * editx)
{
	return editx->left+3+editx->cursorpos*(editx->font/2);//光标的X坐标;
}	  
//画出光标
//editx:编辑框
//color:光标颜色
void edit_draw_cursor(_edit_obj * editx,u16 color)
{
	u16 x,y;
	u8 i;
	x=edit_get_cursorxpos(editx);//光标的X坐标
	y=editx->top+3;
 	for(i=0;i<editx->font;i++)
	{
		if(i>(editx->top+editx->height-3-y))break;//剩余部分不需要显示
		gui_phy.draw_point(x,y+i,color);
 		gui_phy.draw_point(x+1,y+i,color);					 
	}
}		 
//显示光标
//editx:编辑框
//sta:0,不显示出来;1,显示出来
void edit_show_cursor(_edit_obj * editx,u8 sta)
{
	u8 txt[3];
	u16 color;
	if((editx->type&0x02)==0)//不用显示光标
	{
		if((editx->sta&0x02)==0)sta=0;//之前是有显示光标的,先清除光标
		else return;
	}
	if(sta==0)
	{
		color=editx->textbkcolor;//不需要显示,用背景色
 		editx->sta&=~(1<<1);//设置状态为未显示
	}else 
	{
		color=editx->textcolor;//需要显示,用文字色
		editx->sta|=1<<1;//设置状态为显示
	}	 
	edit_draw_cursor(editx,color);//显示光标
	if(sta==0)//不要显示光标
	{
		txt[0]=editx->text[editx->cursorpos];
		if(txt[0]>0X80)
		{
			txt[1]=editx->text[editx->cursorpos+1];
			txt[2]='\0';
		}else txt[1]='\0';
		gui_phy.back_color=editx->textbkcolor;//设置背景色
		gui_show_ptstr(edit_get_cursorxpos(editx),editx->top+3,editx->left+editx->width-3,editx->top+editx->height-3,0,editx->textcolor,editx->font,txt,0);//-4是因为边框占了3个像素,另外空一个像素
	}
 
}	
//光标闪烁	 
//editx:编辑框
void edit_cursor_flash(_edit_obj *editx)
{
 	if(gui_disabs(editx->edit_timer_old,GUI_TIMER_10MS)>=50)//超过ms了.
	{
		editx->edit_timer_old=GUI_TIMER_10MS;
		if(editx->sta&0x02)edit_show_cursor(editx,0);//之前是显示出来了的
		else edit_show_cursor(editx,1);//之前没有显示出来	 
	}
}
//显示文字串
//editx:编辑框
void edit_draw_text(_edit_obj * editx)
{
	if(editx->type&(1<<2))gui_fill_rectangle(editx->left,editx->top,editx->width,editx->height,editx->textbkcolor);//填充整个
	else gui_fill_rectangle(editx->left+2,editx->top+2,editx->width-3,editx->height-3,editx->textbkcolor);//填充内部 
	gui_phy.back_color=editx->textbkcolor;
 	gui_show_ptstr(editx->left+3,editx->top+3,editx->left+editx->width-3,editx->top+editx->height-3,0,editx->textcolor,editx->font,editx->text,0);
	editx->sta&=~(1<<1);		//设置光标状态为未显示
	edit_show_cursor(editx,1); 	//显示光标
}

//在edit里面添加字符串
//editx:编辑框
//str:字符串
void edit_add_text(_edit_obj * editx,u8 * str)
{				  
	u8 *tempstr;
	u16 temp;
	u16 temp2;
	u8 *laststr='\0';
	if(*str!=NULL)//有内容
	{
		tempstr=editx->text+editx->cursorpos;
		if(*str!=0x08)//不是退格
		{
			temp=strlen((const char*)str)+strlen((const char*)editx->text);//得到他们相加后的长度
			temp2=(editx->width-6)/(editx->font/2);//得到编辑框所支持的最大长度
			if(temp2<temp)
			{
				if(editx->type&0x01)//自动清除型edit,超过范围直接清空
				{
					editx->cursorpos=0;//光标回0
					tempstr='\0';	
				}else return;//太长了.直接不执行操作
			}
		}
		if(tempstr!='\0')//不是结束符
		{
			temp=strlen((const char*)tempstr);//得到tempstr的长度
			laststr=gui_memin_malloc(temp+1);//申请内存
			if(laststr==NULL)return;//申请失败,直接退出.
			laststr[0]='\0';//结束符
			strcpy((char*)laststr,(const char*)tempstr);//复制tempstr的内容到laststr
		}
		if(*str==0x08)//退格
		{
			if(editx->cursorpos>0)//可以退格
			{
				editx->cursorpos--;
				if(editx->text[editx->cursorpos]>0x80)editx->cursorpos--;//是汉字,要退2格
				editx->text[editx->cursorpos]='\0';//添加结束符
				strcat((char*)editx->text,(const char*)laststr);//再将他们拼起来   	
			} 	
		}else if(*str>=' ')//是字符串
		{
			editx->text[editx->cursorpos]='\0';//添加结束符
			strcat((char*)editx->text,(const char*)str);//凑起来   	
			editx->cursorpos+=strlen((const char*)str);//光标偏移
			strcat((char*)editx->text,(const char*)laststr);//后续的再凑起来   	
		}
		gui_memin_free(laststr);//释放内存
		edit_draw_text(editx);//重新显示字符串
	}
}	 	  
//画编辑框
//editx:编辑框
void edit_draw(_edit_obj * editx)
{
	u8 sta;
	if(editx==NULL)return;//无效,直接退出
	sta=editx->sta&0x03;
	if((editx->type&(1<<2))==0)//带边框? 画边框
	{ 
		gui_draw_vline(editx->left,editx->top,editx->height,EDIT_RIM_LTOC);			//左外线
		gui_draw_hline(editx->left,editx->top,editx->width,EDIT_RIM_LTOC);			//上外线
		gui_draw_vline(editx->left+1,editx->top+1,editx->height-2,EDIT_RIM_LTIC);	//左内线
		gui_draw_hline(editx->left+1,editx->top+1,editx->width-2,EDIT_RIM_LTIC);	//上内线
		gui_draw_vline(editx->left+editx->width-1,editx->top,editx->height,EDIT_RIM_RBOC);		//右外线
		gui_draw_hline(editx->left,editx->top+editx->height-1,editx->width,EDIT_RIM_RBOC);		//下外线
		gui_draw_vline(editx->left+editx->width-2,editx->top+1,editx->height-2,EDIT_RIM_RBIC);	//右内线
		gui_draw_hline(editx->left+1,editx->top+editx->height-2,editx->width-2,EDIT_RIM_RBIC);	//下内线
	}
	edit_draw_text(editx); 
	if(sta&0x80)//编辑框选中有效
	{
	}else
	{
	}	 	    		
}

		 
//根据x坐标重设cursor的位置
//editx:编辑框
//x:x坐标
void edit_cursor_set(_edit_obj * editx,u16 x)
{
	u16 newpos;
	u16 maxpos=strlen((const char *)editx->text);
	if(x>editx->left+3)
	{
		x-=editx->left+3;
		newpos=x/(editx->font/2);
		if((x%(editx->font/2))>editx->font/2)newpos+=1;//大于font/2,则认为在下一个位置
		edit_show_cursor(editx,0);		//清除前一次光标的位置
		if(newpos>maxpos)newpos=maxpos;	//不能超过text的长度
		if(newpos>0)
		{
			maxpos=gui_string_forwardgbk_count(editx->text,newpos-1);//得到之前的汉字内码个数.
			if((maxpos%2)!=0)newpos++;//是在汉字中间,需要跳过一个字节
		}
		editx->cursorpos=newpos;
		edit_show_cursor(editx,1);//重新显示新的光标
	} 	
}

//检查	
//editx:编辑框
//in_key:输入按键指针
//返回值:未用到			 					   
u8 edit_check(_edit_obj * editx,void * in_key)
{
#define	EDIT_UD_EXTEND		20	//上下扩展范围 
#define	EDIT_LR_EXTEND		20	//左右扩展范围 
	_in_obj *key=(_in_obj*)in_key;
	u16 ttop,tleft;
 	static u8 chooseflag=0;//选中标记
  	switch(key->intype)
	{
		case IN_TYPE_TOUCH:	//触摸屏按下了
		 	edit_cursor_flash(editx);
			if(chooseflag==1)//扩展模式
			{
				if(editx->top>=EDIT_UD_EXTEND)ttop=editx->top-EDIT_UD_EXTEND;//计算新的top值
				else ttop=0;
				if(editx->left>=EDIT_LR_EXTEND)tleft=editx->left-EDIT_LR_EXTEND;//计算新的left值
				else tleft=0; 
				if(ttop<key->y&&key->y<(editx->top+editx->height+EDIT_UD_EXTEND)&&(tleft<key->x)&&key->x<(editx->left+editx->width+EDIT_LR_EXTEND))//在拓展edit框内部
				{ 
					if(key->x>editx->left+editx->width)tleft=editx->left+editx->width-1;	//强制在编辑框内
					if(key->x<editx->left)tleft=editx->left+1;								//强制在编辑框内 
					edit_cursor_set(editx,tleft);//得到当前光标的x坐标		 
				}else chooseflag=0;				
			}else if(editx->top<key->y&&key->y<(editx->top+editx->height)&&(editx->left<key->x)&&key->x<(editx->left+editx->width))//还没选中,判断在真实的edit框内部
			{
				chooseflag=1;//标记被选中了,启用扩展模式
				edit_cursor_set(editx,key->x);//得到当前光标的x坐标		 
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


//////////////////////////////////////////////////////////////////////////
void edit_test(u16 x,u16 y,u16 width,u16 height,u8 type,u8 sta,u8 *text)
{
	_edit_obj* tedit;
	tedit=edit_creat(x,y,width,height,0,type,16);//创建按钮
	if(tedit==NULL)return;//创建失败.
	tedit->sta=sta;
	strcpy((char*)tedit->text,(const char *)text);
	edit_draw(tedit);//画按钮
 	edit_delete(tedit);//删除按钮	 
} 






















