#ifndef _MEASURE_
#define _MEASURE_

#ifndef _MEASURE_C
#define EXT_MEASURE extern
#else 
#define EXT_MEASURE
#endif
/**********************/


//应用到蒸发冷系统时，测量值存放在YC缓冲区的位置定义
//定义YC区中的内容。实际跟MODBUS 有关。
//---------START
#define ZJ_AI_BUSHUI       0   //总补水量
#define ZJ_AI_JIAYAO	   2   //总加药量
#define ZJ_AI_MST_1_VAL     4   //主传感器1#值
#define ZJ_AI_MST_2_VAL     5   //主传感器2#值
#define ZJ_AI_MST_3_VAL     6   //主传感器3#值

#define ZJ_AI_SLV_1_VAL     7   //从传感器1#值
#define ZJ_AI_SLV_2_VAL     8   //从传感器2#值
#define ZJ_AI_SLV_3_VAL     9   //从传感器3#值

#define ZJ_AI_LL			10	//流量值

#define ZJ_AI_PAIWU_STATE	11	//排污阀运行状态

#define ZJ_AI_JIAYAO_SUDU_1	12	//加药频率1#
#define ZJ_AI_JIAYAO_SUDU_2	13  //加药频率2#
#define ZJ_AI_JIAYAO_SUDU_3	14	//加药频率1#
#define ZJ_AI_JIAYAO_SUDU_4	15  //加药频率2#
#define ZJ_AI_JIAYAO_SUDU_5	49  //加药频率2#
#define ZJ_AI_JIAYAO_SUDU_6	50	//加药频率1#
#define ZJ_AI_JIAYAO_SUDU_7	51  //加药频率2#


#define ZJ_MODBUS_YC_SET_JIAYAO_SUDU_1	16	//后台设定的加药速度
#define ZJ_MODBUS_YC_SET_JIAYAO_SUDU_2	17	
#define ZJ_MODBUS_YC_SET_JIAYAO_SUDU_3	18
#define ZJ_MODBUS_YC_SET_JIAYAO_SUDU_4	19
#define ZJ_MODBUS_YC_SET_JIAYAO_SUDU_5	28
#define ZJ_MODBUS_YC_SET_JIAYAO_SUDU_6	43
#define ZJ_MODBUS_YC_SET_JIAYAO_SUDU_7	44

#define ZJ_MODBUS_YC_ZHU_CGQ_H_1 20
#define ZJ_MODBUS_YC_ZHU_CGQ_L_1 21
#define ZJ_MODBUS_YC_ZHU_CGQ_H_2 22
#define ZJ_MODBUS_YC_ZHU_CGQ_L_2 23
#define ZJ_MODBUS_YC_ZHU_CGQ_H_3 24
#define ZJ_MODBUS_YC_ZHU_CGQ_L_3 25
//add by hyman 2012-2-15
#define ZJ_MODBUS_YC_SLV2_CGQ_H_2 45
#define ZJ_MODBUS_YC_SLV2_CGQ_L_2 46
#define ZJ_MODBUS_YC_SLV2_CGQ_H_3 47
#define ZJ_MODBUS_YC_SLV2_CGQ_L_3 48

#define ZJ_MODBUS_YC_JIAYAOLIANG_3 26
#define ZJ_MODBUS_YC_JIAYAOLIANG_4 27

#define ZJ_MODBUS_YC_PAIWU_TIME 29  //每次排污时间
#define ZJ_MODBUS_YC_PAIWU_LOOP 30  //最少排污周期

#define ZJ_MODBUS_YC_AUTO_MODE   31	//手自动转换，0手动1自动

#define ZJ_AI_MST_1_WD     32   //主传感器1#值
#define ZJ_AI_MST_2_WD     33   //主传感器2#值

#define ZJ_AI_SLV_1_WD     34   //从传感器1#值
#define ZJ_AI_SLV_2_WD     35   //从传感器2#值

#define ZJ_AI_SLV2_1_WD     36   //再从传感器1#值
#define ZJ_AI_SLV2_2_WD     37   //再从传感器2#值

