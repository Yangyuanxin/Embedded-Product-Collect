/**
  ******************************************************************************
  * @file    lcd12864_menu.c
  * @author  xiaoyuan
  * @version V1.0
  * @date    2016-04-16
  * @brief   This file provides all the lcd12864_menu functions.
  ******************************************************************************
  * @attention
  * 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "lcd12864_menu.h"
#include "lcd12864.h" 
#include "key.h"
#include "delay.h"
#include "timer.h"
#include "malloc.h"
#include "cardreader.h"
#include "marlin_main.h"
#include "exti.h"
#include "flash.h"

#define NULL 0

//FIL fgcode; //用于测试读取文件内容的文件句柄
//char buf[100];//用于测试读取文件内容的接收数组
//UINT brr; //用于测试读取文件内容需要的变量

uint8_t **zzz; //存储文件名列表，必须用malloc（）函数申请空间才可以使用
u16 totgconum; 		//Gcode文件总数
u16 curindex;		     //Gcode文件当前索引
uint8_t lcdDisplayUpdate = 1; //屏幕显示更新标志位
static uint32_t nextUpdateMillis = 0; //用来记录下次更新屏幕的时间
char temperature0[30],temperatureBed[30]; 
char percentDone[30];
char consumingTime[30];
char printingFilename[30];
char cardFileList[100][30];

uint16_t temp_feedMultiply = 100; //进给速度的倍率
uint16_t temp_e0TargetTemp;
uint8_t differenceValue = 5;

static float manual_feedrate[4] = {50*60,50*60,4*60,60};

MenuTypeDef * CurrentMenu; //指向当前菜单的结构体指针


/**********************一级菜单*********************/
MenuTypeDef MainMenu = 
{
 0,0,2,3,0,
	"   3D Printer   ",
	{
		"Prepare         ",
		"Control         ",
		"Card Menu       "
	}
};

/**********************二级菜单*********************/
MenuTypeDef PrepareMenu = 
{
	0,0,2,7,0,
	"    Prepare     ",
	{

		"Printer Status→", //0
		"Preheat PLA     ", //1
		"Preheat ABS     ", //2
		"AutoHome        ", //3
		"Disable Stepper ", //4
		"Move Axis     →", //5
		"Back MainMenu ↑", //6
	}
};

MenuTypeDef ControlMenu = 
{
	0,0,2,6,0,
	"    Control     ",
	{
		"Temperature   →", //0
		"Motion        →", //1
		"Store Memory    ", //2
		"Load Memory     ", //3
		"Restore Fsafe   ", //4
		"Back MainMenu ↑", //5
	}
};

MenuTypeDef CardMenu = 
{
	CARD_MENU_ID,0,2,100,0,
	" Card File List ",

};
/**********************三级菜单*********************/
//MenuTypeDef MoveDistanceMenu = 
//{
//	0,0,3,4,0,
//	"MoveDistanceMenu",
//	{
//		"goMoveAxisMenu",
//		"Move  0.1mm   ",
//		"Move  1  mm   ",
//		"Move 10  mm   ",
//	}
//};

MenuTypeDef TemperatureMenu = 
{
	0,0,2,14,0,
	"  Temperature   ",
	{
		"ControlMenu   ",
		"Nozzle        ",
		"Autotemp      ",
		"Min           ",
		"Max           ",
		"Fact          ",
		"Bed           ",
		"Fan Speed     ",
		"PID-P         ",
		"PID-I         ",
		"PID-D         ",
		"PID-C         ",
		"PreheatPLA Set",
		"PreheatABS Set"
	}
};

MenuTypeDef MotionMenu = 
{
	0,0,2,17,0,
	"MotionMenu",
	{
		"ControlMenu   ",
		"Acc:          ",
		"VXY-Jerk      ",
		"Vmax-X        ",
		"Vmax-Y        ",
		"Vmax-Z        ",
		"Vmax-E        ",
		"Vtrav-min     ",
		"Amax-X        ",
		"Amax-Y        ",
		"Amax-Z        ",
		"Amax-E        ",
		"A-retract     ",
		"Xsteps/mm     ",
		"Ysteps/mm     ",
		"Zsteps/mm     ",
		"Esteps/mm     "
	}
};
/**********************四级菜单*********************/
MenuTypeDef MoveAxisMenu = 
{
	0,0,2,5,0,
	"   Move Axis   ",
	{
		"←uint→ :  10mm",
		" X- ←Back→ X+ ",
		" Y- ←Back→ Y+ ",
		" Z- ←Back→ Z+ ",
		"E0- ←Back→ E0+",
	}
};

//在打印时的菜单
/**********************状态菜单*********************/
MenuTypeDef StatusMenu = 
{
	STATUS_MENU_ID,0,3,4,0,
	"StatusMenu",
	{
    "              ",
	 	"              ",
	  "              ",
	  "              ",
	}
};
/**********************打印选项菜单*********************/
MenuTypeDef PrintingOptionsMenu = 
{
	0,0,2,7,0,
	"Printing Options",
	{
		"Back Last Menu",       
		"Pause         ",
		"Stop Printing ",
		"Adjust Param  ",
		"ChangeFilament",
		"Change Speed  ",
		"Change Temp   ",
		
		
	}
};

MenuTypeDef ChangeParameterMenu = 
{
	0,0,3,4,0,
	" ",
	{
		"                ",
		"                ",
		"                ",
		"                ",
	}
};

MenuTypeDef AdjustParameterMenu = 
{
	0,0,2,4,0,
	" AdjustParameter",
	{
		"              ",
		"              ",
		"              ",
		"              ",
	}
};

MenuTypeDef PrintingFinishedMenu = 
{
	2,0,3,4,0,
	" ",
	{
	  "                ",
		"***Print Done***",
		"                ",
	  "any key back... ",
	}
};
//确认与取消菜单，类似于这种选择形式都可以使用该菜单，
//目前使用该菜单的部分：打印停止的选择，断电续打的选择
MenuTypeDef YesOrNoMenu = 
{
	0,0,3,4,2,
	"",
	{
	  "YES OR NO ?   ",
	  "              ",
	  "YES         ",
	  "NO          ",
	}
};

//显示欢迎界面
void welcome_screen(void)
{
	LCD12864_Clear();
	LCD12864_ShowString(0,0,"***************");
	LCD12864_ShowString(1,0," WECOME TO USE  ");
	LCD12864_ShowString(2,0,"STM32 3D Printer");
	LCD12864_ShowString(3,0,"***************");
	//BEEP_Ring(2500); //显示5s
	delay_ms(5000); //显示5s
//	LCD12864_Clear(); //清屏
}

//显示产品信息
void lcd_productInfo(void)
{
	LCD12864_Clear();
	LCD12864_ShowString(0,0,"OTouch->TopWill ");
	LCD12864_ShowString(1,0," WECOME TO USE  ");
	LCD12864_ShowString(2,0,"STM32 3D Printer");
	LCD12864_ShowString(3,0,"author: wangyuan");
	while(!keyPressed); //等待按键按下
}

//lcd屏幕更新函数
void lcd_update(void)
{
	lcd_cardInsertOrNot(); //检测SD卡的插入与否
	CurrentMenu->displayUpdate_f(); //处理按键操作下的屏幕更新
	
	if(nextUpdateMillis < millis()) //屏幕超时更新
	{
//		FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+114,card.sdpos);
//		FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+118,current_position[Z_AXIS]);
//		FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+122,printingFilename);
		if(STATUS_MENU_ID == CurrentMenu->ID || CARD_MENU_ID == CurrentMenu->ID) //状态需要依据时间刷新的菜单
		{
		  lcdDisplayUpdate = 1;
			CurrentMenu->displayUpdate_f();
		}
		//card_readFileListInfo();//如果不加上该函数只在初始化中执行一次，SD卡中的文件后面几个会显示乱码，原因尚不清楚。现在该函数加上也可以正常连接串口了，不清楚是否有细微异常。
	  nextUpdateMillis = millis() + 800; //刷新周期（每100ms刷新一次）
	}
}

void lcd_backMainMenu(void)
{
	//退出菜单时，将当前菜单的选中项清零,并将显示范围调整到初始化状态
	CurrentMenu->selected = 0; 
	CurrentMenu->range_from = 0; 
	CurrentMenu->range_to = 2;
	CurrentMenu = &MainMenu;
	lcdDisplayUpdate = 1;
	CurrentMenu->displayUpdate_f();
}
 
void lcd_autoHome(void)
{
	enquecommand("G28");
}
void lcd_disableStepper(void)
{
	enquecommand("M84");
}

void lcd_configResetDefault(void) //恢复出厂设置
{
	Config_ResetDefault();
}


//2017/3/10 修复了每次检测都会重新读取卡信息致使连接不上串口的bug
//检测SD卡是插入还是拔出状态
void lcd_cardInsertOrNot()
{
	uint8_t rereadsdfileinfoflag; //重读SD卡文件信息标志位
	//检测SD卡，以更新Card Menu 菜单条目的内容
	if(SD_CD) //未插入SD卡
	{
		rereadsdfileinfoflag = 1; //重读SD卡文件信息标志位置位
		MainMenu.subMenus[2] = NULL;	
		MainMenu.func[2] = &lcd_noCard;
    	//lcdDisplayUpdate = 1;
		if(CurrentMenu == &CardMenu) //在CardMenu菜单下拔出SD卡返回主菜单
		{
			CurrentMenu = &MainMenu;
			lcdDisplayUpdate = 1;
		}
	}
	if(rereadsdfileinfoflag == 1) //从无卡状态下，插入SD卡
	{
		if(!SD_CD)
		{
            //exfuns_init();
			//f_mount(fs[0],"0:",1); 	//挂载SD卡     		
			MainMenu.subMenus[2] = &CardMenu;
			MainMenu.func[2] = NULL;
		  card_readFileListInfo();
		}
		rereadsdfileinfoflag = 0; //清零
	}
}

