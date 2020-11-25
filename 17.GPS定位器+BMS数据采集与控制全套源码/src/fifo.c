
#include <stdio.h>
#include "fifo.h"
#include "utility.h"
#include "gm_stdlib.h"
#include "gm_memory.h"
#include "log_service.h"

/************************************************************************
    Function : 
    Description : Init the fifo structure.
            fifo : reference to fifo struct.
            size : size of the fifo.
    Change history:
    Note:
    Author:     
    Date:
************************************************************************/
GM_ERRCODE fifo_init(FifoType* fifo, u32 size)
{
    if (fifo == NULL || size == 0)
    {
        return GM_PARAM_ERROR;
    }
    
	/* allocate fifo space. */
	fifo->base_addr = (u8 *) GM_MemoryAlloc(size * sizeof(u8));
	if (fifo->base_addr == NULL)
	{
		return GM_MEM_NOT_ENOUGH;
	}
	
	GM_memset((u8 *)fifo->base_addr, 0x00, sizeof(u8) * size);

	fifo->read_idx = 0;
	fifo->write_idx = 0;
	fifo->size = size;
	
	return GM_SUCCESS;
}



/************************************************************************
    Function : 
    Description : Reset the fifo structure.
            fifo : reference to fifo struct.
    Change history:
    Note:
    Author:     
    Date:
************************************************************************/
GM_ERRCODE fifo_reset(FifoType * fifo)
{
    if (fifo == NULL || fifo->base_addr == NULL)
    {
        return GM_PARAM_ERROR;
    }
	
	/* optional (clear memory region) */
	GM_memset((u8 *)fifo->base_addr, 0x00, fifo->size);
	
	fifo->read_idx = 0;
	fifo->write_idx = 0;
	return GM_SUCCESS;
}

/************************************************************************
    Function : 
    Description : delete the fifo structure.
            fifo : reference to fifo struct.
    Change history:
    Note:
    Author:     
    Date:
************************************************************************/
GM_ERRCODE fifo_delete(FifoType * fifo)
{
    if (fifo == NULL)
    {
        return GM_PARAM_ERROR;
    }
    
	/* delete fifo space. */
	if (fifo->base_addr == NULL)
	{
		return GM_SUCCESS;
	}
	
    GM_MemoryFree(fifo->base_addr);
    fifo->base_addr = 0;
	fifo->read_idx = 0;
	fifo->write_idx = 0;
	fifo->size = 0;
	return GM_SUCCESS;
}



/************************************************************************
    Function : 
    Description : returns msg count in the fifo
            fifo : reference to fifo struct.
    Change history:
    Note:
    Author:     
    Date:
************************************************************************/
u32 fifo_get_msg_length(FifoType * fifo)
{
    int read_point = 0;
    int write_point = 0;
    int left_data_size = 0;

    if (fifo == NULL)
    {
        return 0;
    }
	
    read_point = fifo->read_idx;
    write_point = fifo->write_idx;

    left_data_size = write_point - read_point;
    if(left_data_size < 0)
    {
        left_data_size += fifo->size;
    }

    return left_data_size;
}



/************************************************************************
    Function : 
    Description : returns left space of the fifo
            fifo : reference to fifo struct.
    Change history:
    Note:
    Author:     
    Date:
************************************************************************/
u32 fifo_get_left_space(FifoType * fifo)
{
    int read_point = 0;
    int write_point = 0;
    int left_data_size = 0;

    if (fifo == NULL)
    {
        return 0;
    }
	
    read_point = fifo->read_idx;
    write_point = fifo->write_idx;

    left_data_size = write_point - read_point;
    if(left_data_size < 0)
    {
        left_data_size += fifo->size;
    }

    return (fifo->size - left_data_size - 1);
}




/************************************************************************
    Function : 
    Description : Insert msg to the fifo.
            fifo : reference to fifo struct.
            data : reference to msg buffer.
            len : msg length.
    Change history:
    Note:
    Author:     
    Date:
************************************************************************/
GM_ERRCODE fifo_insert(FifoType * fifo, u8 *data, u32 len)
{
    int read_point;
    int write_point;
    int left_size;
    int to_end;

    if(len > MAX_FIFO_MSG_LEN)
    {
        return GM_PARAM_ERROR;
    }

    read_point = fifo->read_idx;
    write_point = fifo->write_idx;

    left_size = read_point - write_point;
    if(left_size <= 0)
    {
        left_size += fifo->size;
    }

    if((u32)left_size <= (len))
    {
        return GM_MEM_NOT_ENOUGH;
    }

    to_end = fifo->size - write_point;
    if((u32)to_end >= len)
    {
        GM_memcpy(fifo->base_addr + write_point,data,len);
        write_point += len;
        write_point %= fifo->size;
    }
    else
    {
        GM_memcpy(fifo->base_addr + write_point,data,to_end);
        GM_memcpy(fifo->base_addr,data + to_end,len -to_end);
        write_point = len -to_end;
    }

    fifo->write_idx = write_point;
    return GM_SUCCESS;
}


