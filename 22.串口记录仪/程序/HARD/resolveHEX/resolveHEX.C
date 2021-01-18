#include "resolveHEX.h"

// ASCII映射表
const static u8 ascii2byte[71] = {
    0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
    0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
    0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X00,0X01,0X02,0X03,0X04,0X05,0X06,0X07,0X08,0X09,0XFF,0XFF,
    0XFF,0XFF,0XFF,0XFF,0XFF,0X0A,0X0B,0X0C,0X0D,0X0E,0X0F
};
// 字符串的两位数转换成1个HEX字节
u8 StrWord2Byte(u16* ascii)
{
     u8* dat = (u8*)ascii;
	// u8 datL = 0;
	// u8 datH = 0;
	// if(dat->dat[0] > '9') datL = (dat->dat[0] - 55)<<4;
	// else datL = (dat->dat[0] - '0')<<4;

	// if(dat->dat[1] > '9') datH = (dat->dat[1] - 55);
	// else datH = (dat->dat[1] - '0');
	// return datL|datH;
    return (ascii2byte[dat[0]]<<4 | ascii2byte[dat[1]]);
}

// 256模校验，0 - 相加值
u8 Get256Mo(u16* buf,u32 len)
{
    u32 i =0;
    u32 cc = 0;
    for(i=0;i<len;i++)
    {
         cc += StrWord2Byte(&buf[i]);  
    }
    return (0 - cc);
}

// 处理HEX文件单条信息
hexError ResolveHex(hexType* hexhead,u8* buf,u8 buflen)
{
    hexError err = ok;
	hexType0* hexhead0;
    u8 checkSum ;
    if(buf[0] != ':' || buf[buflen-2] != '\r' || buf[buflen-1] != '\n')return headError;
    hexhead0 = (hexType0*)&buf[1];
    hexhead->len = StrWord2Byte(&hexhead0->ll);
    checkSum = StrWord2Byte((u16*)&buf[9 + hexhead->len * 2]);//获得校验值
	u16* pword = (u16*)&buf[1];
    if(checkSum != Get256Mo(pword,hexhead->len + 4))
		return ccError;   // 校验出错
    hexhead->type = StrWord2Byte(&hexhead0->tt);
    hexhead->addr = (((u32)(hexhead0->aaaa[0] - '0'))<<12)|\
                    (((u32)(hexhead0->aaaa[1] - '0'))<<8)|\
                     (((u32)(hexhead0->aaaa[2] - '0'))<<4)|\
                     ((u32)(hexhead0->aaaa[3] - '0'));
    // 保存数据
    u16* pdat = (u16*)&buf[9];
    for(u8 i=0;i<hexhead->len;i++)
	{
		hexhead->datbuf[i] = StrWord2Byte(&pdat[i]);
	}
    return err;
}