//SD卡未插入
void lcd_noCard(void)
{
	LCD12864_Clear();
  LCD12864_ShowString(0,1,"  No SD Card");
	while(SD_CD)
	{
		if(keyPressed) //有按键按下
		{
			CurrentMenu = &MainMenu; //返回主菜单
			return; //跳出当前函数
		}
	}
	//由无卡状态下插入了SD卡，恢复显示SD卡文件列表
	card_readFileListInfo();  
	CurrentMenu = &CardMenu; //插入了SD卡
}

//开启SD卡文件打印进程
void lcd_cardPrinting()
{
	uint8_t *fnn;
//	fnn = (u8 *)(CardMenu.menuItems[CardMenu.selected]); //读取当前打印文件的文件名
	fnn = (u8 *)(cardFileList[CardMenu.selected]); //读取当前打印文件的文件名
	strcpy((char *) printingFilename,(const char*)fnn);  //先将文件名存储到字符串数组中，若不这样在状态界面显示文件名时会存在bug（存储在StatusMenu.menuItems[0]中的文件名在处于加热状态时会变成后面隔一个文件之后的文件的文件名），尚不清楚原因。
	StatusMenu.menuItems[0] = (uint8_t *)printingFilename; //将文件名存储到状态菜单条目中，用于显示当前打印文件名	
	//执行打印，有待于进一步理解与改进
				card_initsd();
				card_openFile((char *)fnn,true);
				card_startFileprint();
				starttime=millis();
	
//  CurrentMenu = &PrintingOptionsMenu;	
	CurrentMenu = &StatusMenu;
	lcdDisplayUpdate = 1; //切换菜单后要将显示更新标志位置位，用于使当前菜单更新到切换的菜单
	CurrentMenu->displayUpdate_f();
}

//暂停打印进程还是继续打印进程
void lcd_printingPauseOrContinue()
{
	if(card.sdprinting == true)
	{
		PrintingOptionsMenu.menuItems[1] = "Continue      ", 
		card.sdprinting = false;
	}
	else
	{
		PrintingOptionsMenu.menuItems[1] = "Pause         ",
		card.sdprinting = true;
		//TIM_ITConfig(TIM3,TIM_IT_Update, ENABLE); //更新TIM3中断
	} 
	CurrentMenu = &PrintingOptionsMenu;
	lcdDisplayUpdate = 1; //切换菜单后要将显示更新标志位置位，用于使当前菜单更新到切换的菜单
	if(card_eof()) //读取到了文件末尾
	{
		card_printingHasFinished();
		CurrentMenu = &MainMenu;
		lcdDisplayUpdate = 1; //切换菜单后要将显示更新标志位置位，用于使当前菜单更新到切换的菜单
  }
}

//是否终止打印进程
void lcd_stopPrintingOrNot(void)
{
	YesOrNoMenu.menuItems[0] = "Stop Printing?";
	YesOrNoMenu.menuItems[1] = "              ";
	YesOrNoMenu.menuItems[2] = "YES           ";
	YesOrNoMenu.menuItems[3] = "NO            ";
	YesOrNoMenu.func[2] = &lcd_stopPrinting;
	YesOrNoMenu.subMenus[3] = &PrintingOptionsMenu; //2017.3.14选择NO直接将子菜单指向到父菜单，不知这样可不可行，晚上测试一下，如果可行就可以省略掉选择NO的功能函数了 
    CurrentMenu = &YesOrNoMenu;
	lcdDisplayUpdate = 1; //切换菜单后要将显示更新标志位置位，用于使当前菜单更新到切换的菜单
	CurrentMenu->displayUpdate_f();
}

//终止打印进程
void lcd_stopPrinting()
{
	quickStop(); 
	card_closefile();
	starttime=0;
	card.sdprinting = false;

	autotempShutdown();
	setTargetHotend(0,active_extruder);
	//heater_0_temp = 0;
	//bed_temp = 0;

	disable_x(); 
	disable_y(); 
	disable_z(); 
	disable_e0(); 
	disable_e1(); 
	enquecommand("M84") ;
	
	CurrentMenu = &MainMenu;
	lcdDisplayUpdate = 1; //切换菜单后要将显示更新标志位置位，用于使当前菜单更新到切换的菜单
	CurrentMenu->displayUpdate_f();
}

//改变打印速度
void lcd_changePrintingSpeed(void)
{
	char tempStr[16];
	ChangeParameterMenu.menuItems[0] = "  Change Speed  ";
	sprintf(tempStr,"Speed:%d%%",temp_feedMultiply );
	ChangeParameterMenu.menuItems[1] = (uint8_t *)tempStr;
	ChangeParameterMenu.menuItems[2] = "                ";
	ChangeParameterMenu.menuItems[3] = "by up/down key  ";
	ChangeParameterMenu.displayUpdate_f = &lcd_displayUpdate_changeParameterMenu_speed;
	CurrentMenu = &ChangeParameterMenu;
	lcdDisplayUpdate = 1;
	CurrentMenu->displayUpdate_f();
}
//改变打印温度
void lcd_changePrintingTemp(void)
{
	char tempStr[16];
	ChangeParameterMenu.menuItems[0] = "  Change Temp  ";
	sprintf(tempStr,"Temp : %ddeg",temp_e0TargetTemp );
	ChangeParameterMenu.menuItems[1] = (uint8_t *)tempStr;
	ChangeParameterMenu.menuItems[2] = "                ";
	ChangeParameterMenu.menuItems[3] = "by up/down key  ";
	ChangeParameterMenu.displayUpdate_f = &lcd_displayUpdate_changeParameterMenu_temp;
	CurrentMenu = &ChangeParameterMenu;
	lcdDisplayUpdate = 1;
	CurrentMenu->displayUpdate_f();
}
////调整参数
//void lcd_changeParam(void)
//{
//	char temp_diff[16];
//	char temp_speed[16];
//	char temp_temp[16];
//	char temp_fanSpeed[16];
//	sprintf(temp_diff,"Diff  :  %d%%",feedMultiply );
//	ChangeParamMenu.menuItems[0] = (uint8_t *)temp_diff;
//	sprintf(temp_speed,"Speed  :  %d%%",feedMultiply );
//	ChangeParamMenu.menuItems[1] = (uint8_t *)temp_speed;
//	sprintf(temp_temp,"Temp_E0  :  %d%%",target_temperature[0] );
//	ChangeParamMenu.menuItems[2] = (uint8_t *)temp_temp;
//	sprintf(temp_fanSpeed,"FanSpeed  :  %d%%",fanSpeed );
//	ChangeParamMenu.menuItems[3] = (uint8_t *)temp_fanSpeed;
//}

//更换打印耗材
void lcd_changePrintingFilament(void)
{	
	enquecommand("M600"); //更换耗材的G代码 //只是将M600加入了缓冲区，并没有立即执行

}



//断电重新上电后是否恢复先前的打印进程
void lcd_poweroff_recoverPrintingOrNot(void)
{
//	//读取存储的参数
	FLASH_READ_VAR(FLASH_SET_STORE_OFFSET+114,poweroff_sdpos); 
	FLASH_READ_VAR(FLASH_SET_STORE_OFFSET+118,poweroff_position_z);
	FLASH_READ_VAR(FLASH_SET_STORE_OFFSET+122,printingFilename);
//	AT24CXX_Read(60,(u8 *)&poweroff_sdpos,8);
//		AT24CXX_Read(68,(u8 *)&poweroff_position_z,8);
//		AT24CXX_Read(76,(u8 *)&printingFilename,8);
	if(poweroff_sdpos != 0 && poweroff_position_z != 0)
	{
		LCD12864_Clear();
		YesOrNoMenu.menuItems[0] = "Recover Print?  ";
		YesOrNoMenu.menuItems[1] = "                ";
		YesOrNoMenu.menuItems[2] = "YES           ";
		YesOrNoMenu.menuItems[3] = "NO            ";
		YesOrNoMenu.func[2] = &lcd_poweroff_recoverPrinting;
		YesOrNoMenu.func[3] = &lcd_poweroff_stopPrinting; 

		CurrentMenu = &YesOrNoMenu;
		lcdDisplayUpdate = 1; //切换菜单后要将显示更新标志位置位，用于使当前菜单更新到切换的菜单
		CurrentMenu->displayUpdate_f();
	}
}
//恢复断电之前的打印进程
void lcd_poweroff_recoverPrinting(void) //断电续打
{
	lcd_contiune_print_after_poweroff = 1;
	card_initsd();
	card_openFile((char *)printingFilename,true);
	card_startFileprint();
	StatusMenu.menuItems[0] = (uint8_t *)printingFilename; //将文件名存储到状态菜单条目中，用于显示当前打印文件名	
	CurrentMenu = &StatusMenu;
	lcdDisplayUpdate = 1; //切换菜单后要将显示更新标志位置位，用于使当前菜单更新到切换的菜单
}
//终止断电前的打印进程
void lcd_poweroff_stopPrinting(void) //不再续打
{
	uint8_t i;
	//将存储的断电续打参数清零（暂且将变量清零在写入，若找到更好的方法再替换）
	poweroff_sdpos = 0;
	poweroff_position_z = 0;
	for(i=0;i<30;i++)
	{
		poweroff_printing_filename[i] = 0; 
	}
	FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+114,poweroff_sdpos); 
	FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+118,poweroff_position_z);
	FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+122,poweroff_printing_filename);
	CurrentMenu = &MainMenu;
	lcdDisplayUpdate = 1; //切换菜单后要将显示更新标志位置位，用于使当前菜单更新到切换的菜单
}

