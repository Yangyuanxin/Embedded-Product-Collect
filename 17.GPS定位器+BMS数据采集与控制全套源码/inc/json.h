/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        log_service.h
 * Author:           王志华       
 * Version:          1.0
 * Date:             2019-05-21
 * Description:      来自于cJSON，删除了大部分不用的功能和特性，print函数完全重写
 * Others:      
 * Function List:    
    1. 创建对象
    2. 添加数字
    3. 添加字符串
    4. 打印到字符串
    5. 释放内存
 * History: 
    1. Date:         2019-03-01
       Author:       王志华
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */


#ifndef __JSON_H__
#define __JSON_H__

typedef enum
{
	JSON_OBJECT = 0,
	JSON_INT,
	JSON_DOUBLE,
	JSON_STRING,
}JsonDataType;

typedef struct
{
    struct JsonObject* next;
    struct JsonObject* prev;
    struct JsonObject* child;
	char* name;
    JsonDataType type;
	S32 int_value;
    char* str_value;
    double double_value;
}JsonObject;

/**
 * Function:   创建json对象
 * Description:无
 * Input:      无
 * Output:     无
 * Return:     指向对象的指针
 * Others:     
 */
JsonObject* json_create(void);

/**
 * Function:   销毁json对象
 * Description:使用完以后必须销毁，否则内存泄露
 * Input:      p_object：指向对象的指针
 * Output:     无
 * Return:     无
 * Others:     
 */
void json_destroy(JsonObject* p_object);

/**
 * Function:   在json对象中添加一个整数
 * Description:
 * Input:      p_object：指向对象的指针；name：名称；value：值
 * Output:     无
 * Return:     无
 * Others:     
 */
JsonObject* json_add_int(JsonObject*     p_object, const char* name, const int value);

/**
 * Function:   在json对象中添加一个double数值
 * Description:float类型也用这个函数
 * Input:      p_object：指向对象的指针；name：名称；value：值
 * Output:     无
 * Return:     无
 * Others:     
 */
JsonObject* json_add_double(JsonObject*     p_object, const char* name, const double value);

/**
 * Function:   在json对象中添加一个字符串
 * Description:对象内部会申请新的内存存储字符串，在销毁对象时释放
 * Input:      p_object：指向对象的指针；name：名称；value：值
 * Output:     无
 * Return:     无
 * Others:     
 */
JsonObject* json_add_string(JsonObject*     p_object, const char* name, const char* string);

/**
 * Function:   把json对象转成字符串打印到缓存
 * Description:
 * Input:      p_object：指向对象的指针；buffer：缓存；length：缓存长度
 * Output:     无
 * Return:     无
 * Others:     
 */
bool json_print_to_buffer(JsonObject* object, char* buffer, const int length);

#endif

