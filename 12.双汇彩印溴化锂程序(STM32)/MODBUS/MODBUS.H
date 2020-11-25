#ifndef _MODBUS_H
#define _MODBUS_H

#ifndef _MODBUS_C
#define EXT_MODBUS extern
#else
#define EXT_MODBUS
#endif

#define LOCAL_ADDR				0x01

#define USE_IN_COM				COM2
#define SENSOR_USE_COM          COM3

#define MODBUS_RECE_BUF_LEN		256
#define MODBUS_SEND_BUF_LEN		256

#define CODE_YX		2		//遥信
#define CODE_YC		3		//遥测
#define CODE_YK		5		//
#define ONE_EDIT	0x06		//写单个寄存器
#define CODE_EDIT	0x10		//写多个寄存器
#define READ_YK  	0x01		//读遥控地址
//-------------------------------------------------------------------------------
#define YX_OFFSET	0	//遥信基地址
#define YK_OFFSET	96	//遥控基址yx缓冲区中的第7个字
#define YC_OFFSET	0x000	//遥测基地址

#define PARA_OFFSET	16	//在遥测缓冲区中参数开始存放的位置
//--------------------------------------

#define YX_MAX_NUM		10		//字

#define YC_MAX_NUM		128		//寄存器最大数，数值同相应装置相同

//#define YK_MAX_NUM      2      //根据最大的遥信数减去遥控偏移得出，字的个数


//-------------------------------------------------------------------------------



//#define R_TO_T_DELAY	2	//应答转换时间	*5MS

#define MODBUS_FRAME_MIN_LEN	8

struct RECE_BUF
{
	UINT8 modbus_rece_buf[MODBUS_RECE_BUF_LEN];
	UINT8 rece_len;
};

struct SEND_BUF
{
	UINT8 modbus_send_buf[MODBUS_SEND_BUF_LEN];
	UINT8 send_len;
};

struct MODBUS_STATE
{	
 	UINT32 rece_byte_num;	//接收总字节计数
 	UINT32 send_byte_num;	//发送总字节计数	
 	UINT8  connect_flag;	//连接标志
};
union SENSOR_SEND
{
  struct
  {
    u8 addr;
    u8 command;
    u8 reg_h;
    u8 reg_l;
    u8 len_h;
    u8 len_l;
    u8 crc_h;
    u8 crc_l;      
  }data;
  u8 sensor_send_group[8];
};



EXT_MODBUS struct MODBUS_STATE	modbus_state;

EXT_MODBUS struct RECE_BUF	rece_buf[PORT_MAX];
EXT_MODBUS struct SEND_BUF 	send_buf[PORT_MAX];

//EXT_MODBUS struct RECE_BUF	sensor_rece_buf;
EXT_MODBUS union SENSOR_SEND 	sensor_send_buf[4];
EXT_MODBUS u8 sensor_send_index;
EXT_MODBUS u16 yk_num;
//EXT_MODBUS u8 yk_on_off;


//EXT_MODBUS UINT16 yk_buf[YK_MAX_NUM];
EXT_MODBUS UINT16 yx_buf[YX_MAX_NUM],yx_OldBuf[YX_MAX_NUM];


EXT_MODBUS UINT16 yc_buf[YC_MAX_NUM];


//-----------------------------------------------------

EXT_MODBUS void InitModbus(void);
EXT_MODBUS void TaskModbus(u8 port);
EXT_MODBUS void TaskSensor(u8 port); 
//应用层写数据进入规约的接口函数
EXT_MODBUS void UpdataModbusYc(void);
EXT_MODBUS void UpdataModbusYx(UINT16 *p_scr,UINT8 len);
EXT_MODBUS UINT16 CheckCrc(UINT8 *source,UINT8 length);

EXT_MODBUS void pop_one_byte(UINT8 *scr,UINT8 length);
EXT_MODBUS void PopNByte(UINT8 *scr,UINT8 length, UINT8 pop_len);

EXT_MODBUS void ProReadYx(UINT8 *pFrame);
EXT_MODBUS void ProReadYc(UINT8 *pFrame);
EXT_MODBUS void SetupYc(UINT16 *scr,UINT8 len);

#endif