//屏幕菜单初始化
void lcd_menuInit()
{
	uint8_t i;
	
	welcome_screen(); //初始化欢迎界面
	
	/*读取SD卡文件数目和文件名列表*/
	card_readFileListInfo(); //必须先执行否则CardMenu菜单初始化会有问题
	/**********************一级菜单*********************/
	MainMenu.subMenus = mymalloc(sizeof(&MainMenu)*3);
	MainMenu.subMenus[0] = &PrepareMenu;
	MainMenu.subMenus[1] = &ControlMenu;
	MainMenu.subMenus[2] = &CardMenu;
	MainMenu.parent = NULL;
	MainMenu.func = mymalloc(sizeof(NULL)*3);
	MainMenu.func[0] = NULL;//&lcd_productInfo;
	MainMenu.func[1] = NULL;
	MainMenu.func[2] = NULL;
	MainMenu.displayUpdate_f = &lcd_displayUpdate_general;
	/**********************二级菜单*********************/
	PrepareMenu.subMenus = mymalloc(sizeof(&PrepareMenu)*7);
	PrepareMenu.subMenus[0] = 
	PrepareMenu.subMenus[1] =
	PrepareMenu.subMenus[2] =
	PrepareMenu.subMenus[3] =
	PrepareMenu.subMenus[4] = NULL;
	PrepareMenu.subMenus[5] = &MoveAxisMenu;
	PrepareMenu.subMenus[6] = NULL;//&MainMenu; //用于返回主菜单
	PrepareMenu.parent = &MainMenu;
	PrepareMenu.func = mymalloc(sizeof(&PrepareMenu)*7);
	PrepareMenu.func[0] = 
	PrepareMenu.func[1] = 
	PrepareMenu.func[2] = NULL;
	PrepareMenu.func[3] = &lcd_autoHome;
	PrepareMenu.func[4] = &lcd_disableStepper;
	PrepareMenu.func[5] = NULL;
	PrepareMenu.func[6] = &lcd_backMainMenu;
	//PrepareMenu.displayUpdate_f = mymalloc(sizeof(&PrepareMenu));
	PrepareMenu.displayUpdate_f = &lcd_displayUpdate_general;
	
	ControlMenu.subMenus = mymalloc(sizeof(&ControlMenu)*6);
	ControlMenu.subMenus[0] = NULL;
	ControlMenu.subMenus[1] = &TemperatureMenu;
	ControlMenu.subMenus[2] = &MotionMenu;
	ControlMenu.subMenus[3] =
	ControlMenu.subMenus[4] =
	ControlMenu.subMenus[5] = NULL;
	ControlMenu.parent = &MainMenu;
	ControlMenu.func = mymalloc(sizeof(&ControlMenu)*6);
	ControlMenu.func[0] = 
	ControlMenu.func[1] =
	ControlMenu.func[2] =
	ControlMenu.func[3] = NULL;
	ControlMenu.func[4] = &lcd_configResetDefault;
	ControlMenu.func[5] = &lcd_backMainMenu;//
	//ControlMenu.displayUpdate_f = mymalloc(sizeof(&ControlMenu));
	ControlMenu.displayUpdate_f = &lcd_displayUpdate_general;
	
	CardMenu.subMenus = mymalloc(sizeof(&CardMenu)*totgconum);
	for(i=0;i<totgconum;i++)
	{
		CardMenu.subMenus[i] = NULL;
	}
	CardMenu.parent = &MainMenu;
	CardMenu.func = mymalloc(sizeof(&CardMenu)*totgconum);
	for(i=0;i<totgconum;i++)
	{
		CardMenu.func[i] = &lcd_cardPrinting;
	}
	//CardMenu.displayUpdate_f = (void (*)())mymalloc(sizeof(&CardMenu));
	CardMenu.displayUpdate_f = &lcd_displayUpdate_cardMenu;
	/**********************三级菜单*********************/
//	MoveDistanceMenu.subMenus = mymalloc(sizeof(&MoveDistanceMenu)*4);
//	MoveDistanceMenu.subMenus[0] = NULL;
//	MoveDistanceMenu.subMenus[1] = &MoveAxisMenu;
//	MoveDistanceMenu.subMenus[2] = &MoveAxisMenu;
//	MoveDistanceMenu.subMenus[3] = &MoveAxisMenu;//NULL;
//	MoveDistanceMenu.parent = &PrepareMenu;
//	MoveDistanceMenu.func = mymalloc(sizeof(NULL)*4);
//	MoveDistanceMenu.func[0] = NULL;
//	MoveDistanceMenu.func[1] = NULL;
//	MoveDistanceMenu.func[2] = NULL;
//	MoveDistanceMenu.func[3] = NULL;
//	MoveDistanceMenu.displayUpdate_f = &lcd_displayUpdate_general;
	
	TemperatureMenu.subMenus = mymalloc(sizeof(&TemperatureMenu)*9);
	TemperatureMenu.subMenus[0] = 
	TemperatureMenu.subMenus[1] =
	TemperatureMenu.subMenus[2] =
	TemperatureMenu.subMenus[3] =
	TemperatureMenu.subMenus[4] =
	TemperatureMenu.subMenus[5] =
	TemperatureMenu.subMenus[6] =
	TemperatureMenu.subMenus[7] =
	TemperatureMenu.subMenus[8] = 
	TemperatureMenu.subMenus[9] =
	TemperatureMenu.subMenus[10] =
	TemperatureMenu.subMenus[11] =
	TemperatureMenu.subMenus[12] =
	TemperatureMenu.subMenus[13] = NULL;
	TemperatureMenu.parent = &ControlMenu;
	TemperatureMenu.func = mymalloc(sizeof(&TemperatureMenu)*9);
	TemperatureMenu.func[0] = 
	TemperatureMenu.func[1] =
	TemperatureMenu.func[2] =
	TemperatureMenu.func[3] =
	TemperatureMenu.func[4] =
	TemperatureMenu.func[5] =
	TemperatureMenu.func[6] =
	TemperatureMenu.func[7] =
	TemperatureMenu.func[8] = 
	TemperatureMenu.func[9] =
	TemperatureMenu.func[10] =
	TemperatureMenu.func[11] =
	TemperatureMenu.func[12] =
	TemperatureMenu.func[13] = NULL;
	TemperatureMenu.displayUpdate_f = &lcd_displayUpdate_general;
	
	MotionMenu.subMenus = mymalloc(sizeof(&MotionMenu)*9);
	MotionMenu.subMenus[0] = 
	MotionMenu.subMenus[1] =
	MotionMenu.subMenus[2] =
	MotionMenu.subMenus[3] =
	MotionMenu.subMenus[4] =
	MotionMenu.subMenus[5] =
	MotionMenu.subMenus[6] =
	MotionMenu.subMenus[7] =
	MotionMenu.subMenus[8] = 
	MotionMenu.subMenus[9] =
	MotionMenu.subMenus[10] =
	MotionMenu.subMenus[11] =
	MotionMenu.subMenus[12] =
	MotionMenu.subMenus[13] = 
	MotionMenu.subMenus[14] =
	MotionMenu.subMenus[15] =
	MotionMenu.subMenus[16] = NULL;
	MotionMenu.parent = &ControlMenu;
	MotionMenu.func = mymalloc(sizeof(&MotionMenu)*9);
	MotionMenu.func[0] = 
	MotionMenu.func[1] =
	MotionMenu.func[2] =
	MotionMenu.func[3] =
	MotionMenu.func[4] =
	MotionMenu.func[5] =
	MotionMenu.func[6] =
	MotionMenu.func[7] =
	MotionMenu.func[8] = 
	MotionMenu.func[9] =
	MotionMenu.func[10] =
	MotionMenu.func[11] =
	MotionMenu.func[12] =
	MotionMenu.func[13] = 
	MotionMenu.func[14] =
	MotionMenu.func[15] =
	MotionMenu.func[16] = NULL;
	MotionMenu.displayUpdate_f = &lcd_displayUpdate_general;
	/**********************四级菜单*********************/
	MoveAxisMenu.subMenus = mymalloc(sizeof(&MoveAxisMenu)*6);
	MoveAxisMenu.subMenus[0] = 
	MoveAxisMenu.subMenus[1] = 
	MoveAxisMenu.subMenus[2] = 
	MoveAxisMenu.subMenus[3] =
	MoveAxisMenu.subMenus[4] = 
	MoveAxisMenu.subMenus[5] = NULL;
	MoveAxisMenu.parent = &PrepareMenu;//&MoveDistanceMenu;
	MoveAxisMenu.func = mymalloc(sizeof(&MoveAxisMenu)*6);
	MoveAxisMenu.func[0] = 
	MoveAxisMenu.func[1] =
	MoveAxisMenu.func[2] =
	MoveAxisMenu.func[3] =
	MoveAxisMenu.func[4] =
	MoveAxisMenu.func[5] = NULL;
	//MoveAxisMenu.displayUpdate_f = mymalloc(sizeof(&MoveAxisMenu));
	MoveAxisMenu.displayUpdate_f = &lcd_displayUpdate_MoveAxisMenu;

//打印中的屏幕菜单
/**********************一级菜单*********************/
	StatusMenu.subMenus = mymalloc(sizeof(&StatusMenu)*4);
	StatusMenu.subMenus[0] = &PrintingOptionsMenu;
	StatusMenu.subMenus[1] = &PrintingOptionsMenu;
	StatusMenu.subMenus[2] = &PrintingOptionsMenu;
	StatusMenu.subMenus[3] = &PrintingOptionsMenu;
	StatusMenu.parent = NULL;
	StatusMenu.func = mymalloc(sizeof(NULL)*4);
	StatusMenu.func[0] = NULL;
	StatusMenu.func[1] = NULL;
	StatusMenu.func[2] = NULL;
	StatusMenu.func[3] = NULL;//&Card_Menu;
	StatusMenu.displayUpdate_f = &lcd_displayUpdate_statusMenu;
	
	YesOrNoMenu.subMenus = mymalloc(sizeof(&YesOrNoMenu)*4);
	YesOrNoMenu.subMenus[0] = NULL;
	YesOrNoMenu.subMenus[1] = NULL;
	YesOrNoMenu.subMenus[2] = NULL;
	YesOrNoMenu.subMenus[3] = NULL;
	YesOrNoMenu.parent = &StatusMenu;
	YesOrNoMenu.func = mymalloc(sizeof(NULL)*4);
	YesOrNoMenu.func[0] = NULL;
	YesOrNoMenu.func[1] = NULL;
	YesOrNoMenu.func[2] = NULL;//lcd_stopPrinting;
	YesOrNoMenu.func[3] = NULL;//&Card_Menu;
	YesOrNoMenu.displayUpdate_f = &lcd_displayUpdate_YesOrNotMenu;
	
	PrintingOptionsMenu.subMenus = mymalloc(sizeof(&PrintingOptionsMenu)*4);
	PrintingOptionsMenu.subMenus[0] = &StatusMenu;
	PrintingOptionsMenu.subMenus[1] = NULL;
	PrintingOptionsMenu.subMenus[2] = NULL;
	PrintingOptionsMenu.subMenus[3] = &AdjustParameterMenu;
	PrintingOptionsMenu.subMenus[4] = NULL;
	PrintingOptionsMenu.subMenus[5] = NULL;
	PrintingOptionsMenu.subMenus[6] = NULL;
	PrintingOptionsMenu.parent = &StatusMenu;
	PrintingOptionsMenu.func = mymalloc(sizeof(NULL)*4);
	PrintingOptionsMenu.func[0] = NULL;
	PrintingOptionsMenu.func[1] = &lcd_printingPauseOrContinue;
	PrintingOptionsMenu.func[2] = &lcd_stopPrintingOrNot;//lcd_stopPrinting;
	PrintingOptionsMenu.func[3] = NULL;//&Card_Menu;
	PrintingOptionsMenu.func[4] = &lcd_changePrintingFilament;
	PrintingOptionsMenu.func[5] = &lcd_changePrintingSpeed;
	PrintingOptionsMenu.func[6] = &lcd_changePrintingTemp;
	PrintingOptionsMenu.displayUpdate_f = &lcd_displayUpdate_general;
	
	ChangeParameterMenu.subMenus = mymalloc(sizeof(&ChangeParameterMenu)*4);
	ChangeParameterMenu.subMenus[0] = NULL;
	ChangeParameterMenu.subMenus[1] = NULL;
	ChangeParameterMenu.subMenus[2] = NULL;
	ChangeParameterMenu.subMenus[3] = NULL;
	ChangeParameterMenu.parent = &PrintingOptionsMenu;
	ChangeParameterMenu.func = mymalloc(sizeof(NULL)*4);
	ChangeParameterMenu.func[0] = NULL;
	ChangeParameterMenu.func[1] = NULL;
	ChangeParameterMenu.func[2] = NULL;
	ChangeParameterMenu.func[3] = NULL;
	ChangeParameterMenu.displayUpdate_f = NULL;//&lcd_displayUpdate_changePrintingSpeedMenu;	
	
	AdjustParameterMenu.subMenus = mymalloc(sizeof(&AdjustParameterMenu)*4);
	AdjustParameterMenu.subMenus[0] = NULL;
	AdjustParameterMenu.subMenus[1] = NULL;
	AdjustParameterMenu.subMenus[2] = NULL;
	AdjustParameterMenu.subMenus[3] = NULL;
	AdjustParameterMenu.parent = &PrintingOptionsMenu;
	AdjustParameterMenu.func = mymalloc(sizeof(NULL)*4);
	AdjustParameterMenu.func[0] = NULL;
	AdjustParameterMenu.func[1] = NULL;
	AdjustParameterMenu.func[2] = NULL;
	AdjustParameterMenu.func[3] = NULL;
	AdjustParameterMenu.displayUpdate_f = &lcd_displayUpdate_adjustParameterMenu;	
	
	PrintingFinishedMenu.subMenus = mymalloc(sizeof(&PrintingFinishedMenu)*4);
	PrintingFinishedMenu.subMenus[0] = NULL;
	PrintingFinishedMenu.subMenus[1] = NULL;
	PrintingFinishedMenu.subMenus[2] = NULL;
	PrintingFinishedMenu.subMenus[3] = NULL;
	PrintingFinishedMenu.parent = NULL;
	PrintingFinishedMenu.func = mymalloc(sizeof(NULL)*4);
	PrintingFinishedMenu.func[0] = NULL;
	PrintingFinishedMenu.func[1] = NULL;
	PrintingFinishedMenu.func[2] = NULL;
	PrintingFinishedMenu.func[3] = NULL;
	PrintingFinishedMenu.displayUpdate_f = &lcd_displayUpdate_printingFinishedMenu;	
	
	lcd_poweroff_recoverPrintingOrNot(); //检测是否有未完成打印的模型
	//welcome_screen(); //显示5s的欢迎界面
	CurrentMenu = &MainMenu; //将当前菜单指向到主菜单
	lcdDisplayUpdate = 1; //切换菜单后要将显示更新标志位置位，用于使当前菜单更新到切换的菜单
  CurrentMenu->displayUpdate_f();
}

