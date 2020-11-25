#include "head.h"

//文本框失去焦点函数
void T_LossFocus( struct _text_t *item )
{
//  int8u tx,ty;  //绝对坐标
//  int8s str[20];
//  tx = item->x + item->Px + strlen(item->Caption1);
//  ty = item->y + item->Py;
//  if (item->MaskChar == 0)
//    sprintf(str,wid_dec_strs[item->Width-1],item->Value);
//  else
//  {
//    int8u i;
//    for (i=0; i<item->Width; i++)
//      str[i] = item->MaskChar;
//  }
//  outtextxy(tx,ty,str,item->Width,0);
//  hide_cur();
}

//菜单获得焦点函数
void M_LossFocus( struct _menu_t *item )
{
  return;
}

void L_LossFocus( struct _list_t *item )
{
  return;
}

void IP_LossFocus( struct _ipad_t *item )
{
//  int8u tx,ty;
//  int8s s[10];
//  tx = item->x+ item->Px+ strlen(item->Caption1)+ 4*(item->wz);
//  ty = item->y+ item->Py;
//  sprintf(s,"%03d",item->Val[item->wz]);
//  outtextxy(tx, ty, s, 3, 0);
//  hide_cur();
}

void T_LossFocus1( struct _text_t *item )
{
  return;
}