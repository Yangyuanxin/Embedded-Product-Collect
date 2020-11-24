/**
  ******************************************************************************
  * @file    ui.c
  * @author  SZQVC
  * @version V0.0.1
  * @date    2015.2.14
  * @brief   灯塔计划.海啸项目 (QQ：49370295)
  ******************************************************************************
  * @attention                                                                 *
  *                                                                            *
  * <h2><center>&copy; COPYRIGHT 2015 SZQVC</center></h2>                      *
  *                                                                            *
  * 文件版权归“深圳权成安视科技有限公司”(简称SZQVC）所有。                   *
  *                                                                            *
  *        http://www.szqvc.com                                                *
  *                                                                            *
  ******************************************************************************
**/
#include "task.h"
#include "sys_includes.h"
#include "ui.h"

/* define */

/* public */
char gPowerOnFlg;

/* extern */
extern KEY_INFO gKey;

/* private */
uint32_t last_rt_tick;
uint32_t timer_rt_tick;
//
void win_debug_init(void);
char win_debug_process(void);
//
void win_time_init(void);
char win_time_process(void);
//
void win_timer_init(void);
char win_timer_process(void);
//
void display_system_bar(int x, int y, int type);
//
static char tmp_str[150];     //110 公共字符串缓冲

//任务信息调用接口
POWER_TSK_MSG_T    *pwr;
DISPLAY_TSK_MSG_T  *displayer;

//
tm *now;      //当前时间指针



/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                                UI TASK                                     */
/*                             系统最顶层任务                                  */
/*                                                                            */
/* 建立一个任务的三个重要步骤                                                   */
/* 1.定义一个任务描述结构,并在任务初始化时设定参数。如:TASK_CTRL_INFO UiTskInfo   */
/* 2.建立任务初始化函数。如: void UiTask_Init(void)                             */
/* 3.建立任务主体函数。如：char UiTask(void)                                   */
/*----------------------------------------------------------------------------*/
TASK_CTRL_INFO UiTskInfo;   //ui任务控制结构
UI_TSK_MSG_T UiTskMsg;      //ui任务信息结构
KEY_INFO ui_key;

void UiTask_Init(void)
{
    //初始化任务信息
    UiTskInfo.TickGap = 0;        //UI任务运行速度，=0 不进行速度控制
    UiTskInfo.TickMsk = 0;        //UI任务运行时间戳
    UiTskInfo.Msg = 0;            //UI信息输出的指针，=0 为无信息输出结构
    UiTskInfo.MsgFlg = 0;         //UI信息输出标志位
    UiTskInfo.Process = UiTask;   //UI注册任务运行主函数指针
    //各驱动模块接口指针初始化
  	pwr=(POWER_TSK_MSG_T *)QstGetMsg(&PowerTskInfo);              //获取电源管理任务信息指针
  	displayer = (DISPLAY_TSK_MSG_T*)QstGetMsg(&DisplayTskInfo);   //获取显示器任务信息指针
  	//
  	now = sys_time(0x0);          //获取系统时钟指针
  	//
    QstCtrl(&UiTskInfo,UI_TIME);                //UI任务控制：运行实时时间界面
}

//下一个界面的索引与控制
int next_win(void)
{
  int next_state = UiTskInfo.State;
  char flg=0;
  
  while(1){
    next_state++;
    switch( next_state ){
      case UI_DEBUG:          //调试界面
      case UI_TIME:           //时间界面
      case UI_TIMER:          //计时器界面
        flg = 1;
        break;

      case UI_COUNTDOWN:       //倒计时界面        
      case UI_WEATHER:         //环境界面
        flg = 0;
        break;
      
      case UI_END_WIN:
      default:
        next_state = UI_START_WIN+1;
        flg = 1;
        break;
    }
    if( flg==1 )
      break;
  }
  //
  return next_state;
}