/************************************************************************
    Function : 
    Description : Retrieve one msg from the fifo.
            fifo : reference to fifo struct.
            data : reference to msg data buffer.
            len : data buffer length.
    Change history:
    Note:
    Author:     
************************************************************************/
GM_ERRCODE fifo_peek(FifoType * fifo, u8 *data, u32 len)
{
    int read_point = 0;
    int write_point = 0;
    int left_data_size = 0;
    int to_end = 0;

    read_point = fifo->read_idx;
    write_point = fifo->write_idx;

    left_data_size = write_point - read_point;
    if(left_data_size < 0)
    {
        left_data_size += fifo->size;
    }

    if((u32)left_data_size < len)
    {
        return GM_EMPTY_BUF;
    }

    to_end = fifo->size - read_point;
    if((u32)to_end >= len)
    {
        GM_memcpy(data,fifo->base_addr + read_point,len);
        read_point += len;
        if(read_point >=fifo->size)
        {
            read_point = 0;
        }
    }
    else
    {
        GM_memcpy(data,fifo->base_addr + read_point,to_end);
        GM_memcpy(data+to_end,fifo->base_addr,len -to_end);
        read_point = len - to_end;
    }

    return GM_SUCCESS;
}


/************************************************************************
    Function : 
    Description : Retrieve one msg from the fifo.
            fifo : reference to fifo struct.
            data : reference to msg data buffer.
            len : data buffer length.  it is input and output varible
    Change history:
    Note:
    Author:     
************************************************************************/
GM_ERRCODE fifo_peek_and_get_len(FifoType * fifo, u8 *data, u32 *len_p)
{
    int read_point = 0;
    int write_point = 0;
    int left_data_size = 0;
    int to_end = 0;

    read_point = fifo->read_idx;
    write_point = fifo->write_idx;

    left_data_size = write_point - read_point;
    if(left_data_size < 0)
    {
        left_data_size += fifo->size;
    }

    if((u32)left_data_size < (*len_p))
    {
        (*len_p) = (u32)left_data_size;
    }

    to_end = fifo->size - read_point;
    if((u32)to_end >= (*len_p))
    {
        GM_memcpy(data,fifo->base_addr + read_point,(*len_p));
        read_point += (*len_p);
        if(read_point >=fifo->size)
        {
            read_point = 0;
        }
    }
    else
    {
        GM_memcpy(data,fifo->base_addr + read_point,to_end);
        GM_memcpy(data+to_end,fifo->base_addr,(*len_p) -to_end);
        read_point = (*len_p) - to_end;
    }

    return GM_SUCCESS;
}



GM_ERRCODE fifo_pop_len(FifoType * fifo, u32 len)
{
    int read_point = 0;
    int write_point = 0;
    int left_data_size = 0;

    read_point = fifo->read_idx;
    write_point = fifo->write_idx;
	left_data_size = write_point - read_point;
	
    if(left_data_size < 0)
    {
        left_data_size += fifo->size;
    }

    if((u32)left_data_size < len)
    {
        return GM_PARAM_ERROR;
    }
    read_point += len;
    if(read_point >= fifo->size)
    {
        read_point -= fifo->size;
    }

    fifo->read_idx = read_point;

    return GM_SUCCESS;
}

GM_ERRCODE fifo_peek_until(FifoType* fifo, u8* data, u16* len_p, const u8 until_char)
{
	U32 read_point = fifo->read_idx;
    U32 write_point = fifo->write_idx;
	S32 data_size = write_point - read_point;
	U32 index = 0;
	bool has_until_char = false;

	if (NULL == fifo || NULL == data || NULL == len_p)
	{
		return GM_PARAM_ERROR;
	}
	
    if(data_size < 0)
    {
        data_size += fifo->size;
    }

	//取分隔符前面的所有字符
	for (index = 0; index < data_size && (fifo->base_addr[read_point] != until_char) && index < *len_p - 1; index++)
	{
		data[index] = fifo->base_addr[read_point];
		read_point++;
		read_point %= fifo->size;
	}

	//取分隔符后面的所有分隔符,包括'\r'
	for (; index < data_size && index < *len_p - 1 && ((fifo->base_addr[read_point] == until_char) || (fifo->base_addr[read_point] == '\r')); index++)
	{
		data[index] = fifo->base_addr[read_point];
		read_point++;
		read_point %= fifo->size;
		has_until_char = true;
	}

	*len_p = index;
	data[index] = '\0';
	if (has_until_char)
	{
    	return GM_SUCCESS;
	}
	else
	{
		return GM_EMPTY_BUF;
	}	    
}


