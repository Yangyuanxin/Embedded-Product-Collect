#ifndef SERVER_H_
#define SERVER_H_

#include "stm32f10x.h"


#define  SERVER_SATUS_IDLE    1  //空闲状态  表示没有在充电
#define  SERVER_SATUS_CHGING  2  //正在充电  发送充电数据报文
#define  SERVER_SATUS_CHGEND  3  //充电结束  发送充电结算报文
#define  SERVER_SATUS_ERR     4  //充电机故障  发送充电机故障报文

typedef struct
{
	int (*send)(u8 *buf, u16 len);  //后台发送函数  在后台任务中调用   可以是 以太网  gprs wifi 发送方式
	void (*recv_callback)(u8 *buf, u16 len);  //接收服务器数据回调函数    在外面调用   如用以太网   gprs  wifi
}server_handler_def;


#pragma pack(push) //保存对齐状态
#pragma pack(1)//设定为1字节对齐

//通讯数据帧头
typedef struct
{
	u8  fh;  //帧头  固定0x5A
	u16 len;  //所有数据长度  fh+len+id+crc32+帧尾 = 12bytes
	u32 id;   //充电桩ID号
	u8 cmd;
	u8  data[1];  //数据  长度不定
}com_header_def;


typedef struct
{
	u8 y;  //年  从15年算   y==15 表示2015年
	u8 m;  //月
	u8 d;  //日
	u8 h;  //时
	u8 mm; //分
	u8 s;  //秒
}com_time_def;

//充电中数据
typedef struct
{
	u32 id;    //IC卡号
	u16 v;      //充电电压  0.1V
	u16 i;     //充电电流    0.1A
	u16 kwh;   //消耗电量    0.01kwh
	u16 amount;  //消耗金额  0.01元
	u16 time;    //已充时长  分钟
	u8  soc;     //电池soc
	com_time_def start_time;  //开始充电时间
	u8  gun_id;  //枪号
	u16 max_s_v;  //最高单体电压  0.01V
}com_chging_def;

//充电结算数据
typedef struct
{
	u32 id;    //IC卡号
	u32 balances;  //余额
	u16 kwh;   //消耗电量    0.01kwh
	u16 amount;  //消耗金额  0.01元
	u16 time;    //已充时长  分钟
	u8  soc;     //电池soc
	com_time_def start_time;  //开始充电时间
	u8  gun_id;  //枪号
}com_chgend_def;


//服务器读取充电机信息
typedef struct
{
	u8  ver[5];    //固件版本号
	u16 price;  //电价
	u32 kwh;   //电表计数    0.01kwh
}machine_info_def;

#pragma pack(pop) //恢复对齐状态


//充电数据信息
typedef struct
{
	u32 id;    //IC卡号
	u32 balances;  //余额
	u32 kwh;   //消耗电量    0.01kwh
	u32 amount;  //消耗金额  0.01元
	u32 time;    //已充时长  分钟
	u8  soc;     //电池soc
	u8  status;  //状态   1:空闲状态  2:正在充电  3:充电完成
	com_time_def start_time;  //开始充电时间
	u8  gun_id;  //枪号
}chg_info_def;



typedef struct
{
	u8 send_cnt;  //数据发送次数
	u8 rsp_cnt;   //服务器响应次数

	u8  state;   //当前阶段
	u8  status;  //与服务器连接状态  1:正常   0:离线
}server_info_def;


extern server_handler_def server_handler;
extern chg_info_def chg_info;
extern server_info_def server_info;

void task_server(void);

void server_send_idle(void);
void server_send_chging(void);
void server_send_chgend(void);

void server_recv_callback(u8 *buf, u16 len);

#endif
