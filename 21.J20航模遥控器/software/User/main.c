/*
J20航模遥控器遥控器端
	微控制器：STM32F103C8T6
	版本：基础版V2.2
	具体说明见Doc/ReadMe.txt
	仓库：https://github.com/J20RC/STM32_RC_Transmitter
*/
#include "main.h"

/*函数预定义*/
void keyEventHandle(void);
void menuEventHandle(void);

/*变量定义*/
extern unsigned char logo[];
extern unsigned char iconClock[];
extern unsigned char iconAlarm[];
u16 loca;//存放坐标
u16 thrNum;//油门换算后的大小
extern char batVoltStr[8];//电池电压字符串
extern char timeStr[9];//时间字符串
u16 loopCount = 0;//循环次数计数
u16 clockCount = 0;

/*只在程序开始时运行一次的代码*/
void setup(void)
{
	delay_init();//初始化延时函数
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2，2位抢占优先级和2位子优先级
	usart_init(115200);//初始化串口1，波特率为115200
	TIM2_Init(1999,71);//1MHz，每10ms进行ADC采样一次
	TIM3_Init(19999,71);//1MHz，每20ms检测按键一次；
	DMA1_Init();	//DMA初始化
	ADC_Pin_Init();		//ADC初始化
	SET_Init();	//读取用户数据
	RTC_Init();		//RTC初始化
	BEEPER_Init();	//BEEPER初始化
	KEY_Init();		//KEY初始化
	NRF24L01_Init();//NRF24L01初始化
	PPM_Init();//PPM引脚初始化
	
	beeperOnce(20);//蜂鸣器响一下
	OLED_Init();	//初始化OLED
	OLED_Clear();
	OLED_DrawPointBMP(0,0,logo,128,64,setData.onImage);//显示logo
	OLED_Refresh_Gram();//刷新显存
	while(NRF24L01_Check())
	{
 		delay_ms(100);
		Beeper = !Beeper;//蜂鸣器5Hz报警，表示无线模块故障
	}
	Beeper = 0;//蜂鸣器停止响
	delay_ms(1000);
	OLED_Clear_GRAM();//清空
	
	if(setData.clockCheck == ON)//油门开机自检
	{
		if(PWMvalue[2]>1200)
		{
			u8 hzIndex[4] = {youMen,men,baoJing,jing};
			OLED_ShowChineseWords(30,24,hzIndex,4,1);
			OLED_ShowString(94,24,(u8 *)"!",16,1);
			OLED_Refresh_Gram();//写入显存
		}
		while(PWMvalue[2]>1200){}
		OLED_Clear_GRAM();//清空
	}
	if(setData.NRF_Mode == ON)  NRF24L01_TX_Mode(setData.NRF_Power);//发射模式
	else NRF24L01_LowPower_Mode();//掉电模式
	homeWindow();//显示主界面
	OLED_Refresh_Gram();//刷新显存
}

