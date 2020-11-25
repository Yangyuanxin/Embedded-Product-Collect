
#include <gm_stdlib.h>
#include <gm_memory.h>
#include <gm_fs.h>
#include <gm_timer.h>
#include "utility.h"
#include "config_service.h"
#include "log_service.h"


typedef struct 
{
	U32 clock;
}Util;

Util g_util;


GM_ERRCODE util_create(void)
{
	g_util.clock = 0;
	return GM_SUCCESS;
}


GM_ERRCODE util_destroy(void)
{
	g_util.clock = 0;
	return GM_SUCCESS;
}

GM_ERRCODE util_timer_proc(void)
{
	g_util.clock++;
	return GM_SUCCESS;
}


u8 util_chr(u8 x)
{
    u8 bRet = 0;

    if ((x>='0') && (x<='9'))
    {
       bRet = x - '0';
    }
    else if ((x>='A') && (x<='F'))
    {
       bRet = x - 'A' + 0x0a;
    }
    else if ((x>='a') && (x<='f'))
    {
       bRet = x - 'a' + 0x0a;
    }
    else
    {
        bRet = 0;
    }

    return bRet;
}


u8 util_asc(u8 x, u8 unrecognize_char)
{
    u8 bret;


    if (x <= 9)
    {
        bret = x + '0';
    }
    else if (x <= 0x0f)
    {
        bret = x - 0x0a + 'A';
    }
    else
    {
        bret = unrecognize_char;
    }

    return bret;
}


u16 util_remove_char(u8 *pdata, u16 len,char c)
{
    u16 idxr = 0;
    u16 idxw = 0;

    for (idxr=0; idxr<len; idxr++)
    {
        if (c != pdata[idxr])
        {
            pdata[idxw++] = pdata[idxr];
        }
    }
    len = idxw;
    for (; idxw<idxr; idxw++)
    {
        pdata[idxw] = 0;
    }
    return len;
}

char util_to_upper(char c)
{
	if ((c >= 'a') && (c <= 'z'))
    {
        c = c - 'a' + 'A';
    }
	return c;
}

char util_to_lower(char c)
{
	if ((c >= 'a') && (c <= 'z'))
    {
        c = c - 'A' + 'a';
    }
	return c;
}


void util_string_upper(u8 *pdata, u16 len)
{
    u16 idx;

    for (idx=0; idx < len; idx++)
    {
        if ((pdata[idx] >= 'a') && (pdata[idx] <= 'z'))
        {
            pdata[idx] = pdata[idx] - 'a' + 'A';
        }
    }
}



void util_string_lower(u8 *pdata, u16 len)
{
    u16 idx;

    for (idx=0; idx < len; idx++)
    {
        if ((pdata[idx] >= 'A') && (pdata[idx] <= 'Z'))
        {
            pdata[idx] = pdata[idx] - 'A' + 'a';
        }
    }
}



u8 util_is_valid_dns(const u8 *pdns, u16 len)
{
    u16 idx;
    u8 have_point = 0;

    if (len == 0)
    {
        return 0;
    }

    for (idx=0; idx<len; idx++)
    {
        if ((pdns[idx]<0x20) || (pdns[idx]>0x7F))
        {
            return 0;
        }

        if (IS_DNS_CHAR(pdns[idx]) == 0)
        {
            return 0;
        }

        if(pdns[idx]=='.')
        {
            have_point = 1;
        }
    }

    if(have_point)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}



u8 util_is_valid_ip(const u8 *ip, u16 len)
{
    if (len != 4)
    {
        return 0;
    }

    if(ip[0] == 0 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0)
    {
        return 0;
    }

    if(ip[0] == 255 && ip[1] == 255 && ip[2] == 255 && ip[3] == 255)
    {
        return 0;
    }

    return 1;
}