//lcd_displayUpdate_xxx 函数用于更新显示包括根据按键操作更新和根据时间更新，按键操作的更新放在
//在case 1、2、3 。。。等处。按时间更新需要放在default处并加时间判断if(nextUpdateMillis < millis())。
//如果需要更新就将屏幕更新标志位置位，即lcdDisplayUpdate = 1；待后面的显示更新操作处理。


//SD卡菜单下的显示更新（纯按键操作更新无需依据时间更新）
void lcd_displayUpdate_cardMenu(void)
{
	uint8_t i;
	uint8_t m;
	static uint8_t n = 0;
	uint8_t lineSeclected; //当前选中的行
	char seclected_filename[32];
	char temp_string[32];
	//char temp_string2[32];
	//处理按键消息
	switch(keyPressed)  //keyPressed（按下的键值） 由定时中断扫描获得
	{
		case 1://上移
			keyPressed = 0; //按下的键值清零，否则回一直执行该状态
			if(CurrentMenu->selected == 0) //上移到顶级向上翻页（翻到尾页）
			{
				CurrentMenu->selected = CurrentMenu->itemCount-1;
				CurrentMenu->range_to = CurrentMenu->selected;
				CurrentMenu->range_from = CurrentMenu->range_to-2;
				lcdDisplayUpdate = 1; //屏幕显示更新标记置位
				break;
			}
			else //逐行上翻
			{
				CurrentMenu->selected--;
				if(CurrentMenu->selected < CurrentMenu->range_from)
				{
					CurrentMenu->range_from = CurrentMenu->selected;
					CurrentMenu->range_to = CurrentMenu->range_from+2;
				}
				lcdDisplayUpdate = 1; //屏幕显示更新标记置位
				break;
			}
			case 2: //下移
				keyPressed = 0; //按下的键值清零，否则回一直执行该状态
			if(CurrentMenu->selected == CurrentMenu->itemCount-1) //下移到顶向下翻页（翻到首页）
			{	
			  CurrentMenu->selected = 0;
				CurrentMenu->range_from = CurrentMenu->selected;
				CurrentMenu->range_to = CurrentMenu->range_from+2;
				lcdDisplayUpdate = 1; //屏幕显示更新标记置位
				break;
			}
			else
			{
				CurrentMenu->selected++;
				if(CurrentMenu->selected > CurrentMenu->range_to)
				{
					CurrentMenu->range_to = CurrentMenu->selected;
					CurrentMenu->range_from = CurrentMenu->range_to-2;
				}
				lcdDisplayUpdate = 1; //屏幕显示更新标记置位
				break;
			}
			case 3://返回键
			{
				keyPressed = 0; //按下的键值清零，否则回一直执行该状态
				if(CurrentMenu->parent!=NULL)//父菜单不为空，将显示父菜单
			  {
				  //退出菜单时，将当前菜单的选中项清零,并将显示范围调整到初始化状态
			      CurrentMenu->selected = 0; 
				  CurrentMenu->range_from = 0; 
				  CurrentMenu->range_to = 2;
				  
				  CurrentMenu = CurrentMenu->parent;
			      lcdDisplayUpdate = 1; //屏幕显示更新标记置位
				  return;
			  }
			 break;
			}
			case 4: //进入下一级菜单
			{
				keyPressed = 0; //按下的键值清零，否则回一直执行该状态
//				if(CurrentMenu->subMenus[CurrentMenu->selected] != NULL)
//				{
//					CurrentMenu = CurrentMenu->subMenus[CurrentMenu->selected];
//					lcdDisplayUpdate = 1; //屏幕显示更新标记置位
//				}
				break;
			}
			case 5: //确认键
			{
				keyPressed = 0; //按下的键值清零，否则回一直执行该状态
				if(CurrentMenu->subMenus[CurrentMenu->selected] != NULL)
				{
					CurrentMenu = CurrentMenu->subMenus[CurrentMenu->selected];
					lcdDisplayUpdate = 1; //屏幕显示更新标记置位
					CurrentMenu->displayUpdate_f();
				}
				else
				{
					if(CurrentMenu->func[CurrentMenu->selected] != NULL)
					{
						CurrentMenu->func[CurrentMenu->selected]();//执行相应的函数
//						lcdDisplayUpdate = 1; //屏幕显示更新标记置位  //不去掉会以当前形式更新下一级菜单，致使显示bug
					}
				}
				break;
			}
			
			default:
			  break;
	}
	//更新LCD屏幕上显示的条目内容及选中行的标记
	if(1 == lcdDisplayUpdate) //按键更新标记
	{
		lcdDisplayUpdate = 0; //屏幕显示更新标记清零
		
		LCD12864_Clear();
		LCD12864_ShowString(0,0,CurrentMenu->title);
		for(i=1;i<4;i++)
		{
			if(strlen((const char *)CurrentMenu->menuItems[i-1+CurrentMenu->range_from])>14) //超出显示范围显示部分内容
			{
				strncpy((char *)temp_string,(const char *)CurrentMenu->menuItems[i-1+CurrentMenu->range_from],14);
			  LCD12864_ShowString(i,1,(uint8_t *)temp_string);
			}
			else //未超出显示范围全部显示
			{
			  LCD12864_ShowString(i,1,CurrentMenu->menuItems[i-1+CurrentMenu->range_from]);
			}
		}
		lineSeclected = CurrentMenu->selected; //读取当前菜单的选中的条目号
		lineSeclected = 3-(CurrentMenu->range_to - lineSeclected);	//转化位适合屏幕显示的当前选中行
        LCD12864_ShowString(lineSeclected,0,"→");	//为当前选中的行绘制选中标记
		//LCD12864_HightlightShow(0,0,16,1);
		//选中行文件名显示不下滚动显示
		if(strlen((const char *)CurrentMenu->menuItems[CurrentMenu->selected])>14)
		{
			strcpy(seclected_filename,(const char *)CurrentMenu->menuItems[CurrentMenu->selected]);
			for(m=0;m<14;m++)
			{
				temp_string[m] = seclected_filename[m+n];
			}
			n++;
			if(n>(strlen(seclected_filename)-14))
			{
				n = 0;
			}
			LCD12864_ShowString(lineSeclected,1,(uint8_t *)temp_string);
            			
		}	
	}
}