/*不断循环执行的代码*/
void loop(void)
{
	if(clockTime>setData.clockTime*60*50 && setData.clockMode == ON)
	{
		if(clockCount%5==0)
		{
			beeperOnce(20);
			if(nowMenuIndex==home)
			{
				OLED_DrawPointBMP(68,1,iconClock,15,12,1);//时间图标
				OLED_Refresh_Gram();//写入显存
			}
		}
		else{delay_ms(20);}
		if(clockCount%10==0 && nowMenuIndex==home){
			OLED_DrawPointBMP(68,1,iconAlarm,16,12,1);//报警图标
			drawClockTime();//显示时间00:00:00
			OLED_Refresh_Gram();//写入显存
		}
		clockCount++;
		if(clockCount>100)//闹钟响2s
		{
			clockTime = 0;//闹钟时间清零
			clockCount = 0;
		}
	}
	if(loopCount%100==0 && nowMenuIndex==home)//更新油门、后四个通道
	{
		drawClockTime();//显示时间00:00:00
		showSwState();//显示后四个通道状态
		thrNum = (int)(PWMvalue[2]-1000)/22;//更新油门
		if(setData.throttlePreference)//左手油门
		{
			OLED_Fill(2,62-thrNum,2,62,0);//下部分写1
			OLED_Fill(2,16,2,62-thrNum,1);//上部分写0
		}
		else{//右手油门
			OLED_Fill(125,62-thrNum,125,62,0);//下部分写1
			OLED_Fill(125,16,125,62-thrNum,1);//上部分写0
		}
		if(loopCount%1000==0)//检测电池电压
		{
			if(batVoltSignal==1) beeperOnce(10);//蜂鸣器间断鸣叫，报警
			sprintf((char *)batVoltStr,"%1.2fV",batVolt);
			OLED_ShowString(80,19, (u8 *)batVoltStr,16,1);//显示电池电压
		}
		OLED_Refresh_Gram();//刷新显存
	}
	if(keyEvent>0)//微调更新事件
	{
		keyDownSound();
		keyEventHandle();
	}
	if(nowMenuIndex==xcjz14)//行程校准
	{
		menu_xcjz14();
		OLED_Refresh_Gram();//刷新显存
		for(int i=0;i<4;i++)
		{
			if(chResult[i]>setData.chUpper[i]) setData.chUpper[i]=chResult[i];
			if(chResult[i]<setData.chLower[i]) setData.chLower[i]=chResult[i];
		}
	}
	if(nowMenuIndex==dljs18)//舵量监视
	{
		menu_dljs18();
		OLED_Refresh_Gram();//刷新显存
	}
	if(menuEvent[0])//菜单事件
	{
		keyDownSound();
		menuEventHandle();
	}
	lastMenuIndex = nowMenuIndex;
	loopCount++;
}

/*主函数*/
int main()
{
	setup();
	while(1)
	{
		loop();
	}
}


//微调事件处理函数：更新主界面
void keyEventHandle(void)
{
	if(nowMenuIndex==home)
	{
		if(keyEvent==1|keyEvent==2) 
		{
			if(setData.throttlePreference)//左手油门
			{//第1通道微调-右横线
				OLED_Fill(66,61,119,61,1);//写1，清除原来的标志
				loca = (int)93+setData.PWMadjustValue[0]/12.5;
				OLED_Fill(loca-2,61,loca+2,61,0);//写0
			}
			else//右手油门
			{//第4通道微调-右横线
				OLED_Fill(66,61,119,61,1);//写1，清除原来的标志
				loca = (int)93+setData.PWMadjustValue[3]/12.5;
				OLED_Fill(loca-2,61,loca+2,61,0);//写0
			}
		}
		if(keyEvent==3|keyEvent==4) 
		{
			if(setData.throttlePreference)//左手油门
			{//第2通道微调-右竖线
				OLED_Fill(125,16,125,62,1);//写1
				loca = (int)39-setData.PWMadjustValue[1]/14.29;
				OLED_Fill(125,loca-2,125,loca+2,0);//写1
			}
			else//右手油门
			{//第2通道微调-左竖线
				OLED_Fill(2,16,2,56,1);//写1
				loca = (int)39-setData.PWMadjustValue[1]/12;
				OLED_Fill(2,loca-2,2,loca+2,0);//写1
			}
		}
		if(keyEvent==5|keyEvent==6) 
		{	
			if(setData.throttlePreference)//左手油门
			{//第4通道微调-左横线
				OLED_Fill(7,61,61,61,1);//写1，清除原来的标志
				loca = (int)35+setData.PWMadjustValue[3]/12.5;
				OLED_Fill(loca-2,61,loca+2,61,0);//写0
			}
			else//右手油门
			{//第1通道微调-左横线
				OLED_Fill(7,61,61,61,1);//写1，清除原来的标志
				loca = (int)34+setData.PWMadjustValue[0]/12.5;
				OLED_Fill(loca-2,61,loca+2,61,0);//写0
			}
		}
		OLED_Refresh_Gram();//刷新显存
		STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//将用户数据写入FLASH
	}
	keyEvent = 0;
}

