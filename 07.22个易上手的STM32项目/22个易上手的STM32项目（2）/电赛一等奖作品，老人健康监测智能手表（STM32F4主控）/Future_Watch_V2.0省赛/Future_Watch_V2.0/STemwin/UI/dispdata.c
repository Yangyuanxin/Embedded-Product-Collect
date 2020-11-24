#include "dispdata.h"
#include "main.h"

extern SYSTEM_STA System_Sta;
extern GUI_CONST_STORAGE GUI_BITMAP bmheart;
extern GUI_CONST_STORAGE GUI_BITMAP bmtmep;
extern GUI_CONST_STORAGE GUI_BITMAP bmBLPR;
extern GUI_CONST_STORAGE GUI_FONT GUI_Fontmyfont;

#define ID_WINDOW_0    (GUI_ID_USER + 0x00)
#define ID_BUTTON_0    (GUI_ID_USER + 0x01)
#define ID_BUTTON_1    (GUI_ID_USER + 0x02)
#define ID_BUTTON_2    (GUI_ID_USER + 0x03)


static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 0, 240, 240, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "temp", ID_BUTTON_0, 0, 0, 80, 80, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "heart", ID_BUTTON_1, 0, 81, 80, 80, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "BP", ID_BUTTON_2, 0, 162, 80, 80, 0, 0x0, 0 },
};


static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  switch (pMsg->MsgId) 
  {
	case WM_PAINT:  //画线
					GUI_SetColor(GUI_BLACK);
					GUI_DrawLine(0,80,240,80);
					GUI_DrawLine(0,161,240,161);
					GUI_DrawLine(80,0,80,240);	
					GUI_SetFont(&GUI_Fontmyfont);
					//"体温:"
					GUI_DispStringAt("\xe4\xbd\x93\xe6\xb8\xa9:",83,2);
					//"均值:"
					GUI_DispStringAt("\xe5\x9d\x87\xe5\x80\xbc:",83,42);
					//"心率:"
					GUI_DispStringAt("\xe5\xbf\x83\xe7\x8e\x87:",83,82);
					//"均值:"
					GUI_DispStringAt("\xe5\x9d\x87\xe5\x80\xbc:",83,122);
					//"舒张压:"
					GUI_DispStringAt("\xe8\x88\x92\xe5\xbc\xa0\xe5\x8e\x8b:",83,202);
					//"收缩压:"
					GUI_DispStringAt("\xe6\x94\xb6\xe7\xbc\xa9\xe5\x8e\x8b:",83,162);
					break;		
	case WM_INIT_DIALOG://体温
						hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
						BUTTON_SetText(hItem, "");
						BUTTON_SetBitmapEx(hItem,0,&bmtmep,0,0);
						//心率
						hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
						BUTTON_SetText(hItem, "");
						BUTTON_SetBitmapEx(hItem,0,&bmheart,0,0);
						// 血压
						hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
						BUTTON_SetText(hItem, "");
						BUTTON_SetBitmapEx(hItem,0,&bmBLPR,0,0);
						break;
	case WM_NOTIFY_PARENT://点击事件
						{	
							Id = WM_GetId(pMsg->hWinSrc);
							NCode = pMsg->Data.v;
							switch(Id) 
							{
								case ID_BUTTON_0: //体温
												{	
													switch(NCode) 
													{
														case WM_NOTIFICATION_CLICKED:break;
														case WM_NOTIFICATION_RELEASED:
															System_Sta.Motor|= (1<<7);//开启震动
															System_Sta.Get_Temp = 301;
															break;
													}
												break;
												}
								case ID_BUTTON_1: //心率
												{	
													switch(NCode) 
													{
														case WM_NOTIFICATION_CLICKED:break;
														case WM_NOTIFICATION_RELEASED:
															System_Sta.Motor|= (1<<7);//开启震动
															System_Sta.Get_Heart = 301;	//触发一次测量
																break;
													  }
													  break;
												  }
								case ID_BUTTON_2: //血压
												{
													switch(NCode) 
													{
														case WM_NOTIFICATION_CLICKED:break;
														case WM_NOTIFICATION_RELEASED:
															if((System_Sta.Measure&(1<<7))==0)
															{
																System_Sta.Motor|= (1<<7);//开启震动
																System_Sta.Measure|=(1<<7);//开始一次心率测量															
															}
															break;
													}	 
												 break;
												}
							}
						break;
						}
	default:WM_DefaultProc(pMsg);break;													
  }
}

WM_HWIN CreateWindow(void) {
	WM_HWIN hWin;
	GUI_MEMDEV_Handle hMem;
	hMem = GUI_MEMDEV_Create(0, 0,240,240);	
	hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
	GUI_MEMDEV_CopyToLCDAt(hMem, 0,0);
	GUI_MEMDEV_Select(0); 
	GUI_MEMDEV_Delete(hMem);
	return hWin;
}

WM_HWIN Create_DataWindow(void) {
  WM_HWIN hWin;
	//BUTTON_SetDefaultSkinClassic();
	GUI_MEMDEV_Handle hMem;
	hMem = GUI_MEMDEV_Create(0, 0,240,240);	
	hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
	GUI_MEMDEV_CopyToLCDAt(hMem, 0,0);
	GUI_MEMDEV_Select(0); 
	GUI_MEMDEV_Delete(hMem);
	GUI_Exec();
	//WM_EnableMemdev(hWin);
	
return hWin;
}

