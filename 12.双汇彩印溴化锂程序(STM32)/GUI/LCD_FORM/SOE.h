/*

//DATA:2008-03-01
//VER:	V10	
//该头文件在每个保护中会有一个拷贝，通用的不必改动，保护相关处需改动


//该处描述旨在说明保护程序中SOE处理部分的改动，得到此文件后，删掉此段落即可

reportsoe函数去掉INT16U valueh参数，SOE结构里面去掉 pevtw->soevalueh = valueh;
LCD_COMMU422中函数void SendFrameSeqSoe(void)里在组织发送数据时，动作值改为2个字节
SOESTRU结构中去掉INT16U soevalueh;

//删除结束


//-----------------------------------------------------------------------
//		通 用 部 分(不必改动)
//-----------------------------------------------------------------------
//SOE描述部分占用二字节，该字通用于液晶及485通讯中而不必更改位定义

////D15-D8：动作描述
			D8:
				1:事故
				0:非事故
			D9:	
				1:预告
				0:非预告
			D10:	
				1:带方向
				0:不带方向
			D11:	
				1:合分性质
				0:动作复归性质
			D12:	(取决于D11)
				1:合/动作
				0:分/复归
			D15:	动作值有效标志	
				1.有效 
				0.无效

//D7-D5	：格式编码
			0:无小数点
			1:一位小数点
			2:二位小数点
			3:三位小数点
			4:四位小数点
			5:五位小数点		
//D0-D4	：单位编码
			0:无单位
			1:A
			2:V
			3:Ω
			4:S
			5:Hz
			6:Hz/s
			7:相
			8:℃
			9:kΩ
			10:%
			11:度
			12:W
			13:Var
		
*/
#ifndef __SOE_H
#define __SOE_H
#include "prodef.h"
#include "typedefine.h"


/***************************************************
SOE_CODE
****************************************************/

#define	BASEKIEVTNUM	0x0

#define EVTKI1					(0x0 + BASEKIEVTNUM)
#define EVTKI2					(0x1 + BASEKIEVTNUM)
#define EVTKI3					(0x2 + BASEKIEVTNUM)
#define EVTKI4					(0x3 + BASEKIEVTNUM)
#define EVTKI5					(0x4 + BASEKIEVTNUM)
#define EVTKI6					(0x5 + BASEKIEVTNUM)
#define EVTKI7					(0x6 + BASEKIEVTNUM)
#define EVTKI8					(0x7 + BASEKIEVTNUM)
#define EVTKI9					(0x8 + BASEKIEVTNUM)
#define EVTKI10					(0x9 + BASEKIEVTNUM)
#define EVTKI11					(0xa + BASEKIEVTNUM)
#define EVTKI12					(0xb + BASEKIEVTNUM)
#define EVTKI13					(0xc + BASEKIEVTNUM)
#define EVTKI14					(0xd + BASEKIEVTNUM)
#define EVTKI15					(0xe + BASEKIEVTNUM)
#define EVTKI16					(0xf + BASEKIEVTNUM)
#define EVTKIHWJ				(0x10 + BASEKIEVTNUM)//合位置
#define EVTKITWJ				(0x11 + BASEKIEVTNUM)//跳位置
#define EVTKIXHFG				(0x12 + BASEKIEVTNUM)//信号复归
#define EVTKIYFJD				(0x13 + BASEKIEVTNUM)//远方/就地
#define EVTKITHYCN				(0x14 + BASEKIEVTNUM)//弹簧已储能
#define EVTKIDDHW				(0x15 + BASEKIEVTNUM)//接地刀闸合位
#define EVTKIGDHW				(0x16 + BASEKIEVTNUM)//隔离刀闸合位
#define EVTKIYLJD				(0x17 + BASEKIEVTNUM)//压力降低
#define EVTKICHZBS				(0x18 + BASEKIEVTNUM)//重合闸闭锁
#define EVTKIWTB				(0x19 + BASEKIEVTNUM)//外变跳本变引入
#define EVTKIUHF				(0x1a + BASEKIEVTNUM)//高压侧复合电压引入
#define EVTKIUMF				(0x1b + BASEKIEVTNUM)//中压侧复合电压引入
#define EVTKIULF				(0x1c + BASEKIEVTNUM)//低压侧复合电压引入
#define EVTKIZWS				(0x1d + BASEKIEVTNUM)//重瓦斯
#define EVTKITYZWS				(0x1e + BASEKIEVTNUM)//调压重瓦斯
#define EVTKIQWS				(0x1f + BASEKIEVTNUM)//轻瓦斯
#define EVTKITYQWS				(0x20 + BASEKIEVTNUM)//调压轻瓦斯
#define EVTKIYWG				(0x21 + BASEKIEVTNUM)//油温高
#define EVTKIYWSG				(0x22 + BASEKIEVTNUM)//油温升高
#define EVTKIYWGG				(0x23 + BASEKIEVTNUM)//油温过高
#define EVTKIYYJD				(0x24 + BASEKIEVTNUM)//油压降低
#define EVTKIMCLT				(0x25 + BASEKIEVTNUM)//灭磁开关联跳
#define EVTKISJSGLT				(0x26 + BASEKIEVTNUM)//水机事故联跳
#define EVTKILCGZ				(0x27 + BASEKIEVTNUM)//励磁故障
#define EVTKIFJYX				(0x28 + BASEKIEVTNUM)//风机运行



