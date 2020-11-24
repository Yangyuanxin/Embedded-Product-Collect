/*
**************************************************************
*
* 文件名称：FT6236.c;
*
* 版本说明：V1.0；
*
* 运行环境：(1) STM32F103RCT;
*           (2) 外部晶振8M9倍频；
*           (3) MDK 5.0；
*
* 修改说明：无；
*           
* 完成日期：2016/06/26;
*
* 作者姓名：郑子烁;
*
* 其他备注：触屏芯片FT6236驱动程序；
*           来自厂家例程，参考正点原子FT5206；
*          
* 修改说明：无；
*
**************************************************************
*/

#include "FT6236.h"
#include "main.h"

TouchPointRefTypeDef TPR_Structure; 
u8 Touch_Buf[30];
const u16 FT6236_TPX_TBL[5]=
{
	FT_TP1_REG,
	FT_TP2_REG,
	FT_TP3_REG,
	FT_TP4_REG,
	FT_TP5_REG
};


/*
* 函数名称：FT6236_Init；
* 功能说明：FT6236初始化；
* 入口参数：无；
* 返回值  ：无；
* 备注    ：无；
*/		
void FT6236_Init(void)
{
	u8 temp; 
	GPIO_Clock_Set(IOPBEN);                              //先使能外设IO PORTC时钟 	
	GPIO_Clock_Set(IOPCEN);
	TPR_Structure.Point_Num = 0;		//触摸点数清0
	TPR_Structure.Gesture_Sta = 0;		//触摸点数清0
	GPIO_Init(GPIOC,P12,IO_MODE_OUT,IO_SPEED_50M,GPIO_OTYPE_PP,IO_PULL,IO_H); //PC12 SDA
	GPIO_Init(GPIOB,P3,IO_MODE_OUT,IO_SPEED_50M,GPIO_OTYPE_PP,IO_PULL,IO_H); //PB3 SCL
	GPIO_Init(GPIOC,P11,IO_MODE_OUT,IO_SPEED_50M,GPIO_OTYPE_PP,IO_PULL,IO_H); //PC11 RST
	GPIO_Init(GPIOB,P4,IO_MODE_IN,IO_SPEED_50M,GPIO_OTYPE_PP,IO_PULL,IO_H);	//PB4 ENINT
	FT6236_RST = 0;
	delay_ms(20);
	FT6236_RST = 1;
	delay_ms(60);
	temp=0;  
	FT6236_Write_Reg(FT_DEVIDE_MODE,&temp,1);   //进入正常操作模式 	
 	temp=22;                                      //触摸有效值，22，越小越灵敏	
 	FT6236_Write_Reg(FT_ID_G_THGROUP,&temp,1);  //设置触摸有效值
 	temp=12;                                      //激活周期，不能小于12，最大14
 	FT6236_Write_Reg(FT_ID_G_PERIODACTIVE,&temp,1); 
}


/*
* 函数名称：FT6236_Write_Reg；
* 功能说明：FT6236连续写寄存器；
* 入口参数：reg-->寄存器地址，buf-->发送缓存空间，len-->连续长度；
* 返回值  ：0-->成功，1-->失败；
* 备注    ：无；
*/
u8 FT6236_Write_Reg(u16 reg,u8 *buf,u8 len)
{
	u8 i;
	u8 ret=0;
	FT6236_IIC_Start();
	
	FT6236_Send_Byte(FT_CMD_WR);        //发送写命令；
	FT6236_Wait_Ack();
	
	FT6236_Send_Byte(reg&0XFF);         //发送低8位地址
	FT6236_Wait_Ack();
	
	for(i=0;i<len;i++)
	{
    	FT6236_Send_Byte(buf[i]);       //发数据
		ret=FT6236_Wait_Ack();
		if(ret)break;                   //应答为'1'，失败，立即返回；
	}
    FT6236_IIC_Stop();                  //产生一个停止条件	    
	return ret; 
}


/*
* 函数名称：FT6236_Read_Reg；
* 功能说明：FT6236连续读寄存器；
* 入口参数：reg-->寄存器地址，buf-->接收缓存空间，len-->连续长度；
* 返回值  ：0-->成功，1-->失败；
* 备注    ：无；
*/		  
u8 FT6236_Read_Reg(u16 reg,u8 *buf,u8 len)
{
	u8 i; 
 	FT6236_IIC_Start();	
	
 	FT6236_Send_Byte(FT_CMD_WR);                  //发送写命令 	 
	FT6236_Wait_Ack();		
	
	FT6236_Send_Byte(reg&0XFF);                   //发送低8位地址
	FT6236_Wait_Ack();
	  
 	FT6236_IIC_Start();  
	
	FT6236_Send_Byte(FT_CMD_RD);                  //发送读命令		
	FT6236_Wait_Ack();			

	for(i=0;i<len;i++)
	{	   
		*buf++=FT6236_Read_Byte(i==(len-1)?0:1);  //读入1B数据到接收数据缓冲区中  
	} 
    FT6236_IIC_Stop();                            //产生一个停止条件	  
	return 0;
} 



int fun(u16 a[], int n)
{
    if(n==1)
        return 1;
    if( n==2 )
        return (a[n-1]) >= a[n-2];
    return fun(a,n-1) && ((a[n-1]) >= a[n-2]);
}


