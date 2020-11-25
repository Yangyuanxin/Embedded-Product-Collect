/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        applied_math.h
 * Author:           王志华       
 * Version:          1.0
 * Date:             2019-02-28
 * Description:      应用数学运算
 * Others:      
 * Function List:    
    1. 获取数值的符号
    2. 四舍五入取整
    3. 三维向量取模
    4. 二维向量取模
    5. 三维向量的归一化处理
    6. 二维向量的归一化处理
    7. 三维向量内积（点积）
    8. 二维向量内积（点积）
    9. 三维向量叉积
    10.三维向量之间的夹角
    11.根据经纬度计算两点间距离
    12.计算普通CRC校验值
    13.计算EPO CRC校验值
 * History: 
    1. Date:         2019-02-28
       Author:       王志华
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#ifndef __APPLIED_MATH_H__
#define __APPLIED_MATH_H__

#include "gm_type.h"

#define GRAVITY_CONSTANT 9.80665

#define MSEC_PER_SECONDS 1000

#define SECONDS_PER_MIN 60

#define MINS_PER_HOUR 60

#define SECONDS_PER_HOUR (SECONDS_PER_MIN*MINS_PER_HOUR)

#define HOURS_PER_DAY 24

#define DAYS_PER_YEAR 365

#define SECONDS_PER_DAY (HOURS_PER_DAY * SECONDS_PER_HOUR)

#define SECONDS_PER_YEAR (SECONDS_PER_HOUR*HOURS_PER_DAY*DAYS_PER_YEAR)


//二维向量类型及指针类型
typedef struct
{
    float x;
    float y;
} Vector2D, *PVector2D;

//三维向量类型及指针类型
typedef struct
{
    float x;
    float y;
    float z;
} Vector3D, *PVector3D;

/**
 * Function:   获取数值的符号
 * Description:获取双精度浮点型变量的符号
 * Input:	   value——输入的双精度浮点型变量
 * Output:	   无
 * Return:	   1——正数；-1——负数；0——0值
 * Others:	   
 */
char applied_math_get_sign(double value);

/**
 * Function:   四舍五入取整
 * Description:获取双精度浮点型变量的四舍五入整数值
 * Input:	   value——输入的双精度浮点型变量
 * Output:	   无
 * Return:	   取整后的整型值
 * Others:	   
 */
S32 applied_math_round(double value);

/**
 * Function:   三维向量取模
 * Description:三维向量取模
 * Input:	   v——输入的三维向量
 * Output:	   无
 * Return:	   三维向量的模
 * Others:	   
 */
double applied_math_get_magnitude_3d(Vector3D v);

/**
 * Function:   二维向量求模
 * Description:二维向量求模
 * Input:	   v——输入的三维向量
 * Output:	   无
 * Return:	   二维向量的模
 * Others:	   
 */
double applied_math_get_magnitude_2d(Vector2D v);

/**
 * Function:   三维向量的归一化处理
 * Description:三维向量的的各个分量除以向量的模
 * Input:	   v——输入的三维向量
 * Output:	   无
 * Return:	   三维向量的归一化向量
 * Others:	   
 */
Vector3D applied_math_normalize_3d(Vector3D v);

/**
 * Function:   二维向量的归一化处理
 * Description:二维向量的的各个分量除以向量的模
 * Input:	   v——输入的三维向量
 * Output:	   无
 * Return:	   二维向量的归一化向量
 * Others:	   
 */
Vector2D applied_math_normalize_2d(Vector2D v);

/**
 * Function:   三维向量内积（点积）
 * Description:求两个三维向量的内积
 * Input:	   a——第一个三维向量；b——第二个三维向量
 * Output:	   无
 * Return:	   三维向量的内积（点积）
 * Others:	   
 */
float applied_math_inner_product_3d(Vector3D a, Vector3D b);

/**
 * Function:   二维向量内积（点积）
 * Description:求两个二维向量的内积
 * Input:	   a——第一个二维向量；b——第二个二维向量
 * Output:	   无
 * Return:	   二维向量的内积（点积）
 * Others:	   
 */
float applied_math_inner_product_2d(Vector2D a, Vector2D b);

/**
 * Function:   三维向量叉积
 * Description:求两个三维向量的叉积
 * Input:	   a——第一个三维向量；b——第二个三维向量
 * Output:	   无
 * Return:	   三维向量的叉积
 * Others:	   
 */
Vector3D applied_math_cross_product_3d(Vector3D u, Vector3D v);

/**
 * Function:   三维向量之间的夹角
 * Description:求两个三维向量之间的夹角
 * Input:	   a——第一个三维向量；b——第二个三维向量
 * Output:	   无
 * Return:	   夹角（单位为度,四舍五入取整）,范围[0,180]
 * Others:	   
 */
U8 applied_math_get_angle_3d(Vector3D a, Vector3D b);

/**
 * Function:   圆周上两个角度之间的夹角
 * Description:
 * Input:	   angle_degree1——第1个角度（单位度）；angle_degree2——第2个角度（单位度）
 * Output:	   无
 * Return:	   夹角,范围[0,180]
 * Others:	   
 */
U16 applied_math_get_angle_diff(U16 angle_degree1, U16 angle_degree2);

/**
 * Function:   根据经纬度计算两点间距离
 * Description:根据起始和结束的经纬度坐标,求两点之间的距离
 * Input:	   start_lng——起始点经度；start_lat——起始点纬度；end_lng——结束点经度；end_lat——结束点纬度；
 * Output:	   无
 * Return:	   距离（单位为米）
 * Others:	   
 */
double applied_math_get_distance(double start_lng, double start_lat, double end_lng, double end_lat);

/**
 * Function:   计算普通CRC校验值
 * Description:计算一段数据的16位CRC校验值
 * Input:	   p_data——数据起始地址；len——数据长度
 * Output:	   无
 * Return:	   CRC校验值
 * Others:	   
 */
U16 applied_math_calc_common_crc16(const U8* p_data, U32 len);

/**
 * Function:   计算EPO CRC校验值
 * Description:计算一段数据的16位CRC校验值
 * Input:	   p_data——数据起始地址；offset——偏移量；len——数据长度；pre_val——
 * Output:	   无
 * Return:	   CRC校验值
 * Others:	   
 */
U32 applied_math_calc_epo_crc16(const U8* p_data, U32 len, U32 pre_val);

/**
 * Function:   一阶滞后滤波
 * Description:优点:对周期性干扰具有良好的抑制作用适用于波动频率较高的场合
               缺点:相位滞后,灵敏度低,不能消除滤波频率高于采样频率的 1/2 的干扰信号
 * Input:	   last_value——历史值；new_value——实时值；len——数据长度；alpha(0,1]——滤波系数
 * Output:	   无
 * Return:	   滤波后的值
 * Others:	   
 */
float applied_math_lowpass_filter(const float last_value, const float new_value, float alpha);

/**
 * Function:   计算平均值
 * Description:
 * Input:	   p_array——数组指针；len——数组长度
 * Output:	   无
 * Return:	   数组平均值
 * Others:	   
 */
float applied_math_avage(const float* p_array,const U16 len);

#endif

