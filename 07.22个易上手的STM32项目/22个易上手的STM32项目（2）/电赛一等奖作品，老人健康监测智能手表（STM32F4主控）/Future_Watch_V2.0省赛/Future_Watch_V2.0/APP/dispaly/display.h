#ifndef _DISPALY_H_
#define _DISPALY_H_

#include "sys.h"
#include "delay.h"
#include "GUI.h"
#include "includes.h"
#include "rtc.h"
#include "watch.h"
#include "WM.h"
void Help_Window(WM_HWIN *head);
void Remind_Window(WM_HWIN *head);
void Main_Window(WM_HWIN *head);
int Disp_lockWindow(WM_HWIN *head);
void Wifi_Window(WM_HWIN *head);
void Disp_Data(WM_HWIN *head);
void Disp_Circle(WM_HWIN *head);
void Disp_Update(WM_HWIN *head);
void Data_Window(WM_HWIN *head);
void Disp_Connection(WM_HWIN *head);
void Disp_No(void);
void Disp_Ok(void);
void Motor_Work(void);
#endif
