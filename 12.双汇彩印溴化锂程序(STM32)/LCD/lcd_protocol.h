#ifndef _LCD_PROTOCOL_H
#define _LCD_PROTOCOL_H

#ifndef _LCD_PROTOCOL_C
#define EXT_LCD_PROTOCOL extern
#else
#define EXT_LCD_PROTOCOL
#endif



//
#define LCD_PROTOCOL_FRAME_MIN_LEN		8
#define SEND_RETRY_TIMES				3

#define SEQ_YC_CIRCLE_TIME				20		//400ms
#define FRAME_UNACK_TIMEOVER_TIME		50		//500ms
//----------------------------------------------------------------
//需要发送帧类型标志 UINT32 seq_state	,低字的标志不需要循环发送，高字的标志需要循环发送。

//	
#define STA_SEQ_DEVICE_TYPE_FLAG		0x00000001		//设备类型查询	
#define STA_SEQ_EVENT_CALL_FLAG		 	0x00000002		//事件记录召唤
#define STA_SEQ_ADDR_FLAG				0x00000004		//地址查询
#define STA_SEQ_COMM_FLAG				0x00000008		//通讯参数查询


#define STA_SEQ_PWD_CALL_FLAG           0x00000010		//密码召唤
#define STA_SAVE_PWD_FLAG	            0x00000020		//密码设置
#define STA_SEQ_DISP_FLAG				0x00000040		//召唤显示参数(包括背光延时及显示一次或二次)
#define STA_SAVE_COMM_FLAG				0x00000080		//通讯参数固化

#define STA_SAVE_ADDR_FLAG				0x00000100		//地址固化
#define STA_SAVE_TIME_FLAG				0x00000200		//时间固化
#define STA_SAVE_DISP_FLAG				0x00000400		//显示参数固化
#define STA_SEQ_FIX_VALUE_FLAG			0x00000800		//定值查询帧

#define STA_SAVE_FIX_VALUE_FLAG			0x00001000		//定值固化帧
#define STA_SEQ_KI_PARA_FLAG			0x00002000		//召开入参数
#define STA_SAVE_KI_PARA_FLAG			0x00004000		//开入参数固化
#define STA_SEQ_ADJ_FLAG				0x00008000		//召修正系数

#define STA_SAVE_ADJ_FLAG				0x00010000		//保存修正系数
#define STA_SEQ_SOE_FLAG				0x00020000		//召SOE
#define STA_SET_SIGNAL_RST_FLAG			0x00040000		//信号复归
#define STA_SEQ_DEV_INFO_FLAG			0x00080000		//召唤装置说明

#define STA_SEQ_ACK_FLAG				0x40000000		//发送确认帧
#define STA_SEQ_REAL_DATA_FLAG			0x80000000 		//实时数据查询

//---------------------------------------------------------------
//发送帧类型定义(上传或下发的帧均使用此处定义类型)
//
#define FRAME_TYPE_SEQ_DEVICE		1	//设备类型
#define FRAME_TYPE_SEQ_YC			2	//实时数据
#define FRAME_TYPE_SEQ_ADDR			3	//地址查询
#define FRAME_TYPE_SET_TIME			4	//设置时间
#define FRAME_TYPE_SIGNAL_RST		5	//信号复归
#define FRAME_TYPE_SEQ_FIX			6	//定值查询,			上传定值(上行)
#define FRAME_TYPE_SET_FIX			7	//定值设置
#define FRAME_TYPE_SEQ_PWD			8	//密码查询
#define FRAME_TYPE_SET_PWD			9	//密码设置
#define FRAME_TYPE_SET_YK			10	//遥控命令
#define FRAME_TYPE_SEQ_EVENT		11	//召SOE
#define FRAME_TYPE_SEQ_DISP			12	//显示参数
#define FRAME_TYPE_SEQ_COMM			13	//通讯参数查询,		上传通讯参数(上行)
#define FRAME_TYPE_SET_COMM			14	//通讯参数固化
#define FRAME_TYPE_SET_ADDR			15	//地址设置
#define FRAME_TYPE_SET_DISP			16	//显示参数固化
#define FRAME_TYPE_SEQ_KI			17	//开入参数查询
#define FRAME_TYPE_SET_KI			18	//开入参数固化
#define FRAME_TYPE_SEQ_ADJ			19	//修正系数查询
#define FRAME_TYPE_SET_ADJ			20	//修正系数固化
#define FRAME_TYPE_SEQ_DEV_INFO		21	//召唤装置说明
//
#define FRAME_TYPE_TIME_UP			22	//时间上传
#define FRAME_TYPE_LIGHT_UP			23	//灯显上传
#define FRAME_TYPE_SOE_UP			24	//事件请求上传
//
#define FRAME_TYPE_ACK				25	//确认帧

//参数类型定义
#define PARA_TYPE_DEVICE			0X20	//装置参数
#define PARA_TYPE_ADDRESS			0X21	//地址参数
#define PARA_TYPE_JIANKONG		   0X22	//监控通讯口参数
#define PARA_TYPE_DISPYX			0X23	//显示遥信参数
#define PARA_TYPE_SOE				0X24	//SOE参数
#define PARA_TYPE_YC				0x25	//遥测参数
#define PARA_TYPE_GPRS			    0x26	//GPRS通信参数
#define PARA_TYPE_SENSOR			0x27	//传感器通信参数
#define PARA_TYPE_FIXED				0x28	//定值参数
#define PARA_TYPE_YK				0x29	//遥控参数
#define PARA_TYPE_WORK				0x2a	//工作参数
#define PARA_TYPE_MODULPARA			0X2B	//修正系数参数
#define PARA_TYPE_YX				0X2C	//遥信参数
#define PARA_TYPE_MODUL				0x2D	//修正系数

