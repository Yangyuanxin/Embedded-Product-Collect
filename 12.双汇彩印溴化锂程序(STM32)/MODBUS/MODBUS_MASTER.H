
#ifndef _MODBUS_MASTER_H
#define _MODBUS_MASTER_H

#ifndef _MODBUS_MASTER_C
#define EXT_MB_MASTER extern
#else
#define EXT_MB_MASTER
#endif

#define MODBUS_MST_USE_PORT          COM5	//主站占用的端口号

#define MODBUS_MST_RECE_BUF_LEN		128
#define MODBUS_MST_SEND_BUF_LEN		128

typedef struct 
{
	UINT8 modbus_rece_buf[MODBUS_MST_RECE_BUF_LEN];
	UINT8 rece_len;
}MODBUS_MST_RECE_BUF;

typedef struct 
{
	UINT8 modbus_send_buf[MODBUS_MST_SEND_BUF_LEN];
	UINT8 send_len;
}MODBUS_MST_SEND_BUF;


//-------------------------------------------------------------------------------
//以下定义代码共用于主站
#define CODE_YX		2		//遥信
#define CODE_YC		3		//遥测
#define CODE_YK		5		//
#define ONE_EDIT	0x06		//写单个寄存器
#define CODE_EDIT	0x10		//写多个寄存器
#define READ_YK  	0x01		//读遥控地址
//
#define MODBUS_MST_FRAME_MIN_LEN	8		//作为主站接收到的最小帧长

//
typedef struct 
{	
 	UINT32 rece_byte_num;	//接收总字节计数
 	UINT32 send_byte_num;	//发送总字节计数	
 	UINT8  connect_flag;	//连接标志
}MODBUS_STATE;
//-------------------------------------------------------------------------------



//定义主站所接终端参数结构
typedef struct 
{	
	UINT8  num;				//数量
	
	struct TERMINAL_ATTR
	{	
		UINT8 addr;				//地址
		//
		UINT8 word_len;			//所请求的寄存器长度
		UINT16 word_offset;		//所请求的寄存器基址
		//
		UINT8 available_num;	//可用的寄存器数量	(最大2个)
		UINT8 save_place[2];	//数据存放到YC块中的位置
		UINT16 reg_offset[2];	//寄存器位置	
	} terminal[6];				//终端属性,最大挂4个终端

	//TERMINAL_ATTR mb_mst_terminal_para;	//终端参数块
}MB_MST_TERMINAL_PARA;




//定义终端参数，此变量需要在上电时进行初始化，如果此变量块的第一个量NUM为0，则此主站功能自动禁止。
EXT_MB_MASTER MB_MST_TERMINAL_PARA mb_mst_terminal_para;	

EXT_MB_MASTER MODBUS_STATE mst_modbus_state[6];

EXT_MB_MASTER u32 Float2Int(UINT8 byte3,UINT8 byte2, UINT8 byte1, UINT8 byte0);

//该函数在上电时被LOAD PARA时调用，其填充的也即是主站的终端参数
EXT_MB_MASTER void Init_Mb_Mst(void);

//主站的运行函数，需要周期调用
EXT_MB_MASTER void Pro_Mb_Mst(void);

#endif

