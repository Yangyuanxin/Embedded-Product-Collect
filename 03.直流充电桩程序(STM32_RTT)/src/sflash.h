#ifndef SFLASH_H_
#define SFLASH_H_

#include"stm32f10x.h"

int sflash_write_page(u32 page_addr, u8 *buf);
int sflash_read_page(u32 page_addr, u8 *buf);
int sflash_save_cfg(void);

int sflash_read(u32 page_addr, u16 offset, u8 *buf, u16 len);
int sflash_write(u32 page_addr, u16 offset, u8 *buf, u16 len);

void sflash_init(void);

#endif
