/**
  ******************************************************************************
  * @file    lcd12864_menu.h
  * @author  xiaoyuan
  * @version V1.0
  * @date    2016-04-12
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD12864_MENU_H
#define __LCD12864_MENU_H	

#include "stm32f10x.h"

#include "exfuns.h"
#include "string.h"	
#include "led.h"

#define MENU_SIZE 6

//void (*Operation_Function)(void);

//MenuTypeDef fun_index;

//宏定义菜单的ID号
#define STATUS_MENU_ID 10
#define CARD_MENU_ID 11


typedef struct menu//定义一个菜单
{
	uint8_t ID;
	u8 range_from,range_to; //当前显示的项开始及结束序号
	u8 itemCount;//项目总数
	u8 selected;//当前选择项
	u8 title[16]; //菜单标题
	u8 *menuItems[30];//菜单项目 //取所有菜单条目数目的最大值
	struct menu **subMenus;//子菜单
	struct menu *parent;//上级菜单 ,如果是顶级则为null
	void (**func)();//选择相应项按确定键后执行的函数
  void (*displayUpdate_f)(); //用于更新菜单显示（包括按键操作的更新和自动的更新）	
}MenuTypeDef;

extern MenuTypeDef *CurrentMenu;
extern MenuTypeDef StatusMenu;
extern MenuTypeDef PrintingFinishedMenu;
extern MenuTypeDef YesOrNoMenu;

extern u16 totgconum; 		//Gcode文件总数
extern uint8_t **zzz;
extern char consumingTime[30];
extern char printingFilename[30];
extern uint8_t lcdDisplayUpdate;

void welcome_screen(void);
void lcd_productInfo(void);

void lcd_cardPrinting(void);
void lcd_printingPauseOrContinue(void);
void lcd_stopPrinting(void);
void lcd_cardInsertOrNot(void);
void lcd_noCard(void);

void lcd_menuInit(void);
void lcd_update(void);

void lcd_poweroff_recoverPrintingOrNot(void);
void lcd_poweroff_stopPrinting(void);
void lcd_poweroff_recoverPrinting(void);

void lcd_displayUpdate_mainMenu(void);
void lcd_displayUpdate_cardMenu(void);
void lcd_displayUpdate_statusMenu(void);
void lcd_displayUpdate_printingFinishedMenu(void);
void lcd_displayUpdate_YesOrNotMenu(void);
void lcd_displayUpdate_general(void);
void lcd_displayUpdate_MoveAxisMenu(void);
void lcd_displayUpdate_adjustParameterMenu(void);
void lcd_changePrintingSpeed(void);
void lcd_changePrintingTemp(void);	
void lcd_changePrintingFilament(void);
void lcd_displayUpdate_changeParameterMenu_speed(void);
void lcd_displayUpdate_changeParameterMenu_temp(void);

uint16_t card_getFileNum(uint8_t *path);
void card_readFileListInfo(void);

#endif