/*
    根据zone获取当前时间, pdata是bcd(基于2000)格式, time是ST_Time格式
    pdata与time 可以是NULL, 表示不输出对应格式的时间
*/
GM_ERRCODE util_get_current_local_time(u8* pdata, ST_Time* time, u8 zone)
{
    struct tm tm_t;
    time_t t;
    ST_Time mtk_t;
    u32 adj_sec = (zone & 0x7F) * 3600;

    //app内部使用zone==0 的时间, 涉及具体通信时,才转成对应的时区的时间
    t = util_get_utc_time();
    if(t == (time_t)-1)
    {
        return GM_SYSTEM_ERROR;
    }
    
    if (zone & 0x80)
    {
        t -= adj_sec;
    }
    else
    {
        t += adj_sec;
    }

    tm_t = *util_localtime(&t);

    if(!time)
    {
        time = &mtk_t;
    }
        
    util_tm_to_mtktime(&tm_t, time);

    if(pdata)
    {
        pdata[0] = HEX2BCD(time->year - BASE_YEAR_2000);
        pdata[1] = HEX2BCD(time->month);
        pdata[2] = HEX2BCD(time->day);
        pdata[3] = HEX2BCD(time->hour);
        pdata[4] = HEX2BCD(time->minute);
        pdata[5] = HEX2BCD(time->second);
    }

    return GM_SUCCESS;
}


void util_utc_sec_to_bcdtime_base2000(time_t sec_time, u8 *bcd, u8 zone)
{
    struct tm tm_t;
    ST_Time mtk_t, *time = 0;
    u32 adj_sec = (zone & 0x7F) * 3600;

    //app内部使用zone==0 的时间, 涉及具体通信时,才转成对应的时区的时间
    if(sec_time == (time_t)-1)
    {
        return;
    }
    
    if (zone & 0x80)
    {
        sec_time -= adj_sec;
    }
    else
    {
        sec_time += adj_sec;
    }

    tm_t = *util_localtime(&sec_time);
    time = &mtk_t;
        
    util_tm_to_mtktime(&tm_t, time);

    bcd[0] = HEX2BCD(time->year - BASE_YEAR_2000);
    bcd[1] = HEX2BCD(time->month);
    bcd[2] = HEX2BCD(time->day);
    bcd[3] = HEX2BCD(time->hour);
    bcd[4] = HEX2BCD(time->minute);
    bcd[5] = HEX2BCD(time->second);

    return;
}


/**
* Function:   判断是否是闰年
* Description:
* Input:	   year:年
* Output:
* Return:	   true——是闰年；false——不是闰年
* Others:
*/
bool util_is_leap_year(U16 year)
{
	if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0))
	{
		return true;
	}
	else
	{
		return false;
	}
}


static const int days[4][13] = 
{
    {31, 28, 31, 30, 31,  30,  31,  31,  30,  31,  30,  31,  0},
    {31, 29, 31, 30, 31,  30,  31,  31,  30,  31,  30,  31,  0},
    {0,  31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365},
    {0,  31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366},
};

#define WRAP(a,b,m)	((a) = ((a) <  0  ) ? ((b)--, (a) + (m)) : (a))

struct tm util_gmtime(time_t t)
{
    S32 v_tm_sec = 0;
	S32 v_tm_min = 0;
	S32 v_tm_hour = 0;
	S32 v_tm_mon = 0;
	S32 v_tm_wday = 0;
	S32 v_tm_tday = 0;
    bool leap = false;
    S32 m = 0;
	
	struct tm result;
	
    v_tm_sec = ( t %  60);
    t /= 60;
    v_tm_min = ( t %  60);
    t /= 60;
    v_tm_hour = ( t %  24);
    t /= 24;
    v_tm_tday = t;
	
    //WRAP (v_tm_sec, v_tm_min, 60);
    //WRAP (v_tm_min, v_tm_hour, 60);
    //WRAP (v_tm_hour, v_tm_tday, 24);
	
    if ((v_tm_wday = (v_tm_tday + 4) % 7) < 0)
    {
        v_tm_wday += 7;
    }
	