//状态菜单下的显示更新（包括按键操作更新和超时更新）
void lcd_displayUpdate_statusMenu(void)
{
	uint8_t i;
	uint8_t m;
	static uint8_t n = 0;
	char temp_string[32];
//	uint8_t lineSeclected; //当前选中的行
	//处理按键消息
	switch(keyPressed)  //keyPressed（按下的键值） 由外部中断获得
	{
		case 1://上移
		case 2: //下移
		case 3://返回键
		case 4: //进入下一级菜单
			keyPressed = 0; //按下的键值清零，否则回一直执行该状态
			break;
		case 5: //确认键
			keyPressed = 0; //按下的键值清零，否则回一直执行该状态
		    CurrentMenu = &PrintingOptionsMenu;
		    lcdDisplayUpdate = 1; //切换菜单后要将显示更新标志位置位，用于使当前菜单更新到切换的菜单
		    CurrentMenu->displayUpdate_f();
			break;			
		default:
			if(strlen(printingFilename) > 16) //实现单行显示字符超过16个滚动显示
			{
				for(m=0;m<16;m++)
				{
					temp_string[m] = printingFilename[m+n];
				}
				n++;
				if(n>(strlen(printingFilename)-16))
				{
					n = 0;
				}
				StatusMenu.menuItems[0] = (uint8_t *)temp_string;
			}
			sprintf(percentDone,"percentDone:%d%%",card_percentDone());
			//sprintf(percentDone," %d ",f_tell(&card.fgcode)); //测试，文件读取到的位置。
			StatusMenu.menuItems[1] = (uint8_t *)percentDone;
			sprintf(temperature0," T0   :  %d/%d",(int)degHotend(0),(int)degTargetHotend(0));
			StatusMenu.menuItems[2] = (uint8_t *)temperature0;
			sprintf(temperatureBed," BED  :  %d/%d",(int)degBed(),(int)degTargetBed());
			StatusMenu.menuItems[3] = (uint8_t *)temperatureBed;
			break;
	}
	//更新LCD屏幕上显示的条目内容及选中行的标记
	if(1 == lcdDisplayUpdate) //按键更新标记
	{
		lcdDisplayUpdate = 0; //屏幕显示更新标记清零
		LCD12864_Clear();
		for(i=0;i<4;i++)
		{
			LCD12864_ShowString(i,0,CurrentMenu->menuItems[i+CurrentMenu->range_from]);
		}
	}
}
//打印完成后菜单的显示更新（纯按键操作更新无需依据时间更新）
void lcd_displayUpdate_printingFinishedMenu(void)
{
	uint8_t i;
	//处理按键消息
	switch(keyPressed)  //keyPressed（按下的键值） 由外部中断获得
	{
		case 1://上移
		case 2: //下移
		case 3://返回键
		case 4: //进入下一级菜单
		case 5: //确认键
			keyPressed = 0; //按下的键值清零，否则回一直执行该状态
		    CurrentMenu = &MainMenu;
		    lcdDisplayUpdate = 1; //切换菜单后要将显示更新标志位置位，用于使当前菜单更新到切换的菜单
		    CurrentMenu->displayUpdate_f();
			break;	
		default:
      //为打印完成的菜单条目的赋值是在get_commond()函数内完成的。
			break;
  }
	//更新LCD屏幕上显示的条目内容及选中行的标记
	if(1 == lcdDisplayUpdate) //按键更新标记
	{
		lcdDisplayUpdate = 0; //屏幕显示更新标记清零
		LCD12864_Clear();
		for(i=0;i<4;i++)
		{
			LCD12864_ShowString(i,0,CurrentMenu->menuItems[i+CurrentMenu->range_from]);
		}
	}
}

//确认与否菜单的显示更新函数（纯按键操作更新无需依据时间更新）
void lcd_displayUpdate_YesOrNotMenu(void)
{
	uint8_t i;
	uint8_t lineSeclected; //当前选中的行
	//处理按键消息
	switch(keyPressed)  //keyPressed（按下的键值） 由外部中断获得
	{
		case 1://上移
			keyPressed = 0; //按下的键值清零，否则回一直执行该状态
			if(CurrentMenu->selected <= 2)
				break;
			else
			{
				CurrentMenu->selected--;
//				if(CurrentMenu->selected < CurrentMenu->range_from)
//				{
//					CurrentMenu->range_from = CurrentMenu->selected;
//					CurrentMenu->range_to = CurrentMenu->range_from+3;
//				}
				lcdDisplayUpdate = 1;
				break;
			}
			case 2: //下移
				keyPressed = 0; //按下的键值清零，否则回一直执行该状态
			if(CurrentMenu->selected == CurrentMenu->itemCount-1)
				break;
			else
			{
				CurrentMenu->selected++;
//				if(CurrentMenu->selected > CurrentMenu->range_to)
//				{
//					CurrentMenu->range_to = CurrentMenu->selected;
//					CurrentMenu->range_from = CurrentMenu->range_to-3;
//				}
				lcdDisplayUpdate = 1;
				break;
			}
			case 3://返回键
			{
//				keyPressed = 0; //按下的键值清零，否则回一直执行该状态
//				if(CurrentMenu->parent!=NULL)//父菜单不为空，将显示父菜单
//			  {
//			    CurrentMenu = CurrentMenu->parent;
//			    lcdDisplayUpdate = 1;
//			  }
//				lcdDisplayUpdate = 1;
			 break;
			}
			case 4: //进入下一级菜单
			{
//				keyPressed = 0; //按下的键值清零，否则回一直执行该状态
//				if(CurrentMenu->subMenus[CurrentMenu->selected] != NULL)
//				{
//					CurrentMenu = CurrentMenu->subMenus[CurrentMenu->selected];
//					lcdDisplayUpdate = 1;
//				}

				break;
			}
			case 5: //确认键
			{
				keyPressed = 0; //按下的键值清零，否则回一直执行该状态
				//CurrentMenu->selected = 0; //退出菜单时，将当前菜单的选中项清零
				if(CurrentMenu->subMenus[CurrentMenu->selected] != NULL)
				{
					//选中No时，将当前菜单的选中项清零,并将显示范围调整到初始化状态
//			        CurrentMenu->selected = 2; 
//				    CurrentMenu->range_from = 0; 
//				    CurrentMenu->range_to = 3;
					
					CurrentMenu = CurrentMenu->subMenus[CurrentMenu->selected];
					lcdDisplayUpdate = 1;
					CurrentMenu->displayUpdate_f();
				}
				else
				{
					if(CurrentMenu->func[CurrentMenu->selected] != NULL)
					{
						CurrentMenu->func[CurrentMenu->selected]();//执行相应的函数
						lcdDisplayUpdate = 1;
						CurrentMenu->displayUpdate_f();
					}
				}
				break;
			}
			
			default:
			break;
	}
	//更新LCD屏幕上显示的条目内容及选中行的标记
	if(1 == lcdDisplayUpdate) //按键更新标记
	{
		lcdDisplayUpdate = 0; //屏幕显示更新标记清零
		LCD12864_Clear();
		for(i=0;i<4;i++)
		{
			if(i<2) //纯显示字符串的行，顶行开始显示
			{
			  LCD12864_ShowString(i,0,CurrentMenu->menuItems[i+CurrentMenu->range_from]);
			}
			else //须有按钮动作的行，留出选择图标的位置再开始显示
			{
				LCD12864_ShowString(i,0,CurrentMenu->menuItems[i+CurrentMenu->range_from]);
			}
		}
		lineSeclected = CurrentMenu->selected; //读取当前菜单的选中的条目号
		lineSeclected = 3-(CurrentMenu->range_to - lineSeclected);	//转化位适合屏幕显示的当前选中行
        //LCD12864_ShowString(lineSeclected,0,"->");	//为当前选中的行绘制选中标记
        LCD12864_HightlightShow(lineSeclected,0,16,1);		
	}
}

//一般性的显示更新函数（纯按键操作更新无需依据时间更新）
void lcd_displayUpdate_general(void)
{
	uint8_t i;
	uint8_t lineSeclected; //当前选中的行
	//处理按键消息
	switch(keyPressed)  //keyPressed（按下的键值） 由外部中断获得
	{
		case 1://上移
			keyPressed = 0; //按下的键值清零，否则回一直执行该状态
			if(CurrentMenu->selected <= 0)
				break;
			else
			{
				CurrentMenu->selected--;
				if(CurrentMenu->selected < CurrentMenu->range_from)
				{
					CurrentMenu->range_from = CurrentMenu->selected;
					CurrentMenu->range_to = CurrentMenu->range_from+2;
				}
				lcdDisplayUpdate = 1;
				break;
			}
			case 2: //下移
				keyPressed = 0; //按下的键值清零，否则回一直执行该状态
			if(CurrentMenu->selected == CurrentMenu->itemCount-1)
				break;
			else
			{
				CurrentMenu->selected++;
				if(CurrentMenu->selected > CurrentMenu->range_to)
				{
					CurrentMenu->range_to = CurrentMenu->selected;
					CurrentMenu->range_from = CurrentMenu->range_to-2;
				}
				lcdDisplayUpdate = 1;
				break;
			}
			case 3://返回键
			{
				keyPressed = 0; //按下的键值清零，否则回一直执行该状态
				//退出菜单时，将当前菜单的选中项清零,并将显示范围调整到初始化状态
			    CurrentMenu->selected = 0; 
				CurrentMenu->range_from = 0; 
				CurrentMenu->range_to = 2;
				
				if(CurrentMenu->parent!=NULL)//父菜单不为空，将显示父菜单
			  {
			    CurrentMenu = CurrentMenu->parent;
			    lcdDisplayUpdate = 1;
			    CurrentMenu->displayUpdate_f();
			  }
				lcdDisplayUpdate = 1;
			 break;
			}
			case 4: //进入下一级菜单
			{
				keyPressed = 0; //按下的键值清零，否则回一直执行该状态
				if(CurrentMenu->subMenus[CurrentMenu->selected] != NULL)
				{
					CurrentMenu = CurrentMenu->subMenus[CurrentMenu->selected];
					lcdDisplayUpdate = 1;
					CurrentMenu->displayUpdate_f();
				}

				break;
			}
			case 5: //确认键
			{
				keyPressed = 0; //按下的键值清零，否则回一直执行该状态
				if(CurrentMenu->subMenus[CurrentMenu->selected] != NULL)
				{
					CurrentMenu = CurrentMenu->subMenus[CurrentMenu->selected];
					lcdDisplayUpdate = 1;
					CurrentMenu->displayUpdate_f();
				}
				else
				{
					if(CurrentMenu->func[CurrentMenu->selected] != NULL)
					{
						CurrentMenu->func[CurrentMenu->selected]();//执行相应的函数
						//lcdDisplayUpdate = 1;
					}
				}
				break;
			}
			
			default:
			break;
	}
	//更新LCD屏幕上显示的条目内容及选中行的标记
	if(1 == lcdDisplayUpdate) //按键更新标记
	{
		lcdDisplayUpdate = 0; //屏幕显示更新标记清零
		LCD12864_Clear();
		LCD12864_ShowString(0,0,CurrentMenu->title);
		for(i=1;i<4;i++)
		{
			LCD12864_ShowString(i,0,CurrentMenu->menuItems[i-1+CurrentMenu->range_from]);
		}
		lineSeclected = CurrentMenu->selected; //读取当前菜单的选中的条目号
		lineSeclected = 3-(CurrentMenu->range_to - lineSeclected);	//转化位适合屏幕显示的当前选中行
    //LCD12864_ShowString(lineSeclected,0,"->");	//为当前选中的行绘制选中标记	
		LCD12864_HightlightShow(lineSeclected,0,16,1);
	}
}

