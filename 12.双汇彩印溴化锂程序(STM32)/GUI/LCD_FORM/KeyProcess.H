
#ifndef _KEY_PROCESS_H
#define _KEY_PROCESS_H

#ifndef _KEY_PROCESS_C
#define EXT_KEY_PROCESS extern
#else
#define EXT_KEY_PROCESS
#endif



EXT_KEY_PROCESS void T_KeyProcess( struct _text_t *item, int16u ky );
EXT_KEY_PROCESS void M_KeyProcess( struct _menu_t *item, int16u ky );
EXT_KEY_PROCESS void L_KeyProcess( struct _list_t *item, int16u ky );
EXT_KEY_PROCESS void L_KeyProcessGern( struct _list_t *item, int16u ky );
EXT_KEY_PROCESS void IP_KeyProcess( struct _ipad_t *item, int16u ky );
EXT_KEY_PROCESS void T_KeyProcess1( struct _text_t *item, int16u ky );
EXT_KEY_PROCESS void L_KeyProcessControl( struct _list_t *item, int16u ky );




#endif