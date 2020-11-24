/***********************************************************************/
/*                         SZQVC.Lighthouse                            */
/*                           www.szqvc.com                             */
/***********************************************************************/
#ifndef __BASIC_TASK_H
#define __BASIC_TASK_H

typedef struct{
  float BatteryVol;
  uint8_t PlugIn;
}POWER_TSK_MSG_T;

typedef enum _ADC_PROCESS_STATE{
  ADC_CONVERT,
  ADC_CONVERT_WAIT,
  ADC_READ,
  ADC_CONVERT_FINISHED,
}ADC_PROCESS_STATE;


typedef enum _POWER_MONITOR_STATE{
  PWR_MONITOR_INIT,
  PWR_MONITOR_OFF,
}POWER_MONITOR_STATE;

void PowerMonitorTask_Init(void);
void PowerMonitorTask(void);
void GetBatteryAtOnce(void);

/*----------------------------------------------------------------------------------------*/
#define NULL_KEY     0
#define WAKUP_KEY    1
#define MODE_KEY     1
#define PWR_KEY      2
 
typedef struct _KEY_INFO{
  uint8_t value;
  uint8_t type;     //1 单按,2 双击,3 长按
  uint8_t wkup_int; //中断唤醒
}KEY_INFO;

//
typedef enum _KEY_TYPE
{
  KEY_NULL  = 0,
  KEY_CLICK,
  KEY_DB_CLICK,
  KEY_LONG_PRESS,
  KEY_LONG_RELEASE,
  KEY_HOLD,
}KEY_TYPE;
//
#define RD_WKUP_KEY() GPIO_ReadInputDataBit(WKUP_KEY,WKUP_KEY_PIN)
void KeyTask_Init(void);
void KeyTask(void);
void ClrKey(void);
/*----------------------------------------------------------------------------------------*/
typedef enum _USB_TASK_ST
{
  USB_UNLINK,
  USB_OPEN,
  USB_CHECK,
  USB_LINKING,
  USB_CLOSE,
}USB_TASK_ST;

void UsbMonitorTask_Init(void);
char UsbMonitorTask(void);

void RealTime_Init(void);
void UpdateSysTime(void);
void RealTime_Task(void);


#endif
/*********************** (C) COPYRIGHT SZQVC **************************/
/*                          END OF FILE                               */
/**********************************************************************/