void lcd_displayUpdate_MoveAxisMenu(void)
{
	uint8_t i;
	uint8_t lineSeclected; //当前选中的行
	static float move_menu_scale = 10;
//	if(CurrentMenu->parent->selected == 1) //建立父菜单选中项和移动缩放因子的关联
//	{
//		move_menu_scale = 0.1;
//	}
//	else if(CurrentMenu->parent->selected == 2)
//	{
//		move_menu_scale = 1;
//	}
//	else if(CurrentMenu->parent->selected == 3)
//	{
//		move_menu_scale = 10;
//	}
	//处理按键消息
	switch(keyPressed)  //keyPressed（按下的键值） 由外部中断获得
	{
		case 1://上移
			keyPressed = 0; //按下的键值清零，否则回一直执行该状态
			if(CurrentMenu->selected == 0)
				break;
			else
			{
				CurrentMenu->selected--;
				if(CurrentMenu->selected < CurrentMenu->range_from)
				{
					CurrentMenu->range_from = CurrentMenu->selected;
					CurrentMenu->range_to = CurrentMenu->range_from+2;
				}
				lcdDisplayUpdate = 1;
				break;
			}
			case 2: //下移
				keyPressed = 0; //按下的键值清零，否则回一直执行该状态
			if(CurrentMenu->selected == CurrentMenu->itemCount-1)
				break;
			else
			{
				CurrentMenu->selected++;
				if(CurrentMenu->selected > CurrentMenu->range_to)
				{
					CurrentMenu->range_to = CurrentMenu->selected;
					CurrentMenu->range_from = CurrentMenu->range_to-2;
				}
				lcdDisplayUpdate = 1;
				break;
			}
			case 3://返回键
			{
				keyPressed = 0; //按下的键值清零，否则回一直执行该状态
				/*根据当前选中项，调整移动距离的缩放因子*/
				if(CurrentMenu->selected == 0) //当前选中项为调整单位距离菜单
				{
					if(move_menu_scale == 100)
					{
						move_menu_scale = 10;
						CurrentMenu->menuItems[0] = "←uint→ :  10mm";
					}
					else if(move_menu_scale == 10)
					{
						move_menu_scale = 1;
						CurrentMenu->menuItems[0] = "←uint→ :   1mm";
					}
					lcdDisplayUpdate = 1;
					break;
				}
				current_position[CurrentMenu->selected - 1] -= 1 * move_menu_scale; //建立菜单标号(CurrentMenu->selected)与轴号（X_AXIS）的关联
				plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], manual_feedrate[Y_AXIS]/60, active_extruder);
//				if(CurrentMenu->parent!=NULL)//父菜单不为空，将显示父菜单
//			  {
//			    CurrentMenu = CurrentMenu->parent;
//			    lcdDisplayUpdate = 1;
//					CurrentMenu->displayUpdate_f();
//			  }
//				lcdDisplayUpdate = 1;
			 break;
			}
			case 4: //进入下一级菜单
			{
				keyPressed = 0; //按下的键值清零，否则回一直执行该状态
				/*根据当前选中项，调整移动距离的缩放因子*/
				if(CurrentMenu->selected == 0) //当前选中项为调整单位距离菜单
				{
				    if(move_menu_scale == 1)
					{
						move_menu_scale = 10;
						CurrentMenu->menuItems[0] = "←uint→ :  10mm";
					}
					else if(move_menu_scale == 10)
					{
						move_menu_scale = 100;
						CurrentMenu->menuItems[0] = "←uint→ : 100mm";
					}
					lcdDisplayUpdate = 1;
					break;
				}
				/*建立当前菜单标号(CurrentMenu->selected)与轴号（X_AXIS）的关联*/
				current_position[CurrentMenu->selected - 1] += 1 * move_menu_scale; 
				plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], manual_feedrate[Y_AXIS]/60, active_extruder);

//				if(CurrentMenu->subMenus[CurrentMenu->selected] != NULL)
//				{
//					CurrentMenu = CurrentMenu->subMenus[CurrentMenu->selected];
//					lcdDisplayUpdate = 1;
//				}

				break;
			}
			case 5: //确认键
			{
				keyPressed = 0; //按下的键值清零，否则回一直执行该状态
				if(CurrentMenu->parent!=NULL)//父菜单不为空，将显示父菜单
			  {
			    CurrentMenu = CurrentMenu->parent;
			    lcdDisplayUpdate = 1;
					CurrentMenu->displayUpdate_f();
				  return;
			  }
				lcdDisplayUpdate = 1;
//				if(CurrentMenu->subMenus[CurrentMenu->selected] != NULL)
//				{
//					CurrentMenu = CurrentMenu->subMenus[CurrentMenu->selected];
//					lcdDisplayUpdate = 1;
//				}
//				else
//				{
//					if(CurrentMenu->func[CurrentMenu->selected] != NULL)
//					{
//						CurrentMenu->func[CurrentMenu->selected]();//执行相应的函数
//						lcdDisplayUpdate = 1;
//					}
//				}
				break;
			}
			
			default:
			break;
	}
	//更新LCD屏幕上显示的条目内容及选中行的标记
	if(1 == lcdDisplayUpdate) //按键更新标记
	{
		lcdDisplayUpdate = 0; //屏幕显示更新标记清零
		LCD12864_Clear();
		LCD12864_ShowString(0,0,CurrentMenu->title);
		for(i=1;i<4;i++)
		{
			LCD12864_ShowString(i,0,CurrentMenu->menuItems[i-1+CurrentMenu->range_from]);
		}
		lineSeclected = CurrentMenu->selected; //读取当前菜单的选中的条目号
		lineSeclected = 3-(CurrentMenu->range_to - lineSeclected);	//转化位适合屏幕显示的当前选中行
//        LCD12864_ShowString(lineSeclected,0,"->");	//为当前选中的行绘制选中标记
        LCD12864_HightlightShow(lineSeclected,0,16,1);		
	}
}

