#ifndef __RESOLVEHEX_H
#define __RESOLVEHEX_H

#include "uhead.h"

typedef enum{
    ok = 0,
    headError = 1,  // 头不是“:”
    ccError = 2,    // 数据校验错误

}hexError;

#define     TYPE_DATARECORDER          0         // 数据记录
#define     TYPE_ENDFILE               1         // 文件结束记录
#define     TYPE_ELAR                  4         // 扩展线性地址记录,


typedef struct{
    u16 ll;         // 字符型数据长度域
    u8 aaaa[4];    // 字符型地址域, 它代表记录当中数据的起始地址
    u16 tt;         // 字符型代表HEX 记录类型的域
}hexType0;

typedef struct{
	u32 addr;       // 地址域
	u8 datbuf[16];  // 保存数据
    u8 len;         // 数据长度域
    u8 type;        // 代表HEX 记录类型的域
}hexType;

/* 函数 */

hexError ResolveHex(hexType* hexhead,u8* buf,u8 buflen);

#endif // !__RESOLVEHEX_H
