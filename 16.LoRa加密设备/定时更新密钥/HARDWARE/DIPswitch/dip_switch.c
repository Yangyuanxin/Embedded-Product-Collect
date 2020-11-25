#include "dip_switch.h"


//记录设备属性
u8 LoRaDeviceIs = 0;


/*********************************************
	* 功能描述：拨码开关初始化
	* 入口参数：无
	* 返回值：	无
	* 备注：	上拉输入，如判断有无播下？
**********************************************/
void DIP_SwitchInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA| 
							RCC_APB2Periph_GPIOB|
							RCC_APB2Periph_GPIOC, ENABLE );		//使能时钟
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);			//PB3 PB4 复用GPIO
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//使用SW 禁用JTAG （PB3与PB4）	
	
	//GPIOB-3/4/8/9
	GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_3 |
									GPIO_Pin_4 |
									GPIO_Pin_8 | 
									GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	//设置成下拉输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);			//初始化GPIO
	
	//GPIOA-11/12/15
	GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_11 |	//PA11/12/15
									GPIO_Pin_12 |
									GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPD;	//设置成下拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);			//初始化GPIO
	
	//GPIOC-13
	GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_13;	//PC13
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPD;	//设置成下拉输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);			//初始化GPIO
	
	JudgeMasterOrSlave();
}


/*******************************************
	* 功能描述：判断是主机OR从机
	* 入口参数：无
	* 返回值：	无
	* 备注：	
********************************************/
void JudgeMasterOrSlave(void)
{
	if( ID8 == 1 )
		LoRaDeviceIs = MASTER;
	else
		LoRaDeviceIs = SLAVE;
}

/******************************************
	* 获取串口号

*******************************************/
void ConfirmUartx(void)
{
	if( ID1 == 1 )
	{
		hostcom_struct.WhichUart = UART1;
	}
	else
		hostcom_struct.WhichUart = UART3;
}


/**********************************************
	* 功能描述：设置波特率
	* 入口参数：无
	* 返回值：
	* 备注：
***********************************************/
void SetUartBound(void)
{
	u8 uartx_bound = 0x00;
	
	uartx_bound = ID2 | ID3<<1;
	
	switch(uartx_bound)
	{
		case 0:
			hostcom_struct.Bound = 9600;
		break;
		
		case 1:
			hostcom_struct.Bound = 19200;
		break;
		
		case 2:
			hostcom_struct.Bound = 38400;
		break;
		
		case 3:
			hostcom_struct.Bound = 115200;
		break;
		
		default:
		break;
	}
}


/********************************************
	* 功能描述：读取设备地址
	* 入口参数：
	* 返回值：
	* 备注：
*********************************************/
u8 ReadDeviceID(void)
{
	u8 device_id = 0x00;
	
	device_id |= ID4 | (ID5<<1) | (ID6<<2) | (ID7<<3);
    return device_id;
}



