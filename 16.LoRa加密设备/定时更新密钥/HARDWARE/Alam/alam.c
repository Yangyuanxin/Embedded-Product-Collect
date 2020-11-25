/*******************************************************************
	* 文件描述：防拆按钮
	* 作者：	ZWQ
	* 时间：	2019-3-13
	* 备注：	检测按键，检测到拆除，做出相应的措施！
********************************************************************/
#include "alam.h"

u16 AlamKeyPressCounter = 0x00;


//报警按键初始化
void AlamKeyPinInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(ALAM_RCC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = ALAM_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//设置成上拉输入
	GPIO_Init(ALAM_PORT, &GPIO_InitStructure);		//初始化GPIO
}


void ScanAlamKey(void)
{
	if( ALAM_KEY == 0 )
	{
		if(AlamKeyPressCounter <1000)
		{
			AlamKeyPressCounter ++;
		}
		
		else
		{
			AlamKeyPressCounter = 0x00;
		}
	}
}



