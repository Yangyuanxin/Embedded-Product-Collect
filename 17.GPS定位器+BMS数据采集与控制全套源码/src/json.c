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

#include <gm_memory.h>
#include <gm_stdlib.h>
#include "json.h"

static char* json_strdup(const char* string)
{
    U8 length = 0;
    char* copy = NULL;

    if (NULL == string)
    {
        return NULL;
    }

    length = GM_strlen(string) + 1;
    copy = (char*)GM_MemoryAlloc(length);
    if (NULL == copy)
    {
        return NULL;
    }
    GM_memcpy(copy, string, length - 1);
	copy[length - 1] = '\0';
    return copy;
}

static void suffix_object(JsonObject* prev, JsonObject* item)
{
    prev->next = (struct JsonObject*)item;
    item->prev = (struct JsonObject*)prev;
}


static bool add_item_to_array(JsonObject* array, JsonObject* item)
{
    JsonObject* child = NULL;

    if ((item == NULL) || (array == NULL))
    {
        return false;
    }

    child = (JsonObject*)array->child;

    if (child == NULL)
    {
        array->child = (struct JsonObject*)item;
    }
    else
    {
        while (child->next)
        {
            child = (JsonObject*)child->next;
        }
        suffix_object(child, item);
    }
    return true;
}


static bool add_item_to_object(JsonObject* object, const char* name, JsonObject* item)
{
    if ((object == NULL) || (name == NULL) || (item == NULL))
    {
        return false;
    }

    item->name = json_strdup(name);
    return add_item_to_array(object, item);
}

static JsonObject* json_create_int(double value)
{
    JsonObject* item = json_create();
    if(item)
    {
        item->type = JSON_INT;
        item->int_value = value;
    }

    return item;
}

static JsonObject* json_create_double(double value)
{
    JsonObject* item = json_create();
    if(item)
    {
        item->type = JSON_DOUBLE;
        item->double_value = value;
    }

    return item;
}

static JsonObject* json_create_string(const char *string)
{
    JsonObject* item = json_create();
    if(item)
    {
        item->type = JSON_STRING;
        item->str_value = json_strdup(string);
    }
    return item;
}

JsonObject* json_create(void)
{
	JsonObject* p_object = (JsonObject*)GM_MemoryAlloc(sizeof(JsonObject));
	if (p_object)
	{
		GM_memset(p_object, 0, sizeof(JsonObject));
		p_object->type = JSON_OBJECT;
	}
    return p_object;
}

void json_destroy(JsonObject* p_object)
{
    JsonObject* pChild;
	pChild = (JsonObject*)p_object->child;
    while(NULL != pChild)
    {	
        JsonObject* pNext;
		if (JSON_STRING == pChild->type)
		{
			GM_MemoryFree(pChild->str_value);
		}
		GM_MemoryFree(pChild->name);

        pNext = (JsonObject*)pChild->next;
        GM_MemoryFree(pChild);

		pChild = (JsonObject*)pNext;
    }
    GM_MemoryFree(p_object);
}

JsonObject* json_add_int(JsonObject*     p_object, const char* name, const int value)
{
	JsonObject* int_item = json_create_int(value);
	if (add_item_to_object(p_object, name, int_item))
	{
		return int_item;
	}
	return NULL;
}

JsonObject* json_add_double(JsonObject*     p_object, const char* name, const double value)
{
    JsonObject* double_item = json_create_double(value);
    if (add_item_to_object(p_object, name, double_item))
    {
        return double_item;
    }
    return NULL;
}

JsonObject* json_add_string(JsonObject*     p_object, const char* name, const char* string)
{
    JsonObject* string_item = json_create_string(string);
    if (add_item_to_object(p_object, name, string_item))
    {
        return string_item;
    }
    return NULL;
}

bool json_print_to_buffer(JsonObject* object, char* buffer, const int length)
{
	U16 index = 0;
	U8 item_num = 0;
	JsonObject* item = NULL;
	
	if(NULL == buffer || NULL == object)
	{
		return false;
	}
	buffer[index++] = '{';

	item = (JsonObject*)object->child;
    while(NULL != item)
    {	
		if (0 != item_num)
		{
			buffer[index++] = ',';
		}
    	index += sprintf(buffer+index, "\"%s\"", item->name);
		buffer[index++] = ':';
	    switch ((item->type) & 0xFF)
	    {
	    	case JSON_INT:
	            index += GM_sprintf(buffer+index, "%d", item->int_value);
				break;
	        case JSON_DOUBLE:
	            index += GM_sprintf(buffer+index, "%.2f", item->double_value);
				break;
	        case JSON_STRING:
	            index += GM_sprintf(buffer+index, "\"%s\"", item->str_value);
				break;
	        default:
	            return false;
	    }
		item = (JsonObject*)item->next;
		item_num++;
    }
	buffer[index++] = '}';
	buffer[index++] = '\0';
	return true;
}


