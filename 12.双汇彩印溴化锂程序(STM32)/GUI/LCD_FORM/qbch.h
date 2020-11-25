#ifndef _QBCH_H
#define _QBCH_H

#ifndef _QBCH_C
#define EXT_QBCH extern
#else
#define EXT_QBCH
#endif


EXT_QBCH void T_qbch( struct _text_t *item );
EXT_QBCH void M_qbch( struct _menu_t *item );
EXT_QBCH void L_qbch( struct _list_t *item );
EXT_QBCH void IP_qbch( struct _ipad_t *item );
EXT_QBCH void T_qbch1( struct _text_t *item );



#endif