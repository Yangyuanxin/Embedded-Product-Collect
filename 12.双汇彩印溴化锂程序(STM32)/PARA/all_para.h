/**/
#ifndef _ALL_PARA_H
#define _ALL_PARA_H

#ifndef _ALL_PARA_C
#define EXT_ALL_PARA extern
#else
#define EXT_ALL_PARA 
#endif


//----------------------------------------------------------------
//设备类型定义,该处定义应与COMM422.C中定义一致。每
//版不同的参数表，即会有一个不同的型号。
//该定义为一个字节，高半字节表示装置的类型，低半字节表示繁衍类
//组成的此字节为唯一值，对应一组参数表

//线路
#define DEV_TYPE_XL_A	0x01	//线路A型
#define DEV_TYPE_XL_B	0x02	//
//变压器主保护
#define DEV_TYPE_BYQ_CD_A	0x11
#define DEV_TYPE_BYQ_CD_B	0x12
#define DEV_TYPE_BYQ_CD_C	0x13

#define CTRL_PARA_LENGTH                   6*10





//----------------------------------------------------------------
EXT_ALL_PARA const UINT8 ctrl_para_xl_a[];
//参数指针的定义
EXT_ALL_PARA UINT8 *p_fix_para;			//定值参数指针
EXT_ALL_PARA UINT8 *p_jiankong_para;			//定值参数指针
EXT_ALL_PARA UINT8 *p_gprs_para;			//定值参数指针
EXT_ALL_PARA UINT8 *p_sensor_para;			//定值参数指针
EXT_ALL_PARA UINT8 *p_dispyx_para;		//
EXT_ALL_PARA UINT8 *p_soe_para;
EXT_ALL_PARA UINT8 *p_yc_para;
EXT_ALL_PARA UINT8 *p_kwh_para;
EXT_ALL_PARA UINT8 *p_jiayao_para;
EXT_ALL_PARA UINT8 *p_modulpara_para;
EXT_ALL_PARA UINT8 *p_adj_xs_para;		//修正系数参数
EXT_ALL_PARA UINT8 *p_yx_para;			//遥信参数

EXT_ALL_PARA void InitAllPara(void);

#endif