void lcd_displayUpdate_adjustParameterMenu(void)
{
	uint8_t i;
	uint8_t lineSeclected; //当前选中的行
//	uint8_t tempDiff;
	char temp_diff[16];
	char temp_speed[16];
	char temp_temp[16];
	char temp_fanSpeed[16];
	
	//处理按键消息
	switch(keyPressed)  //keyPressed（按下的键值） 由外部中断获得
	{
		case 1://上移
			keyPressed = 0; //按下的键值清零，否则回一直执行该状态
			if(CurrentMenu->selected == 0)
				break;
			else
			{
				CurrentMenu->selected--;
				if(CurrentMenu->selected < CurrentMenu->range_from)
				{
					CurrentMenu->range_from = CurrentMenu->selected;
					CurrentMenu->range_to = CurrentMenu->range_from+2;
				}
				lcdDisplayUpdate = 1;
				break;
			}
			case 2: //下移
				keyPressed = 0; //按下的键值清零，否则回一直执行该状态
			if(CurrentMenu->selected == CurrentMenu->itemCount-1)
				break;
			else
			{
				CurrentMenu->selected++;
				if(CurrentMenu->selected > CurrentMenu->range_to)
				{
					CurrentMenu->range_to = CurrentMenu->selected;
					CurrentMenu->range_from = CurrentMenu->range_to-2;
				}
				lcdDisplayUpdate = 1;
				break;
			}
			//建立当前菜单的选中项与需要改变参数的关联
			case 3:
				if(CurrentMenu->selected == 0)  //调整差值
				{
					keyPressed = 0;
					differenceValue -= 1;
					if(differenceValue <= 1) //最小值限幅
					{
						differenceValue = 1;
					}
//					sprintf(temp_diff,"DiffValue : %d",differenceValue);
//					AdjustParameterMenu.menuItems[0] = (uint8_t *)temp_diff;
					lcdDisplayUpdate = 1;
//					CurrentMenu->displayUpdate_f();
					break;
				}
				else if(CurrentMenu->selected == 1)
				{
					keyPressed = 0;
					feedmultiply -= differenceValue;
					if(feedmultiply <= 5) //最小值限幅
					{
						feedmultiply = 5;
					}
//					sprintf(temp_speed,"Speed : %d%%",feedmultiply );
//					AdjustParameterMenu.menuItems[1] = (uint8_t *)temp_speed;
					lcdDisplayUpdate = 1;
//					CurrentMenu->displayUpdate_f();
					break;
				}
				else if(CurrentMenu->selected == 2)
				{
					keyPressed = 0;
					target_temperature[0] -= differenceValue;
					if(target_temperature[0] <= 0) //最小值限幅
					{
						target_temperature[0] = 0;
					}
//					sprintf(temp_temp,"Temp_E0:%ddeg",target_temperature[0] );
//					AdjustParameterMenu.menuItems[2] = (uint8_t *)temp_temp;
					lcdDisplayUpdate = 1;
//					CurrentMenu->displayUpdate_f();
					break;
				}
				else if(CurrentMenu->selected == 3)
				{
					keyPressed = 0;
					fanSpeed -= differenceValue;
					if(fanSpeed <= 0) //最小值限幅
					{
						fanSpeed = 0;
					}
//					sprintf(temp_fanSpeed,"FanSpeed : %d%%",fanSpeed );
//					AdjustParameterMenu.menuItems[3] = (uint8_t *)temp_fanSpeed;
					lcdDisplayUpdate = 1;
//					CurrentMenu->displayUpdate_f();
					break;
				}
				//break;
			case 4:
				if(CurrentMenu->selected == 0)
				{
					keyPressed = 0;
					differenceValue += 1;
					if(differenceValue >= 10) //最大值限幅
					{
						differenceValue = 10;
					}
//					sprintf(temp_diff,"DiffValue : %d",differenceValue);
//					AdjustParameterMenu.menuItems[0] = (uint8_t *)temp_diff;
					lcdDisplayUpdate = 1;
//					CurrentMenu->displayUpdate_f();
					break;
				}
				else if(CurrentMenu->selected == 1)
				{
					keyPressed = 0;
					feedmultiply += differenceValue;
					if(feedmultiply >= 500) //最大值限幅
					{
						feedmultiply = 500;
					}
//					sprintf(temp_speed,"Speed : %d%%",feedmultiply );
//					AdjustParameterMenu.menuItems[1] = (uint8_t *)temp_speed;
					lcdDisplayUpdate = 1;
//					CurrentMenu->displayUpdate_f();
					break;
				}
				else if(CurrentMenu->selected == 2)
				{
					keyPressed = 0;
					target_temperature[0] += differenceValue;
					if(target_temperature[0] >= HEATER_0_MAXTEMP - 15) //最小值限幅
					{
						target_temperature[0] = HEATER_0_MAXTEMP - 15;
					}
//					sprintf(temp_temp,"Temp_E0  :  %d%%",target_temperature[0] );
//					AdjustParameterMenu.menuItems[2] = (uint8_t *)temp_temp;
					lcdDisplayUpdate = 1;
//					CurrentMenu->displayUpdate_f();
					break;
				}
				else if(CurrentMenu->selected == 3)
				{
					keyPressed = 0;
					fanSpeed += differenceValue;
					if(fanSpeed >= 255) //最小值限幅
					{
						fanSpeed = 255;
					}
//					sprintf(temp_fanSpeed,"FanSpeed  :  %d%%",fanSpeed );
//					AdjustParameterMenu.menuItems[3] = (uint8_t *)temp_fanSpeed;
					lcdDisplayUpdate = 1;
//					CurrentMenu->displayUpdate_f();
					break;
				}
			case 5: //确认键
			{
				keyPressed = 0; //按下的键值清零，否则回一直执行该状态
				//退出菜单时，将当前菜单的选中项清零,并将显示范围调整到初始化状态
			    CurrentMenu->selected = 0; 
				CurrentMenu->range_from = 0; 
				CurrentMenu->range_to = 3;
				
				if(CurrentMenu->parent!=NULL)//父菜单不为空，将显示父菜单
				{
					CurrentMenu = CurrentMenu->parent;
					lcdDisplayUpdate = 1;
					CurrentMenu->displayUpdate_f();
					return;
		        }
				lcdDisplayUpdate = 1;
			    break;
//				keyPressed = 0; //按下的键值清零，否则回一直执行该状态
//				if(CurrentMenu->parent!=NULL)//父菜单不为空，将显示父菜单
//			  {
//			    CurrentMenu = CurrentMenu->parent;
//			    lcdDisplayUpdate = 1;
//					CurrentMenu->displayUpdate_f();
//			  }
//				lcdDisplayUpdate = 1;
//				if(CurrentMenu->subMenus[CurrentMenu->selected] != NULL)
//				{
//					CurrentMenu = CurrentMenu->subMenus[CurrentMenu->selected];
//					lcdDisplayUpdate = 1;
//				}
//				else
//				{
//					if(CurrentMenu->func[CurrentMenu->selected] != NULL)
//					{
//						CurrentMenu->func[CurrentMenu->selected]();//执行相应的函数
//						lcdDisplayUpdate = 1;
//					}
//				}
//				break;
			}
			
			default:
			break;
	}
	//更新LCD屏幕上显示的条目内容及选中行的标记
	if(1 == lcdDisplayUpdate) //按键更新标记
	{
		lcdDisplayUpdate = 0; //屏幕显示更新标记清零
        sprintf(temp_diff,"←-Diff+→  %d",differenceValue);
		AdjustParameterMenu.menuItems[0] = (uint8_t *)temp_diff;
		sprintf(temp_speed,"←Speed → %d%%",feedmultiply );
		AdjustParameterMenu.menuItems[1] = (uint8_t *)temp_speed;
		sprintf(temp_temp,"←Temp_0→ %d ",target_temperature[0] );
		AdjustParameterMenu.menuItems[2] = (uint8_t *)temp_temp;
		sprintf(temp_fanSpeed,"←FanSpeed→%d",fanSpeed );
		AdjustParameterMenu.menuItems[3] = (uint8_t *)temp_fanSpeed;
		LCD12864_Clear();
		LCD12864_ShowString(0,0,CurrentMenu->title);
		for(i=1;i<4;i++)
		{
			LCD12864_ShowString(i,0,CurrentMenu->menuItems[i-1+CurrentMenu->range_from]);
		}
		lineSeclected = CurrentMenu->selected; //读取当前菜单的选中的条目号
		lineSeclected = 3-(CurrentMenu->range_to - lineSeclected);	//转化位适合屏幕显示的当前选中行
        //LCD12864_ShowString(lineSeclected,0,"->");	//为当前选中的行绘制选中标记
        LCD12864_HightlightShow(lineSeclected,0,16,1);		
	}
}

void lcd_displayUpdate_changeParameterMenu_speed(void)
{
	uint8_t i;
	static uint8_t diffValue = 5; //每次增加或减小的差值
	char tempStr1[16],tempStr2[16];
	//处理按键消息
	switch(keyPressed)  //keyPressed（按下的键值） 由外部中断获得
	{
		case 1://上移
			keyPressed = 0; //按下的键值清零，否则回一直执行该状态
		  temp_feedMultiply += diffValue;
		  if(temp_feedMultiply >= 500) //最大值限幅
			{
				temp_feedMultiply = 500;
			}
		  sprintf(tempStr1,"Speed:%d%%",temp_feedMultiply );
		  ChangeParameterMenu.menuItems[2] = (uint8_t *)tempStr1;
		  lcdDisplayUpdate = 1;
		  CurrentMenu->displayUpdate_f();
			break;
		case 2: //下移
			keyPressed = 0; //按下的键值清零，否则回一直执行该状态
			temp_feedMultiply -= diffValue;
			if(temp_feedMultiply <= 5)
			{
				temp_feedMultiply = 5;
			}
			sprintf(tempStr1,"Speed:%d%%",temp_feedMultiply );
			ChangeParameterMenu.menuItems[2] = (uint8_t *)tempStr1;
			lcdDisplayUpdate = 1;
			CurrentMenu->displayUpdate_f();
			break;
		case 3://返回键
			{
				keyPressed = 0; //按下的键值清零，否则回一直执行该状态
//			  diffValue--;
//				if(diffValue <= 1) //差值最小值限幅
//				{
//					diffValue = 1;
//				}
//				sprintf(tempStr2,"DiffValue:%d",diffValue );
//				ChangeParameterMenu.menuItems[3] = (uint8_t *)tempStr2;
//				lcdDisplayUpdate = 1;
//				CurrentMenu->displayUpdate_f();
				break;
			}
			case 4: //进入下一级菜单
			{
				keyPressed = 0; //按下的键值清零，否则回一直执行该状态
//			  diffValue++;
//				if(diffValue >= 10) //差值最小值限幅
//				{
//					diffValue = 10;
//				}
//				sprintf(tempStr2,"DiffValue:%d",diffValue );
//				ChangeParameterMenu.menuItems[3] = (uint8_t *)tempStr2;
//				lcdDisplayUpdate = 1;
//				CurrentMenu->displayUpdate_f();
				break;
			}
			case 5: //确认键
			{
				keyPressed = 0; //按下的键值清零，否则回一直执行该状态
				feedmultiply = temp_feedMultiply;
				if(CurrentMenu->parent!=NULL)//父菜单不为空，将显示父菜单
			  {
			    CurrentMenu = CurrentMenu->parent;
			    lcdDisplayUpdate = 1;
				CurrentMenu->displayUpdate_f();
			  }
				break;
			}
			
			default:
			break;
	}
  
	//更新LCD屏幕上显示的条目内容及选中行的标记
	if(1 == lcdDisplayUpdate) //按键更新标记
	{
		lcdDisplayUpdate = 0; //屏幕显示更新标记清零
		LCD12864_Clear();
		for(i=0;i<4;i++)
		{
			LCD12864_ShowString(i,0,CurrentMenu->menuItems[i+CurrentMenu->range_from]);
		}
	}
}

