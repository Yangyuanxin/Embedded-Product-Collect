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

#include "circular_queue.h"
#include "gm_memory.h"

GM_ERRCODE circular_queue_create(PCircularQueue p_queue, const U16 capacity, const GM_QUEUE_TYPE type)
{
    //多申请1个单元,留个空
	p_queue->capacity = (capacity + 1);
    if (GM_QUEUE_TYPE_FLOAT == type)
    {
        p_queue->f_buf = (float*)GM_MemoryAlloc(sizeof(float) * p_queue->capacity);
        if (NULL == p_queue->f_buf)
        {
            return GM_MEM_NOT_ENOUGH;
        }
    }
    else
    {
        p_queue->i_buf = (S32*)GM_MemoryAlloc(sizeof(S32) * p_queue->capacity);
        if (NULL == p_queue->i_buf)
        {
            return GM_MEM_NOT_ENOUGH;
        }
    }
    p_queue->head = 0;
    p_queue->tail = 0;
    return GM_SUCCESS;
}

void circular_queue_destroy(PCircularQueue p_queue, const GM_QUEUE_TYPE type)
{
    if (GM_QUEUE_TYPE_FLOAT == type)
    {
        GM_MemoryFree(p_queue->f_buf);
		p_queue->f_buf = NULL;
		p_queue->head = 0;
		p_queue->tail = 0;
    }
    else
    {
        GM_MemoryFree(p_queue->i_buf);
		p_queue->i_buf = NULL;
	    p_queue->head = 0;
		p_queue->tail = 0;
    }
}

U16 circular_queue_get_capacity(const PCircularQueue p_queue)
{
    return (p_queue->capacity - 1);
}

bool circular_queue_is_empty(const PCircularQueue p_queue)
{
    return (p_queue->head == p_queue->tail);
}

void circular_queue_empty(PCircularQueue p_queue)
{
    p_queue->head = 0;
    p_queue->tail = 0;
}

bool circular_queue_is_full(const PCircularQueue p_queue)
{
    return (p_queue->head == ((p_queue->tail + 1) % p_queue->capacity));
}

U16 circular_queue_get_len(const PCircularQueue p_queue)
{
    return ((p_queue->tail + p_queue->capacity - p_queue->head) % p_queue->capacity);
}

void circular_queue_en_queue_f(PCircularQueue p_queue, float value)
{
    //如果队列已满,队头被覆盖
    if (circular_queue_is_full(p_queue))
    {
        p_queue->head = (p_queue->head + 1) % p_queue->capacity;
    }
	
    *(p_queue->f_buf + p_queue->tail) = value;
    p_queue->tail = (p_queue->tail + 1) % p_queue->capacity;
}
void circular_queue_en_queue_i(PCircularQueue p_queue, S32 value)
{
	//如果队列已满,队头被覆盖
	if (circular_queue_is_full(p_queue))
	{
		p_queue->head = (p_queue->head + 1) % p_queue->capacity;
	}

    *(p_queue->i_buf + p_queue->tail) = value;
    p_queue->tail = (p_queue->tail + 1) % p_queue->capacity;
}

bool circular_queue_de_queue_f(PCircularQueue p_queue, float* p_value)
{
    if (!circular_queue_is_empty(p_queue))
    {
        *p_value = *(p_queue->f_buf + p_queue->head);
        p_queue->head = (p_queue->head + 1) % p_queue->capacity;
        return true;
    }
    else
    {
        return false;
    }
}

bool circular_queue_de_queue_i(PCircularQueue p_queue, S32* p_value)
{
    if (!circular_queue_is_empty(p_queue))
    {
        *p_value = *(p_queue->i_buf + p_queue->head);
        p_queue->head = (p_queue->head + 1) % p_queue->capacity;
        return true;
    }
    else
    {
        return false;
    }
}

bool circular_queue_get_head_f(const PCircularQueue p_queue, float* p_value)
{
    if (!circular_queue_is_empty(p_queue))
    {
        *p_value = *(p_queue->f_buf + p_queue->head);
        return true;
    }
    else
    {
        return false;
    }
}

bool circular_queue_get_head_i(const PCircularQueue p_queue, S32* p_value)
{
    if (!circular_queue_is_empty(p_queue))
    {
        *p_value = *(p_queue->i_buf + p_queue->head);
        return true;
    }
    else
    {
        return false;
    }
}

bool circular_queue_get_tail_f(const PCircularQueue p_queue, float* p_value)
{
    if (!circular_queue_is_empty(p_queue))
    {
        *p_value = *(p_queue->f_buf + (p_queue->tail - 1 + p_queue->capacity) % p_queue->capacity);
        return true;
    }
    else
    {
        return false;
    }
}
bool circular_queue_get_tail_i(const PCircularQueue p_queue, S32* p_value)
{
    if (!circular_queue_is_empty(p_queue))
    {
        *p_value = *(p_queue->i_buf + (p_queue->tail - 1 + p_queue->capacity) % p_queue->capacity);
        return true;
    }
    else
    {
        return false;
    }
}

bool circular_queue_get_by_index_f(const PCircularQueue p_queue, const U16 index, float* p_value)
{
    if (index >= circular_queue_get_capacity(p_queue) || index >= circular_queue_get_len(p_queue))
    {
        return false;
    }
    if (!circular_queue_is_empty(p_queue))
    {
        *p_value = *(p_queue->f_buf + (p_queue->tail - 1 - index + 2 * p_queue->capacity) % p_queue->capacity);
        return true;
    }
    else
    {
        return false;
    }
}

bool circular_queue_get_by_index_i(const PCircularQueue p_queue, const U16 index, S32* p_value)
{
    if (index >= circular_queue_get_capacity(p_queue) || index >= circular_queue_get_len(p_queue))
    {
        return false;
    }
    if (!circular_queue_is_empty(p_queue))
    {
        *p_value = *(p_queue->i_buf + (p_queue->tail - 1 - index + 2 * p_queue->capacity) % p_queue->capacity);
        return true;
    }
    else
    {
        return false;
    }
}

