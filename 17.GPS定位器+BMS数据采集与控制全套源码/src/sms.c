/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        sms.c
 * Author:           王志华     
 * Version:          1.0
 * Date:             2019-04-08
 * Description:      
 * Others:      
 * Function List:    
    1. 创建sms模块
    2. 销毁sms模块
    3. sms模块定时处理入口
 * History: 
    1. Date:         2019-04-08
       Author:       王志华
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#include <gm_stdlib.h>
#include <gm_callback.h>
#include <gm_timer.h>
#include "sms.h"
#include "fifo.h"
#include "log_service.h"
#include "command.h"
#define GM_NUMBER_MAX_LEN 20
#define GM_SMS_QUEUE_LEN 3


typedef struct
{
	ENUM_SMSAL_DCS doc_type;
	char number[GM_NUMBER_MAX_LEN];
	char content[CMD_MAX_LEN];
}Message;

typedef struct
{
	FifoType message_queue;

	//发送失败次数
	U8 send_error_times;
}SMS;


static SMS s_sms;

static void send_sms_callback(void* p_msg);
static void send_sms_real(void);
static void on_recv_sms(void* p_msg);

GM_ERRCODE sms_create(void)
{
	GM_RegisterCallBack(GM_CB_SMS_RECEIVE, (U32)on_recv_sms);
	fifo_init(&s_sms.message_queue, GM_SMS_QUEUE_LEN * sizeof(Message));
	return GM_SUCCESS;
}

GM_ERRCODE sms_destroy(void)
{
	GM_ERRCODE ret;
	ret = fifo_delete(&s_sms.message_queue);
	return ret;
}

GM_ERRCODE sms_send(const char* p_data, u16 data_len, char* number, ENUM_SMSAL_DCS doc_type)
{
	GM_ERRCODE ret = GM_SUCCESS;
	Message msg;

    data_len = (data_len > sizeof(msg.content) - 2) ? sizeof(msg.content) - 2 : data_len;
	LOG(INFO,"Send SMS(%s),len:%d",number,data_len);
    GM_memcpy(msg.content, p_data, data_len);
    msg.content[data_len] = 0;
    msg.content[data_len + 1] = 0;
	msg.doc_type = doc_type;
	GM_strncpy(msg.number, number, GM_strlen((char*)number));
	
	ret = fifo_insert(&s_sms.message_queue, (U8*)&msg, sizeof(msg));
	
	if (GM_SUCCESS != ret)
	{
		return ret;
	}
	else
	{
		send_sms_real();
		return GM_SUCCESS;
	}
}

static void send_sms_callback(void* p_msg)
{
	if(NULL == p_msg)
	{
		return;
	}

	//发送成功
	if (*(U32*)p_msg)
	{
		s_sms.send_error_times = 0;
		
		//发送成功,从队列中移除
		LOG(INFO,"Succeed to send SMS.");
		fifo_pop_len(&s_sms.message_queue, sizeof(Message));
	}
	else
	{
		s_sms.send_error_times++;
		if (s_sms.send_error_times >= 3)
		{
			//丢弃一条短信
			LOG(WARN,"Faild to send SMS.err=%d",*(U32*)p_msg);
			fifo_pop_len(&s_sms.message_queue, sizeof(Message));
			s_sms.send_error_times = 0;
		}
	}

	//300毫秒后发送下一条短信
	GM_StartTimer(GM_TIMER_SMS_SEND, TIM_GEN_100MS*3, send_sms_real);
}

static void send_sms_real(void)
{
	GM_ERRCODE ret = GM_SUCCESS;
	
	Message msg;
	
	ret = fifo_peek(&s_sms.message_queue, (U8*)&msg, sizeof(msg));
	if (GM_SUCCESS != ret)
	{
		return;
	}
	GM_SendSMS(msg.number, msg.content, msg.doc_type, send_sms_callback);
	LOG(INFO,"Send SMS:%s",msg.content);
}

static void on_recv_sms(void* p_msg)
{
	char respons[CMD_MAX_LEN + 1] = {0};
	gm_sms_new_msg_struct* p_sms =(gm_sms_new_msg_struct*)p_msg;
	command_on_receive_data(COMMAND_SMS,p_sms->content,p_sms->messageLen,respons,p_msg);
    if(GM_strlen(respons))
    {
	    sms_send(respons, GM_strlen(respons),p_sms->asciiNum,GM_DEFAULT_DCS);
    }
}
 
