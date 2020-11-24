
#include "GUI.h"
#include "FT6236.h"
#include "st7789.h"

extern TouchPointRefTypeDef TPR_Structure;

void GUI_TOUCH_X_ActivateX(void) 
{
}


void GUI_TOUCH_X_ActivateY(void)
{
}

//获取X的值
int  GUI_TOUCH_X_MeasureX(void) 
{
	int32_t xvalue;
	FT6236_Scan();
	xvalue=TPR_Structure.x[0];
	return xvalue;

}

//获取Y的值
int  GUI_TOUCH_X_MeasureY(void) 
{	
	int32_t yvalue;
	FT6236_Scan();
	yvalue=TPR_Structure.y[0];
	return yvalue;
}

