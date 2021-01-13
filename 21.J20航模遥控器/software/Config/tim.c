#include "main.h"
#include "tim.h"

volatile u32 nowTime = 0;//程序运行时间，单位20ms，在key.c中更新
volatile u32 clockTime = 0;//闹钟定时时间，单位20ms，在key.c中更新
extern char timeStr[9];//时间字符串

void drawClockTime(void)
{
    u16 hour = 0, min = 0, sec = 0;
    sec = nowTime / 50; //系统计时，在key.c中的TIM3中断服务函数中更新
    hour = sec / 3600;
    min = (sec % 3600) / 60;
    sec = sec % 60;
    OLED_Fill(85, 1, 127, 12, 1); //冒号
    OLED_DrawPoint(99, 5, 0);
    OLED_DrawPoint(99, 8, 0);
    OLED_DrawPoint(114, 5, 0);
    OLED_DrawPoint(114, 8, 0);
    sprintf((char *)timeStr, "%02d", hour); //组合时间字符串
    OLED_ShowString(86, 1, (u8*)timeStr, 12, 0);
    sprintf((char *)timeStr, "%02d", min); //组合时间字符串
    OLED_ShowString(101, 1, (u8*)timeStr, 12, 0);
    sprintf((char *)timeStr, "%02d", sec); //组合时间字符串
    OLED_ShowString(116, 1, (u8*)timeStr, 12, 0);
}

