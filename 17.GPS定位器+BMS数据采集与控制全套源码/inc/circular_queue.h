/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        circular_queue.h
 * Author:           王志华
 * Version:          1.0
 * Date:             2019-03-07
 * Description:      循环队列,存放整型和浮点型数据,循环覆盖写
 * Others:           
 * Function List:    
    1. 创建circular_queue
    2. 销毁circular_queue
    3. 获取队列容量
    4. 队列是否为空
    5. 清空队列
    6. 队列是否已满
    7. 获取队列长度
    8. 入队
    9. 出队
    10.获取队列头元素
    11.获取队列尾元素
    12.获取队列第n个元素
 * History: 
    1. Date:         2019-03-07
       Author:       王志华
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#ifndef __CIRCULAR_QUEUE_H__
#define __CIRCULAR_QUEUE_H__

#include "gm_type.h"
#include "error_code.h"

typedef enum
{
	GM_QUEUE_TYPE_INT = 0,
    GM_QUEUE_TYPE_FLOAT = 1
}GM_QUEUE_TYPE;

//使用者不要直接使用结构体成员变量
typedef struct
{
    float* f_buf;
    S32* i_buf;
	
	//指向第一个元素
    U16 head;

	//指向最后一个元素的下一个位置
    U16 tail;

	//队列容量
    U16 capacity;
} CircularQueue, *PCircularQueue;

/**
 * Function:   创建CircularQueue
 * Description:不要直接使用结构体内的成员变量 
 * Input:	   capacity:队列容量（单元个数,不是字节数）；type:整型或者浮点型
 * Output:	   p_queue:指向队列的指针
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   使用前必须调用,否则调用其它接口返回失败错误码
 */
GM_ERRCODE circular_queue_create(PCircularQueue p_queue, const U16 capacity, const GM_QUEUE_TYPE type); 

/**
 * Function:   销毁CircularQueue
 * Description:不要直接使用结构体内的成员变量 
 * Input:	   p_queue:指向队列的指针；type:整型或者浮点型
 * Output:	   p_queue:指向队列的指针
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
void circular_queue_destroy(PCircularQueue p_queue, GM_QUEUE_TYPE type);

/**
 * Function:   获取队列的容量
 * Description:不要直接使用结构体内的成员变量
 * Input:	   p_queue:指向队列的指针 
 * Output:	   无
 * Return:	   队列的容量
 * Others:	   
 */
U16 circular_queue_get_capacity(const PCircularQueue p_queue);

/**
 * Function:   队列是否为空
 * Description:不要直接使用结构体内的成员变量
 * Input:	   p_queue:指向队列的指针 
 * Output:	   无
 * Return:	   true——空;false——不空
 * Others:	   
 */
bool circular_queue_is_empty(const PCircularQueue p_queue);

/**
 * Function:   清空队列（注意不是销毁）
 * Description:不要直接使用结构体内的成员变量
 * Input:	   p_queue:指向队列的指针 
 * Output:	   p_queue:指向队列的指针
 * Return:	   无
 * Others:	   
 */
void circular_queue_empty(PCircularQueue p_queue);

/**
 * Function:   队列是否已满
 * Description:不要直接使用结构体内的成员变量
 * Input:	   p_queue:指向队列的指针 
 * Output:	   无
 * Return:	   true——满;false——未满
 * Others:	   
 */
bool circular_queue_is_full(const PCircularQueue p_queue);

/**
 * Function:   获取队列长度
 * Description:不要直接使用结构体内的成员变量
 * Input:	   p_queue:指向队列的指针 
 * Output:	   无
 * Return:	   队列长度
 * Others:	   
 */
U16 circular_queue_get_len(const PCircularQueue p_queue);

/**
 * Function:   浮点型数据入队
 * Description:不要直接使用结构体内的成员变量
 * Input:	   p_queue:指向队列的指针；value:入队的数据
 * Output:	   p_queue:指向队列的指针
 * Return:	   无
 * Others:	   如果队列已满,会覆盖队头数据
 */
void circular_queue_en_queue_f(PCircularQueue p_queue, float value);

/**
 * Function:   整型数据入队
 * Description:不要直接使用结构体内的成员变量
 * Input:	   p_queue:指向队列的指针；value:入队的数据
 * Output:	   p_queue:指向队列的指针
 * Return:	   无
 * Others:	   如果队列已满,会覆盖队头数据
 */
void circular_queue_en_queue_i(PCircularQueue p_queue, S32 value);

/**
 * Function:   浮点型数据出队
 * Description:不要直接使用结构体内的成员变量
 * Input:	   p_queue:指向队列的指针；p_value:出队的数据指针
 * Output:	   p_queue:指向队列的指针
 * Return:	   true——成功；false——失败（队列空)
 * Others:	   
 */
bool circular_queue_de_queue_f(PCircularQueue p_queue, float* p_value);

/**
 * Function:   整型数据出队
 * Description:不要直接使用结构体内的成员变量
 * Input:	   p_queue:指向队列的指针；p_value:出队的数据指针
 * Output:	   p_queue:指向队列的指针
 * Return:	   true——成功；false——失败（队列空)
 * Others:	   
 */
bool circular_queue_de_queue_i(PCircularQueue p_queue, S32* p_value);

/**
 * Function:   获取队头元素（浮点型）
 * Description:不要直接使用结构体内的成员变量
 * Input:	   p_queue:指向队列的指针
 * Output:	   p_value:数据指针
 * Return:	   true——成功；false——失败（队列空)
 * Others:	   
 */
bool circular_queue_get_head_f(const PCircularQueue p_queue, float* p_value);

/**
 * Function:   获取队头元素（整型）
 * Description:不要直接使用结构体内的成员变量
 * Input:	   p_queue:指向队列的指针
 * Output:	   p_value:数据指针
 * Return:	   true——成功；false——失败（队列空)
 * Others:	   
 */
bool circular_queue_get_head_i(const PCircularQueue p_queue, S32* p_value);

/**
 * Function:   获取队尾元素（浮点型）
 * Description:不要直接使用结构体内的成员变量
 * Input:	   p_queue:指向队列的指针
 * Output:	   p_value:数据指针
 * Return:	   true——成功；false——失败（队列空)
 * Others:	   
 */
bool circular_queue_get_tail_f(const PCircularQueue p_queue, float* p_value);

/**
 * Function:   获取队尾元素（整型）
 * Description:不要直接使用结构体内的成员变量
 * Input:	   p_queue:指向队列的指针
 * Output:	   p_value:数据指针
 * Return:	   true——成功；false——失败（队列空)
 * Others:	   
 */
bool circular_queue_get_tail_i(const PCircularQueue p_queue, S32* p_value);

/**
 * Function:   获取第index（从0开始）个元素（浮点型）
 * Description:不要直接使用结构体内的成员变量
 * Input:	   p_queue:指向队列的指针
 * Output:	   p_value:数据指针
 * Return:	   true——成功；false——失败（队列空)
 * Others:	   
 */
bool circular_queue_get_by_index_f(const PCircularQueue p_queue, const U16 index, float* p_value);

/**
 * Function:   获取第index（从0开始）个元素（整型）
 * Description:不要直接使用结构体内的成员变量
 * Input:	   p_queue:指向队列的指针
 * Output:	   p_value:数据指针
 * Return:	   true——成功；false——失败（队列空)
 * Others:	   
 */
bool circular_queue_get_by_index_i(const PCircularQueue p_queue, const U16 index, S32* p_value);

#endif

