#ifndef IC_H_
#define IC_H_

#include "stm32f10x.h"
#include "rtc.h"

extern int ic_link_isok(void);
extern int ic_halt(void);
extern int ic_find(u32 *id);
extern void ic_init(void);

extern int ic_read_sum(u32 *sum);
extern int ic_is_locked(void);

extern int ic_lock_card(time_t *t);
extern int ic_unlock_card(time_t *t);

extern int ic_pay(u32 sum);

extern int ic_download_key(u8 ab, u8* key, u8 sector);

#endif /* IC_H_ */
