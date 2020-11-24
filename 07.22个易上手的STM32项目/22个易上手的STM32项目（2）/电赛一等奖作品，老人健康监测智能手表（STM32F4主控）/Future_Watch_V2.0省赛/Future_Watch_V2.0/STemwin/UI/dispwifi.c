#include <string.h>
#include "WM.h"
#include "FRAMEWIN.h"
#include "BUTTON.h"
#include "led.h"
#include "DIALOG.h"
#include "dispwifi.h"
#include "usr_wifi.h"
#include "includes.h"
#include "wifibotton.h"
#include "main.h"

extern GUI_CONST_STORAGE GUI_BITMAP bmconnection;
extern GUI_CONST_STORAGE GUI_BITMAP bmunconnection;
extern OS_EVENT * sem_smartlink;	
extern GUI_CONST_STORAGE GUI_BITMAP bmwifino;
extern GUI_CONST_STORAGE GUI_BITMAP bmwifiok;
extern GUI_CONST_STORAGE GUI_FONT GUI_Fontmyfont;
extern _Wifi_Information My_Wifi;
extern SYSTEM_STA System_Sta;
#define ID_WINDOW_0    (GUI_ID_USER + 0x00)
#define ID_BUTTON_0    (GUI_ID_USER + 0x01)



static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 0, 240, 240, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_0, 70, 70, 100, 100, 0, 0x0, 0 },
};

static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;

  switch (pMsg->MsgId) 
  {
	case WM_PAINT:	
					GUI_SetColor(GUI_BLACK);
					GUI_SetFont(&GUI_Fontmyfont);	
					GUI_DrawRect(200,5,230,20);
					GUI_FillRect(194,8,200,17);
					GUI_SetColor(GUI_GREEN);
					if((System_Sta.battery&(1<<7))!=0)
					{
						GUI_FillRect(200,5,229,20);
					}
					else
					{
						if((System_Sta.battery&(1<<6))!=0)
						{
							GUI_FillRect(210,5,229,20);
						}
						else
						{
							if((System_Sta.battery&(1<<5))!=0)
							{
								GUI_SetColor(GUI_RED);
								GUI_FillRect(220,5,229,20);
							}
						}
					}
					if(System_Sta.App_Sta == 0)	//APP未连接
					{
						GUI_DrawBitmap(&bmunconnection,160,2);
					}
					else
					{
						GUI_DrawBitmap(&bmconnection,160,0);
					}
					//"电量剩余:"
					//GUI_DispStringAt("\xe7\x94\xb5\xe9\x87\x8f\xe5\x89\xa9\xe4\xbd\x99:",10,60);
					//"Wifi链接状态:"
					//GUI_DispStringAt("Wifi\xe9\x93\xbe\xe6\x8e\xa5\xe7\x8a\xb6\xe6\x80\x81:",10,20);
					break;
	case WM_INIT_DIALOG://Button
						hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
						BUTTON_SetText(hItem, "");
						if((My_Wifi.Wifi_Sta&(1<<7))!=0)
						{
							BUTTON_SetBitmapEx(hItem,0,&bmwifiok,0,0);
						}
						else
						{
							BUTTON_SetBitmapEx(hItem,0,&bmwifino,0,0);
						}
						break;
	case WM_NOTIFY_PARENT://点击事件
							Id    = WM_GetId(pMsg->hWinSrc);
							NCode = pMsg->Data.v;
							switch(Id) 
							{
								case ID_BUTTON_0: //WIFI按钮
													switch(NCode) 
													{
														case WM_NOTIFICATION_CLICKED:
																break;
														case WM_NOTIFICATION_RELEASED:
															System_Sta.Wifi_Connection = 0;		//清除之前所有标志位
															System_Sta.Wifi_Connection |= (1<<7);	//启动smart link
															System_Sta.Motor|= (1<<7);//开启震动
															// OSSemPost(sem_smartlink);      
															  break;
     
													}break;
													
							}break;							   
	default: WM_DefaultProc(pMsg);break;
    
   }
}


WM_HWIN Create_WifiWindow(void) {
	WM_HWIN hWin;
	GUI_MEMDEV_Handle hMem;
	hMem = GUI_MEMDEV_Create(0, 0,240,240);	
	hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
	GUI_MEMDEV_CopyToLCDAt(hMem, 0,0);
	GUI_MEMDEV_Select(0); 
	GUI_MEMDEV_Delete(hMem);
return hWin;
}


