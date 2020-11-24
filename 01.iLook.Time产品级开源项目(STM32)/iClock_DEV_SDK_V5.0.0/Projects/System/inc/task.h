/**
  ******************************************************************************
  * @file    task.h
  * @author  SZQVC
  * @version V1.0.0
  * @date    2015.2.14
  * @brief   QST前后台进程管理系统
  ******************************************************************************
**/
#ifndef __TASK_H

#define __TASK_H


/*---------------------------------------------------------------------*/
/* QST进程管理系统定义 */
#define TASK_CMD            0x80      //任务标志
#define TASK_MSG_NULL       0x00      //无信息
#define TASK_MSG_ST_CHANGE  0x80      //状态切换信息

typedef struct _TASK_CTRL_INFO{
  unsigned char Ctrl;       //任务命令输入，第8位必须是1。TASK_CMD|New State
  unsigned char State;      //任务当前状态
  unsigned long TickMsk;    //任务时间戳
  unsigned long TickGap;    //任务时间间隔
  unsigned int  MsgFlg;     //任务新信息标志位
  unsigned char *Msg;       //任务信息指针
  char (*Process)(void);         //任务函数指针
}TASK_CTRL_INFO;

//任务信息处理
void QstMsgClr( TASK_CTRL_INFO *tsk );
unsigned char QstGetMsgState( TASK_CTRL_INFO *tsk );
unsigned char *QstGetMsg( TASK_CTRL_INFO *tsk );

//任务状态机控制
void QstCtrl( TASK_CTRL_INFO *tsk, unsigned char ctrl );

//任务状态机跳转
void QstEnter( TASK_CTRL_INFO *tsk, unsigned char st );

//获取外部控制命令
unsigned char QstGetCmd( TASK_CTRL_INFO *tsk );

//获取任务状态
unsigned char QstGetState( TASK_CTRL_INFO *tsk );

//复位任务计时器
void QstRestTskTick( TASK_CTRL_INFO *tsk );

//QST看守进程
void QstMonitor_Init(void);
void QstMonitor(void);

//任务公有状态定义
#define T_NULL      0x00    //空状态
#define T_PWR_ON    0x01    //任务打开状态
#define T_PWR_OFF   0x70    //任务关闭状态
#define T_HW_ERR    0x71    //任务相关硬件错误状态


/*---------------------------------------------------------------------*/
/* 项目所涉及的TASK声明全部放到这里 */
extern TASK_CTRL_INFO UiTskInfo;        //系统顶层任务

extern TASK_CTRL_INFO CompassTskInfo;   //指南针驱动任务

extern TASK_CTRL_INFO DisplayTskInfo;   //显示驱动任务

extern TASK_CTRL_INFO gSensorTskInfo;   //加速度传感器驱动任务

extern TASK_CTRL_INFO GpsTskInfo;       //GPS驱动任务

extern TASK_CTRL_INFO PowerTskInfo;     //电源管理任务

extern TASK_CTRL_INFO BaroTskInfo;      //气压传感器任务
/*---------------------------------------------------------------------*/

#endif


/*************************** (C) COPYRIGHT SZQVC ******************************/
/*                              END OF FILE                                   */
/******************************************************************************/


