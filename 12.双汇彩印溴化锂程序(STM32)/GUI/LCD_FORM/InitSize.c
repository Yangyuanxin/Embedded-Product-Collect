#define _INITSIZE_C

#include "head.h"



void SetItemPx(void *item, int8s x)
{
  switch( ((int8u *)(item))[0] )
  {
  case K_TEXT:
    ((k_text_t *)(item))->Px = x;
    break;
  case K_MENU:
    ((k_menu_t *)(item))->Px = x;
    break;
  case K_LIST:
    ((k_list_t *)(item))->Px = x;
    break;
  case K_IP:
    ((k_ipad_t *)(item))->Px = x;
    break;
  }
}


void SetItemPy(void *item, int8s y)
{
  switch( ((int8u *)(item))[0] )
  {
  case K_TEXT:
    ((k_text_t *)(item))->Py = y;
    break;
  case K_MENU:
    ((k_menu_t *)(item))->Py = y;
    break;
  case K_LIST:
    ((k_list_t *)(item))->Py = y;
    break;
  case K_IP:
    ((k_ipad_t *)(item))->Py = y;
    break;
  }
}

void qbchItem(void *item)
{
  switch( ((int8u *)(item))[0] )
  {
  case K_TEXT:
    ((k_text_t *)(item))->qbch( (struct _text_t *)item );
    break;
  case K_MENU:
    ((k_menu_t *)(item))->qbch( (struct _menu_t *)item );
    break;
  case K_LIST:
    ((k_list_t *)(item))->qbch( (struct _list_t *)item );
    break;
  case K_IP:
    ((k_ipad_t *)(item))->qbch( (struct _ipad_t *)item );
    break;
  }
}

void ItemGFocus(void *item)
{
  switch( ((int8u *)(item))[0] )
  {
  case K_TEXT:
    ((k_text_t *)(item))->GetFocus( (struct _text_t *)item );
    break;
  case K_MENU:
    ((k_menu_t *)(item))->GetFocus( (struct _menu_t *)item );
    break;
  case K_LIST:
    ((k_list_t *)(item))->GetFocus( (struct _list_t *)item );
    break;
  case K_IP:
    ((k_ipad_t *)(item))->GetFocus( (struct _ipad_t *)item );
    break;
  }
}

void *GetEnter(void *item)
{
  switch( ((int8u *)(item))[0] )
  {
  case K_MENU:
    return ((k_menu_t *)(item))->Token;
  default:
    return 0x0000;
  }
}

int8s GetItemX(void *item)
{
  switch( ((int8u *)(item))[0] )
  {
  case K_TEXT:
    return ((k_text_t *)(item))->x;
//    break;
  case K_MENU:
    return ((k_menu_t *)(item))->x;
//    break;
  case K_LIST:
    return ((k_list_t *)(item))->x;
//    break;
  case K_IP:
    return ((k_ipad_t *)(item))->x;
//    break;
  default:
  	return 0;
  }
}

int8s GetItemPx(void *item)
{
  switch( ((int8u *)(item))[0] )
  {
  case K_TEXT:
    return ((k_text_t *)(item))->Px;
//    break;
  case K_MENU:
    return ((k_menu_t *)(item))->Px;
//    break;
  case K_LIST:
    return ((k_list_t *)(item))->Px;
 //   break;
  case K_IP:
    return ((k_ipad_t *)(item))->Px;
//    break;
  default:
  	return 0;
  }
}

int8s GetItemY(void *item)
{
  switch( ((int8u *)(item))[0] )
  {
  case K_TEXT:
    return ((k_text_t *)(item))->y;
//    break;
  case K_MENU:
    return ((k_menu_t *)(item))->y;
//    break;
  case K_LIST:
    return ((k_list_t *)(item))->y;
//    break;
  case K_IP:
    return ((k_ipad_t *)(item))->y;
//    break;
  default:
  	return 0;
  }
}

int8s GetItemPy(void *item)
{
  switch( ((int8u *)(item))[0] )
  {
  case K_TEXT:
    return ((k_text_t *)(item))->Py;
//    break;
  case K_MENU:
    return ((k_menu_t *)(item))->Py;
//    break;
  case K_LIST:
    return ((k_list_t *)(item))->Py;
//    break;
  case K_IP:
    return ((k_ipad_t *)(item))->Py;
//    break;
  default:
  	return 0;
  }
}