void lcd_displayUpdate_changeParameterMenu_temp(void)
{
	uint8_t i;
	static uint8_t diffValue = 1; //每次增加或减小的差值
//  uint16_t temp_e0TargetTemp ; //
	char tempStr1[16],tempStr2[16];
	temp_e0TargetTemp = degTargetHotend(0);
	//处理按键消息
	switch(keyPressed)  //keyPressed（按下的键值） 由外部中断获得
	{
		case 1://上移
			keyPressed = 0; //按下的键值清零，否则回一直执行该状态
		  temp_e0TargetTemp +=diffValue;
		  if(temp_e0TargetTemp >= HEATER_0_MAXTEMP - 15) //最大值限幅
			{
				temp_e0TargetTemp = HEATER_0_MAXTEMP - 15;
			}
		  sprintf(tempStr1,"Temp0:%ddeg",temp_e0TargetTemp );
		  ChangeParameterMenu.menuItems[2] = (uint8_t *)tempStr1;
		  lcdDisplayUpdate = 1;
		  CurrentMenu->displayUpdate_f();
		case 2: //下移
			keyPressed = 0; //按下的键值清零，否则回一直执行该状态
			temp_e0TargetTemp -=diffValue;
			if(temp_e0TargetTemp <= 5)
			{
				temp_e0TargetTemp = 5;
			}
			sprintf(tempStr1,"Temp0:%ddeg",temp_e0TargetTemp );
			ChangeParameterMenu.menuItems[2] = (uint8_t *)tempStr1;
			lcdDisplayUpdate = 1;
			CurrentMenu->displayUpdate_f();
			break;
		case 3://返回键
			{
				keyPressed = 0; //按下的键值清零，否则回一直执行该状态
			  diffValue--;
				if(diffValue <= 1) //差值最小值限幅
				{
					diffValue = 1;
				}
				sprintf(tempStr2,"DiffValue:%d",diffValue );
				ChangeParameterMenu.menuItems[3] = (uint8_t *)tempStr2;
				lcdDisplayUpdate = 1;
				CurrentMenu->displayUpdate_f();
				break;
			}
			case 4: //进入下一级菜单
			{
				keyPressed = 0; //按下的键值清零，否则回一直执行该状态
			  diffValue++;
				if(diffValue >= 10) //差值最小值限幅
				{
					diffValue = 10;
				}
				sprintf(tempStr2,"DiffValue:%d",diffValue );
				ChangeParameterMenu.menuItems[3] = (uint8_t *)tempStr2;
				lcdDisplayUpdate = 1;
				CurrentMenu->displayUpdate_f();
				break;
			}
			case 5: //确认键
			{
				keyPressed = 0; //按下的键值清零，否则回一直执行该状态
				setTargetHotend(temp_e0TargetTemp,0);
				if(CurrentMenu->parent!=NULL)//父菜单不为空，将显示父菜单
			  {
			    CurrentMenu = CurrentMenu->parent;
			    lcdDisplayUpdate = 1;
					CurrentMenu->displayUpdate_f();
			  }
				break;
			}
			
			default:
			break;
	}
  
	//更新LCD屏幕上显示的条目内容及选中行的标记
	if(1 == lcdDisplayUpdate) //按键更新标记
	{
		lcdDisplayUpdate = 0; //屏幕显示更新标记清零
		LCD12864_Clear();
		for(i=0;i<4;i++)
		{
			LCD12864_ShowString(i,0,CurrentMenu->menuItems[i+CurrentMenu->range_from]);
		}
	}
}


//得到path路径下,目标文件的总个数
//path:路径		    
//返回值:总有效文件数  Example：card_getFileNum("0:/GCODE");
uint16_t card_getFileNum(uint8_t *path)
{	  
	u8 res;   //函数内部调用函数的返回值，用于测试函数是否正常执行
	u16 rval=0;  //函数返回值
 	DIR tdir;	 		//临时目录
	FILINFO tfileinfo;	//临时文件信息	
	u8 *fn;
	
  res=f_opendir(&tdir,(const TCHAR*)path); 	//打开目录	
//	while(res)//打开GCODE文件夹
// 	{	    
//		LCD12864_ShowString(1,0,"SD卡无此根目录！");
//		delay_ms(200);				  
//		LCD12864_Clear();//清除显示	     
//		delay_ms(200);				  
//	} 
  tfileinfo.lfsize=_MAX_LFN*2+1;				//长文件名最大长度
	tfileinfo.lfname=mymalloc(tfileinfo.lfsize);//为长文件缓存区分配内存
	if(res==FR_OK&&tfileinfo.lfname!=NULL)
	{
		while(1)//查询总的有效文件数
		{
	    res=f_readdir(&tdir,&tfileinfo);       		//读取目录下的一个文件
	    if(res!=FR_OK||tfileinfo.fname[0]==0)break;	//错误了/到末尾了,退出		  
      fn=(u8*)(*tfileinfo.lfname?tfileinfo.lfname:tfileinfo.fname);			 
			res=f_typetell(fn);	 //获取文件类型
			if((res&0XF0)==0X70)//取高四位,看看是不是Gcode文件	
			{
				rval++;//有效文件数增加1
			}	    
		}  
	} 
	myfree(tfileinfo.lfname);
	return rval;
}
//用于读取SD卡GCODE目录下的有效文件的数目，记录索引值，并将文件名列表读取出来
void card_readFileListInfo(void)
{ 
	u8 res;              //函数返回值
 	DIR gcodir;	 		     //Gcode文件目录
	FILINFO gcofileinfo; //文件信息
	u8 *fn;   			     //长文件名
	u8 temp_fn[100][30];  //用于存储sd卡文件名的临时数组
	u8 *pname;			     //带路径的文件名
	//u16 totgconum; 		   //Gcode文件总数
//	u16 curindex;		     //Gcode文件当前索引
	u16 temp;
	u16 *gcoindextbl;	   //Gcode文件索引表 	
	 
 	//LCD12864_Clear(); //进入该界面后先清屏，为新界面的显示做准备
	 
		//检测SD卡根目录是否有GCODE文件夹	
	//	while(f_opendir(&gcodir,"0:/GCODE"))//打开GCODE文件夹
	// 	{	    
	//		LCD12864_ShowString(1,0,"SD卡中无GCODE 文件夹...");
	//		delay_ms(200);				  
	//		LCD12864_Clear();//清除显示	     
	//		delay_ms(200);				  
	//	} 
		
		totgconum = card_getFileNum("0:/GCODE"); //得到总有效文件数
		//LCD12864_ShowNum(2,0,totgconum); //显示有效文件的数量，用于测试
		//检查有效文件的数目是否为0
	//	while(totgconum==NULL)//文件有效文件数目为0		
	// 	{	    
	//		LCD12864_ShowString(1,0,"没有可打印文件!");
	//		delay_ms(200);				  
	//		LCD12864_Clear();//清除显示	     
	//		delay_ms(200);				  
	//	}
		gcofileinfo.lfsize=_MAX_LFN*2+1;					     	  //长文件名最大长度
		gcofileinfo.lfname=mymalloc(gcofileinfo.lfsize);	//为长文件缓存区分配内存
		pname=mymalloc(gcofileinfo.lfsize);				        //为带路径的文件名分配内存
		gcoindextbl=mymalloc(2*totgconum);				//申请2*totgconum个字节的内存,用于存放文件索引
		//zzz = mymalloc(10*totgconum);    //为文件名存储数组分配空间，不知道改分配多大空间，现在是随便给的。
		//检查内存分配是否出错
	// 	while(gcofileinfo.lfname==NULL||pname==NULL||gcoindextbl==NULL)//内存分配出错
	// 	{	    
	//		LCD12864_ShowString(1,0,"内存分配失败!");
	//		delay_ms(200);				  
	//		LCD12864_Clear();//清除显示	     
	//		delay_ms(200);				  
	//	}
		//记录索引
		CardMenu.itemCount = totgconum; //初始化文件列表中文件数目
		res = f_opendir(&gcodir,"0:/GCODE"); //打开目录
		if(res == FR_OK)
		{
			curindex=0;//当前索引为0
			while(1)//全部查询一遍，记录索引值并将文件当前目录Gcode文件名列表存储到zzz数组中
			{
				temp=gcodir.index;								//记录当前index
				res=f_readdir(&gcodir,&gcofileinfo);       		//读取目录下的一个文件，在下次执行时会自动读取下一个文件
				if(res!=FR_OK||gcofileinfo.fname[0]==0) break;	//错误了/到末尾了,退出		  
				fn=(u8*)(*gcofileinfo.lfname?gcofileinfo.lfname:gcofileinfo.fname);	//获取文件名				
				strcpy((char*)pname,"0:/GCODE/");				    //复制路径(目录)
				strcat((char*)pname,(const char*)fn);  			//将文件名接在后面，形成带路径的文件名 
				res=f_typetell(fn);	 //获取文件类型
				if((res&0XF0)==0X70)//取高四位,看看是不是Gcode文件	 
				{
					gcoindextbl[curindex]=temp;//记录索引
					//strcpy((char *)temp_fn[curindex],"  "); //
	//				zzz[curindex] = temp_fn[curindex]; //测试用，将二维数组中的字符串存取到zzz数组中
					strcpy((char *)cardFileList[curindex],(const char*)fn);
//					strncpy((char *)temp_fn[curindex],(const char*)fn,14); //将文件名前14个字符存储到1个二维数组中
					strcpy((char *)temp_fn[curindex],(const char*)fn);
					CardMenu.menuItems[curindex] = temp_fn[curindex]; //将二维数组中的字符串存取到CardMenu.menuItems数组中，即初始化文件列表名称
					curindex++;
				}	    
			} 
		}   
			//用于测试CardMenu.menuItems[]是否存储了SD卡文件列表
	//	for(curindex=0;curindex<totgconum;curindex++) 
	//	{
	//	  LCD12864_ShowString(3,0,"显示到这。。");
	//		LCD12864_ShowString(curindex,0,CardMenu.menuItems[curindex]);
	//	}   	 
	//while(1);
		 	
//  if(SD_CD) //用于在显示sd卡文件列表页面时，拔出sd卡返回主页面
//	{
//		CurrentMenu = &MainMenu;
//		lcdDisplayUpdate = 1; //切换菜单后要将显示更新标志位置位，用于使当前菜单更新到切换的菜单
//	}		
		myfree(gcofileinfo.lfname);	//释放内存			    
		myfree(pname);				//释放内存			    
		myfree(gcoindextbl);		//释放内存
		//myfree(zzz);	//测试用
}












