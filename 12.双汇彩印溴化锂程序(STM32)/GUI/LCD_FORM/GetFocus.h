
#ifndef _GETFOCUS_H
#define _GETFOCUS_H

#ifndef _GETFOCUS_C
#define EXT_GETFOCUS extern
extern const int8s *wid_dec_strs;
#else
#define EXT_GETFOCUS
#endif



EXT_GETFOCUS UINT8 cur_s;
EXT_GETFOCUS void T_GetFocus( struct _text_t *item );
EXT_GETFOCUS void M_GetFocus( struct _menu_t *item );
EXT_GETFOCUS void L_GetFocus( struct _list_t *item );
EXT_GETFOCUS void IP_GetFocus( struct _ipad_t *item );
EXT_GETFOCUS void T_GetFocus1( struct _text_t *item );





#endif