char UiTask(void)
{
  //获取控制命令(所有任务的运行都是先看看是否有控制信息进来)
  char tsk_cmd = QstGetCmd(&UiTskInfo);

  //按键处理
  if( (QstGetState(&DisplayTskInfo)==DISPLAY_WORKING)&&(gKey.type!=KEY_NULL) ){
    memcpy(&ui_key,&gKey,sizeof(KEY_INFO));
    ClrKey();
  }else{
    ui_key.type = 0;
    ui_key.value = 0;
  }

  //处理命令(UiTask的命令主要是用户界面跳转)
  if( tsk_cmd != T_NULL ){
    switch(UiTskInfo.Ctrl){
      case UI_TIME:
        win_time_init();
        QstEnter(&UiTskInfo,UI_TIME);
        break;
                    
      case UI_DEBUG:
        win_debug_init();
        QstEnter(&UiTskInfo,UI_DEBUG);
        break;
              
      case UI_TIMER:
        win_timer_init();
        QstEnter(&UiTskInfo,UI_TIMER);
        break;
      
      default:
        break;
    }
  }
  
  //任务进程(所有任务必须采用状态机结构，屏幕关闭状态不处理）
  if( QstGetState(&DisplayTskInfo)==DISPLAY_WORKING ){
    switch(UiTskInfo.State){
      case UI_DEBUG:
        if( win_debug_process()==1 )
          QstCtrl(&UiTskInfo,next_win());
        break;
          
      case UI_TIME:
        if( win_time_process()==1 )
          QstCtrl(&UiTskInfo,next_win());
        break;
              
      case UI_TIMER:
        if( win_timer_process()==1 )
          QstCtrl(&UiTskInfo,next_win());
        break;
    }
  }
  
  return 1;
}


//------------------------------------------------------------------------------------//
//                                  debug界面                                          //
//------------------------------------------------------------------------------------//
void win_debug_init(void)
{
  QstCtrl( &DisplayTskInfo,DISPLAY_PWR_ON);
  
  UiTskMsg.ui_reflash_tick = 0;
}

char win_debug_process(void)
{
  if((ui_key.value==1)&&(ui_key.type==KEY_CLICK)){
    return 1;
  }
  
  if( TimeOutCheck_ms(UiTskMsg.ui_reflash_tick+100) ){
    UiTskMsg.ui_reflash_tick = GetSysTick_ms();

  }

  return 0;
}

//------------------------------------------------------------------------------------//
//                                  计时器显示界面                                      //
//------------------------------------------------------------------------------------//
void win_timer_init(void)
{
  QstCtrl( &DisplayTskInfo,DISPLAY_PWR_ON);
  //系统菜单初始化
  display_system_bar(84,24,0);
  //
  UiTskMsg.ui_reflash_tick = 0;
  last_rt_tick = 0;
}

char win_timer_process(void)
{
  char rslt=0;
  uint32_t counter;
  
  //按键处理
  if( (ui_key.value==MODE_KEY)&&(ui_key.type==KEY_CLICK) ){
    return 1;
  }
  if( (ui_key.value==MODE_KEY)&&(ui_key.type==KEY_DB_CLICK) ){
    timer_rt_tick = now->tick;
  }
 
  //时钟显示
  if( now->tick != last_rt_tick ){
    //计时器
    counter = now->tick-timer_rt_tick;
    if( (counter>0)&&(counter<59999) ){
      sprintf(tmp_str,"%03d:%02d",counter/60,counter%60);
      Glph_Print(0,0,LED_14X32,tmp_str);
    }else{
      Glph_Print(0,0,LED_14X32,"000:00");
	}
    //当前时间
    sprintf(tmp_str,"%02d:%02d:%02d",now->hour,now->min,now->sec);
    if( strlen(tmp_str)==8 ){
      Glph_Print(88,0,LED_5X7,tmp_str);
    }
    //
    display_system_bar(84,24,0);
  //
    last_rt_tick = now->tick;
  }

  return rslt;
}

//------------------------------------------------------------------------------------//
//                                  时间显示界面                                      //
//------------------------------------------------------------------------------------//
void display_week(int pos_x, int pos_y);


void win_time_init(void)
{
  QstCtrl( &DisplayTskInfo,DISPLAY_PWR_ON);
  //系统菜单初始化
  display_system_bar(84,24,0);
  //
  UiTskMsg.ui_reflash_tick = 0;
  last_rt_tick = 0;
}

