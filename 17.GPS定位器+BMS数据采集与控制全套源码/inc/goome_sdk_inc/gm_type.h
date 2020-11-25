/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        common_type_defines.h
 * Author:           王志华
 * Version:          1.0
 * Date:             2019-02-28
 * Description:      通用数据类型定义
 * Others:
 * Function List:
 * History:
    1. Date:         2019-02-28
       Author:       王志华
       Modification: 创建初始版本
    2. Date:
	   Author:
	   Modification:

 */

#ifndef __COMMON_TYPE_DEFINES_H__
#define __COMMON_TYPE_DEFINES_H__

typedef unsigned char U8,u8;
typedef signed char S8,s8;
typedef unsigned short U16,u16;
typedef signed short S16,s16;
typedef unsigned int U32,u32;
typedef signed int S32,s32;
typedef unsigned long long U64,u64;
typedef long long S64,s64;

#ifndef NULL
#ifdef __cplusplus
#define NULL (0)
#else
#define NULL ((void *)0)
#endif
#endif
#ifndef null
#define null NULL
#endif

#ifndef __cplusplus
#ifndef bool
typedef U8 bool;
typedef U8 BOOL;
#define true 1
#define TRUE 1
#define false 0
#define FALSE 0
#endif
#endif

typedef unsigned int UINT;
typedef unsigned char kal_uint8;
typedef signed char kal_int8;
typedef char kal_char;
typedef unsigned short kal_wchar;
typedef unsigned short int kal_uint16;
typedef signed short int kal_int16;
typedef unsigned int kal_uint32;
typedef signed int kal_int32;

typedef U16 module_type;
typedef U16 msg_type;
typedef U16 sap_type;
typedef int MMI_BOOL;

typedef enum 
{
    KAL_FALSE,
    KAL_TRUE
} kal_bool;
	
typedef void (*FuncPtr) (void);
typedef void (*PsFuncPtr) (void *);

#define GOOME_APN_MAX_LENGTH 30
#define GOOME_DNS_MAX_LENTH  50
#endif


