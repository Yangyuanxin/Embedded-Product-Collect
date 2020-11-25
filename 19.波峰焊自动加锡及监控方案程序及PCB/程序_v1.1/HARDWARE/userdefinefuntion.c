#include "userdefinefuntion.h"

//小写字符转大写
void Str_UpperCase(char* s) 
{
    int i=0;
    u16 iLength = strlen(s); 
    for(i=0;i<iLength;i++)
    {
        if((s[i]>=97) && (s[i]<=122) )
        {
            s[i] -= 32;
        }
    }
}

//大写字符转小写
void Str_LowerCase(char* s)   
{
    int i=0;
    u16 iLength = strlen(s); 
    for(i=0;i<iLength;i++)
    {
        if((s[i]>=65) && (s[i]<=90) )
        {
            s[i] += 32;
        }
    }
    
}

//比较字符是否相同,不同返回0
//否则返回字符串长度
//区分大小写
u8 Str_Compare(char *source,char *p)    
{
    if((source[0]=='\0') || (p[0] == '\0'))
    {
        return 0;
    }   
    if(0 == strcmp(source,p) )
    {
        return strlen(source);
    }
    else
    {
        return 0;
    }
}

//查找source中p字符首次出现的位置
//存在返回首次出现的位置,否则返回0
//区分大小写
u16 Str_Pos(char *source,char *p)
{
    u16 i,j;
    if((source[0]=='\0') || (p[0] == '\0'))
    {
        return 0;
    }
    for(i=0,j=0;i<strlen(source);i++)
    {
        if(source[i] == p[j])
			j++;
        else
            j = 0;
        if(j == strlen(p))
        {
        	return (i-j+2);
        }
    }
    return 0;
}

//将source字符集替换为p字符集
void Str_Copy(char *source,char *p)
{
    strcpy(source,p); 
}

//将p字符集连接在source后字符集并返回
//禁止指针随机修改RAM值造成无法预测的结果
void Str_Cat(char* source,char* p) 
{
    strcat(source,p);
}

//将str字符串中的第nNUM个数字提取并转换为int型数字
//仅提取连续数字.eg: "QW233LD4325,3232"..若提取第二个，
//则返回4325,若字符串无数字，则返回0.若nNUM大于实际中数字个数，则返回最后一个连续数字的
u16 Str_StrToInt(char* str,u16 nNum)
{
    u16 iTstr=strlen(str);
    u32 iReturn=0;
    u16 inNum=0;
    u8 iFlag=0;
    u16 i=0;
    if(nNum<1)
        nNum=1;
    for(i=0;i<iTstr;i++)
    {
        if((str[i]>=48) && (str[i]<=57))
        {
            if(iFlag)
            	{continue;}
            iReturn *= 10;
            iReturn += (str[i]-0x30);
            if((iReturn>0xFFFF) || (iReturn/10000) || (str[i+1]<48) || (str[i+1]>57))
            {
                if(iReturn>0xFFFF)
                {
                    iReturn = iReturn/10;
                }
                iFlag = 1;
                inNum++;
        	}
        }
        else
        {
            iFlag = 0;
            iReturn = 0;
        }
        if(inNum==nNum)
            break;
    }
    return (u16)iReturn;
}

//将数字转换为字符串
void Str_IntToStr(u16 val,char* toStr)
{
    sprintf(toStr,"%d",val);
}
/*
void Str_IntToStr(u16 val,char toStr[])
{
    u16 tt = 10000;
	u8 i=0;
	if(val <= 0)
	{
		toStr[0] = val+0x30;
		toStr[1] = '\0';
		return ; 
	}
	for(i=0;i<8;i++)
	{
		if(val/tt%10)
		{
			break;
		}
		tt = tt/10;
	}
	if(tt == 1)
	{		
		toStr[0] = val%10+0x30;;
		toStr[1] = '\0';
	}
	else
	{   
		for(i=0;i<8;i++)
		{			
			toStr[i] = val/tt%10+0x30;
			if(tt == 1)
			{
				toStr[i+1]='\0';
				break;
			}
			tt = tt/10;
		}		
	}	
}
*/

//从第first个开始,包含第first个字符 截取length长度字符
void Str_SubString(char* p,u16 first,u16 length )
{
    u16 iT_Length=strlen(p);    //字符总长  
    u16 i;

    if(first<1)
        first = 1;
	if(length<1)
        length = 1;
    if(iT_Length < first)
        first = iT_Length;
    for(i=0;i<length;i++)
    {
        if(p[i]=='\0')
        	break;
        p[i]=p[first-1+i];
    }
    p[i]='\0';
}





