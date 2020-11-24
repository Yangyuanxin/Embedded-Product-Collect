/**
  ******************************************************************************
  * @file    ui.h
  * @author  SZQVC
  * @version V1.0.0
  * @date    2015.2.14
  * @brief   顶层任务,用户进程
  ******************************************************************************
**/


#ifndef UI_H
#define UI_H

#define TEMP_UNKNOW   -10000
#define PRESS_UNKNOW  -10000
#define ALTITUDE_UNKNOW  -10000

/*----------------------------------------------------------------------------------------*/
typedef enum _UI_TASK_STATE
{
    UI_TASK_NULL    = T_NULL,
    UI_PWR_ON       = T_PWR_ON,
    //
    UI_START_WIN,
    //
    UI_TIME,            //时间
    UI_TIMER,           //计时器界面
    UI_COUNTDOWN,       //倒计时界面
    UI_WEATHER,         //凸显温度+气压+海拔
    UI_SPORT,           //运动
    UI_HERE,            //我在这
    //
    UI_DEBUG,           //调试界面
    UI_END_WIN,
    //
    UI_PWR_OFF      = T_PWR_OFF,
    UI_HW_ERR       = T_HW_ERR,
}UI_TASK_STATE;

typedef enum _SMART_LIFE_STATE{
  GPS_AUTO_ON         = 1,                //GPS自动开机
  GPS_AUTO_OFF        = 1<<1,             //GPS自动关机
  DISPLAY_AUTO_ONOFF  = 1<<2,             //显示屏自动开关
}SMART_LIFE_STATE;


typedef struct _UI_TSK_MSG_T{
  char              deep_sleep;           //0.非深度睡眠状态 1: 处于深度睡眠状态
  unsigned char     resume_ui_state;      //进入休眠前的ui状态
  SMART_LIFE_STATE  smart_life;
  char              flg_wkup;             //唤醒
  //
  char              new_day;
  tm                smart_life_tm;
  uint32_t          gps_sleep_t_msk;      //gps睡眠时间戳
  uint32_t          sysclock_t_msk;       //系统实时时钟同步时间戳
  uint32_t          ui_reflash_tick;      //界面刷新时间戳
  //
  double            altitude;
  int16_t           altitude_dis;
}UI_TSK_MSG_T;

void UiTask_Init(void);
char UiTask(void);

/*----------------------------------------------------------------------------------------*/
void SmartLifeTask_Init(void);
void SmartLifeTask(void);
/*----------------------------------------------------------------------------------------*/
typedef struct{
  //固定在该位置#define FLASH_REC_ADDR  0x0801F800
  uint32_t id;      //标识0x5aa5a55a
  //
}FL_REC_TYPE;

#endif

/*************************** (C) COPYRIGHT SZQVC ******************************/
/*                              END OF FILE                                   */
/******************************************************************************/


