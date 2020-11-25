#include "progressbar.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//GUI-进度条 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2012/10/4
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved	
//********************************************************************************
//升级说明 
//V1.1 20140722
//修改progressbar_check函数加入模糊检测 
//////////////////////////////////////////////////////////////////////////////////

//创建进度条
//top,left,width,height:坐标.及尺寸. 
//type:按键类型 
//[bit7]:方向,0,水平;1,垂直;
//[bit6]:0,不显示浮标;1,显示浮标(百分进度条的时候,表示百分数);
//[bit5]:0,不显示进度柱,1,显示进度柱(其实就是颜色是否填充,是否跟着浮标填充颜色.)
//[bit4:2]:保留;
//[bit1:0]:0,标准进度条;1,百分数进度条;2,3,保留.
_progressbar_obj * progressbar_creat(u16 left,u16 top,u16 width,u16 height,u8 type)
{
 	_progressbar_obj * progressbar_crt;
	if(width<2||height<2)return NULL;//尺寸不能太小
	progressbar_crt=(_progressbar_obj*)gui_memin_malloc(sizeof(_progressbar_obj));//分配内存
	if(progressbar_crt==NULL)return NULL;//内存分配不够.
	progressbar_crt->top=top;
	progressbar_crt->left=left;
	progressbar_crt->width=width;
	progressbar_crt->height=height;
	progressbar_crt->type=type; 
	progressbar_crt->sta=0; 
	progressbar_crt->id=0; 
	progressbar_crt->totallen=0; 
	progressbar_crt->curpos=0; 
	//当使用百分数进度条和屏蔽进度条时,确保prgbarlen设置为0. 也就是不用显示浮标 
 	if(type&PRGB_TYPE_TEXT||(type&PRGB_TYPE_FEN)==0)progressbar_crt->prgbarlen=0;//默认滚条长度为10
	else progressbar_crt->prgbarlen=10;						//默认滚条长度为10
  	progressbar_crt->inbkcolora=PRGB_DFT_BKACOLOR;			//默认色
 	progressbar_crt->inbkcolorb=PRGB_DFT_BKBCOLOR;			//默认色 
 	progressbar_crt->infcolora=PRGB_DFT_FILLACOLOR;			//默认色
 	progressbar_crt->infcolorb=PRGB_DFT_FILLBCOLOR;			//默认色 
  	progressbar_crt->btncolor=PRGB_DFT_BTNCOLOR;			//默认色  
  	progressbar_crt->rimcolor=PRGB_DFT_RIMCOLOR;			//默认色  
  	return progressbar_crt;
}
//删除进度条
//btn_del:要删除的按键
void progressbar_delete(_progressbar_obj *progressbar_del)
{
 	gui_memin_free(progressbar_del);
}
//检查	
//prgbx:进度条指针
//in_key:输入按键指针
//返回值:0,没有有效的操作
//       1,有有效的操作			   
u8 progressbar_check(_progressbar_obj * prgbx,void * in_key)
{
#define PRGBAR_UPDOWN_EXTEND		20	//上下扩展范围 
	_in_obj *key=(_in_obj*)in_key;
	u16 x=0XFFFF;//非法值
	u16 y=0XFFFF; 
	u16 ytemp;
	u32 temp,temp1;	 
	u32  prgblen;
	u8 prgbok=0;
	switch(key->intype)
	{
		case IN_TYPE_TOUCH:	//触摸屏按下了
			if(key->ksta&0X01)//触摸屏按下
			{
				x=key->x;
				y=key->y;
			}
			if(prgbx->type&PRGB_TYPE_VER)//垂直进度条
			{
				temp=prgbx->top+prgbx->height;	//底部坐标

				if((temp-y)<prgbx->prgbarlen/2)temp1=temp;
				else temp1=y+prgbx->prgbarlen/2;//当前x坐标
 				prgblen=prgbx->height-prgbx->prgbarlen;
			}else						   	//水平进度条
			{
				if((x-prgbx->left)<prgbx->prgbarlen/2)temp=0;
				else temp=x-prgbx->prgbarlen/2;//当前x坐标
				temp1=prgbx->left;	//滑动条起始位置
				prgblen=prgbx->width-prgbx->prgbarlen;
			}
			if(prgbx->sta&PRGB_BTN_DOWN)
			{
				if(prgbx->top<PRGBAR_UPDOWN_EXTEND)ytemp=0;
				else ytemp=prgbx->top-PRGBAR_UPDOWN_EXTEND;
				if((y>ytemp)&&(y<(prgbx->top+prgbx->height+PRGBAR_UPDOWN_EXTEND)))//y坐标在误差内
				{
					y=prgbx->top;
				}  		
			}
			//在中间区域内
			if(prgbx->top<=y&&y<=(prgbx->top+prgbx->height)&&prgbx->left<=x&&x<=(prgbx->left+prgbx->width))
			{
				if((prgbx->sta&PRGB_BTN_DOWN)==0)prgbx->sta|=PRGB_BTN_DOWN;//标记被按下
				if(temp>=temp1)temp-=temp1;
				else temp=0;   
				temp=((float)temp/prgblen)*prgbx->totallen;//得到当前应该要到的位置(在总长度里面的位置)
				if(temp>prgbx->totallen)temp=prgbx->totallen;
				if(prgbx->curpos!=temp)//得到当前坐标
				{
					prgbx->curpos=temp;
					progressbar_setpos(prgbx);
				}
				prgbok=1;
			}else if(prgbx->sta&PRGB_BTN_DOWN)
			{
				prgbx->sta&=~PRGB_BTN_DOWN;//标记松开
				prgbok=1;
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
	return prgbok;
}

//5*8大小的0~9及%点阵表
const u8 pasciitbl[11][5]=
{
{0x00,0x7C,0x82,0x82,0x7C},/*"0",0*/
{0x00,0x84,0xFE,0x80,0x00},/*"1",1*/
{0x00,0xC4,0xA2,0x92,0x8C},/*"2",2*/
{0x00,0x44,0x92,0x92,0x6C},/*"3",3*/
{0x00,0x70,0x4C,0xFE,0x40},/*"4",4*/
{0x00,0x9E,0x92,0x92,0x62},/*"5",5*/
{0x00,0x7C,0x92,0x92,0x64},/*"6",6*/
{0x00,0x06,0xE2,0x12,0x0E},/*"7",7*/
{0x00,0x6C,0x92,0x92,0x6C},/*"8",8*/
{0x00,0x4C,0x92,0x92,0x7C},/*"9",9*/
{0x00,0xCC,0x2C,0xD0,0xCC},/*"%",10*/
};
//显示一个字符
//x,y:坐标
//dir:方向.0,水平;1,垂直.
//num:0~10;代表0~9:'0'~'9';10:%
//color:颜色   
void progressbar_show_char(u16 x,u16 y,u8 dir,u8 num,u16 color)
{
	u16 x0,y0;
	u8 t,i,temp;
	if(dir==0)
	{	 
		x0=x;
		for(t=0;t<5;t++)
		{
			y0=y;
			temp=pasciitbl[num][t];
			for(i=0;i<8;i++)
			{
				if(temp&0x01)gui_phy.draw_point(x0,y0,color);//水平
				y0++;	
				temp>>=1;
			}
			x0++;
		}
	}else//垂直
	{
		y0=y;
		for(t=0;t<5;t++)
		{
			x0=x;
			temp=pasciitbl[num][t];
			for(i=0;i<8;i++)
			{
				if(temp&0x01)gui_phy.draw_point(x0,y0,color);//水平
				x0++;	
				temp>>=1;
			}
			y0--;
		}
	}   
}	   
//显示一个百分数
//x,y:坐标
//dir:方向.0,水平;1,垂直.
//num:0~100;代表 0%~100%
//color:颜色   
void progressbar_show_percnt(u16 x,u16 y,u8 dir,u8 num,u16 color)
{
	u8 t,temp;
	u8 enshow=0;
	u8 len=1;
	if(num>99)len=3;
	else if(num>9)len=2;
	else len=1;
	for(t=0;t<len;t++)
	{
		temp=(num/gui_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)continue;
		    else enshow=1;  
		}
	 	if(dir==0)progressbar_show_char(x+5*t,y,dir,temp,color); 
		else progressbar_show_char(x,y-5*t,dir,temp,color); 
	}
 	if(dir==0)progressbar_show_char(x+5*t,y,dir,10,color);//显示百分号 
	else progressbar_show_char(x,y-5*t,dir,10,color); 
}
//画进度浮标
//prgbx:进度条指针
//x,y:坐标
void progressbar_draw_sign(_progressbar_obj *prgbx,u16 x,u16 y)
{
	u16 width,height;
	u8 percnt;
	if((prgbx->type&0x03)==PRGB_TYPE_STD)//标准进度条
	{
		if(prgbx->type&PRGB_TYPE_VER)//垂直的
		{
			width=prgbx->width-2;
			height=prgbx->prgbarlen;
			y=y-prgbx->prgbarlen;
			gui_fill_rectangle(x,y,width,height,prgbx->btncolor); //填充内部颜色
			gui_draw_hline(x,y,width,prgbx->rimcolor);
			gui_draw_hline(x,y+prgbx->prgbarlen-1,width,prgbx->rimcolor);
		}else  //水平的
		{
			width=prgbx->prgbarlen;
			height=prgbx->height-2;
			gui_fill_rectangle(x,y,width,height,prgbx->btncolor); //填充内部颜色
			gui_draw_vline(x,y,height,prgbx->rimcolor);
			gui_draw_vline(x+width-1,y,height,prgbx->rimcolor);
		}
	}else if((prgbx->type&0x03)==PRGB_TYPE_TEXT)//百分数进度条
	{
		percnt=(100*prgbx->curpos)/prgbx->totallen;//得到百分数
		if(prgbx->type&PRGB_TYPE_VER)//垂直的
		{
			if(prgbx->width<8&&prgbx->height<20)return;//无法显示完整的百分数,直接不显示.
			progressbar_show_percnt(prgbx->left+(prgbx->width-8)/2,prgbx->top+prgbx->height/2+10,1,percnt,prgbx->btncolor);
		}else  //水平的
		{
			if(prgbx->height<8&&prgbx->width<20)return;//无法显示完整的百分数,直接不显示.
			progressbar_show_percnt(prgbx->left+prgbx->width/2-10,prgbx->top+(prgbx->height-8)/2,0,percnt,prgbx->btncolor);
 		}
	}
}

//设置进度条的位置
//prgbx:进度条指针
void progressbar_setpos(_progressbar_obj *prgbx)
{
 	u16 prgblen;//进度柱的长度
	u16 x,y,xlen,ylen;
	u8 prgbtlen=0;//浮标长度

	if((prgbx->type&0x03)==PRGB_TYPE_TEXT)prgbtlen=0;//百分数进度条
	else if((prgbx->type&0x03)==PRGB_TYPE_STD)prgbtlen=prgbx->prgbarlen;//
	
	x=prgbx->left+1;
	y=prgbx->top+1;
	xlen=prgbx->width-2;
	ylen=prgbx->height-2;

	if(prgbx->type&PRGB_TYPE_VER)//垂直进度条
	{
		prgblen=prgbx->height-2-prgbtlen;
		gui_fill_rectangle(x,y,xlen/2,ylen,prgbx->inbkcolora);//清空进度条
	 	gui_fill_rectangle(x+xlen/2,y,xlen-xlen/2,ylen,prgbx->inbkcolorb);//清空进度条
	}else  //水平进度条
	{		 
		prgblen=prgbx->width-2-prgbtlen;
		gui_fill_rectangle(x,y,xlen,ylen/2,prgbx->inbkcolora);//清空进度条
	 	gui_fill_rectangle(x,y+ylen/2,xlen,ylen-ylen/2,prgbx->inbkcolorb);//清空进度条
	}
	if(prgbx->curpos>prgbx->totallen)prgbx->curpos=prgbx->totallen;//范围限定	
	prgblen=((float)prgbx->curpos/prgbx->totallen)*prgblen;//计算当前进度条的长度
	if(prgbx->type&PRGB_TYPE_VER)//垂直进度条 
	{		 
		x=prgbx->left+1;
		y=prgbx->top+prgbx->height-1-prgblen;	 			 
		xlen=prgbx->width-2;
 		ylen=prgblen;   
		if(prgbx->type&PRGB_TYPE_PEN)//允许显示进度条
		{
			gui_fill_rectangle(x,y,xlen/2,ylen,prgbx->infcolora);//
  			gui_fill_rectangle(x+xlen/2,y,xlen-xlen/2,ylen,prgbx->infcolorb);//
    	}
	}else//水平进度条 
	{
		x=prgbx->left+1;
		y=prgbx->top+1;
		xlen=prgblen;
		ylen=prgbx->height-2;	 	 		  
		if(prgbx->type&PRGB_TYPE_PEN)//允许显示进度条
		{
 			gui_fill_rectangle(x,y,xlen,ylen/2,prgbx->infcolora);//
  			gui_fill_rectangle(x,y+ylen/2,xlen,ylen-ylen/2,prgbx->infcolorb);//
		}
		x+=xlen;
	}
	if(prgbx->type&PRGB_TYPE_FEN)progressbar_draw_sign(prgbx,x,y);//允许画浮标/文字
	progressbar_hook(prgbx);//执行钩子函数
}
//画进度条
//prgbx:进度条指针
void progressbar_draw_progressbar(_progressbar_obj *prgbx)
{
	if(prgbx==NULL)return;//无效,直接退出
	gui_draw_rectangle(prgbx->left,prgbx->top,prgbx->width,prgbx->height,prgbx->rimcolor);	
 	progressbar_setpos(prgbx); 
}
//进度条的钩子函数
//prgbx:滑动条指针
void progressbar_hook(_progressbar_obj *prgbx)
{
	//printf("progressbar%d pos:%d\r\n",prgbx->id,prgbx->curpos);
}
//测试函数
void test_prgb(u16 x,u16 y,u16 width,u16 height,u8 type,u16 icur)
{
	_progressbar_obj* prgb;
	prgb=progressbar_creat(x,y,width,height,type);//创建进度条
	if(prgb==NULL)return;//创建失败.
							  		
	prgb->totallen=100;
	prgb->curpos=icur;
		 
	progressbar_draw_progressbar(prgb);//画按钮	 
	progressbar_delete(prgb);//删除按钮	 
}




