#define ZJ_AI_PAIWUED_TIME	38  //距上次排污时间
#define ZJ_AI_PAIWUING_TIME	39  //当前已排污时间

#define ZJ_AI_SLV2_1_VAL     40   //再从传感器1#值
#define ZJ_AI_SLV2_2_VAL     41   //再从传感器2#值
#define ZJ_AI_SLV2_3_VAL     42   //再从传感器3#值

//------------END

#define PERIOD                 10000//每5S计算一次流量



//AD通道的系数定义
#define AD1_xishu               1   //4-20MA代表的最大值	wd
/*#define AD2_xishu               1000     //4-20MA代表的最大值	yl
#define AD3_xishu               1000   //4-20MA代表的最大值	ph
#define AD4_xishu               1000 
*/
#define AD2_xishu               1     //4-20MA代表的最大值	yl
#define AD3_xishu               1   //4-20MA代表的最大值	ph
#define AD4_xishu               1


struct AD_MEASURE
{	
 	UINT32 ad_samp;	//ad采样值
 	UINT32 ad_value;//ad通道对应的现场值	
 //	UINT8  proportion;//采样与再场值的比例
};
typedef union 
{	
  struct
  {    
    u32 orp_samp;               //orp采样值
    u32 by1_samp;               //流量采样值
    u32 by2_samp;        //备用采样值      
    u32 by3_samp;      //备用采样值
//    u32 fulx_samp;               //流量采样值
//    u32 water_samp;              //补水量采样值
  }value;        
  u32 ad_group[6];        
}AD_BUF;

EXT_MEASURE AD_BUF ad_buf[6];//采6次后计算一次，存放当前6次的采样值
EXT_MEASURE struct AD_MEASURE ad_measure[16];//存放计算后的最终值
EXT_MEASURE struct AD_MEASURE old_ad_measure[16];//存放计算后的最终值
//EXT_MEASURE u16 AD_Value[4];
EXT_MEASURE u16 lcd_disp_measure[16];
EXT_MEASURE u8 measure_flag;//用于判断是否采了6次，需要计算了
EXT_MEASURE vu8 fulx_measure_flag;//用于判断流量是否需要计算了
EXT_MEASURE vu8 fulx_measure_flag1;//用于判断流量是否需要计算了

//EXT_MEASURE u8 update_measure_lcd;
EXT_MEASURE vu16 pulse_num,pulse_minus;
EXT_MEASURE vu16 pulse_num1,pulse_minus1;

//hyman2011-4
EXT_MEASURE float jlb1_need_times;  //计量泵1已经加药量
EXT_MEASURE float jlb1_all_times;  //计量泵1总共加药量

EXT_MEASURE float jlb2_need_times;  //计量泵2已经加药量
EXT_MEASURE float jlb2_all_times;  //计量泵2总共加药量

EXT_MEASURE float jlb3_need_times;  //计量泵3已经加药量
EXT_MEASURE float jlb3_all_times;  //计量泵3总共加药量

EXT_MEASURE float jlb4_need_times;  //计量泵4已经加药量
EXT_MEASURE float jlb4_all_times;  //计量泵4总共加药量

EXT_MEASURE float jlb5_need_times;  //计量泵5已经加药量
EXT_MEASURE float jlb5_all_times;  //计量泵5总共加药量

EXT_MEASURE float jlb6_need_times;  //计量泵5已经加药量
EXT_MEASURE float jlb6_all_times;  //计量泵5总共加药量

EXT_MEASURE float jlb7_need_times;  //计量泵6已经加药量
EXT_MEASURE float jlb7_all_times;  //计量泵6总共加药量

EXT_MEASURE float ll_single_jiayao3_value;	//单次流量加药量3
EXT_MEASURE float ll_single_jiayao4_value;	//单次流量加药量3

EXT_MEASURE u32 liuliangjiMaster_all_times;  //主流量计总接收次数
EXT_MEASURE u32 liuliangjiMaster_all_water;  //主所有的补水量



EXT_MEASURE void Adc_Init(void);
EXT_MEASURE void time_control(void);
EXT_MEASURE void ad_calucate(void);
EXT_MEASURE void TIM_Configuration(void);

#endif
