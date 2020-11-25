#ifndef _INITSIZE_H
#define _INITSIZE_H

#ifndef _INITSIZE_C
#define EXT_INITSIZE extern
#else
#define EXT_INITSIZE
#endif

EXT_INITSIZE void ItemGFocus(void *item);
EXT_INITSIZE int8s GetItemY(void *item);
EXT_INITSIZE int8s GetItemX(void *item);
EXT_INITSIZE void ItemLFocus(void *item);

EXT_INITSIZE void qbchItem(void *item);

EXT_INITSIZE void SetItemPx(void *item, int8s x);
EXT_INITSIZE void SetItemPy(void *item, int8s y);

EXT_INITSIZE void R_Initsize(struct _rq_t *item );
EXT_INITSIZE void FixInitsize(struct _rq_t *item);

EXT_INITSIZE void *GetEnter(void *item);

EXT_INITSIZE int8u GetThisDs( struct _rq_t *item );
EXT_INITSIZE int8u GetThisCont(int8u start, struct _rq_t *item );

#endif