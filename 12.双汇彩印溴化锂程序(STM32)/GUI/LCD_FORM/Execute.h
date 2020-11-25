#ifndef _EXTCUTE_H
#define _EXTCUTE_H

#ifndef _EXECUTE_C
#define EXT_EXECUTE extern
#else
#define EXT_EXECUTE
#endif

EXT_EXECUTE UINT16 ocx_key_type;	//从容器执行函数退出时的触发键值

EXT_EXECUTE int8u GetItemFocus(void *item);
EXT_EXECUTE void *CurFocus( void **item, int8u f, int8u c);
EXT_EXECUTE void PutKey(void *item, int16u k);
EXT_EXECUTE void Inititem(void *item);
EXT_EXECUTE void Submis(void *item);
EXT_EXECUTE int8u find_down(struct _rq_t *Item);
EXT_EXECUTE int8u find_up(struct _rq_t *Item);
EXT_EXECUTE int8u find_left(struct _rq_t *Item);
EXT_EXECUTE int8u find_right(struct _rq_t *Item);

EXT_EXECUTE void R_Initsize( struct _rq_t *item );
EXT_EXECUTE void R_Execute(struct _rq_t *item);
//










#endif