/*
**************************************************************
*
* 文件名称：mpu6050.h;
*
* 版本说明：V.10；
*
* 运行环境：(1) STM32F103RCT;
*           (2) 外部晶振8M9倍频；
*           (3) MDK 5.0；
*
* 修改说明：暂无；
*           
* 完成日期：2016/04/24;
*
* 作者姓名：郑子烁;
*
* 其他备注：mpu6050读取实验，模仿正点原子例程；
*
**************************************************************
*/
#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "sys.h"
#include "delay.h"
#include "mpu_iic.h"
#include "inv_mpu.h"                       
#include "inv_mpu_dmp_motion_driver.h"  
#include "math.h"

/**********************寄存器定义*************************/
#define MPU_SELF_TESTX_REG		0X0D	//自检寄存器X
#define MPU_SELF_TESTY_REG		0X0E	//自检寄存器Y
#define MPU_SELF_TESTZ_REG		0X0F	//自检寄存器Z
#define MPU_SELF_TESTA_REG		0X10	//自检寄存器A
#define MPU_SAMPLE_RATE_REG		0X19	//采样频率分频器
#define MPU_CFG_REG				0X1A	//配置寄存器
#define MPU_GYRO_CFG_REG		0X1B	//陀螺仪配置寄存器
#define MPU_ACCEL_CFG_REG		0X1C	//加速度计配置寄存器
#define MPU_MOTION_DET_REG		0X1F	//运动检测阀值设置寄存器
#define MPU_FIFO_EN_REG			0X23	//FIFO使能寄存器
#define MPU_I2CMST_CTRL_REG		0X24	//IIC主机控制寄存器
#define MPU_I2CSLV0_ADDR_REG	0X25	//IIC从机0器件地址寄存器
#define MPU_I2CSLV0_REG			0X26	//IIC从机0数据地址寄存器
#define MPU_I2CSLV0_CTRL_REG	0X27	//IIC从机0控制寄存器
#define MPU_I2CSLV1_ADDR_REG	0X28	//IIC从机1器件地址寄存器
#define MPU_I2CSLV1_REG			0X29	//IIC从机1数据地址寄存器
#define MPU_I2CSLV1_CTRL_REG	0X2A	//IIC从机1控制寄存器
#define MPU_I2CSLV2_ADDR_REG	0X2B	//IIC从机2器件地址寄存器
#define MPU_I2CSLV2_REG			0X2C	//IIC从机2数据地址寄存器
#define MPU_I2CSLV2_CTRL_REG	0X2D	//IIC从机2控制寄存器
#define MPU_I2CSLV3_ADDR_REG	0X2E	//IIC从机3器件地址寄存器
#define MPU_I2CSLV3_REG			0X2F	//IIC从机3数据地址寄存器
#define MPU_I2CSLV3_CTRL_REG	0X30	//IIC从机3控制寄存器
#define MPU_I2CSLV4_ADDR_REG	0X31	//IIC从机4器件地址寄存器
#define MPU_I2CSLV4_REG			0X32	//IIC从机4数据地址寄存器
#define MPU_I2CSLV4_DO_REG		0X33	//IIC从机4写数据寄存器
#define MPU_I2CSLV4_CTRL_REG	0X34	//IIC从机4控制寄存器
#define MPU_I2CSLV4_DI_REG		0X35	//IIC从机4读数据寄存器

#define MPU_I2CMST_STA_REG		0X36	//IIC主机状态寄存器
#define MPU_INTBP_CFG_REG		0X37	//中断/旁路设置寄存器
#define MPU_INT_EN_REG			0X38	//中断使能寄存器
#define MPU_INT_STA_REG			0X3A	//中断状态寄存器

#define MPU_ACCEL_XOUTH_REG		0X3B	//加速度值,X轴高8位寄存器
#define MPU_ACCEL_XOUTL_REG		0X3C	//加速度值,X轴低8位寄存器
#define MPU_ACCEL_YOUTH_REG		0X3D	//加速度值,Y轴高8位寄存器
#define MPU_ACCEL_YOUTL_REG		0X3E	//加速度值,Y轴低8位寄存器
#define MPU_ACCEL_ZOUTH_REG		0X3F	//加速度值,Z轴高8位寄存器
#define MPU_ACCEL_ZOUTL_REG		0X40	//加速度值,Z轴低8位寄存器

#define MPU_TEMP_OUTH_REG		0X41	//温度值高八位寄存器
#define MPU_TEMP_OUTL_REG		0X42	//温度值低8位寄存器

#define MPU_GYRO_XOUTH_REG		0X43	//陀螺仪值,X轴高8位寄存器
#define MPU_GYRO_XOUTL_REG		0X44	//陀螺仪值,X轴低8位寄存器
#define MPU_GYRO_YOUTH_REG		0X45	//陀螺仪值,Y轴高8位寄存器
#define MPU_GYRO_YOUTL_REG		0X46	//陀螺仪值,Y轴低8位寄存器
#define MPU_GYRO_ZOUTH_REG		0X47	//陀螺仪值,Z轴高8位寄存器
#define MPU_GYRO_ZOUTL_REG		0X48	//陀螺仪值,Z轴低8位寄存器