//菜单事件处理函数：更新菜单界面、数据处理
void menuEventHandle(void)
{
	OLED_display();
	if(nowMenuIndex==xcjz14 && lastMenuIndex != xcjz14)//通道中立点校准
	{
		for(int i=0;i<4;i++)
		{
			setData.chLower[i] 	= chResult[i];	//遥杆的最小值更新
			setData.chMiddle[i] = chResult[i];	//遥杆的中值
			setData.chUpper[i] 	= chResult[i];	//遥杆的最大值更新
		}
	}
	
	for(int i=0;i<4;i++)//限制微调范围
	{
		if(setData.PWMadjustValue[i]>300-setData.PWMadjustUnit) setData.PWMadjustValue[i]=300-setData.PWMadjustUnit;
		if(setData.PWMadjustValue[i]<setData.PWMadjustUnit-300) setData.PWMadjustValue[i]=setData.PWMadjustUnit-300;
	}
	if(menuEvent[1]==NUM_up)
	{
		if(nowMenuIndex==tdwt1){setData.PWMadjustValue[0] += setData.PWMadjustUnit;menu_tdwt1();}
		if(nowMenuIndex==tdwt2){setData.PWMadjustValue[1] += setData.PWMadjustUnit;menu_tdwt2();}
		if(nowMenuIndex==tdwt3){setData.PWMadjustValue[2] += setData.PWMadjustUnit;menu_tdwt3();}
		if(nowMenuIndex==tdwt4){setData.PWMadjustValue[3] += setData.PWMadjustUnit;menu_tdwt4();}
		if(nowMenuIndex==tdwt5){setData.PWMadjustValue[4] += setData.PWMadjustUnit;menu_tdwt5();}
		if(nowMenuIndex==tdwt6){setData.PWMadjustValue[5] += setData.PWMadjustUnit;menu_tdwt6();}
		if(nowMenuIndex==tdwt7){setData.PWMadjustValue[6] += setData.PWMadjustUnit;menu_tdwt7();}
		if(nowMenuIndex==tdwt8){setData.PWMadjustValue[7] += setData.PWMadjustUnit;menu_tdwt8();}
		if(nowMenuIndex==tdzf1){setData.chReverse[0] = !setData.chReverse[0];menu_tdzf1();}
		if(nowMenuIndex==tdzf2){setData.chReverse[1] = !setData.chReverse[1];menu_tdzf2();}
		if(nowMenuIndex==tdzf3){setData.chReverse[2] = !setData.chReverse[2];menu_tdzf3();}
		if(nowMenuIndex==tdzf4){setData.chReverse[3] = !setData.chReverse[3];menu_tdzf4();}
		if(nowMenuIndex==tdzf5){setData.chReverse[4] = !setData.chReverse[4];menu_tdzf5();}
		if(nowMenuIndex==tdzf6){setData.chReverse[5] = !setData.chReverse[5];menu_tdzf6();}
		if(nowMenuIndex==tdzf7){setData.chReverse[6] = !setData.chReverse[6];menu_tdzf7();}
		if(nowMenuIndex==tdzf8){setData.chReverse[7] = !setData.chReverse[7];menu_tdzf8();}
		if(nowMenuIndex==ymph) {setData.throttlePreference = !setData.throttlePreference;menu_ymph();}
		if(nowMenuIndex==dyjz) {setData.batVoltAdjust += 1;menu_dyjz();}
		if(nowMenuIndex==bjdy) {setData.warnBatVolt += 0.01;menu_bjdy();}
		if(nowMenuIndex==jsbj) {setData.RecWarnBatVolt += 0.1;menu_jsbj();}
		if(nowMenuIndex==wtdw) 
		{
			setData.PWMadjustUnit += 1;
			if(setData.PWMadjustUnit>9) {setData.PWMadjustUnit = 9;}//限制微调单位范围
			menu_wtdw();
		}
		if(nowMenuIndex==xzmx) {setData.modelType += 1;if(setData.modelType>2) {setData.modelType=0;}menu_xzmx();}
		if(nowMenuIndex==wxfs) {setData.NRF_Mode =!setData.NRF_Mode;menu_wxfs();}
		if(nowMenuIndex==ppmsc) {setData.PPM_Out =!setData.PPM_Out;menu_ppmsc();}
		if(nowMenuIndex==ajyx) {setData.keySound =!setData.keySound;menu_ajyx();}
		if(nowMenuIndex==kjhm) {setData.onImage =!setData.onImage;menu_kjhm();}
		if(nowMenuIndex==nzbj) {setData.clockMode =!setData.clockMode;menu_nzbj();}
		if(nowMenuIndex==nzsc) 
		{
			setData.clockTime += 1;
			if(setData.clockTime>60) {setData.clockTime=60;}//限制闹钟时长
			menu_nzsc();
		}
		if(nowMenuIndex==kjzj) {setData.clockCheck =!setData.clockCheck;menu_kjzj();}
		if(nowMenuIndex==skbh) 
		{
			setData.throttleProtect += 5;
			if(setData.throttleProtect>100) {setData.throttleProtect = 100;}//限制油门保护值
			menu_skbh();
		}
		if(nowMenuIndex==fsgl)
		{
			setData.NRF_Power += 2;
			if(setData.NRF_Power>0x0f) {setData.NRF_Power=0x0f;}//限制功率11,13,15
			menu_fsgl();
		}
	}
	if(menuEvent[1]==NUM_down)
	{
		if(nowMenuIndex==tdwt1){setData.PWMadjustValue[0] -= setData.PWMadjustUnit;menu_tdwt1();}
		if(nowMenuIndex==tdwt2){setData.PWMadjustValue[1] -= setData.PWMadjustUnit;menu_tdwt2();}
		if(nowMenuIndex==tdwt3){setData.PWMadjustValue[2] -= setData.PWMadjustUnit;menu_tdwt3();}
		if(nowMenuIndex==tdwt4){setData.PWMadjustValue[3] -= setData.PWMadjustUnit;menu_tdwt4();}
		if(nowMenuIndex==tdwt5){setData.PWMadjustValue[4] -= setData.PWMadjustUnit;menu_tdwt5();}
		if(nowMenuIndex==tdwt6){setData.PWMadjustValue[5] -= setData.PWMadjustUnit;menu_tdwt6();}
		if(nowMenuIndex==tdwt7){setData.PWMadjustValue[6] -= setData.PWMadjustUnit;menu_tdwt7();}
		if(nowMenuIndex==tdwt8){setData.PWMadjustValue[7] -= setData.PWMadjustUnit;menu_tdwt8();}
		if(nowMenuIndex==tdzf1){setData.chReverse[0] = !setData.chReverse[0];menu_tdzf1();}
		if(nowMenuIndex==tdzf2){setData.chReverse[1] = !setData.chReverse[1];menu_tdzf2();}
		if(nowMenuIndex==tdzf3){setData.chReverse[2] = !setData.chReverse[2];menu_tdzf3();}
		if(nowMenuIndex==tdzf4){setData.chReverse[3] = !setData.chReverse[3];menu_tdzf4();}
		if(nowMenuIndex==tdzf5){setData.chReverse[4] = !setData.chReverse[4];menu_tdzf5();}
		if(nowMenuIndex==tdzf6){setData.chReverse[5] = !setData.chReverse[5];menu_tdzf6();}
		if(nowMenuIndex==tdzf7){setData.chReverse[6] = !setData.chReverse[6];menu_tdzf7();}
		if(nowMenuIndex==tdzf8){setData.chReverse[7] = !setData.chReverse[7];menu_tdzf8();}
		if(nowMenuIndex==ymph) {setData.throttlePreference = !setData.throttlePreference;menu_ymph();}
		if(nowMenuIndex==dyjz) {setData.batVoltAdjust -= 1;menu_dyjz();}
		if(nowMenuIndex==bjdy) {setData.warnBatVolt -= 0.01;menu_bjdy();}
		if(nowMenuIndex==jsbj) {setData.RecWarnBatVolt -= 0.1;menu_jsbj();}
		if(nowMenuIndex==wtdw) 
		{
			setData.PWMadjustUnit -= 1;
			if(setData.PWMadjustUnit<1) {setData.PWMadjustUnit = 1;}//限制微调单位范围
			menu_wtdw();
		}
		if(nowMenuIndex==xzmx) {if(setData.modelType==0){setData.modelType=2;}else {setData.modelType -= 1;}menu_xzmx();}
		if(nowMenuIndex==wxfs) {setData.NRF_Mode =!setData.NRF_Mode;menu_wxfs();}
		if(nowMenuIndex==ppmsc) {setData.PPM_Out =!setData.PPM_Out;menu_ppmsc();}
		if(nowMenuIndex==ajyx) {setData.keySound =!setData.keySound;menu_ajyx();}
		if(nowMenuIndex==kjhm) {setData.onImage =!setData.onImage;menu_kjhm();}
		if(nowMenuIndex==nzbj) {setData.clockMode =!setData.clockMode;menu_nzbj();}
		if(nowMenuIndex==nzsc) 
		{
			setData.clockTime -= 1;
			if(setData.clockTime<1) {setData.clockTime=1;}//限制闹钟时长
			menu_nzsc();
		}
		if(nowMenuIndex==kjzj) {setData.clockCheck =!setData.clockCheck;menu_kjzj();}
		if(nowMenuIndex==skbh) 
		{
			if(setData.throttleProtect<5) {setData.throttleProtect = 5;}//限制油门保护值
			setData.throttleProtect -= 5;
			menu_skbh();
		}
		if(nowMenuIndex==fsgl)
		{
			setData.NRF_Power -= 2;
			if(setData.NRF_Power<0x0b) {setData.NRF_Power=0x0b;}//限制功率11,13,15
			menu_fsgl();
		}
	}
	if(menuEvent[1]==MENU_enter)//旋转编码器短按后，改变菜单显示
	{
		if(nowMenuIndex==tdwt1){menu_tdwt1();}
		if(nowMenuIndex==tdwt2){menu_tdwt2();}
		if(nowMenuIndex==tdwt3){menu_tdwt3();}
		if(nowMenuIndex==tdwt4){menu_tdwt4();}
		if(nowMenuIndex==tdwt5){menu_tdwt5();}
		if(nowMenuIndex==tdwt6){menu_tdwt6();}
		if(nowMenuIndex==tdwt7){menu_tdwt7();}
		if(nowMenuIndex==tdwt8){menu_tdwt8();}
		if(nowMenuIndex==tdzf1){menu_tdzf1();}
		if(nowMenuIndex==tdzf2){menu_tdzf2();}
		if(nowMenuIndex==tdzf3){menu_tdzf3();}
		if(nowMenuIndex==tdzf4){menu_tdzf4();}
		if(nowMenuIndex==tdzf5){menu_tdzf5();}
		if(nowMenuIndex==tdzf6){menu_tdzf6();}
		if(nowMenuIndex==tdzf7){menu_tdzf7();}
		if(nowMenuIndex==tdzf8){menu_tdzf8();}
		if(nowMenuIndex==ymph) {menu_ymph();}
		if(nowMenuIndex==dyjz) {menu_dyjz();}
		if(nowMenuIndex==bjdy) {menu_bjdy();}
		if(nowMenuIndex==wtdw) {menu_wtdw();}
		if(nowMenuIndex==xzmx) {menu_xzmx();}
		if(nowMenuIndex==wxfs) {menu_wxfs();}
		if(nowMenuIndex==ppmsc) {menu_ppmsc();}
		if(nowMenuIndex==ajyx) {menu_ajyx();}
		if(nowMenuIndex==kjhm) {menu_kjhm();}
		if(nowMenuIndex==nzbj) {menu_nzbj();}
		if(nowMenuIndex==nzsc) {menu_nzsc();}
		if(nowMenuIndex==kjzj) {menu_kjzj();}
		if(nowMenuIndex==jsbj) {menu_jsbj();}
		if(nowMenuIndex==skbh) {menu_skbh();}
		if(nowMenuIndex==fsgl) {menu_fsgl();}
	}
	if(nowMenuIndex!=lastMenuIndex)
	{
		STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//将用户数据写入FLASH
	}
	OLED_Refresh_Gram();//刷新显存
	menuEvent[0] = 0;
}