	//计算第多少天（从0开始）
    m =  v_tm_tday;
    result.tm_year = 70;
    leap = util_is_leap_year (result.tm_year + 1900);
    while (m >= days[leap + 2][12]) 
	{
        m -=  days[leap + 2][12];
        result.tm_year++;
        leap = util_is_leap_year (result.tm_year + 1900);
    }
	
    v_tm_mon = 0;
    while (m >= days[leap][v_tm_mon])
	{
        m -= days[leap][v_tm_mon];
        v_tm_mon++;
    }
    
    result.tm_mday = (int) m + 1;
    result.tm_yday = days[leap + 2][v_tm_mon] + m;
    result.tm_sec = v_tm_sec;
	result.tm_min = v_tm_min;
	result.tm_hour = v_tm_hour;
	result.tm_mon = v_tm_mon;
	result.tm_wday = v_tm_wday;
	
    return result;
}



enum TIME_DEF
{
	TIME_DEF_SEC = 1,
	TIME_DEF_MIN = TIME_DEF_SEC * 60,
	TIME_DEF_HOUR = TIME_DEF_MIN * 60,
	TIME_DEF_DAY = TIME_DEF_HOUR * 24,
	TIME_DEF_YEAR = TIME_DEF_DAY * 365,
};

time_t time_difference = 0; // 8 * TIME_DEF_HOUR;
static time_t util_mon_yday[2][12] =
{
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 },
	{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 },
};


long long util_get_day(int year) {
    int util_is_leap_year_year_num;
    u32 tol_day;
	year = year - 1;
	util_is_leap_year_year_num = year / 4 - year / 100 + year / 400;
	tol_day = year * 365 + util_is_leap_year_year_num;
	return tol_day;
}

time_t util_items_mktime(int year, int mon, int day, int hour, int min, int sec) {
    u32 ret = 0;
	u32 tol_day = 0;
	year += 1900;
	tol_day = util_get_day(year) - util_get_day(1970);
	tol_day += util_mon_yday[util_is_leap_year(year)][mon];
	tol_day += day - 1;

	ret = 0;
	ret += tol_day * TIME_DEF_DAY;
	ret += hour * TIME_DEF_HOUR;
	ret += min * TIME_DEF_MIN;
	ret += sec * TIME_DEF_SEC;

	return ret - time_difference;
}

time_t util_mktime(struct tm* mk) {
	if (!mk) {
		return (time_t)-1;
	}
	if (!(mk->tm_sec >= 0 && mk->tm_sec <= 59)) {
		return (time_t)-1;
	}
	if (!(mk->tm_min >= 0 && mk->tm_min <= 59)) {
		return (time_t)-1;
	}
	if (!(mk->tm_hour >= 0 && mk->tm_hour <= 23)) {
		return (time_t)-1;
	}
	if (!(mk->tm_mday >= 1 && mk->tm_hour <= 31)) {
		return (time_t)-1;
	}
	if (!(mk->tm_mon >= 0 && mk->tm_mon <= 11)) {
		return (time_t)-1;
	}
	if (!(mk->tm_year >= 70)) {
		return (time_t)-1;
	}

	return util_items_mktime(mk->tm_year, mk->tm_mon, mk->tm_mday, mk->tm_hour, mk->tm_min, mk->tm_sec);
}



struct tm * util_localtime(const time_t *srctime)
{
    static struct tm tm_time;
    long int n32_Pass4year,n32_hpery;

    // 每个月的天数  非闰年
    const static char Days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // 一年的小时数
    const static int ONE_YEAR_HOURS = 8760; // 365 * 24 (非闰年)

    //计算时差8*60*60 固定北京时间
    time_t time = *srctime;
    tm_time.tm_isdst=0;

    //取秒时间
    tm_time.tm_sec=(int)(time % 60);
    time /= 60;

    //取分钟时间
    tm_time.tm_min=(int)(time % 60);
    time /= 60;

    //计算星期
    tm_time.tm_wday=(time/24+4)%7;

