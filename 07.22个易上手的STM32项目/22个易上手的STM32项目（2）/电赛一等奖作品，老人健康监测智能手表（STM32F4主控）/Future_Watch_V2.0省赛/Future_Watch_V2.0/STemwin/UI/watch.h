#ifndef _WATCH_H_
#define _WATCH_H_
#include "rtc.h"
#include "GUI.h"
#include <stdlib.h>
typedef struct 
{
	GUI_AUTODEV_INFO AutoDevInfo;  // Information about what has to be displayed
	GUI_POINT        HourPoints[4];   // Polygon data
	GUI_POINT        MinPoints[4]; 
	float            HourAngle;
	float            MinAngle;
	u8 				 sec;
	u8				 month;
	u8 				 day;
	u8				week;
} PARAM;  
 
void Disp_Watch(void);
#endif
