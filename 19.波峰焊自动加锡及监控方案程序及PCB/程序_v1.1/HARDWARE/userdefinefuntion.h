#ifndef _USERDEFINEFUNTION_H
#define _USERDEFINEFUNTION_H

#include "sys.h"
#include "string.h"
//#include "malloc.h" 
#include "main.h"

#include <stdlib.h>

#define TEMPDATA_BUF_LENGTH (256*1) //临时数据长度，依据需要修改



void Str_UpperCase(char* s);//小写字符转大写
void Str_LowerCase(char* s) ;//大写字符转小写
void Str_Copy(char *source,char *p);//将source字符集替换为p字符集
void Str_Cat(char* source,char* p);//将p字符集连接在source后字符集
u8  Str_Compare(char *source,char *p);//比较字符是否相同,不同返回0,否则返回字符串长度
u16 Str_Pos(char *source,char *p);//查找source中p字符首次出现的位置,存在返回首次出现的位置,否则返回0
void Str_IntToStr(u16 val,char toStr[]);//将数字转换为字符串
void Str_SubString(char* p,u16 first,u16 length );//从第first个开始,包含第first个字符 截取length长度字符
u16 Str_StrToInt(char* str,u16 nNum);//将str字符串中的第nNUM个数字提取并转换为int型数字

char* Str_Cat111(char* source,char* p) ;


#endif




