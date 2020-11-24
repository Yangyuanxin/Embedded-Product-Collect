#include "main.h"
#include "ucosii_task.h"

#define FLASH_SAVE_ADDR  0X080FFF00 
/**********外部变量声明********/
extern  u8  Data_All[];
extern _Wifi_Information My_Wifi;
extern u16 AdcDma_Buf;
extern int acc;   
extern short a_x,a_y,a_z;    
extern GUI_CONST_STORAGE GUI_BITMAP bmstartmcu;

/**********函数声明************/
void ALL_test(void);
int System_Init(void);

/**********全局变量定义*********/
SYSTEM_STA System_Sta;			//系统标志位
SYSTEM_DATA	System_Data;		//系统数据
SYSTEM_SET	System_Set;			//系统设置
u8 H_Average_Buf[12]={0,0,0,0,0,0,0,0,0,0,0,0};//存放心率的数值，第11位表示数据是否已经10个，第10位表示当前存放的下标
u8 T_Average_Buf[12]={0,0,0,0,0,0,0,0,0,0,0,0};//存放体温的数值，第11位表示数据是否已经10个，第10位表示当前存放的下标

int main(void)
{  
	System_Init();
	OSInit();   				//初始化UCOSII
	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	OSStart();	  	 			//启动UCOSII
}


//系统总初始化
int System_Init(void)
{
	/*系统时钟*/
	Stm32_Clock_Init(336,8,2,7);	//设置时钟,168Mhz
	delay_init(168);				//初始化延时函数
	/*开机处理*/
	Key_Init();				//按键初始化
	delay_ms(5000);		//等待10s
	START_MCU;				//开机
	MOTOR_START;
	delay_ms(500);
	MOTOR_STOP;
	System_Sta.Mcu_Power = 1;
	/*关全局中断*/
	INTX_DISABLE();
	/*硬件初始*/
	Usart_1_Init(84,115200);		//串口2，链接WIFI
	Time_4_Init(1000-1,8399);		//定时器4，系统心跳 100ms
	TIM3_PWM_Init(500,83);			//光敏PWM			
	TIM3->CCR3 = 500;
	Time_1_Init(0xffff,16799);			//定时器1心率，0.1ms一个计数值	
	MyAdc_DMAConfig(DMA2_Stream0,0,(u32)&ADC1->DR,(u32)&AdcDma_Buf,1);
	//MyAdc_DMAConfig(DMA2_Stream0,0,(u32)&ADC1->DR,(u32)&Adc_Buf,1);	//ADC的DMA
	RTC_Init();		//RTC初始化
	/*软件初始化*/
	RCC->AHB1ENR |= (1<<12);		//使能CRC时钟
	GUI_Init();						//GUI初始化
	GUI_UC_SetEncodeUTF8();
	/*外设初始*/
	Init_ST7789();		   	//初始化LCD
	FT6236_Init();			//初始化触摸屏
	GUI_SetBkColor(GUI_BLACK);
	GUI_Clear();
	GUI_DrawBitmap(&bmstartmcu,0,20);
	GUI_Exec();
	GUI_AA_EnableHiRes();
	GUI_AA_SetFactor(2);
	SMBus_Init();			//温度传感器
	Motor_Init();			//振动马达初始化
	Adc_Init();				//电量ADC初始化
	delay_ms(1000);
	if(MPU6050_Init()){GUI_DispStringAt("mpu6050 err",120,120);GUI_Exec1();}//6050初始化
	delay_ms(200);
	mpu_dmp_init(); 		//DMP初始化
	MPU_Set_Accel_Fsr(3);	 //加速度传感器,±2g
	Lsens_Init();			//光敏初始化
	IWDG_Init(4,20000);		//溢出时间为2s
	/*开全局中断*/
	Interrupt_Set();
	INTX_ENABLE();
	System_Sta.Disp_Sta |= (1<<1);		//开机主界面
	Wifi_Init();
	Read_Data();
	RTC_Set_Time(12,30,00,0);
	System_Sta.Get_Heart = 300;		//启动一次心率检测
	System_Sta.Get_Temp = 300;		//启动一次体温检测
	return 0;
}


//void ALL_test(void)
//{
////	u8 i = 0;
////	GUI_SetBkColor(GUI_WHITE);
////	GUI_Clear();
////	GUI_SetFont(&GUI_Font32_ASCII);
////	GUI_SetColor(GUI_RED);
////	while(1)
////	{
////		Check_Fall();
////		GUI_DispDecAt(acc,120,120,5);
//////			i = Lsens_Get_Val();
//////			TIM3->CCR3 = (i*10);
//////		GUI_Exec();
////		delay_ms(10);
////	}
//}

//从FLASH读数据
int Read_Data(void)
{
	STMFLASH_Read(FLASH_SAVE_ADDR,(u32*)(&System_Set.warning_temp),1);
	STMFLASH_Read(FLASH_SAVE_ADDR+4,(u32*)(&System_Set.eat_min1),1);
	STMFLASH_Read(FLASH_SAVE_ADDR+8,(u32*)(&System_Set.eat_time1),1);
	STMFLASH_Read(FLASH_SAVE_ADDR+4,(u32*)(&System_Set.eat_min1),2);
	STMFLASH_Read(FLASH_SAVE_ADDR+8,(u32*)(&System_Set.eat_time1),2);
	STMFLASH_Read(FLASH_SAVE_ADDR+4,(u32*)(&System_Set.eat_min1),3);
	STMFLASH_Read(FLASH_SAVE_ADDR+8,(u32*)(&System_Set.eat_time1),3);
	STMFLASH_Read(FLASH_SAVE_ADDR+16,(u32*)(&System_Set.warning_fall),1);
	STMFLASH_Read(FLASH_SAVE_ADDR+20,(u32*)(&System_Set.warning_heart),1);
	STMFLASH_Read(FLASH_SAVE_ADDR+24,(u32*)(&System_Set.warning_eat),1);
	return 0;
}
//保存数据到FLASH
int Save_Data(void)
{
	STMFLASH_Write(FLASH_SAVE_ADDR,(u32*)(&System_Set.warning_temp),1);
	STMFLASH_Write(FLASH_SAVE_ADDR+4,(u32*)(&System_Set.eat_min1),1);
	STMFLASH_Write(FLASH_SAVE_ADDR+8,(u32*)(&System_Set.eat_time1),1);	
	STMFLASH_Write(FLASH_SAVE_ADDR+4,(u32*)(&System_Set.eat_min2),1);
	STMFLASH_Write(FLASH_SAVE_ADDR+8,(u32*)(&System_Set.eat_time2),1);	
	STMFLASH_Write(FLASH_SAVE_ADDR+4,(u32*)(&System_Set.eat_min3),1);
	STMFLASH_Write(FLASH_SAVE_ADDR+8,(u32*)(&System_Set.eat_time3),1);
	STMFLASH_Write(FLASH_SAVE_ADDR+16,(u32*)(&System_Set.warning_fall),1);
	STMFLASH_Write(FLASH_SAVE_ADDR+20,(u32*)(&System_Set.warning_heart),1);
	STMFLASH_Write(FLASH_SAVE_ADDR+24,(u32*)(&System_Set.warning_eat),1);
	return 0;
}

