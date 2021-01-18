#ifndef __MENU_H
#define __MENU_H			  	 
#include "sys.h"
#include "stdlib.h"

typedef enum// 菜单索引
{
	home,//主页面：HOME
	xmsj,//一级菜单：休眠时间
	gjsj,//一级菜单：关机时间
	gzms,//一级菜单：工作模式
	fmq,//一级菜单：蜂鸣器
	xmsjSet,//设置：休眠时间
	gjsjSet,//设置：关机时间
	gzmsSet,//设置：工作模式
	fmqSet,//设置：蜂鸣器
	yysz,//一级菜单：语言设置
	hfmr,//一级菜单：恢复默认设置
	gybj,//一级菜单：关于本机
	tc,//一级菜单：退出
	yyszSet,//设置：语言设置
	hfmrTip,//设置：恢复默认设置
	gybjTip,//设置：关于本机
	
	MENU_NUM, // 菜单页总数
}MENU_INDEX_LIST;

typedef struct{
	u8 current_index;	//存放当前界面的索引号；
	u8 down_index;		//存放按下“down（向下）”键时需要跳转到的索引号；
	u8 up_index;		//存放按下“up（向上）”键时需要跳转到的索引号；
	u8 enter_index;		//存放按下“enter（进入）”键时需要跳转的索引号；
	u8 esc_index;		//存放按下“esc（退出）”键时需要跳转的索引号；
	u8 home_index;		//存放按下“home（主界面）”键时需要跳转的索引号；
	void (*operate)();	//函数指针变量，存放当前索引号下需要执行的函数的入口地址。
}Key_index_struct;


extern volatile u8 nowMenuIndex;
extern volatile u8 lastMenuIndex;
extern volatile u8 sleepFlag;
extern volatile u8 shutFlag;

void OLED_display(void);
void homeWindow(void);//主界面
void menu_xmsj(void);//一级菜单：休眠时间
void menu_gjsj(void);//一级菜单：关机时间
void menu_gzms(void);//一级菜单：工作模式
void menu_fmq(void);//一级菜单：蜂鸣器
void menu_yysz(void);//一级菜单：语言设置
void menu_hfmr(void);//一级菜单：恢复默认设置
void menu_gybj(void);//一级菜单：关于本机
void menu_tc(void);//一级菜单：退出

void menu_xmsjSet(void);//设置：休眠时间
void menu_gjsjSet(void);//设置：关机时间
void menu_gzmsSet(void);//设置：工作模式
void menu_fmqSet(void);//设置：蜂鸣器
void menu_yyszSet(void);//设置：语言设置
void menu_hfmrTip(void);//设置：恢复默认设置
void menu_gybjTip(void);//设置：关于本机
#endif