#define EVTKZD					(0x50 + BASEKIEVTNUM)//控故障
#define EVTPTDX					(0x51 + BASEKIEVTNUM)//PT断线
#define EVTCTDX					(0x52 + BASEKIEVTNUM)//CT断线
#define EVTTBLQD				(0x53 + BASEKIEVTNUM)//突变量启动
#define EVTGFHQD				(0x54 + BASEKIEVTNUM)//过负荷启动
#define EVTGFH					(0x55 + BASEKIEVTNUM)//过负荷
#define EVTGFHGJ				(0x56 + BASEKIEVTNUM)//过负荷告警
#define EVTFSHGFH				(0x57 + BASEKIEVTNUM)//反时限过负荷
#define EVTFSHGFHGJ				(0x58 + BASEKIEVTNUM)//反时限过负荷告警
#define EVTSD					(0x59 + BASEKIEVTNUM)//速断
#define EVTI1					(0x5a + BASEKIEVTNUM)//过流I段
#define EVTJSI1					(0x5b + BASEKIEVTNUM)//加速过流I段
#define EVTI1T1					(0x5c + BASEKIEVTNUM)//过流I段I时限
#define EVTI1T2					(0x5d + BASEKIEVTNUM)//过流I段II时限
#define EVTI2					(0x5e + BASEKIEVTNUM)//过流II段
#define EVTJSI1					(0x5f + BASEKIEVTNUM)//加速过流II段
#define EVTI2T1					(0x60 + BASEKIEVTNUM)//过流II段I时限
#define EVTI2T2					(0x61 + BASEKIEVTNUM)//过流II段II时限
#define EVTFQ					(0x62 + BASEKIEVTNUM)//风机启动
#define EVTTYBS					(0x63 + BASEKIEVTNUM)//调压闭锁
#define EVTIDL_QD				(0x64 + BASEKIEVTNUM)//启动中短路保护
#define EVTIDL_RUN				(0x65 + BASEKIEVTNUM)//运行中短路保护
#define EVTIFSXGL				(0x66 + BASEKIEVTNUM)//反时限过流
#define EVTGR					(0x67 + BASEKIEVTNUM)//过热保护
#define EVTGR_WARN				(0x68 + BASEKIEVTNUM)//过热保护告警
#define EVTQDTL					(0x69 + BASEKIEVTNUM)//启动时间过长
#define EVTIMEMORY				(0x6a + BASEKIEVTNUM)//记忆过流
#define EVTCDSD					(0x6b + BASEKIEVTNUM)//差动速断
#define EVTBLCD					(0x6c + BASEKIEVTNUM)//比率差动
#define EVTIDIFFWARN			(0x6d + BASEKIEVTNUM)//差流告警
#define EVTNGL_WARN				(0x6e + BASEKIEVTNUM)//逆功率告警
#define EVTNGL					(0x6f + BASEKIEVTNUM)//逆功率跳闸
#define EVTIFX					(0x70 + BASEKIEVTNUM)//负序过流
#define EVTIFX_WARN				(0x71 + BASEKIEVTNUM)//负序过流告警
#define EVTIFX1					(0x72 + BASEKIEVTNUM)//负序过流I段
#define EVTIFX2					(0x73 + BASEKIEVTNUM)//负序过流II段
#define EVTIFXFSX				(0x74 + BASEKIEVTNUM)//反时限负序过流
#define EVTIFXFSX_WARN			(0x75 + BASEKIEVTNUM)//反时限负序过流告警
#define EVTI0					(0x76 + BASEKIEVTNUM)//零序过流
#define EVTI0_WARN				(0x77 + BASEKIEVTNUM)//零序过流告警
#define EVTI0BAL				(0x78 + BASEKIEVTNUM)//不平衡零序过流
#define EVTI0GYC				(0x79 + BASEKIEVTNUM)//高压侧零序过流
#define EVTI0GYC_WARN			(0x7a + BASEKIEVTNUM)//高压侧零序过流告警
#define EVTI0FSX_DYC			(0x7b + BASEKIEVTNUM)//低压侧反时限零序过流
#define EVTI0DYC_WARN			(0x7c + BASEKIEVTNUM)//低压侧零序过流告警
#define EVTI01					(0x7d + BASEKIEVTNUM)//零序过流I段
#define EVTI01_WARN				(0x7e + BASEKIEVTNUM)//零序过流I段告警
#define EVTJSI01				(0X7f + BASEKIEVTNUM)//加速零序过流I段
#define EVTI01T1				(0x80 + BASEKIEVTNUM)//零序过流I段I时限
#define EVTI01T2				(0x81 + BASEKIEVTNUM)//零序过流I段II时限
#define EVTI02					(0x82 + BASEKIEVTNUM)//零序过流II段
#define EVTI02_WARN				(0x83 + BASEKIEVTNUM)//零序过流II段告警
#define EVTJSI02				(0x84 + BASEKIEVTNUM)//加速零序过流II段
#define EVTI02T1				(0x85 + BASEKIEVTNUM)//零序过流II段I时限
#define EVTI02T2				(0x86 + BASEKIEVTNUM)//零序过流II段II时限
#define EVTI03					(0x87 + BASEKIEVTNUM)//零序过流III段
#define EVTI03_WARN				(0x88 + BASEKIEVTNUM)//零序过流III段告警
#define EVTJSI03				(0x89 + BASEKIEVTNUM)//加速零序过流III段
#define EVTI0JX					(0x8a + BASEKIEVTNUM)//间隙零序过流
#define EVTWBT					(0x8b + BASEKIEVTNUM)//外变跳本变
#define EVTBTW					(0x8c + BASEKIEVTNUM)//本变跳外变




