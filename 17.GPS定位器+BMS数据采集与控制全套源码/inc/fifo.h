/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        fifo.h
 * Author:           李耀轩       
 * Version:          1.0
 * Date:             2019-03-01
 * Description:      
 * Others:      
 * Function List:    
    1. 
    2. 
    3. 
 * History: 
    1. Date:         2019-03-01
       Author:       李耀轩
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#ifndef __FILO_H__
#define __FILO_H__

#include "gm_type.h"
#include "error_code.h"


#define MAX_FIFO_MSG_LEN 4096

typedef struct
{
    u32 read_idx;
    u32 write_idx;
	u32 size;   /*  total size  */
	u32 msg_count;
    u8* base_addr;
}FifoType;


GM_ERRCODE fifo_init(FifoType * fifo, u32 size);
GM_ERRCODE fifo_reset(FifoType * fifo);
GM_ERRCODE fifo_delete(FifoType * fifo);

u32 fifo_get_msg_length(FifoType * fifo);
u32 fifo_get_left_space(FifoType * fifo);

GM_ERRCODE fifo_insert(FifoType * fifo, u8 *data, u32 len);
GM_ERRCODE fifo_retrieve(FifoType * fifo, u8 *data, u32 *len_p);
GM_ERRCODE fifo_peek(FifoType * fifo, u8 *data, u32 len);
GM_ERRCODE fifo_pop_len(FifoType * fifo, u32 len);

//取出最大长度为*len_p的数据
GM_ERRCODE fifo_peek_and_get_len(FifoType * fifo, u8 *data, u32 *len_p);


//取出最大长度为*len_p的数据,遇到split_by就停止,返回的数据包含until_char
GM_ERRCODE fifo_peek_until(FifoType* fifo, u8* data, u16* len_p, const u8 until_char);
#endif



