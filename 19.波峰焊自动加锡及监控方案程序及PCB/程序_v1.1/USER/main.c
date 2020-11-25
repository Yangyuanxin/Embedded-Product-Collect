#include "main.h"
#include "adc.h"
//extern void Adc_Init(void);
//----------全局变量---------------
u16 DeviceErrorCode=0;  //设备错误会置位该标志
u8 ResetLinitFlag=0;    //设置上下限标志重新设置上下限标志,
u8 Wifi_ConnetRightFlag=0;  //wifi连接正常会置位该标志位
u8 Wifi_ConnetSeverFlag=0;  //wifi连接服务器正常会置位该标志位
u8 ResetRomFlag=0;  //TIM2接收到置位ROM信息的数据会置位该标志
u8 Cmd_Flag_OK=0;   //命令标志,应答OK
u8 Cmd_SendErrorFlag=0;    //发送命令失败时置位该标志,可能发送不完整等
u8 SendFluxDoingFlag=0; //正在发送流量监控信息时会置位该标志,防止回复中所包含的信息频繁访问ROM
u8 Timer3_EnableFlag=0; //定时器3使能标志,置位该标志将使能定时器3做普通定时器
u8 SolenoidValveOnFlag=0;   //电磁阀打开标准,电磁阀打开将置位该标志
u8 FluxStartFlag=0; //开始监控流量数据标志,进入监控时会置位该标志

char RecvicePN[32]="";  //保存接收到的PN
u16 FluxValue_Now=0;    //实时流量值
u16 Flowmeter_H=0;  //流量上下限
u16 Flowmeter_L=0;  //流量上下限
//--------------------------------

//---------此C文件有效之变量-------
u8 CommSuccess_Flag=0; //步骤一,建立通讯成功标志
u8 RightRomOk_Flag=0; //步骤二,确定ROM值成功标志
//--------------------------------