    //取过去多少个四年,每四年有 1461*24 小时
    n32_Pass4year=((unsigned int)time / (1461L * 24L));

    //计算年份
    tm_time.tm_year=(n32_Pass4year << 2)+70;

    //四年中剩下的小时数
    time %= 1461L * 24L;

    //计算在这一年的天数
    tm_time.tm_yday=(time/24)%365;

    //校正闰年影响的年份,计算一年中剩下的小时数
    for (;;)
    {
        //一年的小时数
        n32_hpery = ONE_YEAR_HOURS;

        //判断闰年
        if ((tm_time.tm_year & 3) == 0)
        {
            //是闰年,一年则多24小时,即一天
            n32_hpery += 24;
        }

        if (time < n32_hpery)
        {
            break;
        }

        tm_time.tm_year++;
        time -= n32_hpery;
    }

    //小时数
    tm_time.tm_hour=(int)(time % 24);

    //一年中剩下的天数
    time /= 24;

    //假定为闰年
    time++;

    //校正润年的误差,计算月份,日期
    if ((tm_time.tm_year & 3) == 0)
    {
        if (time > 60)
        {
            time--;
        }
        else
        {
            if (time == 60)
            {
                tm_time.tm_mon = 1;
                tm_time.tm_mday = 29;
                return &tm_time;
            }
        }
    }

    //计算月日
    for (tm_time.tm_mon = 0;Days[tm_time.tm_mon] < time;tm_time.tm_mon++)
    {
        time -= Days[tm_time.tm_mon];
    }

    tm_time.tm_mday = (int)(time);
    return &tm_time;
}