char win_time_process(void)
{
  char rslt=0;
//  char disp_str[20];
  
  //按键处理
  if( (ui_key.value==MODE_KEY)&&(ui_key.type==KEY_CLICK) ){
    return 1;
  }
 
  //时钟显示
  if( now->tick != last_rt_tick ){
    //时间
    sprintf(tmp_str,"%02d:%02d",now->hour,now->min);
    if( strlen(tmp_str)==5 ){
      Glph_Print(0,0,LED_14X32,tmp_str);
    }
    sprintf(tmp_str,"%02d",now->sec);
    if( strlen(tmp_str)==2){
      Glph_Print(72,21,LED_5X7,tmp_str);
    }
    //日期
    sprintf(tmp_str,"%02d/%02d",now->month,now->mday);
    if( strlen(tmp_str)==5){
      Glph_Print(88,0,LED_8X16,tmp_str);
    }
    //系统状态
    display_system_bar(84,24,0);
    //
    display_week(88,16);
    //
    last_rt_tick = now->tick;
  }

  return rslt;
}

void init_system_bar(int x, int y, int type)
{
  DrawBitmap(103+4*5,16,BIT_MAP_VERT,5,8,(uint8_t*)ICO_V_m_5X8);  //m符号
  DrawBitmap(83+3*5,16,BIT_MAP_VERT,5,8,(uint8_t*)ICO_V_c_5X8);   //摄氏度符号
}

void display_system_bar(int x, int y, int type)
{
//  char tmp_str[10];
  int x1=x+4;

  //电源状态
  if( pwr->PlugIn ){
    if( pwr->BatteryVol>=4.13 )
      Glph_DrawBattery(x1,y,2,ICO_BAT_4);
    else
      Glph_DrawBattery(x1,y,2,ICO_BAT_CHARGE);
  }else{
    if( pwr->BatteryVol>3.95 )
      Glph_DrawBattery(x1,y,2,ICO_BAT_4);
    else if( pwr->BatteryVol>3.8 )
      Glph_DrawBattery(x1,y,2,ICO_BAT_3);
    else if( pwr->BatteryVol>3.65 )
      Glph_DrawBattery(x1,y,2,ICO_BAT_2);
    else if( pwr->BatteryVol>3.5 )
      Glph_DrawBattery(x1,y,2,ICO_BAT_1);
    else
      Glph_DrawBattery(x1,y,2,ICO_BAT_0);
  }
}

void display_week(int pos_x, int pos_y)
{
  Glph_DrawRectangle(72,3,79,13,0,1);
  Glph_Print(74,4,LED_5X7,(char*)Int2DecStr(now->wday));
}



/*----------------------------------------------------------------------------------------*/
/*                                                                                        */
/*                                ARM Flash配置区                                         */
/*                                                                                        */
/* 从FLASH_REC_ADDR地址开始（0x801F800)，大小2K。                                          */
/* 存储FL_REC_TYPE的结构变量                                                               */
/*----------------------------------------------------------------------------------------*/
#define FLASH_REC_ADDR  0x0801F800

char read_fl_rec_info(FL_REC_TYPE *out)
{
  int i;
  uint8_t *fl_p = (uint8_t *)FLASH_REC_ADDR;
  uint8_t *out_p = (uint8_t *)out;

  for( i=0; i<sizeof(FL_REC_TYPE); i++ ){
    *out_p++ = *fl_p++;
  }
  return 1;
}


char write_fl_rec_info(FL_REC_TYPE *in )
{
  int i,struct_size;
  uint32_t fl_p = (uint32_t)FLASH_REC_ADDR;
  uint32_t *in_p = (uint32_t *)in;
  
  //
  FLASH_Unlock();
  FLASH_ErasePage( FLASH_REC_ADDR );        /* 擦除页 */
  //
  struct_size = sizeof(FL_REC_TYPE);
  if(struct_size%4)
    struct_size = struct_size/4+1;
  else
    struct_size = struct_size/4;
  //
  for( i=0; i<struct_size; i++ ){
    FLASH_ProgramWord( fl_p,*in_p++);
    fl_p += 4;
  }
  
  return 1;
}


/*************************** (C) COPYRIGHT SZQVC ******************************/
/*                              END OF FILE                                   */
/******************************************************************************/

