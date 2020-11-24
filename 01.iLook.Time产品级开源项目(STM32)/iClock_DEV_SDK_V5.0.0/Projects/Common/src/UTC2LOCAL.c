void UTCTime2LocalTime(void);   
bool IsLeapYear(unsigned int uiYear);   
   
void UTCTime2LocalTime(tm *local_time, tm *utc_time, int iTimeZone)
{   
    tm *st;       //UTC时间
    tm *st_l      //LOCAL时间
    int iHourInterval,iInterval,iDays=0;
    
    memset(&st,0,sizeof(tm));
    memset(&st_l,0,sizeof(tm));   
   
    memcpy(&st, utc_time, sizeof(tm)); 
    memcpy(&st_l, utc_time, sizeof(tm)); 


    //将UTC时间加上本地时区小时数   
    st_l.hour += iTimeZone;
   
    //根据加上时区后的时间刷新本地时间   
    //根据小时数判断日期是否需要进位   
    iHourInterval = st_l.hour/24;
    
    st_l.hour %= 24;   
    if(iHourInterval>0)   
    {   
        //小时数已经大于24，日期需要加1   
        st_l.mday += 1;   
   
        //小时数加1后继续判断天数是否需要加1   
        //要根据当前月份判断   
        switch(st_l.month)   
        {   
        case 1:   
        case 3:   
        case 5:   
        case 7:   
        case 8:   
        case 10:   
        case 12:   
            {   
                iDays = 31;   
            }   
            break;   
        case 2:   
            {   
                //2月份比较特殊，需要根据是不是闰年来判断当月是28天还29天   
                iDays = IsLeapYear(st.year)?29:28;   
            }   
            break;   
        case 4:   
        case 6:   
        case 9:   
        case 11:   
            {   
                iDays = 30;   
            }   
            break;   
        }   
   
        iInterval = st_l.mday-iDays);
        if(iInterval>0)   
        {   
            st_l.mday = iInterval;   
            st_l.month += 1;   
   
            //判断月份是否大于12，如果大于12，则年份需要再加1   
            iInterval = st_l.month/12;   
            st_l.wMonth %= 12;   
            if(iInterval>0)   
            {   
                st_l.year += 1;   
            }   
        }   
    } 
#if 0   
    printf("当前 UTC 日期时间： %04d-%02d-%02d %02d:%02d:%02d\n",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);   
    printf("当前 本地日期时间： %04d-%02d-%02d %02d:%02d:%02d\n",st_l.wYear,st_l.wMonth,st_l.wDay,st_l.wHour,st_l.wMinute,st_l.wSecond);   
#endif
}   
   
bool IsLeapYear(unsigned int uiYear)   
{   
    return (((uiYear%4)==0)&&((uiYear%100)!=0))||((uiYear%400)==0);   
}