void ItemLFocus(void *item)
{
  switch( ((int8u *)(item))[0] )
  {
  case K_TEXT:
    ((k_text_t *)(item))->LossFocus( (struct _text_t *)item );
    break;
  case K_MENU:
    ((k_menu_t *)(item))->LossFocus( (struct _menu_t *)item );
    break;
  case K_LIST:
    ((k_list_t *)(item))->LossFocus( (struct _list_t *)item );
    break;
  case K_IP:
    ((k_ipad_t *)(item))->LossFocus( (struct _ipad_t *)item );
    break;
  }
}

//获得本屏第一个对象
/*int8u GetThisDs( struct _rq_t *item )
{
  	int8u i;
  	int8u s=0;
  	int8u tmp=252;
  	for(i=0;i<item->cont;i++)
  	{
    	if (tmp!=GetItemY(item->kj[s]))
    	{
      		tmp = GetItemY(item->kj[s]);      		
      		//
      		s ++;      
    	}
    	if ((s-1)==item->df_line)    	
    	{
    		break;
    	}    	
  	}
  	//
  	return i;
}
*/
int8u GetThisDs( struct _rq_t *item )
{
  	int8u i;  	
  	int8u tmp=252;
  	for(i=0;i<item->cont;i++)
  	{
    	if (tmp!=GetItemY(item->kj[i]))
    	{
      		tmp = GetItemY(item->kj[i]);      		
      		//
      		//s ++;      
    	}
    	if (tmp>=item->df_line*2)    	
    	{
    		break;
    	}    	
  	}
  	//
  	return i;
}
//获得本屏的对象个数
//参数：
//  start:开始显示的行
int8u GetThisCont(int8u start, struct _rq_t *item )
{
  	int8u i;  	
  	int8u tmp=252;
  	int8u result=0;
  	for ( i=0; i<item->cont; i++ )
  	{
    	if (tmp!=GetItemY(item->kj[i]))
    	{
      		tmp = GetItemY(item->kj[i]);      		
    	}
    	if (tmp>=start*2)break;
  	}
  	for (; i<item->cont; i++)//=
  	{
    	if (item->Height-1 <= (GetItemY(item->kj[i])-item->df_line-item->df_line))break;	
    
    	result ++;
  	}
//  if (start!=0)outtextxy(22,item->y,"↑",2,1);
//  else outtextxy(22,item->y,"  ",2,1);
//  if (i<item->cont)outtextxy(22,item->y+item->Height-2,"↓",2,1);
//  else outtextxy(22,item->y+item->Height-2,"  ",2,1);

	if (start != 0)
	{
		GUI_PutString16_16(GUI_LCM_XMAX-16,0,"∧",1);
	}
	else
	{
		GUI_PutString16_16(GUI_LCM_XMAX-16,0,"  ",0);
	}
	if (i<item->cont)
	{
		GUI_PutString16_16(GUI_LCM_XMAX-16,GUI_LCM_YMAX-16,"∨",1);
	}
	else
	{
		GUI_PutString16_16(GUI_LCM_XMAX-16,GUI_LCM_YMAX-16,"  ",0);
	}
	

  	return result;
}

//容器初始化函数
void R_Initsize( struct _rq_t *item )
{
  int8u i;
  //清除容器位置的区域
  CurHideCur();
  GUI_RectangleFill(item->x*8, item->y*8, item->Width*8, item->Height*8,back_color);  
  //
  for (i=0; i<item->cont; i++)
  {
    SetItemPx(item->kj[i], item->x);
    SetItemPy(item->kj[i], item->y - item->df_line - item->df_line);
    	
  }
  //CurHideCur();
  //得到当前屏幕对像的个数
  item->ThisCont = GetThisCont(item->df_line,item);
  for (i=GetThisDs(item); i<(item->ThisCont)+GetThisDs(item); i++)
  {
    qbchItem(item->kj[i]);//在对应的位置上输出对应的菜单字
  }
  item->cur_ocx = item->kj[item->Focus];
//  printf("praa=%d\r\n",item->Focus);
  ItemGFocus(item->cur_ocx);//显示光标
  item->enter = GetEnter(item->cur_ocx);  
}