#define MPU_I2CSLV0_DO_REG		0X63	//IIC从机0数据寄存器
#define MPU_I2CSLV1_DO_REG		0X64	//IIC从机1数据寄存器
#define MPU_I2CSLV2_DO_REG		0X65	//IIC从机2数据寄存器
#define MPU_I2CSLV3_DO_REG		0X66	//IIC从机3数据寄存器
#define MPU_I2CMST_DELAY_REG	0X67	//IIC主机延时管理寄存器
#define MPU_SIGPATH_RST_REG		0X68	//信号通道复位寄存器
#define MPU_MDETECT_CTRL_REG	0X69	//运动检测控制寄存器
#define MPU_USER_CTRL_REG		0X6A	//用户控制寄存器

#define MPU_PWR_MGMT1_REG       0X6B	//电源管理寄存器1；
#define MPU_PWR_MGMT2_REG       0X6C	//电源管理寄存器1；
#define MPU6050_ADDR            0x75    //我是谁,这个寄存器用于标识设备的身份；

#define MPU_ACCEL_OFFS_REG      0X06    //accel_offs寄存器,可读取版本号,寄存器手册未提到
#define MPU_PROD_ID_REG         0X0C	//prod id寄存器,在寄存器手册未提到

#define MPU_ADDR                0x68    //6050地址(接地)；
#define MPU_READ                0XD1    //读操作(左移)；
#define MPU_WRITE               0XD0    //写操作；



/*
* 函数名称：MPU6050_Init；
* 功能说明：mpu6050初始化；
* 入口参数：无；
* 返回值  ：无；
* 备注    ：无；
*/	
u8 MPU6050_Init(void);


/*
* 函数名称：MPU_Write_Reg；
* 功能说明：写一个字节到寄存器；
* 入口参数：addr->寄存器地，data->发送的数据；
* 返回值  ：无；
* 备注    ：无；
*/
u8 MPU_Write_Reg(u8 reg,u8 data);


/*
* 函数名称：MPU_Read_Reg；
* 功能说明：读寄存器一个字节数据；
* 入口参数：addr->寄存器地；
* 返回值  ：data；
* 备注    ：无；
*/
u8 MPU_Read_Reg(u8 reg);


/*
* 函数名称：MPU_Write_Len；
* 功能说明：连续写寄存器；
* 入口参数：reg->寄存器地，buf->发送的数据；
* 返回值  ：0->:成功，其他->:失败；
* 备注    ：无；
*/
u8 MPU_Write_Len(u8 mpu_addr,u8 reg,u8 len,u8* buf);


/*
* 函数名称：MPU_Read_Len；
* 功能说明：连续读寄存器；
* 入口参数：reg->寄存器，buf->数据接收区；
* 返回值  ：0->:成功，其他->:失败；；
* 备注    ：无；
*/
u8 MPU_Read_Len(u8 mpu_addr,u8 reg,u8 len,u8* buf);


/*
* 函数名称：MPU_Read_Temperature；
* 功能说明：读取温度；
* 入口参数：无；
* 返回值  ：温度值(扩大了100倍)；
* 备注    ：无；
*/
short MPU_Read_Temperature(void);


/*
* 函数名称：MPU_Read_Accel；
* 功能说明：读取加速度原始数据；
* 入口参数：加速度存储区；
* 返回值  ：0->成功，其他->：失败；
* 备注    ：无；
*/
u8 MPU_Read_Accel(short *ax,short *ay,short *az);


/*
* 函数名称：MPU_Read_Gyros；
* 功能说明：读取陀螺仪原始数据；
* 入口参数：角速度存储区；
* 返回值  ：0->成功，其他->：失败；
* 备注    ：无；
*/
u8 MPU_Read_Gyros(short *gx,short *gy,short *gz);


/*
* 函数名称：MPU_Set_Gyros_Fsr；
* 功能说明：设置陀螺仪满量程；
* 入口参数：0,1,2,3；
* 返回值  ：0->成功，其他->：失败；
* 备注    ：无；
*/
u8 MPU_Set_Gyros_Fsr(u8 fs_sel);


/*
* 函数名称：MPU_Set_Accel_Fsr；
* 功能说明：设置加速度计满量程；
* 入口参数：0,1,2,3；
* 返回值  ：0->成功，其他->：失败；
* 备注    ：无；
*/
u8 MPU_Set_Accel_Fsr(u8 afs_sel);


/*
* 函数名称：MPU_Set_Configuration；
* 功能说明：设置数字滤波器；
* 入口参数：0--7Hz；
* 返回值  ：0->成功，其他->：失败；
* 备注    ：根据公式转换为分频系数；
*/
u8 MPU_Set_Configuration(u8 lpf);


/*
* 函数名称：MPU_Set_Rate；
* 功能说明：设置采样速率；
* 入口参数：4--1000(Hz)；
* 返回值  ：0->成功，其他->：失败；
* 备注    ：根据公式转换为分频系数；
*/
u8 MPU_Set_Rate(u16 rate);

//翻腕检测
void Action_Task(void);
//摔倒检测
void Check_Fall(void);

#endif 

