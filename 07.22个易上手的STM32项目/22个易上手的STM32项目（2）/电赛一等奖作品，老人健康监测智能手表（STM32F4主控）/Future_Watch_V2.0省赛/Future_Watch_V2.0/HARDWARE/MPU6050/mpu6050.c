///*
//**************************************************************
//*
//* 文件名称：mpu6050.c;
//*
//* 版本说明：V.10；
//*
//* 运行环境：(1) STM32F103RCT;
//*           (2) 外部晶振8M9倍频；
//*           (3) MDK 5.0；
//*
//* 修改说明：暂无；
//*           
//* 完成日期：2016/04/24;
//*
//* 作者姓名：郑子烁;
//*
//* 其他备注：mpu6050读取实验，模仿正点原子例程；
//*
//**************************************************************
//*/

#include "mpu6050.h"
#include "main.h"

extern SYSTEM_STA System_Sta;	
extern float Pitch;       //pitch(在inv_mpu.c里定义)
extern float Roll;        //roll
extern float Yaw;         //float

//u8 unlock = 0;            //6050解锁标志，1->检测到翻腕，0->没有；



short a_x,a_y,a_z;             //加速度值暂存单元；
int acc;                       //合加速度；
int sin_temp;                //空间正弦值；

u8 MPU6050_Init(void)
{
	u8 res=0;
	MPU_IIC_Init();                               //端口初始化；
	delay_ms(1000);
	//GPIO_Init(GPIOB,P12,IN_MODE,PULL,PULL_UP);    //PA2为上拉输入；
	//JTAG_Set(SWD_ENABLE);	                      //禁止JTAG,从而PA15可以做普通IO使用,否则PA15不能做普通IO!!!
	//
	res = MPU_Write_Reg(MPU_PWR_MGMT1_REG,0X80);	      //复位MPU6050；
	res = MPU_Write_Reg(MPU_PWR_MGMT1_REG,0X80);	      //复位MPU6050；
	res = MPU_Read_Reg(MPU_PWR_MGMT1_REG);
    delay_ms(5000);
	res = MPU_Write_Reg(MPU_PWR_MGMT1_REG,0X01);	      //唤醒MPU6050 ；
	res =MPU_Read_Reg(MPU_PWR_MGMT1_REG);
	delay_ms(1000);
	MPU_Set_Gyros_Fsr(3);					      //陀螺仪传感器,±2000dps
	MPU_Set_Accel_Fsr(3);					      //加速度传感器,±2g
	MPU_Set_Rate(50);						      //设置采样率50Hz
	MPU_Write_Reg(MPU_INT_EN_REG,0X00);	          //关闭所有中断
	MPU_Write_Reg(MPU_USER_CTRL_REG,0X00);	      //I2C主模式关闭
	MPU_Write_Reg(MPU_FIFO_EN_REG,0X00);	      //关闭FIFO
	MPU_Write_Reg(MPU_INTBP_CFG_REG,0X80);	      //INT引脚低电平有效
	res=MPU_Read_Reg(MPU6050_ADDR);
	if(res==MPU_ADDR)                             //器件ID正确
	{
		res=MPU_Write_Reg(MPU_PWR_MGMT1_REG,0X01);    //设置CLKSEL,PLL X轴为参考
		MPU_Write_Reg(MPU_PWR_MGMT2_REG,0X00);	  //加速度与陀螺仪都工作
		MPU_Set_Rate(50);						  //设置采样率为50Hz
 	}
	else return 1;                                //出错；
	res =MPU_Read_Reg(MPU_PWR_MGMT1_REG);
	//MPU_Write_Reg(MPU_PWR_MGMT1_REG,0X01);
	return 0;                                     //初始化成功；
}


/*
* 函数名称：MPU_Write_Reg；
* 功能说明：写一个字节到寄存器；
* 入口参数：reg->寄存器地，data->发送的数据；
* 返回值  ：无；
* 备注    ：无；
*/
u8 MPU_Write_Reg(u8 reg,u8 data)
{
	MPU_IIC_Start();
	MPU_IIC_Send_Byte(MPU_WRITE);
	if(MPU_IIC_Wait_Ack())
	{
		MPU_IIC_Stop();
		return 1;
	}
	MPU_IIC_Send_Byte(reg);
	if(MPU_IIC_Wait_Ack())
	{
		MPU_IIC_Stop();
		return 2;
	}
	MPU_IIC_Send_Byte(data);
	MPU_IIC_Wait_Ack();	
	MPU_IIC_Stop();
	return 0;
}


