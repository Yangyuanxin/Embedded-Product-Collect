#define _SETFOCUS_C

#include "head.h"

//对容器内的东东分配焦点

void R_SetFocus( struct _rq_t *item )
{
  int8u *c;
  c = (int8u *)item->cur_ocx;
  switch(c[0])
  {
  case K_TEXT:
    ((k_text_t *)(c))->GetFocus( (struct _text_t *)(c) );
    break;
  case K_MENU:
    ((k_menu_t *)(c))->GetFocus( (struct _menu_t *)(c) );
    break;
  case K_LIST:
    ((k_list_t *)(c))->GetFocus( (struct _list_t *)(c) );
    break;
  }
}
