#include "my_delay.h"


static u16 Counter = 0x00;
u16 DelayTimer = 0;

void TimerDelay_Ms(u16 counter)
{
	Counter = 0x00;
	while( Counter<counter );
}


void RunDelayCounter(void)
{
	if( Counter<DELAY_MAX_VALUE )
	{
		Counter ++;
	}
	
	if(DelayTimer< 10000)
		DelayTimer++;
}


void Clear(void)
{
	DelayTimer = 0;
}

u16 GetCounterValue()
{
	return DelayTimer;
}