/*
* 函数名称：MPU_Read_Reg；
* 功能说明：读寄存器一个字节数据；
* 入口参数：Reg->寄存器地；
* 返回值  ：data；
* 备注    ：无；
*/
u8 MPU_Read_Reg(u8 reg)
{
	u8 data;
	MPU_IIC_Start();
	MPU_IIC_Send_Byte(MPU_WRITE);
	MPU_IIC_Wait_Ack();
	MPU_IIC_Send_Byte(reg);
	MPU_IIC_Wait_Ack();
	MPU_IIC_Start();
	MPU_IIC_Send_Byte(MPU_READ);
	MPU_IIC_Wait_Ack();
	data=MPU_IIC_Read_Byte(0);
	MPU_IIC_Stop();
	return data;
}


/*
* 函数名称：MPU_Write_Len；
* 功能说明：连续写寄存器；
* 入口参数：reg->寄存器地，buf->发送的数据；
* 返回值  ：0->:成功，其他->:失败；
* 备注    ：无；
*/
u8 MPU_Write_Len(u8 mpu_addr,u8 reg,u8 len,u8* buf)
{
	u8 i;
	MPU_IIC_Start();
	MPU_IIC_Send_Byte(mpu_addr<<1);
	if(MPU_IIC_Wait_Ack())
	{
		MPU_IIC_Stop();
		return 1;
	}
	MPU_IIC_Send_Byte(reg);
	MPU_IIC_Wait_Ack();
	for(i=0;i<len;i++)
	{
		MPU_IIC_Send_Byte(buf[i]);
		if(MPU_IIC_Wait_Ack())
		{
			MPU_IIC_Stop();
			return 0xff;
		}			
	}
	MPU_IIC_Stop();
	return 0;
}


/*
* 函数名称：MPU_Read_Len；
* 功能说明：连续读寄存器；
* 入口参数：reg->寄存器，buf->数据接收区；
* 返回值  ：0->:成功，其他->:失败；；
* 备注    ：无；
*/
u8 MPU_Read_Len(u8 mpu_addr,u8 reg,u8 len,u8* buf)
{
	u8 i;
	MPU_IIC_Start();
	MPU_IIC_Send_Byte(mpu_addr<<1);
	MPU_IIC_Wait_Ack();
	MPU_IIC_Send_Byte(reg);
	MPU_IIC_Wait_Ack();
	
	MPU_IIC_Start();
	MPU_IIC_Send_Byte((mpu_addr<<1)|1);
	MPU_IIC_Wait_Ack();  
	for(i=0;i<len-1;i++)
	{
		buf[i]=MPU_IIC_Read_Byte(1);       //发送ACK；
	}
	buf[i]=MPU_IIC_Read_Byte(0);           //发送NACK；
	MPU_IIC_Stop();
	return 0;
}


/*
* 函数名称：MPU_Read_Temperature；
* 功能说明：读取温度；
* 入口参数：无；
* 返回值  ：温度值(扩大了100倍)；
* 备注    ：无；
*/
short MPU_Read_Temperature(void)
{
    u8 buf[2]; 
    short raw;
	float temp;
	MPU_Read_Len(MPU_ADDR,MPU_TEMP_OUTH_REG,2,buf); 
    raw=((u16)buf[0]<<8)|buf[1];  
    temp=36.53+((double)raw)/340;  
    return temp*100;;
}


/*
* 函数名称：MPU_Read_Accel；
* 功能说明：读取加速度原始数据；
* 入口参数：加速度存储区；
* 返回值  ：0->成功，其他->：失败；
* 备注    ：无；
*/
u8 MPU_Read_Accel(short *ax,short *ay,short *az)
{
	u8 read,buf[6];
	read=MPU_Read_Len(MPU_ADDR,MPU_ACCEL_XOUTH_REG,6,buf);
	if(read==0)
	{
		*ax=((u16)buf[0]<<8)|buf[1];
		*ay=((u16)buf[2]<<8)|buf[3];
		*az=((u16)buf[4]<<8)|buf[5];
	}
	return read;
}


/*
* 函数名称：MPU_Read_Gyros；
* 功能说明：读取陀螺仪原始数据；
* 入口参数：角速度存储区；
* 返回值  ：0->成功，其他->：失败；
* 备注    ：无；
*/
u8 MPU_Read_Gyros(short *gx,short *gy,short *gz)
{
	u8 read,buf[6];
	read=MPU_Read_Len(MPU_ADDR,MPU_GYRO_XOUTH_REG,6,buf);
	if(read==0)
	{
		*gx=((u16)buf[0]<<8)|buf[1];
		*gy=((u16)buf[2]<<8)|buf[3];
		*gz=((u16)buf[4]<<8)|buf[5];
	}
	return read;
}


/*
* 函数名称：MPU_Set_Gyros_Fsr；
* 功能说明：设置陀螺仪满量程；
* 入口参数：0,1,2,3；
* 返回值  ：0->成功，其他->：失败；
* 备注    ：无；
*/
u8 MPU_Set_Gyros_Fsr(u8 fs_sel)
{
	return MPU_Write_Reg(MPU_GYRO_CFG_REG,fs_sel<<3);
}