#define EVTUCOMPLEX_UL			(0xa0 + BASEKIEVTNUM)//复合电压之低电压
#define EVTUCOMPLEX_UFX			(0xa1 + BASEKIEVTNUM)//复合电压之负序电压
#define EVTUH					(0xa2 + BASEKIEVTNUM)//过电压保护
#define EVTUL					(0xa3 + BASEKIEVTNUM)//失电压保护
#define EVTUL1					(0xa4 + BASEKIEVTNUM)//低电压I段
#define EVTUL21					(0xa5 + BASEKIEVTNUM)//低电压II段I时限
#define EVTUL22					(0xa6 + BASEKIEVTNUM)//低电压II段II时限
#define EVTU0					(0xa7 + BASEKIEVTNUM)//零序过压
#define EVTU0_WARN				(0xa8 + BASEKIEVTNUM)//零序过压告警
#define EVTU0_GND_STATOR		(0xa9 + BASEKIEVTNUM)//定子95%接地保护
#define EVTU0_GND_STATOR_WARN	(0xaa + BASEKIEVTNUM)//定子95%接地保护告警
#define EVTU3_GND_STATOR		(0xab + BASEKIEVTNUM)//定子100%接地保护
#define EVTU3_GND_STATOR_WARN 	(0xac + BASEKIEVTNUM)//定子100%接地保护告警
#define EVTONE_GND_RATOR		(0xad + BASEKIEVTNUM)//转子一点接地
#define EVTONE_GND_RATOR_WARN	(0xae + BASEKIEVTNUM)//转子一点接地告警
#define	EVTTWO_GND_RATOR		(0xaf + BASEKIEVTNUM)//转子两点接地
#define	EVTTWO_GND_RATOR_WARN	(0xb0 + BASEKIEVTNUM)//转子两点接地告警
#define EVTLOSE_MAGNETISM		(0Xb1 + BASEKIEVTNUM)//失磁保护



#define EVTREMOTECTRH			(0xd0 + BASEKIEVTNUM)//遥控合
#define EVTREMOTECTRF			(0xd1 + BASEKIEVTNUM)//遥控分
#define EVTLOCALCTRH			(0xd2 + BASEKIEVTNUM)//就地合
#define EVTLOCALCTRF			(0xd3 + BASEKIEVTNUM)//就地分
#define EVTREMOTECTRQ			(0xd4 + BASEKIEVTNUM)//遥控风机启动
#define EVTREMOTECTRT			(0xd5 + BASEKIEVTNUM)//遥控风机停止
#define EVTLOCALCTRQ			(0xd6 + BASEKIEVTNUM)//就地风机启动
#define EVTLOCALCTRT			(0xd7 + BASEKIEVTNUM)//就地风机停止