void util_mtktime_to_gpstime(const ST_Time st_time,const U8 leap_sencond, double* p_tow, U16* p_wn)
{
	U8 days_per_month_table[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	U16 index = 0;
	// 天内时间
	double tod = 0;

	U16 days = 0;

	// 天内时间
	tod = st_time.hour * 3600 + st_time.minute * 60 + st_time.second;

	// 参考时间: 1980.1.6
	days = st_time.day;
	// 年->天
	for (index = 1980; index < st_time.year; index++)
	{
		if (util_is_leap_year(index))
		{
			days += 366;
		}
		else
		{
			days += 365;
		}
	}

	days -= 6;

	// 月->天
	if (util_is_leap_year(st_time.year))
	{
		days_per_month_table[1] = 29;
	}
	for (index = 1; index < st_time.month - 1; index++)
	{
		days += days_per_month_table[index - 1];
	}

	// 周数
	*p_wn = (days / 7);
	// 周内时间,闰秒修正
	*p_tow = (days % 7) * 86400 + tod + leap_sencond;
}

void util_mtktime_to_tm(const ST_Time *mtk_t, struct tm *tm_t)
{
	tm_t->tm_sec = mtk_t->second;
	tm_t->tm_min = mtk_t->minute;
	tm_t->tm_hour = mtk_t->hour;
	tm_t->tm_mday = mtk_t->day;
	tm_t->tm_mon = mtk_t->month - 1;
	tm_t->tm_year = mtk_t->year -1900;
	tm_t->tm_wday = mtk_t->dayindex;
	tm_t->tm_isdst = 0;
}

void util_tm_to_mtktime(const struct tm *tm_t, ST_Time *mtk_t)
{
	mtk_t->second = tm_t->tm_sec ;
	mtk_t->minute = tm_t->tm_min ;
	mtk_t->hour = tm_t->tm_hour ;
	mtk_t->day = tm_t->tm_mday;
	mtk_t->month= tm_t->tm_mon + 1;
	mtk_t->year = tm_t->tm_year + 1900;
	mtk_t->dayindex = tm_t->tm_wday;
}


time_t util_get_utc_time(void)
{
	ST_Time utc_time_mtk;
	struct tm utc_time_tm;
    time_t ret;
	GM_GetLocalTime(&utc_time_mtk,0);

	utc_time_tm.tm_sec = utc_time_mtk.second;
	utc_time_tm.tm_min = utc_time_mtk.minute;
	utc_time_tm.tm_hour = utc_time_mtk.hour;
	utc_time_tm.tm_mday = utc_time_mtk.day;
	utc_time_tm.tm_mon = utc_time_mtk.month - 1;
	utc_time_tm.tm_year = utc_time_mtk.year - 1900;
	utc_time_tm.tm_wday = utc_time_mtk.dayindex;
	utc_time_tm.tm_isdst = 0;
    ret = util_mktime(&utc_time_tm);
	return ret;
}


u32 util_clock(void)
{
    return g_util.clock;
}


/************************************************************************
    Function :
    Description :

    Unicode符号范围         UTF-8编码方式
       十六进制               二进制
    0000 0000-0000 007F | 0xxxxxxx
    0000 0080-0000 07FF | 110xxxxx 10xxxxxx
    0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
    0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx

    Parameter :
    Return :
    Author:
    Date:
************************************************************************/
u16 util_utf8_msg_len(const u8 *msg)
{
    u16 len = 0;

    if (msg == NULL)
    {
        return 0;
    }

    if (((*msg) & 0x80) == 0)
    {
        len = 1;   //0xxx,xxxx Normal ASCII code
    }
    else if ((((*msg)&0xe0) == 0xc0)
      && (((*(msg+1))&0xc0) == 0x80))
    {
        if (((*msg)&0xfe ) == 0xc0)  // 1100,000x is illegal data
        {
            len = 0;
        }
        else
        {
            len = 2;   // 110x,xxxx 10xx,xxxx
        }
    }
    else if ((((*msg)&0xf0) == 0xe0)
      && (((*(msg+1))&0xc0) == 0x80)
      && (((*(msg+2))&0xc0) == 0x80))
    {
        //1110,0000  100x,xxxx is illegal data
        if ((*msg ==0xe0) && (((*(msg+1))&0xe0)==0x80))
        {
            len = 0;
        }
        else
        {
            len = 3; // 1110,xxxx 10xx,xxxx 10xx,xxxx
        }
    }
    else
    {
        len = 0;
    }

    return len;
}


u16 util_utf8_to_unicode(const u8 *msg, u16 msg_len, u16 *ucs2_msg, u16 max_len)
{
    u16 byte_len=0;
    u16 total_len=0;
    u8 utf8_len = 0;

    if (msg_len == 0)
    {
        return 0;
    }


    for(; (*msg != 0x00) && (total_len <= max_len - 1) && (utf8_len < msg_len); )
    {
        byte_len = util_utf8_msg_len(msg);

        if (byte_len == 1)
        {
            (*ucs2_msg) = *(msg);
            ucs2_msg++;
            msg++;
            utf8_len++;
            total_len += 1;
        }
        else if (byte_len == 2)
        {
            *ucs2_msg = ((((*msg) >> 2) & (u8)0x07 ) & 0x00ff) << 8;
            *ucs2_msg |= (((*msg) << 6) | ((*(msg+1)) & (u8)0x3f)) & 0x00ff;
            ucs2_msg++;
            msg += 2;
            utf8_len += 2;
            total_len += 1;
        }
        else if (byte_len == 3)
        {
            *ucs2_msg = ((((*msg) << 4 ) | (((*(msg+1)) >> 2) & 0x0f)) & 0x00ff) << 8;
            *ucs2_msg |= (((*(msg+1)) << 6) | ((*(msg+2)) & (u8)0x3f)) & 0x00ff;
            ucs2_msg++;
            msg += 3;
            utf8_len += 3;
            total_len += 1;
        }
        else
        {
            msg++;
            utf8_len++;
        }
    }

    return total_len;
}


u16 util_ucs2_u16_to_u8(const u16 *input, u16 len, u8 *output)
{
    u16 i=0,j=0;

    for (i=0; i<len; i++)
    {
        output[j++] = BLOW_BYTE(input[i]);
        output[j++] = BHIGH_BYTE(input[i]);
    }

    return j;
}


u16 util_ucs2_byte_revert(u8 *pdata, u16 len)
{
    u16 idx = 0;
    u8 swap = 0;

    for (idx=0; (idx+1)<len; idx+=2)
    {
        swap = pdata[idx+1];
        pdata[idx+1] = pdata[idx];
        pdata[idx] = swap;
    }

    return len;
}



u8 util_long_to_asc(u32 value, u8 *buffer, u8 with_zero)
{
    u8 k,r,idx,jval=28;
    u32 flag = 0xffffffff;
    
    k=0;
    for (idx=0; idx<8; idx++)
    {
        r = (u8)(value>>jval);
        
        if ((r==0)&&(k==0)&&(with_zero==0)) /* withZero=1 返回8字节,withZero=0可变的字节 */
        {
            
        }
        else
        {
            *(buffer+k)=util_asc(r, '0');
            k++;
        }
        
        flag >>= 4;
        value &= flag;
        jval -= 4;
    }
    
    if (k == 0)
    {
        *buffer = '0';
        k = 1;
    }
    
    return k;
}


s32 util_delete_file(const u16 *file)
{
    s32  ret=0;
    s32  handle; 
    
    
    // 检查文件是否存在
    handle = GM_FS_CheckFile(file);
    if (handle >= 0)
    {
        // 文件存在删掉,重新创建
        ret = GM_FS_Delete(file);
    }
    
    return ret;
}


/************************************************************************
    Function :
    Description : 
        
    Parameter : 
    Return : 
    Author: 
    Date:  Seo-26-2016
************************************************************************/
u16 util_ucs2_to_ascii(const u8 *w_in, u8 *a_out, u16 len)
{
    u16 i = 0;
    
    while (w_in[i * 2] != '\0' || w_in[i * 2 + 1] != '\0')
    {
        a_out[i] = w_in[i * 2];
        i += 1;
        if (i*2 >= len)
        {
            break;
        }
    }
    a_out[i] = '\0';
    
    return i;
}

float util_mile_to_km(const float miles)
{	
	return 1.852 * miles;
}
	
bool util_isdigit(const char c)
{
	if (c >= '0' && c <= '9')
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool util_isprint(const char c)
{
	if (c >= 0x20 && c <= 0x7E)
	{
		return true;
	}
	else
	{
		return false;
	}
}


U32 util_strtoul(const char* cp,char** endp)
{
    unsigned long result = 0;
	while (util_isdigit(*cp))
	{	
		result = result * 10 + (*cp - '0');
		cp++;
	}
	if (endp)
	{
		*endp = (char*)cp;
	}
	return result;
}

S32 util_strtol(const char* cp,char** endp)
{
    if(*cp == '-')
    {
    	return (S32)(util_strtoul(cp+1,endp)) * (-1);
    }
    return util_strtoul(cp,endp);
}

GM_CHANGE_ENUM util_check_state_change(bool current_state, PStateRecord p_record, U16 true_hold_time_threshold, U16 false_hold_time_threshold)
{
	GM_CHANGE_ENUM ret = GM_NO_CHANGE;

	if (current_state)
	{
		p_record->true_state_hold_seconds++;
		p_record->false_state_hold_seconds = 0;
	}
	else
	{
		p_record->false_state_hold_seconds++;
		p_record->true_state_hold_seconds = 0;
	}
	
	//与历史状态不同
	if (current_state ^ p_record->state )
	{
		if ((p_record->true_state_hold_seconds >= true_hold_time_threshold) && current_state)
		{
			ret = GM_CHANGE_TRUE;
			p_record->state = current_state;
		}
		else if((p_record->false_state_hold_seconds >= false_hold_time_threshold) && !current_state)
		{
			ret = GM_CHANGE_FALSE;
			p_record->state = current_state;
		}
		else
		{
			ret = GM_NO_CHANGE;
		}
	}
	else
	{
		ret = GM_NO_CHANGE;
	}
	
	return ret;
}