/*
* 函数名称：MPU_Set_Accel_Fsr；
* 功能说明：设置加速度计满量程；
* 入口参数：0,1,2,3；
* 返回值  ：0->成功，其他->：失败；
* 备注    ：无；
*/
u8 MPU_Set_Accel_Fsr(u8 afs_sel)
{
	return MPU_Write_Reg(MPU_ACCEL_CFG_REG,afs_sel<<3);
}


/*
* 函数名称：MPU_Set_Configuration；
* 功能说明：设置数字滤波器；
* 入口参数：0--7Hz；
* 返回值  ：0->成功，其他->：失败；
* 备注    ：根据公式转换为分频系数；
*/
u8 MPU_Set_Configuration(u8 lpf)
{
	u8 data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return MPU_Write_Reg(MPU_CFG_REG,data);//设置数字低通滤波器  
}


/*
* 函数名称：MPU_Set_Rate；
* 功能说明：设置采样速率；
* 入口参数：4--1000(Hz)；
* 返回值  ：0->成功，其他->：失败；
* 备注    ：根据公式转换为分频系数；
*/
u8 MPU_Set_Rate(u16 rate)
{
	u16 div;
	if(rate>1000)                //限制速率；
	{rate=1000;}
	if(rate<4)
	{rate=4;}
	div=1000/rate-1;             //采样频率=陀螺仪输出频率/（1+SMPLRT_DIV）
	MPU_Write_Reg(MPU_SAMPLE_RATE_REG,div);
	return MPU_Set_Configuration(rate/2);
}


/*
* 函数名称：Action_Task；
* 功能说明：判断翻腕手势；
* 入口参数：无；
* 返回值  ：无；
* 备注    ：无；
*/
void Action_Task(void)
{
	static u8 cont_flag = 0;           //bit0:是否进行翻腕判断标志；bit1:Roll达到90°标志；bit2:完成翻腕动作标志；
	static u8 cont = 0;                
	static int last_data = 0;
	int my_angle;
//	while(mpu_dmp_get_data()!=0);
	if(mpu_dmp_get_data()==0)
	{
		my_angle = Pitch;
		if(((cont_flag&0x01)==1)||(my_angle>-10&&my_angle<10))   //判断小臂是否大概处于水平状态，如果是则进行手势判断；
		{
			cont_flag |= 1<<0;                               //检测到符合情况的状态，标志置1，表示接下来进行连续判断；
			if((cont_flag&(1<<1))==0)                        //判断是否还没翻过设定值；
			{
				if(my_angle<last_data)
				{	
					cont = 0;
					last_data = my_angle; 
					if(my_angle<-70)
					{
						cont_flag |= 1<<1;                    //翻过90°，标志cont_flag[1];
						cont = 0;
					}
				}
				else if(my_angle==last_data)
				{
					cont++;
					if(cont==50)
					{
						cont = 0;
						cont_flag = 0;                       //动作错误，此次判断无效；
					}
				}
			}
			else
			{		
				if(my_angle<=-70)
				{
					cont++;     
					if(cont>50)                            //翻过设定角度后停留超时；
					{
						cont = 0;
						cont_flag = 0;                     //动作错误，此次判断无效；
					}
				}
				else if(my_angle>last_data)
				{
					last_data = my_angle;                            
					if(my_angle>-10)
					{
						System_Sta.unlock = 1;                        //解锁成功；
						cont_flag |= 1<<2;                 //标志cont_flag[2];
						cont = 0;
						cont_flag &= ~(3<<0);
					}
				}
			}
		}
	}
}


void Check_Fall(void)
{
	static u8 time=0,j=0,flag=0;
	float buf;
	MPU_Read_Accel(&a_x,&a_y,&a_z);     //获取加速度；
	acc=sqrt((a_x*a_x+a_y*a_y+a_z*a_z));  //得到合加速度；
	if(acc>15000){flag=1;}              //捕获到可疑信息；
	if(flag)                            //允许连续取值；
	{
		if(++time==200)                 //判断是否到达2s，如果是，再连续取值20次求平均；
		{
			if(++j<20)
			{
				arm_sqrt_f32(a_y*a_y+a_z*a_z,&buf);
				sin_temp+=(int)(buf/acc*5000); //得到空间正弦值，放大了5000倍(0~5000)；
				time=199;
			}
			else
			{
				sin_temp/=20;           //得到20次角度采样的平均值；
				j=0;                    //计数清零；
				time=0;
				flag=0;
				if(sin_temp>4600)
				{System_Sta.Tumble_Flag|=(1<<7);sin_temp=0;}         //摔倒，清除角度；
				else
				{System_Sta.Tumble_Flag&=~(1<<7);}        //没摔倒；
			}
		}
	}
}