//-----------------------------------------------------------------------
//definition for d15,VAL_VALIABLE
#define EVT_VAL_VALIABLE			(1<<15)

//definition for d8-d14,EVT_DESC
#define EVT_DESC_FAULT				(1<<8)
#define EVT_DESC_WARNIING			(1<<9)
#define EVT_DESC_WITH_FX			(1<<10)
#define EVT_DESC_ATTR_OPEN_CLOSE	(1<<11)
#define EVT_DESC_SET				(1<<12)

//definition for d5-d7,EVT_VAL_FORMAT
#define EVT_VAL_FORMAT_DOT_0		(0)
#define EVT_VAL_FORMAT_DOT_1		(1<<5)
#define EVT_VAL_FORMAT_DOT_2		(2<<5)
#define EVT_VAL_FORMAT_DOT_3		(3<<5)
#define EVT_VAL_FORMAT_DOT_4		(4<<5)
#define EVT_VAL_FORMAT_DOT_5		(5<<5)

//definition for d0-d4,EVT_UNIT
#define EVT_UNIT_NULL				(0)
#define EVT_UNIT_A					(1)
#define EVT_UNIT_V					(2)
#define EVT_UNIT_OU					(3)
#define EVT_UNIT_S					(4)
#define EVT_UNIT_HZ					(5)
#define EVT_UNIT_HZS				(6)
#define EVT_UNIT_XIANG				(7)
#define EVT_UNIT_DU_ASC				(8)
#define EVT_UNIT_K_OU				(9)
#define EVT_UNIT_BFB				(10)
#define EVT_UNIT_DU_CN				(11)
#define EVT_UNIT_W					(12)
#define EVT_UNIT_VAR				(13)
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//			保护相关部分(需要改动)
/*
例子介绍
//速段
#define EVT_DESC_SD		(EVT_VAL_VALIABLE											\	//动作值是否可用，若不可用，则省略此行
						| EVT_DESC_FAULT | EVT_DESC_ATTR_OPEN_CLOSE | EVT_DESC_SET	\	//动作描述
						| EVT_VAL_FORMAT_DOT_2										\	//小数点位数
						| EVT_UNIT_A)													//单位
*/
//-----------------------------------------------------------------------
//速段
#define EVTSD_DESCRIBE		(EVT_VAL_VALIABLE											\
							| EVT_DESC_FAULT | EVT_DESC_ATTR_OPEN_CLOSE | EVT_DESC_SET	\
							| EVT_VAL_FORMAT_DOT_2										\
							| EVT_UNIT_A)
//过流二段
#define EVTI2_DESCRIBE		(EVT_VAL_VALIABLE											\
							| EVT_DESC_FAULT | EVT_DESC_ATTR_OPEN_CLOSE | EVT_DESC_SET	\
							| EVT_VAL_FORMAT_DOT_2										\
							| EVT_UNIT_A)

/*
#define CLEAROLDACTIONL	0xC000
#define CLEAROLDACTIONH	0xf880
#define FRACCSTART	0X10

#define	EVTDZ_DESCRIBE		0x4129
#define EVTSD_DESCRIBE		0X7931
#define EVTI1_DESCRIBE		0X7931
#define EVTI2_DESCRIBE		0X7931
#define EVTJSI1_DESCRIBE	0X7931
#define EVTJSI2_DESCRIBE	0X7931
#define EVTCHZ_DESCRIBE		0xF809
#define EVTI01_DESCRIBE		0X7929	
#define EVTI02_DESCRIBE		0X7929	
#define EVTI03_DESCRIBE		0X7929	
#define	EVTKZD_DESCRIBE		0XF809
#define EVTTBLQD_DESCRIBE	0XF809
#define	EVTGFHQD_DESCRIBE	0X7929
#define EVTPTDX_DESCRIBE	0XFC09
#define	EVTETHERERR_DESCRIBE	0xff09
#define EVTREMOTECTRH_DESCRIBE	0XFC01
#define EVTREMOTECTRF_DESCRIBE	0XFC09
#define EVTLOCALCTRH_DESCRIBE	0XFC01
#define EVTLOCALCTRF_DESCRIBE	0XFC09

#define	EVTI0QD_DESCIRBE	0X6B29
*/

/************************************************************************/
#endif