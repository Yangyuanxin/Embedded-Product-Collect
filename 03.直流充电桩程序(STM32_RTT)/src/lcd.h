#ifndef LCD_H_
#define LCD_H_

#include "stm32f10x.h"

typedef struct PAGE
{
	u8 (*poll_handler)(u32 poll_cnt);  //页面轮询处理函数  用于更新显示数据   NULL不刷新   若要切换页面 返回页面ID即可  pool_cnt轮轮次数 从切换到当前页面开始计数
	void (*vchange_handler)(u8 offset, u32 data);  //页面变量数据变化处理函数  比如当用户输入数据时自动发送数据上来  这进调用此函数处理
	u8 (*button_handler)(u8 offset);
	void (*page_init)();   //切换页面之前调用  用于初始化页面
	void* wo;              //只写变量
	void* ro;              //只读变量
	u16 poll_time;         //轮询时间间隔   单位ms
	u8  wo_len;		         //只写变量长度
	u8  ro_len;		         //只读变量长度
	u8 prev_id;            //前一级页面  用于从当前页面返回前一级
}page_t;

//DGUS时间
//HEX表示  比如2015年 则  YY == 0x15
typedef struct
{
	u8 YY;  //年 2000 + YY
	u8 MM;  //月
	u8 DD;  //日
	u8 WW;  //星期
	u8 h;   //时
	u8 m;   //分
	u8 s;   //秒
}dgus_time_t;


void task_lcd(void);

//无效页面ID
#define PAGE_ID_INVALID 0xff

#define PAGE_PREV 0xfe

#endif /* LCD_H_ */
