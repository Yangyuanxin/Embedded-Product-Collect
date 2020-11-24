/**
  ******************************************************************************
  * @file    task.c
  * @author  SZQVC
  * @version V1.0.0
  * @date    2015.2.14
  * @brief   灯塔计划.海啸项目 (QQ：49370295)
  *          QST前后台进程管理系统
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
#include "stm32f10x.h"
#include "sys_tick.h"
#include "task.h"

/* define */
struct _QST_STATE{
  uint32_t mloop_per_sec;
}Qst;

/* public */

/* extern */

/* private */
static unsigned long mon_task_tick,mon_task_loop_cnt;


/*******************************************************************************
* Function Name  : TaskCtrl
* Description    : 任务状态切换
* Input          : - tsk: 任务结构指针
*                  - ctrl: 切换到什么状态
* Output         : None
* Return         : None
*******************************************************************************/
void QstCtrl(TASK_CTRL_INFO *tsk, unsigned char ctrl)
{
  tsk->Ctrl = ctrl|TASK_CMD;
  tsk->TickMsk = GetSysTick_ms();  //记录进入该状态的时间标签
  //立即执行一次任务
  if( tsk->Process != 0x0 )
    tsk->Process();
}


/*******************************************************************************
* Function Name  : TaskEnter
* Description    : 任务状态跳转
* Input          : - tsk: 任务结构指针
*                  - st: 任务直接进入到什么状态
* Output         : None
* Return         : None
*******************************************************************************/
void QstEnter( TASK_CTRL_INFO *tsk, unsigned char st )
{
  tsk->MsgFlg = TASK_MSG_ST_CHANGE;      //进入新的状态，信息应该被更新  
  tsk->State = st;                       //设定状态
  tsk->TickMsk = GetSysTick_ms();        //记录进入该状态的时间标签
}

/*******************************************************************************
* Function Name  : QstRestTaskTick
* Description    : 复位任务计数器
* Input          : - tsk: 任务结构指针
* Output         : None
* Return         : None
*******************************************************************************/
void QstRestTskTick( TASK_CTRL_INFO *tsk )
{
  tsk->TickMsk = GetSysTick_ms();        //记录进入该状态的时间标签
}


/*******************************************************************************
* Function Name  : QstGetCmd
* Description    : 获取任务控制命令
* Input          : - tsk: 任务结构指针
* Output         : T_NULL or Command
* Return         : None
*******************************************************************************/
unsigned char QstGetCmd( TASK_CTRL_INFO *tsk )
{
  if( tsk->Ctrl&TASK_CMD ){
    tsk->Ctrl &= ~TASK_CMD;
    return tsk->Ctrl;
  }else
    return T_NULL;
}

/*******************************************************************************
* Function Name  : QstGetState
* Description    : 获取任务状态
* Input          : - tsk: 任务结构指针
* Output         : Task state
* Return         : None
*******************************************************************************/
unsigned char QstGetState( TASK_CTRL_INFO *tsk )
{
  return tsk->State;
}

/*******************************************************************************
* Function Name  : QstGetMsg
* Description    : 获取任务信息指针
* Input          : - tsk: 任务结构指针
* Output         : 输出任务信息指针
* Return         : None
*******************************************************************************/
unsigned char *QstGetMsg( TASK_CTRL_INFO *tsk )
{
    return tsk->Msg;
}

/*******************************************************************************
* Function Name  : QstGetMsgState
* Description    : 获取任务信息标志
* Input          : - tsk: 任务结构指针
* Output         : 0 没有信息
* Return         : None
*******************************************************************************/
unsigned char QstGetMsgState( TASK_CTRL_INFO *tsk )
{
  return tsk->MsgFlg;
}

/*******************************************************************************
* Function Name  : QstMsgClr
* Description    : 清除任务信息
* Input          : - tsk: 任务结构指针
* Output         : None
* Return         : None
*******************************************************************************/
void QstMsgClr( TASK_CTRL_INFO *tsk )
{
  tsk->MsgFlg = TASK_MSG_NULL;
}

/*******************************************************************************
* Function Name  : TaskMonitor
* Description    : 主循环每秒执行次数，任务信息监视任务.
* Input          : - 
* Output         : None
* Return         : None
*******************************************************************************/
void QstMonitor_Init(void)
{
  mon_task_tick = 0;
}

void QstMonitor(void)
{
  //主循环速度
  if( GetSysTick_ms()>mon_task_tick+1000 ){
    mon_task_tick = GetSysTick_ms();
    Qst.mloop_per_sec = mon_task_loop_cnt;
    mon_task_loop_cnt = 0;
  }else{
    mon_task_loop_cnt++;
  }
  //
}

/*************************** (C) COPYRIGHT SZQVC ******************************/
/*                              END OF FILE                                   */
/******************************************************************************/
