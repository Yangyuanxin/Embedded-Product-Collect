#ifndef BMS_H_
#define BMS_H_

#include"stm32f10x.h"

#include"rtc.h"

typedef enum
{
	BMS_stage_handshake = 1,  //握手阶段
	BMS_stage_config = 2,     //参数配置阶段
	BMS_stage_charging = 3,   //充电阶段
	BMS_stage_end = 4,        //充电结束阶段
	BMS_stage_timeout = 5,
}bms_stage;


#pragma pack(push) //保存对齐状态
#pragma pack(1)//设定为1字节对齐
//多包传输协议结构
typedef struct
{
	u8  ctl_byte;   //控制字节
	u16 msg_lenth;  //消息总长度  字节
	u8  pkg_cnt;  //数据包个数
	u8  not_use;  //保留  固定 0xff
	u8  pgn[3];  //PGN
}TP_CM_RTS_def;
#pragma pack(pop) //恢复对齐状态


//用于管理接收BMS多包
typedef struct
{
	u16 len;  //消息总数据长度  字节
	u16 received_len;  //实际收到的字节
	u8  pkg_cnt;  //分包个数
	u8  recv_pkg_id;  //下一个期望收到的分包id
	u8  pkg_rx_en;  //是否接收分包  0:不接收   1:接收
	u8  pgn[3];   //pgn编号
	u8  *buf;  //数据缓冲区
}mult_pkg_info_def;


typedef struct
{
	void (*init)(void);  //协议初始化
	void (*msg_handler)(u8 pgn, u8* msg, u16 len);  //报文处理
	void (*timeout_handler)(void);   //can接收超时处理
	int (*poll)(void);  //轮询处理   可以在这里做数据包超时检测     返回oxff表示充电结束    0继续充电      (返回1表示辅助电源要切换为24V)
}bms_ptl_handler_def;


typedef struct
{
	u8 start_type;  //启动方式    1:后台     2:刷卡      3:管理员
	u8 chg_type;  //充电方式  0:自动充满   1:按度数   2:时间    3:按金额
	u32 value;    //参数   电量0.01kwh   时间:1分钟       金额:0.01元
}bms_chg_info_def;

extern bms_chg_info_def bms_chg_info;


//BMS协议类型
#define BMS_PROTOCOL_GB 1  //国标
#define BMS_PROTOCOL_XGB 2  //新国标

typedef struct
{
	u16 out_v;  //BMS返回的电压输出值  单位0.1V
	u16 out_i;  //BMS返回的电流输出值  单位0.1A

	u16 limt_v;  //最高充电电压 单位0.1V
	u16 limt_i;  //最高充电电流 单位0.1A
	u16 limt_v_single;  //单体动力蓄电池最高允许充电电压  单位0.01V
	u16 limt_t;  //最高充许温度

	u16 set_v;  //请求电压
	u16 set_i;  //请求电流
	u8  mode;  //恒压  恒流


	u8  SOC;   //电量百分比

	//01铅酸
	//02镍氢
	//03磷酸铁锂
	//04锰酸锂
	//05钴酸锂
	//06三元材料
	//07聚合物锂离子
	//08钛酸锂
	//FF其它
	u8  battery_type;

	u8  timeout_cnt;  //连续数据接收超时次数

	u16 Ah;  //容量  单位0.1Ah
	u16 V;   //额定电压  单位0.1V

	u16 max_v_single;  //最高单体动力电池电压  单位0.01V

	u16 min_v_single;  //最低单体动力电池电压  单位0.01V

	u16 remain_t;  //估算剩余充电时间  单位分钟

	u16 max_kWh;  //标称总能量  单位0.1kWh

	u8  max_temp;  //最高单位电池温度  单位1度  -50度偏移

	u8  min_temp;  //最高单位电池温度  单位1度  -50度偏移

	u8  protocol;  //BMS协议

	// 1 达到SOC值   2 达到总电压  3 达到单体电压
	// 4 绝缘故障     5 输出连接器过温  6 BMS元件 输出连接器过温
	// 7 充电连接器故障  8 电池组温度过高  9 其它故障  10 电流过大
	// 11 电压异常   12 功率过大  13 手动停止
	// 14 接收完成充电准备报文超时  15 电池请求停止 16 接收辨识报文超时  17 接收电池充电参数报文超时
	// 18 接收电池充电总状态报文超时  19 接收电池充电需求报文超时    20 电流过小   21 电池组温度过高2  22单体电压过高        23 接收辨识报文超时  24 接收电池充电参数报文超时            50  电表故障      69余额不足 70 达到设定度数   71 达到设定时间   72  达到设定金额   73 服务器停止   74 服务器离线
	u8  stop_reason;  //充电停止原因

	u8  bms_reuest_stop;  //BMS主动请求停止

	u8  bms_wait;  //BMS请求暂停充电

	u8  reset_cnt;  //当非充电结束阶段发生报文超时后  充电桩应重新进行连接  重新连接次数不超时三次

	u32 err_id;  //bms返回的错误ID 默认为0 表示无错误

	u32 timer_tick;  //BMS计时  单位10ms

	u8  can_out;

	bms_stage stage;  //当前充电阶段

	bms_ptl_handler_def *event;  //事件处理函数
	
	u8 vin[17];  //车辆标识

	u32 no;  //电池组序号

	u8 manufacturer[4]; //生产商名称  ASCII

	u8 year;  //生产日期年份 2000偏移
	u8 month;  //生产日期月
	u8 day;  //生产日期日

	time_t start_t;  //开始充电时间
	time_t stop_t;   //结束充电时间

	u8 v12_v24;  //当前辅助电源电压   0:12V  1:24V

	u8 msg_id;  //当前信息ID
	u8 msg[50];  //bms 告警信息  错误信息

}bms_info_def;

//多包数据帧
#define PGN_60160 0xEB
//传输协议连接管理消息
#define PGN_60416 0xEC


#define PGN_9984   0x27
#define PGN_256   0x01
#define PGN_512   0x02
#define PGN_1536  0x06
#define PGN_2304  0x09
#define PGN_4096  0x10
#define PGN_4352  0x11
#define PGN_4864  0x13
#define PGN_6400  0x19
#define PGN_7168  0x1C
#define PGN_7680  0x1E
#define PGN_5376  0x15
#define PGN_5632  0x16
#define PGN_5888  0x17



extern bms_info_def bms_info;
extern void CAN2_send(CanTxMsg *tx);
extern u8 get_pgn(u32 id);
extern void bms_stop_charge(u8 r);
extern void bms_gb_send_crm(u8 code);
extern void bms_pt_send_crm(u8 code);
extern int bms_start_charge(u8 start_type, u8 chg_type, u32 value);
extern u8 bms_charge_complete(void);
extern u8 bms_test_isok(void);
extern void bms_dc_out_dis(void);
extern void bms_dc_out_en(void);
extern void task_bms(void);

extern void bms_dc1_out_en(void);
extern void bms_dc1_out_dis(void);
extern void bms_sng1_en(void);
extern void bms_sng1_dis(void);

extern void bms_dc2_out_en(void);
extern void bms_dc2_out_dis(void);
extern void bms_sng2_en(void);
extern void bms_sng2_dis(void);

#endif /* BMS_H_ */
