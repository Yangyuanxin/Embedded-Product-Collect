#include <stdio.h>
#include "N76E003.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_Define.h"

UINT8 UID_BYTE(UINT8 Addr)
{
		UINT8 DATATEMP;
		set_IAPEN;
		IAPAL = Addr;
    IAPAH = 0x00;
    IAPCN = READ_UID;
    set_IAPGO;
		DATATEMP = IAPFD;
		clr_IAPEN;
		return DATATEMP;
}

void main(void)
{
	UINT8 READ1,READ2;
	Set_All_GPIO_Quasi_Mode;
//---------toggle GPIO1---------	
	clr_GPIO1;
	Timer0_Delay1ms(100);
	set_GPIO1;
	Timer0_Delay1ms(100);
	clr_GPIO1;
	Timer0_Delay1ms(100);
	set_GPIO1;
	Timer0_Delay1ms(100);
//---------end toggle GPIO1---------
	
	READ1 = UID_BYTE(0x02);
	READ2 = UID_BYTE(0x05);
	while(1);

}