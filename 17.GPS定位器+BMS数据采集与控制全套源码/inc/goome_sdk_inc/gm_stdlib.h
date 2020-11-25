/*****************************************************************************
*
* www.goome.net
*
*****************************************************************************/



#ifndef __GM_STDLIB_H__
#define __GM_STDLIB_H__

#include "gm_type.h"
#include "stdio.h"

extern s32   GM_atoi(const char* s);
extern double GM_atof(const char* s);
extern void* GM_memset(void* dest, u8 value, u32 size);
extern void* GM_memcpy(void* dest, const void* src, u32 size);
extern s32   GM_memcmp(const void* dest, const void*src, u32 size);
extern void* GM_memmove(void* dest, const void* src, u32 size);
extern char* GM_strcpy(char* dest, const char* src);
extern char* GM_strncpy(char* dest, const char* src, u32 size);
extern char* GM_strcat(char* s1, const char* s2);
extern char* GM_strncat(char* s1, const char* s2, u32 size);
extern s32   GM_strcmp(const char*s1, const char*s2);
extern s32   GM_strncmp(const char* s1, const char* s2, u32 size);
extern char* GM_strchr(const char* s1, s32 ch);
extern u32   GM_strlen(const char* str);
extern char* GM_strstr(const char* s1, const char* s2);
extern s32 GM_sprintf(char *, const char *, ...);
extern s32 GM_snprintf(char *, u32, const char *, ...);
extern s32 GM_vsprintf(char*, const char*, __va_list);
extern s32 GM_sscanf(const char*, const char*, ...);
extern s32 GM_toupper(s32 c);
extern s32 GM_tolower(s32 c);
extern s32 GM_isdigit(char c);


#endif  // __GM_STDLIB_H__