int Gesture_Scan(void)
{
	float TanPoint;
	if(fun(TPR_Structure.Point_X, 29))		//X递增
	{
		if(fun(TPR_Structure.Point_Y, 29))	//Y递增
		{
			TanPoint = (float)(TPR_Structure.Point_Y[5]-TPR_Structure.Point_Y[29])/(float)((TPR_Structure.Point_X[5]-TPR_Structure.Point_X[29]));
			
			if(TanPoint>=(float)1.1)		//向下
			{	
				return 3;
			}
			else					//向右
			{
				if(((float)-0.10<=TanPoint) & (TanPoint<= (float)0.6))
				{
					return 2;
				}
				else
				{
					if(TanPoint <= (float)-0.2)
					{
						return 3;
					}
					if(!(TanPoint == TanPoint))
					{
						return 2;
					}
				}
				return 5;
			}
		}
		else						//Y递减
		{
			TanPoint = (-(float)(TPR_Structure.Point_Y[5]-TPR_Structure.Point_Y[29]))/(float)((TPR_Structure.Point_X[5]-TPR_Structure.Point_X[29]));
			if(TanPoint >= (float)1.1)	//向上
			{
				return 1;
			}
			else			//向右
			{
				if(((float)-0.10<=TanPoint) & (TanPoint<= (float)0.6))
				{
					return 2;
				}
				else
				{
					if(TanPoint <= -0.2)
					{
						return 1;
					}
					if(!(TanPoint == TanPoint))
					{
						return 1;
					}
				}
				return 4;
			}
		}
	}
	else				//X递减
	{
		
		if(fun(TPR_Structure.Point_Y, 29))	//Y递增
		{
			TanPoint = (float)((TPR_Structure.Point_Y[5]-TPR_Structure.Point_Y[29]))/(-(float)(TPR_Structure.Point_X[5]-TPR_Structure.Point_X[29]));
			if(TanPoint >= (float)1.1)	//向下
			{
				return 3;
			}
			else			//向左
			{
				if(((float)-0.10<=TanPoint) && (TanPoint<= (float)0.6))
				{
					return 0;
				}
				else
				{
					if(TanPoint <= (float)-0.2)
					{
						return 3;
					}
					if(!(TanPoint == TanPoint))
					{
						return 0;
					}
				}
				return 5;
			}	
		}
		else						//Y递减
		{
			TanPoint = ((float)(TPR_Structure.Point_Y[5]-TPR_Structure.Point_Y[29]))/((float)(TPR_Structure.Point_X[5]-TPR_Structure.Point_X[29]));
			if((TanPoint) >= (float)(1.1))	//向上
			{
				return 1;
			}
			else			//向左
			{
				if(((float)-0.10<=TanPoint) && (TanPoint<= (float)0.6))
				{
					return 0;
				}
				else
				{
					if(TanPoint <= -0.2)
					{
						return 1;
					}
					if(!(TanPoint == TanPoint))
					{
						return 0;
					}
				}
				return 5;
			}
		}
	}
}


/*
* 函数名称：FT6236_Scan；
* 功能说明：触屏扫描；
* 入口参数：无；
* 返回值  ：无；
* 备注    ：无；
*/	
void FT6236_Scan(void)
{
	u8 sta = 0;	
	u8 buf[4] = {0}; 	
	FT6236_Read_Reg(0x02,&sta,1);                           //读取触摸点的状态寄存器；  	   
 	if(sta&0x0f)                                            //判断是否有触摸点按下，0x02寄存器的低4位表示有效触点个数(0~5)；
 	{

		TPR_Structure.TouchSta=~(0xFF<<(sta&0x0F));         //~(0xFF << (sta & 0x0F))将点的个数转换为触摸点按下有效标志
 		if(TPR_Structure.TouchSta&(1<<0))               //读取触摸点坐标
 		{											    //被按下则读取对应触摸点坐标数据
			FT6236_Read_Reg(FT6236_TPX_TBL[0],buf,4);	//读取XY坐标值
			if((buf[0]&0XC0)!=0X80)
 			{
				TPR_Structure.x[0]=TPR_Structure.y[0]=0xffff;	
				TPR_Structure.Point_Num = 0;		//清0
				return;
			}
			TPR_Structure.LastX_Point=TPR_Structure.x[0];
			TPR_Structure.LastY_Point=TPR_Structure.y[0];
			TPR_Structure.x[0]=((u16)(buf[0]&0X0F)<<8)|buf[1];		
			TPR_Structure.y[0]=((u16)(buf[2]&0X0F)<<8)|buf[3];
 			if((TPR_Structure.Gesture_Sta&(1<<7)) == 0)
			{
				TPR_Structure.Point_X[TPR_Structure.Point_Num]= TPR_Structure.x[0];	//存储X点
				TPR_Structure.Point_Y[TPR_Structure.Point_Num]= TPR_Structure.y[0];	//存储Y点
				TPR_Structure.Point_Num++;		//点数+1
			}
 		}
 		TPR_Structure.TouchSta |= TP_PRES_DOWN;             //触摸按下标记
		if(TPR_Structure.Point_Num == 30)
		{
			TPR_Structure.Gesture_Sta |= (1<<7);			//手势发生标志位
			TPR_Structure.Point_Num = 0;					//点数清0
		}
	}
 	else
 	{
 		if(TPR_Structure.TouchSta &TP_PRES_DOWN) 	        //之前是被按下的 第一次松开会进来这里
 		{
			TPR_Structure.TouchSta &= ~0x80;                //触摸松开标记
			if((TPR_Structure.Gesture_Sta&(1<<7)) != 0)
			{	
				TPR_Structure.Gesture_Sta |= (1<<6);			//开始手势判断标志位
			}
			TPR_Structure.Point_Num = 0;
		}
 		else
 		{
			TPR_Structure.x[0]=TPR_Structure.y[0]=0xffff;
 			TPR_Structure.TouchSta &=0xe0;	                //清除触摸点有效标记
 		}
 	}
}