// LED_RUN
// LED_RGB
// V_SIGNAL
char data1[] = "CMD,PN9898989898E,H3009,L2001,LINE2,END";
char ActivePN[32]="";  //用于存储当前机种信息
int main(void)
{

    char iDataBuf[128];
    char iTemp_Buf[64];
    u16 iTemp=0;




    delay_init();	    	 //延时函数初始化
    NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    uart_init(115200);	 	//串口初始化
    uart3_init(115200);
    TIMx_Int_Init(TIM3,2000,7200);	//初始化3,用做普通定时功能 1000 = 100ms
    TIMx_Int_Init(TIM2,2000,7200);	//初始化2,用于检测接收命令,1000=100ms
    DeviceErrorCode = 0;
    ResetLinitFlag = 1; //为1则需要获取上线限
    ResetRomFlag = 1;   //初始时需要进行获取
    CommSuccess_Flag=0; //步骤一,建立通讯成功标志,初始假设为OK
    RightRomOk_Flag=0;  //步骤二,确定ROM值成功标志,初始假设为OK
    Wifi_ConnetRightFlag = 0;//连接则为1
    Cmd_Flag_OK = 0;

    motor_Init();
    LED_Init();			     //LED端口初始化
    KEY_Init();              //初始化与按键连接的硬件接口
    Adc_Init();
    MCU_RED=1; MCU_YELLOW=1; MCU_GREEN=1; MCU_BEEP=0; MCU_LOCK=0;
    Cmd_SendErrorFlag=0;
    test_mcu=1;
    while(1)
    {

        while(0) //Debug
        {
			ENA=1;
		//	PUL=0;
			motor_R(1000,1);			
        }

        usart3_printf("Location 1\r\n");
        while(0 == Wifi_ConnetRightFlag)    //WIFI 未连接是先连接WiFi
        {
            iTemp = WIFI_Config();
            if(0 != iTemp)
            {
                DeviceErrorCode |= ERRORCODE_CONNECT;
            }
            else
            {
                DeviceErrorCode = 0;    //消除所有设备异常
                Wifi_ConnetRightFlag = 1; //表明WiFi连接正常
                CommSuccess_Flag = 0;   //允许下一步操作连接
                TIM_Cmd(TIM2,ENABLE);	//开启定时器
				MCU_RED=0; MCU_YELLOW=0; MCU_GREEN=1;
                break;
            }
            delay_ms(100);
        }
        while(Wifi_ConnetRightFlag)
        {
            while(1)
            {
				TIM_Cmd(TIM2,DISABLE);	//关闭定时器
				MCU_YELLOW=1; MCU_RED=0; MCU_GREEN=0;
                if(IR_UP==0)							//若IR_UP检测到物体时跳出当前循环
                {
					
					
                    delay_ms(10);
                    if(IR_UP==0)
                    {
                        break;
                    }
				}
                else
				{
					while(motor_R(1000,1));				//若IR_UP检测不到物体时电机保持向上转动，直到检测到物体跳出循环
				}
					break;
            }
            motor_S();
            delay_ms(10);
            while(MCU_start!=0)
            {
                delay_ms(1);
                while(MCU_start!=0);
            }										//开始运行
            MCU_YELLOW=0; MCU_RED=0; MCU_GREEN=1;
			delay_ms(10);
			
			motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);
			motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);
			motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);
			motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);
			motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);
			motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);
			motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);
			motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);
			motor_S();
			
			delay_ms(1000);
            while(1)
            {
                ////////////////////////////////////////
                ////////////////////////////////////////
                /**************锡块重量监测************/
                weigh();											//称重
                delay_ms(10);
                if(w<=0.2)											//判断是否<=1KG
                {

                    while(1)  //检查位置
                    {
                        if(IR_UP==0)
                        {
                            delay_ms(10);
                            if(IR_UP==0)
                            {
                                break;
                            }
                        }
                        else
						{
							delay_ms(10000);
                            while(motor_R(1000,1))
							{
							}
							break;
						}
							
                    }

                    motor_S();
                    delay_ms(10);
                    while(MCU_start!=0) 								//监测开始运行开关是否触发
                    {
                        delay_ms(1);
                        while(MCU_start!=0) 								//监测开始运行开关是否触发
                        {
                            MCU_YELLOW = 1; MCU_RED=0; MCU_GREEN = 0;
                        }
                    }
                    MCU_YELLOW=0; MCU_RED=0; MCU_GREEN=1;
					motor_S();
					delay_ms(10);
					
					motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
					motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
					motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
					motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
					motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
					motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
					motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
					motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
					motor_S();
					delay_ms(1000);
                }
                ////////////////////////////////////////
                ////////////////////////////////////////
                /**************锡位监测模块************/

                if(TBA_MCU==0 && TBB_MCU==0) 					//锡量达到预警位置，加锡设备可能存在故障
                {
                    motor_S();
                    MCU_RED=0; MCU_YELLOW=1; MCU_GREEN=1;		//黄灯、绿灯亮代表锡量达到探棒A和B处
                }
                else if(TBA_MCU==0 && TBB_MCU==1)				//锡量正常，电机需停止转动
                {
                    motor_S();
                    MCU_RED=0; MCU_YELLOW=0; MCU_GREEN=1;
                }
                else if(TBA_MCU==1 && TBB_MCU==0) 				//锡量达到预警位置，且探棒A异常
                {
                    motor_S();
                    MCU_RED=1; MCU_YELLOW=1; MCU_GREEN=0;		//红灯、黄灯亮代表探棒A异常
                }

                else if(TBA_MCU==1 && TBB_MCU==1) 				//锡量过低，需要加锡
                {
                    MCU_GREEN = 1; MCU_RED=0; MCU_YELLOW=0;
                    motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
										motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
										motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
										motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
										motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
										motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
										motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
										motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
										motor_S();
										delay_ms(10);

					if(IR_DOWN==0)							//若IR_UP检测到物体时跳出当前循环
					{
						delay_ms(10);
						if(IR_DOWN==0)
						{
							while(motor_R(1000,1))				//若IR_UP检测不到物体时电机保持向上转动，直到检测到物体跳出循环
							{
							}
							motor_S();
							delay_ms(10);
							while(MCU_start!=0)
							{
								delay_ms(1);
								while(MCU_start!=0);
							}
						
						
							
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_S();
						}
					}							
				
					
                    weigh();                    delay_ms(10);//时间短了程序会卡死
                    //sprintf(iTemp_Buf,"CMD,OFF%dFAILH,LINED-L3,END",w);
                    Str_DoubleToStr(iTemp_Buf,w,2);
                    sprintf(iDataBuf,"TIN_WEIGHT,%s,LINED-L3,END",iTemp_Buf);

                    if(w<=0.2)
                    {

                        while(1)
                        {
                            if(IR_UP==0)
                            {
                                delay_ms(10);
                                if(IR_UP==0)
                                {

                                    break;
                                }
                            }
                            else
                                while(motor_R(1000,1))
								{
								}
								break;
                        }
                        motor_S();
                        delay_ms(10);
                        while(MCU_start!=0)
                        {
                            delay_ms(1);
                            while(MCU_start!=0)
                            {
                                MCU_YELLOW = 1; MCU_RED=0; MCU_GREEN=0;
                            }
                        }
                        MCU_YELLOW=0; MCU_RED=0; MCU_GREEN=1;  
						delay_ms(10);
						
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_S();
					}
					delay_ms(10);
                    motor_S();
                    delay_ms(1000); delay_ms(1000); delay_ms(1000); delay_ms(1000); delay_ms(1000); delay_ms(1000);	//电机停止等待锡块融化时间
                    WIFI_SendData(iDataBuf,"SEND OK",2000);
                    while(Cmd_SendErrorFlag)
                    {

                    }
                }

                motor_S();
            }


        }

    }
}



