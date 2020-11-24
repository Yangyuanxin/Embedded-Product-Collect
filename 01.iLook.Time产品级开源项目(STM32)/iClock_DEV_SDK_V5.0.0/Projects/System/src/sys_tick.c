/**
  ******************************************************************************
  * @file    sys_tick.c
  * @author  SZQVC
  * @version V1.0.0
  * @date    2015.2.14
  * @brief   灯塔计划.海啸项目 (QQ：49370295)
  *          system tick,与CPU相关
  ******************************************************************************
  * @attention                                                                 *
  *                                                                            *
  * <h2><center>&copy; COPYRIGHT 2015 SZQVC</center></h2>                      *
  *                                                                            *
  * 文件版权归“深圳权成安视科技有限公司”(简称SZQVC）所有。                      *
  *                                                                            *
  *        http://www.szqvc.com                                                *
  *                                                                            *
  ******************************************************************************
**/
#include "stm32f10x.h"
#include "sys_tick.h"

/* define */
struct _SYS_TICK_TYPE
{
    uint32_t ms;
    uint32_t ten_ms;
    uint32_t Sec;
}systick;

#define us        12      //@72MHz


/* public */

/* extern */

/* private */


/*******************************************************************************
* Function Name  : SysTick_Init
* Description    : 系统定时器时钟初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_Init(void)
{
    systick.ms = 0;
    systick.Sec =0;
    SysTick_Config(SystemCoreClock/1000);    //1ms中断一次
}

/*******************************************************************************
* Function Name  : SysTick_Ctrl
* Description    : 系统定时器时钟ENABLE/DISABLE
* Input          : ENABLE/DISABLE
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_Ctrl(uint16_t cmd)
{
    if( cmd==ENABLE ){
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    }else if( cmd==DISABLE){
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    }
}

/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    : 系统定时器中断
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern void KeyTask(void);

void SysTick_Handler(void)
{
    systick.ms++;
    if(systick.ms%1000==0)
      systick.Sec++;
    /*需要在定时器中处理的任务 */
    KeyTask();
}

/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    : 获取ms计数器
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint32_t GetSysTick_ms(void)
{
    return systick.ms;
}

void MarkSysTick_ms(uint32_t *t)
{
  *t = systick.ms;
}

/*******************************************************************************
* Function Name  : GetSysTick_Sec
* Description    : 获取sec计数器
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint32_t GetSysTick_Sec(void)
{
    return systick.Sec;
}

void MarkSysTick_Sec(uint32_t *t)
{
  *t = systick.Sec;
}

/*******************************************************************************
* Function Name  : delay_nus
* Description    : 延时n us
* Input          : i
* Output         : None
* Return         : None
*******************************************************************************/
void delay_us(uint32_t i)
{
  i = i*us;
	while(i--);
}

/*******************************************************************************
* Function Name  : delay_ms
* Description    : 延时n ms
* Input          : i
* Output         : None
* Return         : None
*******************************************************************************/
void delay_ms(uint32_t i)
{
    uint32_t end_t = systick.ms+i;
    
    while( systick.ms<end_t );
}

/*******************************************************************************
* Function Name  : TimeOutCheck_Sec, TimeOutCheck_ms
* Description    : 延时n ms
* Input          : i
* Output         : None
* Return         : None
*******************************************************************************/
char TimeOutCheck_Sec(uint32_t i)
{
  if( systick.Sec>=i )
    return 1;
  else
    return 0;
}

char TimeOutCheck_ms(uint32_t i)
{
  if( systick.ms>=i )
    return 1;
  else
    return 0;
}

