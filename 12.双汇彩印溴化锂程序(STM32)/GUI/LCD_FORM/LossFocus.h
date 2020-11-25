#ifndef _LOSEFOCUS_H
#define _LOSEFOCUS_H

#ifndef _LOSEFOCUS_C
#define EXT_LOSEFOCUS extern
#else
#define EXT_LOSEFOCUS
#endif



EXT_LOSEFOCUS void T_LossFocus( struct _text_t *item );
EXT_LOSEFOCUS void M_LossFocus( struct _menu_t *item );
EXT_LOSEFOCUS void L_LossFocus( struct _list_t *item );
EXT_LOSEFOCUS void IP_LossFocus( struct _ipad_t *item );
EXT_LOSEFOCUS void T_LossFocus1( struct _text_t *item );




#endif