//----------------------------------------------------------------
//何种数据类型占用公共数据区定义 share_buf_use_type
#define SHARE_BUF_TYPE_REAL_DATA	0x01			//实时数据
#define SHARE_BUF_TYPE_EVENT		0x02			//事件记录
#define SHARE_BUF_TYPE_COMM			0x03			//通讯数据
#define SHARE_BUF_TYPE_KI			0x04			//召唤开入参数
#define SHARE_BUF_ADJ				0x05			//召修正系数
#define SHARE_BUF_FIX				0x06			//定值数据

//----------------------------------------------------------------	
//----------------------------------------------------
//召唤成功的标志 valid_para_flag	(非占用公共缓冲区的数据)
#define ENABLE_TYPE_DEVICE_FLAG		0x00000001		//装置类型召唤完成
#define ENABLE_TYPE_ADDRESS_FLAG	0x00000002		//地址参数召唤完成
#define ENABLE_TYPE_COMMPORT_FLAG	0x00000004		//通讯口参数召唤完成
#define ENABLE_TYPE_DISPYX_FLAG		0x00000008		//显示遥信参数召唤完成
//
#define ENABLE_TYPE_PASSWORD_FLAG	0x00000010		//密码召唤成功

#define ENABLE_TYPE_ACKED_FLAG		0x00000020		//成功收到确认帧

#define ENABLE_DATA_FLAG			0x80000000		//公用缓冲区数据可用标志



//----------------------------------------------------
#define TASK_LCD_PROTOCOL_STK_SIZE		256

#define LCD_PROTOCOL_RECE_BUF_LEN		256
#define LCD_PROTOCOL_SEND_BUF_LEN		256

#define SHARE_BUF_LEN					256			//公共数据缓冲区，所有的召唤上来的数据放此
//

//----------------------------------------------------------------------------
typedef struct 
{
	UINT8 lcd_protocol_rece_buf[LCD_PROTOCOL_RECE_BUF_LEN];
	UINT8 rece_len;
}RECE_BUF;

typedef struct 
{
	UINT8 lcd_protocol_send_buf[LCD_PROTOCOL_SEND_BUF_LEN];
	UINT8 send_len;
}SEND_BUF;

typedef struct 
{	
 	UINT32 rece_byte_num;	//接收总字节计数
 	UINT32 send_byte_num;	//发送总字节计数	
}LCD_CONNECTED_COUNT;

//循环召唤帧时的相应计时变量
typedef struct
{
	UINT16 yc_circle_time;
}STRU_SEQ_CIRCLE_TIME;

//--------------------------------------------------------------------------------
//参数缓冲区定义
//设备类型参数
typedef struct 
{
	UINT8 type;
	UINT8 dev_version; 
}PARA_DEVICE_STRUCT;

//通讯口参数
//显示遥信参数
typedef struct
{
	UINT8 yx_num;
	UINT8 yx_type;
	UINT8 yx_lcdposition;
	UINT8 yx_name;
}DISP_YX_PARA_STRU;
//SOE参数
//遥测参数
typedef struct 
{
	UINT8 type;		//类型
	UINT8 channel;	//路数
	UINT8 format;	//数据格式
	UINT8 unit;		//单位
}PARA_YC_STRU;

//----------------------------------------------------------------------------------

EXT_LCD_PROTOCOL PARA_DEVICE_STRUCT dev_type;			//设备类型
//EXT_LCD_PROTOCOL UINT16  local_addr;						//本机地址
EXT_LCD_PROTOCOL UINT16 led_disp_data;					//灯显数据
EXT_LCD_PROTOCOL UINT8 new_soe_num;						//保护新生成的SOE数，是主动上传数据
														//0则是没有新SOE产生


//
EXT_LCD_PROTOCOL LCD_CONNECTED_COUNT lcd_t_r_count;		//接收发送计数
//变量定义
EXT_LCD_PROTOCOL UINT32 seq_status;						//交互状态，通过此状态，可以知道发送何帧
EXT_LCD_PROTOCOL UINT32 valid_para_flag;				//可以使用的参数类标志
//
EXT_LCD_PROTOCOL UINT16  bkg_delay;						//背光延时
EXT_LCD_PROTOCOL UINT16  yc_disp_mode;					//显示模式，1：一次值，0，二次值。仅用此区分单位


//公共数据缓冲区
//EXT_LCD_PROTOCOL UINT16 share_buf[SHARE_BUF_LEN];
EXT_LCD_PROTOCOL UINT8 share_buf_use_type;				//何类数据占用了公用缓冲区，0则无占用
EXT_LCD_PROTOCOL UINT16 share_buf_data_len;				//公用区可用数据长度
//-----------------------------------------------------

EXT_LCD_PROTOCOL OS_STK TaskLcdProtocolStk[TASK_LCD_PROTOCOL_STK_SIZE];
EXT_LCD_PROTOCOL void InitLcdProtocol(void);
EXT_LCD_PROTOCOL void TaskLcdProtocol(void *);

//
EXT_LCD_PROTOCOL void SetProRunStatus(UINT32 flag);
EXT_LCD_PROTOCOL void ResetProRunStatus(UINT32 flag);
EXT_LCD_PROTOCOL UINT32 GetProRunStatus(void);

EXT_LCD_PROTOCOL void SetParaStatus(UINT32 flag);
EXT_LCD_PROTOCOL void ResetParaStatus(UINT32 falg);
EXT_LCD_PROTOCOL UINT32 GetParaStatus(void);

extern const UINT8 fix_data[];


#endif